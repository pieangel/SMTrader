#pragma once

#include "VtFundPLGrid.h"
#include "VtFundProductRemainGrid.h"
#include "HdWindowEvent.h"
#include "Poco/BasicEvent.h"
#include "afxwin.h"
#include "SimpleBinStream.h"
#include "Xml/pugixml.hpp"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;

using Poco::BasicEvent;

// VtFundMiniJango dialog
class VtAccount;
class VtSymbol;
struct VtOrder;
class VtFund;

class VtFundMiniJango : public CDialogEx
{
	DECLARE_DYNAMIC(VtFundMiniJango)

public:
	VtFundMiniJango(CWnd* pParent = NULL);   // standard constructor
	virtual ~VtFundMiniJango();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MINI_JANGO_FUND };
#endif

	VtFund* Fund() const { return _Fund; }
	void Fund(VtFund* val) { _Fund = val; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	BasicEvent<HdWindowEventArgs> _WindowEvent;
	void FireWindowEvent(HdWindowEventArgs&& arg)
	{
		_WindowEvent(this, arg);
	}

	VtFundPLGrid _FundPLGrid;
	VtFund* _Fund;
	VtFundProductRemainGrid _ProductGrid;
public:
	void SaveToXml(pugi::xml_node& node);
	void LoadFromXml(pugi::xml_node& node);

	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);

	std::string _DefaultFund;
	void SetDefaultFund();
	void OnFundAdded();
	void OnFundDeleted(VtFund* fund);
	void InitFund();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CComboBox _ComboFund;
	afx_msg void OnCbnSelchangeComboFund();

	void OnReceiveQuote(VtSymbol* sym);
	void OnOrderFilledHd(VtOrder* order);
	void OnSymbolMaster(VtSymbol* sym);
	void OnReceiveAccountInfo();
	afx_msg void OnBnClickedCheckSubacnt();
	afx_msg void OnBnClickedCheckAcnt();
	CButton _CheckMode;
};
