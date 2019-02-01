#include "stdafx.h"
#include "VtStochasticIndex.h"


VtStochasticIndex::VtStochasticIndex()
{
}


VtStochasticIndex::VtStochasticIndex(VtIndexType type)
	: VtBaseIndex(type)
{

}

VtStochasticIndex::~VtStochasticIndex()
{
}

void VtStochasticIndex::SaveToXml(pugi::xml_node& node)
{
	VtBaseIndex::SaveToXml(node);

	/*
	int Height;
	int FastK;
	int SlowK;
	int SlowD;
	int ColorSlowK;
	int ColorSlowD;
	int Range;
	int UpColor;
	int DownColor;*/

	CString msg;
	std::string value;

	msg.Format(_T("%d"), Height);
	value = (LPCTSTR)msg;
	pugi::xml_node index_height = node.append_child("index_height");
	index_height.append_child(pugi::node_pcdata).set_value(value.c_str());


	msg.Format(_T("%d"), FastK);
	value = (LPCTSTR)msg;
	pugi::xml_node index_fastk = node.append_child("index_fastk");
	index_fastk.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), SlowD);
	value = (LPCTSTR)msg;
	pugi::xml_node index_slowd = node.append_child("index_slowd");
	index_slowd.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), ColorSlowK);
	value = (LPCTSTR)msg;
	pugi::xml_node index_fastk_color = node.append_child("index_fastk_color");
	index_fastk_color.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), ColorSlowD);
	value = (LPCTSTR)msg;
	pugi::xml_node index_slowd_color = node.append_child("index_slowd_color");
	index_slowd_color.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), Range);
	value = (LPCTSTR)msg;
	pugi::xml_node index_range = node.append_child("index_range");
	index_range.append_child(pugi::node_pcdata).set_value(value.c_str());


	msg.Format(_T("%d"), UpColor);
	value = (LPCTSTR)msg;
	pugi::xml_node index_up_color = node.append_child("index_up_color");
	index_up_color.append_child(pugi::node_pcdata).set_value(value.c_str());

	msg.Format(_T("%d"), DownColor);
	value = (LPCTSTR)msg;
	pugi::xml_node index_down_color = node.append_child("index_down_color");
	index_down_color.append_child(pugi::node_pcdata).set_value(value.c_str());
}

void VtStochasticIndex::LoadFromXml(pugi::xml_node& node)
{
	VtBaseIndex::LoadFromXml(node);

	std::string index_height = node.child_value("index_height");
	Height = std::stoi(index_height);

	std::string index_fastk = node.child_value("index_fastk");
	FastK = std::stoi(index_fastk);

	std::string index_slowd = node.child_value("index_slowd");
	SlowD = std::stoi(index_slowd);

	std::string index_fastk_color = node.child_value("index_fastk_color");
	ColorSlowK = std::stoi(index_fastk_color);

	std::string index_slowd_color = node.child_value("index_slowd_color");
	ColorSlowD = std::stoi(index_slowd_color);

	std::string index_range = node.child_value("index_range");
	Range = std::stoi(index_range);

	std::string index_up_color = node.child_value("index_up_color");
	UpColor = std::stoi(index_up_color);

	std::string index_down_color = node.child_value("index_down_color");
	DownColor = std::stoi(index_down_color);
}
