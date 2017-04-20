#ifndef _XILOG_H_
#define _XILOG_H_

#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/Priority.hh>

#include <stdarg.h>
#include <syslog.h>

// confirm to log4cpp::priority::PriorityLevel
#define DEBUG_LEVEL  700
#define INFO_LEVEL   600
#define WARN_LEVEL   400
#define ERROR_LEVEL  300
#define FATAL_LEVEL  0

#define LOG(level,format, args...) {do {\
    XiLog::getInstance().write(level, __FUNCTION__,__LINE__, format, ##args);\
}while(0);}

namespace Xilinx{

class XiLog
{
    public:
        static XiLog& getInstance() { return instance_; }
        void write(int level, const char* func,int line, const char* format, ...);

    private:
        XiLog();
        virtual ~XiLog();

        void init();
        void cleanup();
        void create();

        log4cpp::Priority::Value currentPriority();
        void formatFunc(const char* func, int line, const char* format, va_list alist, std::string& msg);

    private:
        std::string confile;
        static XiLog instance_;
        log4cpp::Category* logger_;
};

}

#endif //_XILOG_H_
