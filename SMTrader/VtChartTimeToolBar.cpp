// VtChartTimeToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "VtChartTimeToolBar.h"
#include "afxdialogex.h"
#include "resource.h"
#include "BtnST.h"
#include <string>
#include "VtChartContainer.h"
#include "System/VtSystemManager.h"
#include "System/VtSystem.h"
#include "Market/SmMarketManager.h"
#include "VtSymbol.h"
#include "HdSymbolSelecter.h"

// VtChartTimeToolBar dialog

IMPLEMENT_DYNAMIC(VtChartTimeToolBar, CDialogEx)

VtChartTimeToolBar::VtChartTimeToolBar(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TIME_TOOLBAR, pParent)
{
	_Mode = 3;
	_Container = nullptr;
}

VtChartTimeToolBar::~VtChartTimeToolBar()
{
}

void VtChartTimeToolBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_DAY, _BtnDay);
	DDX_Control(pDX, IDC_BTN_MIN, _BtnMin);
	DDX_Control(pDX, IDC_BTN_MONTH, _BtnMonth);
	DDX_Control(pDX, IDC_BTN_TICK, _BtnTick);
	DDX_Control(pDX, IDC_BTN_WEEK, _BtnWeek);
	DDX_Control(pDX, IDC_COMBO_TICK, _ComboTick);
	DDX_Control(pDX, IDC_COMBO_TIME, _ComboTime);
	DDX_Control(pDX, IDC_STATIC_TIME, _StaticTime);
	DDX_Control(pDX, IDC_COMBO_SYSTEM, _ComboSystem);
	DDX_Control(pDX, IDC_STATIC_SYSTEM, _StaticSystem);
	DDX_Control(pDX, IDC_BUTTON_SEARCH, _SearchSymbol);
	DDX_Control(pDX, IDC_COMBO_SYMBOL, _ComboSymbol);
	DDX_Control(pDX, IDC_COMBO_STYLE, _CombolStyle);
}


BEGIN_MESSAGE_MAP(VtChartTimeToolBar, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_MONTH, &VtChartTimeToolBar::OnBnClickedBtnMonth)
	ON_BN_CLICKED(IDC_BTN_WEEK, &VtChartTimeToolBar::OnBnClickedBtnWeek)
	ON_BN_CLICKED(IDC_BTN_DAY, &VtChartTimeToolBar::OnBnClickedBtnDay)
	ON_BN_CLICKED(IDC_BTN_MIN, &VtChartTimeToolBar::OnBnClickedBtnMin)
	ON_BN_CLICKED(IDC_BTN_TICK, &VtChartTimeToolBar::OnBnClickedBtnTick)
	ON_CBN_SELCHANGE(IDC_COMBO_TIME, &VtChartTimeToolBar::OnCbnSelchangeComboTime)
	ON_CBN_SELCHANGE(IDC_COMBO_SYSTEM, &VtChartTimeToolBar::OnCbnSelchangeComboSystem)
	ON_CBN_SELCHANGE(IDC_COMBO_SYMBOL, &VtChartTimeToolBar::OnCbnSelchangeComboSymbol)
	ON_CBN_SELCHANGE(IDC_COMBO_TICK, &VtChartTimeToolBar::OnCbnSelchangeComboTick)
	ON_CBN_SELCHANGE(IDC_COMBO_STYLE, &VtChartTimeToolBar::OnCbnSelchangeComboStyle)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &VtChartTimeToolBar::OnBnClickedButtonSearch)
END_MESSAGE_MAP()


// VtChartTimeToolBar message handlers


BOOL VtChartTimeToolBar::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	_StaticTime.SetTextColor(RGB(0, 0, 0));
	_StaticTime.SetColor(GetSysColor(COLOR_BTNFACE));
	_StaticTime.SetGradientColor(GetSysColor(COLOR_BTNFACE));
	// Align to center
	_StaticTime.SetTextAlign(1);

	_StaticSystem.SetTextColor(RGB(0, 0, 0));
	_StaticSystem.SetColor(GetSysColor(COLOR_BTNFACE));
	_StaticSystem.SetGradientColor(GetSysColor(COLOR_BTNFACE));
	// Align to center
	_StaticSystem.SetTextAlign(1);

	_ButtonVec.push_back(&_BtnMonth);
	_ButtonVec.push_back(&_BtnWeek);
	_ButtonVec.push_back(&_BtnDay);
	_ButtonVec.push_back(&_BtnMin);
	_ButtonVec.push_back(&_BtnTick);

	std::vector<std::string> _TooltipVec;
	_TooltipVec.push_back(_T("월설정"));
	_TooltipVec.push_back(_T("주설정"));
	_TooltipVec.push_back(_T("일설정"));
	_TooltipVec.push_back(_T("분설정"));
	_TooltipVec.push_back(_T("틱설정"));

	int i = 0;
	for (auto btn : _ButtonVec)
	{
		btn->SetColor(BTNST_COLOR_BK_IN, RGB(255, 67, 0), true);
		btn->SetColor(BTNST_COLOR_FG_IN, RGB(255, 255, 255), true);
		btn->SetColor(BTNST_COLOR_BK_OUT, RGB(255, 255, 255), true);
		btn->SetColor(BTNST_COLOR_FG_OUT, RGB(0, 0, 0), true);
		btn->SetTooltipText(_TooltipVec[i++].c_str());
	}
	for (int i = 1; i < 901; i++)
	{
		_ComboTick.AddString(std::to_string(i).c_str());
	}

	// TODO:  Add extra initialization here
	switch (_Mode)
	{
	case 0: // for month
	{
		for (int i = 1; i < 31; i++)
		{
			_ComboTime.AddString(std::to_string(i).c_str());
		}
		_ComboTime.EnableWindow(TRUE);
		_ComboTick.EnableWindow(FALSE);
	}
		break;
	case 1: // for week
	{
		for (int i = 1; i < 5; i++)
		{
			_ComboTime.AddString(std::to_string(i).c_str());
		}
		_ComboTime.EnableWindow(TRUE);
		_ComboTick.EnableWindow(FALSE);
	}
		break;
	case 2: // day
	{
		for (int i = 1; i < 601; i++)
		{
			_ComboTime.AddString(std::to_string(i).c_str());
		}
		_ComboTime.EnableWindow(TRUE);
		_ComboTick.EnableWindow(FALSE);
	}
		break;
	case 3: // for min
	{
		for (int i = 1; i < 61; i++)
		{
			_ComboTime.AddString(std::to_string(i).c_str());
		}
		_ComboTime.EnableWindow(TRUE);
		_ComboTick.EnableWindow(FALSE);
	}
		break;
	case 4: // for tick
	{
		for (int i = 1; i < 61; i++)
		{
			_ComboTick.AddString(std::to_string(i).c_str());
		}
		_ComboTime.EnableWindow(FALSE);
		_ComboTick.EnableWindow(TRUE);
	}
		break;
	default:
	{
		{
			for (int i = 1; i < 61; i++)
			{
				_ComboTime.AddString(std::to_string(i).c_str());
			}
		}
		_ComboTime.EnableWindow(TRUE);
		_ComboTick.EnableWindow(FALSE);
	}
		break;
	}

	_ButtonVec[_Mode]->SetColor(BTNST_COLOR_BK_IN, RGB(255, 67, 0), true);
	_ButtonVec[_Mode]->SetColor(BTNST_COLOR_FG_IN, RGB(255, 255, 255), true);
	_ButtonVec[_Mode]->SetColor(BTNST_COLOR_BK_OUT, RGB(0, 67, 178), true);
	_ButtonVec[_Mode]->SetColor(BTNST_COLOR_FG_OUT, RGB(255, 255, 255), true);
	_ButtonVec[_Mode]->SetColor(BTNST_COLOR_BK_FOCUS, RGB(0, 67, 178), true);
	_ButtonVec[_Mode]->SetColor(BTNST_COLOR_FG_FOCUS, RGB(255, 255, 255), true);

	_ComboTime.SetCurSel(0);
	_ComboTick.SetCurSel(0);

	std::vector<VtSymbol*> symbolVector = SmMarketManager::GetInstance()->GetRecentMonthSymbolList();
	for (auto it = symbolVector.begin(); it != symbolVector.end(); ++it) {
		VtSymbol* symbol = *it;
		int index = _ComboSymbol.AddString(symbol->ShortCode.c_str());
		_ComboSymbol.SetItemDataPtr(index, (void*)symbol);
	}

	if (symbolVector.size() > 0) {
		_ComboSymbol.SetCurSel(0);
	}

	std::vector<std::string> style_vec;
	style_vec.push_back("CandleStick");
	style_vec.push_back("OHLC");
	style_vec.push_back("ClosePrice");
	style_vec.push_back("TypicalPrice");
	style_vec.push_back("WeightedClose");
	style_vec.push_back("MedianPrice");

	for (size_t i = 0; i < style_vec.size(); ++i) {
		_CombolStyle.AddString(style_vec[i].c_str());
	}
	_CombolStyle.SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void VtChartTimeToolBar::ChangeButtonColor()
{
	for (int i = 0; i < 5; i++) {
		if (i == _Mode){
			_ButtonVec[i]->SetColor(BTNST_COLOR_BK_IN, RGB(255, 67, 0), true);
			_ButtonVec[i]->SetColor(BTNST_COLOR_FG_IN, RGB(255, 255, 255), true);
			_ButtonVec[i]->SetColor(BTNST_COLOR_BK_OUT, RGB(0, 67, 178), true);
			_ButtonVec[i]->SetColor(BTNST_COLOR_FG_OUT, RGB(255, 255, 255), true);
			_ButtonVec[i]->SetColor(BTNST_COLOR_BK_FOCUS, RGB(0, 67, 178), true);
			_ButtonVec[i]->SetColor(BTNST_COLOR_FG_FOCUS, RGB(255, 255, 255), true);
		}
		else {
			_ButtonVec[i]->SetColor(BTNST_COLOR_BK_IN, RGB(255, 67, 0), true);
			_ButtonVec[i]->SetColor(BTNST_COLOR_FG_IN, RGB(255, 255, 255), true);
			_ButtonVec[i]->SetColor(BTNST_COLOR_BK_OUT, RGB(255, 255, 255), true);
			_ButtonVec[i]->SetColor(BTNST_COLOR_FG_OUT, RGB(0, 0, 0), true);
			_ButtonVec[i]->SetColor(BTNST_COLOR_BK_FOCUS, RGB(255, 255, 255), true);
			_ButtonVec[i]->SetColor(BTNST_COLOR_FG_FOCUS, RGB(0, 0, 0), true);
		}
	}

	ChangeChartData();
}

void VtChartTimeToolBar::ChangeChartData()
{
	if (!_Container)
		return;
	_Container->ChangeChartData(_Symbol, _ChartType, _Cycle);
}

void VtChartTimeToolBar::ChangeChartData(VtSymbol* symbol)
{
	if (!symbol)
		return;
	_Symbol = symbol;
	ChangeChartData();
}

void VtChartTimeToolBar::OnBnClickedBtnMonth()
{
	_ComboTime.EnableWindow(TRUE);
	_ComboTick.EnableWindow(FALSE);
	_Mode = 0;
	_ChartType = SmChartType::MONTH;
	ChangeButtonColor();
}


void VtChartTimeToolBar::OnBnClickedBtnWeek()
{
	_ComboTime.EnableWindow(TRUE);
	_ComboTick.EnableWindow(FALSE);
	_Mode = 1;
	_ChartType = SmChartType::WEEK;
	ChangeButtonColor();
}


void VtChartTimeToolBar::OnBnClickedBtnDay()
{
	_ComboTime.EnableWindow(TRUE);
	_ComboTick.EnableWindow(FALSE);
	_Mode = 2;
	_ChartType = SmChartType::DAY;
	ChangeButtonColor();
}


void VtChartTimeToolBar::OnBnClickedBtnMin()
{
	_ComboTime.EnableWindow(TRUE);
	_ComboTick.EnableWindow(FALSE);
	_Mode = 3;
	_ChartType = SmChartType::MIN;
	ChangeButtonColor();
}


void VtChartTimeToolBar::OnBnClickedBtnTick()
{
	_ComboTime.EnableWindow(FALSE);
	_ComboTick.EnableWindow(TRUE);
	_Mode = 4;
	_ChartType = SmChartType::TICK;
	ChangeButtonColor();
}


void VtChartTimeToolBar::OnCbnSelchangeComboTime()
{
	int curSel = _ComboTime.GetCurSel();
	if (curSel < 0)
		return;
	_Cycle = curSel + 1;
	ChangeChartData();
}

void VtChartTimeToolBar::InitSystem(std::vector<VtSystem*>& sysVector)
{
	_ComboSystem.ResetContent();
	for (auto it = sysVector.begin(); it != sysVector.end(); ++it)
	{
		VtSystem* system = *it;
		int index = _ComboSystem.AddString(system->Name().c_str());
		_ComboSystem.SetItemDataPtr(index, system);
	}

	if (_ComboSystem.GetCount() > 0)
		_ComboSystem.SetCurSel(0);
}


void VtChartTimeToolBar::OnCbnSelchangeComboSystem()
{
	int curSel = _ComboSystem.GetCurSel();
	if (_Container && curSel != -1) {
		VtSystem* system = (VtSystem*)_ComboSystem.GetItemDataPtr(curSel);
		_Container->ChangeSystem(system);
	}
}


void VtChartTimeToolBar::OnCbnSelchangeComboSymbol()
{
	int curSel = _ComboSymbol.GetCurSel();
	if (curSel < 0)
		return;
	_Symbol = (VtSymbol*)_ComboSymbol.GetItemDataPtr(curSel);
	ChangeChartData();
}


void VtChartTimeToolBar::OnCbnSelchangeComboTick()
{
	int curSel = _ComboTick.GetCurSel();
	if (curSel != -1 && _Container)
		_Cycle = curSel + 1;
	ChangeChartData();
}


void VtChartTimeToolBar::OnCbnSelchangeComboStyle()
{
	int curSel = _CombolStyle.GetCurSel();
	if (curSel < 0)
		return;

	if (!_Container)
		return;
	SmChartStyle style = SmChartStyle::CandleStick;
	switch (curSel)
	{
	case 0:
		style = SmChartStyle::CandleStick;
		break;
	case 1:
		style = SmChartStyle::OHLC;
		break;
	case 2:
		style = SmChartStyle::ClosePrice;
		break;
	case 3:
		style = SmChartStyle::TypicalPrice;
		break;
	case 4:
		style = SmChartStyle::WeightedClose;
		break;
	case 5:
		style = SmChartStyle::MedianPrice;
		break;
	default:
		break;
	}
	_Container->ChangeChartStyle(style);
}


void VtChartTimeToolBar::OnBnClickedButtonSearch()
{
	HdSymbolSelecter symSelecter;
	symSelecter.SetChartTimeToolBar(this);
	symSelecter.DoModal();
}
