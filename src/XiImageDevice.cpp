#include "XiImageDevice.h"
#include <sys/time.h>

#include "XiLog.h"

namespace Xilinx{

XiImageDevice::XiImageDevice()
{
    LOG(INFO_LEVEL, "start ..构造");
    mPictureDriver = new XiPictureDriver;

    mEventList = new XiList;

    isBlockModuleWorking = false;

    LOG(INFO_LEVEL, "end   ..构造");
}

XiImageDevice::~XiImageDevice()
{
    LOG(INFO_LEVEL, "start ..虚构");
    delete mEventList;
    delete mPictureDriver;
    mPictureDriver = NULL;
    LOG(INFO_LEVEL, "end   ..虚构");
}

int XiImageDevice::getWidth()
{
    return mPictureDriver->getWidth();
}

int XiImageDevice::getHeight()
{
    return mPictureDriver->getHeight();
}

void XiImageDevice::registerImageCallback(FHandler* pfun)
{
    mPictureDriver->registerImageCallback(pfun);
}

void XiImageDevice::softTrigger() //软触发
{
    mPictureDriver->softTrigger();
}

uint32_t XiImageDevice::getImage(unsigned char** buff)
{
    mPictureDriver->getImage(buff);
}

void* XiImageDevice::GrabPicture()
{
    //判断当前模式，连续，还是触发
    uint32_t status = getRegisterValue(0xE0);
    std::cout<<status<<std::endl;
    if(status == 1 || status == 0)//触发模式
    {

    }else if(status == 2 ){ //连续模式
        return NULL;
    }
}

void XiImageDevice::setRegisterValue(uint32_t address, uint32_t value)
{
    mPictureDriver->setRegisterValue(address, value);
}

uint32_t XiImageDevice::getRegisterValue(uint32_t address)
{
    return mPictureDriver->getRegisterValue(address);
}

bool XiImageDevice::BlockModuleCondition(void* p)
{
    TListNode* pNode = (TListNode*)p;
    XiImageDevice* pImageDevice = (XiImageDevice*)pNode->pXiImageDevice;

    if(!(pImageDevice->getRegisterValue(0x308)&0x1))
        return false;
    else
        return true;
}

void XiImageDevice::BlockModuleExec(void *p)
{
    TListNode* pNode = (TListNode*)p;
    XiImageDevice* pImageDevice = (XiImageDevice*)pNode->pXiImageDevice;

    TBlockInfo *pTBlockInfo = new TBlockInfo;

    //判断是否溢出
    if((pImageDevice->getRegisterValue(0x308)&0x2)==0x2)
    {
        pTBlockInfo->status = RE_BLOCKMODULE_BUSY;
    }else{
        pTBlockInfo->status = RE_BLOCKMODULE_SUCCESS;
    }

    //查看run总数
    pTBlockInfo->count = pImageDevice->getRegisterValue(0x30C);
    pTBlockInfo->buff  = (Hrun*)pImageDevice->mPictureDriver->AllocDataBuff(1);
    pNode->funcallback(pTBlockInfo);

    delete pTBlockInfo;

    //清理状态，为下次准备
    pImageDevice->setRegisterValue(0x300, 0x2);
    pImageDevice->setRegisterValue(0x300, 0x1);
}

int XiImageDevice::BlockModule(uint8_t thresholdvalue, TBlockInfo& BlockInfo,TImageType* Image)
{
    if(isBlockModuleWorking == true)
        return RE_BLOCKMODULE_BUSY;

    //使能模块
    enableBlockModule(Image, 160);

    while(!(getRegisterValue(0x308)&0x1))
    {
        usleep(1000);
    }

    //判断是否溢出
    if((getRegisterValue(0x308)&0x2)==0x2)
    {
        BlockInfo.status = RE_BLOCKMODULE_BUSY;
    }else{
        BlockInfo.status = RE_BLOCKMODULE_SUCCESS;
    }

    //查看run总数
    BlockInfo.count = getRegisterValue(0x30C);
    BlockInfo.buff  = (Hrun*)mPictureDriver->AllocDataBuff(1);

    isBlockModuleWorking = false;
    return 0;
}

int XiImageDevice::BlockModule(uint8_t thresholdvalue, FEventType* callback, TImageType* Image)
{
    if(isBlockModuleWorking == true)
        return RE_BLOCKMODULE_BUSY;

    //使能模块
    enableBlockModule(Image, thresholdvalue);

    //添加 检测队列中
    TListNode lnode;
    lnode.name="BlockModule";

    lnode.pCondition     = BlockModuleCondition; //判断条件
    lnode.pExecute       = BlockModuleExec;
    lnode.funcallback    = callback;
    lnode.Mode           = "continue";
    lnode.pXiImageDevice = this;
    gettimeofday(&lnode.jointime, NULL);
    mEventList->addEvent(lnode);
    return 0;
}

bool XiImageDevice::enableBlockModule(TImageType* Image, uint8_t thresholdvalue)
{
    //清理Blob状态
    setRegisterValue(0x300, 0x2);
    //设置阀值
    setRegisterValue(0x304, thresholdvalue);

    //设置图片源
    if(Image == NULL)
        setRegisterValue(0x31C, 0); //这是来源于sensor
    else{
        setRegisterValue(0x31C, 1);
        //获取0x1F000000 的虚拟地址 ,并且拷贝数据到对应的地址
        memcpy(mPictureDriver->AllocDataBuff(0) , Image->imagebuff, Image->imagelen);
        setRegisterValue(0x320, 0x1F000000);
        setRegisterValue(0x328, Image->width);
        setRegisterValue(0x32C, Image->height);
    }
    //设置接受地址
    setRegisterValue(0x324, 0x1F400000);

    //开始Blob处理
    setRegisterValue(0x300, 0x1);

    isBlockModuleWorking = true;
    return true;
}

void XiImageDevice::disableBlockModule()
{


}


bool XiImageDevice::testModule(void* p)
{
    TListNode* pNode = (TListNode*)p;
    XiImageDevice* pImageDevice = (XiImageDevice*)pNode->pXiImageDevice;
    return true;
}

void XiImageDevice::testModuleExec(void *p)
{
    TListNode* pNode = (TListNode*)p;
    std::cout<<pNode->name<<std::endl;
    std::cout<<pNode->Mode<<std::endl;
    pNode->funcallback(NULL);
}

bool XiImageDevice::PictureMirror(uint8_t* dest, uint8_t* Src, TImageInformation &info)
{
    printf("ok\n");
    printf("src data: %x, %x , %x\n", Src[0], Src[1000], Src[1280*1024-1]);

    struct timeval tv, tv1, tv2, tv3, tv4;
    gettimeofday(&tv, NULL);
    int lenght = 1280*1024;
    memcpy(mPictureDriver->mPicture->Fb3VirtualAddress, Src, lenght );

    int i;
    gettimeofday(&tv1, NULL);

    gettimeofday(&tv2, NULL);
    for(i = 0; i < 1000; i++)
    {
        setRegisterValue(0x400, 0x2);  //处理前的地址
        setRegisterValue(0x414, 0x1E000000+0x800000);  //处理前的地址
        setRegisterValue(0x410, 0x1); //设置处理图片的来源为 soc
        setRegisterValue(0x41C, 1280);  //设置宽度
        setRegisterValue(0x420, 1024);  //设置高度
        setRegisterValue(0x418, 0x1E000000+0xC00000); //处理后的地址
        setRegisterValue(0x400, 0x1); //开始处理

        while(!(getRegisterValue(0x40C) & 0x1) )
            usleep(500);
    }
    gettimeofday(&tv3, NULL);

    unsigned int usetime = (((tv3.tv_sec-tv2.tv_sec)*1000000 + tv3.tv_usec) - tv2.tv_usec);
    std::cout<<"User time:"<<usetime<<std::endl;



    memcpy(dest ,mPictureDriver->mPicture->Fb4VirtualAddress , lenght);
    gettimeofday(&tv4, NULL);

    std::cout<<"起始时间:"<<tv.tv_sec <<"."<<tv.tv_usec <<std::endl;
    std::cout<<"拷贝到源:"<<tv1.tv_sec<<"."<<tv1.tv_usec<<"---:"<<tv1.tv_usec- tv.tv_usec<<std::endl;
    std::cout<<"开始处理:"<<tv2.tv_sec<<"."<<tv2.tv_usec<<"---:"<<tv2.tv_usec-tv1.tv_usec<<std::endl;
    std::cout<<"处理结束:"<<tv3.tv_sec<<"."<<tv3.tv_usec<<"---:"<<tv3.tv_usec-tv2.tv_usec<<std::endl;
    std::cout<<"结束拷贝:"<<tv4.tv_sec<<"."<<tv4.tv_usec<<"---:"<<tv4.tv_usec-tv3.tv_usec<<std::endl;

    printf("Fb3 data: %x, %x , %x \n",
            mPictureDriver->mPicture->Fb3VirtualAddress[0],
            mPictureDriver->mPicture->Fb3VirtualAddress[1000],
            mPictureDriver->mPicture->Fb3VirtualAddress[1280*1024-1]);

    std::cout<<"run time:"<<"---:"<<tv4.tv_usec-tv.tv_usec<<std::endl;
    printf("dest data: %x, %x , %x\n", dest[0], dest[1000], dest[1280*1024-1]);
    printf("Fb4 data: %x, %x , %x\n",
            mPictureDriver->mPicture->Fb4VirtualAddress[0],
            mPictureDriver->mPicture->Fb4VirtualAddress[1000],
            mPictureDriver->mPicture->Fb4VirtualAddress[1280*1024-1]);

#if 0
    if(Flags & MIRROR_EN)
    {
        if(Flags & MIRROR_X)
            setRegisterValue(0x404, 0x01);
        else
            setRegisterValue(0x404, 0x00);

        if(Flags & MIRROR_Y)
            setRegisterValue(0x408, 0x01);
        else
            setRegisterValue(0x408, 0x00);

        bool loopBlockModule(std::string name, FEventType* Execute, void *pdata);
        if(Flags & MIRROR_SOURCE)
            setRegisterValue(0x410, 0x1);

        mPictureDriver->mPicture->PhysAddress;
    }else{
        return false;
    }
#endif
}


void XiImageDevice::addModule(std::string Name, FEventType* callback, void *pdata)
{
    TListNode lnode;
    if(Name == "test")
    {
        lnode.name="test";
        lnode.pCondition     = testModule; //判断条件
        lnode.pExecute       = testModuleExec;
        lnode.funcallback    = callback;
        lnode.Mode           = "continue";
        lnode.pXiImageDevice = this;
        lnode.pdata          = pdata;
        gettimeofday(&lnode.jointime, NULL);
        mEventList->addEvent(lnode);
    }
}


void XiImageDevice::setFbValue(int buffno, char value, int len)
{
    uint8_t *buff = new uint8_t[len];
    memset(buff, value, len);
    switch(buffno)
    {
        case 1:
            memcpy(mPictureDriver->mPicture->Fb1VirtualAddress ,buff,  len);
            break;
        case 2:
            memcpy(mPictureDriver->mPicture->Fb2VirtualAddress ,buff,  len);
            break;
        case 3:
            memcpy(mPictureDriver->mPicture->Fb3VirtualAddress ,buff,  len);
            break;
        case 4:
            memcpy(mPictureDriver->mPicture->Fb4VirtualAddress ,buff,  len);
            break;
    }
    delete[] buff;
}


}



