#include "XiLog.h"

#include <syslog.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/timeb.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>
#include <log4cpp/Configurator.hh>
#include <iostream>
#include <fstream>

using namespace std;
namespace Xilinx{

#define BUF_LEN 4096


XiLog XiLog::instance_;

XiLog::XiLog()
{
    syslog(LOG_INFO, "Log constructor");
    create();
    init();
}

XiLog::~XiLog()
{
    syslog(LOG_INFO, "Log deconstructor");
    cleanup();
}

void XiLog::create()
{
    std::string filedata="log4cpp.rootCategory=DEBUG, rootAppender\n";
    filedata +="\n";
    filedata += "log4cpp.appender.rootAppender=ConsoleAppender\n";
    filedata += "log4cpp.appender.rootAppender.layout=PatternLayout\n";
    filedata += "log4cpp.appender.rootAppender.layout.ConversionPattern=%d [%p] %m%n \n";

    ofstream ofile;
    confile = "log4cpp.properties";
    ofile.open(confile);
    ofile<<filedata;
    ofile.close();
}

void XiLog::init()
{
    logger_ = NULL;

    try {
        log4cpp::PropertyConfigurator::configure(confile);
        log4cpp::Category& root = log4cpp::Category::getRoot();
        logger_ = &root;
    } catch(std::exception const& e) {
        syslog(LOG_NOTICE, "init log error[%s]", e.what());
    } catch(...) {
        syslog(LOG_INFO, "init log error[%d, %s]", errno, strerror(errno));
    }

    syslog(LOG_INFO, "log init finished");
}

void XiLog::cleanup()
{
    log4cpp::Category::shutdown();
}

int XiLog::currentPriority()
{
    if (logger_) {
        return logger_->getPriority();
    }
    return INFO_LEVEL;

}

void XiLog::write(int level, const char* func, int line, const char* format, ...)
{
    if (level > currentPriority()) {
        return;
    }
    if (NULL == logger_) {
        return;
    }

    std::string info;
    va_list alist;
    va_start (alist, format);
    formatFunc(func, line, format, alist, info);
    va_end(alist);

    try {
        switch(level) {
            case FATAL_LEVEL:
                logger_->fatal(info);
                break;
            case ERROR_LEVEL:
                logger_->error(info);
                break;
            case WARN_LEVEL:
                logger_->warn(info);
                break;
            case INFO_LEVEL:
                logger_->info(info);
                break;
            case DEBUG_LEVEL:
                logger_->debug(info);
                break;
            default:
                break;
        }
    } catch(...) {
        syslog(LOG_INFO, "output log failed");
    }
}

void XiLog::formatFunc(const char* func, int line, const char* format, va_list alist, std::string& msg)
{
    char buf[BUF_LEN] = {0};
    int prefixLen = snprintf(buf, BUF_LEN-1, "%#08X %s(),[%d]:", pthread_self(), func, line);
    int contentLen = vsnprintf(buf+prefixLen, BUF_LEN-1-prefixLen, format, alist);
    int dataLen = prefixLen + contentLen;
    buf[dataLen] = '\0';
    msg = buf;
}

}
