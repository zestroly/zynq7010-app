#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <stdlib.h>
#include <regex>

#include <stdlib.h>

#include "libdate.h"

using namespace MxcDriver;

#include <signal.h>
void recvsignal(int s)
{
    cout<<"Caught signal:"<<s<<endl;
    exit(1);
}


void help(int argc , char* argv[])
{
    cout<<"usage: "<<endl;
	cout<<"       "<<argv[0]<<" [-ptTl] \"addr/datetime/timezone\" -[crs]\n";

	cout<<"       "<<argv[0]<<" -p addr -[crs]\n";
    cout<<"       "<<"\t\t\t -c \t , Get Ntp Server DateTime\n";
    cout<<"       "<<"\t\t\t -r \t , Set DateTime to RTC from Ntp Server\n";
    cout<<"       "<<"\t\t\t -s \t , Set DateTime to System from Ntp Server\n";
	cout<<"       "<<"\t\texample get Ntp time: "<<argv[0]<<" -p 52.169.179.91 -c\n";
    cout<<endl;
	cout<<"       "<<argv[0]<<" -l datetime -[rs]\n";
    cout<<"       "<<"\t\t\t -r \t , Set RTC DateTime\n";
    cout<<"       "<<"\t\t\t -s \t , Set System DateTime\n";
	cout<<"       "<<"\t\texample Set Rtc: "<<argv[0]<<" -l \"2015/08/22 09:09:09\" -r\n\n";

	cout<<"       "<<argv[0]<<" -T \"timezone\"\t, set timezone\n";
	cout<<"       "<<"\t\texample Set Rtc: "<<argv[0]<<" -T \"Asia/Shanghai\" -r\n\n";
	cout<<"       "<<argv[0]<<" -l  \t\t, list all timezones\n";

}


void inputaddr(string &addr)
{
    cout<<"please input Ntp server addr (example: 52.169.179.91)"<<endl;
    cout<<"new server addr:";
    cin>>addr;
    const std::regex pattern("^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$");
    while(!std::regex_match(addr, pattern))
    {
        cout<<"Addr format error:"<<addr<<endl;
        cout<<"please re input:";
        cin>>addr;
    }
}

#include <curses.h>
#include <termios.h>
#include <term.h>

void intputstr()
{
    char ch;
    struct termios old_term;
    struct termios cur_term;
    int ret ;
    tcgetattr(STDIN_FILENO, &old_term);
    memcpy(&cur_term, &old_term, sizeof(cur_term));
    cur_term.c_lflag &= ~(ICANON);
    cur_term.c_cc[VMIN] = 1;
    cur_term.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &cur_term);

    char buf[1024]={0};
    int i = 0;
    while(1){
        ret = read(STDIN_FILENO, &ch, 1);
        if (ret <= 0)
        {

        }
        else
        {
            buf[i] = ch;

            if(ch == '\n')
            {
                break;
            } else if (ch == '\b')
            {
                i--;
            }else{
                i++;
            }
        }
        buf[i] = '\0';
        printf("\r                                   ");
        fflush(stdout);
        printf("\r%s",buf);
        fflush(stdout);
    }

    cout<<buf<<endl;
    tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
    return;
}

int main(int argc, char * argv[])
{
   // intputstr();

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = recvsignal;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    MxcDate  w;




//    w.initLocalZone();

    MxcDate::UDate udate;
    std::cout<<(w.getNtpDate("115.28.122.198", udate) ? w.strTime(udate):"fail!")<<endl;
    return 0;
    bool   timevalue=false;

    bool   addrvalue=false;
    string remoteaddr;

    const std::regex pattern("^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$");
    const std::regex patterntime("^(((20[0-3][0-9]/(0[13578]|1[02])/(0[1-9]|[12][0-9]|3[01]))|(20[0-3][0-9]-(0[2469]|11)-(0[1-9]|[12][0-9]|30))) (20|21|22|23|[0-1][0-9]):[0-5][0-9]:[0-5][0-9])$");

    int aflag=0, bflag=0, cflag=0;
    int ch;

    cout<<"System Time:\t"<<\
        (w.getSysDate(udate) ? w.strTime(udate):"fail!")<<endl;
    cout<<"RTC Time:\t"<<\
        (w.getRtcDate(udate) ? w.strTime(udate):"fail!")<<endl;
    string tz;
    w.getTimeZone(tz);
    cout<<"TimeZone is:\t"<<(tz.empty()?"null":tz)<<endl;

    while ((ch = getopt(argc, argv, "p:T:rschtl")) != -1)
    {
        switch (ch) {
            case 'l':
                {
                    vector<MxcDate::ZoneSrc> zones;
                    vector<MxcDate::ZoneSrc>::iterator zoneIt;
                    w.getZoneInfo(zones);
                    for(zoneIt = zones.begin(); zoneIt != zones.end(); zoneIt++)
                    {
                        cout<<zoneIt->s_name<<"\t\t|" << zoneIt->s_time<<"\t\t|"<< zoneIt->s_offset<<endl;
                    }
                    zones.clear();
                }
                break;
            case 'T':
                {
                    vector<MxcDate::ZoneSrc> zones;
                    vector<MxcDate::ZoneSrc>::iterator zoneIt;
                    w.getZoneInfo(zones);
                    bool isset=false;
                    for(zoneIt = zones.begin(); zoneIt != zones.end(); zoneIt++)
                    {
                        string current=optarg;

                        if(zoneIt->s_name == current)
                        {
                            cout<<"set timezone:"<<(w.setTimeZone(current)?"ok":"fail")<<endl;
                            isset = true;
                            break;
                        }
                    }
                    if (!isset)
                        cout<<"set timezone:no find this item("<< optarg <<")!"<<endl;
                }
                break;
            case 'p':
                if(std::regex_match(optarg, pattern))
                {
                    remoteaddr=optarg;
                    addrvalue=true;
                }else{
                    cout<<"the addr format error, example -p 52.169.179.91"<<endl;
                    return 1;
                }
                break;
            case 't':
                if(std::regex_match(optarg, patterntime))
                {
                    sscanf(optarg,"%d/%d/%d %d:%d:%d",&udate.year, &udate.mon, &udate.mday
                            ,&udate.hour , &udate.min, &udate.sec);
                    timevalue=true;
                }else{
                    cout<<"the time format error!\n";
                }
                break;
            case 'c':
                if(addrvalue == true)
                    if(std::regex_match(remoteaddr, pattern))
                        cout<<"Get Time from Ntp Server( "<<remoteaddr<<" ): "<<\
                            (w.getNtpDate(remoteaddr, udate) ? w.strTime(udate):"fail!")<<endl;
                break;
            case 'r':
                if(addrvalue == true)
                    if(std::regex_match(remoteaddr, pattern))
                        cout<<"Set Time from Ntp Server To RTC( "<<remoteaddr<<" ): "<<\
                            (w.NtpToHc(remoteaddr)?"ok!":"fail!")<<endl;
                if(timevalue == true)
                    w.setRtcDate(udate);
                break;
            case 's':
                if(addrvalue == true)
                    if(std::regex_match(remoteaddr, pattern))
                        cout<<"Set Time from Ntp Server To System( "<<remoteaddr<<" ): "<<\
                            (w.NtpToSys(remoteaddr)?"ok!":"fail!")<<endl;
                if(timevalue == true)
                    w.setSysDate(udate);
                break;
            case 'h':
                help(argc , argv);
                break;
            case '?':
                printf("Unknown option: %c\n",(char)optopt);
                break;
        }
    }
}
