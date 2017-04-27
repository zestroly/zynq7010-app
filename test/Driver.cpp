#include "XiDriver.h"

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


int main(int argc, char** argv)
{


    XiDriver* driver = new XiDriver;
    driver->SetSensorParam ("Width", 1000);
    getchar();
    //driver->softTrigger();
  //  while(1)
  //    {
   //      driver->GrabPicture();
  //       usleep(1000);
   //  }
    char* inputbuff = new char[1024*1280];
    char* outputbuff = new char[1024*1280];
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
    driver->GetOutputObject("HrunOutPutBuff", (void**)&Pointer);
    printf("HrunCount:%d\n", Count);
    printf("%d , %d , %d,  %d \n", Pointer->mark, Pointer->r, Pointer->cb, Pointer->ce);

    std::cout<<"---------------------------------------------------------"<<std::endl;


    char* buffImage;
    driver->ExecuteProcedure ("MirrorModule");
    driver->SetInputParam ("Width", 1280);
    driver->SetInputParam ("Height", 1024);
    driver->SetInputObject ("InputImage", inputbuff);
    driver->Execute();
    driver->GetOutputObject("OutputImage", (void**)&buffImage);




    delete driver;
    return 0;
}
