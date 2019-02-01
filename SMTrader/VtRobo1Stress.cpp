#include "stdafx.h"
#include "VtRobo1Stress.h"


VtRobo1Stress::VtRobo1Stress()
{
}


VtRobo1Stress::VtRobo1Stress(VtStressType type)
	:VtBaseStress(type)
{

}

void VtRobo1Stress::SaveToXml(pugi::xml_node& node)
{
	VtBaseStress::SaveToXml(node);
}

void VtRobo1Stress::LoadFromXml(pugi::xml_node& node)
{
	VtBaseStress::LoadFromXml(node);
}

VtRobo1Stress::~VtRobo1Stress()
{
}
