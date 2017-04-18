#ifndef __XINETSERVER_H_
#define __XINETSERVER_H_
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>

namespace Xilinx{

class XiNetServer{
    public:
        XiNetServer();
        ~XiNetServer();
        void sendImage(unsigned char* buff, int bufflen);

    private:
        int sockfd;
        struct sockaddr_in servaddr;
};




}

#endif //__XINETSERVER_H_
