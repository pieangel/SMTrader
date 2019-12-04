#pragma once
#include <string>
#include "Xml/pugixml.hpp"
class SmChartDataSource;
class VtReferenceChart
{
public:
	VtReferenceChart();
	~VtReferenceChart();

	bool show = true;
	bool shareAxisWithMain;
	bool embedded;
	int height;
	int type;
	int color;
	//int colorIndex;
	std::string name;
	SmChartDataSource* data;

	virtual void SaveToXml(pugi::xml_node& node);
	virtual void LoadFromXml(pugi::xml_node& node);
};

