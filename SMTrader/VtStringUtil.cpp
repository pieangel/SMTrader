#include "stdafx.h"
#include "VtStringUtil.h"
#include <string>
#include <sstream>
#include <chrono>
#include <ctime>
#include <time.h>
#include <iomanip>

VtStringUtil::VtStringUtil()
{
}


VtStringUtil::~VtStringUtil()
{
}

std::pair<std::string, std::string> VtStringUtil::GetCurrentDateTime()
{
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d");
	std::string date = ss.str();
	ss.str("");
	ss << std::put_time(std::localtime(&in_time_t), "%H%M%S");
	std::string time = ss.str();
	return std::make_pair(date, time);
}

std::string VtStringUtil::getTimeStr()
{
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");
	return ss.str();
}

std::string VtStringUtil::getCurentDate()
{
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d");
	return ss.str();
}

bool VtStringUtil::endsWith(const std::string& s, const std::string& suffix)
{
	return s.size() >= suffix.size() &&
		s.substr(s.size() - suffix.size()) == suffix;
}

std::vector<std::string> VtStringUtil::split(const std::string& s, const std::string& delimiter, const bool& removeEmptyEntries /*= false*/)
{
	std::vector<std::string> tokens;

	for (size_t start = 0, end; start < s.length(); start = end + delimiter.length())
	{
		size_t position = s.find(delimiter, start);
		end = position != std::string::npos ? position : s.length();

		std::string token = s.substr(start, end - start);
		if (!removeEmptyEntries || !token.empty())
		{
			tokens.push_back(token);
		}
	}

	if (!removeEmptyEntries &&
		(s.empty() || endsWith(s, delimiter)))
	{
		tokens.push_back("");
	}

	return tokens;
}


std::string VtStringUtil::PadLeft(int input, char padding, int len)
{
	std::ostringstream out;
	out << std::internal << std::right << std::setfill(padding) << std::setw(len) << input;
	return out.str();
}

std::string VtStringUtil::PadLeft(std::string input, char padding, int len)
{
	std::ostringstream out;
	out << std::right << std::setfill(padding) << std::setw(len) << input;
	return out.str();
}

std::string VtStringUtil::PadLeft(double input, char padding, int len, int decimal)
{
	std::ostringstream out;
	out << std::fixed << std::setprecision(decimal) << std::right << std::setfill(padding) << std::setw(len) << input;
	return out.str();
}

std::string VtStringUtil::PadRight(int input, char padding, int len)
{
	std::ostringstream out;
	out << std::internal << std::left << std::setfill(padding) << std::setw(len) << input;
	return out.str();
}

std::string VtStringUtil::PadRight(std::string input, char padding, int len)
{
	std::ostringstream out;
	out << std::left << std::setfill(padding) << std::setw(len) << input;
	return out.str();
}

std::string VtStringUtil::PadRight(double input, char padding, int len, int decimal)
{
	std::ostringstream out;
	out << std::fixed << std::setprecision(decimal) << std::left << std::setfill(padding) << std::setw(len) << input;
	return out.str();
}

std::time_t VtStringUtil::GetUTCTimestamp(std::string datetime_string)
{
	time_t rawtime;
	struct tm* timeinfo;

	int year = std::stoi(datetime_string.substr(0, 4));
	int month = std::stoi(datetime_string.substr(4, 2));
	int day = std::stoi(datetime_string.substr(6, 2));
	int hour = 0;
	int min = 0;
	int sec = 0;
	if (datetime_string.length() > 8) {
		hour = std::stoi(datetime_string.substr(8, 2));
		min = std::stoi(datetime_string.substr(10, 2));
		sec = std::stoi(datetime_string.substr(12, 2));
	}


	/* get current timeinfo: */
	time(&rawtime); //or: rawtime = time(0);
					/* convert to struct: */
	timeinfo = localtime(&rawtime);

	/* now modify the timeinfo to the given date: */
	timeinfo->tm_year = year - 1900;
	timeinfo->tm_mon = month - 1;    //months since January - [0,11]
	timeinfo->tm_mday = day;          //day of the month - [1,31] 
	timeinfo->tm_hour = hour;         //hours since midnight - [0,23]
	timeinfo->tm_min = min;          //minutes after the hour - [0,59]
	timeinfo->tm_sec = sec;          //seconds after the minute - [0,59]

									 /* call mktime: create unix time stamp from timeinfo struct */
	std::time_t date = mktime(timeinfo);

	return date;
}

std::string VtStringUtil::GetLocalTime(std::string utc_time_format)
{
	std::wstring wstrUTCTime = std::wstring(utc_time_format.begin(), utc_time_format.end());
	time_t utctime = getEpochTime(wstrUTCTime.c_str());
	struct tm tm;
	/*Convert UTC TIME To Local TIme*/
	localtime_s(&tm, &utctime);
	char CharLocalTimeofUTCTime[30];
	strftime(CharLocalTimeofUTCTime, 30, "%Y-%m-%dT%H:%M:%SZ", &tm);
	std::string strLocalTimeofUTCTime(CharLocalTimeofUTCTime);
	return strLocalTimeofUTCTime;
}

std::string VtStringUtil::GetLocalTime(time_t utc_time_t)
{
	struct tm tm;
	/*Convert UTC TIME To Local TIme*/
	localtime_s(&tm, &utc_time_t);
	char CharLocalTimeofUTCTime[30];
	strftime(CharLocalTimeofUTCTime, 30, "%Y-%m-%dT%H:%M:%SZ", &tm);
	std::string strLocalTimeofUTCTime(CharLocalTimeofUTCTime);
	return strLocalTimeofUTCTime;
}

std::string VtStringUtil::GetLocalTimeByDatetimeString(std::string date_time)
{
	std::time_t utc_time_t = GetUTCTimestamp(date_time);
	return GetLocalTime(utc_time_t);
}

std::time_t VtStringUtil::getEpochTime(const std::wstring& dateTime)
{
	/* Standard UTC Format*/
	static const std::wstring dateTimeFormat{ L"%Y-%m-%dT%H:%M:%SZ" };

	std::wistringstream ss{ dateTime };
	std::tm dt;
	ss >> std::get_time(&dt, dateTimeFormat.c_str());

	/* Convert the tm structure to time_t value and return Epoch. */
	return _mkgmtime(&dt);
}

std::time_t VtStringUtil::GetEpochTime(const std::string& dateTime)
{
	/* Standard UTC Format*/
	static const std::string dateTimeFormat{ "%Y%m%d%H%M%S" };

	std::istringstream ss{ dateTime };
	std::tm dt;
	ss >> std::get_time(&dt, dateTimeFormat.c_str());

	/* Convert the tm structure to time_t value and return Epoch. */
	return _mkgmtime(&dt);
}

long long VtStringUtil::GetCurrentNanoseconds()
{
	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();

	typedef std::chrono::duration<int, std::ratio_multiply<std::chrono::hours::period, std::ratio<8>
	>::type> Days; /* UTC: +8:00 */

	Days days = std::chrono::duration_cast<Days>(duration);
	duration -= days;
	auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
	duration -= hours;
	auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
	duration -= minutes;
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
	duration -= seconds;
	auto tot_nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
	return tot_nanoseconds.count();
}

std::time_t VtStringUtil::GetUTCTimestampByDate()
{
	time_t rawtime;
	struct tm* timeinfo;

	/* get current timeinfo: */
	time(&rawtime); //or: rawtime = time(0);
					/* convert to struct: */
	timeinfo = localtime(&rawtime);

	/* now modify the timeinfo to the given date: */
	timeinfo->tm_hour = 0;         //hours since midnight - [0,23]
	timeinfo->tm_min = 0;          //minutes after the hour - [0,59]
	timeinfo->tm_sec = 0;          //seconds after the minute - [0,59]

								   /* call mktime: create unix time stamp from timeinfo struct */
	std::time_t date = mktime(timeinfo);

	return date;
}

std::string VtStringUtil::NowTime()
{
	const int MAX_LEN = 200;
	char buffer[MAX_LEN];
	if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0,
		"HH':'mm':'ss", buffer, MAX_LEN) == 0)
		return "Error in NowTime()";

	char result[100] = { 0 };
	static DWORD first = GetTickCount();
	std::sprintf(result, "%s.%03ld", buffer, (long)(GetTickCount() - first) % 1000);
	return result;
}
