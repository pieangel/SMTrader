#pragma once
#include "VtBaseStress.h"
#include "Xml/pugixml.hpp"
class VtSimpleMaStress :
	public VtBaseStress
{
public:
	VtSimpleMaStress();
	VtSimpleMaStress(VtStressType type);
	virtual  ~VtSimpleMaStress();

	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
};

