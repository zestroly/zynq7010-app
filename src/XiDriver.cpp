#include "XiDriver.h"

#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>

#include "XiLog.h"

namespace Xilinx{

XiDriver::XiDriver()
{
    LOG(INFO_LEVEL, "start ..构造");
    ImageDevice = new XiImageDevice;
    LOG(INFO_LEVEL, "end   ..构造");
}


XiDriver::~XiDriver()
{
    LOG(INFO_LEVEL, "start ..虚构");
    delete ImageDevice;
    LOG(INFO_LEVEL, "end   ..虚构");
}

int XiDriver::GetWidth()
{
    return ImageDevice->getWidth();
}

int XiDriver::GetHeight()
{
    return ImageDevice->getHeight();
}

void* XiDriver::GrabPicture()
{
    ImageDevice->GrabPicture();
    LOG(INFO_LEVEL, "抓图 ---->");
}

void XiDriver::softTrigger()
{
    ImageDevice->softTrigger();
    LOG(INFO_LEVEL, "软出发 ---->");
}


//设置输出地址
Herror XiDriver::SetInputObject(void* Pointer)
{

}

Herror XiDriver::SetOutputObject(void* Pointer)
{

}

//获取输出地址
Herror XiDriver::GetInputObject(void** Pointer)
{

}
Herror XiDriver::GetOutputObject(void** Pointer)
{

}

Herror XiDriver::Execute()
{

}
Herror XiDriver::ExecuteProcedure(const char* name)
{

}




}
