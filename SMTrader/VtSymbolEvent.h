#pragma once


#include "Poco/EventArgs.h"

using Poco::EventArgs;

class VtSymbolManager;
class VtSymbolEventArgs : public EventArgs
{
public:
	VtSymbolManager* pSymMgr = nullptr;
};

