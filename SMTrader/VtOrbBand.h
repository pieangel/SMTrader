#pragma once
#include "VtBaseBand.h"
#include "Xml/pugixml.hpp"
class VtOrbBand :
	public VtBaseBand
{
public:
	VtOrbBand();
	virtual ~VtOrbBand();
	VtOrbBand(VtBandType type);
	int BackLen;
	int Dividance;

	int HighColor;
	int LowColor;

	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
};

