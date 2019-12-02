// GridCellBase.cpp : implementation file
//
// MFC Grid Control - Main grid cell base class
//
// Provides the implementation for the base cell type of the
// grid control. No data is stored (except for state) but default
// implementations of drawing, printingetc provided. MUST be derived
// from to be used.
//
// Written by Chris Maunder <chris@codeproject.com>
// Copyright (c) 1998-2005. All Rights Reserved.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. 
//
// An email letting me know how you are using it would be nice as well. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// For use with CGridCtrl v2.22+
//
// History:
// Ken Bertelson - 12 Apr 2000 - Split CGridCell into CGridCell and CGridCellBase
// C Maunder     - 19 May 2000 - Fixed sort arrow drawing (Ivan Ilinov)
// C Maunder     - 29 Aug 2000 - operator= checks for NULL font before setting (Martin Richter)
// C Maunder     - 15 Oct 2000 - GetTextExtent fixed (Martin Richter)
// C Maunder     -  1 Jan 2001 - Added ValidateEdit
// Yogurt        - 13 Mar 2004 - GetCellExtent fixed
//
// NOTES: Each grid cell should take care of it's own drawing, though the Draw()
//        method takes an "erase background" paramter that is called if the grid
//        decides to draw the entire grid background in on hit. Certain ambient
//        properties such as the default font to use, and hints on how to draw
//        fixed cells should be fetched from the parent grid. The grid trusts the
//        cells will behave in a certain way, and the cells trust the grid will
//        supply accurate information.
//        
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GridCtrl.h"
#include "GridCellBase.h"
#include "../VtOrder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CGridCellBase, CObject)

/////////////////////////////////////////////////////////////////////////////
// GridCellBase

constexpr int Round(float x) { return static_cast<int>(x + 0.5f); }

CGridCellBase::CGridCellBase()
{
    Reset();
}

CGridCellBase::~CGridCellBase()
{
}

/////////////////////////////////////////////////////////////////////////////
// GridCellBase Operations

void CGridCellBase::Reset()
{
    m_nState  = 0;
}

void CGridCellBase::operator=(const CGridCellBase& cell)
{
	if (this == &cell) return;

    SetGrid(cell.GetGrid());    // do first in case of dependencies

    SetText(cell.GetText());
    SetImage(cell.GetImage());
    SetData(cell.GetData());
    SetState(cell.GetState());
    SetFormat(cell.GetFormat());
    SetTextClr(cell.GetTextClr());
    SetBackClr(cell.GetBackClr());
    SetFont(cell.IsDefaultFont()? NULL : cell.GetFont());
    SetMargin(cell.GetMargin());
}

/////////////////////////////////////////////////////////////////////////////
// CGridCellBase Attributes

// Returns a pointer to a cell that holds default values for this particular type of cell
CGridCellBase* CGridCellBase::GetDefaultCell() const
{
    if (GetGrid())
        return GetGrid()->GetDefaultCell(IsFixedRow(), IsFixedCol());
    return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CGridCellBase Operations

// EFW - Various changes to make it draw cells better when using alternate
// color schemes.  Also removed printing references as that's now done
// by PrintCell() and fixed the sort marker so that it doesn't draw out
// of bounds.
BOOL CGridCellBase::Draw(CDC* pDC, int nRow, int nCol, CRect rect,  BOOL bEraseBkgnd /*=TRUE*/)
{
    // Note - all through this function we totally brutalise 'rect'. Do not
    // depend on it's value being that which was passed in.

    CGridCtrl* pGrid = GetGrid();
    ASSERT(pGrid);

	CRect closeRect, borderRect, btnRect, moveRect, posiRect;
	closeRect.CopyRect(&rect);
	borderRect.CopyRect(&rect);
	btnRect.CopyRect(&rect);
	moveRect.CopyRect(&rect);
	posiRect.CopyRect(&rect);

    if (!pGrid || !pDC)
        return FALSE;

    if( rect.Width() <= 0 || rect.Height() <= 0)  // prevents imagelist item from drawing even
        return FALSE;                             //  though cell is hidden

    //TRACE3("Drawing %scell %d, %d\n", IsFixed()? _T("Fixed ") : _T(""), nRow, nCol);

    int nSavedDC = pDC->SaveDC();
    pDC->SetBkMode(TRANSPARENT);

    // Get the default cell implementation for this kind of cell. We use it if this cell
    // has anything marked as "default"
    CGridDefaultCell *pDefaultCell = (CGridDefaultCell*) GetDefaultCell();
    if (!pDefaultCell)
        return FALSE;

    // Set up text and background colours
    COLORREF TextClr, TextBkClr;

    TextClr = (GetTextClr() == CLR_DEFAULT)? pDefaultCell->GetTextClr() : GetTextClr();
    if (GetBackClr() == CLR_DEFAULT)
        TextBkClr = pDefaultCell->GetBackClr();
    else
    {
        bEraseBkgnd = TRUE;
        TextBkClr = GetBackClr();
    }

	/*
    // Draw cell background and highlighting (if necessary)
    if ( IsFocused() || IsDropHighlighted() )
    {
        // Always draw even in list mode so that we can tell where the
        // cursor is at.  Use the highlight colors though.
        if(GetState() & GVIS_SELECTED)
        {
            TextBkClr = ::GetSysColor(COLOR_HIGHLIGHT);
            TextClr = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
            bEraseBkgnd = TRUE;
        }

        rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
        if (bEraseBkgnd)
        {
            TRY 
            {
                CBrush brush(TextBkClr);
                pDC->FillRect(rect, &brush);
            } 
            CATCH(CResourceException, e)
            {
                //e->ReportError();
            }
            END_CATCH
        }

        // Don't adjust frame rect if no grid lines so that the
        // whole cell is enclosed.
        if(pGrid->GetGridLines() != GVL_NONE)
        {
            rect.right--;
            rect.bottom--;
        }

        if (pGrid->GetFrameFocusCell())
        {
                // Use same color as text to outline the cell so that it shows
                // up if the background is black.
            TRY 
            {
                CBrush brush(TextClr);
				// 여기서 선택된 사각형의 테두리를 그린다.
                pDC->FrameRect(rect, &brush);
            }
            CATCH(CResourceException, e)
            {
                //e->ReportError();
            }
            END_CATCH
        }
        pDC->SetTextColor(TextClr);

        // Adjust rect after frame draw if no grid lines
        if(pGrid->GetGridLines() == GVL_NONE)
        {
            rect.right--;
            rect.bottom--;
        }

		//rect.DeflateRect(0,1,1,1);  - Removed by Yogurt
    }
    else if ((GetState() & GVIS_SELECTED))
    {
        rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
        pDC->FillSolidRect(rect, ::GetSysColor(COLOR_HIGHLIGHT));
        rect.right--; rect.bottom--;
        pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
    }
    else
	*/
    {
        if (bEraseBkgnd)
        {
			rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
		    CBrush brush(TextBkClr);
            pDC->FillRect(rect, &brush);
            rect.right--; rect.bottom--;

			/*
			CPen lightpen(PS_SOLID, 1, RGB(255, 0, 0)),
				darkpen(PS_SOLID, 1, RGB(0, 0, 255)),
				*pOldPen = pDC->GetCurrentPen();

			pDC->SelectObject(&lightpen);
			pDC->MoveTo(borderRect.right, borderRect.top);
			pDC->LineTo(borderRect.left, borderRect.top);
			pDC->LineTo(borderRect.left, borderRect.bottom);

			pDC->SelectObject(&darkpen);
			pDC->MoveTo(borderRect.right, borderRect.top);
			pDC->LineTo(borderRect.right, borderRect.bottom);
			pDC->LineTo(borderRect.left, borderRect.bottom);
			pDC->SelectObject(pOldPen);
			*/
        }
        pDC->SetTextColor(TextClr);
    }

    // Draw lines only when wanted
    if (IsFixed() && pGrid->GetGridLines() != GVL_NONE)
    {
        CCellID FocusCell = pGrid->GetFocusCell();

        // As above, always show current location even in list mode so
        // that we know where the cursor is at.
        BOOL bHiliteFixed = pGrid->GetTrackFocusCell() && pGrid->IsValid(FocusCell) &&
                            (FocusCell.row == nRow || FocusCell.col == nCol);

        // If this fixed cell is on the same row/col as the focus cell,
        // highlight it.
        if (bHiliteFixed)
        {
            rect.right++; rect.bottom++;
            //pDC->DrawEdge(rect, BDR_SUNKENINNER /*EDGE_RAISED*/, BF_RECT);
            rect.DeflateRect(1,1);
        }
        else
        {
            CPen lightpen(PS_SOLID, 1,  ::GetSysColor(COLOR_3DHIGHLIGHT)),
                darkpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DDKSHADOW)),
                *pOldPen = pDC->GetCurrentPen();

            pDC->SelectObject(&lightpen);
            pDC->MoveTo(rect.right, rect.top);
            pDC->LineTo(rect.left, rect.top);
            pDC->LineTo(rect.left, rect.bottom);

            pDC->SelectObject(&darkpen);
            pDC->MoveTo(rect.right, rect.top);
            pDC->LineTo(rect.right, rect.bottom);
            pDC->LineTo(rect.left, rect.bottom);
            pDC->SelectObject(pOldPen);
            rect.DeflateRect(1,1);
        }
    }

    // Draw Text and image
#if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
    if (!pDC->m_bPrinting)
#endif
    {
        CFont *pFont = GetFontObject();
		ASSERT(pFont);
        if (pFont)
            pDC->SelectObject(pFont);
    }

    //rect.DeflateRect(GetMargin(), 0); - changed by Yogurt
    rect.DeflateRect(GetMargin(), GetMargin());    
    rect.right++;    
    rect.bottom++;

    if (pGrid->GetImageList() && GetImage() >= 0)
    {
        IMAGEINFO Info;
        if (pGrid->GetImageList()->GetImageInfo(GetImage(), &Info))
        {
            //  would like to use a clipping region but seems to have issue
            //  working with CMemDC directly.  Instead, don't display image
            //  if any part of it cut-off
            //
            // CRgn rgn;
            // rgn.CreateRectRgnIndirect(rect);
            // pDC->SelectClipRgn(&rgn);
            // rgn.DeleteObject();

            /*
            // removed by Yogurt
            int nImageWidth = Info.rcImage.right-Info.rcImage.left+1;
            int nImageHeight = Info.rcImage.bottom-Info.rcImage.top+1;
            if( nImageWidth + rect.left <= rect.right + (int)(2*GetMargin())
                && nImageHeight + rect.top <= rect.bottom + (int)(2*GetMargin())  )
            {
                pGrid->GetImageList()->Draw(pDC, GetImage(), rect.TopLeft(), ILD_NORMAL);
            }
            */
            // Added by Yogurt
            int nImageWidth = Info.rcImage.right-Info.rcImage.left;            
            int nImageHeight = Info.rcImage.bottom-Info.rcImage.top;            
            if ((nImageWidth + rect.left <= rect.right) && (nImageHeight + rect.top <= rect.bottom))                
                pGrid->GetImageList()->Draw(pDC, GetImage(), rect.TopLeft(), ILD_NORMAL);

            //rect.left += nImageWidth+GetMargin();
        }
    }

    // Draw sort arrow
    if (pGrid->GetSortColumn() == nCol && nRow == 0)
    {
        CSize size = pDC->GetTextExtent(_T("M"));
        int nOffset = 2;

        // Base the size of the triangle on the smaller of the column
        // height or text height with a slight offset top and bottom.
        // Otherwise, it can get drawn outside the bounds of the cell.
        size.cy -= (nOffset * 2);

        if (size.cy >= rect.Height())
            size.cy = rect.Height() - (nOffset * 2);

        size.cx = size.cy;      // Make the dimensions square

        // Kludge for vertical text
        BOOL bVertical = (GetFont()->lfEscapement == 900);

        // Only draw if it'll fit!
        //if (size.cx + rect.left < rect.right + (int)(2*GetMargin())) - changed / Yogurt
        if (size.cx + rect.left < rect.right)
        {
            int nTriangleBase = rect.bottom - nOffset - size.cy;    // Triangle bottom right
            //int nTriangleBase = (rect.top + rect.bottom - size.cy)/2; // Triangle middle right
            //int nTriangleBase = rect.top + nOffset;                 // Triangle top right

            //int nTriangleLeft = rect.right - size.cx;                 // Triangle RHS
            //int nTriangleLeft = (rect.right + rect.left - size.cx)/2; // Triangle middle
            //int nTriangleLeft = rect.left;                            // Triangle LHS

            int nTriangleLeft;
            if (bVertical)
                nTriangleLeft = (rect.right + rect.left - size.cx)/2; // Triangle middle
            else
                nTriangleLeft = rect.right - size.cx;               // Triangle RHS

            CPen penShadow(PS_SOLID, 0, ::GetSysColor(COLOR_3DSHADOW));
            CPen penLight(PS_SOLID, 0, ::GetSysColor(COLOR_3DHILIGHT));
            if (pGrid->GetSortAscending())
            {
                // Draw triangle pointing upwards
                CPen *pOldPen = (CPen*) pDC->SelectObject(&penLight);
                pDC->MoveTo( nTriangleLeft + 1, nTriangleBase + size.cy + 1);
                pDC->LineTo( nTriangleLeft + (size.cx / 2) + 1, nTriangleBase + 1 );
                pDC->LineTo( nTriangleLeft + size.cx + 1, nTriangleBase + size.cy + 1);
                pDC->LineTo( nTriangleLeft + 1, nTriangleBase + size.cy + 1);

                pDC->SelectObject(&penShadow);
                pDC->MoveTo( nTriangleLeft, nTriangleBase + size.cy );
                pDC->LineTo( nTriangleLeft + (size.cx / 2), nTriangleBase );
                pDC->LineTo( nTriangleLeft + size.cx, nTriangleBase + size.cy );
                pDC->LineTo( nTriangleLeft, nTriangleBase + size.cy );
                pDC->SelectObject(pOldPen);
            }
            else
            {
                // Draw triangle pointing downwards
                CPen *pOldPen = (CPen*) pDC->SelectObject(&penLight);
                pDC->MoveTo( nTriangleLeft + 1, nTriangleBase + 1 );
                pDC->LineTo( nTriangleLeft + (size.cx / 2) + 1, nTriangleBase + size.cy + 1 );
                pDC->LineTo( nTriangleLeft + size.cx + 1, nTriangleBase + 1 );
                pDC->LineTo( nTriangleLeft + 1, nTriangleBase + 1 );
    
                pDC->SelectObject(&penShadow);
                pDC->MoveTo( nTriangleLeft, nTriangleBase );
                pDC->LineTo( nTriangleLeft + (size.cx / 2), nTriangleBase + size.cy );
                pDC->LineTo( nTriangleLeft + size.cx, nTriangleBase );
                pDC->LineTo( nTriangleLeft, nTriangleBase );
                pDC->SelectObject(pOldPen);
            }
            
            if (!bVertical)
                rect.right -= size.cy;
        }
    }

	DrawOrder(pDC, rect);

	// 셀 경계선을 그린다. 고정 셀이 아닌경우만 그린다.
	if (!IsFixed())
	{
		CPen lightpen(PS_SOLID, 1, ::GetSysColor(COLOR_3DHIGHLIGHT)),
			darkpen(PS_SOLID, 1, ::GetSysColor(COLOR_3DDKSHADOW)),
			borderPen(PS_SOLID, 1, RGB(192, 192, 192)),
			*pOldPen = pDC->GetCurrentPen();

		// 버튼 셀 그리기
		if (_Style == 2) {
			pDC->SelectObject(&borderPen);
			pDC->MoveTo(borderRect.right, borderRect.top);
			pDC->LineTo(borderRect.left, borderRect.top);
			pDC->LineTo(borderRect.left, borderRect.bottom + 1);

			pDC->SelectObject(&lightpen);
			btnRect.left++;
			btnRect.top++;
		
			pDC->MoveTo(btnRect.right, btnRect.top);
			pDC->LineTo(btnRect.left, btnRect.top);
			pDC->LineTo(btnRect.left, btnRect.bottom);

			pDC->SelectObject(&darkpen);
			pDC->MoveTo(btnRect.right, btnRect.top);
			pDC->LineTo(btnRect.right, btnRect.bottom);
			pDC->LineTo(btnRect.left, btnRect.bottom);
		}
		else { // 일반 셀 그리기
			pDC->SelectObject(&borderPen);
			pDC->MoveTo(borderRect.right, borderRect.top);
			pDC->LineTo(borderRect.left, borderRect.top);
			pDC->LineTo(borderRect.left, borderRect.bottom + 1);
		}
		pDC->SelectObject(pOldPen);
	}

	// 선택된 사각형을 그린다.
	if (_Clicked)
		DrawSelectedgRect(pDC, moveRect);

	// 호가선 표시
	if (_Style == 1)
	{
		CPen pen, *oldPen = NULL;
		pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

		oldPen = pDC->SelectObject(&pen);

		CPoint pt1, pt2;
		pt1.x = closeRect.left - 1;
		pt1.y = closeRect.bottom;
		pt2.x = closeRect.right + 1;
		pt2.y = closeRect.bottom;

		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);
		pDC->SelectObject(oldPen);
	}

	// 움직이는 사각형을 그린다.
	if (_MovingRect)
		DrawMovingRect(pDC, moveRect);

	// We want to see '&' characters so use DT_NOPREFIX
    GetTextRect(rect);
    rect.right++;    
    rect.bottom++;


    DrawText(pDC->m_hDC, GetText(), -1, rect, GetFormat() | DT_NOPREFIX);

	// 레이블 표시
	{
		CRect tempRect(closeRect);
		tempRect.top = tempRect.top + 4;
		tempRect.left = tempRect.right - 10;
		pDC->ExtTextOut(tempRect.left, tempRect.top, ETO_CLIPPED, tempRect, GetLabel(), NULL);
	}

	// 포지션 표시
	if (_Position == 1 || _Position == 2)
	{
		CRect tempRect;
		int halfY = (posiRect.bottom - posiRect.top) / 2;
		CBrush brush1;   // Must initialize!
		CBrush* pTempBrush = NULL;
		CBrush OrigBrush;
		if (_Position == 1)
		{
			brush1.CreateSolidBrush(RGB(255, 0, 0));

			tempRect.top = posiRect.top + halfY - 5;
			tempRect.left = posiRect.right - 17;
			tempRect.right = posiRect.right - 4;
			tempRect.bottom = posiRect.bottom - halfY + 7;
		}
		else if (_Position == 2)
		{
			brush1.CreateSolidBrush(RGB(0, 0, 255));

			tempRect.top = posiRect.top + halfY - 5;
			tempRect.left = posiRect.left + 4;
			tempRect.right = posiRect.left + 17;
			tempRect.bottom = posiRect.bottom - halfY + 7;
		}
		
		tempRect.DeflateRect(1, 1);
		pTempBrush = (CBrush*)pDC->SelectObject(&brush1);
		// Save original brush.
		OrigBrush.FromHandle((HBRUSH)pTempBrush);

		pDC->SelectObject(&brush1);
		pDC->Ellipse(tempRect);
		pDC->SetBkMode(OPAQUE);
		pDC->SelectObject(&OrigBrush);
	}

    pDC->RestoreDC(nSavedDC);

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGridCellBase Mouse and Cursor events

// Not yet implemented
void CGridCellBase::OnMouseEnter()
{
    TRACE0("Mouse entered cell\n");
}

void CGridCellBase::OnMouseOver()
{
    //TRACE0("Mouse over cell\n");
}

// Not Yet Implemented
void CGridCellBase::OnMouseLeave()
{
    TRACE0("Mouse left cell\n");
}

void CGridCellBase::OnClick( CPoint PointCellRelative)
{
    UNUSED_ALWAYS(PointCellRelative);
    TRACE2("Mouse Left btn up in cell at x=%i y=%i\n", PointCellRelative.x, PointCellRelative.y);
	CString msg;
	msg.Format("click row = %d, col = %d", 4, 4);
	//AfxMessageBox(msg);
}

void CGridCellBase::OnClickDown( CPoint PointCellRelative)
{
    UNUSED_ALWAYS(PointCellRelative);
    TRACE2("Mouse Left btn down in cell at x=%i y=%i\n", PointCellRelative.x, PointCellRelative.y);
}

void CGridCellBase::OnRClick( CPoint PointCellRelative)
{
    UNUSED_ALWAYS(PointCellRelative);
    TRACE2("Mouse right-clicked in cell at x=%i y=%i\n", PointCellRelative.x, PointCellRelative.y);
}

void CGridCellBase::OnDblClick( CPoint PointCellRelative)
{
    UNUSED_ALWAYS(PointCellRelative);
    TRACE2("Mouse double-clicked in cell at x=%i y=%i\n", PointCellRelative.x, PointCellRelative.y);
}

// Return TRUE if you set the cursor
BOOL CGridCellBase::OnSetCursor()
{
#ifndef _WIN32_WCE_NO_CURSOR
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
#endif
    return TRUE;
}

void CGridCellBase::AddOrder(VtOrder* order)
{
	if (!order)
		return;
	OrderMap[order->orderNo] = order;
}

void CGridCellBase::RemoveOrder(int orderNo)
{
	auto it = OrderMap.find(orderNo);
	if (it != OrderMap.end()) {
		OrderMap.erase(it);
	}
}

void CGridCellBase::ClearOrders()
{
	OrderMap.clear();
}

int CGridCellBase::GetOrderCount()
{
	int cellOrderCnt = 0;
	for (auto it = OrderMap.begin(); it != OrderMap.end(); ++it)
		cellOrderCnt += it->second->amount;
	return cellOrderCnt;
}

void CGridCellBase::AddStopOrder(HdOrderRequest* order)
{
	if (!order)
		return;
	StopOrderMap[order->RequestId] = order;
}

void CGridCellBase::RemoveStopOrder(int reqNo)
{
	auto it = StopOrderMap.find(reqNo);
	if (it != StopOrderMap.end()) {
		StopOrderMap.erase(it);
	}
}

void CGridCellBase::ClearStopOrders()
{
	StopOrderMap.clear();
}

int CGridCellBase::GetStopOrderCount()
{
	int cellOrderCnt = 0;
	for (auto it = StopOrderMap.begin(); it != StopOrderMap.end(); ++it)
		cellOrderCnt += it->second->Amount;
	return cellOrderCnt;
}

void CGridCellBase::DrawOrder(CDC* pDC, CRect& rect)
{
	// 여기서 주문선을 그린다.
	if (OrderMap.size() > 0) {

		RECT rectout;
		CopyRect(&rectout, rect);
		CPoint pt1;
		CPoint pt2;

		CPen pen;
		CBrush brush1;   // Must initialize!
		CPen* pOldPen = NULL;
		CBrush* pTempBrush = NULL;
		CBrush OrigBrush;
		VtOrder* order = OrderMap.begin()->second;
		if (order->orderPosition == VtPositionType::Sell) {
			pt1.x = rectout.left + (rectout.right - rectout.left) / 2;;
			pt1.y = rectout.top + (rectout.bottom - rectout.top) / 2;
			pt2.x = rectout.right;
			pt2.y = rectout.top + (rectout.bottom - rectout.top) / 2;


			brush1.CreateSolidBrush(RGB(0, 0, 255));   // Blue brush.
			pen.CreatePen(0, 1, RGB(0, 0, 255));
			pTempBrush = (CBrush*)pDC->SelectObject(&brush1);
			pOldPen = pDC->SelectObject(&pen);
			// Save original brush.
			OrigBrush.FromHandle((HBRUSH)pTempBrush);
		}
		else if (order->orderPosition == VtPositionType::Buy)
		{

			pt1.x = rectout.right - (rectout.right - rectout.left) / 2;
			pt1.y = rectout.top + (rectout.bottom - rectout.top) / 2;
			pt2.x = rectout.left;
			pt2.y = rectout.top + (rectout.bottom - rectout.top) / 2;


			brush1.CreateSolidBrush(RGB(255, 0, 0));   // Blue brush.
			pen.CreatePen(0, 1, RGB(255, 0, 0));
			pTempBrush = (CBrush*)pDC->SelectObject(&brush1);
			pOldPen = pDC->SelectObject(&pen);
			// Save original brush.
			OrigBrush.FromHandle((HBRUSH)pTempBrush);
		}


		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);
		int head_length = 6, head_width = 6;

		const float dx = static_cast<float>(pt2.x - pt1.x);
		const float dy = static_cast<float>(pt2.y - pt1.y);
		const auto length = std::sqrt(dx*dx + dy*dy);

		// ux,uy is a unit vector parallel to the line.
		const auto ux = dx / length;
		const auto uy = dy / length;

		// vx,vy is a unit vector perpendicular to ux,uy
		const auto vx = -uy;
		const auto vy = ux;

		const auto half_width = 0.5f * head_width;

		const POINT arrow[3] =
		{ pt2,
			POINT{ Round(pt2.x - head_length*ux + half_width*vx),
			Round(pt2.y - head_length*uy + half_width*vy) },
			POINT{ Round(pt2.x - head_length*ux - half_width*vx),
			Round(pt2.y - head_length*uy - half_width*vy) }
		};
		pDC->Polygon(arrow, 3);

		pDC->SelectObject(pOldPen);

		// Reselect original brush into device context.
		pDC->SelectObject(&OrigBrush);
	}
}

void CGridCellBase::DrawMovingRect(CDC* pDC, CRect& rect)
{
	rect.DeflateRect(1, 1);
	CPen pen;
	CPen* pOldPen = NULL;
	pen.CreatePen(0, 1, RGB(255, 0, 0));
	pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.right + 1, rect.top);
	pDC->LineTo(rect.right + 1, rect.bottom + 1);
	pDC->LineTo(rect.left, rect.bottom + 1);
	pDC->LineTo(rect.left, rect.top);

	pDC->SelectObject(pOldPen);
}

void CGridCellBase::DrawSelectedgRect(CDC* pDC, CRect& rect)
{
	rect.DeflateRect(1, 1);
	CPen pen;
	CPen* pOldPen = NULL;
	pen.CreatePen(0, 2, RGB(0, 102, 51));
	pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.right + 1, rect.top);
	pDC->LineTo(rect.right + 1, rect.bottom + 1);
	pDC->LineTo(rect.left, rect.bottom + 1);
	pDC->LineTo(rect.left, rect.top);

	pDC->SelectObject(pOldPen);
}

/////////////////////////////////////////////////////////////////////////////
// CGridCellBase editing

void CGridCellBase::OnEndEdit() 
{
	ASSERT( FALSE); 
}

BOOL CGridCellBase::ValidateEdit(LPCTSTR str)
{
    UNUSED_ALWAYS(str);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGridCellBase Sizing

BOOL CGridCellBase::GetTextRect( LPRECT pRect)  // i/o:  i=dims of cell rect; o=dims of text rect
{
    if (GetImage() >= 0)
    {
        IMAGEINFO Info;

        CGridCtrl* pGrid = GetGrid();
        CImageList* pImageList = pGrid->GetImageList();
        
        if (pImageList && pImageList->GetImageInfo( GetImage(), &Info))
        {
            int nImageWidth = Info.rcImage.right-Info.rcImage.left+1;
            pRect->left += nImageWidth + GetMargin();
        }
    }

	int order_gap = 3;
	if (OrderMap.size() > 0) {
		VtOrder* order = OrderMap.begin()->second;
		if (order->orderPosition == VtPositionType::Sell) {
			pRect->left += order_gap;
		}
		else if (order->orderPosition == VtPositionType::Buy) {
			pRect->right -= order_gap;
		}
	}

    return TRUE;
}

// By default this uses the selected font (which is a bigger font)
CSize CGridCellBase::GetTextExtent(LPCTSTR szText, CDC* pDC /*= NULL*/)
{
    CGridCtrl* pGrid = GetGrid();
    ASSERT(pGrid);

    BOOL bReleaseDC = FALSE;
    if (pDC == NULL || szText == NULL)
    {
        if (szText)
			pDC = pGrid->GetDC();
        if (pDC == NULL || szText == NULL) 
        {
            CGridDefaultCell* pDefCell = (CGridDefaultCell*) GetDefaultCell();
            ASSERT(pDefCell);
            return CSize(pDefCell->GetWidth(), pDefCell->GetHeight());
        }
        bReleaseDC = TRUE;
    }

    CFont *pOldFont = NULL,
          *pFont = GetFontObject();
    if (pFont)
        pOldFont = pDC->SelectObject(pFont);

    CSize size;
    int nFormat = GetFormat();

    // If the cell is a multiline cell, then use the width of the cell
    // to get the height
    if ((nFormat & DT_WORDBREAK) && !(nFormat & DT_SINGLELINE))
    {
        CString str = szText;
        int nMaxWidth = 0;
        while (TRUE)
        {
            int nPos = str.Find(_T('\n'));
            CString TempStr = (nPos < 0)? str : str.Left(nPos);
            int nTempWidth = pDC->GetTextExtent(TempStr).cx;
            if (nTempWidth > nMaxWidth)
                nMaxWidth = nTempWidth;

            if (nPos < 0)
                break;
            str = str.Mid(nPos + 1);    // Bug fix by Thomas Steinborn
        }
        
        CRect rect;
        rect.SetRect(0,0, nMaxWidth+1, 0);
        pDC->DrawText(szText, -1, rect, nFormat | DT_CALCRECT);
        size = rect.Size();
    }
    else
        size = pDC->GetTextExtent(szText, (int)_tcslen(szText));

    // Removed by Yogurt
    //TEXTMETRIC tm;
    //pDC->GetTextMetrics(&tm);
    //size.cx += (tm.tmOverhang);

    if (pOldFont)
        pDC->SelectObject(pOldFont);
    
    size += CSize(2*GetMargin(), 2*GetMargin());

    // Kludge for vertical text
    LOGFONT *pLF = GetFont();
    if (pLF->lfEscapement == 900 || pLF->lfEscapement == -900)
    {
        int nTemp = size.cx;
        size.cx = size.cy;
        size.cy = nTemp;
        size += CSize(0, 4*GetMargin());
    }
    
    if (bReleaseDC)
        pGrid->ReleaseDC(pDC);

    return size;
}


CSize CGridCellBase::GetCellExtent(CDC* pDC)
{    
    CSize size = GetTextExtent(GetText(), pDC);    
    CSize ImageSize(0,0);    
    
    int nImage = GetImage();    
    if (nImage >= 0)    
    {        
        CGridCtrl* pGrid = GetGrid();        
        ASSERT(pGrid);        
        IMAGEINFO Info;        
        if (pGrid->GetImageList() && pGrid->GetImageList()->GetImageInfo(nImage, &Info))         
        {            
            ImageSize = CSize(Info.rcImage.right-Info.rcImage.left,                                 
                Info.rcImage.bottom-Info.rcImage.top);            
            if (size.cx > 2*(int)GetMargin ())                
                ImageSize.cx += GetMargin();            
            ImageSize.cy += 2*(int)GetMargin ();        
        }    
    }    
    size.cx += ImageSize.cx + 1;    
    size.cy = max(size.cy, ImageSize.cy) + 1;    
    if (IsFixed())    
    {        
        size.cx++;        
        size.cy++;    
    }    
    return size;
}

// EFW - Added to print cells so that grids that use different colors are
// printed correctly.
BOOL CGridCellBase::PrintCell(CDC* pDC, int /*nRow*/, int /*nCol*/, CRect rect)
{
#if defined(_WIN32_WCE_NO_PRINTING) || defined(GRIDCONTROL_NO_PRINTING)
    return FALSE;
#else
    COLORREF crFG, crBG;
    GV_ITEM Item;

    CGridCtrl* pGrid = GetGrid();
    if (!pGrid || !pDC)
        return FALSE;

    if( rect.Width() <= 0
        || rect.Height() <= 0)  // prevents imagelist item from drawing even
        return FALSE;           //  though cell is hidden

    int nSavedDC = pDC->SaveDC();

    pDC->SetBkMode(TRANSPARENT);

    if (pGrid->GetShadedPrintOut())
    {
        // Get the default cell implementation for this kind of cell. We use it if this cell
        // has anything marked as "default"
        CGridDefaultCell *pDefaultCell = (CGridDefaultCell*) GetDefaultCell();
        if (!pDefaultCell)
            return FALSE;

        // Use custom color if it doesn't match the default color and the
        // default grid background color.  If not, leave it alone.
        if(IsFixed())
            crBG = (GetBackClr() != CLR_DEFAULT) ? GetBackClr() : pDefaultCell->GetBackClr();
        else
            crBG = (GetBackClr() != CLR_DEFAULT && GetBackClr() != pDefaultCell->GetBackClr()) ?
                GetBackClr() : CLR_DEFAULT;

        // Use custom color if the background is different or if it doesn't
        // match the default color and the default grid text color.  
        if(IsFixed())
            crFG = (GetBackClr() != CLR_DEFAULT) ? GetTextClr() : pDefaultCell->GetTextClr();
        else
            crFG = (GetBackClr() != CLR_DEFAULT) ? GetTextClr() : pDefaultCell->GetTextClr();

        // If not printing on a color printer, adjust the foreground color
        // to a gray scale if the background color isn't used so that all
        // colors will be visible.  If not, some colors turn to solid black
        // or white when printed and may not show up.  This may be caused by
        // coarse dithering by the printer driver too (see image note below).
        if(pDC->GetDeviceCaps(NUMCOLORS) == 2 && crBG == CLR_DEFAULT)
            crFG = RGB(GetRValue(crFG) * 0.30, GetGValue(crFG) * 0.59,
                GetBValue(crFG) * 0.11);

        // Only erase the background if the color is not the default
        // grid background color.
        if(crBG != CLR_DEFAULT)
        {
            CBrush brush(crBG);
            rect.right++; rect.bottom++;
            pDC->FillRect(rect, &brush);
            rect.right--; rect.bottom--;
        }
    }
    else
    {
        crBG = CLR_DEFAULT;
        crFG = RGB(0, 0, 0);
    }

    pDC->SetTextColor(crFG);

    CFont *pFont = GetFontObject();
    if (pFont)
        pDC->SelectObject(pFont);

    /*
    // ***************************************************
    // Disabled - if you need this functionality then you'll need to rewrite.
    // Create the appropriate font and select into DC.
    CFont Font;
    // Bold the fixed cells if not shading the print out.  Use italic
    // font it it is enabled.
    const LOGFONT* plfFont = GetFont();
    if(IsFixed() && !pGrid->GetShadedPrintOut())
    {
        Font.CreateFont(plfFont->lfHeight, 0, 0, 0, FW_BOLD, plfFont->lfItalic, 0, 0,
            ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
#ifndef _WIN32_WCE
            PROOF_QUALITY,
#else
            DEFAULT_QUALITY,
#endif
            VARIABLE_PITCH | FF_SWISS, plfFont->lfFaceName);
    }
    else
        Font.CreateFontIndirect(plfFont);

    pDC->SelectObject(&Font);
    // ***************************************************
    */

    // Draw lines only when wanted on fixed cells.  Normal cell grid lines
    // are handled in OnPrint.
    if(pGrid->GetGridLines() != GVL_NONE && IsFixed())
    {
        CPen lightpen(PS_SOLID, 1,  ::GetSysColor(COLOR_3DHIGHLIGHT)),
             darkpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DDKSHADOW)),
            *pOldPen = pDC->GetCurrentPen();

        pDC->SelectObject(&lightpen);
        pDC->MoveTo(rect.right, rect.top);
        pDC->LineTo(rect.left, rect.top);
        pDC->LineTo(rect.left, rect.bottom);

        pDC->SelectObject(&darkpen);
        pDC->MoveTo(rect.right, rect.top);
        pDC->LineTo(rect.right, rect.bottom);
        pDC->LineTo(rect.left, rect.bottom);

        rect.DeflateRect(1,1);
        pDC->SelectObject(pOldPen);
    }

    rect.DeflateRect(GetMargin(), 0);

    if(pGrid->GetImageList() && GetImage() >= 0)
    {
        // NOTE: If your printed images look like fuzzy garbage, check the
        //       settings on your printer driver.  If it's using coarse
        //       dithering and/or vector graphics, they may print wrong.
        //       Changing to fine dithering and raster graphics makes them
        //       print properly.  My HP 4L had that problem.

        IMAGEINFO Info;
        if(pGrid->GetImageList()->GetImageInfo(GetImage(), &Info))
        {
            int nImageWidth = Info.rcImage.right-Info.rcImage.left;
            pGrid->GetImageList()->Draw(pDC, GetImage(), rect.TopLeft(), ILD_NORMAL);
            rect.left += nImageWidth+GetMargin();
        }
    }

    // Draw without clipping so as not to lose text when printed for real
	// DT_NOCLIP removed 01.01.01. Slower, but who cares - we are printing!
    DrawText(pDC->m_hDC, GetText(), -1, rect,
        GetFormat() | /*DT_NOCLIP | */ DT_NOPREFIX);

    pDC->RestoreDC(nSavedDC);

    return TRUE;
#endif
}

/*****************************************************************************
Callable by derived classes, only
*****************************************************************************/
LRESULT CGridCellBase::SendMessageToParent(int nRow, int nCol, int nMessage)
{
    CGridCtrl* pGrid = GetGrid();
    if( pGrid)
        return pGrid->SendMessageToParent(nRow, nCol, nMessage);
    else
        return 0;
}