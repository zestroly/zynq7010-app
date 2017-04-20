#ifndef _XIDRIVER_H_
#define _XIDRIVER_H_
#include <iostream>

#include "XiImageDevice.h"

namespace Xilinx{

class XiDriver{
    public:
        XiDriver();
        ~XiDriver();
        int BlockModule(FEventType* callback, TImageType* Image = NULL);
        int BlockModule(TBlockInfo& BlockInfo,TImageType* Image = NULL);

    private:
        XiImageDevice *ImageDevice;

};


}

#endif //_XIDRIVER_H_
