#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "XiImageDevice.h"

#include <iostream>
#include <ctime>

#define VIDEO_BASE_LEN  (0x400000)
#define VIDEO_BASE_ADDR 0x1E000000

#define FPGA_BASE_ADDR  0x40600000
#define FPGA_BASE_LEN   0x2000
#include <iostream>
#include <sstream>
#include <string.h>

void SaveFromatBmp(Xilinx::XiImageDevice::ImageType* info, char * filename)
{
    cv::Mat M(info->height, info->width, CV_8UC1, cv::Scalar(0));
   //保存图片
    unsigned char *p;
    int i;
    for(i = 0 ; i < info->height; i++)
    {
        p = M.ptr<unsigned char>(i);
        memcpy(p, info->ImageBuff+info->width*i, info->width);
    }
    cv::imwrite("1111.bmp", M);
}

int main(int argc, char** argv)
{
    int memfd = open("/dev/mem", O_RDWR|O_SYNC);
    int memddf = open("/dev/Xipicture", O_RDWR|O_SYNC);

    void* pFpgaBaseAddr = mmap(NULL, FPGA_BASE_LEN,  PROT_READ|PROT_WRITE, MAP_SHARED, memfd , FPGA_BASE_ADDR);

    volatile unsigned int* fpgaInt = (volatile unsigned int*)pFpgaBaseAddr;

    void *pVideoBaseAddr= mmap(NULL, (0x400000*4), PROT_READ|PROT_WRITE, MAP_SHARED, memddf , 0);

    int buffNo = 0;
    for(buffNo = 0 ; buffNo < 4 ; buffNo++)
    {
        int rows = *(fpgaInt+0x90/4); //1024
        int cols = *(fpgaInt+0x80/4); //1280


        int i;
        int size = rows * cols;

        std::ostringstream filename;
        filename<<"image"<<buffNo<<".bmp";

        std::cout<<"image size:"<<size<<" width:" << cols << " height:"<< rows <<" savename: "<< filename.str() <<std::endl;

        //保存图片
        cv::Mat M(rows, cols, CV_8UC1, cv::Scalar(0));
        unsigned char *p;
        for(i = 0 ; i < rows; i++)
        {
            p = M.ptr<unsigned char>(i);
            memcpy(p, ((char*)pVideoBaseAddr+ 0x400000*buffNo)+cols*i, cols);
        }
        cv::imwrite(filename.str(), M);
    }

    munmap(pVideoBaseAddr, (0x400000*4));
    munmap(pFpgaBaseAddr, FPGA_BASE_LEN);
    close(memfd);
    return 0;
}


