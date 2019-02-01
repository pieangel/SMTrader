// GridButton.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "GridButton.h"


// CGridButton

IMPLEMENT_DYNAMIC(CGridButton, CMFCButton)

CGridButton::CGridButton()
{

}

CGridButton::~CGridButton()
{
}


void CGridButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

	CFont Font;
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);		//Get device context object
	CRect rt;
	rt = lpDrawItemStruct->rcItem;		//Get button rect
	Font.CreateFont(
		14,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"Arial");                 // lpszFacename

	CFont* pFont = dc.SelectObject(&Font);
	dc.FillSolidRect(rt, RGB(70, 130, 180));		//Fill button with blue color

	UINT state = lpDrawItemStruct->itemState;	//Get state of the button
	if ((state & ODS_SELECTED))		// If it is pressed
	{
		dc.DrawEdge(rt, EDGE_SUNKEN, BF_RECT);    // Draw a sunken face
	}
	else
	{
		dc.DrawEdge(rt, EDGE_RAISED, BF_RECT);	// Draw a raised face
	}
	dc.SetTextColor(RGB(255, 255, 255));		// Set the color of the caption to be yellow
	CString strTemp;
	GetWindowText(strTemp);		// Get the caption which have been set
	/* Rotate and draw caption*/
	int ret = SetGraphicsMode(dc.GetSafeHdc(), GM_ADVANCED);

	XFORM form;
	double angle = -90 * 3.14159 / 180.0;
	double s = sin(angle);
	double c = cos(angle);

	form.eM11 = static_cast<float>(c);
	form.eM12 = static_cast<float>(s);
	form.eM21 = static_cast<float>(-s);
	form.eM22 = static_cast<float>(c);

	form.eDx = 0;
	form.eDy = 0;

	dc.SetTextAlign(TA_LEFT | TA_TOP);
	SetWorldTransform(dc.GetSafeHdc(), &form);
	dc.DPtoLP(&rt);

	dc.DrawText(strTemp, rt, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	/********************************************************************/
	//dc.DrawText(strTemp, rt, DT_CENTER | DT_VCENTER | DT_SINGLELINE);		// Draw out the caption
	if ((state & ODS_FOCUS))       // If the button is focused
	{
		// Draw a focus rect which indicates the user 
		// that the button is focused
		int iChange = 3;
		rt.top += iChange;
		rt.left += iChange;
		rt.right -= iChange;
		rt.bottom -= iChange;
		dc.DrawFocusRect(rt);
	}

	dc.SelectObject(pFont);
	Font.DeleteObject();

	dc.Detach();
}

BEGIN_MESSAGE_MAP(CGridButton, CMFCButton)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CGridButton message handlers




BOOL CGridButton::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	return CMFCButton::PreTranslateMessage(pMsg);
}


void CGridButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CMFCButton::OnLButtonDown(nFlags, point);
}
