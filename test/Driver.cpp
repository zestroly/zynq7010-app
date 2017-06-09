#include "XiDriver.h"
#include "XiLog.h"
using namespace Xilinx;


void blockCallback(void* p)
{
    TBlockInfo* pBlockInfo = (TBlockInfo*)p;

    printf("%d\n", pBlockInfo->status);
    std::cout<<"状态:"<<pBlockInfo->status<<" Hrun count:"<<pBlockInfo->count<<" unit size:"<<sizeof(Hrun) <<std::endl;

    int i;
    printf("         mark      r       cb       ce\n");
    for(i = 0 ; i < 5; i++)
    {
        printf("%03u:%8u %8u %8u %8u\n", i, pBlockInfo->buff[i].mark ,pBlockInfo->buff[i].r,pBlockInfo->buff[i].cb,pBlockInfo->buff[i].ce);
    }

    uint32_t count = pBlockInfo->count;

    std::cout<<pBlockInfo->buff[count-1].r<<std::endl;
    std::cout<<pBlockInfo->buff[count-1].cb<<std::endl;
    std::cout<<pBlockInfo->buff[count-1].ce<<std::endl;
}



int framerate;
unsigned int sec_temp;


void ImageCallback(Xilinx::TImageType *info)
{

    struct timeval tv;
    gettimeofday(&tv, NULL);

    if(sec_temp == tv.tv_sec)
        framerate++;
    else{
        sec_temp = tv.tv_sec;
        std::cout<<"=================================framer rate:"<<framerate<<std::endl;
        framerate = 0;
    }

    std::cout<<tv.tv_sec<<"."<<tv.tv_usec<<":"<<info->width <<"x"<<info->height<<std::endl;
    char* buff = new char[info->imagelen];
    unsigned short* destbuff = (unsigned short*)buff;
    unsigned short* srcbuff  = (unsigned short*)info->imagebuff;

    memcpy(buff, info->imagebuff, info->imagelen);
    delete[] buff;

}

void testPicture()
{
    std::cout<<"-------------------------"<<std::endl;
    uint8_t *Imagebuff = new uint8_t[1024*1280];
    XiDriver* driver = new XiDriver;

    driver->registerImageCallback (ImageCallback);
    getchar();

    uint32_t ImageCount = driver->GrabPicture (Imagebuff, 1024*1280);
    if(ImageCount > 0)
        LOG(INFO_LEVEL, "Image:%d x %d." , driver->GetSensorParam ("Width") , driver->GetSensorParam ("Height") );
     getchar();


     ImageCount = driver->GrabPicture (Imagebuff, 1024*1280);
         if(ImageCount > 0)
             LOG(INFO_LEVEL, "Image:%d x %d." , driver->GetSensorParam ("Width") , driver->GetSensorParam ("Height") );
      getchar();


    while(1)
    {
        ImageCount = driver->GrabPicture (Imagebuff, 1024*1280);
        if(ImageCount > 0)
            LOG(INFO_LEVEL, "Image:%d x %d." , driver->GetSensorParam ("Width") , driver->GetSensorParam ("Height") );
    //   getchar();
    }
    delete driver;

}


int main(int argc, char** argv)
{
    testPicture();
    return 0;
    char* inputbuff = new char[1024*1280];
    char* outputbuff = new char[1024*1280];
    XiDriver* driver = new XiDriver;

    inputbuff[0] = 1;
    inputbuff[1] = 2;
    inputbuff[2] = 3;
    inputbuff[3] = 4;

    uint8_t threshvalue = 150;
    Hrun *Pointer;
    int Count;
    driver->ExecuteProcedure("BlockModule");
    driver->SetInputParam("ThreshValue", 180);
    driver->Execute();

    driver->GetOutputParam("HrunCount", (void*)&Count);
        printf("____\n");
    driver->GetOutputObject("HrunOutPutBuff", (void**)&Pointer);
    printf("HrunCount:%d\n", Count);
    printf("%d , %d , %d,  %d \n", Pointer->mark, Pointer->r, Pointer->cb, Pointer->ce);

    std::cout<< "......  " <<driver->GetErrorMsg () <<std::endl;

    std::cout<<"---------------------------------------------------------"<<std::endl;


    char* buffImage;
    driver->ExecuteProcedure ("MirrorModule");
    driver->SetInputParam ("Width", 1280);
    driver->SetInputParam ("Height", 1024);
    driver->SetInputObject ("InputImage", inputbuff);
    driver->Execute();
    std::cout<< "......  " <<driver->GetErrorMsg () <<std::endl;
    driver->GetOutputObject("OutputImage", (void**)&buffImage);

    driver->ExecuteProcedure ("testModule");
     std::cout<< "......  " <<driver->GetErrorMsg () <<std::endl;



    delete driver;
    return 0;
}
