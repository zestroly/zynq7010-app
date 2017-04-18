#include <stdio.h>
#include <unistd.h>

#include "XiXmlDevice.h"

int test()
{
    Xilinx::XiXmlDevice XmlDevice;
    xmlDocPtr doc;
    xmlNodePtr cur;

    doc = xmlReadFile("cammer.xml", "utf-8", XML_PARSE_RECOVER);
    XmlDevice.XmlSetting(doc);

    xmlFreeDoc(doc);
    xmlCleanupParser();
    return 0;
}


using namespace std;

int main(int argc , char**argv)
{
    if(argc < 3)
    {
        std::cout<<"cmd error\n"
            <<"usage:"<<argv[0]<<" "<<"set/get" <<" xmlpath\n";
        return -1;
    }

    string cmd = argv[1];
    string filename=argv[2];
    Xilinx::XiXmlDevice XmlDevice;

    if(cmd == "set")
    {
        XmlDevice.XmlSetting(filename.c_str());
    }else if(cmd == "get")
    {
        XmlDevice.XmlGetting(filename.c_str());
    }else{
        std::cout<<"cmd error\n"
            <<"usage:"<<argv[0]<<" "<<"set/get" <<" xmlpath\n";
    }

    return 0;
}


