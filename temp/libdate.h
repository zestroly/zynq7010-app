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
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>

#include <time.h>

#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <err.h>
#include <string>
#include <iostream>

#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <vector>
#include <map>
#include <fstream>
using namespace std;

#include <map>
#include <list>


namespace MxcDriver{


#define NVR_NTP_PORT_STR        "123"
#define NVR_NTP_PCK_LEN				48
#define NVR_NTP_LI 					0
#define NVR_NTP_VN 					3
#define NVR_NTP_MODE 				3
#define NVR_NTP_STRATUM 			0
#define NVR_NTP_POLL 				4
#define NVR_NTP_PREC 				-6

#define NVR_NTP_JAN_1970 	  	0x83aa7e80
#define NVR_NTPFRAC(x)			(4294 * (x) + ((1981 * (x)) >> 11))
#define NVR_NTPUSEC(x)			(((x) >> 12) - 759 * ((((x) >> 10) + 32768) >> 16))

typedef struct _NVR_NTP_TIME
{
	unsigned int coarse;
	unsigned int fine;
} NVR_NTP_TIME;

typedef struct _NVR_NTP_PACKET
{
	unsigned char leap_ver_mode;

	unsigned char startum;
	char poll;
	char precision;

	int root_delay;
	int root_dispersion;
	int reference_identifier;

	NVR_NTP_TIME reference_timestamp;
	NVR_NTP_TIME originage_timestamp;
	NVR_NTP_TIME receive_timestamp;
	NVR_NTP_TIME transmit_timestamp;

}NVR_NTP_PACKET;

typedef struct _NVR_NTP_PACKET_STR
{
	NVR_NTP_PACKET ntpPacket;
	char ntpPacketBuf[NVR_NTP_PCK_LEN];
}NVR_NTP_PACKET_STR;



class MxcDate{
	public:
		typedef struct tm      TDate;
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


		bool initLocalZone();  //init current process timezone with  /etc/timezone value
		bool getZoneInfo(vector<ZoneSrc> &infos); // get world zone info
		string strTime(UDate &ud);



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

};



}
#endif //__LIBDATE_H
