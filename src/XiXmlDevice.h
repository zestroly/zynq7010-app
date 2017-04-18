#ifndef __XIXMLDEVICE_H_
#define __XIXMLDEVICE_H_

#include "XiXmlDriver.h"
#include <string>

namespace Xilinx{

#define XILXMLFILEREAD  true
#define XILXMLFILEWRITE false

typedef std::string XmlUrl;

class XiXmlDevice {
    public:
        //用xml内存块设置参数
        void XmlSetting(xmlDocPtr doc);
        //读取参数到xml内存块中
        void XmlGetting(xmlDocPtr doc);
    private:
        xmlDocPtr doc;
        xmlNodePtr cur;
        xmlNodePtr root_node;
        char *name;
        char *value;
    public:
        XiXmlDevice();
        ~XiXmlDevice();
        //设置xml文件
        void XmlSetting(XmlUrl filepath);
        //获取xml文件
        void XmlGetting(XmlUrl filepath);

};

}

#endif //__XIXMLDEVICE_H_
