#pragma once
#include "Poco/LocalDateTime.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"
#include <ctime>
#include <string>

using Poco::LocalDateTime;
using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;

class VtTimeUtil
{
public:

	VtTimeUtil()
	{
	}

	virtual ~VtTimeUtil()
	{
	}

	static struct tm Now()
	{
		time_t timer;
		struct tm t;

		timer = time(NULL); 
		localtime_s(&t, &timer);

		return t;
	}

	static std::string GetDateTimeString()
	{
		LocalDateTime now;
		std::string curDate(DateTimeFormatter::format(now, "%Y%n%e"));
		return curDate;
	}

	static std::string GetDate(char* src)
	{
		std::string date;
		char buff[16];
		memset(buff, 0x00, sizeof(buff));
		memcpy(buff, src, 4);
		date.append(buff);
		date.append(_T("³â"));
		memset(buff, 0x00, sizeof(buff));
		memcpy(buff, src + 4, 2);
		date.append(buff);
		date.append(_T("¿ù"));
		memset(buff, 0x00, sizeof(buff));
		memcpy(buff, src + 6, 2);
		date.append(buff);
		date.append(_T("ÀÏ"));
		return date;
	}
};

