#pragma once
#include "Global/TemplateSingleton.h"

class VtSplitterManager : public TemplateSingleton<VtSplitterManager>
{
public:
	VtSplitterManager();
	~VtSplitterManager();

	int SPLITID[120];
	void Init();
	int GetID();
private:
	int _CurIndex = 0;
};

