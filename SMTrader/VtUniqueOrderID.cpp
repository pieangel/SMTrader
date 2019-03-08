#include "stdafx.h"
#include "VtUniqueOrderID.h"

int VtUniqueOrderID::_current_id = 1;

VtUniqueOrderID::VtUniqueOrderID()
	: _id(_current_id++)
{
}


VtUniqueOrderID::~VtUniqueOrderID()
{
}


