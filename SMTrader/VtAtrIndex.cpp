#include "stdafx.h"
#include "VtAtrIndex.h"


VtAtrIndex::VtAtrIndex()
{
}


VtAtrIndex::VtAtrIndex(VtIndexType type)
	:VtBaseIndex(type)
{

}

VtAtrIndex::~VtAtrIndex()
{
}

void VtAtrIndex::SaveToXml(pugi::xml_node& node)
{
	VtBaseIndex::SaveToXml(node);

	/*
		int Height;
	int Period1;
	int Period2;
	int Color1;
	int Color2;

	*/
	CString msg;
	std::string value;

	msg.Format(_T("%d"), Height);
	value = (LPCTSTR)msg;
	pugi::xml_node index_height = node.append_child("index_height");
	index_height.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), Period1);
	value = (LPCTSTR)msg;
	pugi::xml_node index_period1 = node.append_child("index_period1");
	index_period1.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), Period2);
	value = (LPCTSTR)msg;
	pugi::xml_node index_period2 = node.append_child("index_period2");
	index_period2.append_child(pugi::node_pcdata).set_value(value.c_str());


	msg.Format(_T("%d"), Color1);
	value = (LPCTSTR)msg;
	pugi::xml_node index_color1 = node.append_child("index_color1");
	index_color1.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), Color2);
	value = (LPCTSTR)msg;
	pugi::xml_node index_color2 = node.append_child("index_color2");
	index_color2.append_child(pugi::node_pcdata).set_value(value.c_str());
}

void VtAtrIndex::LoadFromXml(pugi::xml_node& node)
{
	VtBaseIndex::LoadFromXml(node);

	std::string index_height = node.child_value("index_height");
	Height = std::stoi(index_height);

	std::string index_period1 = node.child_value("index_period1");
	Period1 = std::stoi(index_period1);

	std::string index_period2 = node.child_value("index_period2");
	Period2 = std::stoi(index_period2);

	std::string index_color1 = node.child_value("index_color1");
	Color1 = std::stoi(index_color1);

	std::string index_color2 = node.child_value("index_color2");
	Color2 = std::stoi(index_color2);
}
