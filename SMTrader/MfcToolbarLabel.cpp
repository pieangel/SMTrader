#include "stdafx.h"
#include "MfcToolbarLabel.h"


IMPLEMENT_SERIAL(CToolbarLabel, CMFCToolBarButton, 1)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolbarLabel::CToolbarLabel(UINT nID, LPCTSTR lpszText)
{
	if (lpszText != NULL)
	{
		m_strText = lpszText;
	}

	m_bText = TRUE;
	m_nID = nID;
	m_iImage = -1;
}

void CToolbarLabel::OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages,
	BOOL bHorz, BOOL /*bCustomizeMode*/, BOOL /*bHighlight*/,
	BOOL /*bDrawBorder*/, BOOL /*bGrayDisabledButtons*/)
{
	UINT nStyle = m_nStyle;
	m_nStyle &= ~TBBS_DISABLED;

	CMFCToolBarButton::OnDraw(pDC, rect, pImages, bHorz, FALSE,
		FALSE, FALSE, FALSE);

	m_nStyle = nStyle;
}