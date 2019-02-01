#pragma once
#include "Poco/EventArgs.h"

using Poco::EventArgs;
struct VtRealtimeQuote;
class VtRealtimeQuoteEventArgs : public EventArgs
{
public:
	VtRealtimeQuote* pQuote = nullptr;
};
