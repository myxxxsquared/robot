#include "SoftwareServos.h"
#include <EEPROM.h>

int highestPin = 20;

unsigned char g_message[1024];
unsigned char g_config[256];

unsigned int g_messageTop;
unsigned int g_command;
unsigned int g_length;
unsigned int g_crc;
unsigned int g_count;
unsigned int g_channel;
unsigned int g_value;
long g_calc;
unsigned char g_autoRun = 0;

unsigned long g_heartBeat[20];

#define ARDUINO_GET_ID 0
#define ARDUINO_RESET 1
#define ARDUINO_SET_OBJECT 2
#define ARDUINO_SET_SERVO 3
#define ARDUINO_HEARTBEAT 4
#define ARDUINO_RELEASE_SERVO 5
#define ARDUINO_GET_IR 6
#define ARDUINO_GET_SONAR 7

#define ARDUINO_LOAD_CONFIG 32
#define ARDUINO_SAVE_CONFIG 33
#define ARDUINO_SAVE_SEQUENCE 34

#define MAX_CONFIG (85*2)
#define SEQ_START MAX_CONFIG 
#define SEQ_END (1024 - MAX_CONFIG)

void initialize()
{
  int i;
  for (i=0;i<highestPin;i++) g_heartBeat[i]=(long)0;
  
  for (i=0;i<MAX_CONFIG;i++)
    g_config[i]=EEPROM.read(i);
}

void setup()
{
  Serial.begin(57600);

  ss_Init();
}

int readPacket()
{
  // get header byte
  // 128 (bit 8) flag indicates a new g_command packet ..
  //		that means the g_value bytes can never have 128 set!
  // next byte is the g_command 0-8
  // next byte is single byte data length
  // for command >= 32 next byte is high byte data length
  // data
  // crc

  int high;
  int crc;
  g_messageTop=2;

  do
  {
    while (Serial.available() <= 0) continue;
    g_command = Serial.read();
  }
  while ((g_command&128)==0);

  g_message[0] = crc = g_command;
  g_command^=128;

  while (Serial.available() <= 0) continue;
  g_length = g_message[1] = Serial.read();
  crc ^= g_length;

  if (g_command>=32)
  {
    while (Serial.available() <= 0) continue;
    g_message[2] = high = Serial.read();
    g_length = g_length|(high<<7);
    crc ^= high;
    g_messageTop=3;
  }

  // read in entire message
  if (g_length>0)
  {
    int count = g_length;
    while (count>0)
    {
      while (Serial.available() <= 0) continue;
      g_message[g_messageTop] = Serial.read();
      crc ^= g_message[g_messageTop++];
      count--;
    }

    while (Serial.available() <= 0) continue;
    g_message[g_messageTop++] = g_crc = Serial.read();

    if ((crc&127)!=(g_crc&127)) return 0;
  }
  return 1;
}

void echoPacket()
{
  Serial.write(g_message, g_messageTop);
}

// sets a part in the face identified by an id instead of a pin. This 
// allows the Arduino to chose the appropriate pin to activate and
// scale the value according to configuration parameters. This is
// useful for external applications that don't want to download
// the configuration (min, max, etc) and provide scaled values.
// Also used for saved sequences in case pins are reassigned or
// the configuration changes. This allows the saved sequence to
// continue to work as is without having to redownload from the
// Fritz application.
void setObjectPosition(int object, int val)
{
  int i = object*10;
  
  int trimVal = g_config[i] | (g_config[i+1]<<7);
  if (trimVal > 8192) trimVal -= 16384;
  int maxVal = g_config[i+2] | (g_config[i+3]<<7);
  if (maxVal > 8192) maxVal -= 16384;
  int minVal = g_config[i+4] | (g_config[i+5]<<7);
  if (minVal > 8192) minVal -= 16384;

  int pin = g_config[i+6] | (g_config[i+7]<<7);

  if (pin!=0)
  {
    g_calc = (((long)val+1000)* ((long)(maxVal - minVal)))/(long)2000;
    
    val = g_calc + trimVal + minVal;
    if (val<-1000) val=-1000;
    if (val>1000) val=1000;
    if (val<minVal) val=minVal;
    if (val>maxVal) val=maxVal;
    ss_SetPosition(pin, val+1500);
  }
}
// loads a saved sequence from eeprom and executes those movements
void executeSavedSequence()
{
  // first number in eeprom is the sequence size
  long int time = millis();
  int startSeq = SEQ_START;
  int len = EEPROM.read(startSeq) | (EEPROM.read(startSeq+1)<<8);
  if ((len<0)||(len>SEQ_END)) return;
  
  startSeq+=2;
  int stopSeq = startSeq + len;
  while ((startSeq<stopSeq)&&(!Serial.available()))
  {
    int object = EEPROM.read(startSeq++);
    int pos = EEPROM.read(startSeq++);
    pos |= EEPROM.read(startSeq++)<<8;
/*
Serial.println(delayFor);      
Serial.println(object);      
Serial.println(pos);      
Serial.println("");      
*/  
    if (object==255)
    {
      time+=pos;
      int pauseMS = time - millis();
      if (pauseMS>0)
        delay(pauseMS);
    }
    else  
    {
      if (pos > 8192) pos -= 32768;
      setObjectPosition(object, pos);
    }
  }
}

void loop()
{
/*  
  pinMode(19, OUTPUT);
  pinMode(18, INPUT);  
  digitalWrite(19, LOW);
  delayMicroseconds(2);
  digitalWrite(19, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(19, LOW);
  Serial.println(pulseIn(18, HIGH));
  return;
*/
  
  //ss_SetPosition(4, 1500);
  //return;
/*
  int p, q;
  for (p=700, q=800;p<2300;p++, q++)
  {
    //ss_SetPosition(14, p);
    ss_SetPosition(19, q);
    delay(5);
  }

  for (;p>=700;p--,q--)
  {
    //ss_SetPosition(14, p);
    ss_SetPosition(19, q);
    delay(5);
  }
  return;
*/

  // check if we should run the saved sequence or wait for PC response
  if (g_autoRun==0)
  {
    while (millis()<5000)
    {
      if (Serial.available())
      {
        g_autoRun = 1;
        break;
      }  
    }
    
    if (g_autoRun==0)
    {
      while (!Serial.available())
      {
        initialize();
        executeSavedSequence();
      }
      
      g_autoRun=1;
    }
  }

  int crc;
  int val;
  int i,j;
  int trimVal, maxVal, minVal, pin;

  while (Serial.available()>0)
  {
    if (readPacket()==0) 
      return;

    switch (g_command)
    {
      // init
      case  ARDUINO_GET_ID:
        initialize();
        Serial.print("ARDU004");
      break;
      // servo
      case  ARDUINO_RELEASE_SERVO:
        g_channel = g_message[2];
        ss_SetPosition(g_channel, 0);
        echoPacket();
      break;
      case  ARDUINO_SET_SERVO:
        g_channel = g_message[2];
        g_value = g_message[3] | (g_message[4]<<7);

        ss_SetPosition(g_channel, g_value);

        g_heartBeat[g_channel]=millis();

        echoPacket();
      break;
      case ARDUINO_SET_OBJECT:
        // sets the value of a specific part of the face. This uses configuration
        // memory to determine the pin and appropriate scaled min/max values.
        g_channel = g_message[2];
        val = g_message[3] | (g_message[4]<<7);
        
        if (val > 8192) val -= 16384;
        setObjectPosition(g_channel, val);
        
        g_heartBeat[pin]=millis();
        echoPacket();
      break;
      case ARDUINO_HEARTBEAT:
      break;
      case ARDUINO_LOAD_CONFIG:
        g_count = g_message[3]*2;

        g_message[0]=g_command|128;
        g_message[1]=g_count+1;
        g_message[2]=0;

        crc=g_message[0]^g_message[1]^g_message[2];

	for (j=3,i=0;i<g_count;i+=2,j+=2)
	{
	  g_message[j] = g_config[i];
	  g_message[j+1] = g_config[i+1];
	  crc=crc^g_message[j]^g_message[j+1];
	}
	g_message[j]=crc&127;

	for (i=3;i<j;i+=10)
        {
           trimVal = g_message[i] | (g_message[i+1]<<7);
           if (trimVal > 8192) trimVal -= 16384;
           maxVal = g_message[i+2] | (g_message[i+3]<<7);
           if (maxVal > 8192) maxVal -= 16384;
           minVal = g_message[i+4] | (g_message[i+5]<<7);
           if (minVal > 8192) minVal -= 16384;
           pin = g_message[i+6] | (g_message[i+7]<<7);

          if (pin!=0)
          {
            val = ((maxVal-minVal)/2)+minVal+trimVal;
            if (val<-1000) val=-1000;
            if (val>1000) val=1000;
            if (val<minVal) val=minVal;
            if (val>maxVal) val=maxVal;
            ss_SetPosition(pin, val+1500);
           g_heartBeat[pin]=millis();
          }
        }

	Serial.write(g_message, j+1);

      break;
      case ARDUINO_SAVE_CONFIG:

	for (j=3,i=0;i<g_length;i+=2,j+=2)
	{
          EEPROM.write(i, g_message[j]);
          g_config[i] = g_message[j];
          EEPROM.write(i+1, g_message[j+1]);
          g_config[i+1] = g_message[j+1];
        }

        g_message[0]=g_command|128;
        g_message[1]=0;
        g_message[2]=0;
	Serial.write(g_message, 3);
      break;
      case ARDUINO_RESET:

	for (i=0;i<MAX_CONFIG;i+=10)
        {
           trimVal = g_config[i] | (g_config[i+1]<<7);
           if (trimVal > 8192) trimVal -= 16384;
           maxVal = g_config[i+2] | (g_config[i+3]<<7);
           if (maxVal > 8192) maxVal -= 16384;
           minVal = g_config[i+4] | (g_config[i+5]<<7);
           if (minVal > 8192) minVal -= 16384;
           pin = g_config[i+6] | (g_config[i+7]<<7);

          if (pin!=0)
          {
            val = ((maxVal-minVal)/2)+minVal+trimVal;
            if (val<-1000) val=-1000;
            if (val>1000) val=1000;
            if (val<minVal) val=minVal;
            if (val>maxVal) val=maxVal;
            ss_SetPosition(pin, val+1500);
            g_heartBeat[pin]=millis();
          }
        }

      break;
      case ARDUINO_GET_IR:
        g_channel = g_message[2];
        
        g_value = analogRead(g_channel);

        g_message[3] = g_value&127;
        g_message[4] = (g_value>>7)&127;

        echoPacket();

      break;
      case ARDUINO_GET_SONAR:
        pinMode(g_message[2], OUTPUT);
        pinMode(g_message[3], INPUT);  

        digitalWrite(g_message[2], LOW);
        delayMicroseconds(2);
        digitalWrite(g_message[2], HIGH);
        delayMicroseconds(10); // Added this line
        digitalWrite(g_message[2], LOW);

        g_value = pulseIn(g_message[3], HIGH, 12500);

        g_message[3] = g_value&127;
        g_message[4] = (g_value>>7)&127;

        echoPacket();
      break;
      case ARDUINO_SAVE_SEQUENCE:
        EEPROM.write(SEQ_START, g_length&255);
        EEPROM.write(SEQ_START+1, g_length>>8);
	for (j=3,i=SEQ_START+2;i<(SEQ_START+g_length+2);i++,j++)
          EEPROM.write(i, g_message[j]);

        g_message[0]=g_command|128;
        g_message[1]=0;
        g_message[2]=0;
	Serial.write(g_message, 3);
      break;
    }
  }

  long sec = millis();
  for (i=0;i<highestPin;i++)
  {
    if (g_heartBeat[i]!=0)
    {
      if ((sec - g_heartBeat[i])>2000)
      {
        ss_SetPosition(i, 0);
        g_heartBeat[i]=0;
      }
    }
  }
}


