#pragma once
#include "VtBaseIndex.h"
#include "Xml/pugixml.hpp"
class VtTrixIndex :
	public VtBaseIndex
{
public:
	VtTrixIndex();
	VtTrixIndex(VtIndexType type);
	virtual ~VtTrixIndex();
	int Height;
	int Period;
	int Color;

	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
};

