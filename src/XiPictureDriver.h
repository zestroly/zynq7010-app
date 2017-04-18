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

    public:
        void setRegisterValue(uint32_t address, uint32_t value);
        uint32_t getRegisterValue(uint32_t address);

        void unlockBuff(uint8_t& BuffNo);
        void lockBuff(uint8_t& BuffNo);
        uint8_t getReadBuffNo();
        int getPictureBuff(int Number, char** buff);
        static void ImageThread(XiPictureDriver* PictureDriver);
        uint32_t getImageBuff(unsigned char** buff);
        XiPictureDriver();
        ~XiPictureDriver();

        int BlockModule();
        bool enableBlockModule(bool enable);
        friend class XiImageDevice;

};

}



#endif //XIPICTUREDRIVER