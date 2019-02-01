#include "stdafx.h"
#include "VtBaseIndex.h"


VtBaseIndex::VtBaseIndex()
{
	Show = true;
}


VtBaseIndex::VtBaseIndex(VtIndexType type)
{
	IndexBase.Type = type;
	Show = true;
}

VtBaseIndex::~VtBaseIndex()
{
}

void VtBaseIndex::SaveToXml(pugi::xml_node& node)
{
	CString msg;
	std::string value;
	msg.Format(_T("%d"), (int)IndexBase.Type);
	value = (LPCTSTR)msg;
	pugi::xml_node index_type = node.append_child("index_type");
	index_type.append_child(pugi::node_pcdata).set_value(value.c_str());

	pugi::xml_node index_name = node.append_child("index_name");
	index_name.append_child(pugi::node_pcdata).set_value(IndexBase.Name.c_str());

	pugi::xml_node index_desc = node.append_child("index_desc");
	index_desc.append_child(pugi::node_pcdata).set_value(IndexBase.Name.c_str());
}

void VtBaseIndex::LoadFromXml(pugi::xml_node& node)
{
	std::string index_type = node.child_value("index_type");
	std::string index_name = node.child_value("index_name");
	std::string index_desc = node.child_value("index_desc");

	IndexBase.Type = (VtIndexType)std::stoi(index_type);
	IndexBase.Name = index_name;
	IndexBase.Desc = index_desc;
}
