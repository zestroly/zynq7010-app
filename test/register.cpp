
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>



int main(int argc, char** argv)
{
    if(argc < 3)
    {
        std::cout<<argv[0]<<" "<<"read/write PhyAddr [value]"<<std::endl;
        std::cout<<argv[0]<<" "<<"read  PhyAddr"<<std::endl;
        std::cout<<argv[0]<<" "<<"write PhyAddr [value]"<<std::endl;
        std::cout<<argv[0]<<" "<<"read  0x40600080"<<std::endl;
        std::cout<<argv[0]<<" "<<"write 0x40600080 0x400"<<std::endl;
        return 0;
    }

    std::string cmd = argv[1];
    if(cmd == "write" && argc < 4 )
        return 0;

    unsigned int PhyAddr;
    sscanf(argv[2], "%x", &PhyAddr);

    int fd = open("/dev/mem", O_RDWR|O_SYNC);
    char *VirAddr =(char*)mmap(NULL,0x1000 , PROT_READ|PROT_WRITE,  MAP_SHARED, fd, PhyAddr>>12<<12);

    unsigned int dist = (PhyAddr - ((PhyAddr>>12)<<12));

    unsigned int *Inttemp = (unsigned int *)(VirAddr + (PhyAddr - ((PhyAddr>>12)<<12)));

    if(cmd == "write")
    {
        unsigned int value;
        sscanf(argv[3], "%x", &value);
        *Inttemp = value;
    }

    printf("%s %#08x=%#x\n", (cmd=="write"?"write":"read"), PhyAddr, *Inttemp);


    munmap(VirAddr, 0x1000);
    close(fd);
    return 0;
}




