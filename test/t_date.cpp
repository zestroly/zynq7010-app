#include "libdate.h"

struct NtpServerAddr{
    string addr;
    string name;
} NtpServerList[]{
    {"cn.ntp.org.cn", "中国"},
    {"us.ntp.org.cn", "美国"},
    {"sgp.ntp.org.cn","新加坡"},
    {"kr.ntp.org.cn", "韩国"},
    {"ntp.sjtu.edu.cn", "上海交通大学"},
    {"s1a.time.edu.cn", "北京邮电大学"},
    {"time.windows.com", "微软服务器"},
};



int main(int argc, char** argv)
{
    MxcDriver::MxcDate test;
    MxcDriver::MxcDate::UDate ud;

    std::cout<<"check systime time:  "<< (test.getSysDate(ud)?test.strTime(ud):"fail!") <<std::endl;
    std::cout<<"check rtc time:  "<< (test.getRtcDate(ud)?test.strTime(ud):"fail!") <<std::endl;

    ud.year =1988;
    ud.mon = 9;
    ud.mday=20;
    ud.hour=22;
    ud.min =0;
    ud.sec =0;
    std::cout<<"will set time.... :"<<test.strTime(ud) <<std::endl;
    test.setSysDate(ud);
    test.setRtcDate(ud);
    std::cout<<"after set, systime time:  "<< (test.getSysDate(ud)?test.strTime(ud):"fail!") <<std::endl;
    std::cout<<"after set, rtc time:  "<< (test.getRtcDate(ud)?test.strTime(ud):"fail!") <<std::endl;

    int i;
    for(i = 0; i < (sizeof(NtpServerList)/sizeof(struct NtpServerAddr)) ; i++)
    {
        std::cout<<NtpServerList[i].name<< NtpServerList[i].addr<<"服务器,"<<flush<<" 获取时间 :  ";
        if(test.getNtpDate(NtpServerList[i].addr.c_str(), ud))
        {
            std::cout<<test.strTime(ud);
            test.NtpToSys(NtpServerList[i].addr.c_str());
            test.NtpToHc(NtpServerList[i].addr.c_str());
            break;
        }else{
            std::cout<<"fail!";
        }
        std::cout << std::endl;
    }

    std::cout<<"after ntp, systime time:  "<< (test.getSysDate(ud)?test.strTime(ud):"fail!") <<std::endl;
    std::cout<<"after ntp, rtc time:  "<< (test.getRtcDate(ud)?test.strTime(ud):"fail!") <<std::endl;

    string TZ;
    test.getTimeZone(TZ);
    std::cout<<"current TZ is:"<<TZ<<std::endl;

    test.setEnvTimeZone(TZ);
    std::cout<<"after TZ, systime time:  "<< (test.getSysDate(ud)?test.strTime(ud):"fail!") <<std::endl;
    std::cout<<"after TZ, time:  "<< (test.getRtcDate(ud)?test.strTime(ud):"fail!") <<std::endl;

    return 0;
}





