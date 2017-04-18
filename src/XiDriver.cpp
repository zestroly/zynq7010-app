#include "XiDriver.h"

namespace Xilinx{

XiDriver::XiDriver()
{
    std::cout<<"123\n";
    ImageDevice = new XiImageDevice;

    std::cout<<"323\n";
}


XiDriver::~XiDriver()
{

    delete ImageDevice;
}





}
