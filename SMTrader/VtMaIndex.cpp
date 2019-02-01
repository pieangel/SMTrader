#include "stdafx.h"
#include "VtMaIndex.h"


VtMaIndex::VtMaIndex()
{
}


VtMaIndex::VtMaIndex(VtIndexType type)
	: VtBaseIndex(type)
{

}

VtMaIndex::~VtMaIndex()
{
	int i = 0;
}

void VtMaIndex::SaveToXml(pugi::xml_node& node)
{
	VtBaseIndex::SaveToXml(node);

	CString msg;
	std::string value;

	msg.Format(_T("%d"), Period);
	value = (LPCTSTR)msg;
	pugi::xml_node index_period = node.append_child("index_period");
	index_period.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), Color);
	value = (LPCTSTR)msg;
	pugi::xml_node index_color = node.append_child("index_color");
	index_color.append_child(pugi::node_pcdata).set_value(value.c_str());
}

void VtMaIndex::LoadFromXml(pugi::xml_node& node)
{
	VtBaseIndex::LoadFromXml(node);

	std::string index_period = node.child_value("index_period");
	Period = std::stoi(index_period);

	std::string index_color = node.child_value("index_color");
	Color = std::stoi(index_color);
}
