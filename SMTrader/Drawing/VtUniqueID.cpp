#include "stdafx.h"
#include "VtUniqueID.h"

int VtUniqueID::_current_id = 1;

VtUniqueID::VtUniqueID()
	: _id(_current_id++)
{
}


VtUniqueID::~VtUniqueID()
{
}


