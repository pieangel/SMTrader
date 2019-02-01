#pragma once
#include "chartdir.h"
#include <string>
#include <map>
#include "VtIndexDefine.h"
#include "Xml/pugixml.hpp"
class VtBaseBand
{
public:
	VtBaseBand();
	VtBaseBand(VtBandType type);
	virtual ~VtBaseBand();
public:
	std::map<std::string, DoubleArray> DataMap;
	VtBandBase BandBase;
	double Portion;
	int left;
	int top;
	int height;
	int width;
	bool Show = true;

	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
};

