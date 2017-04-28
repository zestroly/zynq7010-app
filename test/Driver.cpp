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

void testPicture()
{
    uint8_t *Imagebuff = new uint8_t[1024*1280];
    XiDriver* driver = new XiDriver;
    while(1)
    {
        uint32_t ImageCount = driver->GrabPicture (Imagebuff, 1024*1280);
        LOG(INFO_LEVEL, "Image:%d x %d." , driver->GetSensorParam ("Width") , driver->GetSensorParam ("Height") );
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

    //driver->softTrigger();
  //  while(1)
  //    {
   //      driver->GrabPicture();
  //       usleep(1000);
   //  }

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
