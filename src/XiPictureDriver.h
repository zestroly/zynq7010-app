#ifndef XIPICTUREDRIVER
#define XIPICTUREDRIVER

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>
#include <ctime>
#include <vector>

#include <string.h>
#include <thread>
#include <mutex>
#include <stdarg.h>
#include <stdint.h>


namespace Xilinx{

typedef struct _Tpicture{
    int Handler;

    unsigned int PhysLength;
    unsigned int PhysAddress;

    void* VirtualAddress;
    char* Fb1VirtualAddress;
    char* Fb2VirtualAddress;
    char* Fb3VirtualAddress;
    char* Fb4VirtualAddress;

    int AxiHandler;
    unsigned int AxiPhysLength;
    unsigned int AxiPhysAddress;
    volatile unsigned int* AxiInt;
    void* AxiVirtualAddress;
}Tpicture;

typedef struct _ImageType{
    uint32_t width;
    uint32_t height;
    uint32_t imagelen;
    unsigned char* imagebuff;
}TImageType;

typedef struct _TFbuffInfo{
    uint32_t Index;   //内存块索引号
    uint32_t Size;    //内存块大小
    char*    Pointer; //内存块指针
    bool     isValid; //是否可用
    std::string name;
}TFbuffInfo;

typedef void FHandler(TImageType* info);

class XiPictureDriver{
public:
    XiPictureDriver();
    ~XiPictureDriver();
    int getWidth();             //获取图片宽度
    int getHeight();            //获取图片长度
    void registerImageCallback(FHandler* pfun);  //注册回调函数
    void softTrigger();      //软触发


    //参数设置
    void setRegisterValue(uint32_t offset, uint32_t value);
    uint32_t getRegisterValue(uint32_t offset);

    //缓冲区图片
    void unlockBuff(uint8_t& BuffNo);    //解锁锁第BuffNo缓冲区
    void lockBuff(uint8_t& BuffNo);        //锁住第BuffNo缓冲区
    uint8_t getReadyBuffNo();                   //获取最新的BuffNo (0-3) 编号，查到图片才停止，或者m_ClassWorking=false
    int getPictureBuff(int BuffNo, char** buff);
    void* getPictureBuff(int BuffNo);   //获取BuffNo (0-3)缓冲区地址，
    uint32_t getImageBuff(unsigned char** buff);
    uint32_t getImage(unsigned char** buff);

    bool m_StopCatchPicture;                  //抓图标志
    void StopCatchPicture();                     //停止抓图
    void StartCatchPicture();                    //可以抓图
    bool hasRegisterImageCallBack();    //查询是否有回调函数
    static void ImageThread(XiPictureDriver* PictureDriver);  //线程，用于回调函数

    //物理地址转化成虚拟地址
    void*     PhyaddrToVirtualaddr(uint32_t Phyaddr);
    //虚拟地址转化成物理地址
    uint32_t VirtualaddrToPhyaddr(void* Virtualaddr);

private:
    bool m_ClassWorking;
    uint32_t m_GetPictureMode;   //0 --- 未设置 ，1 --- 回调 （外触发） ，2 --- 主动抓图（软触发+连续模式）
    Tpicture* mPicture;
    std::thread *mthread;
    std::mutex m_ImageCallBackMutex;

    //回调函数指针
    FHandler *m_CallBackHandler;
    TFbuffInfo DDrDataBuff[4];

public:


    char* AllocDataBuff(uint8_t index);
    void   FreeDataBuff(uint8_t index);
    friend class XiImageDevice;
};

}



#endif //XIPICTUREDRIVER
