#ifndef _XIDRIVER_H_
#define _XIDRIVER_H_
#include <iostream>

#include "XiImageDevice.h"

namespace Xilinx{

class XiDriver{
    public:
        XiDriver();
        ~XiDriver();

    private:
        XiImageDevice *ImageDevice;

};


}

#endif //_XIDRIVER_H_
