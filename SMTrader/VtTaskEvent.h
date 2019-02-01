#pragma once
#include "VtTaskArg.h"
#include "Poco/EventArgs.h"

using Poco::EventArgs;

class VtTaskEventArgs : public EventArgs
{
public:
	VtTaskType TaskType;
};


