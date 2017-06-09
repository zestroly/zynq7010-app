#include "XiPictureDriver.h"

#include "XiLog.h"

namespace Xilinx{

XiPictureDriver::XiPictureDriver()
{
    LOG(INFO_LEVEL, "start ..构造");
    m_GetPictureMode = 0;
    mPicture = new Tpicture;
    memset(mPicture, 0, sizeof(Tpicture));

    mPicture->PhysLength = 0x400000*4*2;
    mPicture->PhysAddress = 0x1E000000;

    mPicture->Handler = open("/dev/Xipicture", O_RDWR|O_SYNC);
    if(mPicture->Handler < 0)
    {
        LOG(ERROR_LEVEL, "open Xipicture dev fail");
    }
    else{
        mPicture->VirtualAddress = mmap(NULL, mPicture->PhysLength,
            PROT_READ|PROT_WRITE,
            MAP_SHARED,
            mPicture->Handler , 0);
        if(mPicture->VirtualAddress != NULL)
        {
           mPicture->Fb1VirtualAddress = (char*)mPicture->VirtualAddress + 0x000000;
           mPicture->Fb2VirtualAddress = (char*)mPicture->VirtualAddress + 0x400000;
           mPicture->Fb3VirtualAddress = (char*)mPicture->VirtualAddress + 0x800000;
           mPicture->Fb4VirtualAddress = (char*)mPicture->VirtualAddress + 0xC00000;

           //分配0x1F00,0000 ~ 0x2000,0000内存块
           int loop;
           for(loop = 0; loop < 4; loop++)
           {
               DDrDataBuff[loop].Index    = loop+1;
               DDrDataBuff[loop].Size     = 0x400000;
               DDrDataBuff[loop].Pointer  = (char*)mPicture->VirtualAddress+0x1000000+0x400000*loop;
               DDrDataBuff[loop].isValid  = true;
           }

        }else{
            LOG(ERROR_LEVEL, "Xipicture mmap!");
        }
    }

    mPicture->AxiPhysLength = 0x20000;
    mPicture->AxiPhysAddress = 0x40600000;
    mPicture->AxiHandler = open("/dev/XiAxi", O_RDWR|O_SYNC);
    if(mPicture->AxiHandler < 0)
    {
        LOG(ERROR_LEVEL, "open XiAxi dev fail");
    }
    else{
        mPicture->AxiVirtualAddress = mmap(NULL, mPicture->AxiPhysLength,
                PROT_READ|PROT_WRITE,
                MAP_SHARED,
                mPicture->AxiHandler , 0);
        if(mPicture->AxiVirtualAddress != NULL)
        {
            mPicture->AxiInt = (volatile unsigned int*)mPicture->AxiVirtualAddress;
        }else{
            LOG(ERROR_LEVEL, "Axi mmap!");
        }
    }

    //创建线程
    m_CallBackHandler = NULL;
    m_ClassWorking = true;
    mthread = new std::thread(ImageThread, this);

  //  this->setRegisterValue(0xE0,1);
    //使能抓图模式
    this->setRegisterValue(0xDC,1);
    //设置成软出发
    this->setRegisterValue(0xFC,0);
    LOG(INFO_LEVEL, "end   ..构造");
}

XiPictureDriver::~XiPictureDriver()
{
    LOG(INFO_LEVEL, "start ..虚构");
    m_ClassWorking = false;
    mthread->join();
    delete mthread;
    mthread = NULL;

    if(mPicture != NULL)
    {
        if(mPicture->AxiVirtualAddress != NULL)
        {
            munmap(mPicture->AxiVirtualAddress, mPicture->AxiPhysLength);
            mPicture->AxiVirtualAddress = NULL;
        }

        if(mPicture->AxiHandler >= 0)
            close(mPicture->AxiHandler);


        if(mPicture->VirtualAddress != NULL)
        {
            munmap(mPicture->VirtualAddress, mPicture->PhysLength);
            mPicture->VirtualAddress = NULL;
        }

        if(mPicture->Handler >= 0)
            close(mPicture->Handler);

        delete mPicture;
        mPicture = NULL;
    }
    m_GetPictureMode = 0;


    LOG(INFO_LEVEL, "end   ..虚构");
}

 void* XiPictureDriver::getPictureBuff(int BuffNo)
 {
     if(mPicture->VirtualAddress != NULL)
     {
         switch(BuffNo)
         {
             case 0:
                 return (void*)mPicture->Fb1VirtualAddress;
                 break;
             case 1:
                 return (void*)mPicture->Fb2VirtualAddress;
                 break;
             case 2:
                 return (void*)mPicture->Fb3VirtualAddress;
                 break;
             case 3:
                 return (void*)mPicture->Fb4VirtualAddress;
                 break;
             default:
                 return NULL;
                 break;
         }
     }else{
         return NULL;
     }
 }

int XiPictureDriver::getPictureBuff(int BuffNo, char** buff)
{//根据Number ，获取buff地址
    if(mPicture->VirtualAddress != NULL)
    {
        switch(BuffNo)
        {
            case 0:
                *buff = mPicture->Fb1VirtualAddress;
                break;
            case 1:
                *buff = mPicture->Fb2VirtualAddress;
                break;
            case 2:
                *buff = mPicture->Fb3VirtualAddress;
                break;
            case 3:
                *buff = mPicture->Fb4VirtualAddress;
                break;
            default:
                *buff = NULL;
                return -1;
        }
        return 0;
    }else{
        return -2;
    }
}


void XiPictureDriver::softTrigger()
{
    if(mPicture->AxiHandler < 0)
        return;
    *(mPicture->AxiInt + 0xE4/4) = 1;
}

void XiPictureDriver::enableCapture(bool status) //允许获取图片
{
    if(mPicture->AxiHandler < 0)
        return;
    if(status)
        *(mPicture->AxiInt + 0xDC/4) = 1;
    else
        *(mPicture->AxiInt + 0xDC/4) = 0;
}


int XiPictureDriver::getWidth()
{
    if(mPicture->AxiHandler < 0)
        return -1;
    return *(mPicture->AxiInt + 0x80/4);
}


int XiPictureDriver::getHeight()
{
    if(mPicture->AxiHandler < 0)
        return -1;
    return *(mPicture->AxiInt + 0x90/4);
}

uint8_t XiPictureDriver::getReadyBuffNo()
{
    uint32_t rel = *((volatile unsigned int*) (mPicture->AxiInt + 0xD4/4));
    uint8_t  buffNo = ((rel>>4) & 0x3);
    uint8_t  buffstatus = (rel & 0xF);  //判断是否有准备好的图片
    //printf("#### %x  %x\n", buffNo, buffstatus);
    while( m_ClassWorking && !buffstatus )
    {
        usleep( 500 );
        rel = *((volatile unsigned int*) (mPicture->AxiInt + 0xD4/4));
        buffNo = ((rel>>4) & 0x3);
        buffstatus = (rel & 0xF);
    }
   // printf("2*****  %x  %x\n", buffNo, buffstatus);
    return buffNo;
}
 uint8_t XiPictureDriver::HardTrigerGetReadyBuffNo()
 {
     uint32_t rel = *((volatile unsigned int*) (mPicture->AxiInt + 0xD4/4));
     uint8_t  buffNo = ((rel>>4) & 0x3);
     uint8_t  buffstatus = (rel & 0xF);  //判断是否有准备好的图片
     while( (getRegisterValue (0xFC) == 1) && m_ClassWorking && !buffstatus )
     {
         usleep( 500 );
         rel = *((volatile unsigned int*) (mPicture->AxiInt + 0xD4/4));
         buffNo = ((rel>>4) & 0x3);
         buffstatus = (rel & 0xF);
     }
     return buffNo;
 }

void XiPictureDriver::lockBuff(uint8_t& BuffNo)
{
    uint32_t status = *(mPicture->AxiInt+0xD8/4);
    *(mPicture->AxiInt+0xD8/4) = status|(0x10<<BuffNo);
}

void XiPictureDriver::unlockBuff(uint8_t& BuffNo)
{
    uint32_t status = *(mPicture->AxiInt+0xD8/4);
    *(mPicture->AxiInt+0xD8/4) = status|(0x1<<BuffNo);
}

uint32_t XiPictureDriver::getImageBuff(unsigned char** buff)
{
    uint8_t buffNo = HardTrigerGetReadyBuffNo();
    lockBuff(buffNo);
    getPictureBuff(buffNo, (char**)buff);  //通过buffNo 编号来获取buff地址
    unlockBuff(buffNo);
    return 0;
}

uint32_t XiPictureDriver::getImage(unsigned char** buff)
{
    getImageBuff(buff);
    return (getWidth()*getHeight());
}


void XiPictureDriver::registerImageCallback(FHandler* pfun)
{
    this->m_ImageCallBackMutex.lock();
    this->m_CallBackHandler = pfun;
    this->m_ImageCallBackMutex.unlock();
}

bool XiPictureDriver::hasRegisterImageCallBack()   //查询是否有回调函数
{
    bool rel;
    this->m_ImageCallBackMutex.lock();
    rel = ((this->m_CallBackHandler == NULL)? false:true );
    this->m_ImageCallBackMutex.unlock();
    return rel;
}

void XiPictureDriver::ImageThread(XiPictureDriver* PictureDriver)
{
    while(PictureDriver->m_ClassWorking)
    {
        PictureDriver->m_ImageCallBackMutex.lock();
        if( (PictureDriver->getRegisterValue (0xFC) == 1)  &&  (PictureDriver->m_CallBackHandler !=NULL)  )
        {
            TImageType* info = new TImageType;
            PictureDriver->getImageBuff( &(info->imagebuff) );
            info->width = PictureDriver->getWidth();
            info->height = PictureDriver->getHeight();
            info->imagelen = info->width * info->height;
            if((PictureDriver->m_CallBackHandler  != NULL)  &&  PictureDriver->m_ClassWorking  &&(PictureDriver->getRegisterValue (0xFC) == 1)  )
                PictureDriver->m_CallBackHandler(info);
            delete info;

        }
        PictureDriver->m_ImageCallBackMutex.unlock();
        usleep(1000);
    }
}



void XiPictureDriver::setRegisterValue(uint32_t offset, uint32_t value)
{
    *(mPicture->AxiInt+offset/4) = value;
}

uint32_t XiPictureDriver::getRegisterValue(uint32_t offset)
{
    return *(mPicture->AxiInt+offset/4);
}


char* XiPictureDriver::AllocDataBuff(uint8_t index)
{
    if(index >=4)
        return NULL;

    DDrDataBuff[index].isValid = false;
    return DDrDataBuff[index].Pointer;
}

void XiPictureDriver::FreeDataBuff(uint8_t index)
{
    if(index >=4)
        return;
    DDrDataBuff[index].isValid = true;

}

void*   XiPictureDriver::PhyaddrToVirtualaddr(uint32_t Phyaddr)
{
   return (void*)( ((char*)mPicture->VirtualAddress) + (Phyaddr - mPicture->PhysAddress));
}

uint32_t XiPictureDriver::VirtualaddrToPhyaddr(void* Virtualaddr)
{
    return (mPicture->PhysAddress + ((char*)Virtualaddr - (char*)mPicture->VirtualAddress));
}


}

