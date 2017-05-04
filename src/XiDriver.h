#ifndef _XIDRIVER_H_
#define _XIDRIVER_H_
#include <iostream>

#include <map>

#include "XiImageDevice.h"



namespace Xilinx{

typedef enum Herror
{
    MODULE_SUCCESS = 0,
    MODULE_NOFIND_MODULE,   //没有模块
    MODULE_NAME_IS_NULL,   //模块名为空

    MODULE_NOFIND_PARAM, //没有此参数
    MODULE_NOFIND_OBJECT, //没有此object

    MODULE_BLOCK_OVERFLOW,  //block 溢出
    MODULE_BLOCK_TERMINATE, //block 外部中断检测
    MOUDLE_BLOCK_NOSET_THRESHVALUE,
}Herror;


class XiDriver{
public:
    XiDriver();
    ~XiDriver();

    //抓图
    void* GrabPicture();  //触发模式用,获取起始地址
    uint32_t GrabPicture (uint8_t* buff,    uint32_t bufflen);

    //相机和FPGA参数设置
    uint32_t GetSensorParam(const char* name);
    void SetSensorParam(const char* name, uint32_t value);

    //软件触发
    void softTrigger();

    /********************************************************/
    //模块运行
    Herror ExecuteProcedure(const char* name);
    //参数设置
    Herror SetInputParam(const char* name, long   value);
    Herror SetInputObject(const char* name, void* pdata);
    //执行模块
    Herror Execute();
    //获取结果
    Herror GetOutputParam(const char* name, void*    pdata);
    Herror GetOutputObject(const char* name, void**  pdata);
    //获取错误信息
    const char* GetErrorMsg();
    /********************************************************/

private:
    Herror ExecuteBlock();
    Herror ExecuteMirror();
    void Terminate();
    void AllocErrLog();
    void DestroyErrLog();

    XiImageDevice *ImageDevice;
    std::string ModuleName;
    Herror ErrNumber;
    std::mutex ModueMutex;

    std::map<Herror, char*> ErrLog;

    std::map<std::string, long> OutPutData;
    std::map<std::string, long> IntPutData;

};


}

#endif //_XIDRIVER_H_
