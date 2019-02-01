#pragma once
#include "Skel/VtGrid.h"
#include <map>
#include <string>
#include <vector>
#include <tuple>
class VtUsdStrategyConfigDlg;
class VtFund;
class VtAccount;

class VtAccountFundGrid : public VtGrid
{
public:
	VtAccountFundGrid();
	~VtAccountFundGrid();

	virtual void OnSetup();
	virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed);
	virtual void OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);
	virtual void OnRClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed);

	void SetColTitle();
	int _ColCount = 2;
	int _RowCount = 60;
	CFont _defFont;
	CFont _titleFont;
	int _HeadHeight = 18;
	void QuickRedrawCell(int col, long row);
	void InitGrid();
	void ClearCells();
	int Mode() const { return _Mode; }
	void Mode(int val) { _Mode = val; }
	void SetConfigDlg(VtUsdStrategyConfigDlg* ConfigDlg);
private:
	VtUsdStrategyConfigDlg* _ConfigDlg;
	std::vector<int> _ColWidthVec;
	// 0 : ��ü, 1 : ���¸�, 2 : �ݵ常
	int _Mode = 0;
	// key : �� ��ȣ 0 ���� ����, value : Ÿ��(�ǰ���, �������, �ݵ�), �̸�.
	std::map<int, std::tuple<int, VtAccount*, VtFund*>> _RowToNameMap;
	std::vector<int> _HeightVec;
	void ResizeWindow();
};

