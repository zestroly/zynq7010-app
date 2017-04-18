#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <iostream>
#include <ctime>

#define VIDEO_BASE_LEN  (0x400000)
#define VIDEO_BASE_ADDR 0x1E000000

#define FPGA_BASE_ADDR  0x40600000
#define FPGA_BASE_LEN   0x2000
#include <iostream>
#include <sstream>


int main(int argc, char** argv)
{
    int memfd = open("/dev/mem", O_RDWR|O_SYNC);

    void* pFpgaBaseAddr = mmap(NULL, FPGA_BASE_LEN,  PROT_READ|PROT_WRITE, MAP_SHARED, memfd , FPGA_BASE_ADDR);

    int buffNo = 0;

    unsigned int sec_temp = 0;
    int framerate = 0;


    for(buffNo = 0 ; buffNo < 4 ; buffNo++)
    {
        printf("buff no. %d:\n", buffNo);
        void *pVideoBaseAddr= mmap(NULL, VIDEO_BASE_LEN, PROT_READ|PROT_WRITE, MAP_SHARED, memfd , VIDEO_BASE_ADDR+(0x400000*buffNo));
        volatile unsigned int *pInt = (volatile unsigned int *)pVideoBaseAddr;
        int i;

        for(i = 0 ; i < (20 / 4) ; i++ )
        {
            printf("%08x ", *(pInt+i));
        }
        usleep(1000);
        printf("\n");

        munmap(pVideoBaseAddr, VIDEO_BASE_LEN);
    }

    munmap(pFpgaBaseAddr, FPGA_BASE_LEN);
    close(memfd);
    return 0;
}


