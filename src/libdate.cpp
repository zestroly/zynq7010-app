#include "libdate.h"


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

#define DEV_RTC "/dev/rtc0"

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


static NVR_NTP_PACKET_STR g_ntpPacketStr;

static void nvr_ntp_packet_init(char *ntpPacketBuf)
{
	long tmp_wrd;
	time_t timer;

	memset(ntpPacketBuf, 0, NVR_NTP_PCK_LEN);

	tmp_wrd = htonl((NVR_NTP_LI << 30)|(NVR_NTP_VN << 27)|(NVR_NTP_MODE << 24)|(NVR_NTP_STRATUM << 16)|(NVR_NTP_POLL << 8)|(NVR_NTP_PREC & 0xff));
	memcpy(ntpPacketBuf, &tmp_wrd, sizeof(tmp_wrd));

	tmp_wrd = htonl(1<<16);
	memcpy(&ntpPacketBuf[4], &tmp_wrd, sizeof(tmp_wrd));
	memcpy(&ntpPacketBuf[8], &tmp_wrd, sizeof(tmp_wrd));

	time(&timer);

	tmp_wrd = htonl(NVR_NTP_JAN_1970 + (long)timer);
	memcpy(&ntpPacketBuf[40], &tmp_wrd, sizeof(tmp_wrd));


	tmp_wrd = htonl((long)NVR_NTPFRAC(timer));
	memcpy(&ntpPacketBuf[44], &tmp_wrd, sizeof(tmp_wrd));
}

static int nvr_get_ntp_time(char *nvr_ntp_server)
{
	int sockfd, recv_size = 0;
	struct addrinfo hints, *res = NULL;

	fd_set readfds;
	struct timeval time_out;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	if(0 != getaddrinfo(nvr_ntp_server, NVR_NTP_PORT_STR, &hints, &res))
	{
		perror("struct getaddrinfo");
		return 0;
	}

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd < 0 )
	{
		perror("create socket");
		return 0;
	}

	nvr_ntp_packet_init(g_ntpPacketStr.ntpPacketBuf);
	if (sizeof(g_ntpPacketStr.ntpPacketBuf) != NVR_NTP_PCK_LEN)
	{
		perror("NTP packet length");
		return 0;
	}

	if (sendto(sockfd, g_ntpPacketStr.ntpPacketBuf, sizeof(g_ntpPacketStr.ntpPacketBuf), 0, res->ai_addr, res->ai_addrlen) < 0)
	{
		perror("sendto NTP packet");
		return 0;
	}

	FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);
	time_out.tv_sec=10;
	time_out.tv_usec=0;
	if (select(sockfd + 1, &readfds, NULL, NULL, &time_out) > 0)
	{
		if ((recv_size = recvfrom(sockfd, g_ntpPacketStr.ntpPacketBuf,sizeof(g_ntpPacketStr.ntpPacketBuf), 0, res->ai_addr, &res->ai_addrlen)) < 0)
		{
			perror("recvfrom NTP packet");
			return 0;
		}

		if (recv_size < NVR_NTP_PCK_LEN)
		{
			perror("recvfrom NTP packet length");
			return 0;
		}
		/* 设置接收NTP包的数据结构 */
		g_ntpPacketStr.ntpPacket.leap_ver_mode 				= ntohl(g_ntpPacketStr.ntpPacketBuf[0]);
		g_ntpPacketStr.ntpPacket.startum 					= ntohl(g_ntpPacketStr.ntpPacketBuf[1]);
		g_ntpPacketStr.ntpPacket.poll 						= ntohl(g_ntpPacketStr.ntpPacketBuf[2]);
		g_ntpPacketStr.ntpPacket.precision 					= ntohl(g_ntpPacketStr.ntpPacketBuf[3]);
		g_ntpPacketStr.ntpPacket.root_delay 				= ntohl(*(int*)&(g_ntpPacketStr.ntpPacketBuf[4]));
		g_ntpPacketStr.ntpPacket.root_dispersion 			= ntohl(*(int*)&(g_ntpPacketStr.ntpPacketBuf[8]));
		g_ntpPacketStr.ntpPacket.reference_identifier 		= ntohl(*(int*)&(g_ntpPacketStr.ntpPacketBuf[12]));
		g_ntpPacketStr.ntpPacket.reference_timestamp.coarse = ntohl(*(int*)&(g_ntpPacketStr.ntpPacketBuf[16]));
		g_ntpPacketStr.ntpPacket.reference_timestamp.fine 	= ntohl(*(int*)&(g_ntpPacketStr.ntpPacketBuf[20]));
		g_ntpPacketStr.ntpPacket.originage_timestamp.coarse = ntohl(*(int*)&(g_ntpPacketStr.ntpPacketBuf[24]));
		g_ntpPacketStr.ntpPacket.originage_timestamp.fine 	= ntohl(*(int*)&(g_ntpPacketStr.ntpPacketBuf[28]));
		g_ntpPacketStr.ntpPacket.receive_timestamp.coarse 	= ntohl(*(int*)&(g_ntpPacketStr.ntpPacketBuf[32]));
		g_ntpPacketStr.ntpPacket.receive_timestamp.fine 	= ntohl(*(int*)&(g_ntpPacketStr.ntpPacketBuf[36]));
		g_ntpPacketStr.ntpPacket.transmit_timestamp.coarse 	= ntohl(*(int*)&(g_ntpPacketStr.ntpPacketBuf[40]));
		g_ntpPacketStr.ntpPacket.transmit_timestamp.fine 	= ntohl(*(int*)&(g_ntpPacketStr.ntpPacketBuf[44]));

		return 1;
	} /* end of if select */
	close(sockfd);
	return 0;
}


static bool getNtpday(string remote, struct timeval &tv)
{
	char *ip =const_cast<char* >(remote.c_str());
	if(!nvr_get_ntp_time(ip))
		return false;

	tv.tv_sec  = g_ntpPacketStr.ntpPacket.transmit_timestamp.coarse - NVR_NTP_JAN_1970;
	tv.tv_usec = NVR_NTPUSEC(g_ntpPacketStr.ntpPacket.transmit_timestamp.fine);
	return true;
}

void MxcDate::TDateToUDate(const TDate &td, UDate &ud)
{
	memset(&ud, 0 , sizeof(ud));
	ud.sec	= td.tm_sec;
	ud.min	= td.tm_min;
	ud.hour	= td.tm_hour;
	ud.mday	= td.tm_mday;
	ud.mon	= td.tm_mon  + 1;
	ud.year	= td.tm_year + 1900;
}

void MxcDate::UDateToTDate(const UDate &ud, TDate &td)
{
	memset(&td, 0 , sizeof(td));
	td.tm_sec = ud.sec;
	td.tm_min = ud.min;
	td.tm_hour= ud.hour;
	td.tm_mday= ud.mday;
	td.tm_mon = ud.mon  - 1;
	td.tm_year= ud.year - 1900;
}



bool MxcDate::getNtpDate(string remote, UDate &ud)
{
	struct timeval tv;
	if(!getNtpday(remote, tv))
		return false;

	struct tm tm;
	localtime_r(&tv.tv_sec, &tm);
	TDateToUDate(tm, ud);
	return true;
}

bool MxcDate::NtpToSys(string remote)
{
	struct timeval tv;
	if(!getNtpday(remote, tv))
		return false;

	settimeofday(&tv, NULL);
	return true;
}

bool MxcDate::NtpToHc(string remote)
{
	struct timeval tv;
	if(!getNtpday(remote, tv))
		return false;

	struct tm tm;
	memset(&tm, 0 , sizeof(tm));
	gmtime_r(&tv.tv_sec, &tm);

	if(!writeRTC(tm))
		return false;
	return true;
}


bool MxcDate::getSysDate(UDate &ud)
{
	struct timeval tv;
	TDate td;
	gettimeofday(&tv, NULL);

	localtime_r(&tv.tv_sec, &td);
	TDateToUDate(td, ud);
	return true;
}

bool MxcDate::setSysDate(const UDate &ud)
{
	struct timeval tv;
	memset(&tv, 0, sizeof(tv));
	TDate td;
	UDateToTDate(ud, td);

	tv.tv_sec = mktime(&td);

	settimeofday(&tv, NULL);
	return true;
}



bool MxcDate::readRTC(struct tm &tm)
{
	int retval;
	int fd = open(DEV_RTC, O_RDONLY);
	if(fd < 0)
		return false;

	retval = ioctl(fd, RTC_RD_TIME, &tm);
	if(retval == -1)
	{
		close(fd);
		return false;
	}

	close(fd);
	return true;

}


bool MxcDate::getRtcDate(UDate &ud)
{

	struct tm utm;
	memset(&utm, 0, sizeof(utm));
	struct tm ltm;
	memset(&ltm, 0, sizeof(ltm));

	if(!readRTC(utm))
		return false;

	char *oldtz = NULL;

	oldtz = getenv("TZ");
	putenv((char*)"TZ=UTC0");
	tzset();
	time_t utimesec = mktime(&utm);
	unsetenv("TZ");
	putenv(oldtz - 3);
	tzset();

	localtime_r(&utimesec, &ltm);

	memset(&ud, 0, sizeof(ud));
	ud.sec	= ltm.tm_sec;
	ud.min	= ltm.tm_min;
	ud.hour	= ltm.tm_hour;
	ud.mday	= ltm.tm_mday;
	ud.mon	= ltm.tm_mon  + 1;
	ud.year	= ltm.tm_year + 1900;
	return true;
}

bool MxcDate::writeRTC(struct tm &tm)
{
	int retval;
	int fd = open(DEV_RTC, O_WRONLY);
	if(fd < 0)
		return false;

	retval = ioctl(fd, RTC_SET_TIME, &tm);
	if(retval == -1)
	{
		close(fd);
		return false;
	}

	close(fd);
	return true;
}

bool MxcDate::setRtcDate(const UDate &ud)
{
	struct tm ltm;
	struct tm gtm;
	struct rtc_time rtc_tm;
	memset(&ltm, 0 , sizeof(ltm));
	memset(&gtm, 0 , sizeof(gtm));
	memset(&rtc_tm, 0 , sizeof(rtc_tm));

	ltm.tm_sec  = ud.sec ;
	ltm.tm_min  = ud.min ;
	ltm.tm_hour = ud.hour;
	ltm.tm_mday = ud.mday;
	ltm.tm_mon  = ud.mon - 1;
	ltm.tm_year = ud.year - 1900;

	time_t  timesec = mktime(&ltm);

	gmtime_r(&timesec, &gtm);

	rtc_tm.tm_sec  = gtm.tm_sec ;
	rtc_tm.tm_min  = gtm.tm_min ;
	rtc_tm.tm_hour = gtm.tm_hour;
	rtc_tm.tm_mday = gtm.tm_mday;
	rtc_tm.tm_mon  = gtm.tm_mon ;
	rtc_tm.tm_year = gtm.tm_year;
	return writeRTC(gtm);
}

void MxcDate::disPlayUDate(const UDate &ud)
{
	std::cout<<ud.year<<"/"\
		<<ud.mon<<"/"\
		<<ud.mday<<" "\
		<<ud.hour<<":"\
		<<ud.min<<":"\
		<<ud.sec<<endl;
}

string MxcDate::strTime(UDate &ud)
{
	char  buf[100] = {0};
	sprintf(buf, "%04d/%02d/%02d %02d:%02d:%02d", ud.year, ud.mon, ud.mday, ud.hour, ud.min, ud.sec);
	return string(buf);
}


int MxcDate::tree (char* dir, vector<string> &infos )
{
	DIR* dp = opendir (dir);
	if (! dp) { perror("opendir"); return -1; }
	if (chdir (dir) == -1) { perror("chdir"); return -1; }

	errno = 0;
	struct dirent* de;

	char  currentpath[256];
	bzero(currentpath,sizeof(currentpath));
	getcwd(currentpath, sizeof(currentpath));

	struct stat buf;

	for (de=readdir(dp); de; de=readdir(dp))
	{
		if ( strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0 )
			continue;
		stat(de->d_name, &buf);
		if (S_ISDIR(buf.st_mode))
		{
			if (tree(de->d_name, infos) == -1)
				return -1;
			continue;
		}
		if (S_ISREG(buf.st_mode))
		{
			string sinfo = currentpath;
			sinfo += "/";
			sinfo += de->d_name;
			infos.push_back(sinfo);
		}
	}

	if (chdir ("..") == -1) {
		perror ("chdir");
		return -1;
	}
	closedir (dp);
	return 0;
}


static char AreaTime[][20] = {
	"Africa", "America", "Antarctica", "Asia", "Australia", "Atlantic", "Europe","Indian","Pacific",
};



bool MxcDate::getZoneNames(vector<string>& names)
{
	char* pparent = (char*)"/usr/share/zoneinfo";

	unsigned int i;
	for(i = 0 ; i < (sizeof(AreaTime)/20); i++)
	{
		string ww = pparent;
		ww += "/";
		ww +=AreaTime[i];
		tree((char*)ww.c_str(),  names);;
	}
	return true;
}

bool MxcDate::getZoneInfo(vector<ZoneSrc> &infos)
{
	vector<string> names;
	vector<string>::iterator it;

	string zonename;
	getZoneNames(names);

	char *p = getenv("TZ");
	string oldTZ = p;

	for(it = names.begin(); it != names.end(); it++)
	{
	    zonename = (*it).substr(strlen("/usr/share/zoneinfo/"));

		setenv("TZ", it->c_str(), 1);
		tzset();

		struct tm ltm;
		struct tm utm;
		time_t utimesec = time(0);
		localtime_r(&utimesec, &ltm);
		gmtime_r(&utimesec, &utm);

		time_t ltimesec = mktime(&utm);
		char tmpb[40] = {0};
		sprintf(tmpb, "%04d/%02d/%02d %02d:%02d:%02d", ltm.tm_year+1900, ltm.tm_mon+1, ltm.tm_mday, ltm.tm_hour, ltm.tm_min, ltm.tm_sec);

	   ZoneSrc cell;
	   cell.s_name = zonename;
	   cell.s_time = tmpb;
	   cell.s_offset = ((utimesec - ltimesec)/3600) ;
	   infos.push_back(cell);
	}

	setenv("TZ", oldTZ.c_str(), 1);
	tzset();
	return true;
}




bool MxcDate::getTimeInfo(map<string, string> &mmp)
{
	unsigned int i;
	vector<string> infos;
	vector<string>::iterator it;

	char* pparent = (char*)"/usr/share/zoneinfo";

	for(i = 0 ; i < (sizeof(AreaTime)/20); i++)
	{
		string ww = pparent;
		ww += "/";
		ww +=AreaTime[i];
		tree((char*)ww.c_str(),  infos);;
		cout<<endl;
	}

	struct stat stf;
	for(it=infos.begin() ;it!=infos.end() ;it++)
	{
		lstat(it->c_str(), &stf);
		if( S_ISLNK(stf.st_mode ))
		{
			char buff[1024] = {0};
			readlink(it->c_str(), buff, sizeof(buff));

			string pathfront = *it;
			string pathappent = buff;

			if(pathappent.at(0) == '.' && pathappent.at(1) == '.' && pathappent.at(2) == '/')
			{
				pathfront = pathfront.substr(0, pathfront.rfind("/"));
				pathfront = pathfront.substr(0, pathfront.rfind("/")+1);
				pathappent = pathappent.substr(3);
			} else {
				pathfront = pathfront.substr(0, pathfront.rfind("/")+1);
			}

			string othername = *it;
			othername = othername.substr(strlen("/usr/share/zoneinfo/"));

			string fullpath = pathfront+pathappent;

			fullpath = fullpath.substr(strlen("/usr/share/zoneinfo/"));

			mmp.insert(pair<string,string> (othername, fullpath ));


		}
        else if(S_ISREG(stf.st_mode))
		{
			string fullpath = *it;

			string othername = *it;
			othername = othername.substr(strlen("/usr/share/zoneinfo/"));

			fullpath = fullpath.substr(strlen("/usr/share/zoneinfo/"));

			mmp.insert(pair<string,string> (othername, fullpath ));
		}
	}
	infos.clear();
	return true;
}

bool MxcDate::getAllZone(map<string, ZoneSrc> &zonemsg)
{
	unsigned int i;
	vector<string> infos;
	vector<string>::iterator it;

	char* pparent = (char*)"/usr/share/zoneinfo";

	for(i = 0 ; i < (sizeof(AreaTime)/20); i++)
	{
		string ww = pparent;
		ww += "/";
		ww +=AreaTime[i];
		tree((char*)ww.c_str(),  infos);;
		cout<<endl;
	}


	ZoneSrc zonecell;

	struct stat stf;
	for(it=infos.begin() ;it!=infos.end() ;it++)
	{
		lstat(it->c_str(), &stf);
		if( S_ISLNK(stf.st_mode ))
		{
			char buff[1024] = {0};
			readlink(it->c_str(), buff, sizeof(buff));

			string pathfront = *it;
			string pathappent = buff;

			if(pathappent.at(0) == '.' && pathappent.at(1) == '.' && pathappent.at(2) == '/')
			{
				pathfront = pathfront.substr(0, pathfront.rfind("/"));
				pathfront = pathfront.substr(0, pathfront.rfind("/")+1);
				pathappent = pathappent.substr(3);
			} else {
				pathfront = pathfront.substr(0, pathfront.rfind("/")+1);
			}

			string othername = *it;
			othername = othername.substr(strlen("/usr/share/zoneinfo/"));

			string fullpath = pathfront+pathappent;

			fullpath = fullpath.substr(strlen("/usr/share/zoneinfo/"));

			setenv("TZ", "UTC0", 1);
			time_t utimesec = time(NULL);
			tzset();

			setenv("TZ", fullpath.c_str(), 1);
			tzset();

			struct tm ltm;
			struct tm utm;
			localtime_r(&utimesec, &ltm);
			gmtime_r(&utimesec, &utm);
			time_t ltimesec = mktime(&utm);
			char tmpb[40] = {0};
			sprintf(tmpb, "%04d/%02d/%02d %02d:%02d:%02d", ltm.tm_year+1900, ltm.tm_mon+1, ltm.tm_mday, ltm.tm_hour, ltm.tm_min, ltm.tm_sec);
			zonecell.s_name   =  fullpath;
			zonecell.s_time   =  tmpb;
			zonecell.s_offset =  (utimesec - ltimesec)/3600;
			zonemsg.insert(pair<string, ZoneSrc> (othername, zonecell ));

		}
        else if(S_ISREG(stf.st_mode))
		{
			string fullpath = *it;

			string othername = *it;
			othername = othername.substr(strlen("/usr/share/zoneinfo/"));

			fullpath = fullpath.substr(strlen("/usr/share/zoneinfo/"));

			setenv("TZ", "UTC0", 1);
			time_t utimesec = time(NULL);
			tzset();

			setenv("TZ", fullpath.c_str(), 1);
			tzset();

			struct tm ltm;
			struct tm utm;
			localtime_r(&utimesec, &ltm);
			gmtime_r(&utimesec, &utm);
			time_t ltimesec = mktime(&utm);
			char tmpb[40] = {0};
			sprintf(tmpb, "%04d/%02d/%02d %02d:%02d:%02d", ltm.tm_year+1900, ltm.tm_mon+1, ltm.tm_mday, ltm.tm_hour, ltm.tm_min, ltm.tm_sec);
			zonecell.s_name   =  fullpath;
			zonecell.s_time   =  tmpb;
			zonecell.s_offset =  (utimesec - ltimesec)/3600;
			zonemsg.insert(pair<string, ZoneSrc> (othername, zonecell ));
		}
	}
	infos.clear();
	return true;
}


bool MxcDate::getAllZone(map<string, string> &mmap)
{
	getTimeInfo(mmap);
	return true;
}

bool MxcDate::setTimeZone(string &timezone)
{

	ofstream fout("/etc/timezone");
	if(fout.is_open())
	{
		fout<<timezone;
		fout.close();
		return true;
	}

	return false;
}

bool MxcDate::setEnvTimeZone(string &timezone)
{
	setenv("TZ", timezone.c_str(), 1);
	tzset();
	return true;
}

bool MxcDate::getTimeZone(string &timezone)
{
	ifstream fin("/etc/timezone");
	if(fin.is_open())
	{
		fin>>timezone;
		fin.close();
		return true;
	}
	return false;
}

bool MxcDate::getEnvTimeZone(string &timezone)
{
	char *p = getenv("TZ");
	timezone = p;
	return true;
}

bool MxcDate::initLocalZone()
{
	string TZ;
	if(getTimeZone(TZ))
	    setEnvTimeZone(TZ);
	return true;
}

MxcDate::MxcDate()
{
    initLocalZone();
}

MxcDate::~MxcDate()
{

}

}




