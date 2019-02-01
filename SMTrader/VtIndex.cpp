#include "stdafx.h"
#include "VtIndex.h"


VtIndex::VtIndex()
{
}


VtIndex::~VtIndex()
{
	for (auto it = OutDataMap.begin(); it != OutDataMap.end(); ++it)
	{
		delete[] it->second;
	}
}
