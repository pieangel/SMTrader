#pragma once


class UG_CLASS_DECL CUGCTRemain : public CUGCellType
{
protected:

	int	  m_fontHeight;
	CFont m_labelFont;
	COLORREF m_labelColor;

public:

	CUGCTRemain();
	~CUGCTRemain();

	//overloaded CUGCellType functions
	virtual LPCTSTR GetName();
	virtual LPCUGID GetUGID();
	virtual int GetEditArea(RECT *rect);
	virtual void OnDraw(CDC *dc, RECT *rect, int col, long row, CUGCell *cell, int selected, int current);
	virtual void GetBestSize(CDC *dc, CSize *size, CUGCell *cell);

	int SetLabelFont(int height, int weight, LPCTSTR name, COLORREF color);
	void DrawOrderArrow(int orderType, CDC* pdc, POINT p0, POINT p1, int head_length, int head_width);
	void DrawFocusedRect(CDC *dc, RECT *rect);
};

