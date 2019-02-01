#pragma once
#include "Poco/EventArgs.h"

using Poco::EventArgs;
class VtChartData;
class VtAccountManager;
class VtChartDataEventArgs : public EventArgs
{
public:
	VtChartData* pChart = nullptr;
};


