#pragma once
class CToolbarLabel : public CMFCToolBarButton
{
	DECLARE_SERIAL(CToolbarLabel)

public:
	CToolbarLabel(UINT nID = 0, LPCTSTR lpszText = NULL);

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages,
		BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
		BOOL bHighlight = FALSE,
		BOOL bDrawBorder = TRUE,
		BOOL bGrayDisabledButtons = TRUE);
};
