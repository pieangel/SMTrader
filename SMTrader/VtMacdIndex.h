#pragma once
#include "VtBaseIndex.h"
class VtMacdIndex :
	public VtBaseIndex
{
public:
	VtMacdIndex();
	VtMacdIndex(VtIndexType type);
	virtual  ~VtMacdIndex();

	int Height;
	int ShortPeriod;
	int LongPeriod;
	int Period;
	int Color;
	int SignalColor;
	int DivColor;

	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
};

