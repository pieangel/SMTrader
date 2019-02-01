#include "stdafx.h"
#include "VtSplitterManager.h"
#include "Resource.h"

VtSplitterManager::VtSplitterManager()
{
	_CurIndex = -1;
	Init();
}


VtSplitterManager::~VtSplitterManager()
{
}

void VtSplitterManager::Init()
{
	for (int i = ID_SPLIT1; i < 120; i++)
	{
		SPLITID[i] = i;
	}
}

int VtSplitterManager::GetID()
{
	if (_CurIndex >= 120)
		return -1;

	_CurIndex++;
	return SPLITID[_CurIndex];
}
