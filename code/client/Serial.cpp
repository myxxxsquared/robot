//
// Created by Du Shikang on 17/01/2018.
//

#include "Serial.h"
#include <iostream>
#include <cmath>

void convStringVector(const std::string & s, std::vector<byte> & v)
{
    v.resize(s.length());
    for(int i=0; i<s.length(); ++i){
        v[i] = (byte)s[i];
    }
}

Serial::Serial()
{

    // TODO: set wait time
    foundBoard = false;
    arduino_is_available = true;
    arduino_port_name = "/dev/ttyUSB0";  // ttyACM0 for official Arduino Uno board, USB0 for CH340G chip

    timeout = serial::Timeout(serial::Timeout::max(), readTimeOut, 0, writeTimeOut, 0);
    arduino = new serial::Serial();
    arduino->setTimeout(timeout);  // TODO

//    Open();
}

Serial::~Serial()
{
    Close();
}



bool Serial::IsConnected()
{
    return foundBoard;
}

int Serial::TestSerialOld()
{
    int version = -1;

    if(!arduino_is_available){
        return -1;
    }

    std::vector<byte> sendData;
    std::vector<byte> requestData;

    sendData.resize(2);
    sendData[0] = (byte)128;
    sendData[1] = (byte)0;

    if(arduino->isOpen())
    {
        arduino->write(sendData);
        std::string requestData_str;
        arduino->waitReadable();  // must wait for data
        arduino->read(requestData_str,7);
        convStringVector(requestData_str, requestData);
        version = GetVersion(requestData);
        std::cout<<"Get Version: "<<version<<std::endl;
    }
    return version;
}

int Serial::Open()
{
    // open and configure the serial port
    arduino->close();

    arduino->setPort(arduino_port_name);
    arduino->setBaudrate(BaudRate);
    arduino->setBytesize(serial::eightbits);
    arduino->setParity(serial::parity_none);
    arduino->setStopbits(serial::stopbits_one);
    arduino->setFlowcontrol(serial::flowcontrol_none);
    arduino->open();
    if( !arduino->isOpen() ){
//        std::cerr<<"Unable to open serial port";
        throw "Open serial port fail";
    }
    else
        arduino_is_available = true;

    sonarValue = 9999.0;
    version = -1;

//    connect(arduino, &QSerialPort::readyRead, this, &Serial::handleReadyRead);
    //connect(arduino, &QSerialPort::serialPortError, this, &Serial::handleError);
    //connect(arduino, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),this, &Serial::handleError);
    //connect(&m_timer, &QTimer::timeout, this, &SerialPortReader::handleTimeout);

//    TestSerial();
    // NOTE: must wait for sometime to get device ready !!!
    arduino->waitReadable();
    return 0;
}

void Serial::Close()
{
    if(arduino->isOpen())
        arduino->close();
    arduino_is_available = false;
    foundBoard = false;
}

double Serial::GetSonar()
{
    return sonarValue;
}

int Serial::GetVersion()
{
    return version;
}

int Serial::TestSerial()
{
    int version = -1;

    if(!arduino_is_available){
        return -1;
    }

    std::vector<byte> sendData;
    std::vector<byte> requestData;
    std::string requestData_str;

    sendData.resize(2);
    sendData[0] = (byte)128;
    sendData[1] = (byte)0;

    if(arduino->isOpen())
    {
        arduino->write(sendData);
        arduino->read(requestData_str,7);
        convStringVector(requestData_str, requestData);
        version = GetVersion(requestData);
    }
    return version;
}

bool Serial::SendPacket(std::vector<byte> buffer, int slen, int rlen)
{  // just get rlen=2, do not wake the sleeping dog. Not the same as the C# version.
    unsigned int command;
    int length;

    std::vector<byte> sendData;
    sendData.resize(9);
    int idx = 0;
    int crc;
    //int high;
    // Before CRC
    // 131 - Command
    //   2 - pin?
    // 102 - lsb position
    //  11
    //   0
    //   0
    //   0
    // 106 - msb position

    crc = command = buffer[0];    // Command
    length = slen - 2;
    if( length < 0 )
        length = 0;

    sendData[idx++] = (byte)command;
    sendData[idx++] = (byte) (length & 127);
    crc ^= (byte) (length & 127);

    if((command & 127) >= 32)
    {
        sendData[idx++] = (byte) (length >> 7);
        crc ^= (byte) (length >> 7);
    }

    for(int i = 1; i < slen; i++)
    {
        sendData[idx++] = buffer[i];
        crc ^= buffer[i];
    }

    sendData[9] = (byte)crc;

    // After CRC
    // 131 - Command
    //   6 - CRC
    //   2 - pin
    // 102 - lsb position
    //  11
    //   0
    //   0
    //   0
    // 106

    if(arduino->isOpen())
    {
        arduino->write(sendData);
//        arduino->waitForBytesWritten(waitTimeOut);
        //qWarning() << "Send: " << QString(sendData.toHex()) << endl;
        return true;
    }
    std::cerr << "Failed to write" << std::endl;
    return false;
}

void Serial::handleReadyRead()
{
//    I::msleep(10);
//    std::vector<byte> requestData = arduino->readAll();
    std::string requestDataStr = arduino->readline();
//    std::vector<byte> requestData = arduino->readline();
    while(arduino->waitReadable())
    {
//        I::msleep(2);
        //todo TIME SLEEP
        requestDataStr += arduino->readline();
//        requestData = requestData + arduino->readAll();
    }

    std::vector<byte> requestData;
    convStringVector(requestDataStr, requestData);

    //qWarning() << "Rec:  " << QString(requestData.toHex()) << endl;

    while(true)
    {
        if((requestData[0] & 127) == 'A')
        { // if fetch VERSION data
            version = GetVersion(requestData);
            std::clog << "Version: " << version << std::endl;
            requestData.erase(requestData.begin(), requestData.begin()+7);
        }
        else if((requestData[0] & 127) == ARDUINO_GET_SONAR)
        {  // if fetch sonar data
            int x = ((int)requestData[3] | (requestData[4] << 7));
            // convert distance to cm
            double dist = (float)((float)x / 29.10f);
            if( dist > 0 )
                sonarValue = dist;

            //qWarning() << "Rec:  " << QString(requestData.toHex()) << endl;
            //qWarning() << "Sonar: " << sonarValue << endl;
//            requestData.remove(0,9);
            requestData.erase(requestData.begin(), requestData.begin()+9);
        }
        else if((requestData[0] & 127) == ARDUINO_SET_SERVO)
        {
//            requestData.remove(0,9);/
            requestData.erase(requestData.begin(), requestData.begin()+9);
        }
        else
//            requestData.remove(0,9);
            requestData.erase(requestData.begin(), requestData.begin()+9);

        if(requestData.size()<9)
            break;
    }
}

void Serial::handleError()
{
    arduino->flush();
}

int Serial::GetVersion(std::vector<byte> buf)
{
    if(buf.size()<7) throw "Version getting fail\n";
    char b1 = buf[0];
    char b2 = buf[1];
    char b3 = buf[2];
    char b4 = buf[3];
    char v1 = buf[4];
    char v2 = buf[5];
    char v3 = buf[6];

    int version = ((v1-'0')*100)+((v2-'0')*10)+(v3-'0');

    if ((b1 == 'A') && (b2 == 'R') && (b3 == 'D') && (b4 == 'U'))
    {
        foundBoard = (version >= 4);
//        if (!foundBoard)info = "Fritz found but firmware version is too old!\n";
        // Found "+verion+(std::string)" but this application requires 3 and above."
    }
    else
    {
        foundBoard = false;
        version = -2;
    }
    return version;
}

bool Serial::SendPacketOld(std::vector<byte> buffer, int slen, int rlen)
{
    unsigned int command;
    int length;

    std::vector<byte> sendData;
    int idx = 0;
    int crc;
    //int high;
    // Before CRC
    // 131 - Command
    //   2 - pin?
    // 102 - lsb position
    //  11
    //   0
    //   0
    //   0
    // 106 - msb position

    crc = command = buffer[0];    // Command
    length = slen - 2;
    if( length < 0 )
        length = 0;

    sendData[idx++] = (byte) command;
    sendData[idx++] = (byte) (length & 127);
    crc ^= (byte) (length & 127);

    if((command & 127) >= 32)
    {
        sendData[idx++] = (byte) (length >> 7);
        crc ^= (byte) (length >> 7);
    }

    for(int i = 1; i < slen; i++)
    {
        sendData[idx++] = buffer[i];
        crc ^= buffer[i];
    }

    sendData[9] = (byte)crc;

    // After CRC
    // 131 - Command
    //   6 - CRC
    //   2 - pin
    // 102 - lsb position
    //  11
    //   0
    //   0
    //   0
    // 106
    byte sendBuffer_[9];
    for (int j = 0; j < 9; ++j) {
        sendBuffer_[j] = sendData[j];
    }

//    std::vector<byte> requestData;
    std::string requestDataStr;
    if(arduino->isOpen())
    {
        arduino->write(sendBuffer_,9);
//        if (!arduino->waitForBytesWritten(waitTimeOut))
//            return false;

        if (arduino->waitReadable())
        {
            // read request
            std::string tmp_str;
            tmp_str = arduino->readline();
            requestDataStr += tmp_str;
            //requestData += arduino->readAll();

            if((requestDataStr[0] & 127) == ARDUINO_GET_SONAR)
            {
                int x = ((int)requestDataStr[3] | (requestDataStr[4] << 7));
                // convert distance to cm
                sonarValue = (float)((float)x / 29.10f);
            }
        }
        return true;
    }
    return false;
}

//void Serial::ReadOld(std::vector<byte> requestData)
//{
//    if (arduino->waitForReadyRead(waitTimeOut))
//    {
//        // read request
//        requestData = arduino->readAll();
//        while (arduino->waitForReadyRead(100))
//            requestData += arduino->readAll();
//
//        if(((unsigned char)requestData[0] & 127) == ARDUINO_GET_SONAR)
//        {
//            int x = ((int)requestData[3] | (requestData[4] << 7));
//            // convert distance to cm
//            sonarValue = (float)((float)x / 29.10f);
//        }
//    }
//}


void Serial::Read_(std::vector<byte> buffer, int len)
{
    arduino->waitReadable();
    arduino->read(buffer, len);
}

void Serial::DoTest(CheckState state, int min, int max, int pin, int val)
{
    //range min - max maps to 0 - 100 so val maps to min + val * ( range / 100 )
    double range = max - min;
    auto pos = (int)nearbyint(min + (val * range/100));

    if(state == Checked)
    {
        SetServo(pin, pos);
    }
}

void Serial::SetSonar(int sonarOutPin, int sonarInPin)
{
    SendCommand(ARDUINO_GET_SONAR, sonarOutPin, (short) sonarInPin);
}

void Serial::SetServo(int pin, int value)
{
    SendCommand(ARDUINO_SET_SERVO, pin, (short)value + 1500);
}

void Serial::SendCommand(int cmd)
{
    std::vector<byte> buffer;
    buffer.resize(1);

    buffer[0] = (byte)(128 | cmd);

    SendPacket(buffer, 1, 1);
}

void Serial::SendCommand(int cmd, int pin)
{
    std::vector<byte> buffer;
    buffer.resize(3);

    buffer[0] = (byte)(128 | cmd);
    buffer[1] = (byte)(pin & 127);
    buffer[2] = (byte)((buffer[0] ^ buffer[1] ^ 1) & 127);

    SendPacket(buffer, 3, 2);
}

void Serial::SendCommand(int cmd, int pin, int value)
{
    std::vector<byte> buffer;
    buffer.resize(8);

    buffer[0] = (byte)(128 | cmd);
    buffer[1] = (byte)(pin & 127);
    buffer[2] = (byte)(value & 127);
    buffer[3] = (byte)((value >> 7) & 127);
    buffer[4] = (byte)((value >> 14) & 127);
    buffer[5] = (byte)((value >> 21) & 127);
    buffer[6] = (byte)((value >> 28) & 127);
    buffer[7] = (byte)((buffer[0] ^ buffer[1] ^ buffer[2] ^ buffer[3] ^ buffer[4] ^ buffer[5] ^ buffer[6] ^ 6) & 127);

    SendPacket(buffer, 8, 2);
}

void Serial::SendCommand(int cmd, int pin, short value)
{
    std::vector<byte> buffer;
    buffer.resize(5);

    buffer[0] = (byte)(128 | cmd);
    buffer[1] = (byte)(pin & 127);
    buffer[2] = (byte)(value & 127);
    buffer[3] = (byte)((value >> 7) & 127);
    buffer[4] = (byte)((buffer[0] ^ buffer[1] ^ buffer[2] ^ buffer[3] ^ 3) & 127);

    SendPacket(buffer, 5, 2);
}

void Serial::SendCommand(int cmd, int pin, byte value)
{
    std::vector<byte> buffer;
    buffer.resize(4);

    buffer[0] = (byte)(128 | cmd);
    buffer[1] = (byte)(pin & 127);
    buffer[2] = (byte)(value & 127);
    buffer[3] = (byte)((buffer[0] ^ buffer[1] ^ buffer[2] ^ 2) & 127);

    SendPacket(buffer, 4, 2);
}

void Serial::SendCommand(int cmd, std::vector<int> dat)
{
    std::vector<byte> buffer;
    buffer.resize(4096);

    buffer[0] = (byte)(128 | cmd);

    int i, j;
    for (j = 1, i = 0; (i < dat.size()); i++)
    {
        buffer[j++] = (byte)(dat[i] & 127);
        buffer[j++] = (byte)((dat[i] >> 7) & 127);
    }

    int crc = buffer[0];
    crc ^= (j - 1) & 127;
    crc ^= (j - 1) >> 7;
    for (i = 1; i < j; i++) crc ^= buffer[i];

    buffer[j++] = (byte)(crc & 127);

    SendPacket(buffer, j, 1);
}

void Serial::SendCommand(int cmd, std::vector<byte> dat)
{
    std::vector<byte> buffer;

    buffer.resize(SEND_QUEUE_MAX);
    buffer[0] = (byte)(128 | cmd);

    int i, j;
    for (j = 1, i = 0; (i < dat.size()); i++)
        buffer[j++] = (byte)dat[i];

    int crc = buffer[0];
    crc ^= (j - 1) & 127;
    crc ^= (j - 1) >> 7;
    for (i = 1; i < j; i++) crc ^= buffer[i];

    buffer[j++] = (byte)(crc & 127);

    SendPacket(buffer, j, 1);
}
