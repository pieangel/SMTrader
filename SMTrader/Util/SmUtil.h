#pragma once
#include <tuple>
#include <time.h>
#include <vector>
#include <string>
#define MAX_BUFFER 128

class SmUtil {
public:
	static std::tuple<int, int, int> GetLocalTime();
	static std::vector<int> GetLocalDate();
	static std::vector<int> GetTime(std::string datetime_string);
	static std::vector<int> GetLocalDateTime();
	static std::vector<int> GetUtcDateTime();
	static std::string GetUTCDateTimeString();
	static std::string GetUTCDateTimeStringForNowMin();
	static std::string GetUTCDateTimeStringForPreMin(int previousMinLen);
	static double GetDifTimeBySeconds(std::string newTime, std::string oldTime);
	static double GetDifTimeForNow(std::string srcTime);
};

