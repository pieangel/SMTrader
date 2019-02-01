#include "stdafx.h"
#include "VtBaseStress.h"


VtBaseStress::VtBaseStress()
{
}


VtBaseStress::VtBaseStress(VtStressType type)
{
	StressBase.Type = type;
	Show = true;
}

VtBaseStress::~VtBaseStress()
{
}

void VtBaseStress::SaveToXml(pugi::xml_node& node)
{
	/*
	double Portion;
	int left;
	int top;
	int height;
	int width;
	bool Show = true;*/

	CString msg;
	std::string value;
	msg.Format(_T("%d"), (int)StressBase.Type);
	value = (LPCTSTR)msg;
	pugi::xml_node stress_type = node.append_child("stress_type");
	stress_type.append_child(pugi::node_pcdata).set_value(value.c_str());

	pugi::xml_node stress_name = node.append_child("stress_name");
	stress_name.append_child(pugi::node_pcdata).set_value(StressBase.Name.c_str());

	pugi::xml_node stress_desc = node.append_child("stress_desc");
	stress_desc.append_child(pugi::node_pcdata).set_value(StressBase.Name.c_str());
}

void VtBaseStress::LoadFromXml(pugi::xml_node& node)
{
	std::string stress_type = node.child_value("stress_type");
	std::string stress_name = node.child_value("stress_name");
	std::string stress_desc = node.child_value("stress_desc");

	StressBase.Type = (VtStressType)std::stoi(stress_type);
	StressBase.Name = stress_name;
	StressBase.Desc = stress_desc;
}
