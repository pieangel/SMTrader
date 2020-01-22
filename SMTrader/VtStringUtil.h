#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <time.h>
#include <ctime>
#include <windows.h>

class VtStringUtil
{
public:
	VtStringUtil();
	~VtStringUtil();
	static std::pair<std::string, std::string> GetCurrentDateTime();
	static std::string getTimeStr();
	static std::string getCurentDate();
	static bool endsWith(const std::string& s, const std::string& suffix);
	static std::vector<std::string> split(const std::string& s, const std::string& delimiter, const bool& removeEmptyEntries = false);
	static std::string PadLeft(double input, char padding, int len, int decimal);
	static std::string PadRight(double input, char padding, int len, int decimal);
	static std::string PadLeft(int input, char padding, int len);
	static std::string PadRight(int input, char padding, int len);
	static std::string PadLeft(std::string input, char padding, int len);
	static std::string PadRight(std::string input, char padding, int len);
	static std::time_t GetUTCTimestamp(std::string datetime_string);
	static std::string GetLocalTime(std::string utc_time_format);
	static std::string GetLocalTime(time_t utc_time_t);
	static std::string GetLocalTimeByDatetimeString(std::string date_time);
	static std::time_t getEpochTime(const std::wstring& dateTime);
	static std::time_t GetEpochTime(const std::string& dateTime);
	static long long GetCurrentNanoseconds();
	static std::time_t GetUTCTimestampByDate();
	static std::string NowTime();
};

