#include "stdafx.h"
#include "VtUniqueChartID.h"

int VtUniqueChartID::_current_id = 1;

VtUniqueChartID::VtUniqueChartID()
	: _id(_current_id++)
{
}


VtUniqueChartID::~VtUniqueChartID()
{
}
