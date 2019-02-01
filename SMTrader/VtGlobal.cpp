#include "stdafx.h"
#include "VtGlobal.h"
#include "resource.h"
#include "VtSymbolSelector.h"
#include "VtProductCategoryManager.h"
#include "VtProductCategory.h"
#include "VtProductSection.h"
#include "VtSymbol.h"
#include "CmdWaitDlg.h"
#include <time.h>

COLORREF VtGlobal::GridTitleBackColor = RGB(210, 224, 237);
COLORREF VtGlobal::GridTitleTextColor = RGB(0, 0, 0);
COLORREF VtGlobal::SelDialogBackColor = RGB(210, 224, 237);
COLORREF VtGlobal::SelCellBackColor = RGB(252, 252, 190);
COLORREF VtGlobal::MainBackColor = RGB(220, 220, 220);
COLORREF VtGlobal::MainTextColor = RGB(0, 0, 0);

std::pair<int, int> VtGlobal::GetScaleFact()
{
	HDC desktopDc = ::GetDC(nullptr);
	// Get native resolution
	int horizontalDPI = GetDeviceCaps(desktopDc, LOGPIXELSX);
	int verticalDPI = GetDeviceCaps(desktopDc, LOGPIXELSY);

	return std::make_pair(horizontalDPI, verticalDPI);
}

int VtGlobal::GetHorWidthByScaleFactor(int width)
{
	HDC desktopDc = ::GetDC(nullptr);
	// Get native resolution
	int horizontalDPI = GetDeviceCaps(desktopDc, LOGPIXELSX);
	switch (horizontalDPI)
	{
	case 96:
		width *= 1.0;
		break;
	case 120:
		width *= 1.25;
		break;
	case 144:
		width *= 1.50;
	case 192:
		width *= 2.00;
	default:
		width *= 1.0;
		break;
	}

	return (int)width;
}

int VtGlobal::GetVerHeightByScaleFactor(int height)
{
	HDC desktopDc = ::GetDC(nullptr);
	// Get native resolution
	int verticalDPI = GetDeviceCaps(desktopDc, LOGPIXELSY);

	switch (verticalDPI)
	{
	case 96:
		height *= 1.0;
		break;
	case 120:
		height *= 1.25;
		break;
	case 144:
		height *= 1.50;
	case 192:
		height *= 2.00;
	default:
		height *= 1.0;
		break;
	}

	return (int)height;
}

VtTime VtGlobal::GetTime(int time)
{
	int hourMin = time / 100;
	int sec = time % (hourMin * 100);
	int hour = hourMin / 100;
	int min = hourMin % (hour * 100);

	VtTime result;
	result.hour = hour;
	result.min = min;
	result.sec = sec;
	result.mil = 0;

	return result;
}

int VtGlobal::GetTime(VtTime time)
{
	int timeNo = time.hour * 10000;
	timeNo += time.min * 100;
	timeNo += time.sec;

	return timeNo;
}

VtDate VtGlobal::GetDate(int date)
{
	int yearMon = date / 100;
	int day = date % (yearMon * 100);
	int year = yearMon / 100;
	int mon = yearMon % (year * 100);

	VtDate result;
	result.year = year;
	result.month = mon;
	result.day = day;

	return result;
}

int VtGlobal::GetDate(VtDate date)
{
	int timeNo = date.year * 10000;
	timeNo += date.month * 100;
	timeNo += date.day;

	return timeNo;
}

VtTime VtGlobal::GetLocalTime()
{
	time_t now = time(0);
	tm timeinfo;
	localtime_s(&timeinfo, &now);

	VtTime curTime;
	curTime.hour = timeinfo.tm_hour;
	curTime.min = timeinfo.tm_min;
	curTime.sec = timeinfo.tm_sec;

	return curTime;
}

VtDate VtGlobal::GetLocalDate()
{
	time_t now = time(0);
	tm timeinfo;
	localtime_s(&timeinfo, &now);

	VtDate curDate;
	curDate.year = (timeinfo.tm_year + 1900);
	curDate.month = (timeinfo.tm_mon + 1);
	curDate.day = timeinfo.tm_mday;

	return curDate;
}

int VtGlobal::DefaultChartDataCount = 1000;


int VtGlobal::CenterWndYPos = 24;

VtGlobal::VtGlobal()
{
	std::string value;
	value = _T("6A");
	FavoriteDefaultSymbols[value] = value;
	value = _T("NKD");
	FavoriteDefaultSymbols[value] = value;
	value = _T("CL");
	FavoriteDefaultSymbols[value] = value;
	value = _T("ES");
	FavoriteDefaultSymbols[value] = value;
	value = _T("CN");
	FavoriteDefaultSymbols[value] = value;
	value = _T("GC");
	FavoriteDefaultSymbols[value] = value;
	value = _T("6E");
	FavoriteDefaultSymbols[value] = value;
	value = _T("M6E");
	FavoriteDefaultSymbols[value] = value;

#ifdef _DEBUG
	_ServerType = VtServerType::Simul;
#else
	_ServerType = VtServerType::Op;
#endif
	_WaitDlg = nullptr;

	ColorTable[0] = 0x00ff0000;
	ColorTable[1] = 0x000033ff;
	ColorTable[2] = 0x00006600;
	ColorTable[3] = 0x00ff00ff;
	ColorTable[4] = 0x009900cc;
	ColorTable[5] = 0x000099ff;
	ColorTable[6] = 0x000000ff;
	ColorTable[7] = 0x009900cc;
	ColorTable[8] = 0x00ff0099;
	ColorTable[9] = 0x00cc0000;

	ColorTable[10] = 0x00cc3300;
	ColorTable[11] = 0x00ffcc00;
	ColorTable[12] = 0x00009900;
	ColorTable[13] = 0x00006666;
	ColorTable[14] = 0x000066ff;
	ColorTable[15] = 0x000000cc;
	ColorTable[16] = 0x00663399;
	ColorTable[17] = 0x00cc0099;
	ColorTable[18] = 0x00990000;
	ColorTable[19] = 0x00993300;
	ColorTable[20] = 0x00cc9900;
	ColorTable[21] = 0x00006600;
	ColorTable[22] = 0x00336666;
	ColorTable[23] = 0x000033ff;
	ColorTable[24] = 0x00000099;
	ColorTable[25] = 0x00660099;
	ColorTable[26] = 0x00990066;
	ColorTable[27] = 0x00660000;
	ColorTable[28] = 0x00663300;

	ColorTable[29] = 0x00996633;
	ColorTable[30] = 0x00003300;
	ColorTable[31] = 0x00003333;
	ColorTable[32] = 0x00003399;
	ColorTable[33] = 0x00000066;
	ColorTable[34] = 0x00330066;

	ColorTable[35] = 0x00660066;
	ColorTable[36] = 0x00cc3333;
	ColorTable[37] = 0x00ff6633;
	ColorTable[38] = 0x00ffff33;
	ColorTable[39] = 0x0000ff00;
	ColorTable[40] = 0x0000cccc;
	ColorTable[41] = 0x0000ccff;

	ColorTable[42] = 0x003366ff;
	ColorTable[43] = 0x009933ff;
	ColorTable[44] = 0x00ff66ff;
}


VtGlobal::~VtGlobal()
{
	if (_SymSelector)
	{
		delete _SymSelector;
		_SymSelector = nullptr;
	}

	for (auto it = IndexList.begin(); it != IndexList.end(); ++it)
	{
		delete *it;
	}

	for (auto it = BandList.begin(); it != BandList.end(); ++it)
	{
		delete *it;
	}

	for (auto it = StressList.begin(); it != StressList.end(); ++it)
	{
		delete *it;
	}
}


void VtGlobal::InitBandList()
{
	VtBandBase* base = new VtBandBase();
	base->Name = _T("N일고-저밴드");
	base->Desc = _T("N일고-저밴드를 표시합니다");
	base->Type = VtBandType::Ndaily;
	BandList.push_back(base);

	base = new VtBandBase();
	base->Name = _T("일고-저밴드");
	base->Desc = _T("일고-저밴드 표시합니다");
	base->Type = VtBandType::Daily;
	BandList.push_back(base);

	base = new VtBandBase();
	base->Name = _T("고저밴드");
	base->Desc = _T("고저밴들를 표시합니다");
	base->Type = VtBandType::Entire;
	BandList.push_back(base);

	base = new VtBandBase();
	base->Name = _T("ORB밴드");
	base->Desc = _T("ORB밴들를 표시합니다");
	base->Type = VtBandType::ORB;
	BandList.push_back(base);
}

void VtGlobal::InitStressList()
{
	VtStressBase* base = new VtStressBase();
	base->Name = _T("VR전략구간1");
	base->Desc = _T("VR전략구간1을 표시합니다");
	base->Type = VtStressType::VRobo1;
	StressList.push_back(base);

	/*
	base = new VtStressBase();
	base->Name = _T("VR전략구간2");
	base->Desc = _T("VR전략구간2을 표시합니다");
	base->Type = VtStressType::VRobo2;
	StressList.push_back(base);
	*/
}

void VtGlobal::InitSystemList()
{
	VtSystemTemplate sysTempl;
	sysTempl.Type = VtSystemType::MaSystem;
	sysTempl.Name = _T("V-Trend1");
	SystemList.push_back(sysTempl);

	sysTempl.Type = VtSystemType::ChannelCross;
	sysTempl.Name = _T("V-Trend2");
	SystemList.push_back(sysTempl);

	sysTempl.Type = VtSystemType::ChannelJump;
	sysTempl.Name = _T("V-Trend3");
	SystemList.push_back(sysTempl);

	sysTempl.Type = VtSystemType::StoSystem;
	sysTempl.Name = _T("STO-K");
	SystemList.push_back(sysTempl);

	sysTempl.Type = VtSystemType::MacdSystem;
	sysTempl.Name = _T("MACD-POP");
	SystemList.push_back(sysTempl);



	sysTempl.Type = VtSystemType::Osillator;
	sysTempl.Name = _T("VRobo1");
	SystemList.push_back(sysTempl);

	sysTempl.Type = VtSystemType::DailyHeight;
	sysTempl.Name = _T("VRobo2-ORB");
	SystemList.push_back(sysTempl);
}

void VtGlobal::InitIndexList()
{
	VtIndexBase* base = new VtIndexBase();
	base->Name = _T("이동평균");
	base->Desc = _T("이동평균을 표시합니다");
	base->Type = VtIndexType::SimpleMa;
	IndexList.push_back(base);

	base = new VtIndexBase();
	base->Name = _T("거래량");
	base->Desc = _T("거래량을 표시합니다");
	base->Type = VtIndexType::DataVolume;
	IndexList.push_back(base);

	base = new VtIndexBase();
	base->Name = _T("볼린저밴드");
	base->Desc = _T("볼린저밴드를 표시합니다");
	base->Type = VtIndexType::BollingerBand;
	IndexList.push_back(base);

	base = new VtIndexBase();
	base->Name = _T("가격채널");
	base->Desc = _T("가격채널을 표시합니다");
	base->Type = VtIndexType::PriceChannel;
	IndexList.push_back(base);

	base = new VtIndexBase();
	base->Name = _T("스토캐스틱");
	base->Desc = _T("스토캐스틱을 표시합니다");
	base->Type = VtIndexType::Stochastic;
	IndexList.push_back(base);

	base = new VtIndexBase();
	base->Name = _T("MACD");
	base->Desc = _T("MACD를 표시합니다");
	base->Type = VtIndexType::Macd;
	IndexList.push_back(base);

	base = new VtIndexBase();
	base->Name = _T("RSI");
	base->Desc = _T("RSI를 표시합니다");
	base->Type = VtIndexType::Rsi;
	IndexList.push_back(base);

	base = new VtIndexBase();
	base->Name = _T("TRIX");
	base->Desc = _T("TRIX를 표시합니다");
	base->Type = VtIndexType::Trix;
	IndexList.push_back(base);

	base = new VtIndexBase();
	base->Name = _T("ATR");
	base->Desc = _T("ATR을 표시합니다");
	base->Type = VtIndexType::Atr;
	IndexList.push_back(base);
}


VtSymbolSelector* VtGlobal::GetSymSelector(CWnd* wnd)
{
	if (!_SymSelector)
	{
		_SymSelector = new VtSymbolSelector(wnd);
		_SymSelector->Create(IDD_SYMBOL_SELECTOR, nullptr);
		CRect rect;
		_SymSelector->GetClientRect(rect);

		_SymSelector->SetWindowPos(nullptr, 0, 0, rect.Width(), rect.Height(), SWP_NOZORDER);
		_SymSelector->ShowWindow(SW_SHOW);
	}
	else
		_SymSelector->ShowWindow(SW_SHOW);

	return _SymSelector;
}

void VtGlobal::HideSymSelector()
{
	if (_SymSelector)
	{
		_SymSelector->ShowWindow(SW_HIDE);
	}
}

void VtGlobal::InitFavorite()
{
	VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
	int i = 0;
	for (auto it = prdtCatMgr->SectionMap.begin(); it != prdtCatMgr->SectionMap.end(); it++)
	{
		VtProductSection* section = it->second;
		auto it2 = FavoriteDefaultSymbols.find(section->Code);
		if (it2 != FavoriteDefaultSymbols.end())
		{
			VtSymbol* symbol = section->SymbolList.front();
			symbol->UserDefinedName = section->Name;
			FavoriteSymbolList.push_back(symbol);
		}
	}
}

void VtGlobal::AddSymbol(VtSymbol* sym)
{
	if (!sym)
		return;
	if (FavoriteSymbolList.size() == 8)
		return;

	VtSymbol* fSym = FindSymbol(sym);
	if (fSym)
		return;

	FavoriteSymbolList.push_back(sym);
}

void VtGlobal::RemoveSymbol(VtSymbol* sym)
{
	for (auto it = FavoriteSymbolList.begin(); it != FavoriteSymbolList.end(); ++it)
	{
		VtSymbol* symbol = *it;
		if (symbol == sym)
		{
			FavoriteSymbolList.erase(it);
			break;
		}
	}
}

void VtGlobal::ClearSymbol()
{
	FavoriteSymbolList.clear();
}

VtSymbol* VtGlobal::FindSymbol(VtSymbol* sym)
{
	for (auto it = FavoriteSymbolList.begin(); it != FavoriteSymbolList.end(); ++it)
	{
		VtSymbol* symbol = *it;
		if (symbol == sym)
		{
			return sym;
		}
	}
	return nullptr;
}


void VtGlobal::OpenWaitDialog()
{
	_WaitDlg = new cWaitDialog();
	_WaitDlg->Show();
	_Waiting = true;
}

void VtGlobal::OpenWaitDialog(int state)
{
	_WaitDlg = new cWaitDialog();
	_WaitDlg->State = state;
	_WaitDlg->Show();
	_Waiting = true;
}

void VtGlobal::OpenWaitDialog(CString msg)
{
	_WaitDlg = new cWaitDialog();
	_WaitDlg->m_Text = msg;
	_WaitDlg->Show();
	_Waiting = true;
}

void VtGlobal::CloseWaitDialog()
{
	if (_WaitDlg)
	{
		_WaitDlg->Close();
		delete _WaitDlg;
		_WaitDlg = nullptr;
	}
	_Waiting = false;
}

void VtGlobal::CloseWaitDialog(int state)
{
	if (_WaitDlg && _WaitDlg->State == state)
	{
		_WaitDlg->Close();
		delete _WaitDlg;
		_WaitDlg = nullptr;
	}
	_Waiting = false;
}


