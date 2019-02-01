#pragma once
#include "chartdir.h"
#include <string>
#include <map>
#include "VtIndexDefine.h"
#include "Xml/pugixml.hpp"
class VtBaseStress
{
public:
	VtBaseStress();
	VtBaseStress(VtStressType type);
	virtual ~VtBaseStress();
public:
	std::map<std::string, DoubleArray> DataMap;
	VtStressBase StressBase;
	double Portion;
	int left;
	int top;
	int height;
	int width;
	bool Show = true;

	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
};

