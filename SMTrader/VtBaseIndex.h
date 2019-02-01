#pragma once
#include "chartdir.h"
#include <string>
#include <map>
#include "VtIndexDefine.h"
#include "Xml/pugixml.hpp"
class VtBaseIndex
{
public:
	VtBaseIndex();
	VtBaseIndex(VtIndexType type);
	virtual ~VtBaseIndex();
public:
	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
	std::map<std::string, DoubleArray> DataMap;
	VtIndexBase IndexBase;
	double Portion;
	int left;
	int top;
	int height;
	int width;
	bool Show = true;
};

