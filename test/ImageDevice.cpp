#include "XiImageDevice.h"
#include "XiPictureDriver.h"
#include <sys/time.h>

using namespace std;

int framerate;
time_t sec_temp;
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
    memcpy(buff, info->imagebuff, info->imagelen);
    delete[] buff;

}


void testPicture()
{
    Xilinx::XiImageDevice *ImageDevice =new Xilinx::XiImageDevice;
    uint8_t *src  = new uint8_t[1280*1024];
    memset(src, 10, 1024*1280);
    uint8_t *dest = new uint8_t[1024*1280];
    Xilinx::XiImageDevice::TImageInformation info;
    ImageDevice->PictureMirror(dest, src, info);
    sleep(1);
    delete ImageDevice;
}

void testCallback(void* p)
{
    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;

}


void testModule()
{
    Xilinx::XiImageDevice *ImageDevice =new Xilinx::XiImageDevice;
    ImageDevice->addModule("test", testCallback, NULL);

    getchar();
    delete ImageDevice;
    getchar();
}


void setbuffvalue(int argc, char**argv)
{
    Xilinx::XiImageDevice *ImageDevice =new Xilinx::XiImageDevice;

    int buffno;
    sscanf(argv[2], "%d", &buffno);
    int value;
    sscanf(argv[3], "%d", &value);
    int len;
    sscanf(argv[4], "%d", &len);

    ImageDevice->setFbValue(buffno, value, len);
    getchar();
    delete ImageDevice;
}

void Help(char* name)
{
    cout<<"User:"<<name<<" Num"<<endl;
    cout<<"User:"<<name<<" 1  "<<endl;
    cout<<"User:"<<name<<" 2  "<<endl;
    cout<<"User:"<<name<<" 3  Block module test"<<endl;

}


void testBlockModule()
{
    Xilinx::XiImageDevice *ImageDevice =new Xilinx::XiImageDevice;
    std::cout<<"start test BlockModule ..."<<std::endl;
    ImageDevice->BlockModule();
    std::cout<<"end   test BlockModule ..."<<std::endl;
    getchar();
    delete ImageDevice;
}


int main(int argc, char** argv)
{
    if (argc<2)
    {
        Help(argv[0]);
        return 0;
    }

    std::string cmd = argv[1];

    if (cmd == "1")
        testModule();
    else if(cmd == "2")
        testPicture();
    else if(cmd == "3")
        testBlockModule();
    return 0;


    Xilinx::XiImageDevice *ImageDevice =new Xilinx::XiImageDevice;
    ImageDevice->enableBlockModule();
    sleep(1);
    ImageDevice->loopBlockModule();
    getchar();
    delete ImageDevice;

    return 0;
}
