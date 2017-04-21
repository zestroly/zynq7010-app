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

    driver->softTrigger();
    driver->GrabPicture();




    getchar();
    delete driver;
    return 0;
}
