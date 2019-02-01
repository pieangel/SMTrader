#pragma once
#pragma once

#include "Poco/EventArgs.h"

using Poco::EventArgs;
class CWnd;
enum class HdWindowEventType
{
	None = 0,
	Created,
	Closed
};

enum class HdWindowType
{
	None = 0,
	OrderWindow,
	LeftWindow,
	CenterWindow,
	RightWindow,
	MiniJangoWindow,
	AssetWindow,
	FundDesigner,
	FundMiniJangoWindow,
	ChartWindow,
	StrategyToolWindow
};
class HdWindowEventArgs : public EventArgs
{
public:
	CWnd* pWnd = nullptr;
	HdWindowType wndType = HdWindowType::None;
	HdWindowEventType eventType = HdWindowEventType::None;
};

