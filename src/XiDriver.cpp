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


int XiDriver::BlockModule(FEventType* callback, TImageType* Image)
{

}

int XiDriver::BlockModule(TBlockInfo& BlockInfo,TImageType* Image)
{

}






}
