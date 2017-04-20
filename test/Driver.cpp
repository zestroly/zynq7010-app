#include "XiDriver.h"

using namespace Xilinx;


void blockCallback(void* p)
{
    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;

}


int main(int argc, char** argv)
{
    XiDriver* driver = new XiDriver;

    driver->BlockModule(blockCallback);

    getchar();
    delete driver;
    return 0;
}
