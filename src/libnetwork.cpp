#include <iostream>
#include "libnetwork.h"
#include <fstream>
#include <stdio.h>
#include <string.h>


namespace MxcDriver{

string Net_ioctl(string iface, unsigned int flag)
{
    string ret = "";
    int inet_sock;
    struct ifreq ifr;
    inet_sock = socket(AF_INET, SOCK_DGRAM, 0);

    strcpy(ifr.ifr_name, iface.c_str());
    switch(flag)
    {
        case SIOCGIFADDR:
            if(ioctl(inet_sock, flag, &ifr) < 0)
            {
                ret = "";
            }else{
                ret = inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr);
            }
            break;
        case SIOCGIFNETMASK:
            if(ioctl(inet_sock, flag, &ifr) < 0)
            {
                ret = "";
            }else{
                ret = inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_netmask))->sin_addr);
            }
            break;
        case SIOCGIFHWADDR:
            {
                if(ioctl(inet_sock, flag, &ifr) < 0)
                {
                    ret = "";
                }else{
                    char mac_addr[60] = {0};
                    sprintf(mac_addr,"%02X%02X%02X%02X%02X%02X",
                            (unsigned char)ifr.ifr_hwaddr.sa_data[0],
                            (unsigned char)ifr.ifr_hwaddr.sa_data[1],
                            (unsigned char)ifr.ifr_hwaddr.sa_data[2],
                            (unsigned char)ifr.ifr_hwaddr.sa_data[3],
                            (unsigned char)ifr.ifr_hwaddr.sa_data[4],
                            (unsigned char)ifr.ifr_hwaddr.sa_data[5]);
                    ret = mac_addr;
                }
            }
            break;
        case SIOCGIFBRDADDR:
            if(ioctl(inet_sock, flag, &ifr) < 0){
                ret = "";
            }else{
                ret = inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_broadaddr))->sin_addr);
            }
            break;
    }
    close(inet_sock);
    return ret;
}





MxcNetWork::MxcNetWork(const char *ifname)
{
    ethX = ifname;
    isaddEthX = true;
}

bool MxcNetWork::readFile()
{
    pfile = fopen("/etc/network/interfaces", "r+");
    if (NULL == pfile)
    {
        std::cout<<"can not open file interfaces"<<std::endl;
    }

    interfacesDoc.clear();
    EthXNode.clear();

    char strLine[1024];
    while(!feof(pfile))
    {
        memset(strLine, 0, sizeof(strLine));
        fgets(strLine, 1024, pfile);
        interfacesDoc.push_back(strLine);
    }

    EthXPos = interfacesDoc.end();
    fclose(pfile);
    return true;
}

bool MxcNetWork::saveFile()
{
    pfile = fopen("/etc/network/interfaces", "w+");
    ListStr::iterator i;
    for(i = interfacesDoc.begin(); i != interfacesDoc.end(); i++)
    {
        fputs(i->c_str(), pfile);
        *i;
    }
    fclose(pfile);
}


void MxcNetWork::setNetWork(const INetWork::TNetWork &network)
{
    INetWork::TNetWork temp = network;
    paraseFile(temp, true);
    saveFile();
}

void MxcNetWork::getNetWork(INetWork::TNetWork &network)
{
    paraseFile(network, false);
}



void MxcNetWork::setEthXNode(INetWork::TNetWork &network)
{
    ListStr::iterator i;
    EthXNode.clear();
    if(isaddEthX == true)
    {
        EthXNode.push_back("auto "+ ethX + "\n");
    }

    switch(network.method)
    {
        case INetWork::IPM_AUTO:
            {
                std::string temp;
                temp += "iface "+ ethX + " inet"+" manual\n";
                EthXNode.push_back(temp);
            }
            break;
        case INetWork::IPM_DHCP:
            {
                std::string temp;
                temp += "iface "+ ethX + " inet"+" dhcp\n";
                EthXNode.push_back(temp);
            }
            break;
        case INetWork::IPM_STATIC:
            {
                std::string temp;
                temp += "iface "+ ethX + " inet"+" static\n";
                EthXNode.push_back(temp);
                if(network.ip != "")
                {
                    EthXNode.push_back("address " + network.ip + "\n");
                }
                if(network.netmask != "")
                {
                    EthXNode.push_back("netmask " + network.netmask + "\n");
                }
                if(network.gateway != "")
                {
                    EthXNode.push_back("gateway " + network.gateway + "\n");
                }
                if(network.mac != "")
                {
                    //  EthXNode.push_back("pre-up ip link set dev " + ethX + " down\n");
                    //  EthXNode.push_back("pre-up ip link set dev " + ethX + " address " + network.mac + "\n");
                }
            }
            break;
    }
}


void trimSpace(string& s)
{
    s.erase(std::remove(s.begin(), s.end(), ' '), s.end());
}

void trimEnter(string& s)
{
    s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
}

void MxcNetWork::getEthXNode(INetWork::TNetWork &network)
{
    if(EthXNode.empty())
    {
        network.method = INetWork::IPM_NONE;
        return;
    }

    ListStr::iterator i = EthXNode.begin();
    string temp = *i;
    if(temp.rfind("manual") != string::npos) {
        network.method = INetWork::IPM_AUTO;
    }else if(temp.rfind("static") != string::npos) {
        network.method = INetWork::IPM_STATIC;
    }else if(temp.rfind("dhcp") != string::npos) {
        network.method = INetWork::IPM_DHCP;
    }

    i++;
    for(i = EthXNode.begin(); i != EthXNode.end(); i++)
    {
        temp = *i;
        int pos;
        if( (pos = temp.find("address")) != string::npos && temp.find("hwaddress ether") == string::npos ){
            temp = temp.substr(strlen("address"));
            trimSpace(temp);
            trimEnter(temp);
            network.ip = temp;
        }else if( (pos = temp.find("netmask")) != string::npos){
            temp = temp.substr(strlen("netmask"));
            trimSpace(temp);
            trimEnter(temp);
            network.netmask = temp;
        } else if( (pos = temp.find("gateway")) != string::npos){
            temp = temp.substr(strlen("gateway"));
            trimSpace(temp);
            trimEnter(temp);
            network.gateway = temp;
        }
    }
    network.mac =Net_ioctl(ethX, SIOCGIFHWADDR);

}

string MxcNetWork::getMac(string ethX)
{
    return Net_ioctl(ethX, SIOCGIFHWADDR);
}

void MxcNetWork::paraseFile(INetWork::TNetWork &network, bool isSetting)
{
    readFile();
    ListStr::iterator i;
    bool EthXStart = false;
    bool EthXend   = false;
    bool EthXEixst = false;
    for(i = interfacesDoc.begin(); i != interfacesDoc.end(); i++)
    {
        while(((*i)[0] == 32) || ((*i)[0] == '\t'))
        {
            (*i) = i->substr(1);
        }

        if ((*i)[0] == '\n' ||  (*i)[0] == '#')
            continue;

        if (i->find("auto", 0) == 0 )
        {
            if(i->find(ethX , 0) != string::npos)
            {
                EthXStart = true;
                EthXPos = i;
                isaddEthX = false;
                continue;
            }
            else
                EthXStart = false;
        }

        if(EthXStart == true)
        {
            EthXNode.push_back(*i);
            i = interfacesDoc.erase(i);
            i--;
        }
    }


    if(isSetting == true)
    {
        setEthXNode(network);
    }else{
        getEthXNode(network);
    }

    if(EthXPos != interfacesDoc.end())
        interfacesDoc.splice(++EthXPos, EthXNode);
    else{
        interfacesDoc.splice(EthXPos , EthXNode);
    }
}


void MxcNetWork::restartNetWork()
{
    system("/etc/init.d/networking restart");
}



void MxcNetWork::listDoc()
{
    ListStr::iterator i;
    for(i = interfacesDoc.begin(); i != interfacesDoc.end(); i++)
    {
        std::cout<<(*i);
    }
}

MxcNetWork::~MxcNetWork()
{
    interfacesDoc.clear();
}





}



