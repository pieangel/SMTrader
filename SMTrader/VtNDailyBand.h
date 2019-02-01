#pragma once
#include "VtBaseBand.h"
#include "Xml/pugixml.hpp"
class VtNDailyBand :
	public VtBaseBand
{
public:

	VtNDailyBand();
	VtNDailyBand(VtBandType type);
	virtual  ~VtNDailyBand();

	int BackLen;
	int HighColor;
	int LowColor;

	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
};

