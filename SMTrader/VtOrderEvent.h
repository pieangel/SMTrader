#pragma once

#include "Poco/EventArgs.h"

using Poco::EventArgs;

struct VtOrder;
class VtOrderEventArgs : public EventArgs
{
public:
	VtOrder* pOrder = nullptr;
};

