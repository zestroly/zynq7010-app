#ifndef MXCNETWORK_H
#define MXCNETWORK_H

#include <sys/socket.h>
#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <list>

#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <fcntl.h>
#include <unistd.h>

#include <net/route.h>
#include <net/if_arp.h>
#include <netinet/ether.h>
#include <sys/types.h>


using namespace std;

namespace MxcDriver{

string Net_ioctl(string iface, unsigned int flag);

class INetWork
{
    public:
        typedef enum{
            IPM_AUTO,
            IPM_DHCP,
            IPM_STATIC,
            IPM_NONE,
        }TIPMethod;

        typedef struct _NetWork{
            TIPMethod method;
            string ip;
            string netmask;
            string gateway;
            string mac;
        }TNetWork;
};


class MxcNetWork
{
    public:
        MxcNetWork(const char *ifname = "eth0");
        ~MxcNetWork();

        void setNetWork(const INetWork::TNetWork &network);
        void getNetWork(INetWork::TNetWork &network);
        void restartNetWork();

        string getMac(string iface);

    public:
        void listDoc();
        typedef list<string> ListStr;
    private:
        bool readFile();
        bool saveFile();
        void paraseFile(INetWork::TNetWork &network, bool isSetting);
        void setEthXNode(INetWork::TNetWork &network);
        void getEthXNode(INetWork::TNetWork &network);


        ListStr interfacesDoc;
        FILE* pfile;
        string ethX;
        ListStr EthXNode;
        ListStr::iterator EthXPos;
        bool isaddEthX;

};


}

#endif

