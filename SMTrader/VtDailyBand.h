#pragma once
#include "VtBaseBand.h"
#include "Xml/pugixml.hpp"
class VtDailyBand :
	public VtBaseBand
{
public:

	VtDailyBand();
	VtDailyBand(VtBandType type);
	virtual  ~VtDailyBand();

	int HighColor;
	int LowColor;

	int ShowDailyLine;

	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
};

