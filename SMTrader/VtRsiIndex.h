#pragma once
#include "VtBaseIndex.h"
#include "Xml/pugixml.hpp"
class VtRsiIndex :
	public VtBaseIndex
{
public:
	VtRsiIndex();
	VtRsiIndex(VtIndexType type);
	virtual ~VtRsiIndex();
	int Height;
	int Period;
	int Color;
	int Range;
	int UpColor;
	int DownColor;

	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
};

