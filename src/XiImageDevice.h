#ifndef __XIIMAGEDEVICE_H_
#define __XIIMAGEDEVICE_H_

#include "XiPictureDriver.h"
#include "XiList.h"


#define MIRROR_EN       (1<<1)
#define MIRROR_X        (1<<2)
#define MIRROR_Y        (1<<3)
#define MIRROR_SOURCE   (1<<4)

namespace Xilinx{
class XiImageDevice{
    public:
        typedef struct _ImageInformation{
            uint8_t Width;
            uint8_t Heght;
            bool    MirrorX;
            bool    MirrorY;
            bool    SourceSoc;
        }TImageInformation;

        int getWidth();
        int getHeight();
        void registerImageCallback(FHandler* pfun);
        void softTrigger(); //软触发
        uint32_t getImage(unsigned char** buff);

        void setFbValue(int buffno, char value, int len);
    private:
        XiPictureDriver *mPictureDriver;
        XiList *mEventList;
    public:
        XiImageDevice();
        ~XiImageDevice();

        void setRegisterValue(uint32_t address, uint32_t value);
        uint32_t getRegisterValue(uint32_t address);
        //设置回调函数
        void addModule(std::string Name, FEventType* callback, void *pdata);

        static bool testModule(void* p)
        {
            TListNode* pNode = (TListNode*)p;
            XiImageDevice* pdata = (XiImageDevice*)pNode->pXiImageDevice;
            //printf("111:%d\n", pdata->getRegisterValue(0x80));
            return true;
        }

        static void testModuleExec(void *p)
        {
            TListNode* pNode = (TListNode*)p;
            std::cout<<pNode->name<<std::endl;
            std::cout<<pNode->Mode<<std::endl;
            pNode->funcallback(NULL);

        }

        //BlockModule
        bool enableBlockModule(bool enable=true);
        int BlockModule();
        bool loopBlockModule();

        //镜像Mirror
        bool enableMirror(bool enable);
        bool enableFilpV(bool enable);//垂直镜像 Y轴
        bool enableFilpH(bool enable);//水平镜像 X轴
        bool setMirrorSourcePicture(uint8_t methods);
        bool PictureMirror(uint8_t* dest, uint8_t* Src, TImageInformation& info);
};


}

#endif //__XIIMAGEDEVICE_H_