class XiDriver  Driver

/*Block Module
@ExecuteProcedure       BlockModule             -----模块名称
@SetInputParam             ThreshValue              -----设置模块阀值
@GetOutputParam         HrunCount                 ----Hrun 个数
@GetOutputParam         HrunOutPutBuff        ----获取Hrun 结构体地址

@SetInputParam             Width                         -----图片宽度
@SetInputParam             Height                        -----图片高度
@SetInputObject             InputImage               -----图片输入
*/
//设置当工作模块
driver->ExecuteProcedure("BlockModule");
//设置模块阀值
driver->SetInputParam("ThreshValue", 180);
//执行模块，接收到外触发后返回
driver->Execute();
//获取Hrun 个数
int Count;
driver->GetOutputParam("HrunCount", (void*)&Count);
//获取Hrun 结构体地址
Hrun *Pointer;
driver->GetOutputObject("HrunOutPutBuff", (void**)&Pointer);



/*Mirror Module
//设置输入图片 镜像
@ExecuteProcedure    MirrorModule         -----模块名称
@SetInputParam          MirrorY                   ------0不使能，非0使能
@SetInputParam          MirrorX                   ------0不使能，非0使能
@GetOutputObject      OutputImage         ------图片输出地址
@SetInputParam          Width                      ------图片宽度，图片来源应用层时需要设置，来源于sensor 忽略
@SetInputParam          Height                     ------图片高度，图片来源应用层时需要设置，来源于sensor 忽略
@SetInputObject          InputImage            ------图片输入，图片来源应用层时需要设置，来源于sensor 忽略
*/

char* inputbuff = new char[1024*1280];
char* buffImage;
driver->ExecuteProcedure ("MirrorModule");
driver->SetInputParam ("Width", 1280);
driver->SetInputParam ("Height", 1024);
driver->SetInputObject ("InputImage", inputbuff);
driver->SetInputParam("MirrorY", 1);
driver->Execute();
driver->GetOutputObject("OutputImage", (void**)&buffImage);
//设置来源sensor 镜像
driver->ExecuteProcedure ("MirrorModule");
driver->SetInputParam("MirrorX", 1);
driver->Execute();
driver->GetOutputObject("OutputImage", (void**)&buffImage);








