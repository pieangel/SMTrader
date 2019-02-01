#pragma once
#include "VtBaseIndex.h"
#include "Xml/pugixml.hpp"
class VtMaIndex :
	public VtBaseIndex
{
public:
	VtMaIndex();
	VtMaIndex(VtIndexType type);
	virtual ~VtMaIndex();

	int Period;
	int Color;

	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
};

