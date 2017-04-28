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
        int getWidth();
        int getHeight();
        void registerImageCallback(FHandler* pfun);
        void softTrigger(); //软触发
        uint32_t getImage(unsigned char** buff);

    private:
        Tpicture* mPicture;
        std::thread *mthread;
        std::mutex mMtx;
        bool mthreadWork;
        bool mExitThread;
        FHandler *workfun;

        TFbuffInfo DDrDataBuff[4];

    public:
        void setRegisterValue(uint32_t address, uint32_t value);
        uint32_t getRegisterValue(uint32_t address);

        void unlockBuff(uint8_t& BuffNo);
        void lockBuff(uint8_t& BuffNo);
        uint8_t getReadBuffNo();
        int getPictureBuff(int Number, char** buff);
        void* getPictureBuff(int Number);
        void   clearPictureBuff(int Number);  //清理状态

        static void ImageThread(XiPictureDriver* PictureDriver);
        uint32_t getImageBuff(unsigned char** buff);

        XiPictureDriver();
        ~XiPictureDriver();

        char* AllocDataBuff(uint8_t index);
        void   FreeDataBuff(uint8_t index);
        void*     PhyaddrToVirtualaddr(uint32_t Phyaddr);
        uint32_t VirtualaddrToPhyaddr(void* Virtualaddr);

        friend class XiImageDevice;

};

}



#endif //XIPICTUREDRIVER
