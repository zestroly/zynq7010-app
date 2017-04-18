#include "XiXmlDriver.h"
#include "libnetwork.h"

#include <string.h>
#include <iostream>
#include <sstream>


#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

namespace Xilinx{

#define AXI_EPC_WRITE 1
#define AXI_EPC_READ  2<<1

//AxiConfigDev
AxiConfigDev::AxiConfigDev()
{
    this->devfd = open(DEVNAME, O_RDWR|O_SYNC);
}

AxiConfigDev::~AxiConfigDev()
{
    close(this->devfd);
}

void AxiConfigDev::setRegisterValue(uint32_t address, uint32_t value)
{
    if(this->devfd <= 0)
        return;
    memset(&this->Rtemp, 0, sizeof(this->Rtemp));
    this->Rtemp.address  = address;
    this->Rtemp.value = value;
    ioctl(this->devfd, AXI_EPC_WRITE, &this->Rtemp);
}

uint32_t AxiConfigDev::getRegisterValue(uint32_t address)
{
    if(this->devfd <= 0)
        return 0;
    memset(&this->Rtemp, 0, sizeof(this->Rtemp));
    this->Rtemp.address = address;
    ioctl(this->devfd, AXI_EPC_READ, &this->Rtemp);
    return Rtemp.value;
}




/* m001
 *
 * */

XiXmlDriver::XiXmlDriver(std::string filexml)
{
    this->pdoc = xmlReadFile(filexml.c_str(), "utf-8", XML_PARSE_RECOVER);
    if(this->pdoc == NULL)
    {
        std::cout<<"Loading xml file failed."<<std::endl;
    }

    //获取根节点
    this->pRootNode = xmlDocGetRootElement(pdoc);
    if(this->pRootNode == NULL)
    {
        std::cout<<"empty file"<<std::endl;
    }
    this->EpcDev = new AxiConfigDev;
}





XiXmlDriver::~XiXmlDriver()
{
    if(this->pdoc != NULL)
        xmlFreeDoc(this->pdoc);
    delete this->EpcDev;
}

bool XiXmlDriver::ParamItemGet(xmlNodePtr cmdNode, std::string PItem, std::string& PValue)
{
    xmlNodePtr children_node = NULL;

    std::string Propname;
    std::string Propreg;
    std::string Proptype;
    xmlChar* attr_value = NULL;

    if(xmlHasProp(cmdNode, BAD_CAST "name")){
        attr_value = xmlGetProp(cmdNode, BAD_CAST "name");
        if(attr_value != NULL)
        {
            Propname =(char*)attr_value;
            xmlFree(attr_value);
        }
        if(PItem != Propname)
        {
            return false;
        }
    }else{
        return false;
    }

    if(!xmlHasProp(cmdNode, BAD_CAST "reg"))
        return false;

    attr_value = xmlGetProp(cmdNode, BAD_CAST "reg");
    Propreg =(char*)attr_value;
    xmlFree(attr_value);


    if(xmlHasProp(cmdNode, BAD_CAST "type"))
    {
        attr_value = xmlGetProp(cmdNode, BAD_CAST "type");
        Proptype =(char*)attr_value;
        xmlFree(attr_value);

        if(Proptype == "String")
        {
            //读取寄存器的值
            char* offset;
            unsigned int Epcaddress = strtol(Propreg.c_str(), &offset, 0);
            unsigned int Epcvalue   = this->EpcDev->getRegisterValue(Epcaddress);

            std::ostringstream stream;
            stream<<Epcvalue;

            std::string intvaluetemp = stream.str();

            for(children_node = cmdNode->children; children_node != NULL ; children_node = children_node->next)
            {
                if(!xmlStrcmp(children_node->name, (const xmlChar*)"Combo"))
                {
                    std::string StringValue;
                    std::string IntValue;
                    if(xmlHasProp(children_node, BAD_CAST "IntValue"))
                    {
                        attr_value = xmlGetProp(children_node, BAD_CAST "IntValue");
                        IntValue =(char*)attr_value;
                        xmlFree(attr_value);
                        if(IntValue == intvaluetemp)
                        {
                            if(xmlHasProp(children_node, BAD_CAST "StringValue"))
                            {
                                attr_value = xmlGetProp(children_node, BAD_CAST "StringValue");
                                StringValue =(char*)attr_value;
                                xmlFree(attr_value);
                                PValue = StringValue;
                                return true;
                            }
                        }
                    }
                }
            }

            if(PItem == "DeviceDNA")
            {
                unsigned int DNAlow   = this->EpcDev->getRegisterValue(0x4060EF00);
                unsigned int DNAhight = this->EpcDev->getRegisterValue(0x4060EF04);
                char temp[100] = {0};
                sprintf(temp, "%06x%08x", DNAhight, DNAlow);
                MxcDriver::MxcNetWork* MNet = new MxcDriver::MxcNetWork;
                std::string macaddr=MNet->getMac("eth0");
                delete MNet;
                PValue=string(temp) +string("-") + macaddr;
                return true;
            }

        }else if(Proptype == "Int")
        {
            char* offset;
            unsigned int Epcaddress = strtol(Propreg.c_str(), &offset, 0);
            unsigned int Epcvalue;
            Epcvalue = this->EpcDev->getRegisterValue(Epcaddress);
            std::ostringstream stream;
            stream<<Epcvalue;
            PValue = stream.str();
            return true;
        }else if(Proptype == "Bool")
        {
            char* offset;
            unsigned int Epcaddress = strtol(Propreg.c_str(), &offset, 0);
            unsigned int Epcvalue;
            Epcvalue = this->EpcDev->getRegisterValue(Epcaddress);
            if(Epcvalue == 1)
            {
                PValue = "True";
            }else{
                PValue = "False";
            }
            return true;
        }else if(Proptype == "Double")
        {

            return true;
        }
    }
    return false;
}


unsigned char BuildGammaTable(int i,float fPrecompensation)
{
    float f;
    f = (i+0.5F)/256;
    f = (float)pow(f, fPrecompensation);
    return (unsigned )(f*256-0.5F);

}

bool XiXmlDriver::ParamItemSet(xmlNodePtr cmdNode, std::string PItem, std::string PValue)
{
    xmlNodePtr children_node = NULL;

    std::string Propname;
    std::string Propreg;
    std::string Proptype;
    xmlChar* attr_value = NULL;



    if(xmlHasProp(cmdNode, BAD_CAST "name"))
    {//找到cmdNode->name 和 PTem 相等的项
        attr_value = xmlGetProp(cmdNode, BAD_CAST "name");
        if(attr_value != NULL)
        {
            Propname =(char*)attr_value;
            xmlFree(attr_value);
        }

        if(PItem != Propname)
        {
            return false;
        }
    }else{
        return false;
    }

    if(!xmlHasProp(cmdNode, BAD_CAST "reg"))
        return false;

    attr_value = xmlGetProp(cmdNode, BAD_CAST "reg");
    if(attr_value != NULL)
    {
        Propreg =(char*)attr_value;
        xmlFree(attr_value);
    }


    if(xmlHasProp(cmdNode, BAD_CAST "type"))
    {
        attr_value = xmlGetProp(cmdNode, BAD_CAST "type");
        Proptype =(char*)attr_value;
        xmlFree(attr_value);

        if(Proptype == "String")
        {
            for(children_node = cmdNode->children; children_node != NULL ; children_node = children_node->next)
            {
               // std::cout<<PItem<<std::endl;
                if(!xmlStrcmp(children_node->name, (const xmlChar*)"Combo"))
                {
                    std::string StringValue;
                    std::string IntValue;
                    if(xmlHasProp(children_node, BAD_CAST "StringValue"))
                    {
                        attr_value = xmlGetProp(children_node, BAD_CAST "StringValue");
                        StringValue =(char*)attr_value;
                        xmlFree(attr_value);
                        if(StringValue == PValue)
                        {
                            if(xmlHasProp(children_node, BAD_CAST "IntValue"))
                            {
                                attr_value = xmlGetProp(children_node, BAD_CAST "IntValue");
                                IntValue =(char*)attr_value;
                                xmlFree(attr_value);
                                //设置寄存器
                             //   std::cout<<PItem<<"\t\t:"<<Propreg<<" <---> " <<IntValue<<std::endl;

                                char* offset;
                                unsigned int Epcaddress = strtol(Propreg.c_str(), &offset, 0);
                                unsigned int Epcvalue   = strtol(IntValue.c_str(), &offset, 0);

                                this->EpcDev->setRegisterValue(Epcaddress, Epcvalue);
                                // uint32_t getRegisterValue(uint32_t address);
                                return true;
                            }
                        }
                    }
                }
            }

        }else if(Proptype == "Int")
        {
            char* offset;
            unsigned int Epcaddress = strtol(Propreg.c_str(), &offset, 0);
            unsigned int Epcvalue   = strtol(PValue.c_str(), &offset, 10);
            this->EpcDev->setRegisterValue(Epcaddress, Epcvalue);
            return true;
            // std::cout<<Proptype<<std::endl;
        }else if(Proptype == "Bool")
        {
            char* offset;
            unsigned int Epcaddress = strtol(Propreg.c_str(), &offset, 0);

            if(PValue == "True")
            {
                this->EpcDev->setRegisterValue(Epcaddress, 1);
            }else{
                this->EpcDev->setRegisterValue(Epcaddress, 0);
            }
            return true;
        }
        else if(Proptype == "Double")
        {
            if(PItem == "Gamma")
            {//在fpga中加入gamma查找表
                char* offset;
                unsigned int Epcaddress = strtol(Propreg.c_str(), &offset, 0);
                double dEpcvalue   = atof(PValue.c_str());
              //  printf("%f\n", dEpcvalue);

                float  y;
                int i;
                unsigned short u16value;
                for(i = 0; i < 1024; i++)
                {
                    y = pow((i+0.0F)/1023, dEpcvalue) * 1023 - 0.0F;
                    u16value = (unsigned short)y;
                    this->EpcDev->setRegisterValue(0x4060F000+i*4, u16value);
                  //  printf("%u : %u : %u\n", i, u16value,  this->EpcDev->getRegisterValue(0x4060F000 + i*4));
                    // y = pow((i+0.5F)/256, 0.8F) * 256 - 0.5F;
                    // printf("%u : %f :%u :%u\n", i, y, (unsigned char) y, BuildGammaTable(i, 0.8));
                }
            }else{

            }
            return true;
        }
    }
    return false;
}

std::string XiXmlDriver::XmlSersor(std::string Nitem, std::string& Nvalue,  bool isSet)
{
    std::string itemvalue;

    if(this->pRootNode != NULL)
    {
        xmlNodePtr RootNode = pRootNode;

        for(; RootNode != NULL; RootNode = RootNode->next)
        {//root
            xmlNodePtr cmd_node = NULL;
            for(cmd_node = RootNode->children; cmd_node != NULL; cmd_node = cmd_node->next)
            {//cmd

                if(!xmlStrcmp(cmd_node->name, (const xmlChar*)"cmd"))
                {
                    if(isSet)
                    {
                        if(ParamItemSet(cmd_node, Nitem,  Nvalue))
                        {
                            return itemvalue;
                        }
                    }else{
                        if(ParamItemGet(cmd_node, Nitem, Nvalue))
                        {
                            return Nvalue;
                        }
                    }
                }
            }
        }
    }
    return itemvalue;
}

std::string XiXmlDriver::getItem(std::string Item)
{
    std::string value;
    //系统相关的参数在这里直接获取
    if(Item == "Address" || Item == "Netmask" || Item == "Gateway" || Item == "Method")
    {
        MxcDriver::MxcNetWork network;
        MxcDriver::INetWork::TNetWork nw;
        network.getNetWork(nw);
        if(Item == "Method")
        {
            if(nw.method == MxcDriver::INetWork::IPM_STATIC)
                value = "static";
            else if(nw.method == MxcDriver::INetWork::IPM_DHCP)
                value = "dhcp";
            else
                value = "none";
        }
    }else{
        value = XmlSersor(Item, value);
    }
    return value;
}


void XiXmlDriver::setItem(std::string Item, std::string value)
{
    XmlSersor(Item, value ,true);
    return;
}



}
