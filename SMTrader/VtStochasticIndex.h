#pragma once
#include "VtBaseIndex.h"
#include "Xml/pugixml.hpp"
class VtStochasticIndex :
	public VtBaseIndex
{
public:
	VtStochasticIndex();
	VtStochasticIndex(VtIndexType type);
	virtual ~VtStochasticIndex();
	int Height;
	int FastK;
	int SlowK;
	int SlowD;
	int ColorSlowK;
	int ColorSlowD;
	int Range;
	int UpColor;
	int DownColor;

	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
};

