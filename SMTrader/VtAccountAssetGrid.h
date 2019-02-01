#pragma once
#include "Skel/VtGrid.h"
#include <vector>
class VtAccount;
class VtAssetPage;
class VtAccountAssetGrid : public VtGrid
{
public:
	VtAccountAssetGrid();
	virtual ~VtAccountAssetGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);

	void SetRowTitle();
	int _ColCount = 4;
	int _RowCount = 5;
	CFont _defFont;
	CFont _titleFont;
	void QuickRedrawCell(int col, long row);
	VtAssetPage* AssetPage() const { return _AssetPage; }
	void AssetPage(VtAssetPage* val) { _AssetPage = val; }
private:
	VtAssetPage* _AssetPage = nullptr;
	std::vector<int> _ColWidVec;
public:
	void InitGrid();
};

