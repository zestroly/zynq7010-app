
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdint.h>

#define IOMMAP_LEN 0x400000

int main()
{
    printf("cou\n");
    int fd = open("/dev/Image_m" , O_RDWR|O_SYNC);
    if(fd < 0)
    {
        printf("open device is error, fd=%d\n", fd);;
        return -1;
    }

    void* gg = mmap(NULL, 0x20000,  PROT_READ|PROT_WRITE, MAP_SHARED, fd , 0x40600000);


    int axifd= open("/dev/XiAxi" , O_RDWR|O_SYNC);
    void *axiaddr = mmap(NULL, 0x20000,
            PROT_READ|PROT_WRITE,
            MAP_SHARED,
            axifd , 0);


    unsigned int *fpgaAxi = (unsigned int *)axiaddr;



    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned int sec_temp = 0;
    int framerate = 0;

    unsigned int* pfpga = (unsigned int*)gg;



    while(1)
    {

        uint32_t rel = *(pfpga + 0xD4/4);
        uint8_t  buffNo = ((rel>>4) & 0x3);
        uint8_t  buffstatus = (rel & 0xF);
        while(!buffstatus)
        {
            usleep(1000);
            rel = *(pfpga + 0xD4/4);
            buffNo = ((rel>>4) & 0x3);
            buffstatus = (rel & 0xF);
        }

        uint32_t status = *(pfpga+0xD8/4);
        *(pfpga+0xD8/4) = status|(0x10<<buffNo);

        void* mapbuf = mmap(NULL, IOMMAP_LEN,  PROT_READ|PROT_WRITE, MAP_SHARED, fd , 0x1E000000);
        unsigned int *pInt = (unsigned int *)mapbuf;
        gettimeofday(&tv, NULL);
        if(sec_temp == tv.tv_sec)
            framerate++;
        else{
            sec_temp = tv.tv_sec;
            printf("    framer rate:%d\n", framerate);
            framerate = 0;
        }

        void *buff = malloc(1280 * 1024+1000);
        int *ptemp = (int*)buff;
        int i;
      //  memcpy(buff, mapbuf, 1024*1280);
        for(i = 0 ; i < (1280*1024 / 4) ; i++ )
        {
           *(ptemp + i) = *(pInt+i);
        }
        printf("%d:", buffNo);
        printf("%08x ", ptemp[0]);
        printf("%08x ", ptemp[1]);
        printf("%08x ", ptemp[2]);
        printf("%08x ", ptemp[3]);
        printf("%08x ", ptemp[4]);
        printf("%08x \n", ptemp[5]);

        free(buff);
        munmap(mapbuf, IOMMAP_LEN);

        status = *(pfpga+0xD8/4);
        *(pfpga+0xD8/4) = status|(0x1<<buffNo);
    }

    close(fd);
    return 0;
}
