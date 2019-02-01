#pragma once
#include "VtBaseStress.h"
#include "Xml/pugixml.hpp"
class VtRobo1Stress :
	public VtBaseStress
{
public:
	VtRobo1Stress();
	virtual ~VtRobo1Stress();
	VtRobo1Stress(VtStressType type);

	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
};

