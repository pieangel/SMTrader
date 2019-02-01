#pragma once

#include "Poco/EventArgs.h"

using Poco::EventArgs;
class CVtOrderWnd;
class VtOrderWndHd;
enum class VtOrderWindowEventType 
{
	None = 0,
	Created,
	Closed
};
class VtOrderWindowEventArgs : public EventArgs
{
public:
	CVtOrderWnd* pOrderWnd = nullptr;
	VtOrderWindowEventType type = VtOrderWindowEventType::None;
};

class VtOrderWndEventArgs : public EventArgs
{
public:
	VtOrderWndHd* pOrderWnd = nullptr;
	VtOrderWindowEventType type = VtOrderWindowEventType::None;
};

