#include "stdafx.h"
#include "VtSimpleMaStress.h"


VtSimpleMaStress::VtSimpleMaStress()
{
}


VtSimpleMaStress::VtSimpleMaStress(VtStressType type)
	:VtBaseStress(type)
{

}

VtSimpleMaStress::~VtSimpleMaStress()
{
}

void VtSimpleMaStress::SaveToXml(pugi::xml_node& node)
{
	VtBaseStress::SaveToXml(node);
}

void VtSimpleMaStress::LoadFromXml(pugi::xml_node& node)
{
	VtBaseStress::LoadFromXml(node);
}
