
#include "chatrobot.hpp"
#include "qisr.h"
#include "msp_cmn.h"
#include "msp_errors.h"

#include <cstddef>

void init_xf()
{
    int errcode = MSP_SUCCESS;
    const char* login_params = "appid = 5a2e1454, work_dir = .";
    errcode = MSPLogin(NULL, NULL, login_params);
    if (MSP_SUCCESS != errcode)
        throw "MSPLogin() failed.";
}
