#include "libnetwork.h"
#include <sstream>
#include <iostream>
#include <unistd.h>
int main()
{
    std::cout<< MxcDriver::Net_ioctl("eth0", SIOCGIFADDR)<<"|"<<std::endl;
    std::cout<< MxcDriver::Net_ioctl("eth0", SIOCGIFNETMASK)<<"|"<<std::endl;
    std::cout<< MxcDriver::Net_ioctl("eth0", SIOCGIFHWADDR)<<"|"<<std::endl;
    std::cout<< MxcDriver::Net_ioctl("eth0", SIOCGIFBRDADDR)<<"|"<<std::endl;
    return 0;

    MxcDriver::MxcNetWork network;
    MxcDriver::INetWork::TNetWork nw;
    network.getNetWork(nw);

    std::cout<<nw.method<<std::endl;
    std::cout<<nw.ip<<std::endl;
    std::cout<<nw.netmask<<std::endl;
    std::cout<<nw.gateway<<std::endl;
    std::cout<<nw.mac<<std::endl;

    int bb = 2;
    while(true)
    {
        bb++;
        stringstream sster;
        sster<<bb;
        nw.method = MxcDriver::INetWork::IPM_STATIC;
        nw.ip = "192.168.0."+sster.str();
        nw.netmask = "255.255.255.0";
        nw.gateway = "192.168.0.1";
        network.setNetWork(nw);
        network.restartNetWork();
        sleep(5);
        system("ifconfig");
    }

}

