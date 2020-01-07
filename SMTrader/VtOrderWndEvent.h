#pragma once

#include "Poco/EventArgs.h"

using Poco::EventArgs;
class VtOrderWnd;
class VtOrderWndHd;
enum class VtOrderWindowEventType 
{
	None = 0,
	Created,
	Closed
};
class VtOrderWndEventArgs : public EventArgs
{
public:
	VtOrderWnd* pOrderWnd = nullptr;
	VtOrderWindowEventType type = VtOrderWindowEventType::None;
};

class VtOrderWndHdEventArgs : public EventArgs
{
public:
	VtOrderWndHd* pOrderWnd = nullptr;
	VtOrderWindowEventType type = VtOrderWindowEventType::None;
};

