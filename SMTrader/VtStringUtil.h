#pragma once
#include <string>
#include <vector>
#include <algorithm>

class VtStringUtil
{
public:
	VtStringUtil();
	~VtStringUtil();
	static std::string getTimeStr();
	static bool endsWith(const std::string& s, const std::string& suffix);
	static std::vector<std::string> split(const std::string& s, const std::string& delimiter, const bool& removeEmptyEntries = false);
};

