
#include "chatrobot.hpp"
#include "json.hpp"
using json = nlohmann::json;

#include <sstream>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include "dialog.hpp"

static const char* API_KEY = "aaa31673c154432e92ae0d56c0bdd320";
static const char* URL = "http://www.tuling123.com/openapi/api";

Dialog::Dialog(SAFE_DEQUE<std::string> *qi, SAFE_DEQUE<std::string> *qo)
    : queue_instr(qi), queue_out(qo)
{
}

void Dialog::dialog()
{
    std::string str;
    std::string poststr;

    str = queue_instr->pop();

    {
        json post;
        post["key"] = API_KEY;
        post["userid"] = USERID;
        post["info"] = str;
        poststr = post.dump();
    }

    {
        curlpp::Cleanup cleanup;
        std::ostringstream os;

        curlpp::Easy request;
        request.setOpt(new curlpp::options::Url(URL));
        std::list<std::string> header;
        header.push_back("Content-Type: application/json");
        request.setOpt(new curlpp::options::HttpHeader(header));
        request.setOpt(new curlpp::options::WriteStream(&os));
        request.setOpt(new curlpp::options::PostFields(poststr.c_str()));
        request.setOpt(new curlpp::options::PostFieldSize(poststr.size() + 1));
        request.perform();
        poststr = os.str();
    }

    {
        auto result = json::parse(poststr);
        poststr = result["text"];
    }

    queue_out->emplace(std::move(poststr));
}

void Dialog::do_dialog()
{
    srand((unsigned int)time(NULL));
    for(int i = 0; i < UID_LENGTH; ++i)
    {
        int c = rand() % 62;
        if(c <= 10)
            USERID[i] = '0' + c;
        else if(c <= 36)
            USERID[i] = 'a' - 10 + c;
        else
            USERID[i] = 'A' - 36 + c;
    }
    USERID[UID_LENGTH] = 0;

    while(true)
        dialog();
}

