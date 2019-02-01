#pragma once
#include "VtBaseIndex.h"
#include "Xml/pugixml.hpp"
class VtPriceChannelIndex :
	public VtBaseIndex
{
public:
	VtPriceChannelIndex();
	VtPriceChannelIndex(VtIndexType type);
	virtual ~VtPriceChannelIndex();

	int Period;
	int LineColor;
	int FillColor;

	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
};

