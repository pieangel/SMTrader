#pragma once
#include "Global/VtDefine.h"
#include <queue>
#include "VtOrder.h"
// VtOrderMangerWnd

class VtOrderManagerWnd : public CStatic
{
	DECLARE_DYNAMIC(VtOrderManagerWnd)

public:
	VtOrderManagerWnd();
	virtual ~VtOrderManagerWnd();
	afx_msg LRESULT OnOrderAccepted(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOrderAdded(WPARAM wParam, LPARAM lParam);
protected:
	DECLARE_MESSAGE_MAP()
public:
	void AddOrderRequest(HdOrderRequest&& req);
private:
	std::queue<HdOrderRequest> _OrderQ;
	bool _Working = false;
};


