#include "XiImageDevice.h"
#include "XiNetServer.h"
#include <regex>


namespace Xilinx{

XiNetServer::XiNetServer()
{
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9000);
    servaddr.sin_addr.s_addr = inet_addr("192.168.0.35");
}

XiNetServer::~XiNetServer()
{
    close(sockfd);
}

void XiNetServer::sendImage(unsigned char* buff, int bufflen)
{
   // sendto(sockfd, "2", 1, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
    sendto(sockfd, "12312312312", 10, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
}

}


#include <sys/time.h>
#include <jrtplib3/rtpsession.h>
#include <jrtplib3/rtpsessionparams.h>
#include <jrtplib3/rtpudpv4transmitter.h>
#include <jrtplib3/rtpipv4address.h>
#include <jrtplib3/rtptimeutilities.h>
#include <jrtplib3/rtppacket.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace jrtplib;
Xilinx::XiNetServer Nserver;

void SaveFromatRaw(Xilinx::XiImageDevice::ImageType* info)
{
    FILE *pFile;
    pFile = fopen("1111.raw", "wb+");
    fwrite(info->ImageBuff, info->ImageLen, 1, pFile);
    fclose(pFile);
}


void SaveFromatBmp(Xilinx::XiImageDevice::ImageType* info)
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


void ImageCallback(Xilinx::XiImageDevice::ImageType* info)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    std::cout<<tv.tv_sec<<"."<<tv.tv_usec<<":"<<info->width<<"X"<<info->height<<std::endl;

    std::ostringstream stream;
    stream<<tv.tv_sec<<"_"<<tv.tv_usec<<".bmp";
    std::cout<<"****************8\n";

#if 0
    SaveFromatBmp(info);
    for(int i=0; i< 100; i++)
    {
        printf("%02x ", info->ImageBuff[i]);
    }
    printf("\n");

    Nserver.sendImage(info->ImageBuff, info->ImageLen);
#endif

}


void checkerror(int rtperr)
{
    if(rtperr <  0)
    {
        std::cout <<"ERROR: " <<RTPGetErrorString(rtperr) <<std::endl;
        exit(-1);
    }
}

RTPSession *session;
RTPSessionParams *sessionparams;
RTPUDPv4TransmissionParams *transparams;

int framerate;
unsigned int sec_temp;

void ImageCallbackSendPacket(Xilinx::XiImageDevice::ImageType* info)
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

    return;
	unsigned char silencebuffer[1024*1280];

    int templen = info->ImageLen;

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

  //  RTPTime delay(0.02);
   // RTPTime::Wait(delay);
    while(templen > 0)
    {
        if( templen > 1200)
        {
            session->SendPacket(info->ImageBuff+(info->ImageLen - templen),1200);
            templen -= 1200;
        }
        else{
            session->SendPacket(info->ImageBuff+(info->ImageLen - templen),templen);
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

#include <sstream>
int main(int argc, char** argv)
{
	uint8_t remoteip[]={192,168,0,36};
    if(argc < 2)
    {
        remoteip[0] = 192;
        remoteip[1] = 168;
        remoteip[2] = 0;
        remoteip[3] = 36;
    }else {
        std::cout<<"will send stream to:"<<argv[1]<<std::endl;
        std::regex pattern("^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$");
        if(regex_match(argv[1],pattern))
        {
            int temp;
            char ctemp;
            std::istringstream iss(argv[1]);
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

    Xilinx::XiImageDevice* image = new Xilinx::XiImageDevice();
#ifdef WIN_CLIENT
    initRtp(remoteip);
    image->registerImageCallback(ImageCallbackSendPacket);
#else
    image->registerImageCallback(ImageCallback);
#endif
    getchar();
    image->registerImageCallback(NULL);
    delete image;
    sleep(1);
    destroyRtp();
    return 0;
}


