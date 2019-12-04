#include "stdafx.h"
#include "VtReferenceChart.h"
#include "VtChartData.h"


VtReferenceChart::VtReferenceChart()
{
	data = nullptr;
}


VtReferenceChart::~VtReferenceChart()
{
}

void VtReferenceChart::SaveToXml(pugi::xml_node& node)
{
	/*
	bool show = true;
	bool shareAxisWithMain;
	bool embeded;
	int height;
	int type;
	int color;
	std::string name;
	VtChartData* data;*/

	CString msg;
	std::string value;

	
	pugi::xml_node refchart_show = node.append_child("refchart_show");
	if (show)
		value = _T("true");
	else
		value = _T("false");
	refchart_show.append_child(pugi::node_pcdata).set_value(value.c_str());

	pugi::xml_node share_axis_with_main = node.append_child("share_axis_with_main");
	if (shareAxisWithMain)
		value = _T("true");
	else
		value = _T("false");
	share_axis_with_main.append_child(pugi::node_pcdata).set_value(value.c_str());

	pugi::xml_node  refchart_embedded = node.append_child("refchart_embedded");
	if (embedded)
		value = _T("true");
	else
		value = _T("false");
	refchart_embedded.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), height);
	value = (LPCTSTR)msg;
	pugi::xml_node refchart_height = node.append_child("refchart_height");
	refchart_height.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), type);
	value = (LPCTSTR)msg;
	pugi::xml_node refchart_type = node.append_child("refchart_type");
	refchart_type.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), color);
	value = (LPCTSTR)msg;
	pugi::xml_node refchart_color = node.append_child("refchart_color");
	refchart_color.append_child(pugi::node_pcdata).set_value(value.c_str());

	//if (data)
	//	data->SaveToXml(node);
}

void VtReferenceChart::LoadFromXml(pugi::xml_node& node)
{
	std::string refchart_show = node.child_value("refchart_show");
	std::string share_axis_with_main = node.child_value("share_axis_with_main");
	std::string refchart_embedded = node.child_value("refchart_embedded");

	std::string refchart_height = node.child_value("refchart_height");
	std::string refchart_type = node.child_value("refchart_type");
	std::string refchart_color = node.child_value("refchart_color");

	if (refchart_show.compare(_T("true")) == 0)
		show = true;
	else
		show = false;

	if (share_axis_with_main.compare(_T("true")) == 0)
		shareAxisWithMain = true;
	else
		shareAxisWithMain = false;

	if (refchart_embedded.compare(_T("true")) == 0)
		embedded = true;
	else
		embedded = false;

	height = std::stoi(refchart_height);
	type = std::stoi(refchart_type);
	color = std::stoi(refchart_color);
}
