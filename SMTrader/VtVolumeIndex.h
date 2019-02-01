#pragma once
#include "VtBaseIndex.h"
class VtVolumeIndex :
	public VtBaseIndex
{
public:
	VtVolumeIndex();
	VtVolumeIndex(VtIndexType type);
	virtual ~VtVolumeIndex();
	int Height;
	int UpColor;
	int DownColor;
	int FlatColor;

	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
};

