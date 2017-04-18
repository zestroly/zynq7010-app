#ifndef __LIBDATE_H
#define __LIBDATE_H

#include <sys/socket.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/un.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>

#include <time.h>

#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <err.h>

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <list>

using namespace std;

namespace MxcDriver{

class MxcDate{
	public:
		typedef struct tm TDate;
		typedef struct _UDate{
			int sec;
			int min;
			int hour;
			int mday;
			int mon;
			int year;
		}UDate;

		typedef struct _TimeSrc{
			string  s_name;     //zone name  and will set value  of "/etc/timezone" file
			string  s_time;     //local time  eg: 2016/02/03 05:05:02
			int		s_offset;   //D-value which UTC0
		}ZoneSrc;

		bool getNtpDate(string remote, UDate &ud);  // sync ntp time  and get local time
		bool NtpToSys(string remote);  //Get ntp time and set system time local time
		bool NtpToHc (string remote);  //Get ntp time and  save utc time  to RTC

		bool getSysDate(UDate &ud);       // get system time with local time
		bool setSysDate(const UDate &ud); // set system time with local time

		bool getRtcDate(UDate &ud);		  // get rtc time  with local time
		bool setRtcDate(const UDate &ud); // set rtc time  with local time

		bool setTimeZone(string &timezone);		//set value to /etc/timezone  eg: Asia/Shanghai    Europe/Rome
		bool getTimeZone(string &timezone);     //get /etc/timezone value;
		bool setEnvTimeZone(string &timezone);  // set env TZ value  eg:TZ=Asia/Shanghai
		bool getEnvTimeZone(string &timezone);  // get env TZ value;

		bool getZoneInfo(vector<ZoneSrc> &infos); // get world zone info

	private:
		bool getZoneNames(vector<string>& names);
		int  tree(char* dir, vector<string> &infos);
		bool getAllZone(map<string, ZoneSrc> &zonemsg); //get all zonemsg
		bool getAllZone(map<string, string> &mmap);
		bool getTimeInfo(map<string, string> &mmp);
		void disPlayUDate(const UDate &ud);
		bool writeRTC(struct tm &tm);
		bool readRTC (struct tm &tm);
		void TDateToUDate(const TDate &td, UDate &ud);
		void UDateToTDate(const UDate &ud, TDate &td);
    public:
        MxcDate();
        ~MxcDate();
		bool initLocalZone();  //init current process timezone with  /etc/timezone value
		string strTime(UDate &ud);

};


}
#endif //__LIBDATE_H
