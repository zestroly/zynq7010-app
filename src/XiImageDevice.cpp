#include "XiImageDevice.h"
#include <sys/time.h>
namespace Xilinx{

XiImageDevice::XiImageDevice()
{
    std::cout<<"Create class XiImageDevice."<<std::endl;
    mPictureDriver = new XiPictureDriver;

    mEventList = new XiList;
}

XiImageDevice::~XiImageDevice()
{
    delete mEventList;

    delete mPictureDriver;
    mPictureDriver = NULL;
    std::cout<<"Destroy class XiImageDevice."<<std::endl;
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

void XiImageDevice::setRegisterValue(uint32_t address, uint32_t value)
{
    mPictureDriver->setRegisterValue(address, value);
}

uint32_t XiImageDevice::getRegisterValue(uint32_t address)
{
    return mPictureDriver->getRegisterValue(address);
}

int XiImageDevice::BlockModule()
{
    unsigned int status;
    unsigned short signValue;
    unsigned short signRow;
    unsigned short signColBegin;
    unsigned short signColEnd;

    unsigned int RawCount;
    unsigned int RawDataL;
    unsigned int RawDataH;

    unsigned int modulestatus = getRegisterValue(0x300);

    printf("0x328:%#x\n", getRegisterValue(0x328));
    printf("0x32C:%#x\n\n\n", getRegisterValue(0x32C));
    //清理Blob状态
    setRegisterValue(0x300, 0x2);

    //设置阀值
    setRegisterValue(0x304, 120);

    //设置图片源
    if(0)
        setRegisterValue(0x31C, 0);
    else{
        setRegisterValue(0x31C, 1);
        setRegisterValue(0x320, 0x1E000000);
        setRegisterValue(0x328, 1280);
        setRegisterValue(0x32C, 1024);
    }

    //设置接受地址
    setRegisterValue(0x324, 0x1F000000);

    //开始Blob处理
    setRegisterValue(0x300, 0x1);

    printf("0x300:%#x\n", getRegisterValue(0x300));
    printf("0x304:%d\n", getRegisterValue(0x304));
    printf("0x308:%#x\n", getRegisterValue(0x308));
    printf("0x31C:%#x\n", getRegisterValue(0x31C));
    printf("0x320:%#x\n", getRegisterValue(0x320));
    printf("0x324:%#x\n", getRegisterValue(0x324));
    printf("0x328:%#x\n", getRegisterValue(0x328));
    printf("0x32C:%#x\n", getRegisterValue(0x32C));


    //判断是否完成
    while(!(getRegisterValue(0x308)&0x1))
    {
        usleep(500*1000);
        printf("0x308:%#x\n", getRegisterValue(0x308));
    }

    //判断是否溢出
    std::cout<<((getRegisterValue(0x308)&0x2)==0x2?"溢出":"未溢出")<<std::endl;

    //查看run总数
    std::cout<<"run count:"<<(getRegisterValue(0x30C))<<std::endl;

    return 0;

    std::cout<<"0x300:"<<modulestatus<<std::endl;

    if( (modulestatus&(1<<0)) == 1 )
    {
        if( (getRegisterValue(0x308)&(1<<1)) == 1 )
            return -2;

        std::cout<<"0x308:"<<getRegisterValue(0x308)<<std::endl;
        RawCount = getRegisterValue(0x30C);
        std::cout<<"0x30C:"<<RawCount<<std::endl;
        if( (getRegisterValue(0x308)&(1<<0)) == 1 )
        {
            for(int i =0 ; i < RawCount ; i++)
            {
                signColEnd    = getRegisterValue(0x310) & 0xFFFF;
                signColBegin  = getRegisterValue(0x310) & 0xFFFF;
                signRow       = getRegisterValue(0x310) & 0xFFFF;
                signValue     = getRegisterValue(0x310) & 0xFFFF;
                std::cout<<signValue <<":"<<signRow<<":"<<signColBegin << ":"<<signColEnd<<std::endl;
            }
        }

        //读取完毕后，清理状态;
        status = getRegisterValue(0x300);
        setRegisterValue(0x300, status | (1<<1));
        status = getRegisterValue(0x300);
        std::cout<<"aften:"<<status<<std::endl;
    }else
        return -1;  //no block module or no enable
    return 0;

}


bool XiImageDevice::enableBlockModule(bool enable)
{
    unsigned int status= getRegisterValue(0x300);
    if(enable)
        setRegisterValue(0x300, (status|(1<<0)));
    else
        setRegisterValue(0x300, (status&(~(0x1))));

    if( (getRegisterValue(0x300)&(1)) == 1 )
        return true;
    else
        return false;
}

bool XiImageDevice::loopBlockModule()
{
    setRegisterValue(0x304, 180);
    while(1)
    {
        printf("************************************\n");
        printf("0x300:%#x\n", getRegisterValue(0x300));
        printf("0x304:%#x\n", getRegisterValue(0x304));
        printf("0x308:%#x\n", getRegisterValue(0x308));
        printf("0x30C:%#x\n", getRegisterValue(0x30C));

        unsigned int status = getRegisterValue(0x300);
        status |= (1<<1);
        printf("will write 0x300:%#x\n", status);
        setRegisterValue(0x300, status);

        printf("-------------------------------------\n");
        getchar();
    }
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



