#ifndef __XISENSORDEVICE_H_
#define __XISENSORDEVICE_H_

#include <iostream>
#include <string>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include "AxiEpcDevice.h"

namespace Xilinx{

class XiSensorDevice{
    public:
        XiSensorDevice(std::string filexml);
        ~XiSensorDevice();


        std::string XmlSersor(std::string Nitem, std::string& Nvalue,  bool isSet = false);
        bool ParamItemGet(xmlNodePtr cmdNode, std::string PItem, std::string& PValue);
        bool ParamItemSet(xmlNodePtr cmdNode, std::string PItem, std::string PValue);

        void setItem(std::string Item, std::string value);
        std::string getItem(std::string Item);
    private:
        xmlDocPtr pdoc;
        xmlNodePtr pRootNode;
        AxiEpcDevice *EpcDev;
};

}

#endif //__XISENSORDEVICE_H_
