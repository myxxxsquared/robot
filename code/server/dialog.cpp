
#include "chatrobot.hpp"
#include "json.hpp"
using json = nlohmann::json;

#include <sstream>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

constexpr int UID_LENGTH = 32;
static const char* API_KEY = "aaa31673c154432e92ae0d56c0bdd320";
static const char* URL = "http://www.tuling123.com/openapi/api";
static char USERID[UID_LENGTH + 1];

static void dialog()
{
    std::string str;
    std::string poststr;

    {
        string_queue_use use{q_in};
        str = *use;
    }

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

    q_out.emplace(poststr);
}

void* do_dialog(void*)
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

    while(!signal_exit)
        dialog();
    return NULL;
}
