#include "stdafx.h"
#include "VtVolumeIndex.h"


VtVolumeIndex::VtVolumeIndex()
{
}


VtVolumeIndex::VtVolumeIndex(VtIndexType type)
	: VtBaseIndex(type)
{

}

VtVolumeIndex::~VtVolumeIndex()
{
}

void VtVolumeIndex::SaveToXml(pugi::xml_node & node)
{
	VtBaseIndex::SaveToXml(node);

	/*
	int Height;
	int UpColor;
	int DownColor;
	int FlatColor;*/

	CString msg;
	std::string value;

	msg.Format(_T("%d"), Height);
	value = (LPCTSTR)msg;
	pugi::xml_node index_height = node.append_child("index_height");
	index_height.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), FlatColor);
	value = (LPCTSTR)msg;
	pugi::xml_node index_flat_color = node.append_child("index_flat_color");
	index_flat_color.append_child(pugi::node_pcdata).set_value(value.c_str());


	msg.Format(_T("%d"), UpColor);
	value = (LPCTSTR)msg;
	pugi::xml_node index_up_color = node.append_child("index_up_color");
	index_up_color.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), DownColor);
	value = (LPCTSTR)msg;
	pugi::xml_node index_down_color = node.append_child("index_down_color");
	index_down_color.append_child(pugi::node_pcdata).set_value(value.c_str());
}

void VtVolumeIndex::LoadFromXml(pugi::xml_node& node)
{
	VtBaseIndex::LoadFromXml(node);

	std::string index_height = node.child_value("index_height");
	Height = std::stoi(index_height);

	std::string index_flat_color = node.child_value("index_flat_color");
	FlatColor = std::stoi(index_flat_color);

	std::string index_up_color = node.child_value("index_up_color");
	UpColor = std::stoi(index_up_color);

	std::string index_down_color = node.child_value("index_down_color");
	DownColor = std::stoi(index_down_color);
}
