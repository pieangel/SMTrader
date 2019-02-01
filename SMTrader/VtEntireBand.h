#pragma once
#include "VtBaseBand.h"
#include "Xml/pugixml.hpp"
class VtEntireBand :
	public VtBaseBand
{
public:
	VtEntireBand();
	VtEntireBand(VtBandType type);
	virtual  ~VtEntireBand();

	int HighColor;
	int LowColor;

	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
};

