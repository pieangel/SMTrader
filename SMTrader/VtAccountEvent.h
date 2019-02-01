#pragma once

#include "Poco/EventArgs.h"

using Poco::EventArgs;

class VtAccountManager;
class VtAccountEventArgs : public EventArgs
{
public:
	VtAccountManager* pAcntMgr = nullptr;
};

