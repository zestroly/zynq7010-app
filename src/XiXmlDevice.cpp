#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include <string.h>

#include <string>
#include <iostream>
#include <sstream>

#include "XiXmlDevice.h"

namespace Xilinx{

XiXmlDevice::XiXmlDevice()
{
    this->doc = NULL;
    this->cur = NULL;
    this->name= NULL;
    this->value=NULL;
    this->root_node=NULL;
    xmlKeepBlanksDefault(0);

}

XiXmlDevice::~XiXmlDevice()
{
    this->doc = NULL;
    this->cur = NULL;
    this->name= NULL;
    this->value=NULL;
    this->root_node=NULL;
}




void NodePropInt(xmlNodePtr node, std::string key,  unsigned long Register, bool method)
{
    std::string value;
    xmlChar* attr_value = NULL;
    if(xmlHasProp(node, BAD_CAST key.c_str()))
    {
        if(method == XILXMLFILEREAD)
        {
            attr_value = xmlGetProp(node, BAD_CAST key.c_str());
            if(attr_value != NULL)
            {
                std::string tempvalue = (char*)attr_value;
                //寄存器操作
                int temp32 = atoi((char*)attr_value);
                xmlFree(attr_value);
            }
        }else{
            int temp32 = 1;

            std::stringstream ss;
            ss<<temp32;
            ss>>value;
            xmlSetProp(node, BAD_CAST key.c_str(), BAD_CAST value.c_str());
        }
    }else{
        xmlNewProp(node, BAD_CAST key.c_str(), BAD_CAST value.c_str());
    }
}


void ItemNode(xmlNodePtr node)
{
    std::string key = "Value";
    std::string NodeValue;

    xmlChar* attr_value = NULL;
    std::string NodeName =(char*)node->name;

   Xilinx::XiXmlDriver* sen = new Xilinx::XiXmlDriver("/home/root/config/m001.xml");

    if(xmlHasProp(node, BAD_CAST key.c_str() ))
    {
            attr_value = xmlGetProp(node, BAD_CAST key.c_str());
            if(attr_value != NULL)
            {
                NodeValue = (char*)attr_value;
                sen->setItem((char*)node->name, NodeValue);
                xmlFree(attr_value);
            }
    }
    delete sen;
}

void SensorNodeParama(xmlNodePtr node)
{
    xmlNodePtr temp_node = NULL;
    for(temp_node = node->children; temp_node != NULL ; temp_node = temp_node->next)
    {
        ItemNode(temp_node);
    }
}




void CameraNode(xmlNodePtr node)
{
    xmlNodePtr temp_node = NULL;
    for(temp_node = node->children; temp_node != NULL ; temp_node = temp_node->next)
    {
        SensorNodeParama(temp_node);
    }
}


void XmlRootSetting(xmlNodePtr root)
{
    xmlNodePtr cur = NULL;
    for(cur = root; cur != NULL; cur = cur->next)
    {//parase
        xmlNodePtr camera_node = NULL;
        for(camera_node = root->children; camera_node != NULL; camera_node = camera_node->next)
        {//camera
            if(!xmlStrcmp(camera_node->name, (const xmlChar*)"camera"))
            {
                CameraNode(camera_node);
            }
        }
    }

    //设置完全部通知fpga生效
    AxiConfigDev* EpcDev = new AxiConfigDev;
    EpcDev->setRegisterValue(0x40600200,1);
    delete EpcDev;
}

void XiXmlDevice::XmlSetting(XmlUrl Url)
{
    this->doc = xmlReadFile(Url.c_str(), "utf-8", XML_PARSE_RECOVER);
    if(this->doc == NULL)
    {
        std::cout<<"Loading xml file failed."<<std::endl;
        return;
    }

    //获取根节点
    this->root_node = xmlDocGetRootElement(this->doc);
    if(this->root_node == NULL)
    {
        std::cout<<"empty file"<<std::endl;
        return;
    }

    XmlRootSetting(this->root_node);

    //设置完后释放资源
    xmlFreeDoc(this->doc);
    xmlCleanupParser();
    return;
}

void ItemNodeUpdate(xmlNodePtr node)
{
    std::string key = "Value";
    std::string NodeValue;

    xmlChar* attr_value = NULL;
    std::string NodeName =(char*)node->name;

    Xilinx::XiXmlDriver* sen = new Xilinx::XiXmlDriver("/home/root/config/m001.xml");
    NodeValue = sen->getItem((char*)node->name);
    delete sen;

    if(xmlHasProp(node, BAD_CAST key.c_str() ))
    {
        xmlSetProp(node, BAD_CAST key.c_str(), BAD_CAST NodeValue.c_str());
    }else{
        xmlNewProp(node, BAD_CAST key.c_str(), BAD_CAST NodeValue.c_str());
    }
}

void CameraNodeUpdate(xmlNodePtr node)
{
    xmlNodePtr temp_node = NULL;
    for(temp_node = node->children; temp_node != NULL ; temp_node = temp_node->next)
    {
        xmlNodePtr temp_node_children = NULL;
        for(temp_node_children = temp_node->children; temp_node_children != NULL ; temp_node_children = temp_node_children->next)
        {
            ItemNodeUpdate(temp_node_children);
        }
    }
}

void XmlRootGetting(xmlNodePtr root)
{
    xmlNodePtr cur;
    for(cur = root; cur != NULL; cur = cur->next)
    {//parase
        xmlNodePtr camera_node = NULL;
        for(camera_node = root->children; camera_node != NULL; camera_node = camera_node->next)
        {//camera
            if(!xmlStrcmp(camera_node->name, (const xmlChar*)"camera"))
            {
                CameraNodeUpdate(camera_node);
            }
        }
    }
}

void XiXmlDevice::XmlGetting(XmlUrl Url)
{
    this->doc = xmlReadFile(Url.c_str(), "utf-8", XML_PARSE_RECOVER);
    if(this->doc == NULL)
    {
        std::cout<<"Loading xml file failed."<<std::endl;
        return;
    }

    //获取根节点
    this->root_node = xmlDocGetRootElement(this->doc);
    if(this->root_node == NULL)
    {
        std::cout<<"empty file"<<std::endl;
        return;
    }

    XmlRootGetting(root_node);
#if 0
    for(this->cur = this->root_node; this->cur != NULL; this->cur = this->cur->next)
    {//parase
        xmlNodePtr camera_node = NULL;
        for(camera_node = this->root_node->children; camera_node != NULL; camera_node = camera_node->next)
        {//camera
            if(!xmlStrcmp(camera_node->name, (const xmlChar*)"camera"))
            {
                CameraNodeUpdate(camera_node);
            }
        }
    }
#endif
    //保存
    int nRel=0;
    nRel = xmlSaveFormatFile(Url.c_str(), doc, 1);
    if(nRel!=-1)
    {
      std::cout<<"one xml file create is write"<<nRel<<"bytes"<<std::endl;
    }

    xmlFreeDoc(this->doc);
    xmlCleanupParser();
    return;
}


void XiXmlDevice::XmlGetting(xmlDocPtr doc)
{
    xmlNodePtr root = NULL;
    if(doc != NULL)
        root = xmlDocGetRootElement(doc);
    if(root != NULL)
        XmlRootGetting(root);
}


void XiXmlDevice::XmlSetting(xmlDocPtr doc)
{
    xmlNodePtr root = NULL;
    if(doc != NULL)
        root = xmlDocGetRootElement(doc);
    if(root != NULL)
        XmlRootSetting(root);
}



}


