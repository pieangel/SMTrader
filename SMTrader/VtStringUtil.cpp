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

std::string VtStringUtil::getTimeStr()
{
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");
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
