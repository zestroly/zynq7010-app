#include "XiDriver.h"

#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>

#include "XiLog.h"

#define FPGAMODULEMAXNAME 100


namespace Xilinx{

static char Fpga_Modules[][FPGAMODULEMAXNAME]={
    "BlockModule",
    "MirrorModule",
};


struct _ErrModuleType{
    Herror Number;
    char  Msg[128];
}ErrorModuleInfo[] = {
    {MODULE_SUCCESS,  "successful."},
    {MODULE_NOFIND_MODULE,  "no find module."},
    {MODULE_NAME_IS_NULL,  "input name is null."},
    {MODULE_NOFIND_PARAM,  "input param is no find."},
    {MODULE_NOFIND_OBJECT,  "input object is no find."},
    {MODULE_BLOCK_OVERFLOW,  "the block module result is overflow."},
    {MODULE_BLOCK_TERMINATE,  "the block module was terminate with external."},
    {MOUDLE_BLOCK_NOSET_THRESHVALUE,  "the block module of param (threshvalue)  not setting."},

};


XiDriver::XiDriver()
{
    AllocErrLog();
    LOG(INFO_LEVEL, "start ..构造");
    ImageDevice = new XiImageDevice;

    ModuleName = "";
    LOG(INFO_LEVEL, "end   ..构造");
}


XiDriver::~XiDriver()
{
    LOG(INFO_LEVEL, "start ..虚构");
    Terminate();

    OutPutData.clear();
    IntPutData.clear ();
    ModueMutex.lock ();
    ModueMutex.unlock();
    delete ImageDevice;
    ModuleName="";
    DestroyErrLog();
    LOG(INFO_LEVEL, "end   ..虚构");
}

void XiDriver::Terminate()
{
    ImageDevice->Terminate();
}

void XiDriver::AllocErrLog()
{
    int i;
    int size = sizeof(ErrorModuleInfo)/sizeof(struct _ErrModuleType );
    for(i =0; i < size; i++)
    {
           ErrLog.insert (std::pair<Herror, char*>(ErrorModuleInfo[i].Number,   ErrorModuleInfo[i].Msg ));
    }

}

void XiDriver::DestroyErrLog()
{
    ErrLog.clear ();
}


const char* XiDriver::GetErrorMsg()
{
     if ( ErrLog.find (ErrNumber) != ErrLog.end () )
     {
         return ErrLog[ErrNumber];
     }else{
         return NULL;
     }
}

uint32_t XiDriver::GetSensorParam(const char* name)
{
        return ImageDevice->GetSensorParam(name);
}

void XiDriver::SetSensorParam(const char* name, uint32_t value)
{
        ImageDevice->SetSensorParam (name, value);
}

void* XiDriver::GrabPicture()
{
    ImageDevice->GrabPicture();
    //LOG(INFO_LEVEL, "抓图 ---->");
}

uint32_t XiDriver::GrabPicture (uint8_t* buff,    uint32_t bufflen)
{
    return ImageDevice->GrabPicture (buff,  bufflen);
}

void XiDriver::softTrigger()
{
    ImageDevice->softTrigger();
    LOG(INFO_LEVEL, "软出发 ---->");
}

Herror XiDriver::ExecuteBlock()
{
    if( IntPutData.find ("ThreshValue") == IntPutData.end ())
        return MOUDLE_BLOCK_NOSET_THRESHVALUE;

    if( IntPutData.find("InputImage") != IntPutData.end() &&
            IntPutData.find("Width") != IntPutData.end()  &&
            IntPutData.find("Height") != IntPutData.end() )
    {//设置图片来源soc
        TImageType  ImageInfo;
        ImageInfo.width = IntPutData["Width"];
        ImageInfo.height = IntPutData["Height"];
        ImageInfo.imagelen = IntPutData["Width"]*IntPutData["Height"] ;;
        ImageInfo.imagebuff = (unsigned char*)IntPutData["InputImage"];
        ImageDevice->enableBlockModule(&ImageInfo, IntPutData["ThreshValue"]);
    }else{//设置图片来源sensor
        ImageDevice->enableBlockModule(NULL, IntPutData["ThreshValue"]);
    }

    if(ImageDevice->waitBlockModuleFinished())
    {//获取到数据
        if( ImageDevice->hasOverflow() )
        {//溢出
            ErrNumber = MODULE_BLOCK_OVERFLOW;
        }else{ //未溢出
            ErrNumber = MODULE_SUCCESS;
        }
        //查询run个数
        OutPutData.insert (std::pair<std::string, long>("HrunCount",   ImageDevice->BlockModuleRunCount() ));
        OutPutData.insert (std::pair<std::string, long>("HrunOutPutBuff",  (long)ImageDevice->getHrunBuff() ));

        return ErrNumber;
    }else{//获取失败
        ErrNumber = MODULE_BLOCK_TERMINATE;
        return ErrNumber;
    }
}

Herror XiDriver::ExecuteMirror()
{
    ErrNumber = MODULE_SUCCESS;
    ImageDevice->clearMirror ();

    if( IntPutData.find ("MirrorX")  != IntPutData.end ())
    {
        if(IntPutData["MirrorX"] != 0)
            ImageDevice->setMirrorX(true);
        else
            ImageDevice->setMirrorX(false);
    }

    if( IntPutData.find ("MirrorY")  != IntPutData.end ())
    {
        if(IntPutData["MirrorY"] != 0)
            ImageDevice->setMirrorY(true);
        else
            ImageDevice->setMirrorY(false);
    }

    if( IntPutData.find("InputImage") != IntPutData.end() &&
            IntPutData.find("Width") != IntPutData.end()  &&
            IntPutData.find("Height") != IntPutData.end() )
    {//设置图片来源soc
        ImageDevice->setMirrorMode (1);
        ImageDevice->setMirrorImageWidth (IntPutData["Width"]);
        ImageDevice->setMirrorImageHeigth (IntPutData["Height"]);
        ImageDevice->setMirrorInputImage ((void*)IntPutData["InputImage"], IntPutData["Width"]*IntPutData["Height"] );
    }else{//设置图片来源sensor
        ImageDevice->setMirrorMode (0);
    }
    ImageDevice->setMirrorOutputPhyaddr (0x1F400000);
    if(ImageDevice->waitMirrorModuleFinished ())
       ErrNumber = MODULE_SUCCESS;
    else
        ErrNumber = MODULE_BLOCK_TERMINATE;

    void* OutPutImage = ImageDevice->getMirrorOutputVirturaladdr(0x1F400000);
    OutPutData.insert (std::pair<std::string, long>("OutputImage",   (long)OutPutImage ));
    return ErrNumber;
}


Herror XiDriver::Execute()
{
    ModueMutex.lock ();
    ErrNumber =  MODULE_SUCCESS;
    if(ModuleName == "BlockModule")
    {
        ErrNumber = ExecuteBlock ();
    }else if(ModuleName == "MirrorModule"){
        ErrNumber = ExecuteMirror ();
    }else{
        LOG(INFO_LEVEL, "没有发现可执行的模块");
        ErrNumber =  MODULE_NOFIND_MODULE;
    }
    ModueMutex.unlock ();
    return ErrNumber;
}

Herror XiDriver::SetInputParam(const char* name, long      value)
{
    if(name == NULL)
    {
        return ErrNumber = MODULE_NAME_IS_NULL;
    }
    std::string sname = name;
    if(ModuleName == "BlockModule")
    {
        if(sname == "ThreshValue")
            IntPutData.insert (std::pair<std::string, long>("ThreshValue",  value));
        else if(sname == "Width")
            IntPutData.insert (std::pair<std::string, long>("Width",  value));
        else if(sname == "Height")
            IntPutData.insert (std::pair<std::string, long>("Height",  value));
        else
            return ErrNumber = MODULE_NOFIND_PARAM;
        return ErrNumber = MODULE_SUCCESS;
    }else if( ModuleName == "MirrorModule"){
        if(sname == "Width")
            IntPutData.insert (std::pair<std::string, long>("Width",  value));
        else if(sname == "Height")
            IntPutData.insert (std::pair<std::string, long>("Height",  value));
        else if(sname == "MirrorX")
            IntPutData.insert (std::pair<std::string, long>("MirrorX",  value));
        else if(sname == "MirrorY")
            IntPutData.insert (std::pair<std::string, long>("MirrorY",  value));
        else
            return ErrNumber = MODULE_NOFIND_PARAM;
        return ErrNumber = MODULE_SUCCESS;
    }else{
        return ErrNumber = MODULE_NOFIND_MODULE;
    }
}


Herror XiDriver::SetInputObject(const char* name, void* pdata)
{
    if(name == NULL)
        return ErrNumber = MODULE_NAME_IS_NULL;
    std::string sname = name;

    if(ModuleName == "BlockModule")
    {
        if(sname == "InputImage")
            IntPutData.insert (std::pair<std::string, long>("InputImage",  (long)pdata));
        else
            return ErrNumber = MODULE_NOFIND_OBJECT;
        return ErrNumber = MODULE_SUCCESS;
    }else if(ModuleName == "MirrorModule"){
        if(sname == "InputImage")
            IntPutData.insert (std::pair<std::string, long>("InputImage",  (long)pdata));
        else
            return ErrNumber = MODULE_NOFIND_OBJECT;
        return ErrNumber = MODULE_SUCCESS;
    }else{
        return ErrNumber = MODULE_NOFIND_MODULE;
    }
}


Herror XiDriver::GetOutputParam(const char* name, void* pdata)
{
    if(name == NULL)
        return ErrNumber = MODULE_NAME_IS_NULL;
    std::string sname = name;
    if(ModuleName == "BlockModule")
    {
        if(sname == "HrunCount")
        {
            *((int*)pdata) = (int)OutPutData["HrunCount"];
            return ErrNumber = MODULE_SUCCESS;
        }
        return ErrNumber = MODULE_NOFIND_PARAM;
    }else{
        return ErrNumber = MODULE_NOFIND_MODULE;
    }
}

Herror XiDriver::GetOutputObject(const char* name, void** pdata)
{
    if(name == NULL)
        return ErrNumber = MODULE_NAME_IS_NULL;
    std::string sname = name;

    if(ModuleName == "BlockModule")
    {
        if(sname == "HrunOutPutBuff"){
            *pdata =(void*)OutPutData["HrunOutPutBuff"];
            return ErrNumber = MODULE_SUCCESS;
        }
        return ErrNumber = MODULE_NOFIND_PARAM;
    }else if( ModuleName == "MirrorModule"){
        if(sname == "OutPutImage"){
            *pdata =(void*)OutPutData["OutPutImage"];
            return ErrNumber = MODULE_SUCCESS;
        }
        return ErrNumber = MODULE_NOFIND_PARAM;
    }else{
        return ErrNumber = MODULE_NOFIND_MODULE;
    }
}


Herror XiDriver::ExecuteProcedure(const char* name)
{
    if(name == NULL)
        return ErrNumber = MODULE_NAME_IS_NULL;

    int i;
    ModuleName="";

    OutPutData.clear();
    IntPutData.clear ();

    for(i = 0 ; i < sizeof(Fpga_Modules) /FPGAMODULEMAXNAME;i++)
    {
        if(!strcmp(name, Fpga_Modules[i]))
        {
            ModuleName = name;
            return ErrNumber = MODULE_SUCCESS;
        }else{
            continue;
        }
    }
    LOG(INFO_LEVEL,"没有(%s)模块", name );
    return ErrNumber = MODULE_NOFIND_MODULE;
}





}
