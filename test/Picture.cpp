#include <stdio.h>
#include <regex>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "XiPictureDriver.h"

#include <iostream>
#include <ctime>

#define VIDEO_BASE_LEN  (0x400000)
#define VIDEO_BASE_ADDR 0x1C000000

#define FPGA_BASE_ADDR  0x40600000
#define FPGA_BASE_LEN   0x2000
#include <iostream>
#include <sstream>
#include <string.h>
#include <sys/time.h>


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



void testCatchPictureSpeed()
{
    Xilinx::XiPictureDriver *pDriver =new Xilinx::XiPictureDriver;
    pDriver->registerImageCallback(ImageCallback);
    pDriver->setRegisterValue(0xE0, 2);
    pDriver->setRegisterValue(0x200, 1);
    getchar();
    delete pDriver;

}



void ScanfBuffData(int argc, char** argv)
{
    Xilinx::XiPictureDriver pDriver;
    int buffNo = 0;
    for(buffNo = 0 ; buffNo < 4 ; buffNo++)
    {
        int rows = pDriver.getHeight(); //1024
        int cols = pDriver.getWidth(); //1280

        int i;
        int size = rows * cols;

        std::ostringstream filename;
        filename<<"image"<<buffNo<<".bmp";

        std::cout<<"image size:"<<size<<" width:" << cols << " height:"<< rows <<" savename: "<< filename.str() <<std::endl;

        char *buff;
        pDriver.getPictureBuff(buffNo, &buff);

        //保存图片
        cv::Mat M(rows, cols, CV_8UC1, cv::Scalar(0));
        unsigned char *p;
        for(i = 0 ; i < rows; i++)
        {
            p = M.ptr<unsigned char>(i);
            memcpy(p, buff+cols*i, cols);
        }
        cv::imwrite(filename.str(), M);
    }
}

void getBuffImage(int argc, char** argv)
{
    if(argc < 4)
        return;

    std::string strbuffNo   = argv[2];
    std::string strsavepath = argv[3];


    int buffNo;
    switch(argv[2][0])
    {
        case '0':
            buffNo = 0;
            break;
        case '1':
            buffNo = 1;
            break;
        case '2':
            buffNo = 2;
            break;
        case '3':
            buffNo = 3;
            break;
    }

    Xilinx::XiPictureDriver pDriver;
    int rows = pDriver.getHeight(); //1024
    int cols = pDriver.getWidth(); //1280
    int i;
    int size = rows * cols;

    std::ostringstream filename;
    filename<<argv[3];

    std::cout<<"image size:"<<size<<" width:" << cols << " height:"<< rows <<" savename: "<< filename.str() <<std::endl;

    char *buff;
    pDriver.getPictureBuff(buffNo, &buff);

    //保存图片
    cv::Mat M(rows, cols, CV_8UC1, cv::Scalar(0));
    unsigned char *p;
    for(i = 0 ; i < rows; i++)
    {
        p = M.ptr<unsigned char>(i);
        memcpy(p, buff+cols*i, cols);
    }
    cv::imwrite(filename.str(), M);

}


#include <jrtplib3/rtpsession.h>
#include <jrtplib3/rtpsessionparams.h>
#include <jrtplib3/rtpudpv4transmitter.h>
#include <jrtplib3/rtpipv4address.h>
#include <jrtplib3/rtptimeutilities.h>
#include <jrtplib3/rtppacket.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace jrtplib;

void checkerror(int rtperr)
{
    if( rtperr < 0 )
    {
        std::cout <<"ERROR: " <<RTPGetErrorString(rtperr) <<std::endl;
        exit(-1);
    }
}

RTPSession *session;
RTPSessionParams *sessionparams;
RTPUDPv4TransmissionParams *transparams;

void ImageCallbackSendPacket(Xilinx::TImageType*  info)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    std::cout<<tv.tv_sec<<"."<<tv.tv_usec<<":"<<info->width<<"X"<<info->height;
    if(sec_temp == tv.tv_sec)
        framerate++;
    else{
        sec_temp = tv.tv_sec;
        std::cout<<"    framer rate:"<<framerate;
        framerate = 0;
    }

    std::cout<<std::endl;

    int templen = info->imagelen;
    std::cout<<"***************"<<templen<<std::endl;

    session->SetDefaultPayloadType(88);

    char sendInfo[20];
    memset(sendInfo, 0, 20);
    sendInfo[0] = (info->width>>0  ) & 0xff;
    sendInfo[1] = (info->width>>8  ) & 0xff;
    sendInfo[2] = (info->width>>16 ) & 0xff;
    sendInfo[3] = (info->width>>24 ) & 0xff;
    sendInfo[4] = (info->height>>0 ) & 0xff;
    sendInfo[5] = (info->height>>8 ) & 0xff;
    sendInfo[6] = (info->height>>16) & 0xff;
    sendInfo[7] = (info->height>>24) & 0xff;

    session->SendPacket(sendInfo,8);
    session->SetDefaultPayloadType(96);

    while(templen > 0)
    {
        if( templen > 1200)
        {
            session->SendPacket(info->imagebuff+(info->imagelen - templen),1200);
            templen -= 1200;
        }
        else{
            session->SendPacket(info->imagebuff+(info->imagelen - templen),templen);
            templen -= templen;
        }
    }

    //RTPTime::Wait(delay);
    session->SetDefaultPayloadType(99);
    session->SendPacket(sendInfo,8);

    //  RTPTime::Wait(delay);
}

void initRtp(const uint8_t remoteip[4])
{
    session  = new RTPSession;
    sessionparams = new RTPSessionParams;
    transparams = new RTPUDPv4TransmissionParams;
    sessionparams->SetOwnTimestampUnit(1.0/90000.0);

    transparams->SetPortbase(8000);
    session->Create(*sessionparams,transparams);

    RTPIPv4Address addr(remoteip,9000);

    session->AddDestination(addr);
    session->SetDefaultPayloadType(96);
    session->SetDefaultMark(false);
    session->SetDefaultTimestampIncrement(1);

    RTPTime delay(0.002);
    RTPTime starttime = RTPTime::CurrentTime();

}

void destroyRtp()
{
    RTPTime delay(0.002);
    session->BYEDestroy(delay,"Time's up",9);
    delete transparams;
    delete sessionparams;
    delete session;
}

int NetStreamData(char* ip)
{
    uint8_t remoteip[]={192,168,0,32};
    if(ip == NULL)
    {
        remoteip[0] = 192;
        remoteip[1] = 168;
        remoteip[2] = 0;
        remoteip[3] = 32;
        std::cout<<"will send stream to 192.168.0.32:"<<std::endl;
    }else {
        std::cout<<"will send stream to:"<<ip<<std::endl;
        std::regex pattern("^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$");
        if(regex_match(ip,pattern))
        {
            int temp;
            char ctemp;
            std::istringstream iss(ip);
            iss>>temp;
            remoteip[0] = (char)temp;
            iss>>ctemp;
            iss>>temp;
            remoteip[1] = (char)temp;
            iss>>ctemp;
            iss>>temp;
            remoteip[2] = (char)temp;
            iss>>ctemp;
            iss>>temp;
            remoteip[3] = (char)temp;
        }
    }
    Xilinx::XiPictureDriver* image = new Xilinx::XiPictureDriver();
    initRtp(remoteip);
    image->registerImageCallback(ImageCallbackSendPacket);
    getchar();
    return 0;
}

void testClass()
{
    while(1)
    {
        Xilinx::XiPictureDriver *pDriver =new Xilinx::XiPictureDriver;
        usleep(20000);
        pDriver->registerImageCallback(ImageCallback);
        usleep(20000);
        delete pDriver;
    }
}

void testBuffDataCopy()
{
    struct timeval tv, tv1;

    char* Fb2;
    char* Fb3;
    Xilinx::XiPictureDriver *pDriver =new Xilinx::XiPictureDriver;
    pDriver->getPictureBuff(2, &Fb2);
    pDriver->getPictureBuff(3, &Fb3);

    delete pDriver;
}


int main(int argc, char** argv)
{
    if(argc < 2)
    {
        printf("usage:\n");
        printf("     %s 1                  -----注册回调，测试抓取图片速度\n", argv[0]);
        printf("     %s 2                  -----获取4块缓冲区的数据\n", argv[0]);
        printf("     %s 3 ipaddr           -----通过网络把图片传输的目标远程主机上\n", argv[0]);
        printf("     %s 4                  -----XiPictureDriver new 和 delete 测试\n", argv[0]);
        printf("     %s 5 1 savedir   -----获取buffNo图片到savedir中\n", argv[0]);
        return 0;
    }
    std::string cmd = argv[1];
    if(cmd == "1")
        testCatchPictureSpeed();
    else if (cmd == "2")
        ScanfBuffData(argc, argv);
    else if (cmd == "3")
        NetStreamData(argv[3]);
    else if (cmd == "4")
        testClass();
    else if (cmd == "5")
        getBuffImage(argc, argv);
    else if (cmd =="6")
        testBuffDataCopy();

    return 0;
}


