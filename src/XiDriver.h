#ifndef _XIDRIVER_H_
#define _XIDRIVER_H_
#include <iostream>

#include "XiImageDevice.h"

typedef int Herror;
typedef struct _TWorkModule{
    std::string name;

}TWorkModule;

namespace Xilinx{

class XiDriver{
    public:
        XiDriver();
        ~XiDriver();

        //抓图
        void* GrabPicture();
        int GetWidth();
        int GetHeight();

        //软件触发
        void softTrigger();


        //设置输出地址
        Herror SetInputObject(void* Pointer);
        Herror SetOutputObject(void* Pointer);

        //获取输出地址
        Herror GetInputObject(void** Pointer);
        Herror GetOutputObject(void** Pointer);

        Herror Execute();
        Herror ExecuteProcedure(const char* name);

    private:
        XiImageDevice *ImageDevice;

};


}

#endif //_XIDRIVER_H_
