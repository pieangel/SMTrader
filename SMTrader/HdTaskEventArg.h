#pragma once
#include "HdTaskArg.h"
#include "Poco/EventArgs.h"
#include <string>
using Poco::EventArgs;
class VtAccount;
struct VtOrder;
class VtSymbol;
class HdTaskEventArgs : public EventArgs
{
public:
	HdTaskType TaskType;
	int RequestId;
	VtAccount* Acnt = nullptr;
	VtOrder* Order = nullptr;
	VtSymbol* Symbol = nullptr;
	std::string TaskName;

	HdTaskEventArgs::HdTaskEventArgs()
	{
		RequestId = 0;
		Acnt = nullptr;
		Order = nullptr;
		Symbol = nullptr;
	}

	HdTaskEventArgs::HdTaskEventArgs(const HdTaskEventArgs &obj) 
	{
		RequestId = obj.RequestId;
		Acnt = obj.Acnt;
		Order = obj.Order;
		Symbol = obj.Symbol;
	}

	HdTaskEventArgs& operator=(const HdTaskEventArgs& obj)
	{
		// check for self-assignment
		if (&obj == this)
			return *this;
		RequestId = obj.RequestId;
		Acnt = obj.Acnt;
		Order = obj.Order;
		Symbol = obj.Symbol;

		return *this;
	}
};
