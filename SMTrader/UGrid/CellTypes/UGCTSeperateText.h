#pragma once

class UG_CLASS_DECL CUGCTSeperateText : public CUGCellType
{
protected:

	int	  m_fontHeight;
	CFont m_labelFont;
	COLORREF m_labelColor;

public:

	CUGCTSeperateText();
	~CUGCTSeperateText();

	//overloaded CUGCellType functions
	virtual LPCTSTR GetName();
	virtual LPCUGID GetUGID();
	virtual int GetEditArea(RECT *rect);
	virtual void OnDraw(CDC *dc, RECT *rect, int col, long row, CUGCell *cell, int selected, int current);
	virtual void GetBestSize(CDC *dc, CSize *size, CUGCell *cell);

	int SetLabelFont(int height, int weight, LPCTSTR name, COLORREF color);
	void SetLabelColor(COLORREF color);
	void DrawFocusedRect(CDC *dc, RECT *rect);
};