#pragma once
#include <string>
enum class VtParamType
{
	STRING = 0,
	INT,
	FLOAT,
	BOOL,
	COLOR,
	DATE,
	TIME
};

struct VtSystemArg
{
	VtParamType Type;
	std::string sValue;
	double dValue;
	bool bValue;
	int iValue;
	std::string Name;
	std::string Desc;
	bool Enable;
};