#ifndef __XIXMLDRIVER_H_
#define __XIXMLDRIVER_H_

#include <iostream>
#include <string>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include <sys/types.h>
#include <stdint.h>

#define DEVNAME "/dev/Image_m"

namespace Xilinx{

class AxiConfigDev{
    public:
        typedef struct AXI_EPC_DATA{
            uint32_t address;
            uint32_t value;
        }AxiRegister;

        AxiConfigDev();
        ~AxiConfigDev();

        void setRegisterValue(uint32_t address, uint32_t value);
        uint32_t getRegisterValue(uint32_t address);

    private:
        char dev[128];
        AxiRegister Rtemp;
        int devfd;

};


class XiXmlDriver{
    public:
        XiXmlDriver(std::string filexml);
        ~XiXmlDriver();

        std::string XmlSersor(std::string Nitem, std::string& Nvalue,  bool isSet = false);
        bool ParamItemGet(xmlNodePtr cmdNode, std::string PItem, std::string& PValue);
        bool ParamItemSet(xmlNodePtr cmdNode, std::string PItem, std::string PValue);

        void setItem(std::string Item, std::string value);
        std::string getItem(std::string Item);
    private:
        xmlDocPtr pdoc;
        xmlNodePtr pRootNode;
        AxiConfigDev *EpcDev;
};

}

#endif //__XIXMLDRIVER_H_
