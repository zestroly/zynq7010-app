#ifndef __XIIMAGEDEVICE_H_
#define __XIIMAGEDEVICE_H_

#include "XiPictureDriver.h"
#include "XiList.h"


#define MIRROR_EN       (1<<1)
#define MIRROR_X        (1<<2)
#define MIRROR_Y        (1<<3)
#define MIRROR_SOURCE   (1<<4)


/*Block module return value */
#define RE_BLOCKMODULE_SUCCESS   0
#define RE_BLOCKMODULE_BUSY     -1

namespace Xilinx{

typedef struct {
    uint32_t mark;
    uint32_t r;
    uint32_t cb;
    uint32_t ce;
}Hrun;

typedef struct {
    uint32_t status;
    uint32_t count;
    Hrun *buff;
}TBlockInfo;


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
        void* GrabPicture();

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
        static bool testModule(void* p);
        static void testModuleExec(void *p);


        //BlockModule
        int BlockModule(uint8_t thresholdvalue, FEventType* callback, TImageType* Image = NULL);
        int BlockModule(uint8_t thresholdvalue, TBlockInfo& BlockInfo,TImageType* Image = NULL);
        bool isBlockModuleWorking;
        bool enableBlockModule(TImageType* Image, uint8_t thresholdvalue);
        void disableBlockModule();
        static bool BlockModuleCondition(void* p);
        static void BlockModuleExec(void *p);


        //镜像Mirror
        bool enableMirror(bool enable);
        bool enableFilpV(bool enable);//垂直镜像 Y轴
        bool enableFilpH(bool enable);//水平镜像 X轴
        bool setMirrorSourcePicture(uint8_t methods);
        bool PictureMirror(uint8_t* dest, uint8_t* Src, TImageInformation& info);
};


}

#endif //__XIIMAGEDEVICE_H_
