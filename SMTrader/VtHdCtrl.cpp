// VtHdCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtHdCtrl.h"
#include "afxdialogex.h"
#include "HDDefine.h"
#include "HdScheduler.h"
#include "Poco/LocalDateTime.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"
#include <string>
#include "Poco/Delegate.h"
#include "VtSymbolManager.h"
#include "VtSymbol.h"
#include "VtProductCategoryManager.h"
#include "VtProductSubSection.h"
#include "VtOrderDialogManager.h"
#include "VtOrder.h"
#include <iomanip>
#include <sstream>
#include <ios>
#include "VtOrderManagerSelector.h"
#include "VtOrderManager.h"
#include "VtProductOrderManagerSelector.h"
#include "VtProductOrderManager.h"
#include "VtAccountManager.h"
#include "VtAccount.h"
#include "Poco/NumberFormatter.h"
#include "VtPosition.h"
#include "HdFutureGrid.h"
#include "HdOptionGrid.h"
#include <tuple>
#include "ZmConfigManager.h"
#include "VtRealtimeRegisterManager.h"
#include "HdWindowManager.h"
#include "HdAccountPLDlg.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include "VtGlobal.h"
#include "Global/VtDefine.h"
#include "VtAccountAssetDlg.h"
#include "VtStringUtil.h"
#include "VtFundMiniJango.h"
#include "VtChartDataManager.h"
#include "VtChartData.h"
#include "VtOrderQueueManager.h"
#include "chartdir.h"
#include "MainFrm.h"
#include "VtTotalOrderManager.h"
#include "VtChartDataCollector.h"
#include "Log/loguru.hpp"
using namespace std;

using Poco::Delegate;

using Poco::LocalDateTime;
using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::NumberFormatter;


// VtHdCtrl dialog

IMPLEMENT_DYNAMIC(VtHdCtrl, CDialogEx)

VtHdCtrl::VtHdCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_HDF_CTRL, pParent)
{
	_FutureGrid = nullptr;
	_OptionGrid = nullptr;

	HdScheduler* scheduler = HdScheduler::GetInstance();
	_TaskCompleteEvent += delegate(scheduler, &HdScheduler::OnTaskCompleted);
}

VtHdCtrl::~VtHdCtrl()
{
	HdScheduler* scheduler = HdScheduler::GetInstance();
	_TaskCompleteEvent -= delegate(scheduler, &HdScheduler::OnTaskCompleted);
	if (m_CommAgent.GetSafeHwnd()) {
		if (m_CommAgent.CommGetConnectState() == 1) {
			if (m_sUserId != "")
				m_CommAgent.CommLogout(m_sUserId);
		}

		m_CommAgent.CommTerminate(TRUE);
		m_CommAgent.DestroyWindow();
	}
}

void VtHdCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


int VtHdCtrl::LogIn(CString id, CString pwd, CString cert)
{
	m_sUserId = id;
	m_sUserPw = pwd;
	m_sAuthPw = cert;
	const char *pLoginSuccess[] = { "로그인 성공"	, "Login Successful" };
	const char *pLoginFail[] = { "로그인 실패"	, "Login Failure" };

	int nRet = m_CommAgent.CommLogin(m_sUserId, m_sUserPw, m_sAuthPw);
	if (nRet > 0) {
		//AfxMessageBox(pLoginSuccess[0]);

		//로긴후 반드시 호출...
		m_CommAgent.CommAccInfo();
		LOG_F(INFO, pLoginSuccess[0]);
	}
	else
	{
		CString strRet;
		strRet.Format("[%d]", nRet);
		//AfxMessageBox(pLoginFail[0] + strRet);
		LOG_F(INFO, _T("%s"), strRet);
	}

	return nRet;
}

int VtHdCtrl::Init()
{
	if (!m_CommAgent.GetSafeHwnd()) {
		m_CommAgent.Create("HDF CommAgent", WS_CHILD, CRect(0, 0, 0, 0), this, 2286);
	}
	else
		return -1000;

	if (m_CommAgent.GetSafeHwnd()) {
		int nRet = m_CommAgent.CommInit(1);
		
		if (nRet < 0) {
			//AfxMessageBox("통신프로그램 실행 오류");
			LOG_F(INFO, _T("통신프로그램 실행 오류"));
		}
		else {
			//AfxMessageBox("통신프로그램 실행 성공");
			LOG_F(INFO, _T("통신프로그램 실행 성공"));
		}

		return nRet;
	}

	return -1000;
}

BEGIN_MESSAGE_MAP(VtHdCtrl, CDialogEx)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(VtHdCtrl, CDialogEx)
	ON_EVENT(VtHdCtrl, (UINT)-1, 3, OnDataRecv, VTS_BSTR VTS_I4)
	ON_EVENT(VtHdCtrl, (UINT)-1, 4, OnGetBroadData, VTS_BSTR VTS_I4)
	ON_EVENT(VtHdCtrl, (UINT)-1, 5, OnGetMsg, VTS_BSTR VTS_BSTR)
	ON_EVENT(VtHdCtrl, (UINT)-1, 6, OnGetMsgWithRqId, VTS_I4 VTS_BSTR VTS_BSTR)

END_EVENTSINK_MAP()

// VtHdCtrl message handlers
void VtHdCtrl::LogIn()
{
	int nRet = m_CommAgent.CommLogin(m_sUserId, m_sUserPw, m_sAuthPw);
	if (nRet > 0)
	{
		//AfxMessageBox("로그인 성공");

		// 로긴후 반드시 호출...
		m_CommAgent.CommAccInfo();
		//m_CommAgent.CommReqMakeCod("all", 0);

		//GetAccountInfo();
	}
	else
	{
		CString strRet;
		strRet.Format("[%d]", nRet);
		AfxMessageBox("로그인 실패" + strRet);
	}
}


void VtHdCtrl::LogOut()
{
	// 로그아웃한다.
	int nRet = m_CommAgent.CommLogout(m_sUserId);

	CString strRet;
	strRet.Format("[%d]", nRet);
	if (nRet < 0) {
		AfxMessageBox("로그아웃 실패" + strRet);
	}
	else {
		AfxMessageBox("로그아웃 성공" + strRet);
	}
}

int VtHdCtrl::LogOut(CString id)
{
	return m_CommAgent.CommLogout(m_sUserId);
}


void VtHdCtrl::GetSymbolMasterWithCount(std::string symCode, int count)
{
	CString sInput = VtSymbolManager::TrimRight(symCode, ' ').c_str();
	sInput.Append(_T("4"));
	std::string temp;
	temp = PadLeft(count, '0', 4);
	sInput.Append(temp.c_str());
	//단축코드[000], 호가수신시간[075], 현재가[051], 누적거래량[057], 최종결제가[040], 상장일[006], 고가[053]  1건 조회시
	CString sReqFidInput = _T("000001002003004005006007008009010011012013014015016017018019020021022023024025026027028029030031032033034035036037038039040041042043044045046047048049050051052053054055056057058059060061062063064065066067068069070071072073074075076077078079080081082083084085086087088089090091092093094095096097098099100101102103104105106107108109110111112113114115116117118119120121122123124125126127128129130131132133134135136137138139140141142143144145146147148149150151152153154155156157158159160161162163164165166167168169170171172173174175176177178179180181182183184185186187188189190191192193194195196197198199200201202203204205206207208209210211212213214215216217218219220221222223224225226227228229230231232");
	//CString sReqFidInput = _T("000001002003004005049050051052053054075076078079080081082083084085086087089090091092093094095096097098099100101102103104105106107108109115");
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommFIDRqData(DefSymbolMaster, sInput, sReqFidInput, sInput.GetLength(), strNextKey);
	AddRequest(nRqID, HdTaskType::HdSymbolMaster);
}

void VtHdCtrl::GetMasterFile(std::string fileName)
{
	ZmConfigManager* configMgr = ZmConfigManager::GetInstance();
	std::string path = configMgr->GetAppPath();
	path.append(_T("\\"));
	path.append(fileName);
	ifstream file(path);

	if (!file.is_open())
		return;

	string content;
	std::vector<std::string> line;

	while (std::getline(file, content))
	{
		line.push_back(content);
	}

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	HdProductInfo* prdtInfo = nullptr;
	for (auto it = line.begin(); it != line.end(); ++it)
	{
		int index = 0;
		std::string info = *it;
		std::string temp;
		temp = info.substr(index, 8);
		temp = VtSymbolManager::TrimRight(temp, ' ');
		prdtInfo = symMgr->AddProductInfo(temp);
		index = index + 8;
		temp = info.substr(index, 2);
		prdtInfo->decimal = std::stoi(temp);
		index = index + 2;
		temp = info.substr(index, 5);
		temp = VtSymbolManager::TrimLeft(temp, ' ');
		prdtInfo->tickSize = temp;
		temp.erase(std::remove(temp.begin(), temp.end(), '.'), temp.end());
		prdtInfo->intTickSize = std::stoi(temp);
		index = index + 5;
		temp = info.substr(index, 5);
		prdtInfo->tickValue = std::stoi(temp);
		index = index + 5;
		temp = info.substr(index, 10);
		temp = VtSymbolManager::TrimLeft(temp, ' ');
		prdtInfo->tradeWin = std::stoi(temp);
		index = index + 10;
		temp = info.substr(index, 30);
		temp = VtSymbolManager::TrimLeft(temp, ' ');
		prdtInfo->name = temp;
	}

	int i = 0;
}

BOOL VtHdCtrl::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}



void VtHdCtrl::GetFutureCodeList()
{
	if (_Blocked)
		return;

	CString sTrCode = DEF_AVAILABLE_CODE_LIST;
	CString sInput = "120180328";
	CString strNextKey = _T("");
	int nResult = m_CommAgent.CommGetConnectState();
	if (nResult == 1)
	{
		int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
	}
}

void VtHdCtrl::GetFutureCodeList(CString categoryName)
{
	if (_Blocked)
		return;

	GetSymbolCode(categoryName);
}

void VtHdCtrl::GetSymbolCode(CString categoryName)
{
	if (_Blocked)
		return;

	Sleep(700);
	CString sTrCode = DefSymbolCode;
	CString sInput = categoryName;
	CString strNextKey = _T("");
	int nResult = m_CommAgent.CommGetConnectState();
	if (nResult == 1)
	{
		int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
		AddRequest(nRqID, HdTaskType::HdSymbolCode);
	}
}

void VtHdCtrl::GetOptionCodeList(CString categoryName)
{
	if (_Blocked)
		return;

	GetSymbolCode(categoryName);
}

void VtHdCtrl::GetTradableCodeTable()
{
	LocalDateTime now;
	std::string curDate(DateTimeFormatter::format(now, "%Y%m%d"));
	CString strDate(curDate.c_str());
	CString sTrCode = DEF_AVAILABLE_CODE_LIST;
	CString sInput = _T("1");
	sInput.Append(strDate);
	CString strNextKey = _T("");
	int nResult = m_CommAgent.CommGetConnectState();
	if (nResult == 1)
	{
		int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
		AddRequest(nRqID, HdTaskType::HdTradableCodeTable);
	}
}

void VtHdCtrl::GetSymbolMaster(CString symCode)
{
	if (_Blocked)
		return;

	Sleep(100);

	//CString sInput = "101N600040020";
	CString sInput = symCode.TrimRight();
	sInput.Append(_T("40001"));
	//단축코드[000], 호가수신시간[075], 현재가[051], 누적거래량[057], 최종결제가[040], 상장일[006], 고가[053]  1건 조회시
	CString sReqFidInput = _T("000001002003004005006007008009010011012013014015016017018019020021022023024025026027028029030031032033034035036037038039040041042043044045046047048049050051052053054055056057058059060061062063064065066067068069070071072073074075076077078079080081082083084085086087088089090091092093094095096097098099100101102103104105106107108109110111112113114115116117118119120121122123124125126127128129130131132133134135136137138139140141142143144145146147148149150151152153154155156157158159160161162163164165166167168169170171172173174175176177178179180181182183184185186187188189190191192193194195196197198199200201202203204205206207208209210211212213214215216217218219220221222223224225226227228229230231232");
	//CString sReqFidInput = _T("000001002003004005049050051052053054075076078079080081082083084085086087089090091092093094095096097098099100101102103104105106107108109115");
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommFIDRqData(DefSymbolMaster, sInput, sReqFidInput, sInput.GetLength(), strNextKey);
	AddRequest(nRqID, HdTaskType::HdSymbolMaster);
}

void VtHdCtrl::RegisterProduct(CString symCode)
{
	int nRealType = 0;
	int nResult = 0;
	CString strKey = symCode;
	TCHAR first = symCode.GetAt(0);
	CString prefix = symCode.Left(3);
	if (first == '1' || first == '4') {
		if (prefix.Compare(_T("167")) == 0 || prefix.Compare(_T("175")) == 0) {
			nRealType = 58;
			nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
			nRealType = 71;
			nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
		}
		else {
			nRealType = 51;
			nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
			nRealType = 65;
			nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
		}
	}
	else if (first == '2' || first == '3') {
		nRealType = 52;
		nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
		nRealType = 66;
		nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
	}
	else {
		nRealType = 82;
		nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
	}
}

void VtHdCtrl::UnregisterProduct(CString symCode)
{
	int nRealType = 0;
	int nResult = 0;
	CString strKey = symCode;
	TCHAR first = symCode.GetAt(0);
	if (first == '1' || first == '4')
	{
		nRealType = 51;
		nResult = m_CommAgent.CommRemoveBroad(strKey, nRealType);
		nRealType = 65;
		nResult = m_CommAgent.CommRemoveBroad(strKey, nRealType);
	}
	else if (first == '2' || first == '3')
	{
		nRealType = 52;
		nResult = m_CommAgent.CommRemoveBroad(strKey, nRealType);
		nRealType = 66;
		nResult = m_CommAgent.CommRemoveBroad(strKey, nRealType);
	}
}

void VtHdCtrl::RegisterAccount(CString accountNo)
{
	CString strUserId = m_sUserId;
	CString strAcctNo = accountNo;
	int nResult = m_CommAgent.CommSetJumunChe(strUserId, strAcctNo);
}

void VtHdCtrl::UnregisterAccount(CString accountNo)
{
	CString strUserId = m_sUserId;
	CString strAcctNo = accountNo;
	int nResult = m_CommAgent.CommRemoveJumunChe(strUserId, strAcctNo);
}

bool VtHdCtrl::CheckPassword(HdOrderRequest& request)
{
	if (!request.Password.empty() && request.Password.length() == 4) {
		return true;
	}
	else {
		AfxMessageBox(_T("비번이 없습니다. 주문할수 없습니다."));
		return false;
	}
}

void VtHdCtrl::OnReceiveChartData(CString& sTrCode, LONG& nRqID)
{
	VtChartData* chartData = nullptr;
	VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
	auto it = _ChartDataRequestMap.find(nRqID);
	if (it != _ChartDataRequestMap.end()) { // 차트데이터 요청 맵에서 찾아 본다.
		chartData = chartDataMgr->Find(it->second);
		_ChartDataRequestMap.erase(it);
	}

	if (!chartData)
		return;

	CString strPrevKey = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "이전키");

	int nRepeatCnt = 0;
	if (chartData->Domestic())
		nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec2");
	else
		nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	int savedIndex = ChartDataSize - 1;
	// 먼저 키를 모두 삭제한다.
	chartData->InputDateTimeMap.clear();
	// Received the chart data first.
	auto timeKey = std::make_pair(0, 0);
	for (int i = 0; i < nRepeatCnt; i++) {
		CString strDate;
		CString key(_T(""));
		CString strTime;
		if (chartData->Domestic()) {
			strDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "날짜시간");
			if (chartData->ChartType() == VtChartType::MIN)
				strDate.Append(_T("00"));
			else
				strDate.Append(_T("000000"));
			key = _T("OutRec2");
			strTime = strDate.Right(6);
			CString strDate2 = strDate.Left(8);
			timeKey = std::make_pair(_ttoi(strDate2), _ttoi(strTime));
		}
		else {
			strDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "국내일자");
			strTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "국내시간");
			timeKey = std::make_pair(_ttoi(strDate), _ttoi(strTime));
			strDate.Append(strTime);
			key = _T("OutRec1");
		}
		if (strDate.GetLength() == 0) {
			break;
		}
		CString strOpen = m_CommAgent.CommGetData(sTrCode, -1, key, i, "시가");
		CString strHigh = m_CommAgent.CommGetData(sTrCode, -1, key, i, "고가");
		CString strLow = m_CommAgent.CommGetData(sTrCode, -1, key, i, "저가");
		CString strClose = m_CommAgent.CommGetData(sTrCode, -1, key, i, "종가");
		CString strVol;
		if (chartData->Domestic())
			strVol = m_CommAgent.CommGetData(sTrCode, -1, key, i, "거래량");
		else
			strVol = m_CommAgent.CommGetData(sTrCode, -1, key, i, "체결량");

		savedIndex = ChartDataSize - 1 - i;
		
		if (strHigh.GetLength() != 0)
			chartData->InputChartData.High[savedIndex] = std::stod((LPCTSTR)strHigh) / std::pow(10, chartData->Decimal());
		if (strLow.GetLength() != 0)
			chartData->InputChartData.Low[savedIndex] = std::stod((LPCTSTR)strLow) / std::pow(10, chartData->Decimal());
		if (strOpen.GetLength() != 0)
			chartData->InputChartData.Open[savedIndex] = std::stod((LPCTSTR)strOpen) / std::pow(10, chartData->Decimal());
		if (strClose.GetLength() != 0)
			chartData->InputChartData.Close[savedIndex] = std::stod((LPCTSTR)strClose) / std::pow(10, chartData->Decimal());
		if (strVol.GetLength() != 0)
			chartData->InputChartData.Volume[savedIndex] = std::stod((LPCTSTR)strVol);

		std::pair<VtDate, VtTime> dateTime = VtChartData::GetDateTime(strDate);
		chartData->InputChartData.Date[savedIndex] = dateTime.first;
		chartData->InputChartData.Time[savedIndex] = dateTime.second;
		chartData->InputChartData.DateTime[savedIndex] = Chart::chartTime(dateTime.first.year, dateTime.first.month, dateTime.first.day, dateTime.second.hour, dateTime.second.min, dateTime.second.sec);

		chartData->InputDateTimeMap[timeKey] = savedIndex;
	}
	if (chartData->GetDataCount() == 0) {
		chartData->SetFirstData();
		chartData->FilledCount(ChartDataSize);
		chartData->Filled(true);
		chartDataMgr->OnReceiveFirstChartData(chartData);
		return;
	}
	chartDataMgr->OnReceiveChartData(chartData);
}

void VtHdCtrl::OnChartData(CString& sTrCode, LONG& nRqID)
{
	VtChartData* chartData = nullptr;
	VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
	auto it = _ChartDataRequestMap.find(nRqID);
	if (it != _ChartDataRequestMap.end()) { // 차트데이터 요청 맵에서 찾아 본다.
		chartData = chartDataMgr->Find(it->second);
		_ChartDataRequestMap.erase(it);
	}

	if (!chartData)
		return;
	double lastTimeInfo = chartData->GetLastTimeInfo();
	double firstTimeInfo = 0.0;

	CString strPrevKey = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "이전키");

	int nRepeatCnt = 0;
	if (chartData->Domestic())
		nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec2");
	else
		nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	int foundIndex = nRepeatCnt;
	int realCount = nRepeatCnt;
	int lastSavedIndex = nRepeatCnt - 1;
	int savedIndex = lastSavedIndex;
	// 먼저 키를 모두 삭제한다.
	chartData->InputDateTimeMap.clear();
	// Received the chart data first.
	auto timeKey = std::make_pair(0, 0);
	for (int i = 0; i < nRepeatCnt; i++) {
		CString strDate;
		CString key(_T(""));
		CString strTime;
		if (chartData->Domestic()) {
			strDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "날짜시간");
			if (chartData->ChartType() == VtChartType::MIN)
				strDate.Append(_T("00"));
			else
				strDate.Append(_T("000000"));
			key = _T("OutRec2");
			strTime = strDate.Right(6);
			CString strDate2 = strDate.Left(8);
			timeKey = std::make_pair(_ttoi(strDate2), _ttoi(strTime));
		}
		else {
			strDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "국내일자");
			strTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "국내시간");
			strDate.Append(strTime);
			key = _T("OutRec1");
			timeKey = std::make_pair(_ttoi(strDate), _ttoi(strTime));
		}
		if (strDate.GetLength() == 0) {
			if (foundIndex == nRepeatCnt)
				foundIndex = i;
			realCount = i;
			lastSavedIndex = savedIndex;
			break;
		}
		CString strOpen = m_CommAgent.CommGetData(sTrCode, -1, key, i, "시가");
		CString strHigh = m_CommAgent.CommGetData(sTrCode, -1, key, i, "고가");
		CString strLow = m_CommAgent.CommGetData(sTrCode, -1, key, i, "저가");
		CString strClose = m_CommAgent.CommGetData(sTrCode, -1, key, i, "종가");
		CString strVol;
		if (chartData->Domestic())
			strVol = m_CommAgent.CommGetData(sTrCode, -1, key, i, "거래량");
		else
			strVol = m_CommAgent.CommGetData(sTrCode, -1, key, i, "체결량");

		savedIndex = nRepeatCnt - 1 - i;
		double timeInfo = std::stod((LPCTSTR)strDate);

		if (i == 0) {
			firstTimeInfo = timeInfo;
		}

		chartData->InputChartData.TimeInfo[savedIndex] = timeInfo;
		if (lastTimeInfo == chartData->InputChartData.TimeInfo[savedIndex]) {
			foundIndex = savedIndex;
		}
		if (strHigh.GetLength() != 0)
			chartData->InputChartData.High[savedIndex] = std::stod((LPCTSTR)strHigh) / std::pow(10, chartData->Decimal());
		if (strLow.GetLength() != 0)
			chartData->InputChartData.Low[savedIndex] = std::stod((LPCTSTR)strLow) / std::pow(10, chartData->Decimal());
		if (strOpen.GetLength() != 0)
			chartData->InputChartData.Open[savedIndex] = std::stod((LPCTSTR)strOpen) / std::pow(10, chartData->Decimal());
		if (strClose.GetLength() != 0)
			chartData->InputChartData.Close[savedIndex] = std::stod((LPCTSTR)strClose) / std::pow(10, chartData->Decimal());
		if (strVol.GetLength() != 0)
			chartData->InputChartData.Volume[savedIndex] = std::stod((LPCTSTR)strVol);

		std::pair<VtDate, VtTime> dateTime = VtChartData::GetDateTime(strDate);
		chartData->InputChartData.Date[savedIndex] = dateTime.first;
		chartData->InputChartData.Time[savedIndex] = dateTime.second;
		chartData->InputChartData.DateTime[savedIndex] = Chart::chartTime(dateTime.first.year, dateTime.first.month, dateTime.first.day, dateTime.second.hour, dateTime.second.min, dateTime.second.sec);

		chartData->InputDateTimeMap[timeKey] = savedIndex;
		CString strData;
		strData.Format(_T("savedIndex = %d, index = %d, %s,%s,%s,%s,%s,%s,%s \n"), savedIndex, i, strDate, strTime, strOpen, strHigh, strLow, strClose, strVol);
		TRACE(strData);

		//LOG_F(INFO, _T("code %s, datetime =%s, savedIndex = %d"), chartData->SymbolCode().c_str(), strDate, savedIndex);
	}

	if (nRepeatCnt != realCount) {
		for (int p = 0; p < realCount; p++) {
			chartData->InputChartData.TimeInfo[p] = chartData->InputChartData.TimeInfo[lastSavedIndex + p];
			chartData->InputChartData.High[p] = chartData->InputChartData.High[lastSavedIndex + p];
			chartData->InputChartData.Low[p] = chartData->InputChartData.Low[lastSavedIndex + p];
			chartData->InputChartData.Open[p] = chartData->InputChartData.Open[lastSavedIndex + p];
			chartData->InputChartData.Close[p] = chartData->InputChartData.Close[lastSavedIndex + p];
			chartData->InputChartData.Volume[p] = chartData->InputChartData.Volume[lastSavedIndex + p];
			chartData->InputChartData.Date[p] = chartData->InputChartData.Date[lastSavedIndex + p];
			chartData->InputChartData.Time[p] = chartData->InputChartData.Time[lastSavedIndex + p];
			chartData->InputChartData.DateTime[p] = chartData->InputChartData.DateTime[lastSavedIndex + p];
		}
		// Reset the filled count of chart data
		chartData->FilledCount(0);
		chartData->CopyData(realCount, foundIndex);
	}
	else {
		// 마지막으로 받은 시간이 같으면 처리하지 않는다.
		if (lastTimeInfo != firstTimeInfo)
			chartData->CopyData(realCount, foundIndex);
	}

	chartData->Filled(true);
	chartDataMgr->OnReceiveChartData(chartData);
	//RemoveRequest(nRqID);
}

void VtHdCtrl::OnNewOrderHd(CString& sTrCode, LONG& nRqID)
{
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");


	CString strExchTp = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "접수구분");
	CString strProcTp = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "처리구분");
	CString strAcctNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "계좌번호");
	CString strOrdNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문번호");
	CString strSeries = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "종목코드");
	CString strPrice = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문가격");
	CString strAmount = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문수량");
	CString strPosition = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매매구분");
	CString strPriceType = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "가격구분");
	CString strCustom = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "사용자정의필드");
	CString strMan = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "조작구분");

	CString strFundName = _T("");
	CString strSubAcntNo = _T("");
	auto it = _OrderRequestMap.find(nRqID);
	int oriReqNo = -1;
	int reqType = 0;
	// 등록되지 않은 요청 번호는 처리하지 않는다.
	if (it != _OrderRequestMap.end()) {
		HdOrderRequest& req = it->second;
		strFundName = req.FundName.c_str();
		strSubAcntNo = req.SubAccountNo.c_str();
		oriReqNo = req.RequestId;
		reqType = req.RequestType;
	}
	else {
		LOG_F(INFO, _T("OnNewOrderHd등록되지 않은 주문 요청 입니다!"));
	}



	CString strMsg;
	strMsg.Format("OnNewOrderHd 번호[%d][%s]처리[%s]계좌번호[%s]주문번호[%s]\n", nRqID, strExchTp, strProcTp, strAcctNo, strOrdNo);

	strPrice = strPrice.TrimLeft('0');
	CString strOriOrderPrice;
	strOriOrderPrice = strPrice;

	strPrice.Remove('.');
	strAcctNo.TrimRight();
	strOrdNo.TrimLeft('0');
	strSeries.TrimRight();
	strPrice.TrimRight();
	strAmount.TrimRight();

	VtOrderManagerSelector* orderMgrSeledter = VtOrderManagerSelector::GetInstance();
	VtOrderManager* orderMgr = orderMgrSeledter->FindAddOrderManager((LPCTSTR)strAcctNo);

	VtOrder* order = nullptr;
	order = orderMgr->FindOrder(_ttoi(strOrdNo));
	if (!order)
	{
		order = new VtOrder();
	}
	else
	{
		// 이 경우는 이미 주문이 있는 경우이다. 체결이 먼저 되었을 경우 이런 경우가 생길 수 있다. 
		// 이 때는 주문의 상태가 체결인 경우는 이 과정을 진행 시키지 않는다.
		if (order->state == VtOrderState::Filled)
			return;
	}
	// 일반 주문인지 청산 주문인지 넣어 준다.
	order->RequestType = reqType;
	order->AccountNo = (LPCTSTR)strAcctNo;
	order->shortCode = (LPCTSTR)strSeries;
	order->orderNo = _ttoi(strOrdNo);
	order->intOrderPrice = _ttoi(strPrice);
	order->amount = _ttoi(strAmount);
	order->SubAccountNo = strSubAcntNo;
	order->FundName = strFundName;
	order->orderPrice = _ttof(strOriOrderPrice);

	if (strPosition.Compare(_T("1")) == 0)
	{
		order->orderPosition = VtPositionType::Buy;
	}
	else if (strPosition.Compare(_T("2")) == 0)
	{
		order->orderPosition = VtPositionType::Sell;
	}

	if (strPriceType.Compare(_T("1")) == 0)
	{
		order->priceType = VtPriceType::Price;
	}
	else if (strPosition.Compare(_T("2")) == 0)
	{
		order->priceType = VtPriceType::Market;
	}

	order->orderType = VtOrderType::New;

	VtOrderState prevState = order->state;

	orderMgr->OnOrderReceivedHd(order);

	SendOrderMessage(VtOrderEvent::PutNew, order);

	//RemoveRequest(nRqID);
	OnOrderReceived(nRqID, order);

	OnSubAccountOrder(VtOrderEvent::PutNew, strSubAcntNo, strFundName, order, prevState);

	LOG_F(INFO, _T("신규주문서버확인 : 원요청번호 %d, 선물사 요청번호 = %d, 종목이름 = %s, 주문 번호 = %s, 계좌번호 = %s, 서브계좌번호 = %s, 펀드 이름 = %s, 주문종류 = %s, 주문갯수 = %s, 요청 타입 = %d"), oriReqNo, nRqID, strSeries, strOrdNo, strAcctNo, strSubAcntNo, strFundName, strPosition.Compare(_T("1")) == 0 ? _T("매수") : _T("매도"), strAmount, order->RequestType);
}

void VtHdCtrl::OnModifyOrderHd(CString& sTrCode, LONG& nRqID)
{
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");

	CString strExchTp = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "접수구분");
	CString strProcTp = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "처리구분");
	CString strAcctNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "계좌번호");
	CString strOrdNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문번호");
	CString strSeries = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "종목코드");
	CString strPrice = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문가격");
	CString strAmount = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문수량");
	CString strPosition = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매매구분");
	CString strPriceType = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "가격구분");
	CString strCustom = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "사용자정의필드");
	CString strMan = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "조작구분");
	CString strOriOrdNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "원주문번호");

	CString strFundName = _T("");
	CString strSubAcntNo = _T("");
	int oriReqNo = -1;
	auto it = _OrderRequestMap.find(nRqID);
	int reqType = 0;
	// 등록되지 않은 요청 번호는 처리하지 않는다.
	if (it != _OrderRequestMap.end())
	{
		HdOrderRequest& req = it->second;
		strFundName = req.FundName.c_str();
		strSubAcntNo = req.SubAccountNo.c_str();
		oriReqNo = req.RequestId;
		reqType = req.RequestType;
	}
	else {
		LOG_F(INFO, _T("OnModifyOrderHd등록되지 않은 주문 요청 입니다!"));
	}

	CString strMsg;
	strMsg.Format("OnModifyOrderHd 번호[%d][%s]처리[%s]계좌번호[%s]주문번호[%s]\n", nRqID, strExchTp, strProcTp, strAcctNo, strOrdNo);
	//WriteLog(strMsg);
	//strMsg.Format(_T("%s\n"), strCustom);
	//TRACE(strMsg);

	strPrice = strPrice.TrimLeft('0');
	CString strOriOrderPrice;
	strOriOrderPrice = strPrice;
	strPrice.Remove('.');
	strAcctNo.TrimRight();
	strOrdNo.TrimLeft('0');
	strSeries.TrimRight();
	strPrice.TrimRight();
	strAmount.TrimRight();
	strOriOrdNo.TrimRight();

	VtOrderManagerSelector* orderMgrSeledter = VtOrderManagerSelector::GetInstance();
	VtOrderManager* orderMgr = orderMgrSeledter->FindAddOrderManager((LPCTSTR)strAcctNo);

	VtOrder* order = nullptr;
	order = orderMgr->FindOrder(_ttoi(strOrdNo));
	if (!order)
		order = new VtOrder();
	else {
		// 이 때는 주문의 상태가 체결인 경우는 이 과정을 진행 시키지 않는다.
		if (order->state == VtOrderState::Filled)
			return;
	}
	order->RequestType = reqType;
	order->AccountNo = (LPCTSTR)strAcctNo;
	order->shortCode = (LPCTSTR)strSeries;
	order->orderNo = _ttoi(strOrdNo);
	order->intOrderPrice = _ttoi(strPrice);
	order->amount = _ttoi(strAmount);
	order->oriOrderNo = _ttoi(strOriOrdNo);
	order->orderPrice = _ttof(strOriOrderPrice);

	order->SubAccountNo = strSubAcntNo;
	order->FundName = strFundName;

	if (strPosition.Compare(_T("1")) == 0) {
		order->orderPosition = VtPositionType::Buy;
	}
	else if (strPosition.Compare(_T("2")) == 0) {
		order->orderPosition = VtPositionType::Sell;
	}

	if (strPriceType.Compare(_T("1")) == 0) {
		order->priceType = VtPriceType::Price;
	}
	else if (strPosition.Compare(_T("2")) == 0) {
		order->priceType = VtPriceType::Market;
	}

	order->orderType = VtOrderType::Change;
	order->state = VtOrderState::OrderReceived;
	VtOrderState prevState = order->state;
	orderMgr->OnOrderReceivedHd(order);

	SendOrderMessage(VtOrderEvent::Modified, order);

	//RemoveRequest(nRqID);
	OnOrderReceived(nRqID, order);

	OnSubAccountOrder(VtOrderEvent::Modified, strSubAcntNo, strFundName, order, prevState);

	LOG_F(INFO, _T("정정주문서버확인 : 원요청번호 %d, 선물사 요청번호 = %d, 종목이름 = %s, 주문 번호 = %s, 계좌번호 = %s, 서브계좌번호 = %s, 펀드 이름 = %s, 주문종류 = %s, 주문갯수 = %s, 요청 타입 = %d"), oriReqNo, nRqID, strSeries, strOrdNo, strAcctNo, strSubAcntNo, strFundName, strPosition.Compare(_T("1")) == 0 ? _T("매수") : _T("매도"), strAmount, order->RequestType);
}

void VtHdCtrl::OnCancelOrderHd(CString& sTrCode, LONG& nRqID)
{
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");

	CString strExchTp = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "접수구분");
	CString strProcTp = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "처리구분");
	CString strAcctNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "계좌번호");
	CString strOrdNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문번호");
	CString strSeries = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "종목코드");
	CString strPrice = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문가격");
	CString strAmount = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문수량");
	CString strPosition = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매매구분");
	CString strPriceType = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "가격구분");
	CString strCustom = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "사용자정의필드");
	CString strOriOrdNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "원주문번호");

	CString strFundName = _T("");
	CString strSubAcntNo = _T("");
	auto it = _OrderRequestMap.find(nRqID);
	int oriReqNo = -1;
	int reqType = 0;
	// 등록되지 않은 요청 번호는 처리하지 않는다.
	if (it != _OrderRequestMap.end())
	{
		HdOrderRequest& req = it->second;
		strFundName = req.FundName.c_str();
		strSubAcntNo = req.SubAccountNo.c_str();
		oriReqNo = req.RequestId;
		reqType = req.RequestType;
	}
	else {
		LOG_F(INFO, _T("OnCancelOrderHd등록되지 않은 주문 요청 입니다!"));
	}

	CString strMsg;
	strMsg.Format("OnCancelOrderHd 번호[%d][%s]처리[%s]계좌번호[%s]주문번호[%s]\n", nRqID, strExchTp, strProcTp, strAcctNo, strOrdNo);
	//WriteLog(strMsg);
	//strMsg.Format(_T("%s\n"), strCustom);
	//TRACE(strMsg);

	strPrice = strPrice.TrimLeft('0');
	CString strOriOrderPrice;
	strOriOrderPrice = strPrice;

	strPrice.Remove('.');
	strAcctNo.TrimRight();
	strOrdNo.TrimLeft('0');
	strSeries.TrimRight();
	strAmount.TrimRight();
	strOriOrdNo.TrimRight();

	VtOrderManagerSelector* orderMgrSeledter = VtOrderManagerSelector::GetInstance();
	VtOrderManager* orderMgr = orderMgrSeledter->FindAddOrderManager((LPCTSTR)strAcctNo);

	VtOrder* order = nullptr;
	order = orderMgr->FindOrder(_ttoi(strOrdNo));
	if (!order)
		order = new VtOrder();
	else {
		// 이 때는 주문의 상태가 체결인 경우는 이 과정을 진행 시키지 않는다.
		if (order->state == VtOrderState::Filled)
			return;
	}
	order->RequestType = reqType;
	order->AccountNo = (LPCTSTR)strAcctNo;
	order->shortCode = (LPCTSTR)strSeries;
	order->orderNo = _ttoi(strOrdNo);
	order->intOrderPrice = _ttoi(strPrice);
	order->amount = _ttoi(strAmount);
	order->oriOrderNo = _ttoi(strOriOrdNo);
	order->orderPrice = _ttoi(strOriOrderPrice);

	order->SubAccountNo = strSubAcntNo;
	order->FundName = strFundName;

	if (strPosition.Compare(_T("1")) == 0) {
		order->orderPosition = VtPositionType::Buy;
	}
	else if (strPosition.Compare(_T("2")) == 0) {
		order->orderPosition = VtPositionType::Sell;
	}

	if (strPriceType.Compare(_T("1")) == 0) {
		order->priceType = VtPriceType::Price;
	}
	else if (strPosition.Compare(_T("2")) == 0) {
		order->priceType = VtPriceType::Market;
	}

	order->orderType = VtOrderType::Cancel;
	VtOrderState prevState = order->state;
	orderMgr->OnOrderReceivedHd(order);

	SendOrderMessage(VtOrderEvent::Cancelled, order);

	//RemoveRequest(nRqID);
	OnOrderReceived(nRqID, order);

	OnSubAccountOrder(VtOrderEvent::Cancelled, strSubAcntNo, strFundName, order, prevState);

	LOG_F(INFO, _T("취소주문서버확인 : 원요청번호 %d, 선물사 요청번호 = %d, 종목이름 = %s, 주문 번호 = %s, 계좌번호 = %s, 서브계좌번호 = %s, 펀드 이름 = %s, 주문종류 = %s, 주문갯수 = %s, 요청 타입 = %d"), oriReqNo, nRqID, strSeries, strOrdNo, strAcctNo, strSubAcntNo, strFundName, strPosition.Compare(_T("1")) == 0 ? _T("매수") : _T("매도"), strAmount, order->RequestType);
}

void VtHdCtrl::OnReceiveRealTimeValue(std::string symCode)
{
	auto it = _DataKeyMap.find(symCode);
	if (it != _DataKeyMap.end()) {
		std::set<std::string>& curSet = it->second;
		for (auto its = curSet.begin(); its != curSet.end(); ++its) {
			;
		}
	}
}

void VtHdCtrl::OnReceiveHoga(int time, VtSymbol* sym)
{
	if (!sym)
		return;

	VtChartDataCollector* dataCollector = VtChartDataCollector::GetInstance();
	std::string symCode = sym->ShortCode;
	std::string code = symCode + _T("SHTQ:5:1");
	dataCollector->OnReceiveData(code, time, sym->Hoga.TotSellQty);
	code = symCode + _T("BHTQ:5:1");
	dataCollector->OnReceiveData(code, time, sym->Hoga.TotBuyQty);
	code = symCode + _T("SHTC:5:1");
	dataCollector->OnReceiveData(code, time, sym->Hoga.TotSellNo);
	code = symCode + _T("BHTC:5:1");
	dataCollector->OnReceiveData(code, time, sym->Hoga.TotBuyNo);

	code = symCode + _T("SHTQ:5:5");
	dataCollector->OnReceiveData(code, time, sym->Hoga.TotSellQty);
	code = symCode + _T("BHTQ:5:5");
	dataCollector->OnReceiveData(code, time, sym->Hoga.TotBuyQty);
	code = symCode + _T("SHTC:5:5");
	dataCollector->OnReceiveData(code, time, sym->Hoga.TotSellNo);
	code = symCode + _T("BHTC:5:5");
	dataCollector->OnReceiveData(code, time, sym->Hoga.TotBuyNo);
}

void VtHdCtrl::OnReceiveSise(int time, VtSymbol* sym)
{
	VtChartDataCollector* dataCollector = VtChartDataCollector::GetInstance();
	std::string symCode = sym->ShortCode;
	std::string code = symCode + _T(":5:1");
	double close = sym->Quote.intClose / std::pow(10, sym->IntDecimal);
	dataCollector->OnReceiveData(code, time, close);
	code = symCode + _T(":1:1");
	dataCollector->OnReceiveData(code, time, close);
}

void VtHdCtrl::OnOrderReceived(int reqId, VtOrder* order)
{
	if (!order)
		return;
	auto it = _OrderRequestMap.find(reqId);
	if (it != _OrderRequestMap.end()) {
		HdOrderRequest req = it->second;
		_ReceivedRequestMap[order->orderNo] = req;
		if (order->orderType == VtOrderType::Change ||
			order->orderType == VtOrderType::Cancel) {
			_ReceivedRequestMap[order->oriOrderNo] = req;
		}

		_OrderRequestMap.erase(it);

		CString msg;
		msg.Format(_T("주문타입 = %d, subacnt = %s, fundname = %s \n"), (int)req.orderType, req.SubAccountNo.c_str(), req.FundName.c_str());
		//TRACE(msg);
	}
}

void VtHdCtrl::OnOrderReceivedHd(VtOrder* order)
{
	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	orderDlgMgr->OnOrderReceivedHd(order);
}



void VtHdCtrl::OnOrderReceivedHd(CString& sTrCode, LONG& nRqID)
{

}

void VtHdCtrl::OnOrderAcceptedHd(VtOrder* order)
{
	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	orderDlgMgr->OnOrderAcceptedHd(order);
}



void VtHdCtrl::OnOrderAcceptedHd(CString& strKey, LONG& nRealType)
{
	CString strAcctNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "계좌번호");
	CString strOrdNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strPrice = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문가격");
	CString strAmount = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문수량");
	CString strCustom = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "사용자정의필드");
	CString strPosition = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매매구분");
	CString strPriceType = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "가격구분");
	CString strMan = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "조작구분");
	CString strOriOrderNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "원주문번호");
	CString strFirstOrderNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "최초원주문번호");
	CString strTraderTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "접수시간");

	CString strMsg;
	strMsg.Format("OnOrderAcceptedHd 계좌번호[%s]주문번호[%s]\n", strAcctNo, strOrdNo);
	//WriteLog(strMsg);
	//strMsg.Format(_T("%s\n"), strCustom);
	TRACE(strMsg);

	strAcctNo.TrimRight();
	strOrdNo.TrimLeft('0');
	strSeries.TrimRight();

	strPrice = strPrice.TrimLeft('0');
	CString strOriOrderPrice;
	strOriOrderPrice = strPrice;

	strPrice.Remove('.');

	strPrice.TrimRight();
	strAmount.TrimRight();

	VtOrderManagerSelector* orderMgrSeledter = VtOrderManagerSelector::GetInstance();
	VtOrderManager* orderMgr = orderMgrSeledter->FindAddOrderManager((LPCTSTR)strAcctNo);

	VtOrder* order = nullptr;
	order = orderMgr->FindOrder(_ttoi(strOrdNo));
	if (!order) {
		order = new VtOrder();

		std::string symCode = (LPCTSTR)strSeries;
		VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
		VtSymbol* sym = symMgr->FindHdSymbol(symCode);


		order->AccountNo = (LPCTSTR)strAcctNo;
		order->shortCode = (LPCTSTR)strSeries;
		order->orderNo = _ttoi(strOrdNo);
		order->intOrderPrice = _ttoi(strPrice);
		order->amount = _ttoi(strAmount);

		order->orderPrice = _ttof(strOriOrderPrice);

		if (strPosition.Compare(_T("1")) == 0) {
			order->orderPosition = VtPositionType::Buy;
		}
		else if (strPosition.Compare(_T("2")) == 0) {
			order->orderPosition = VtPositionType::Sell;
		}

		if (strPriceType.Compare(_T("1")) == 0) {
			order->priceType = VtPriceType::Price;
		}
		else if (strPosition.Compare(_T("2")) == 0) {
			order->priceType = VtPriceType::Market;
		}
		order->tradeTime = (LPCTSTR)strTraderTime;
	}
	else {
		order->intOrderPrice = _ttoi(strPrice);
		order->amount = _ttoi(strAmount);
		order->orderPrice = _ttof(strOriOrderPrice);
		order->tradeTime = (LPCTSTR)strTraderTime;
		if (order->state == VtOrderState::Filled) {
			LOG_F(INFO, _T("OnAccepted :: // 주문역전 : 이 때는 주문의 상태가 체결인 경우는 펀드 주문이나 서브 계좌 주문인 경우 초기 과정부터 다시 진행 시켜 준다."));
		}
	}

	if (strMan.Compare(_T("1")) == 0) {
		order->orderType = VtOrderType::New;
	}
	else if (strMan.Compare(_T("8")) == 0 ||
		strMan.Compare(_T("9")) == 0) {
		order->orderType = VtOrderType::Change;
	}
	else if (strMan.Compare(_T("2")) == 0) {
		order->orderType = VtOrderType::Cancel;
	}

	order->firstOrderNo = _ttoi(strFirstOrderNo);
	order->oriOrderNo = _ttoi(strOriOrderNo);
	VtOrderState prevState = order->state;
	orderMgr->OnOrderAcceptedHd(order);

	CString strFundName = _T("");
	CString strSubAcntNo = _T("");
	auto it = _ReceivedRequestMap.find(_ttoi(strOrdNo));
	int oriReqNo = -1;
	int reqType = 0;
	// 등록되지 않은 요청 번호는 처리하지 않는다.
	if (it != _ReceivedRequestMap.end()) {
		HdOrderRequest req = it->second;
		strFundName = req.FundName.c_str();
		strSubAcntNo = req.SubAccountNo.c_str();
		oriReqNo = req.RequestId;
		reqType = req.RequestType;

		order->SubAccountNo = req.SubAccountNo;
		order->FundName = req.FundName;

		VtOrderQueueManager* orderQMgr = VtOrderQueueManager::GetInstance();
		orderQMgr->RemoveOrderRequest(req.RequestId);

	}
	else {
		LOG_F(INFO, _T("OnAccepted :: Unregistered Request id"));
	}
	SendOrderMessage(VtOrderEvent::Accepted, order);

	OnSubAccountOrder(VtOrderEvent::Accepted, strSubAcntNo, strFundName, order, prevState);

	LOG_F(INFO, _T("거래소 접수 : 원요청번호 %d, 선물사 요청번호 = %d, 종목이름 = %s, 주문 번호 = %s, 계좌번호 = %s, 서브계좌번호 = %s, 펀드 이름 = %s, 주문종류 = %s, 주문갯수 = %s, 요청 타입 = %d"), oriReqNo, nRealType, strSeries, strOrdNo, strAcctNo, strSubAcntNo, strFundName, strPosition.Compare(_T("1")) == 0 ? _T("매수") : _T("매도"), strAmount, order->RequestType);
}

void VtHdCtrl::OnOrderUnfilledHd(VtOrder* order)
{
	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	orderDlgMgr->OnOrderUnfilledHd(order);
}



void VtHdCtrl::OnOrderUnfilledHd(CString& strKey, LONG& nRealType)
{
	CString strAcctNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "계좌번호");
	CString strOrdNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strPosition = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매매구분");
	CString strPrice = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문가격");
	CString strAmount = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문수량");
	CString strCustom = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "사용자정의필드");
	CString strMan = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "조작구분");
	CString strCancelCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "취소수량");
	CString strModyCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "정정수량");
	CString strFilledCnt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결수량");
	CString strRemain = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "잔량");

	CString strOriOrderNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "원주문번호");
	CString strFirstOrderNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "최초원주문번호");
	CString strNewBuyRemain = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "신규매수주문잔량");
	CString strNewBuyTotalRemain = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "신규매수주문총잔량");
	CString strNewSellRemain = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "신규매도주문잔량");
	CString strNewSellTotalRemain = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "신규매도주문총잔량");


	strPrice = strPrice.TrimRight();
	CString strOriOrderPrice;
	strOriOrderPrice = strPrice;
	strPrice.Remove('.');
	strAcctNo.TrimRight();
	strOrdNo.TrimLeft('0');
	strOriOrderNo.TrimLeft('0');
	strFirstOrderNo.TrimLeft('0');

	strSeries.TrimRight();
	strAmount.TrimRight();
	strRemain.TrimRight();
	strModyCnt.TrimRight();
	strFilledCnt.TrimRight();
	strCancelCnt.TrimRight();

	CString strMsg;
	strMsg.Format("OnOrderUnfilledHd 계좌번호[%s]주문번호[%s]\n", strAcctNo, strOrdNo);
	//WriteLog(strMsg);
	//strMsg.Format(_T("%s\n"), strCustom);
	//TRACE(strMsg);

	m_strOrdNo = strOrdNo;

	VtOrderManagerSelector* orderMgrSeledter = VtOrderManagerSelector::GetInstance();
	VtOrderManager* orderMgr = orderMgrSeledter->FindAddOrderManager((LPCTSTR)strAcctNo);

	VtOrder* order = nullptr;
	order = orderMgr->FindOrder(_ttoi(strOrdNo));
	if (!order) {
		order = new VtOrder();
	}
	else {
		// 이 경우는 이미 주문이 있는 경우이다. 체결이 먼저 되었을 경우 이런 경우가 생길 수 있다. 
		// 이 때는 주문의 상태가 체결인 경우는 이 과정을 진행 시키지 않는다.
		if (order->state == VtOrderState::Filled)
			return;
	}

	//order->SubAccountNo = strSubAcntNo;
	//order->FundName = strFundName;

	order->AccountNo = (LPCTSTR)strAcctNo;
	order->shortCode = (LPCTSTR)strSeries;
	order->orderNo = _ttoi(strOrdNo);
	order->intOrderPrice = _ttoi(strPrice);
	order->amount = _ttoi(strAmount);
	order->orderPrice = _ttof(strOriOrderPrice);

	//order->SubAccountNo = strSubAcntNo;
	//order->FundName = strFundName;


	if (strPosition.Compare(_T("1")) == 0)
	{
		order->orderPosition = VtPositionType::Buy;
	}
	else if (strPosition.Compare(_T("2")) == 0)
	{
		order->orderPosition = VtPositionType::Sell;
	}
	order->unacceptedQty = _ttoi(strRemain);
	order->firstOrderNo = _ttoi(strFirstOrderNo);
	order->oriOrderNo = _ttoi(strOriOrderNo);
	order->modifiedOrderCount = _ttoi(strModyCnt);

	int remainCnt = _ttoi(strRemain);
	int cancelCnt = _ttoi(strCancelCnt);
	int modifyCnt = _ttoi(strModyCnt);
	int orderCnt = _ttoi(strAmount);

	if (remainCnt == 0 && modifyCnt == orderCnt)
		order->state = VtOrderState::ConfirmModify;
	if (remainCnt == 0 && cancelCnt == orderCnt) {
		order->unacceptedQty = 0;
		order->amount = 0;
		order->state = VtOrderState::ConfirmCancel;
	}
	VtOrderState prevState = order->state;
	orderMgr->OnOrderUnfilledHd(order);

	SendOrderMessage(VtOrderEvent::Unfilled, order);

	CString strFundName = _T("");
	CString strSubAcntNo = _T("");
	auto it = _ReceivedRequestMap.find(_ttoi(strOrdNo));
	int oriReqNo = -1;
	int reqType = 0;
	// 등록되지 않은 요청 번호는 처리하지 않는다.
	if (it != _ReceivedRequestMap.end()) {
		HdOrderRequest& req = it->second;
		strFundName = req.FundName.c_str();
		strSubAcntNo = req.SubAccountNo.c_str();
		oriReqNo = req.RequestId;
		reqType = req.RequestType;

		order->SubAccountNo = req.SubAccountNo;
		order->FundName = req.FundName;

		CString msg;
		//msg.Format(_T("Unfilled주문타입 = %d, subacnt = %s, fundname = %s \n"), (int)req.orderType, req.SubAccountNo.c_str(), req.FundName.c_str());
		//LOG_F(INFO, _T("OnUnfilled :: Req.Request id = %d, order string = %s"), req.RequestId, msg);
	}
	else {
		LOG_F(INFO, _T("OnUnfilled :: Req.Request id = %d, order string = %s"));
	}

	OnSubAccountOrder(VtOrderEvent::Unfilled, strSubAcntNo, strFundName, order, prevState);

	LOG_F(INFO, _T("미체결 수신 : 원요청번호 %d, 선물사 요청번호 = %d, 종목이름 = %s, 주문 번호 = %s, 계좌번호 = %s, 서브계좌번호 = %s, 펀드 이름 = %s, 주문종류 = %s, 주문갯수 = %s, 요청 타입 = %d"), oriReqNo, nRealType, strSeries, strOrdNo, strAcctNo, strSubAcntNo, strFundName, strPosition.Compare(_T("1")) == 0 ? _T("매수") : _T("매도"), strAmount, order->RequestType);

}

void VtHdCtrl::OnOrderFilledHd(VtOrder* order)
{
	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	orderDlgMgr->OnOrderFilledHd(order);
}



void VtHdCtrl::OnOrderFilledHd(CString& strKey, LONG& nRealType)
{
	CString strAcctNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "계좌번호");
	CString strOrdNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strPosition = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매매구분");


	CString strFillPrice = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결가격");
	CString strFillAmount = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결수량");
	CString strFillTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결시간");
	CString strCustom = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "사용자정의필드");

	CString strMan = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "조작구분");

	strFillPrice = strFillPrice.TrimLeft();
	CString strFill = strFillPrice;
	strFillPrice.Remove('.');
	strAcctNo.TrimRight();
	strOrdNo.TrimLeft('0');
	strSeries.TrimRight();
	strFillPrice.TrimRight();
	strFillAmount.TrimLeft();
	strFillTime.TrimRight();

	CString strMsg;
	strMsg.Format("OnOrderFilledHd 일반 >>>>>> 계좌번호[%s]주문번호[%s] 종목코드 %s, 체결가격 %s, 체결수량 %s,\n", strAcctNo, strOrdNo, strSeries, strFillPrice, strFillAmount);
	//WriteLog(strMsg);
	//strMsg.Format(_T("%s\n"), strCustom);
	//TRACE(strMsg);

	VtOrderManagerSelector* orderMgrSeledter = VtOrderManagerSelector::GetInstance();
	VtOrderManager* orderMgr = orderMgrSeledter->FindAddOrderManager((LPCTSTR)strAcctNo);

	VtOrder* order = nullptr;
	order = orderMgr->FindOrder(_ttoi(strOrdNo));
	// 주문이 주문 목록에 없는 경우는 외부 주문이거나 역전되어 오는 주문이다. 
	// 역전되어 오는 주문은 큐에 넣었다가 주문도착과 주문 미체결, 주문 접수확인 메시지를 추후에 처리 한다.
	if (!order)
	{
		order = new VtOrder();
	}
	order->AccountNo = (LPCTSTR)strAcctNo;
	order->shortCode = (LPCTSTR)strSeries;
	order->orderNo = _ttoi(strOrdNo);

	if (strPosition.Compare(_T("1")) == 0)
	{
		order->orderPosition = VtPositionType::Buy;
	}
	else if (strPosition.Compare(_T("2")) == 0)
	{
		order->orderPosition = VtPositionType::Sell;
	}

	order->intFilledPrice = _ttoi(strFillPrice);
	order->filledQty = _ttoi(strFillAmount);
	order->filledTime = (LPCTSTR)strFillTime;
	order->filledPrice = _ttof(strFill);
	order->state = VtOrderState::Filled;

	//order->SubAccountNo = strSubAcntNo;
	//order->FundName = strFundName;
	VtOrderState prevState = order->state;
	orderMgr->OnOrderFilledHd(order);

	HdWindowManager* wndMgr = HdWindowManager::GetInstance();
	std::map<CWnd*, std::pair<HdWindowType, CWnd*>>& wndMap = wndMgr->GetWindowMap();
	for (auto it = wndMap.begin(); it != wndMap.end(); ++it) {
		auto item = it->second;
		HdWindowType type = item.first;
		CWnd* wnd = item.second;
		if (type == HdWindowType::MiniJangoWindow) {
			((HdAccountPLDlg*)wnd)->OnOrderFilledHd(order);
		}
		else if (type == HdWindowType::FundMiniJangoWindow) {
			((VtFundMiniJango*)wnd)->OnOrderFilledHd(order);
		}
	}


	CString strFundName = _T("");
	CString strSubAcntNo = _T("");
	auto it = _ReceivedRequestMap.find(_ttoi(strOrdNo));
	int oriReqNo = -1;
	int reqType = 0;
	// 등록되지 않은 요청 번호는 처리하지 않는다.
	if (it != _ReceivedRequestMap.end()) {
		HdOrderRequest& req = it->second;
		strFundName = req.FundName.c_str();
		strSubAcntNo = req.SubAccountNo.c_str();
		oriReqNo = req.RequestId;
		reqType = req.RequestType;

		order->SubAccountNo = req.SubAccountNo;
		order->FundName = req.FundName;
		order->requestID = oriReqNo;
		order->RequestType = req.RequestType;

		CString msg;
		msg.Format(_T("filled! 주문타입 = %d, subacnt = %s, fundname = %s \n"), (int)req.orderType, req.SubAccountNo.c_str(), req.FundName.c_str());
		//TRACE(msg);
	}
	else {
		LOG_F(INFO, _T("OnOrderFilledHd등록되지 않은 주문 요청 입니다!"));
		//return;
	}
	SendOrderMessage(VtOrderEvent::Filled, order);

	OnSubAccountOrder(VtOrderEvent::Filled, strSubAcntNo, strFundName, order, prevState);

	//strMsg.Format("OnOrderFilledHd 펀드 >>>>>> 계좌번호[%s]주문번호[%s] 종목코드 %s, 체결가격 %s, 체결수량 %s,\n", strAcctNo, strOrdNo, strSeries, strFillPrice, strFillAmount);

	//TRACE(strMsg);

	LOG_F(INFO, _T("체결 확인 : 원요청번호 %d, 선물사 요청번호 = %d, 종목이름 = %s, 주문 번호 = %s, 계좌번호 = %s, 서브계좌번호 = %s, 펀드 이름 = %s, 주문종류 = %s, 체결갯수 = %s, 요청 타입 = %d"), oriReqNo, nRealType, strSeries, strOrdNo, strAcctNo, strSubAcntNo, strFundName, strPosition.Compare(_T("1")) == 0 ? _T("매수") : _T("매도"), strFillAmount, order->RequestType);

	// 주문 수량과 체결수량이 같을 때만 목록에서 없애 준다.
	if (order->amount == order->filledQty)
		RemoveOrderRequest(order);
}

void VtHdCtrl::AddRequest(int reqId, HdTaskType taskType)
{
	HdTaskArg arg;
	arg.Type = taskType;
	RequestMap[reqId] = arg;
}

void VtHdCtrl::AddRequest(int reqId, HdTaskType taskType, std::string acntNo, std::string symCode)
{
	HdTaskArg arg;
	arg.Type = taskType;
	arg.AddArg(_T("AccountNo"), acntNo);
	arg.AddArg(_T("SymbolCode"), symCode);
	RequestMap[reqId] = arg;
}

void VtHdCtrl::AddRequest(int reqId, HdTaskType taskType, std::string acntNo)
{
	HdTaskArg arg;
	arg.Type = taskType;
	arg.AddArg(_T("AccountNo"), acntNo);
	RequestMap[reqId] = std::move(arg);
}

void VtHdCtrl::RemoveRequest(int reqId)
{
	auto it = RequestMap.find(reqId);
	if (it != RequestMap.end())
	{
		RequestMap.erase(it);
	}
}

void VtHdCtrl::RemoveOrderRequest(int reqId)
{
	auto it = _OrderRequestMap.find(reqId);
	if (it != _OrderRequestMap.end())
	{
		_OrderRequestMap.erase(it);
	}
}

void VtHdCtrl::RemoveOrderRequest(VtOrder* order)
{
	if (!order)
		return;

	auto it = _ReceivedRequestMap.find(order->orderNo);
	if (it != _ReceivedRequestMap.end())
	{
		_ReceivedRequestMap.erase(it);
	}

	it = _ReceivedRequestMap.find(order->oriOrderNo);
	if (it != _ReceivedRequestMap.end())
	{
		_ReceivedRequestMap.erase(it);
	}
}

HdTaskArg VtHdCtrl::FindRequest(int reqId)
{
	HdTaskArg arg;
	arg.Type = HdTaskType::HdNoTask;
	auto it = RequestMap.find(reqId);
	if (it != RequestMap.end())
	{
		arg = it->second;
	}

	return arg;
}

void VtHdCtrl::HandleRealData(std::string symCode, int time, int value)
{
	VtChartDataCollector* dataCollector = VtChartDataCollector::GetInstance();
	dataCollector->OnReceiveData(symCode, time, value);
}

void VtHdCtrl::GetServerTime()
{
	CString sTrCode = _T("o44011");
	CString sInput = _T("     ");
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
}

void VtHdCtrl::RegisterRealtimeDataKey(std::string symCode, std::string dataKey)
{
	auto it = _DataKeyMap.find(symCode);
	if (it != _DataKeyMap.end()) {
		std::set<string>& curSet = it->second;
		curSet.insert(dataKey);
	}
	else {
		std::set<std::string> newSet;
		newSet.insert(dataKey);
		_DataKeyMap[symCode] = newSet;
	}
}

void VtHdCtrl::OnAcceptedHistory(CString& sTrCode, LONG& nRqID)
{
	VtOrderManagerSelector* orderMgrSeledter = VtOrderManagerSelector::GetInstance();

	VtOrder* order = nullptr;

	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	for (int i = 0; i < nRepeatCnt; i++)
	{
		CString strAcctNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌번호");
		CString strOrdNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문번호");
		CString strSeries = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목코드");
		CString strPrice = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문가격");
		CString strAmount = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문수량");
		CString strCustom = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "사용자정의필드");
		CString strPosition = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매매구분");
		CString strPriceType = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "가격구분");
		CString strMan = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "조작구분");
		CString strOriOrderNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "원주문번호");
		CString strFirstOrderNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "최초원주문번호");

		strAcctNo.TrimRight();
		strOrdNo.TrimLeft('0');
		strSeries.TrimRight();

		VtOrderManager* orderMgr = orderMgrSeledter->FindAddOrderManager((LPCTSTR)strAcctNo);

		order = orderMgr->FindOrder(_ttoi(strOrdNo));
		if (!order) {
			order = new VtOrder();

			std::string symCode = (LPCTSTR)strSeries;
			VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
			VtSymbol* sym = symMgr->FindHdSymbol(symCode);

			std::string temp;
			strPrice.TrimLeft('0');
			if (sym)
				temp = NumberFormatter::format(_ttof(strPrice), 20, sym->IntDecimal);
			else
				return;

			strPrice = temp.c_str();
			strPrice.TrimLeft();
			strPrice.Remove('.');

			order->AccountNo = (LPCTSTR)strAcctNo;
			order->shortCode = (LPCTSTR)strSeries;
			order->orderNo = _ttoi(strOrdNo);
			order->intOrderPrice = _ttoi(strPrice);
			order->amount = _ttoi(strAmount);

			if (strPosition.Compare(_T("1")) == 0) {
				order->orderPosition = VtPositionType::Buy;
			}
			else if (strPosition.Compare(_T("2")) == 0) {
				order->orderPosition = VtPositionType::Sell;
			}

			if (strPriceType.Compare(_T("1")) == 0) {
				order->priceType = VtPriceType::Price;
			}
			else if (strPosition.Compare(_T("2")) == 0) {
				order->priceType = VtPriceType::Market;
			}

			order->orderType = VtOrderType::New;

			order->firstOrderNo = _ttoi(strFirstOrderNo);
			order->oriOrderNo = _ttoi(strOriOrderNo);

			orderMgr->OnOrderAcceptedHd(order);

			OnOrderAcceptedHd(order);
		}
	}

	HdTaskEventArgs eventArg;
	eventArg.TaskType = HdTaskType::HdAcceptedHistory;
	FireTaskCompleted(std::move(eventArg));

	RemoveRequest(nRqID);
}

void VtHdCtrl::OnFilledHistory(CString& sTrCode, LONG& nRqID)
{
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	for (int i = 0; i < nRepeatCnt; i++)
	{
		CString strData1 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문번호");
	}

	RemoveRequest(nRqID);
}

void VtHdCtrl::OnFilledHistoryTable(CString& sTrCode, LONG& nRqID)
{
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	VtAccount* acnt = nullptr;
	for (int i = 0; i < nRepeatCnt; i++)
	{
		CString strAcnt = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌번호");
		CString strSymbol = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목코드");
		CString strData3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "당일신규거래수량_매도");
		CString strData4 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "당일신규거래수량_매수");
		CString strData5 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매수평균단가");
		CString strData6 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매도평균단가");
		CString strTradePL = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "당일차금");
		CString strFee = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "수수료");
		CString strPurePL = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "순손익");
		acnt = acntMgr->FindAccount((LPCTSTR)strAcnt.TrimRight());
		if (acnt)
		{
			acnt->TempFee += _ttoi(strFee.TrimRight());
		}
	}

	if (acnt)
	{
		acnt->Fee = acnt->TempFee;
		acnt->TempPurePL = 0;
		acnt->TempTradePL = 0;
		acnt->TempFee = 0.0;
		acnt->SumOpenPL();
	}

	HdTaskArg arg = FindRequest(nRqID);
	if (arg.Type == HdTaskType::HdAccountFeeInfoStep2)
	{
		HdWindowManager* wndMgr = HdWindowManager::GetInstance();
		std::map<CWnd*, std::pair<HdWindowType, CWnd*>>& wndMap = wndMgr->GetWindowMap();
		for (auto it = wndMap.begin(); it != wndMap.end(); ++it)
		{
			auto item = it->second;
			HdWindowType type = item.first;
			CWnd* wnd = item.second;
			if (type == HdWindowType::MiniJangoWindow)
			{
				((HdAccountPLDlg*)wnd)->OnReceiveAccountInfo();
			}
			else if (type == HdWindowType::AssetWindow)
			{
				((VtAccountAssetDlg*)wnd)->OnReceiveAccountInfo();
			}
		}

		VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
		orderDlgMgr->OnReceiveAccountInfo();
	}

	RemoveRequest(nRqID);
}

void VtHdCtrl::OnAccountProfitLoss(CString& sTrCode, LONG& nRqID)
{
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	for (int i = 0; i < nRepeatCnt; i++)
	{
		CString strAcnt = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌번호");
		CString strData2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌명");
		CString strSymbol = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목코드");
		CString strData4 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매매구분");
		CString strData5 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "전환매수량");
		CString strData6 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "현재가");
		CString strTradePL = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "실현손익");
		CString strFee = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "수수료");
		CString strOpenPL = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "평가손익");
		CString strData10 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "순손익");
		CString strData11 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "평균단가");
		CString strData12 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "약정금액");
		CString strData13 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "손익율");

		VtAccount* acnt = acntMgr->FindAccount((LPCTSTR)strAcnt.TrimRight());
		if (acnt)
		{
			VtPosition* posi = acnt->FindAdd((LPCTSTR)strSymbol.TrimRight());

			posi->TradePL = atof(strTradePL.TrimRight());
			posi->Fee = atof(strFee.TrimRight());
			posi->OpenProfitLoss = atof(strOpenPL.TrimRight());
			if (i == nRepeatCnt - 1)
				acnt->SumOpenPL();
		}
	}

	HdTaskEventArgs eventArg;
	eventArg.TaskType = HdTaskType::HdAccountProfitLoss;
	FireTaskCompleted(std::move(eventArg));

	RemoveRequest(nRqID);
}

void VtHdCtrl::OnApiCustomerProfitLoss(CString& sTrCode, LONG& nRqID)
{
	CString strData1 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "예탁총액");
	CString strData2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "예탁현금");
	CString strData3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문가능총액");
	CString strData4 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "위탁증거금_당일");
	CString strData5 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "위탁증거금_익일");
	CString strData6 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "평가예탁총액_순자산");
	CString strData7 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "오버나잇가능금");
	CString strData8 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "당일총손익");
	CString strData9 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "당일매매손익");
	CString strData10 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "당일평가손익");
	CString strData11 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "선물위탁수수료");
	CString strData12 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "옵션위탁수수료");
	CString strData13 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "익일예탁총액");
	CString strData14 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "청산후주문가능총액");

	CString strMsg = strData9 + strData10 + strData8 + _T("\n");
	//TRACE(strMsg);

// 	if (acnt)
// 	{
// 		acnt->Fee = acnt->TempFee;
// 		acnt->TempPurePL = 0;
// 		acnt->TempTradePL = 0;
// 		acnt->TempFee = 0.0;
// 		acnt->SumOpenPL();
// 	}

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	HdTaskArg arg = FindRequest(nRqID);
	if (arg.Type == HdTaskType::HdApiCustomerProfitLoss)
	{
		std::string acntNo = arg.GetArg(_T("AccountNo"));
		VtAccount* acnt = acntMgr->FindAccount(acntNo);
		if (acnt)
		{
			//acnt->OpenPL = _ttoi(strData10.TrimRight());
			//acnt->TradePL = _ttoi(strData9.TrimRight());
			//acnt->Fee = _ttoi(strData11.TrimRight()) + _ttoi(strData12.TrimRight());
			//acnt->TotalPL = _ttoi(strData8.TrimRight());
			acnt->Deposit = _ttoi(strData1.TrimRight());
			acnt->OpenDeposit = _ttoi(strData6.TrimRight());
			acnt->OrdableAmt = _ttoi(strData3.TrimRight());
			acnt->Trst_mgn = _ttoi(strData4.TrimRight());
			acnt->Mnt_mgn = _ttoi(strData4.TrimRight());
			acnt->Add_mgn = _ttoi(strData4.TrimRight());

			//acnt->UpdateRemain();
			double fee = 0.0;
			int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec2");
			for (int i = 0; i < nRepeatCnt; i++)
			{
				CString strCode = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "종목코드");
				CString strPos = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "매매구분");
				CString strRemain = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "잔고수량");
				CString strUnitPrice = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "장부단가");
				CString strCurPrice = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "현재가");
				CString strProfit = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "매매손익");
				CString strFee = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "수수료");
				fee += _ttof(strFee);
				CString strTotalProfit = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "총손익");
				CString strMoney = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "장부금액");
				CString strOpenProfit = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "평가금액");
				CString strSettle = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "청산가능수량");
				VtSymbol* sym = symMgr->FindHdSymbol((LPCTSTR)strCode.TrimRight());
			}

			acnt->Fee = fee;
		}
		HdTaskEventArgs eventArg;
		eventArg.TaskType = HdTaskType::HdApiCustomerProfitLoss;
		FireTaskCompleted(std::move(eventArg));

		RemoveRequest(nRqID);
	}
	else if (arg.Type == HdTaskType::HdAccountFeeInfoStep1)
	{
		std::string acntNo = arg.GetArg(_T("AccountNo"));
		VtAccount* acnt = acntMgr->FindAccount(acntNo);
		if (acnt)
		{
			//acnt->OpenPL = _ttoi(strData10.TrimRight());
			//acnt->TradePL = _ttoi(strData9.TrimRight());
			//acnt->TotalPL = _ttoi(strData8.TrimRight());
			acnt->Deposit = _ttoi(strData1.TrimRight());
			acnt->OpenDeposit = _ttoi(strData6.TrimRight());
			acnt->OrdableAmt = _ttoi(strData3.TrimRight());
			acnt->Trst_mgn = _ttoi(strData4.TrimRight());
			acnt->Mnt_mgn = _ttoi(strData4.TrimRight());
			acnt->Add_mgn = _ttoi(strData4.TrimRight());

			double fee = 0.0;
			int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec2");
			for (int i = 0; i < nRepeatCnt; i++)
			{
				CString strCode = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "종목코드");
				CString strPos = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "매매구분");
				CString strRemain = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "잔고수량");
				CString strUnitPrice = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "장부단가");
				CString strCurPrice = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "현재가");
				CString strProfit = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "매매손익");
				CString strFee = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "수수료");
				fee += _ttof(strFee);
				CString strTotalProfit = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "총손익");
				CString strMoney = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "장부금액");
				CString strOpenProfit = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "평가금액");
				CString strSettle = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "청산가능수량");
				VtSymbol* sym = symMgr->FindHdSymbol((LPCTSTR)strCode.TrimRight());
			}

			acnt->Fee = fee;

			HdTaskEventArgs eventArg;
			eventArg.TaskType = HdTaskType::HdAccountFeeInfoStep1;
			eventArg.Acnt = acnt;
			FireTaskCompleted(std::move(eventArg));

			RemoveRequest(nRqID);
		}
	}

	HdWindowManager* wndMgr = HdWindowManager::GetInstance();
	std::map<CWnd*, std::pair<HdWindowType, CWnd*>>& wndMap = wndMgr->GetWindowMap();
	for (auto it = wndMap.begin(); it != wndMap.end(); ++it)
	{
		auto item = it->second;
		HdWindowType type = item.first;
		CWnd* wnd = item.second;
		if (type == HdWindowType::MiniJangoWindow)
		{
			((HdAccountPLDlg*)wnd)->OnReceiveAccountInfo();
		}
		else if (type == HdWindowType::AssetWindow)
		{
			((VtAccountAssetDlg*)wnd)->OnReceiveAccountInfo();
		}
	}

	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	orderDlgMgr->OnReceiveAccountInfo();
}

void VtHdCtrl::OnServerTime(CString& sTrCode, LONG& nRqID)
{
	CString strDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "서버일자");
	CString strTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "서버시간");
	VtTime curTime = VtGlobal::GetTime(_ttoi(strTime));
	VtDate curDate = VtGlobal::GetDate(_ttoi(strDate));

	SYSTEMTIME st;
	st.wYear = curDate.year;
	st.wMonth = curDate.month;
	st.wDay = curDate.day;
	st.wHour = curTime.hour;
	st.wMinute = curTime.min;
	st.wSecond = curTime.sec;
	st.wMilliseconds = 00;
	::SetSystemTime(&st);
}

void VtHdCtrl::OnOutstandingHistory(CString& sTrCode, LONG& nRqID)
{
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	for (int i = 0; i < nRepeatCnt; i++)
	{
		CString strAcnt = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌번호");
		CString strAcntName = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌명");
		CString strSymbol = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목");
		CString strPosition = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매매구분");
		CString strPreOpenQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "전일미결제수량");
		CString strOpenQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "당일미결제수량");
		CString strAvgPrice = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "평균단가");
		CString strUnitPrice = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "장부단가");

		VtAccount* acnt = acntMgr->FindAccount((LPCTSTR)strAcnt.TrimRight());
		if (acnt)
		{
			VtPosition* posi = acnt->FindAdd((LPCTSTR)strSymbol.TrimRight());
			if (strPosition.Compare(_T("1")) == 0)
			{
				posi->Position = VtPositionType::Buy;
				posi->OpenQty = _ttoi(strOpenQty);
				posi->PrevOpenQty = _ttoi(strPreOpenQty);
			}
			else if (strPosition.Compare(_T("2")) == 0)
			{
				posi->Position = VtPositionType::Sell;
				posi->OpenQty = -1 * _ttoi(strOpenQty);
				posi->PrevOpenQty = -1 * _ttoi(strPreOpenQty);
			}
			posi->AvgPrice = _ttof(strUnitPrice);
			VtSymbol* sym = symMgr->FindHdSymbol((LPCTSTR)strSymbol.TrimRight());
			if (sym)
			{
				posi->CurPrice = sym->Quote.intClose / std::pow(10, sym->IntDecimal);
			}
		}
	}

	HdTaskEventArgs eventArg;
	eventArg.TaskType = HdTaskType::HdOutstandingHistory;
	FireTaskCompleted(std::move(eventArg));

	RemoveRequest(nRqID);
}

void VtHdCtrl::OnOutstanding(CString& sTrCode, LONG& nRqID)
{
	VtRealtimeRegisterManager* realRegMgr = VtRealtimeRegisterManager::GetInstance();
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	for (int i = 0; i < nRepeatCnt; i++)
	{
		CString strAcnt = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌번호");
		CString strAcntName = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌명");
		CString strSymbol = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목");
		CString strPosition = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매매구분");
		CString strPreOpenQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "전일미결제수량");
		CString strOpenQty = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "당일미결제수량");
		CString strAvgPrice = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "평균단가");
		CString strUnitPrice = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "장부단가");
		CString strOpenPL = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "평가손익");

		VtAccount* acnt = acntMgr->FindAccount((LPCTSTR)strAcnt.TrimRight());
		if (acnt) {
			VtPosition* posi = acnt->FindAdd((LPCTSTR)strSymbol.TrimRight());
			realRegMgr->RegisterProduct(posi->ShortCode);
			if (strPosition.Compare(_T("1")) == 0) {
				posi->Position = VtPositionType::Buy;
				posi->OpenQty = _ttoi(strOpenQty);
				posi->PrevOpenQty = _ttoi(strPreOpenQty);
			}
			else if (strPosition.Compare(_T("2")) == 0) {
				posi->Position = VtPositionType::Sell;
				posi->OpenQty = -1 * _ttoi(strOpenQty);
				posi->PrevOpenQty = -1 * _ttoi(strPreOpenQty);
			}
			posi->AvgPrice = _ttof(strUnitPrice);
			VtSymbol* sym = symMgr->FindHdSymbol((LPCTSTR)strSymbol.TrimRight());
			if (sym && sym->Quote.intClose > 0) {
				posi->CurPrice = sym->Quote.intClose / std::pow(10, sym->IntDecimal);
				double curClose = sym->Quote.intClose / std::pow(10, sym->IntDecimal);
				posi->OpenProfitLoss = posi->OpenQty * (curClose - posi->AvgPrice)*sym->seungsu;
				acnt->TempOpenPL += posi->OpenProfitLoss;
				if (i == nRepeatCnt - 1) {
					acnt->OpenPL = acnt->TempOpenPL;
					acnt->TempOpenPL = 0.0;
				}
			}
			else {
				acnt->TempOpenPL += _ttoi(strOpenPL);
				if (i == nRepeatCnt - 1) {
					acnt->OpenPL = acnt->TempOpenPL;
					acnt->TempOpenPL = 0.0;
				}
			}

			VtTotalOrderManager* totalOrderMgr = VtTotalOrderManager::GetInstance();
			totalOrderMgr->AddPosition(0, acnt->AccountNo, posi->ShortCode, posi);

			VtOrderManagerSelector* orderMgrSeledter = VtOrderManagerSelector::GetInstance();
			VtOrderManager* orderMgr = orderMgrSeledter->FindAddOrderManager(acnt->AccountNo);
			VtProductOrderManager* prdtOrderMgr = orderMgr->FindAddProductOrderManager((LPCTSTR)strSymbol.TrimRight());
			if (prdtOrderMgr)
				prdtOrderMgr->Init(true);
		}
	}


	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	orderDlgMgr->OnOutstanding();
	HdTaskEventArgs eventArg;
	eventArg.TaskType = HdTaskType::HdOutstanding;
	FireTaskCompleted(std::move(eventArg));

	RemoveRequest(nRqID);
}

void VtHdCtrl::OnCmeAcceptedHistory(CString& sTrCode, LONG& nRqID)
{
	RemoveRequest(nRqID);
}

void VtHdCtrl::OnCmeFilledHistory(CString& sTrCode, LONG& nRqID)
{
	RemoveRequest(nRqID);
}

void VtHdCtrl::OnCmeOutstandingHistory(CString& sTrCode, LONG& nRqID)
{
	RemoveRequest(nRqID);
}

void VtHdCtrl::OnCmeOutstanding(CString& sTrCode, LONG& nRqID)
{
	RemoveRequest(nRqID);
}

void VtHdCtrl::OnAsset(CString& sTrCode, LONG& nRqID)
{
	CString strData1 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "계좌번호");
	CString strData2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "계좌명");
	CString strData3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "예탁총액");
	CString strData4 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "예탁현금");
	CString strData5 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "예탁대용");
	CString strData6 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "가결제금액");
	CString strData7 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "전환매손익");
	CString strData8 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "위탁증거금");
	CString strData9 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "장중선물옵션수수료");
	CString strData10 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "보유포지션에대한위탁증거금");
	CString strData11 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문증거금");
	CString strData12 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문가능총액");
	CString strData13 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "선입선출에의한전환매손익");
	CString strData14 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "장중옵션매수도대금");
	CString strData15 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "평가손익");
	CString strData16 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "총손익");
	CString strData17 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "장중선물옵션수수료2");
	CString strData18 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "순손익");
	CString strData19 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "평가예탁총액");
	CString strData20 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "인출가능총액");
	CString strData21 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "인출가능현금");

	HdTaskEventArgs eventArg;
	eventArg.TaskType = HdTaskType::HdAsset;
	FireTaskCompleted(std::move(eventArg));

	RemoveRequest(nRqID);
}

void VtHdCtrl::OnDeposit(CString& sTrCode, LONG& nRqID)
{
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	for (int i = 0; i < nRepeatCnt; i++)
	{
		CString strAccount = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌번호");
		CString strData2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "예탁금액-총액");
		CString strData3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "예탁금액-현금");
		CString strData4 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "예탁금액-대용");
		CString strData5 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "예탁외화");
		CString strData6 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "당일손익");
		CString strData7 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "위탁수수료");
		CString strData8 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "당일순손익");
		CString strData9 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "평가예탁총액");
		CString strData10 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "순자산-총평가액");
		CString strData11 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문가능금액");

		VtAccount* acnt = acntMgr->FindAccount((LPCTSTR)strAccount.TrimRight());
		if (acnt)
		{
			acnt->OpenDeposit = _ttof(strData9.TrimRight());
			acnt->OrdableAmt = _ttof(strData11.TrimRight());
			/*
			acnt->OpenPL = _ttof(strData6.TrimRight());
			acnt->TradePL = _ttof(strData6.TrimRight());
			acnt->TotalPL = _ttof(strData6.TrimRight());
			*/
		}
	}
	Sleep(700);
	HdTaskEventArgs eventArg;
	eventArg.TaskType = HdTaskType::HdDeposit;
	FireTaskCompleted(std::move(eventArg));
	RemoveRequest(nRqID);
}

void VtHdCtrl::OnCmeAsset(CString& sTrCode, LONG& nRqID)
{
	RemoveRequest(nRqID);
}

void VtHdCtrl::OnCmePureAsset(CString& sTrCode, LONG& nRqID)
{
	RemoveRequest(nRqID);
}

void VtHdCtrl::OnDailyProfitLoss(CString& sTrCode, LONG& nRqID)
{
	VtAccountManager* acntMgr = VtAccountManager::GetInstance();
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	for (int i = 0; i < nRepeatCnt; i++)
	{
		CString strAcnt = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌번호");
		CString strData2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌명");
		CString strSymbol = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목");
		CString strTotalPL = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "총손익");
		CString strData6 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "수수료");
		CString strData7 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "순손익");
		strAcnt.Remove('-');
		VtAccount* acnt = acntMgr->FindAccount((LPCTSTR)strAcnt.TrimRight());
		if (acnt)
		{
			VtPosition* posi = acnt->FindAdd((LPCTSTR)strSymbol.TrimRight());

			posi->TradePL = atof(strTotalPL.TrimRight());

			acnt->SumOpenPL();
		}
	}

	HdTaskEventArgs eventArg;
	eventArg.TaskType = HdTaskType::HdDailyProfitLoss;
	FireTaskCompleted(std::move(eventArg));

	RemoveRequest(nRqID);
}

void VtHdCtrl::OnSymbolCode(CString& sTrCode, LONG& nRqID)
{
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	if (nRepeatCnt > 0) {
		VtSymbol* sym = nullptr;
		for (int i = 0; i < nRepeatCnt; i++) {
			CString sData = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목코드");
			//TRACE(sData);
			//TRACE(_T("\n"));
			//WriteLog(sData);
			sym = new VtSymbol();
			sym->ShortCode = sData.Trim();

			auto ym = VtSymbolManager::GetExpireYearMonth(sym->ShortCode);
			sym->ExpireYear = std::get<0>(ym);
			sym->ExpireMonth = std::get<1>(ym);

			symMgr->AddHdSymbol(sym);

			std::string symCode = sym->ShortCode;
			std::string subSecCode = symCode.substr(0, 3);
			VtProductCategoryManager* prdtCatMgr = VtProductCategoryManager::GetInstance();
			VtProductSubSection* subSection = prdtCatMgr->FindProductSubSection(subSecCode);
			if (subSection) {
				subSection->AddSymbol(sym);
			}
		}

		HdTaskEventArgs eventArg;
		eventArg.TaskType = HdTaskType::HdSymbolCode;
		eventArg.RequestId = nRqID;
		FireTaskCompleted(std::move(eventArg));
		Sleep(700);
	}

	RemoveRequest(nRqID);
}



void VtHdCtrl::OnSymbolMaster(CString& sTrCode, LONG& nRqID)
{
	int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
	CString	strData000 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "단축코드");

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindSymbol((LPCTSTR)strData000.Trim());
	if (!sym)
		return;
	// 	if (sym->RecentMonth())
	// 	{
	// 		VtRealtimeRegisterManager* realTimeRegiMgr = VtRealtimeRegisterManager::GetInstance();
	// 		realTimeRegiMgr->RegisterProduct(sym->ShortCode);
	// 	}

	CString	strData001 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "종목코드");
	CString	strData002 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "한글종목명");
	CString	strData003 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "종목한글약명");
	CString	strData004 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "영문종목명");
	CString	strData005 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "축약종목명");
	CString strCom = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "전일대비");
	CString strUpRate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "등락률");

	LOG_F(INFO, _T("종목코드 = %s"), strData001);
	//TRACE(_T("\n"));
	std::string code = sym->ShortCode.substr(0, 3);
	HdProductInfo* prdtInfo = symMgr->FindProductInfo(code);
	if (prdtInfo) {
		sym->IntDecimal = prdtInfo->decimal;
		sym->seungsu = prdtInfo->tradeWin;
		sym->intTickSize = prdtInfo->intTickSize;
		sym->intTickValue = prdtInfo->tickValue;
		sym->TickSize = prdtInfo->tickSize;
	}
	strCom.TrimRight();
	strUpRate.TrimRight();


	sym->FullCode = (LPCTSTR)strData001.TrimRight();
	sym->Name = (LPCTSTR)strData002.TrimRight();
	sym->ShortName = (LPCTSTR)strData003.TrimRight();
	sym->EngName = (LPCTSTR)strData004.TrimRight();
	sym->BriefName = (LPCTSTR)strData005.TrimRight();




	CString	strData049 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "거래단위");

	CString	strData050 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "체결시간");
	CString	strData051 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "현재가");
	CString	strData052 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "시가");
	CString	strData053 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "고가");
	CString	strData054 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "저가");
	CString strPreClose = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "전일종가");
	CString strPreHigh = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "전일고가");
	CString strPreLow = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "전일저가");


	sym->ComToPrev = _ttoi(strCom);
	sym->UpdownRate = _ttoi(strUpRate);
	sym->Quote.intClose = _ttoi(strData051);
	sym->Quote.intOpen = _ttoi(strData052);
	sym->Quote.intHigh = _ttoi(strData053);
	sym->Quote.intLow = _ttoi(strData054);
	sym->Quote.intPreClose = _ttoi(strPreClose);

	std::string midCode = sym->ShortCode.substr(0, 3);
	if (midCode.compare(_T("101")) == 0) {
		symMgr->Kospi200Current = sym->Quote.intClose;
	}

	CString	strData075 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "호가수신시간");
	CString	strData076 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가1");
	CString	strData077 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가1");
	CString	strData078 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가수량1");
	CString	strData079 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가수량1");
	CString	strData080 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수1");
	CString	strData081 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수1");

	sym->Hoga.Ary[0].IntSellPrice = _ttoi(strData076);
	sym->Hoga.Ary[0].IntBuyPrice = _ttoi(strData077);
	sym->Hoga.Ary[0].SellQty = _ttoi(strData078);
	sym->Hoga.Ary[0].BuyQty = _ttoi(strData079);
	sym->Hoga.Ary[0].SellNo = _ttoi(strData080);
	sym->Hoga.Ary[0].BuyNo = _ttoi(strData081);


	CString	strData082 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가2");
	CString	strData083 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가2");
	CString	strData084 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가수량2");
	CString	strData085 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가수량2");
	CString	strData086 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수2");
	CString	strData087 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수2");

	sym->Hoga.Ary[1].IntSellPrice = _ttoi(strData082);
	sym->Hoga.Ary[1].IntBuyPrice = _ttoi(strData083);
	sym->Hoga.Ary[1].SellQty = _ttoi(strData084);
	sym->Hoga.Ary[1].BuyQty = _ttoi(strData085);
	sym->Hoga.Ary[1].SellNo = _ttoi(strData086);
	sym->Hoga.Ary[1].BuyNo = _ttoi(strData087);
	CString	strData088 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가3");
	CString	strData089 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가3");
	CString	strData090 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가수량3");
	CString	strData091 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가수량3");
	CString	strData092 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수3");
	CString	strData093 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수3");

	sym->Hoga.Ary[2].IntSellPrice = _ttoi(strData088);
	sym->Hoga.Ary[2].IntBuyPrice = _ttoi(strData089);
	sym->Hoga.Ary[2].SellQty = _ttoi(strData090);
	sym->Hoga.Ary[2].BuyQty = _ttoi(strData091);
	sym->Hoga.Ary[2].SellNo = _ttoi(strData092);
	sym->Hoga.Ary[2].BuyNo = _ttoi(strData093);
	CString	strData094 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가4");
	CString	strData095 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가4");
	CString	strData096 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가수량4");
	CString	strData097 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가수량4");
	CString	strData098 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수4");
	CString	strData099 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수4");

	sym->Hoga.Ary[3].IntSellPrice = _ttoi(strData094);
	sym->Hoga.Ary[3].IntBuyPrice = _ttoi(strData095);
	sym->Hoga.Ary[3].SellQty = _ttoi(strData096);
	sym->Hoga.Ary[3].BuyQty = _ttoi(strData097);
	sym->Hoga.Ary[3].SellNo = _ttoi(strData098);
	sym->Hoga.Ary[3].BuyNo = _ttoi(strData099);
	CString	strData100 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가5");
	CString	strData101 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가5");
	CString	strData102 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가수량5");
	CString	strData103 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가수량5");
	CString	strData104 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가건수5");
	CString	strData105 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가건수5");
	sym->Hoga.Ary[4].IntSellPrice = _ttoi(strData100);
	sym->Hoga.Ary[4].IntBuyPrice = _ttoi(strData101);
	sym->Hoga.Ary[4].SellQty = _ttoi(strData102);
	sym->Hoga.Ary[4].BuyQty = _ttoi(strData103);
	sym->Hoga.Ary[4].SellNo = _ttoi(strData104);
	sym->Hoga.Ary[4].BuyNo = _ttoi(strData105);

	CString	strData106 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가총수량");
	CString	strData107 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가총수량");
	CString	strData108 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도호가총건수");
	CString	strData109 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수호가총건수");

	sym->Hoga.TotSellQty = _ttoi(strData106);
	sym->Hoga.TotBuyQty = _ttoi(strData107);
	sym->Hoga.TotSellNo = _ttoi(strData108);
	sym->Hoga.TotBuyNo = _ttoi(strData109);


	CString	strData133 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "만기년월");
	//TRACE(strData133);
	//TRACE(_T("\n"));

	CString	strData159 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "KOSPI200지수");
	//TRACE(strData159);
	//TRACE(_T("\n"));

	char firstCode = sym->ShortCode.at(0);

	if (_FutureGrid && firstCode == '1') {
		_FutureGrid->OnSymbolMaster(sym);
	}

	if (_OptionGrid && (firstCode == '2' || firstCode == '3')) {
		_OptionGrid->OnSymbolMaster(sym);
	}

	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	orderDlgMgr->OnSymbolMaster(sym);

	HdWindowManager* wndMgr = HdWindowManager::GetInstance();
	std::map<CWnd*, std::pair<HdWindowType, CWnd*>>& wndMap = wndMgr->GetWindowMap();
	for (auto it = wndMap.begin(); it != wndMap.end(); ++it) {
		auto item = it->second;
		HdWindowType type = item.first;
		CWnd* wnd = item.second;
		if (type == HdWindowType::MiniJangoWindow) {
			((HdAccountPLDlg*)wnd)->OnSymbolMaster(sym);
		}
	}

	HdTaskEventArgs eventArg;
	eventArg.TaskType = HdTaskType::HdSymbolMaster;
	FireTaskCompleted(std::move(eventArg));
	//TRACE(_T("MasterCompleted!\n"));

	RemoveRequest(nRqID);
}

void VtHdCtrl::OnFutureHoga(CString& strKey, LONG& nRealType)
{
	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindSymbol((LPCTSTR)strSeries.Trim());
	if (!sym)
		return;

	CString	strData076 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가1");
	CString	strData077 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가1");
	CString	strData078 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량1");
	CString	strData079 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량1");
	CString	strData080 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수1");
	CString	strData081 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수1");

	sym->Hoga.Ary[0].IntSellPrice = _ttoi(strData076);
	sym->Hoga.Ary[0].IntBuyPrice = _ttoi(strData077);
	sym->Hoga.Ary[0].SellQty = _ttoi(strData078);
	sym->Hoga.Ary[0].BuyQty = _ttoi(strData079);
	sym->Hoga.Ary[0].SellNo = _ttoi(strData080);
	sym->Hoga.Ary[0].BuyNo = _ttoi(strData081);


	CString	strData082 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가2");
	CString	strData083 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가2");
	CString	strData084 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량2");
	CString	strData085 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량2");
	CString	strData086 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수2");
	CString	strData087 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수2");

	sym->Hoga.Ary[1].IntSellPrice = _ttoi(strData082);
	sym->Hoga.Ary[1].IntBuyPrice = _ttoi(strData083);
	sym->Hoga.Ary[1].SellQty = _ttoi(strData084);
	sym->Hoga.Ary[1].BuyQty = _ttoi(strData085);
	sym->Hoga.Ary[1].SellNo = _ttoi(strData086);
	sym->Hoga.Ary[1].BuyNo = _ttoi(strData087);
	CString	strData088 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가3");
	CString	strData089 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가3");
	CString	strData090 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량3");
	CString	strData091 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량3");
	CString	strData092 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수3");
	CString	strData093 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수3");

	sym->Hoga.Ary[2].IntSellPrice = _ttoi(strData088);
	sym->Hoga.Ary[2].IntBuyPrice = _ttoi(strData089);
	sym->Hoga.Ary[2].SellQty = _ttoi(strData090);
	sym->Hoga.Ary[2].BuyQty = _ttoi(strData091);
	sym->Hoga.Ary[2].SellNo = _ttoi(strData092);
	sym->Hoga.Ary[2].BuyNo = _ttoi(strData093);
	CString	strData094 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가4");
	CString	strData095 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가4");
	CString	strData096 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량4");
	CString	strData097 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량4");
	CString	strData098 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수4");
	CString	strData099 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수4");

	sym->Hoga.Ary[3].IntSellPrice = _ttoi(strData094);
	sym->Hoga.Ary[3].IntBuyPrice = _ttoi(strData095);
	sym->Hoga.Ary[3].SellQty = _ttoi(strData096);
	sym->Hoga.Ary[3].BuyQty = _ttoi(strData097);
	sym->Hoga.Ary[3].SellNo = _ttoi(strData098);
	sym->Hoga.Ary[3].BuyNo = _ttoi(strData099);
	CString	strData100 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가5");
	CString	strData101 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가5");
	CString	strData102 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량5");
	CString	strData103 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량5");
	CString	strData104 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수5");
	CString	strData105 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수5");
	sym->Hoga.Ary[4].IntSellPrice = _ttoi(strData100);
	sym->Hoga.Ary[4].IntBuyPrice = _ttoi(strData101);
	sym->Hoga.Ary[4].SellQty = _ttoi(strData102);
	sym->Hoga.Ary[4].BuyQty = _ttoi(strData103);
	sym->Hoga.Ary[4].SellNo = _ttoi(strData104);
	sym->Hoga.Ary[4].BuyNo = _ttoi(strData105);

	CString strTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "호가시간");

	CString	strData106 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가총수량");
	CString	strData107 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가총수량");
	CString	strData108 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가총건수");
	CString	strData109 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가총건수");

	sym->Hoga.TotSellQty = _ttoi(strData106);
	sym->Hoga.TotBuyQty = _ttoi(strData107);
	sym->Hoga.TotSellNo = _ttoi(strData108);
	sym->Hoga.TotBuyNo = _ttoi(strData109);


	// 	VtChartDataCollector* dataCollector = VtChartDataCollector::GetInstance();
	// 	std::string symCode = sym->ShortCode;
	// 	std::string code = symCode + _T("SHTQ:5:1");
	// 	dataCollector->OnReceiveData(code, _ttoi(strTime), sym->Hoga.TotSellQty);
	// 	code = symCode + _T("BHTQ:5:1");
	// 	dataCollector->OnReceiveData(code, _ttoi(strTime), sym->Hoga.TotBuyQty);
	// 	code = symCode + _T("SHTC:5:1");
	// 	dataCollector->OnReceiveData(code, _ttoi(strTime), sym->Hoga.TotSellNo);
	// 	code = symCode + _T("BHTC:5:1");
	// 	dataCollector->OnReceiveData(code, _ttoi(strTime), sym->Hoga.TotBuyNo);

	OnReceiveHoga(_ttoi(strTime), sym);

	orderDlgMgr->OnReceiveHoga(sym);
}

void VtHdCtrl::OnOptionHoga(CString& strKey, LONG& nRealType)
{
	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindSymbol((LPCTSTR)strSeries.Trim());
	if (!sym)
		return;

	CString	strData076 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가1");
	CString	strData077 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가1");
	CString	strData078 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량1");
	CString	strData079 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량1");
	CString	strData080 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수1");
	CString	strData081 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수1");

	sym->Hoga.Ary[0].IntSellPrice = _ttoi(strData076);
	sym->Hoga.Ary[0].IntBuyPrice = _ttoi(strData077);
	sym->Hoga.Ary[0].SellQty = _ttoi(strData078);
	sym->Hoga.Ary[0].BuyQty = _ttoi(strData079);
	sym->Hoga.Ary[0].SellNo = _ttoi(strData080);
	sym->Hoga.Ary[0].BuyNo = _ttoi(strData081);


	CString	strData082 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가2");
	CString	strData083 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가2");
	CString	strData084 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량2");
	CString	strData085 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량2");
	CString	strData086 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수2");
	CString	strData087 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수2");

	sym->Hoga.Ary[1].IntSellPrice = _ttoi(strData082);
	sym->Hoga.Ary[1].IntBuyPrice = _ttoi(strData083);
	sym->Hoga.Ary[1].SellQty = _ttoi(strData084);
	sym->Hoga.Ary[1].BuyQty = _ttoi(strData085);
	sym->Hoga.Ary[1].SellNo = _ttoi(strData086);
	sym->Hoga.Ary[1].BuyNo = _ttoi(strData087);
	CString	strData088 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가3");
	CString	strData089 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가3");
	CString	strData090 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량3");
	CString	strData091 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량3");
	CString	strData092 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수3");
	CString	strData093 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수3");

	sym->Hoga.Ary[2].IntSellPrice = _ttoi(strData088);
	sym->Hoga.Ary[2].IntBuyPrice = _ttoi(strData089);
	sym->Hoga.Ary[2].SellQty = _ttoi(strData090);
	sym->Hoga.Ary[2].BuyQty = _ttoi(strData091);
	sym->Hoga.Ary[2].SellNo = _ttoi(strData092);
	sym->Hoga.Ary[2].BuyNo = _ttoi(strData093);
	CString	strData094 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가4");
	CString	strData095 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가4");
	CString	strData096 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량4");
	CString	strData097 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량4");
	CString	strData098 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수4");
	CString	strData099 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수4");

	sym->Hoga.Ary[3].IntSellPrice = _ttoi(strData094);
	sym->Hoga.Ary[3].IntBuyPrice = _ttoi(strData095);
	sym->Hoga.Ary[3].SellQty = _ttoi(strData096);
	sym->Hoga.Ary[3].BuyQty = _ttoi(strData097);
	sym->Hoga.Ary[3].SellNo = _ttoi(strData098);
	sym->Hoga.Ary[3].BuyNo = _ttoi(strData099);
	CString	strData100 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가5");
	CString	strData101 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가5");
	CString	strData102 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가수량5");
	CString	strData103 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가수량5");
	CString	strData104 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가건수5");
	CString	strData105 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가건수5");
	sym->Hoga.Ary[4].IntSellPrice = _ttoi(strData100);
	sym->Hoga.Ary[4].IntBuyPrice = _ttoi(strData101);
	sym->Hoga.Ary[4].SellQty = _ttoi(strData102);
	sym->Hoga.Ary[4].BuyQty = _ttoi(strData103);
	sym->Hoga.Ary[4].SellNo = _ttoi(strData104);
	sym->Hoga.Ary[4].BuyNo = _ttoi(strData105);

	CString strTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "호가시간");
	CString	strData106 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가총수량");
	CString	strData107 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가총수량");
	CString	strData108 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가총건수");
	CString	strData109 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가총건수");

	sym->Hoga.TotSellQty = _ttoi(strData106);
	sym->Hoga.TotBuyQty = _ttoi(strData107);
	sym->Hoga.TotSellNo = _ttoi(strData108);
	sym->Hoga.TotBuyNo = _ttoi(strData109);

	// 	VtChartDataCollector* dataCollector = VtChartDataCollector::GetInstance();
	// 	std::string symCode = sym->ShortCode;
	// 	std::string code = symCode + _T("SHTQ:5:1");
	// 	dataCollector->OnReceiveData(code, _ttoi(strTime), sym->Hoga.TotSellQty);
	// 	code = symCode + _T("BHTQ:5:1");
	// 	dataCollector->OnReceiveData(code, _ttoi(strTime), sym->Hoga.TotBuyQty);
	// 	code = symCode + _T("SHTC:5:1");
	// 	dataCollector->OnReceiveData(code, _ttoi(strTime), sym->Hoga.TotSellNo);
	// 	code = symCode + _T("BHTC:5:1");
	// 	dataCollector->OnReceiveData(code, _ttoi(strTime), sym->Hoga.TotBuyNo);

	OnReceiveHoga(_ttoi(strTime), sym);

	orderDlgMgr->OnReceiveHoga(sym);
}

void VtHdCtrl::OnProductHoga(CString& strKey, LONG& nRealType)
{
	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "kfutcode");

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindSymbol((LPCTSTR)strSeries.Trim());
	if (!sym)
		return;

	CString	strData076 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerho1");
	CString	strData077 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidho1");
	CString	strData078 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerrem1");
	CString	strData079 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidrem1");
	CString	strData080 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offercnt1");
	CString	strData081 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidcnt1");

	sym->Hoga.Ary[0].IntSellPrice = _ttoi(strData076);
	sym->Hoga.Ary[0].IntBuyPrice = _ttoi(strData077);
	sym->Hoga.Ary[0].SellQty = _ttoi(strData078);
	sym->Hoga.Ary[0].BuyQty = _ttoi(strData079);
	sym->Hoga.Ary[0].SellNo = _ttoi(strData080);
	sym->Hoga.Ary[0].BuyNo = _ttoi(strData081);


	CString	strData082 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerho2");
	CString	strData083 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidho2");
	CString	strData084 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerrem2");
	CString	strData085 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidrem2");
	CString	strData086 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offercnt2");
	CString	strData087 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidcnt2");

	sym->Hoga.Ary[1].IntSellPrice = _ttoi(strData082);
	sym->Hoga.Ary[1].IntBuyPrice = _ttoi(strData083);
	sym->Hoga.Ary[1].SellQty = _ttoi(strData084);
	sym->Hoga.Ary[1].BuyQty = _ttoi(strData085);
	sym->Hoga.Ary[1].SellNo = _ttoi(strData086);
	sym->Hoga.Ary[1].BuyNo = _ttoi(strData087);
	CString	strData088 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerho3");
	CString	strData089 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidho3");
	CString	strData090 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerrem3");
	CString	strData091 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidrem3");
	CString	strData092 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offercnt3");
	CString	strData093 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidcnt3");

	sym->Hoga.Ary[2].IntSellPrice = _ttoi(strData088);
	sym->Hoga.Ary[2].IntBuyPrice = _ttoi(strData089);
	sym->Hoga.Ary[2].SellQty = _ttoi(strData090);
	sym->Hoga.Ary[2].BuyQty = _ttoi(strData091);
	sym->Hoga.Ary[2].SellNo = _ttoi(strData092);
	sym->Hoga.Ary[2].BuyNo = _ttoi(strData093);
	CString	strData094 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerho4");
	CString	strData095 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidho4");
	CString	strData096 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerrem4");
	CString	strData097 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidrem4");
	CString	strData098 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offercnt4");
	CString	strData099 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidcnt4");

	sym->Hoga.Ary[3].IntSellPrice = _ttoi(strData094);
	sym->Hoga.Ary[3].IntBuyPrice = _ttoi(strData095);
	sym->Hoga.Ary[3].SellQty = _ttoi(strData096);
	sym->Hoga.Ary[3].BuyQty = _ttoi(strData097);
	sym->Hoga.Ary[3].SellNo = _ttoi(strData098);
	sym->Hoga.Ary[3].BuyNo = _ttoi(strData099);
	CString	strData100 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerho5");
	CString	strData101 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidho5");
	CString	strData102 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offerrem5");
	CString	strData103 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidrem5");
	CString	strData104 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "offercnt5");
	CString	strData105 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "bidcnt5");
	sym->Hoga.Ary[4].IntSellPrice = _ttoi(strData100);
	sym->Hoga.Ary[4].IntBuyPrice = _ttoi(strData101);
	sym->Hoga.Ary[4].SellQty = _ttoi(strData102);
	sym->Hoga.Ary[4].BuyQty = _ttoi(strData103);
	sym->Hoga.Ary[4].SellNo = _ttoi(strData104);
	sym->Hoga.Ary[4].BuyNo = _ttoi(strData105);

	CString	strData106 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "totofferrem");
	CString	strData107 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "totbidrem");
	CString	strData108 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "totoffercnt");
	CString	strData109 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "totbidcnt");

	sym->Hoga.TotSellQty = _ttoi(strData106);
	sym->Hoga.TotBuyQty = _ttoi(strData107);
	sym->Hoga.TotSellNo = _ttoi(strData108);
	sym->Hoga.TotBuyNo = _ttoi(strData109);

	CString strTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "호가시간");

	// 	VtChartDataCollector* dataCollector = VtChartDataCollector::GetInstance();
	// 	std::string symCode = sym->ShortCode;
	// 	std::string code = symCode + _T("SHTQ:5:1");
	// 	dataCollector->OnReceiveData(code, _ttoi(strTime), sym->Hoga.TotSellQty);
	// 	code = symCode + _T("BHTQ:5:1");
	// 	dataCollector->OnReceiveData(code, _ttoi(strTime), sym->Hoga.TotBuyQty);
	// 	code = symCode + _T("SHTC:5:1");
	// 	dataCollector->OnReceiveData(code, _ttoi(strTime), sym->Hoga.TotSellNo);
	// 	code = symCode + _T("BHTC:5:1");
	// 	dataCollector->OnReceiveData(code, _ttoi(strTime), sym->Hoga.TotBuyNo);

	OnReceiveHoga(_ttoi(strTime), sym);

	orderDlgMgr->OnReceiveHoga(sym);
}

// 잔고는 매도한번 매수한번 두번이 온다.
void VtHdCtrl::OnRemain(CString& strKey, LONG& nRealType)
{
	CString strMsg;
	strMsg.Format("리얼번호[%d] 수신!", nRealType);
	//WriteLog(strMsg);

	CString strAccount = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "계좌번호");
	CString strSymbol = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목");
	CString strRemainCount = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "당일미결제수량");
	CString strPosition = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매매구분");

	CString strData5 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "미체결수량");
	CString strAvg = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "평균단가");
	CString strPreRemainCount = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "전일미결제수량");
	CString strRemainBalance = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "당일미결제약정금액");
	CString strUnitPrice = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "장부단가");
	int count = _ttoi(strRemainCount.TrimRight());
	// 매수와 매도가 번갈아 오므로 잔고가 0인 포지션은 처리하지 않는다.
	if (count > 0) {
		VtAccountManager* acntMgr = VtAccountManager::GetInstance();
		VtAccount* acnt = acntMgr->FindAccount((LPCTSTR)strAccount.TrimRight());
		if (acnt) {
			VtPosition* posi = acnt->FindAdd((LPCTSTR)strSymbol.TrimRight());
			VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
			orderDlgMgr->OnRemain(posi);


			LOG_F(INFO, _T("잔고 표시 >> 계좌번호 : %s, 종목코드 = %s, 포지션 = %s, 갯수 = %s"), strAccount, strSymbol, posi->Position == VtPositionType::Buy ? _T("매수") : _T("매도"), strRemainCount);
		}
	}
}

void VtHdCtrl::OnRealFutureQuote(CString& strKey, LONG& nRealType)
{
	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결시간");
	CString strVolume = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결량");
	CString strUpdown = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결구분");

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindSymbol((LPCTSTR)strSeries.Trim());
	if (!sym)
		return;

	CString	strData051 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "현재가");
	CString	strData052 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "시가");
	CString	strData053 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "고가");
	CString	strData054 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "저가");
	CString strCom = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "전일대비");
	CString strUpRate = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "등락율");
	sym->ComToPrev = _ttoi(strCom);
	sym->UpdownRate = _ttoi(strUpRate);
	sym->Quote.intClose = _ttoi(strData051);
	sym->Quote.intOpen = _ttoi(strData052);
	sym->Quote.intHigh = _ttoi(strData053);
	sym->Quote.intLow = _ttoi(strData054);

	sym->Quote.close = sym->Quote.intClose / std::pow(10, sym->IntDecimal);
	sym->Quote.open = sym->Quote.intOpen / std::pow(10, sym->IntDecimal);
	sym->Quote.high = sym->Quote.intHigh / std::pow(10, sym->IntDecimal);
	sym->Quote.low = sym->Quote.intLow / std::pow(10, sym->IntDecimal);

	OnReceiveSise(_ttoi(strTime), sym);

	VtQuoteItem quoteItem;

	if (strUpdown.Compare(_T("+")) == 0)
		quoteItem.MatchKind = 1;
	else
		quoteItem.MatchKind = 2;

	quoteItem.ClosePrice = _ttoi(strData051);

	strTime.Insert(2, ':');
	strTime.Insert(5, ':');

	quoteItem.Time = strTime;


	quoteItem.ContQty = _ttoi(strVolume);


	quoteItem.Decimal = sym->IntDecimal;

	sym->Quote.QuoteItemQ.push_front(quoteItem);

	VtOrderManagerSelector* orderMgrSelector = VtOrderManagerSelector::GetInstance();
	for (auto it = orderMgrSelector->_OrderManagerMap.begin(); it != orderMgrSelector->_OrderManagerMap.end(); ++it)
	{
		VtOrderManager* orderMgr = it->second;
		orderMgr->OnReceiveQuoteHd(sym);
	}

	HdWindowManager* wndMgr = HdWindowManager::GetInstance();
	std::map<CWnd*, std::pair<HdWindowType, CWnd*>>& wndMap = wndMgr->GetWindowMap();
	for (auto it = wndMap.begin(); it != wndMap.end(); ++it)
	{
		auto item = it->second;
		HdWindowType type = item.first;
		CWnd* wnd = item.second;
		if (type == HdWindowType::MiniJangoWindow)
		{
			((HdAccountPLDlg*)wnd)->OnReceiveQuote(sym);
		}
		else if (type == HdWindowType::FundMiniJangoWindow)
		{
			((VtFundMiniJango*)wnd)->OnReceiveQuote(sym);
		}
		else if (type == HdWindowType::AssetWindow)
		{
			((VtAccountAssetDlg*)wnd)->OnReceiveAccountInfo();
		}
	}

	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	orderDlgMgr->OnReceiveQuote(sym);

	VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
	chartDataMgr->OnReceiveQuoteHd(sym);

	CMainFrame* mainFrm = (CMainFrame*)AfxGetMainWnd();
	mainFrm->OnReceiveQuoteHd(sym);
}

void VtHdCtrl::OnRealOptionQuote(CString& strKey, LONG& nRealType)
{
	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결시간");
	CString strVolume = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결량");
	CString strUpdown = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결구분");

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindSymbol((LPCTSTR)strSeries.Trim());
	if (!sym)
		return;

	CString	strData051 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "현재가");
	CString	strData052 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "시가");
	CString	strData053 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "고가");
	CString	strData054 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "저가");

	sym->Quote.intClose = _ttoi(strData051);
	sym->Quote.intOpen = _ttoi(strData052);
	sym->Quote.intHigh = _ttoi(strData053);
	sym->Quote.intLow = _ttoi(strData054);

	sym->Quote.close = sym->Quote.intClose / std::pow(10, sym->IntDecimal);
	sym->Quote.open = sym->Quote.intOpen / std::pow(10, sym->IntDecimal);
	sym->Quote.high = sym->Quote.intHigh / std::pow(10, sym->IntDecimal);
	sym->Quote.low = sym->Quote.intLow / std::pow(10, sym->IntDecimal);

	OnReceiveSise(_ttoi(strTime), sym);

	VtQuoteItem quoteItem;

	if (strUpdown.Compare(_T("+")) == 0)
		quoteItem.MatchKind = 1;
	else
		quoteItem.MatchKind = 2;

	VtChartDataCollector* dataCollector = VtChartDataCollector::GetInstance();
	std::string code = sym->ShortCode + _T(":5:1");
	dataCollector->OnReceiveData(code, _ttoi(strTime), sym->Quote.intClose);

	quoteItem.ClosePrice = _ttoi(strData051);

	strTime.Insert(2, ':');
	strTime.Insert(5, ':');
	quoteItem.Time = strTime;


	quoteItem.ContQty = _ttoi(strVolume);


	quoteItem.Decimal = sym->IntDecimal;

	sym->Quote.QuoteItemQ.push_front(quoteItem);


	VtOrderManagerSelector* orderMgrSelector = VtOrderManagerSelector::GetInstance();
	for (auto it = orderMgrSelector->_OrderManagerMap.begin(); it != orderMgrSelector->_OrderManagerMap.end(); ++it)
	{
		VtOrderManager* orderMgr = it->second;
		orderMgr->OnReceiveQuoteHd(sym);
	}

	HdWindowManager* wndMgr = HdWindowManager::GetInstance();
	auto wndMap = wndMgr->GetWindowMap();
	for (auto it = wndMap.begin(); it != wndMap.end(); ++it)
	{
		auto item = it->second;
		HdWindowType type = item.first;
		CWnd* wnd = item.second;
		if (type == HdWindowType::MiniJangoWindow)
		{
			((HdAccountPLDlg*)wnd)->OnReceiveQuote(sym);
		}
		else if (type == HdWindowType::FundMiniJangoWindow)
		{
			((VtFundMiniJango*)wnd)->OnReceiveQuote(sym);
		}
		else if (type == HdWindowType::AssetWindow)
		{
			((VtAccountAssetDlg*)wnd)->OnReceiveAccountInfo();
		}
	}

	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	orderDlgMgr->OnReceiveQuote(sym);

	VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
	chartDataMgr->OnReceiveQuoteHd(sym);
}

void VtHdCtrl::OnRealProductQuote(CString& strKey, LONG& nRealType)
{
	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결시간");
	CString strVolume = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "cvolume");
	CString strUpdown = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "cgubun");

	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindSymbol((LPCTSTR)strSeries.Trim());
	if (!sym)
		return;

	CString	strData051 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "현재가");
	CString	strData052 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "open");
	CString	strData053 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "high");
	CString	strData054 = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "low");

	sym->Quote.intClose = _ttoi(strData051);
	sym->Quote.intOpen = _ttoi(strData052);
	sym->Quote.intHigh = _ttoi(strData053);
	sym->Quote.intLow = _ttoi(strData054);

	sym->Quote.close = sym->Quote.intClose / std::pow(10, sym->IntDecimal);
	sym->Quote.open = sym->Quote.intOpen / std::pow(10, sym->IntDecimal);
	sym->Quote.high = sym->Quote.intHigh / std::pow(10, sym->IntDecimal);
	sym->Quote.low = sym->Quote.intLow / std::pow(10, sym->IntDecimal);

	OnReceiveSise(_ttoi(strTime), sym);

	VtQuoteItem quoteItem;

	if (strUpdown.Compare(_T("+")) == 0)
		quoteItem.MatchKind = 1;
	else
		quoteItem.MatchKind = 2;

	VtChartDataCollector* dataCollector = VtChartDataCollector::GetInstance();
	std::string code = sym->ShortCode + _T(":5:1");
	dataCollector->OnReceiveData(code, _ttoi(strTime), sym->Quote.intClose);

	quoteItem.ClosePrice = _ttoi(strData051);

	strTime.Insert(2, ':');
	strTime.Insert(5, ':');
	quoteItem.Time = strTime;


	quoteItem.ContQty = _ttoi(strVolume);


	quoteItem.Decimal = sym->IntDecimal;

	sym->Quote.QuoteItemQ.push_front(quoteItem);

	VtOrderManagerSelector* orderMgrSelector = VtOrderManagerSelector::GetInstance();
	for (auto it = orderMgrSelector->_OrderManagerMap.begin(); it != orderMgrSelector->_OrderManagerMap.end(); ++it)
	{
		VtOrderManager* orderMgr = it->second;
		orderMgr->OnReceiveQuoteHd(sym);
	}

	HdWindowManager* wndMgr = HdWindowManager::GetInstance();
	std::map<CWnd*, std::pair<HdWindowType, CWnd*>>& wndMap = wndMgr->GetWindowMap();
	for (auto it = wndMap.begin(); it != wndMap.end(); ++it)
	{
		auto item = it->second;
		HdWindowType type = item.first;
		CWnd* wnd = item.second;
		if (type == HdWindowType::MiniJangoWindow)
		{
			((HdAccountPLDlg*)wnd)->OnReceiveQuote(sym);
		}
		else if (type == HdWindowType::FundMiniJangoWindow)
		{
			((VtFundMiniJango*)wnd)->OnReceiveQuote(sym);
		}
		else if (type == HdWindowType::AssetWindow)
		{
			((VtAccountAssetDlg*)wnd)->OnReceiveAccountInfo();
		}
	}

	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	orderDlgMgr->OnReceiveQuote(sym);

	VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
	chartDataMgr->OnReceiveQuoteHd(sym);
}

void VtHdCtrl::OnSubAccountOrder(VtOrderEvent event, CString& strSubAcntNo, CString& strFundName, VtOrder* parentOrder, VtOrderState prevState)
{
	if (!parentOrder)
		return;

	// 여기서 SubAccount 대한 처리를 한다.
	// SubAccount 는 본계좌에 정보가 기록되고 또 따로 다른 주문관리자에서 관리가 된다.
	if (strSubAcntNo.GetLength() > 0) {
		VtOrderManagerSelector* orderMgrSelecter = VtOrderManagerSelector::GetInstance();
		VtOrderManager* subOrderMgr = orderMgrSelecter->FindAddOrderManager((LPCTSTR)strSubAcntNo);
		VtOrder* subAcntOrder = subOrderMgr->FindOrder(parentOrder->orderNo);
		if (!subAcntOrder)
			subAcntOrder = subOrderMgr->CloneOrder(parentOrder);
		else
			subOrderMgr->CopyOrder(parentOrder, subAcntOrder);

		subAcntOrder->state = prevState;
		subAcntOrder->AccountNo = strSubAcntNo;
		subAcntOrder->ParentAccountNo = parentOrder->AccountNo;
		if (strFundName.GetLength() > 0) {
			subAcntOrder->type = 2;
			subAcntOrder->FundName = strFundName;
		}
		else {
			subAcntOrder->type = 1;
			subAcntOrder->FundName = _T("");
		}

		switch (event) {
		case VtOrderEvent::PutNew:
		case VtOrderEvent::Modified:
		case VtOrderEvent::Cancelled: {
			subOrderMgr->OnOrderReceivedHd(subAcntOrder);
		}
									  break;
		case VtOrderEvent::Accepted: {
			subOrderMgr->OnOrderAcceptedHd(subAcntOrder);
		}
									 break;
		case VtOrderEvent::Unfilled: {
			subOrderMgr->OnOrderUnfilledHd(subAcntOrder);
		}
									 break;
		case VtOrderEvent::Filled: {
			subOrderMgr->OnOrderFilledHd(subAcntOrder);
		}
								   break;
		default:
			break;
		}

		SendOrderMessage(event, subAcntOrder);
	}
}

void VtHdCtrl::SendOrderMessage(VtOrderEvent orderEvent, VtOrder* order)
{
	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	orderDlgMgr->SendOrderMessage(orderEvent, order);
}

void VtHdCtrl::OnExpected(CString& strKey, LONG& nRealType)
{
	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
	VtSymbol* sym = symMgr->FindSymbol((LPCTSTR)strSeries.Trim());
	if (!sym)
		return;
	CString strExpected = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "예상체결가격");
	sym->Quote.intExpected = _ttoi(strExpected);
	orderDlgMgr->OnExpected(sym);
}

void VtHdCtrl::RegisterCurrent()
{
	CString strKey = "001";
	int     nRealType = 101;
	int nResult = m_CommAgent.CommSetBroad(strKey, nRealType);

	strKey = "101";
	nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
}

void VtHdCtrl::UnregisterCurrent()
{
	CString strKey = "001";
	int     nRealType = 101;
	int nResult = m_CommAgent.CommRemoveBroad(strKey, nRealType);

	strKey = "101";
	nResult = m_CommAgent.CommRemoveBroad(strKey, nRealType);
}

void VtHdCtrl::GetAcceptedHistory(std::string accountNo, std::string pwd)
{
	Sleep(700);

	std::string reqString;
	std::string temp;
	temp = PadRight(accountNo, ' ', 11);
	reqString.append(temp);
	reqString.append(_T("001"));
	temp = PadRight(pwd, ' ', 8);
	reqString.append(temp);


	CString sTrCode = "g11002.DQ0104&";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
	AddRequest(nRqID, HdTaskType::HdAcceptedHistory);
}

void VtHdCtrl::GetFilledHistory(std::string accountNo, std::string pwd)
{
	std::string reqString;
	std::string temp;
	temp = PadRight(accountNo, ' ', 11);
	reqString.append(temp);
	reqString.append(_T("001"));
	temp = PadRight(pwd, ' ', 8);
	reqString.append(temp);


	CString sTrCode = "g11002.DQ0107&";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
	AddRequest(nRqID, HdTaskType::HdFilledHistory);
}

void VtHdCtrl::GetOutstandingHistory(std::string accountNo, std::string pwd)
{
	std::string reqString;
	std::string temp;
	temp = PadRight(accountNo, ' ', 11);
	reqString.append(temp);
	reqString.append(_T("001"));
	temp = PadRight(pwd, ' ', 8);
	reqString.append(temp);


	CString sTrCode = "g11002.DQ0110&";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
	AddRequest(nRqID, HdTaskType::HdOutstandingHistory);
}

void VtHdCtrl::GetOutstanding(std::string accountNo, std::string pwd)
{
	Sleep(700);
	std::string reqString;
	std::string temp;
	temp = PadRight(accountNo, ' ', 11);
	reqString.append(temp);
	reqString.append(_T("001"));
	temp = PadRight(pwd, ' ', 8);
	reqString.append(temp);


	CString sTrCode = "g11002.DQ1305&";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
	AddRequest(nRqID, HdTaskType::HdOutstanding);
}

void VtHdCtrl::GetCmeAcceptedHistory(std::string accountNo, std::string pwd)
{
	std::string reqString;
	std::string temp;
	temp = PadRight(accountNo, ' ', 11);
	reqString.append(temp);
	reqString.append(_T("001"));
	temp = PadRight(pwd, ' ', 8);
	reqString.append(temp);


	CString sTrCode = "g11002.DQ0116&";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
	AddRequest(nRqID, HdTaskType::HdCmeAcceptedHistory);
}

void VtHdCtrl::GetCmeFilledHistory(std::string accountNo, std::string pwd)
{
	std::string reqString;
	std::string temp;
	temp = PadRight(accountNo, ' ', 11);
	reqString.append(temp);
	reqString.append(_T("001"));
	temp = PadRight(pwd, ' ', 8);
	reqString.append(temp);


	CString sTrCode = "g11002.DQ0119&";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
	AddRequest(nRqID, HdTaskType::HdCmeFilledHistory);
}

void VtHdCtrl::GetCmeOutstandingHistory(std::string accountNo, std::string pwd)
{
	std::string reqString;
	std::string temp;
	temp = PadRight(accountNo, ' ', 11);
	reqString.append(temp);
	reqString.append(_T("001"));
	temp = PadRight(pwd, ' ', 8);
	reqString.append(temp);


	CString sTrCode = "g11002.DQ0122&";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
	AddRequest(nRqID, HdTaskType::HdCmeOutstandingHistory);
}

void VtHdCtrl::GetCmeOutstanding(std::string accountNo, std::string pwd)
{
	std::string reqString;
	std::string temp;
	temp = PadRight(accountNo, ' ', 11);
	reqString.append(temp);
	reqString.append(_T("001"));
	temp = PadRight(pwd, ' ', 8);
	reqString.append(temp);


	CString sTrCode = "g11002.DQ1306&";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
	AddRequest(nRqID, HdTaskType::HdCmeOutstanding);
}

void VtHdCtrl::GetAsset(std::string accountNo, std::string pwd)
{
	if (_Blocked)
		return;

	std::string reqString;
	std::string temp;
	temp = PadRight(accountNo, ' ', 11);
	reqString.append(temp);
	temp = PadRight(pwd, ' ', 8);
	reqString.append(temp);


	CString sTrCode = "g11002.DQ0217&";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
	AddRequest(nRqID, HdTaskType::HdAsset);
}

void VtHdCtrl::GetDeposit(std::string accountNo, std::string pwd)
{
	Sleep(700);
	std::string reqString;
	std::string temp;
	temp = PadRight(accountNo, ' ', 11);
	reqString.append(temp);
	temp = PadRight(pwd, ' ', 8);
	reqString.append(temp);


	CString sTrCode = "g11002.DQ0242&";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
	AddRequest(nRqID, HdTaskType::HdDeposit);
}

void VtHdCtrl::GetCmeAsset(std::string accountNo, std::string pwd)
{
	std::string reqString;
	std::string temp;
	temp = PadRight(accountNo, ' ', 11);
	reqString.append(temp);
	temp = PadRight(pwd, ' ', 8);
	reqString.append(temp);


	CString sTrCode = "g11002.DQ0215&";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
	AddRequest(nRqID, HdTaskType::HdCmeAsset);
}

void VtHdCtrl::GetCmePureAsset(std::string accountNo, std::string pwd)
{
	std::string reqString;
	std::string temp;
	temp = PadRight(accountNo, ' ', 11);
	reqString.append(temp);
	temp = PadRight(pwd, ' ', 8);
	reqString.append(temp);


	CString sTrCode = "g11002.DQ1303&";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
	AddRequest(nRqID, HdTaskType::HdCmePureAsset);
}

void VtHdCtrl::GetDailyProfitLoss(std::string accountNo, std::string pwd)
{
	std::string reqString;
	std::string temp;
	temp = PadRight(accountNo, ' ', 11);
	reqString.append(temp);
	reqString.append(_T("001"));
	temp = PadRight(pwd, ' ', 8);
	reqString.append(temp);


	CString sTrCode = "g11002.DQ0502&";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
	AddRequest(nRqID, HdTaskType::HdDailyProfitLoss);
}

void VtHdCtrl::GetFilledHistoryTable(std::string accountNo, std::string pwd)
{
	std::string reqString;

	LocalDateTime now;
	std::string curDate(DateTimeFormatter::format(now, "%Y%m%d"));

	reqString.append(curDate);

	std::string temp;
	temp = PadRight(accountNo, ' ', 11);
	reqString.append(temp);
	reqString.append(_T("001"));
	temp = PadRight(pwd, ' ', 8);
	reqString.append(temp);


	CString sTrCode = "g11002.DQ0509&";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
	AddRequest(nRqID, HdTaskType::HdFilledHistoryTable);
}

void VtHdCtrl::GetFilledHistoryTable(HdTaskType taskType, std::string accountNo, std::string pwd)
{
	std::string reqString;

	LocalDateTime now;
	std::string curDate(DateTimeFormatter::format(now, "%Y%m%d"));

	reqString.append(curDate);

	std::string temp;
	temp = PadRight(accountNo, ' ', 11);
	reqString.append(temp);
	reqString.append(_T("001"));
	temp = PadRight(pwd, ' ', 8);
	reqString.append(temp);


	CString sTrCode = "g11002.DQ0509&";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
	AddRequest(nRqID, taskType);
}

void VtHdCtrl::AbGetChartData(VtChartDataRequest&& chartReqeust)
{
	std::string temp;
	std::string reqString;

	temp = PadRight(chartReqeust.productCode, ' ', 32);
	reqString.append(temp);

	LocalDateTime now;
	std::string curDate(DateTimeFormatter::format(now, "%Y%m%d"));
	//reqString.append(curDate);
	//reqString.append(curDate);
	reqString.append(_T("99999999"));
	reqString.append(_T("99999999"));
	reqString.append(_T("9999999999"));

	if (chartReqeust.next == 0)
		reqString.append(_T("0"));
	else
		reqString.append(_T("1"));

	if (chartReqeust.chartType == VtChartType::TICK)
		reqString.append("1");
	else if (chartReqeust.chartType == VtChartType::MIN)
		reqString.append("2");
	else if (chartReqeust.chartType == VtChartType::DAY)
		reqString.append("3");
	else
		reqString.append("2");

	temp = PadLeft(chartReqeust.cycle, '0', 2);
	reqString.append(temp);

	temp = PadLeft(chartReqeust.count, '0', 5);
	reqString.append(temp);


	CString sTrCode = "o51200";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	//int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), "");

	CString sReqFidInput = "000001002003004005006007008009010011012013014015";
	//CString strNextKey = m_CommAgent.CommGetNextKey(nRqID, "");
	int nRqID = m_CommAgent.CommFIDRqData(sTrCode, sInput, sReqFidInput, sInput.GetLength(), strNextKey);

	AddRequest(nRqID, HdTaskType::HdChartData);
	_ChartDataRequestMap[nRqID] = chartReqeust.dataKey;
}

void VtHdCtrl::RegisterExpected(CString symCode)
{
	int nRealType = 310;
	int nResult = 0;
	CString strKey = symCode;
	nResult = m_CommAgent.CommSetBroad(strKey, nRealType);
}

void VtHdCtrl::UnregisterExpected(CString symCode)
{
	int nRealType = 310;
	int nResult = 0;
	CString strKey = symCode;
	nResult = m_CommAgent.CommRemoveBroad(strKey, nRealType);
}

void VtHdCtrl::GetAccountProfitLoss(std::string accountNo, std::string pwd)
{
	std::string reqString;
	std::string temp;
	temp = PadRight(accountNo, ' ', 11);
	reqString.append(temp);
	reqString.append(_T("001"));
	temp = PadRight(pwd, ' ', 8);
	reqString.append(temp);


	CString sTrCode = "g11002.DQ0521&";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
	AddRequest(nRqID, HdTaskType::HdAccountProfitLoss);
}

void VtHdCtrl::GetApiCustomerProfitLoss(std::string accountNo, std::string pwd)
{
	Sleep(700);
	std::string reqString;
	std::string temp;
	temp = PadRight(accountNo, ' ', 11);
	reqString.append(temp);
	temp = PadRight(pwd, ' ', 8);
	reqString.append(temp);


	CString sTrCode = "g11002.DQ1302&";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
	AddRequest(nRqID, HdTaskType::HdApiCustomerProfitLoss, accountNo);

	CString msg;
	msg.Format(_T("acnt = %s, rqId = %d \n"), accountNo.c_str(), nRqID);
	//TRACE(msg);
}

void VtHdCtrl::GetApiCustomerProfitLoss(HdTaskType taskType, std::string accountNo, std::string pwd)
{
	if (_Blocked)
		return;

	std::string reqString;
	std::string temp;
	temp = PadRight(accountNo, ' ', 11);
	reqString.append(temp);
	temp = PadRight(pwd, ' ', 8);
	reqString.append(temp);


	CString sTrCode = "g11002.DQ1302&";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), strNextKey);
	AddRequest(nRqID, taskType, accountNo);

	CString msg;
	msg.Format(_T("acnt = %s, rqId = %d \n"), accountNo.c_str(), nRqID);
	//TRACE(msg);
}

void VtHdCtrl::GetChartData()
{
	//AddRequest(nRqID, HdTaskType::HdChartData);
}

void VtHdCtrl::GetChartData(VtChartDataRequest&& chartReqeust)
{
	std::string temp;
	std::string reqString;

	temp = PadRight(chartReqeust.productCode, ' ', 15);
	reqString.append(temp);

	LocalDateTime now;
	std::string curDate(DateTimeFormatter::format(now, "%Y%m%d"));
	reqString.append(curDate);

	reqString.append(_T("999999"));

	temp = PadLeft(chartReqeust.count, '0', 4);
	reqString.append(temp);

	temp = PadLeft(chartReqeust.cycle, '0', 3);
	reqString.append(temp);

	if (chartReqeust.chartType == VtChartType::TICK)
		reqString.append("0");
	else if (chartReqeust.chartType == VtChartType::MIN)
		reqString.append("1");
	else if (chartReqeust.chartType == VtChartType::DAY)
		reqString.append("2");
	else if (chartReqeust.chartType == VtChartType::WEEK)
		reqString.append("3");
	else if (chartReqeust.chartType == VtChartType::MONTH)
		reqString.append("4");
	else
		reqString.append("1");

	if (chartReqeust.next == 0)
		reqString.append(_T("0"));
	else
		reqString.append(_T("1"));

	temp = PadRight(chartReqeust.reqKey, ' ', 21);
	reqString.append(temp);

	reqString.append(_T("0"));
	reqString.append(_T("0"));
	reqString.append(_T("00"));
	reqString.append(_T("000000"));
	reqString.append(_T(" "));

	if (chartReqeust.seq == 0)
		reqString.append(_T("0"));
	else
		reqString.append(_T("1"));

	CString sTrCode = "v90003";
	CString sInput = reqString.c_str();
	CString strNextKey = _T("");
	int nRqID = m_CommAgent.CommRqData(sTrCode, sInput, sInput.GetLength(), "");
	AddRequest(nRqID, HdTaskType::HdChartData);
	_ChartDataRequestMap[nRqID] = chartReqeust.dataKey;
}

void VtHdCtrl::GetAccountInfo()
{
	if (_Blocked)
		return;

	typedef struct
	{
		char szAcctNo[11]; // 계좌번호
		char szAcctNm[30]; // 계좌명
		char szAcctGb[01]; // 계좌구분 '1': 해외, '2': FX, '9':국내
	}HDF_ACCOUNT_UNIT;

	typedef struct
	{
		char szCount[5];
		HDF_ACCOUNT_UNIT *pHdfAccUnit;
	}HDF_ACCOUNT_INFO;

	HDF_ACCOUNT_INFO *pHdfAccInfo = NULL;
	HDF_ACCOUNT_UNIT *pHdfAccUnit = NULL;
	CString strRcvBuff = m_CommAgent.CommGetAccInfo();


	VtAccountManager* acntMgr = VtAccountManager::GetInstance();

	pHdfAccInfo = (HDF_ACCOUNT_INFO *)strRcvBuff.GetBuffer();
	CString strCount(pHdfAccInfo->szCount, sizeof(pHdfAccInfo->szCount));
	for (int i = 0; i < atoi(strCount); i++)
	{
		pHdfAccUnit = (HDF_ACCOUNT_UNIT *)(pHdfAccInfo->szCount + sizeof(pHdfAccInfo->szCount) + (sizeof(HDF_ACCOUNT_UNIT) * i));
		CString strAcctNo(pHdfAccUnit->szAcctNo, sizeof(pHdfAccUnit->szAcctNo));
		CString strAcctNm(pHdfAccUnit->szAcctNm, sizeof(pHdfAccUnit->szAcctNm));
		CString strAcctGb(pHdfAccUnit->szAcctGb, sizeof(pHdfAccUnit->szAcctGb));// 계좌 구분 추가. - 20140331 sivas

		CString strMsg;
		strMsg.Format("[%s][%s][%s]", strAcctNo, strAcctNm, strAcctGb);

		if (!acntMgr->FindAccount((LPCTSTR)strAcctNo.TrimRight()))
		{
			VtAccount* acnt = new VtAccount();
			acnt->AccountNo = (LPCTSTR)strAcctNo.TrimRight();
			acnt->AccountName = (LPCTSTR)strAcctNm.TrimRight();
			acnt->Type = _ttoi(strAcctGb);

			acntMgr->AddAccount(acnt);
		}
	}
}

void VtHdCtrl::SendOrder(HdOrderRequest request)
{
	if (request.orderType == VtOrderType::New)
		PutOrder(std::move(request));
	else if (request.orderType == VtOrderType::Change)
		ChangeOrder(std::move(request));
	else if (request.orderType == VtOrderType::Cancel)
		CancelOrder(std::move(request));
}

void VtHdCtrl::PutOrder(HdOrderRequest&& request)
{
	if (!CheckPassword(request))
		return;

	std::string orderString;
	std::string temp;
	temp = PadRight(request.AccountNo, ' ', 11);
	orderString.append(temp);
	temp = PadRight(request.Password, ' ', 8);
	orderString.append(temp);
	temp = PadRight(request.SymbolCode, ' ', 32);
	orderString.append(temp);

	if (request.Position == VtPositionType::Buy)
		orderString.append(_T("1"));
	else if (request.Position == VtPositionType::Sell)
		orderString.append(_T("2"));

	if (request.PriceType == VtPriceType::Price)
		orderString.append(_T("1"));
	else if (request.PriceType == VtPriceType::Market)
		orderString.append(_T("2"));

	if (request.FillCondition == VtFilledCondition::Fas)
		orderString.append(_T("1"));
	else if (request.FillCondition == VtFilledCondition::Fok)
		orderString.append(_T("2"));
	else if (request.FillCondition == VtFilledCondition::Fak)
		orderString.append(_T("3"));

	if (request.PriceType == VtPriceType::Price)
		temp = PadRight(request.Price, ' ', 13);
	else if (request.PriceType == VtPriceType::Market)
		temp = PadRight(0, ' ', 13);
	orderString.append(temp);

	temp = PadRight(request.Amount, ' ', 5);
	orderString.append(temp);

	std::string userDefined;
	userDefined.append(_T("--"));
	userDefined.append(_T("^"));
	userDefined.append(std::to_string(request.RequestId));
	userDefined.append(_T("^"));
	userDefined.append(std::to_string(request.SourceId));
	userDefined.append(_T("^"));
	userDefined.append(request.SubAccountNo);
	userDefined.append(_T("^"));
	userDefined.append(request.FundName);
	userDefined.append(_T("^"));
	temp = PadRight(userDefined, '-', 60);
	orderString.append(temp);

	CString sTrCode = "g12001.DO1601&";
	CString sInput = orderString.c_str();
	int nRqID = m_CommAgent.CommJumunSvr(sTrCode, sInput);
	AddRequest(nRqID, HdTaskType::HdOrderNew);
	_OrderRequestMap[nRqID] = request;

	//LOG_F(INFO,_T("PutOrder :: Req id = %d, order string = %s"), nRqID, orderString.c_str());
	LOG_F(INFO, _T("신규주문요청 : 원요청번호 %d, 선물사 요청번호 = %d, 종목이름 = %s, 주문 번호 = %s, 계좌번호 = %s, 서브계좌번호 = %s, 펀드 이름 = %s, 주문종류 = %s, 주문갯수 = %d, 요청 타입 = %d"),
		request.RequestId, nRqID, request.SymbolCode.c_str(), _T(""), request.AccountNo.c_str(), request.SubAccountNo.c_str(),
		request.FundName.c_str(), request.Position == VtPositionType::Buy ? _T("매수") : _T("매도"), request.Amount, request.RequestType);

}

void VtHdCtrl::ChangeOrder(HdOrderRequest&& request)
{
	if (!CheckPassword(request))
		return;

	std::string orderString;
	std::string temp;
	temp = PadRight(request.AccountNo, ' ', 11);
	orderString.append(temp);
	temp = PadRight(request.Password, ' ', 8);
	orderString.append(temp);
	temp = PadRight(request.SymbolCode, ' ', 32);
	orderString.append(temp);

	if (request.Position == VtPositionType::Buy)
		orderString.append(_T("1"));
	else if (request.Position == VtPositionType::Sell)
		orderString.append(_T("2"));

	if (request.PriceType == VtPriceType::Price)
		orderString.append(_T("1"));
	else if (request.PriceType == VtPriceType::Market)
		orderString.append(_T("2"));

	if (request.FillCondition == VtFilledCondition::Fas)
		orderString.append(_T("1"));
	else if (request.FillCondition == VtFilledCondition::Fok)
		orderString.append(_T("2"));
	else if (request.FillCondition == VtFilledCondition::Fak)
		orderString.append(_T("3"));

	if (request.PriceType == VtPriceType::Price)
		temp = PadRight(request.Price, ' ', 13);
	else if (request.PriceType == VtPriceType::Market)
		temp = PadRight(0, ' ', 13);
	orderString.append(temp);

	temp = PadRight(request.Amount, ' ', 5);
	orderString.append(temp);

	temp = PadLeft(request.OrderNo, '0', 7);
	orderString.append(temp);

	std::string userDefined;
	userDefined.append(_T("--"));
	userDefined.append(_T("^"));
	userDefined.append(std::to_string(request.RequestId));
	userDefined.append(_T("^"));
	userDefined.append(std::to_string(request.SourceId));
	userDefined.append(_T("^"));
	userDefined.append(request.SubAccountNo);
	userDefined.append(_T("^"));
	userDefined.append(request.FundName);
	userDefined.append(_T("^"));
	temp = PadRight(userDefined, '-', 60);
	orderString.append(temp);

	CString sTrCode = "g12001.DO1901&";
	CString sInput = orderString.c_str();
	int nRqID = m_CommAgent.CommJumunSvr(sTrCode, sInput);
	AddRequest(nRqID, HdTaskType::HdOrderChange);
	_OrderRequestMap[nRqID] = request;
	LOG_F(INFO, _T("정정주문요청 : 원요청번호 %d, 선물사 요청번호 = %d, 종목이름 = %s, 주문 번호 = %s, 계좌번호 = %s, 서브계좌번호 = %s, 펀드 이름 = %s, 주문종류 = %s, 주문갯수 = %d, 요청 타입 = %d "),
		request.RequestId, nRqID, request.SymbolCode.c_str(), _T(""), request.AccountNo.c_str(), request.SubAccountNo.c_str(),
		request.FundName.c_str(), request.Position == VtPositionType::Buy ? _T("매수") : _T("매도"), request.Amount, request.RequestType);
}

void VtHdCtrl::CancelOrder(HdOrderRequest&& request)
{
	if (!CheckPassword(request))
		return;

	std::string orderString;
	std::string temp;
	temp = PadRight(request.AccountNo, ' ', 11);
	orderString.append(temp);
	temp = PadRight(request.Password, ' ', 8);
	orderString.append(temp);
	temp = PadRight(request.SymbolCode, ' ', 32);
	orderString.append(temp);

	if (request.Position == VtPositionType::Buy)
		orderString.append(_T("1"));
	else if (request.Position == VtPositionType::Sell)
		orderString.append(_T("2"));

	if (request.PriceType == VtPriceType::Price)
		orderString.append(_T("1"));
	else if (request.PriceType == VtPriceType::Market)
		orderString.append(_T("2"));

	if (request.FillCondition == VtFilledCondition::Fas)
		orderString.append(_T("1"));
	else if (request.FillCondition == VtFilledCondition::Fok)
		orderString.append(_T("2"));
	else if (request.FillCondition == VtFilledCondition::Fak)
		orderString.append(_T("3"));

	if (request.PriceType == VtPriceType::Price)
		temp = PadRight(request.Price, ' ', 13);
	else if (request.PriceType == VtPriceType::Market)
		temp = PadRight(0, ' ', 13);
	orderString.append(temp);

	temp = PadRight(request.Amount, ' ', 5);
	orderString.append(temp);

	temp = PadLeft(request.OrderNo, '0', 7);
	orderString.append(temp);

	std::string userDefined;
	userDefined.append(_T("--"));
	userDefined.append(_T("^"));
	userDefined.append(std::to_string(request.RequestId));
	userDefined.append(_T("^"));
	userDefined.append(std::to_string(request.SourceId));
	userDefined.append(_T("^"));
	userDefined.append(request.SubAccountNo);
	userDefined.append(_T("^"));
	userDefined.append(request.FundName);
	userDefined.append(_T("^"));
	temp = PadRight(userDefined, '-', 60);
	orderString.append(temp);

	CString sTrCode = "g12001.DO1701&";
	CString sInput = orderString.c_str();
	int nRqID = m_CommAgent.CommJumunSvr(sTrCode, sInput);
	AddRequest(nRqID, HdTaskType::HdOrderCancel);
	_OrderRequestMap[nRqID] = request;
	//LOG_F(INFO, _T("Cancel Order :: Req id = %d, order string = %s"), nRqID, orderString.c_str());
	LOG_F(INFO, _T("취소주문요청 : 원요청번호 %d, 선물사 요청번호 = %d, 종목이름 = %s, 주문 번호 = %s, 계좌번호 = %s, 서브계좌번호 = %s, 펀드 이름 = %s, 주문종류 = %s, 주문갯수 = %d, 요청 타입 = %d"),
		request.RequestId, nRqID, request.SymbolCode.c_str(), _T(""), request.AccountNo.c_str(), request.SubAccountNo.c_str(),
		request.FundName.c_str(), request.Position == VtPositionType::Buy ? _T("매수") : _T("매도"), request.Amount, request.RequestType);
}

std::string VtHdCtrl::PadLeft(int input, char padding, int len)
{
	std::ostringstream out;
	out << std::internal << std::right << std::setfill(padding) << std::setw(len) << input;
	return out.str();
}

std::string VtHdCtrl::PadLeft(std::string input, char padding, int len)
{
	std::ostringstream out;
	out << std::right << std::setfill(padding) << std::setw(len) << input;
	return out.str();
}

std::string VtHdCtrl::PadLeft(double input, char padding, int len, int decimal)
{
	std::ostringstream out;
	out << std::fixed << std::setprecision(decimal) << std::right << std::setfill(padding) << std::setw(len) << input;
	return out.str();
}

std::string VtHdCtrl::PadRight(int input, char padding, int len)
{
	std::ostringstream out;
	out << std::internal << std::left << std::setfill(padding) << std::setw(len) << input;
	return out.str();
}

std::string VtHdCtrl::PadRight(std::string input, char padding, int len)
{
	std::ostringstream out;
	out << std::left << std::setfill(padding) << std::setw(len) << input;
	return out.str();
}

std::string VtHdCtrl::PadRight(double input, char padding, int len, int decimal)
{
	std::ostringstream out;
	out << std::fixed << std::setprecision(decimal) << std::left << std::setfill(padding) << std::setw(len) << input;
	return out.str();
}

void VtHdCtrl::OnDataRecv(CString sTrCode, LONG nRqID)
{
	if (_Blocked)
		return;

	CString strMsg;

	if (sTrCode == DEF_AVAILABLE_CODE_LIST)
	{
		int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
		for (int i = 0; i < nRepeatCnt; i++)
		{
			CString strData1 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "거래가능품목코드");
			CString strData2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "거래가능품목명");
			//TRACE(strData1 + strData2);
			//TRACE(_T("\n"));
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	// 국내
	///////////////////////////////////////////////////////////////////////////////////////////
	if (sTrCode == DefSymbolCode)
	{
		OnSymbolCode(sTrCode, nRqID);
	}
	else if (sTrCode == DefSymbolMaster)
	{
		OnSymbolMaster(sTrCode, nRqID);
	}
	else if (sTrCode == DefPutOrder)
	{
		OnNewOrderHd(sTrCode, nRqID);
	}
	else if (sTrCode == DefModifyOrder)
	{
		OnModifyOrderHd(sTrCode, nRqID);
	}
	else if (sTrCode == DefCancelOrder)
	{
		OnCancelOrderHd(sTrCode, nRqID);
	}
	else if (sTrCode == DefFilledHistoryTable)
	{
		OnFilledHistoryTable(sTrCode, nRqID);
	}
	else if (sTrCode == DefOutstandingHistory)
	{
		OnOutstandingHistory(sTrCode, nRqID);
	}
	else if (sTrCode == DefOutstanding)
	{
		OnOutstanding(sTrCode, nRqID);
	}
	else if (sTrCode == DefAcceptedHistory)
	{
		OnAcceptedHistory(sTrCode, nRqID);
	}
	else if (sTrCode == DefAsset)
	{
		OnAsset(sTrCode, nRqID);
	}
	else if (sTrCode == DefDeposit)
	{
		OnDeposit(sTrCode, nRqID);
	}
	else if (sTrCode == DefChartData || sTrCode == DefAbChartData)
	{
		//OnChartData(sTrCode, nRqID);
		OnReceiveChartData(sTrCode, nRqID);
	}
	else if (sTrCode == DefDailyProfitLoss)
	{
		OnDailyProfitLoss(sTrCode, nRqID);
	}
	else if (sTrCode == DefAccountProfitLoss)
	{
		OnAccountProfitLoss(sTrCode, nRqID);
	}
	else if (sTrCode == DefApiCustomerProfitLoss)
	{
		OnApiCustomerProfitLoss(sTrCode, nRqID);
	}
	else if (sTrCode == DefServerTime) {
		OnServerTime(sTrCode, nRqID);
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	// 해외
	///////////////////////////////////////////////////////////////////////////////////////////
	else if (sTrCode == DEF_HW_JANGO)
	{
		CString strMsg;
		strMsg.Format("해외 예탁자산 및 증거금 응답[%s]", DEF_HW_JANGO);
		//WriteLog(strMsg);

		int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
		for (int i = 0; i<nRepeatCnt; i++)
		{
			CString strAcctNm = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "통화구분");
			CString strEntrTot = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "예탁금총액");
			CString strEntrCh = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "예탁금잔액");

			strAcctNm.TrimRight();
			strMsg.Format("통화구분[%s] 예탁금총액[%s] 예탁금잔액[%s]", strAcctNm, strEntrTot, strEntrCh);
			//WriteLog(strMsg);
		}
	}
	else if (sTrCode == DEF_HW_FID_CODE)
	{
		CString strMsg;
		strMsg.Format("해외 현재가시세응답[%s]", DEF_HW_FID_CODE);
		//WriteLog(strMsg);

		CString strSeries = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "종목코드");
		CString strTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "국내시간"); //해외시세 조회 오류 수정 - 20160830 sivas
		CString strCloseP = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "현재가");
		CString strVolume = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "누적거래량");

		//m_edSeriesO.SetWindowText(strSeries);
		//m_edTimeO.SetWindowText(strTime);
		CString strFormatPrice = strCloseP;
		strFormatPrice = m_CommAgent.CommGetHWOrdPrice(strSeries, strCloseP, 0);
		//m_edClosePO.SetWindowText(strFormatPrice);
		//m_edVolumeO.SetWindowText(strVolume);

		strMsg.Format("FID 종목[%s]시간[%s]현재가[%s]거래량[%s]", strSeries, strTime, strCloseP, strVolume);
		//WriteLog(strMsg);

		CString strType1, strType2, strType3, strType4;
		strCloseP = strCloseP;//"1241300";
		strType1 = m_CommAgent.CommGetHWOrdPrice(strSeries, strCloseP, 0);
		strType2 = m_CommAgent.CommGetHWOrdPrice(strSeries, strType1, 1);
		strType3 = m_CommAgent.CommGetHWOrdPrice(strSeries, strType2, 2);
		strType4 = m_CommAgent.CommGetHWOrdPrice(strSeries, strType3, 0);
		strMsg.Format("FID Recv[%s]->[%s]->[%s]->[%s]->[%s]", strCloseP, strType1, strType2, strType3, strType4);

		//WriteLog(strMsg);
	}
	else if (sTrCode == DEF_HW_ORD_CODE_NEW)
	{
		int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");

		CString strExchTp = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "접수구분");
		CString strProcTp = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "처리코드");
		CString strAcctNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "처리메시지");
		CString strOrdNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문번호");

		CString strMsg;
		strMsg.Format("주문응답 번호[%d][%s]처리[%s]계좌번호[%s]주문번호[%s]", nRqID, strExchTp, strProcTp, strAcctNo, strOrdNo);
		//WriteLog(strMsg);
	}
	else if (sTrCode == DEF_HW_MSTINFO)
	{
		// OCX에서 /mst/JMCODE.cod 파일을 생성시킨다.
		//WriteLog("해외 종목 서비스 요청 완료!!!");
	}
	///////////////////////////////////////////////////////////////////////////////////////////
	// FX마진		//@lhe 2012.06.22
	///////////////////////////////////////////////////////////////////////////////////////////
	else if (sTrCode == DEF_FX_JANGO)
	{
		CString strMsg;
		strMsg.Format("FX 계좌정보 : 자산내역조회[%s]", DEF_FX_JANGO);
		//WriteLog(strMsg);

		int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
		for (int i = 0; i<nRepeatCnt; i++)
		{
			CString strAcctNm = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "계좌명");
			CString strEntrTot = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "예탁총액");
			CString strTrstMgn = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "위탁증거금");
			CString strMntMgn = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "유지증거금");

			strAcctNm.TrimRight();
			strMsg.Format("계좌명[%s] 예탁총액[%s] 위탁증거금[%s] 유지증거금[%s]", strAcctNm, strEntrTot, strTrstMgn, strMntMgn);
			//WriteLog(strMsg);
		}
	}
	else if (sTrCode == DEF_FX_FID_CODE)
	{
		CString strMsg;
		strMsg.Format("FX 마스터데이타[%s]", DEF_FX_FID_CODE);
		//WriteLog(strMsg);

		CString strSeries = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "종목코드");
		CString strOffer = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매도가격");
		CString strBid = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "매수가격");
		CString strTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "호가시간");

		//m_edSeriesO.SetWindowText(strSeries);
		//m_edTimeO.SetWindowText(strTime);
		//m_edClosePO.SetWindowText(strOffer);
		//m_edVolumeO.SetWindowText(strBid);

		strMsg.Format("FID 종목[%s]시간[%s]매도가격[%s]매수가격[%s]", strSeries, strTime, strOffer, strBid);
		//WriteLog(strMsg);
	}
	else if (sTrCode == DEF_FX_ORD_CODE_NEW)
	{
		int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");

		CString strExchTp = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "접수구분");
		CString strProcTp = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "처리코드");
		CString strAcctNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "처리메시지");
		CString strOrdNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문번호");

		CString strMsg;
		strMsg.Format("주문응답 번호[%d][%s]처리[%s]계좌번호[%s]주문번호[%s]", nRqID, strExchTp, strProcTp, strAcctNo, strOrdNo);
		//WriteLog(strMsg);
	}
	else if (sTrCode == "o44005")
	{
		// 해외 차트 조회
		int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec2");

		CString strMsg;
		strMsg.Format("응답개수[%d]", nRepeatCnt);
		//WriteLog(strMsg);

		for (int i = 0; i<nRepeatCnt; i++)
		{
			CString sDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "일자");
			CString sTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "시간");
			CString sCloseP = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "종가");
			CString sVolume = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "체결량");

			strMsg.Format("[%s][%s][%s][%s]", sDate, sTime, sCloseP, sVolume);
			//WriteLog(strMsg);
		}
	}
	// 20140328 계좌정보 수신 부분 추가. - 20140328 sivas
	else if (sTrCode == DEF_ACCT_INFO)
	{
		CString strMsg = "계좌정보 수신완료!!!";
		//WriteLog(strMsg);

		// 계좌 구분 추가. - 20140331 sivas
		typedef	struct
		{
			char 	szAcctNo[11];		// 계좌번호
			char	szAcctNm[30];		// 계좌명
			char	szAcctGb[01];		// 계좌구분  '1': 해외, '2': FX, '9':국내
		}HDF_ACCOUNT_UNIT;

		typedef struct
		{
			char szCount[5];
			HDF_ACCOUNT_UNIT *pHdfAccUnit;
		}HDF_ACCOUNT_INFO;

		HDF_ACCOUNT_INFO *pHdfAccInfo = NULL;
		HDF_ACCOUNT_UNIT *pHdfAccUnit = NULL;
		CString strRcvBuff = m_CommAgent.CommGetAccInfo();

		VtAccountManager* acntMgr = VtAccountManager::GetInstance();
		pHdfAccInfo = (HDF_ACCOUNT_INFO *)strRcvBuff.GetBuffer();
		CString strCount(pHdfAccInfo->szCount, sizeof(pHdfAccInfo->szCount));
		for (int i = 0; i<atoi(strCount); i++) {
			pHdfAccUnit = (HDF_ACCOUNT_UNIT *)(pHdfAccInfo->szCount + sizeof(pHdfAccInfo->szCount) + (sizeof(HDF_ACCOUNT_UNIT) * i));
			CString strAcctNo(pHdfAccUnit->szAcctNo, sizeof(pHdfAccUnit->szAcctNo));
			CString strAcctNm(pHdfAccUnit->szAcctNm, sizeof(pHdfAccUnit->szAcctNm));
			CString strAcctGb(pHdfAccUnit->szAcctGb, sizeof(pHdfAccUnit->szAcctGb));// 계좌 구분 추가. - 20140331 sivas

			strMsg.Format("[%s][%s][%s]\n", strAcctNo, strAcctNm, strAcctGb);
			//WriteLog(strMsg);
			//TRACE(strMsg);
			if (strAcctGb.Compare(_T("9")) == 0) {
				VtGlobal* global = VtGlobal::GetInstance();
				VtAccount* acnt = acntMgr->FindAccount((LPCTSTR)strAcctNo.TrimRight());
				if (!acnt) {
					acnt = new VtAccount();
					acnt->AccountNo = (LPCTSTR)strAcctNo.TrimRight();
					acnt->AccountName = (LPCTSTR)strAcctNm.TrimRight();
					acnt->Password = _T("");
					acnt->Type = _ttoi(strAcctGb);
					acntMgr->AddAccount(acnt);
				}
				acnt->Enable(true);
				global->AcntList[acnt->AccountNo] = acnt;
			}

		}
	}
	else if (sTrCode == "l41601")
	{
		// input = "1012014051620020"
		// req = "000001002003004005006007008009"
		int nRepeatCnt = 20;	// 조회시 요청한 개수
		if (nRepeatCnt > 0)
		{
			typedef struct
			{
				char		baseid[8];		/*  [기초자산ID]기초자산ID     */
				char		date[8];		/*  [일자]일자     */
				char		price[6];		/*  [현재가]현재가     */
				char		sign[1];		/*  [대비구분]대비구분     */
				char		change[6];		/*  [대비]대비     */
				char		open[6];		/*  [시가]시가     */
				char		high[6];		/*  [고가]고가     */
				char		low[6];			/*  [저가]저가     */
				char		volume[15];		/*  [누적거래량]누적거래량     */
				char		jnilclose[6];	/*  [전일종가]     */

			}HDF_I41601;
			int nBuffSize = sizeof(HDF_I41601);
			CString strBuff = m_CommAgent.CommGetDataDirect(sTrCode, -1, 0, nRepeatCnt * nBuffSize, 0, "A");

			// Struct만큼 잘라서 사용하세요...

			//WriteLog(strBuff);
		}
	}
	else if (sTrCode == "g11004.AQ0207%")
	{
		CString strProcTp = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "처리코드");
		CString strProMsg = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "처리메시지");
		CString strAcctNo = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "계좌번호");
		CString strAcctNm = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "계좌명");
		CString strTp = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "구분");
		CString strYN = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "일치여부");

		CString strBuff;
		strBuff.Format("[%s][%s][%s][%s][%s][%s]", strProcTp, strProMsg, strAcctNo, strAcctNm, strTp, strYN);

		//WriteLog(strBuff);
	}
	else if (sTrCode == "o51100")
	{
		int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");

		for (int i = 0; i<nRepeatCnt; i++)
		{
			CString sDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목코드");
			CString sTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목명");
			CString sCloseP = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "현재가");
			CString sVolume = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "전일대비구분");

			CString strMsg;
			strMsg.Format("[%s][%s][%s][%s]", sDate, sTime, sCloseP, sVolume);
			//WriteLog(strMsg);
		}
	}
	else if (sTrCode == "o51200")
	{
		CString strMsg;
		int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");

		for (int i = 0; i<nRepeatCnt; i++)
		{
			CString sDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "일자");
			CString sTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "시간");
			CString sCloseP = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종가");
			CString sVolume = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "체결량");

			strMsg.Format("[%s][%s][%s][%s]", sDate, sTime, sCloseP, sVolume);
			//WriteLog(strMsg);
		}

		int nBuffSize = 129 + 16;	// Fid조회는 필드 마지막에 구분자 1자리가 있으므로 각 필드 만큼 더해준다.
		CString strBuff = m_CommAgent.CommGetDataDirect(sTrCode, -1, 0, nRepeatCnt * nBuffSize, 0, "A");
		strMsg.Format("[%s]", strBuff);
		//WriteLog(strMsg);

		CString strNextKey = m_CommAgent.CommGetNextKey(nRqID, "");
		if (strNextKey != "")
		{
			int nRet = MessageBox("조회를 계속 할까요?", "현대선물", MB_YESNO);
			if (nRet == IDYES)
			{
				//m_strNextKey = strNextKey;
				//OnBnClickedRqtest();
			}
		}
	}
	else if (sTrCode == "g11004.AQ0401%")
	{
		CString strMsg;
		int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");

		for (int i = 0; i<nRepeatCnt; i++)
		{
			CString sDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문번호");
			CString sTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌번호");
			CString sCloseP = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목코드");
			CString sVolume = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문가격");

			strMsg.Format("[%s][%s][%s][%s]", sDate, sTime, sCloseP, sVolume);
			//WriteLog(strMsg);
		}

		int nBuffSize = 226;
		CString strBuff = m_CommAgent.CommGetDataDirect(sTrCode, -1, 0, nRepeatCnt * nBuffSize, 0, "A");
		strMsg.Format("[%s]", strBuff);
		//WriteLog(strMsg);
	}
	else if (sTrCode == "g11004.AQ0408%")
	{
		CString strMsg;
		int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");

		for (int i = 0; i<nRepeatCnt; i++)
		{
			CString sData01 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "처리일자");
			CString sData02 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문번호");
			CString sData03 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문/체결구분");
			CString sData04 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목코드");
			CString sData05 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "매매구분");
			CString sData06 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문수량");
			CString sData07 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "정정수량");
			CString sData08 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "취소수량");
			CString sData09 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "체결수량");
			CString sData10 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문잔량");
			CString sData11 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문/체결가격");
			CString sData12 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "가격조건");
			CString sData13 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "통신주문구분");
			CString sData14 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "주문전략구분");
			CString sData15 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "거래소접수/거부구분");
			CString sData16 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "거래소접수/거부처리시간");
			CString sData17 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "거래소접수번호/거부코드");
			CString sData18 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "거래소주문상태");
			CString sData19 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "작업일자");
			CString sData20 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "작업시간");
			CString sData21 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "작업사원");
			CString sData22 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "사설IP");
			CString sData23 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "공인IP");
			CString sData24 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "처리일자");
			CString sData25 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "최초원주문번호");
			CString sData26 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "stop가격");


			strMsg.Format("[%s][%s][%s][%s]", sData01, sData02, sData03, sData04);
			//WriteLog(strMsg);
		}

		int nBuffSize = 226;
		CString strBuff = m_CommAgent.CommGetDataDirect(sTrCode, -1, 0, nRepeatCnt * nBuffSize, 0, "A");
		strMsg.Format("[%s]", strBuff);
		//WriteLog(strMsg);


		CString strNextKey = m_CommAgent.CommGetNextKey(nRqID, "");
		if (strNextKey != "")
		{
			int nRet = MessageBox("조회를 계속 할까요?", "현대선물", MB_YESNO);
			if (nRet == IDYES)
			{
				//m_strNextKey = strNextKey;
				//OnBnClickedRqtest();
			}
		}
	}
	else if (sTrCode == "o44011")	// 서버 현재시간
	{
		CString sDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "서버일자");
		CString sTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "서버시간");

		strMsg.Format("서버날짜[%s]서버시간[%s]", sDate, sTime);
		//WriteLog(strMsg);
	}
	else if (sTrCode == "n51003")
	{
		// 공지사항 조회
		CString sTemp1 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "제목");

		strMsg.Format("[%s]", sTemp1);
		//WriteLog(strMsg);

		CString strNextKey = m_CommAgent.CommGetNextKey(nRqID, "");
		if (strNextKey != "")
		{
			int nRet = MessageBox("n51003 조회를 계속 할까요?", "현대선물", MB_YESNO);
			if (nRet == IDYES)
			{
				//m_strNextKey = strNextKey;
				//OnBnClickedRqtest();
			}
		}
	}
	else if (sTrCode == "g11002.DQ0321&")
	{
		CString sTemp1 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "처리코드");
		CString sTemp2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "처리메시지");
		CString sTemp3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "계좌번호");
		CString sTemp4 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "일치여부");
		CString sTemp5 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "계좌구분");

		strMsg.Format("처리코드[%s]처리메시지[%s]계좌번호[%s]일치여부[%s]계좌구분[%s]", sTemp1, sTemp2, sTemp3, sTemp4, sTemp5);
		//WriteLog(strMsg);
	}
	else if (sTrCode == "n51006")
	{
		CString strMsg;
		int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");

		CString sLen = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "전체사이즈");

		int nBuffSize = 10 + 1 + 255 + atoi(sLen);
		CString strBuff = m_CommAgent.CommGetDataDirect(sTrCode, -1, 0, nBuffSize, 0, "A");
		strMsg.Format("[%s]", strBuff);
		//WriteLog(strMsg);
		AfxMessageBox(strMsg);
	}
	else if (sTrCode == "l41600")
	{
		/***
		if ( nRepeatCnt > 0 )
		{
		typedef struct
		{
		char		baseid[8];		//  [기초자산ID]기초자산ID
		char		date[8];		//  [일자]일자
		char		price[6];		//  [현재가]현재가
		char		sign[1];		//  [대비구분]대비구분
		char		change[6];		//  [대비]대비
		char		open[6];		//  [시가]시가
		char		high[6];		//  [고가]고가
		char		low[6];			//  [저가]저가
		char		volume[15];		//  [누적거래량]누적거래량
		char		jnilclose[6];	//  [전일종가]

		}HDF_I41601;
		int nBuffSize = sizeof(HDF_I41601);
		CString strBuff = m_CommAgent.CommGetDataDirect(sTrCode, -1, 0, nRepeatCnt * nBuffSize, 0, "A");

		// Struct만큼 잘라서 사용하세요...
		WriteLog(strBuff);
		}
		***/

		int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
		CString sLen1 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "품목명");
		CString sLen2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "체결시간");
		CString sLen3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "현재가");

		//WriteLog(sLen1 + sLen2 + sLen3);
	}
	else if (sTrCode == "g11002.CQ0101&")
	{
		int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");
		for (int i = 0; i<nRepeatCnt; i++)
		{
			CString strAcctNm = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "사용자ID");
			CString strEntrTot = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "계좌번호");
			CString strTrstMgn = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "대출구분");
			CString strMntMgn = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "TMP1");

			strAcctNm.TrimRight();
			strMsg.Format("사용자ID[%s] 계좌번호[%s] 대출구분[%s] TMP1[%s]", strAcctNm, strEntrTot, strTrstMgn, strMntMgn);
			//WriteLog(strMsg);
		}
	}
	else if (sTrCode == "o44010")
	{
		// 해외 차트 조회
		int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec2");

		CString sRcvCnt = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "수신봉개수");

		nRepeatCnt = atoi(sRcvCnt);

		CString strMsg;
		strMsg.Format("응답개수[%d]", nRepeatCnt);
		//WriteLog(strMsg);

		for (int i = 0; i<nRepeatCnt; i++)
		{
			CString sDate = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "시작일자");
			CString sTime = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "시작시간");
			CString sCloseP = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "종가");
			CString sVolume = m_CommAgent.CommGetData(sTrCode, -1, "OutRec2", i, "체결량");

			strMsg.Format("[%s][%s][%s][%s]", sDate, sTime, sCloseP, sVolume);
			//WriteLog(strMsg);
		}
	}
	else if (sTrCode == "v90001")
	{
		// 종목정보 요청 기능.
		long nFileSize = atol(m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "파일크기"));
		CString strMsg;
		strMsg.Format("파일크기[%d]", nFileSize);
		//WriteLog(strMsg);


		CString strFileNm = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "파일명");
		CString strProcCd = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "응답코드");

		if (strProcCd == "REOK")
		{
			TCHAR iniFileName[500];

			GetModuleFileNameA(NULL, iniFileName, MAX_PATH);
			CString strFileName;
			const CString strExeName = "HDFCommClient.exe";
			strFileName.Format("%s", iniFileName);
			int nDel = strExeName.GetLength();
			int nFind = strFileName.Find(strExeName);
			strFileName.Delete(nFind, nDel);
			strFileName = strFileName + "mst/" + strFileNm;

			CString strCommonFileName = strFileName;

			CFile commonfile;
			// open file
			if (!commonfile.Open(strCommonFileName, CFile::modeWrite /*| CFile::typeBinary*/))
			{
				if (commonfile.Open(strCommonFileName, CFile::modeCreate | CFile::modeWrite /*| CFile::typeBinary*/) == FALSE)
				{
					CString strMsg;
					strMsg.Format("%s화일 생성에 실패하였습니다. ", strCommonFileName);
					//WriteLog(strMsg);
					return;
				}
			}

			CString strBuff = m_CommAgent.CommGetDataDirect(sTrCode, -1, 128 + 4 + 8, nFileSize, 0, "A");
			commonfile.Write(strBuff, nFileSize);
			commonfile.Close();
		}

		//WriteLog("처리완료");
	}
	else if (sTrCode == "g11002.DQ0242&")
	{
		CString sTemp1 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "계좌번호");
		CString sTemp2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "예탁금액-총액");
		CString sTemp3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "주문가능금액");

		strMsg.Format("계좌번호[%s]예탁금액-당일손익[%s]주문가능금액[%s]", sTemp1, sTemp2, sTemp3);
		//WriteLog(strMsg);
	}
	else if (sTrCode == "g11002.DQ1305&" || sTrCode == "g11002.DQ1306&")
	{
		CString strMsg;
		int nRepeatCnt = m_CommAgent.CommGetRepeatCnt(sTrCode, -1, "OutRec1");

		for (int i = 0; i<nRepeatCnt; i++)
		{
			CString sTemp1 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "계좌번호");
			CString sTemp2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "종목");
			CString sTemp3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", i, "평가손익");

			strMsg.Format("계좌번호[%s]종목[%s]평가손익[%s]", sTemp1, sTemp2, sTemp3);
			//WriteLog(strMsg);
		}
	}
	else if (sTrCode == "g11002.DQ1303&")
	{
		CString sTemp1 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "계좌번호");
		CString sTemp2 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "가결제금액");
		CString sTemp3 = m_CommAgent.CommGetData(sTrCode, -1, "OutRec1", 0, "평가예탁총액");

		strMsg.Format("계좌번호[%s]예탁금액현금[%s]평가예탁총액[%s]", sTemp1, sTemp2, sTemp3);
		//WriteLog(strMsg);
	}
}

void VtHdCtrl::OnGetBroadData(CString strKey, LONG nRealType)
{
	if (_Blocked)
		return;

	VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
	switch (nRealType)
	{
		/////////////////////////////////////////////////////////////////////////////////////////////////
		// 국내
		/////////////////////////////////////////////////////////////////////////////////////////////////
	case 51:	// 선물호가
	{
		OnFutureHoga(strKey, nRealType);
	}
	break;
	case 52:	// 옵션호가
	{
		OnOptionHoga(strKey, nRealType);
	}
	break;
	case 58: //상품선물호가실시간
	{
		OnProductHoga(strKey, nRealType);
	}
	break;
	case 65:	// 선물체결
	{
		OnRealFutureQuote(strKey, nRealType);
	}
	break;
	case 71:	// 상품선물체결
	{
		OnRealProductQuote(strKey, nRealType);
	}
	break;
	case 66:	// 옵션체결
	{
		OnRealOptionQuote(strKey, nRealType);
	}
	break;
	/*
	case 181:	// 주문접수
	{
	CString strAcctNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "계좌번호");
	CString strOrdNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strPosition = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매매구분");
	CString strPrice = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문가격");
	CString strAmount = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문수량");
	CString strPriceType = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "가격구분");

	strOrdNo.TrimRight();
	strSeries.TrimRight();
	strPrice.TrimRight();
	strAmount.TrimRight();

	CString strMsg;
	strMsg.Format("주문접수 번호[%s]종목[%s]매매[%s]가격[%s]수량[%s]", strOrdNo, strSeries, strPosition, strPrice, strAmount);
	//WriteLog(strMsg);

	m_strOrdNo = strOrdNo;

	VtOrderManagerSelector* orderMgrSeledter = VtOrderManagerSelector::GetInstance();
	VtOrderManager* orderMgr = orderMgrSeledter->FindAddOrderManager((LPCTSTR)strAcctNo);

	VtOrder* order = nullptr;
	order = orderMgr->FindOrder(_ttoi(strOrdNo));
	if (!order)
	order = new VtOrder();
	order->accountNo = (LPCTSTR)strAcctNo;
	order->shortCode = (LPCTSTR)strSeries;
	order->orderNo = _ttoi(strOrdNo);
	order->intOrderPrice = _ttoi(strPrice);
	order->amount = _ttoi(strAmount);
	if (strPosition.Compare(_T("1")) == 0)
	{
	order->orderPosition = VtPositionType::Buy;
	}
	else if (strPosition.Compare(_T("2")) == 0)
	{
	order->orderPosition = VtPositionType::Sell;
	}

	if (strPriceType.Compare(_T("1")) == 0)
	{
	order->priceType = VtPriceType::Price;
	}
	else if (strPosition.Compare(_T("2")) == 0)
	{
	order->priceType = VtPriceType::Market;
	}

	orderMgr->OnOrderAcceptedHd(order);
	}
	break;
	case 182:	// 미체결
	{
	CString strAcctNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
	CString strOrdNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strPosition = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매매구분");
	CString strPrice = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문가격");
	CString strAmount = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문수량");

	strOrdNo.TrimRight();
	strSeries.TrimRight();
	strPrice.TrimRight();
	strAmount.TrimRight();

	CString strMsg;
	strMsg.Format("주문접수 번호[%s]종목[%s]매매[%s]가격[%s]수량[%s]", strOrdNo, strSeries, strPosition, strPrice, strAmount);
	//WriteLog(strMsg);

	m_strOrdNo = strOrdNo;

	VtOrderManagerSelector* orderMgrSeledter = VtOrderManagerSelector::GetInstance();
	VtOrderManager* orderMgr = orderMgrSeledter->FindAddOrderManager((LPCTSTR)strAcctNo);

	VtOrder* order = nullptr;
	order = orderMgr->FindOrder(_ttoi(strOrdNo));
	if (!order)
	order = new VtOrder();
	order->accountNo = (LPCTSTR)strAcctNo;
	order->shortCode = (LPCTSTR)strSeries;
	order->orderNo = _ttoi(strOrdNo);
	order->intOrderPrice = _ttoi(strPrice);
	order->amount = _ttoi(strAmount);
	if (strPosition.Compare(_T("1")) == 0)
	{
	order->orderPosition = VtPositionType::Buy;
	}
	else if (strPosition.Compare(_T("2")) == 0)
	{
	order->orderPosition = VtPositionType::Sell;
	}

	orderMgr->OnOrderUnfilledHd(order);
	}
	break;
	case 185:	// 체결
	{
	CString strAcctNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
	CString strOrdNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
	CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
	CString strPosition = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매매구분");
	CString strPrice = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문가격");
	CString strAmount = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문수량");

	strOrdNo.TrimRight();
	strSeries.TrimRight();
	strPrice.TrimRight();
	strAmount.TrimRight();

	CString strMsg;
	strMsg.Format("주문접수 번호[%s]종목[%s]매매[%s]가격[%s]수량[%s]", strOrdNo, strSeries, strPosition, strPrice, strAmount);
	//WriteLog(strMsg);

	m_strOrdNo = strOrdNo;

	VtOrderManagerSelector* orderMgrSeledter = VtOrderManagerSelector::GetInstance();
	VtOrderManager* orderMgr = orderMgrSeledter->FindAddOrderManager((LPCTSTR)strAcctNo);

	VtOrder* order = nullptr;
	order = orderMgr->FindOrder(_ttoi(strOrdNo));
	if (!order)
	order = new VtOrder();
	order->accountNo = (LPCTSTR)strAcctNo;
	order->shortCode = (LPCTSTR)strSeries;
	order->orderNo = _ttoi(strOrdNo);
	order->intOrderPrice = _ttoi(strPrice);
	order->amount = _ttoi(strAmount);
	if (strPosition.Compare(_T("1")) == 0)
	{
	order->orderPosition = VtPositionType::Buy;
	}
	else if (strPosition.Compare(_T("2")) == 0)
	{
	order->orderPosition = VtPositionType::Sell;
	}

	orderMgr->OnOrderFilledHd(order);
	}
	break;
	*/
	case 310: {
		OnExpected(strKey, nRealType);
		break;
	}

	case 183:	// 미결제
	{
		OnRemain(strKey, nRealType);
	}
	break;
	case 261:	// 주문접수
	{
		OnOrderAcceptedHd(strKey, nRealType);
	}
	break;

	case 262:	// 주문미체결
	{
		OnOrderUnfilledHd(strKey, nRealType);
	}
	break;
	case 265:	// 주문체결
	{
		OnOrderFilledHd(strKey, nRealType);
	}
	break;

	/////////////////////////////////////////////////////////////////////////////////////////
	// 해외				        /////////////////////////////////////////////////////////////////////////////////////////
	case 75:	//해외호가
	{

	}
	break;
	case 82:	//해외체결
	{
		CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
		CString strTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "기준체결시간");
		CString strCloseP = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결가");
		CString strOpen = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "시가");
		CString strHigh = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "고가");
		CString strLow = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "저가");
		CString strVolume = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결량");
		CString strUpdown = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "체결구분");

		//m_edSeriesO.SetWindowText(strSeries);
		//m_edTimeO.SetWindowText(strTime);
		CString strFormatPrice = strCloseP;
		strFormatPrice = m_CommAgent.CommGetHWOrdPrice(strSeries, strCloseP, 0);
		//m_edClosePO.SetWindowText(strFormatPrice);
		//m_edVolumeO.SetWindowText(strVolume);

		CString strType1, strType2, strType3, strType4;
		strCloseP = strCloseP;//"1241300";
		strType1 = m_CommAgent.CommGetHWOrdPrice(strSeries, strCloseP, 0);
		strType2 = m_CommAgent.CommGetHWOrdPrice(strSeries, strType1, 1);
		strType3 = m_CommAgent.CommGetHWOrdPrice(strSeries, strType2, 2);
		strType4 = m_CommAgent.CommGetHWOrdPrice(strSeries, strType3, 0);

		CString strMsg;
		strMsg.Format("Real 82 Recv[%s]->[%s]->[%s]->[%s]->[%s]", strCloseP, strType1, strType2, strType3, strType4);

		//WriteLog(strMsg);

		CString strBuff = m_CommAgent.CommGetDataDirect(strKey, nRealType, 0, 230, 0, "A");
		CString strSeries1(strBuff, 10);
		strMsg = strSeries1;
		//WriteLog(strMsg);

		VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
		VtSymbol* sym = symMgr->FindSymbol((LPCTSTR)strSeries.Trim());
		if (!sym)
			return;

		sym->Quote.intClose = _ttoi(strCloseP);
		sym->Quote.intOpen = _ttoi(strOpen);
		sym->Quote.intHigh = _ttoi(strHigh);
		sym->Quote.intLow = _ttoi(strLow);

		VtQuoteItem quoteItem;

		if (strUpdown.Compare(_T("+")) == 0)
			quoteItem.MatchKind = 1;
		else
			quoteItem.MatchKind = 2;


		quoteItem.ClosePrice = _ttoi(strCloseP);

		strTime.Insert(2, ':');
		strTime.Insert(5, ':');

		quoteItem.Time = strTime;


		quoteItem.ContQty = _ttoi(strVolume);


		quoteItem.Decimal = sym->IntDecimal;

		sym->Quote.QuoteItemQ.push_front(quoteItem);

		VtChartDataManager* chartDataMgr = VtChartDataManager::GetInstance();
		chartDataMgr->OnReceiveQuoteHd(sym);
	}
	break;
	case 196: //해외주문접수
	{
		CString strOrdNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
		CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
		CString strBySlTp = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매매구분");
		CString strOrdP = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문가격");
		CString strOrdQ = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문수량");

		strOrdNo.Trim();
		strSeries.Trim();
		strOrdP.Trim();
		strOrdQ.Trim();

		CString strErrCd = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "처리코드");
		CString strErrMsg = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "처리메세지");
		strErrCd.Trim();
		strErrMsg.Trim();

		CString strMsg;

		strMsg.Format("[%d] 주문접수코드[%s][%s]", nRealType, strErrCd, strErrMsg);
		//WriteLog(strMsg);

		strMsg.Format("[%d] 주문접수 번호[%s]종목[%s]매매[%s]가격[%s]수량[%s]", nRealType, strOrdNo, strSeries, strBySlTp, strOrdP, strOrdQ);
		//WriteLog(strMsg);

		m_strOrdNo = strOrdNo;
	}
	break;
	case 186: //해외미체결내역
	{
		CString strOrdNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
		CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
		CString strBySlTp = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매매구분");
		CString strOrdP = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문가격");
		CString strOrdQ = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문수량");

		strOrdNo.TrimRight();
		strSeries.TrimRight();
		strOrdP.TrimRight();
		strOrdQ.TrimRight();

		if (m_strOrdNo == strOrdNo)
		{
			CString strMsg;
			strMsg.Format("미체결 주문번호[%s]종목[%s]매매[%s]가격[%s]수량[%s]", strOrdNo, strSeries, strBySlTp, strOrdP, strOrdQ);
			//WriteLog(strMsg);
		}
	}
	break;
	case 187: //해외미결제내역
	{

	}
	break;
	case 189: //해외체결내역
	{
		CString strOrdNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
		CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
		CString strBySlTp = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매매구분");
		CString strOrdP = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문가격");
		CString strOrdQ = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문수량");

		strOrdNo.TrimRight();
		strSeries.TrimRight();
		strOrdP.TrimRight();
		strOrdQ.TrimRight();

		if (m_strOrdNo == strOrdNo)
		{
			CString strMsg;
			strMsg.Format("체결 주문번호[%s]종목[%s]매매[%s]가격[%s]수량[%s]", strOrdNo, strSeries, strBySlTp, strOrdP, strOrdQ);
			//WriteLog(strMsg);
		}
	}
	break;
	// FX마진		//@lhe 2012.05.16
	case 171: //FX 시세
	{
		CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "단축코드");
		CString strTime = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "호가시간");
		CString strOffer = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매도호가");
		CString strBid = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매수호가");

		//m_edSeriesO.SetWindowText(strSeries);
		//m_edTimeO.SetWindowText(strTime);
		//m_edClosePO.SetWindowText(strOffer);
		//m_edVolumeO.SetWindowText(strBid);

		CString strMsg;
		strMsg.Format("FX시세  종목[%s] 호가시간[%s] 매도호가[%s]매수호가[%s]", strSeries, strTime, strOffer, strBid);

		//WriteLog(strMsg);
	}
	break;
	case 191: //FX미체결내역
	{
		CString strOrdNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
		CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
		CString strBySlTp = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매매구분");
		CString strOrdP = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문가격");
		CString strOrdQ = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문수량");

		strOrdNo.TrimRight();
		strSeries.TrimRight();
		strOrdP.TrimRight();
		strOrdQ.TrimRight();

		if (m_strOrdNo == strOrdNo)
		{
			CString strMsg;
			strMsg.Format("미체결 주문번호[%s]종목[%s]매매[%s]가격[%s]수량[%s]", strOrdNo, strSeries, strBySlTp, strOrdP, strOrdQ);
			//WriteLog(strMsg);
		}
	}
	break;
	case 192: //FX미청산내역
	{
		CString strOrdNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
		CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
		CString strBySlTp = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매매구분");
		CString strOrdP = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "진입가격");
		CString strOrdQ = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "진입수량");

		strOrdNo.TrimRight();
		strSeries.TrimRight();
		strOrdP.TrimRight();
		strOrdQ.TrimRight();

		if (m_strOrdNo == strOrdNo)
		{
			CString strMsg;
			strMsg.Format("미청산 주문번호[%s]종목[%s]매매[%s]가격[%s]수량[%s]", strOrdNo, strSeries, strBySlTp, strOrdP, strOrdQ);
			//WriteLog(strMsg);
		}
	}
	break;
	case 193: //FX청산내역
	{
		CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
		CString strBySlTp = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매매구분");
		CString strOrdP = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "청산가격");
		CString strOrdQ = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "청산수량");

		strSeries.TrimRight();
		strOrdP.TrimRight();
		strOrdQ.TrimRight();

		CString strMsg;
		strMsg.Format("청산 종목[%s]매매[%s]가격[%s]수량[%s]", strSeries, strBySlTp, strOrdP, strOrdQ);
		//WriteLog(strMsg);
	}
	break;
	case 197: //FX주문접수
	{
		CString strOrdNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
		CString strErrcd = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "처리코드");
		CString strErrMsg = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "처리메세지");

		strOrdNo.TrimRight();
		strErrcd.TrimRight();
		strErrMsg.TrimRight();

		CString strMsg;
		strMsg.Format("주문접수 번호[%s]처리코드[%s]처리메시지[%s]", strOrdNo, strErrcd, strErrMsg);
		//WriteLog(strMsg);

		//m_strOrdNo = strOrdNo;
	}
	break;
	case 101:
	{
		CString strCode = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "업종코드");
		CString strOrdNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "지수");
		CString strErrcd = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "전일비");
		CString strErrMsg = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "등락율");
		strCode.TrimRight();
		strOrdNo.TrimRight();
		strErrcd.TrimRight();
		strErrMsg.TrimRight();

		CString strMsg;
		strMsg.Format("지수[%s]전일비[%s]등락율[%s]\n", strOrdNo, strErrcd, strErrMsg);
		//WriteLog(strMsg);
		//TRACE(strMsg);
		VtSymbolManager* symMgr = VtSymbolManager::GetInstance();
		if (strCode.CompareNoCase(_T("001")) == 0)
			symMgr->KospiCurrent = _ttoi(strOrdNo);
		if (strCode.CompareNoCase(_T("101")) == 0)
			symMgr->Kospi200Current = _ttoi(strOrdNo);

	}
	break;
	case 199:
	{
		CString strOrdNo = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문번호");
		CString strSeries = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "종목코드");
		CString strBySlTp = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "매매구분");
		CString strOrdP = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문가격");
		CString strOrdQ = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문수량");

		strOrdNo.TrimRight();
		strSeries.TrimRight();
		strOrdP.TrimRight();
		strOrdQ.TrimRight();

		CString strMsg;
		strMsg.Format("Yes미체결 주문번호[%s]종목[%s]매매[%s]가격[%s]수량[%s]", strOrdNo, strSeries, strBySlTp, strOrdP, strOrdQ);
		//WriteLog(strMsg);
	}
	break;
	case 208:
	{
		CString strBuff = m_CommAgent.CommGetDataDirect("0", nRealType, 0, 15, 0, "A");
		//WriteLog(strBuff);
	}
	break;
	case 184:	// 국내실시간잔고
	case 188:	// 해외실시간잔고
	{
		CString strAccno = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "계좌번호");
		CString strAmt = m_CommAgent.CommGetData(strKey, nRealType, "OutRec1", 0, "주문가능금액");
		CString strMsg;
		strMsg.Format("리얼번호[%d]계좌번호[%s]주문가능금액[%s]", nRealType, strAccno, strAmt);
		//WriteLog(strMsg);
	}
	break;
	default:
	{
		CString strMsg;
		strMsg.Format("[%d] 알수없는 실시간 요청값", nRealType);
		//WriteLog(strMsg);
	}
	break;
	}
}

void VtHdCtrl::OnGetMsg(CString strCode, CString strMsg)
{
	if (!_ServerConnected || _Blocked)
		return;

	CString strLog;
	strLog.Format("OnGetMsg[코드번호 = %s][메시지 = %s]\n", strCode, strMsg);
	std::string msg = (LPCTSTR)strMsg;
	if (strCode.Compare(_T("99997")) == 0) {
		LOG_F(INFO, _T("OnGetMsg[코드번호 = %s][메시지 = 입력값 오류!!]"), strCode);
	}
	else {
		LOG_F(INFO, _T("OnGetMsg[코드번호 = %s][메시지 = %s]"), strCode, strMsg);
		VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
		orderDlgMgr->OnReceiveMsg(strLog);
	}

	TRACE(strLog);
	if (strCode.Compare(_T("-9998")) == 0) {
		_ServerConnected = false;
		AfxMessageBox(_T("서버와의 연결이 끊어졌습니다. 프로그램을 종료합니다."));
		CMainFrame* mainFrm = (CMainFrame*)AfxGetMainWnd();
		mainFrm->SendMessage(WM_CLOSE, 0, 0);
		return;
	}
}

void VtHdCtrl::OnGetMsgWithRqId(int nRqId, CString strCode, CString strMsg)
{
	if (_Blocked)
		return;

	CString strLog;
	//strLog.Format("[요청번호 = %d, 코드번호 = %s][메시지 = %s]\n", nRqId, strCode, strMsg);
	//TRACE(strLog);

	if (strCode.Compare(_T("99997")) == 0) {
		LOG_F(INFO, _T("[요청번호 = %d, 코드번호 = %s][메시지 = 입력갑오류!!]\n"), nRqId, strCode);
	}
	else {
		LOG_F(INFO, _T("[요청번호 = %d, 코드번호 = %s][메시지 = %s]\n"), nRqId, strCode, strMsg);

		VtOrderDialogManager* orderDlgMgr = VtOrderDialogManager::GetInstance();
		orderDlgMgr->OnReceiveMsgWithReqId(nRqId, strLog);
	}

	auto it = _OrderRequestMap.find(nRqId);
	if (it != _OrderRequestMap.end() && strCode.Compare(_T("46528")) == 0) {
		HdOrderRequest req = it->second;
		_OrderRequestMap.erase(it);

		VtOrderQueueManager* orderQMgr = VtOrderQueueManager::GetInstance();
		orderQMgr->RemoveOrderRequest(req.RequestId);
	}

	if (strCode.Compare(_T("00000")) != 0) {
		HdTaskEventArgs eventArg;
		HdTaskArg arg = FindRequest(nRqId);
		eventArg.TaskType = arg.Type;
		eventArg.RequestId = nRqId;
		FireTaskCompleted(std::move(eventArg));
	}
}


