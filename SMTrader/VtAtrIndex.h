#pragma once
#include "VtBaseIndex.h"
#include "Xml/pugixml.hpp"
class VtAtrIndex :
	public VtBaseIndex
{
public:
	VtAtrIndex();
	VtAtrIndex(VtIndexType type);
	virtual ~VtAtrIndex();
	int Height;
	int Period1;
	int Period2;
	int Color1;
	int Color2;

	virtual void SaveToXml(pugi::xml_node& node);
	void LoadFromXml(pugi::xml_node& node);
};

