#pragma once
#include "Global/TemplateSingleton.h"
class VtLogManager : public TemplateSingleton<VtLogManager>
{
public:
	VtLogManager();
	~VtLogManager();
	void InitLog();
};

