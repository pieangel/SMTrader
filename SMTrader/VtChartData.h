#pragma once
#include <string>
#include "Global/VtDefine.h"
#include <array>
#include <map>
#include <vector>
#include <queue>
#include <set>
#include "Xml/pugixml.hpp"
#include "chartdir.h"
class VtChartWindow;
class VtSystem;
class VtSymbol;
struct TempData
{
	std::array<double, TempDataSize> High;
	std::array<double, TempDataSize> Low;
	std::array<double, TempDataSize> Open;
	std::array<double, TempDataSize> Close;
	std::array<double, TempDataSize> Volume;
	std::array<double, TempDataSize> TimeInfo;
	std::array<VtDate, ChartDataSize> Date;
	std::array<VtTime, ChartDataSize> Time;
	std::array<double, ChartDataSize> DateTime;
};

struct VtChartDataItem
{
	int Index = -1;
	bool Exist = false;
	double High = Chart::NoValue;
	double Low = Chart::NoValue;
	double Open = Chart::NoValue;
	double Close = Chart::NoValue;
	double Volume = Chart::NoValue;
	VtDate Date;
	VtTime Time;
	double DateTime;
};


struct ChartDataSet
{
	double High;
	double Low;
	double Open;
	double Close;
	double Volume;
	double TimeInfo;
};

struct DataBorder
{
	int startIndex;
	int endIndex;
	VtDate StartDate;
	VtDate EndDate;
	VtTime StartTime;
	VtTime EndTime;
};

typedef std::map<std::pair<int, int>, VtChartDataItem> ChartDataItemMap;

class VtChartData
{
public:
	VtChartData();
	~VtChartData();
	TempData InputChartData;

	std::queue<DataBorder> BorderQ;
	std::string _ChartID;
	std::array<int, ChartDataSize> Id;
	std::array<double, ChartDataSize> High;
	std::array<double, ChartDataSize> Low;
	std::array<double, ChartDataSize> Open;
	std::array<double, ChartDataSize> Close;
	std::array<double, ChartDataSize> Volume;
	std::array<double, ChartDataSize> TimeInfo;
	// Items under is for the chart display.
	std::array<VtDate, ChartDataSize> Date;
	std::array<VtTime, ChartDataSize> Time;
	std::array<double, ChartDataSize> DateTime;
	std::map<std::pair<int, int>, int> InputDateTimeMap;
	std::set<std::pair<int, int>> DateTimeSet;
	std::map<std::pair<int, int>, int> SyncDateTimeMap;

	void Reset();
	void ShiftLeft(int count);
	void ShiftIdLeft(int count);
	static void ShiftLeft(std::vector<double>& src, int count);
	int FindIndex(VtDate date, VtTime time);

	int GetDataCount()
	{
		return _FilledCount;
	}

	static VtDate GetDate(char* src);
	static VtTime GetTime(char* src);
	static VtDate GetDate(CString src);
	static VtTime GetTimeForAbroad(CString src);
	static std::pair<VtDate, VtTime> GetDateTime(CString datetime);

	int Cycle() const { return _Cycle; }
	void Cycle(int val) { _Cycle = val; }
	VtChartType ChartType() const { return _ChartType; }
	void ChartType(VtChartType val) { _ChartType = val; }
	std::string SymbolCode() const { return _SymbolCode; }
	void SymbolCode(std::string val) { _SymbolCode = val; }
	int CurIndex() const { return _CurIndex; }
	void CurIndex(int val) { _CurIndex = val; }
	std::string MakeID(std::string fullCode, VtChartType type, int cycle);
	void OnTimer();
	std::string GetChartDataKey();
	void RequestChartData();
	double GetLastTimeInfo()
	{
		return TimeInfo.back();
	}
	void AddDataByRecent(int rcvIndex, ChartDataSet& data);
	void AdjustData(int foundIndex);
	void CopyData(int rcvCnt, int foundIndex);
	void FilledCount(int val) { _FilledCount = val; }
	void OnReceiveQuoteHd(VtSymbol* sym);
	void OnReceiveChartData(VtChartData* data);
	void AddSystem(VtSystem* sys);
	void RemoveSystem(VtSystem* sys);
	void AddChartWindow(VtChartWindow* chartWnd);
	void RemoveChartWindow(VtChartWindow* chartWnd);
	//int GetReferenceChartCount();

	std::string Key() const { return _Key; }
	void Key(std::string val) { _Key = val; }
	std::string Exchange() const { return _Exchange; }
	void Exchange(std::string val) { _Exchange = val; }
	std::string FullCode() const { return _FullCode; }
	void FullCode(std::string val) { _FullCode = val; }
	VtChartRequestType RequestType() const { return _RequestType; }
	void RequestType(VtChartRequestType val) { _RequestType = val; }
	std::string Currency() const { return _Currency; }
	void Currency(std::string val) { _Currency = val; }
	bool Selected() const { return _Selected; }
	void Selected(bool val) { _Selected = val; }
	int DataCount() const { return _FilledCount; }
	void DataCount(int val) { _FilledCount = val; }
	int LineColor() const { return _LineColor; }
	void LineColor(int val) { _LineColor = val; }

	double RealTimeClose() const { return _RealTimeClose; }
	void RealTimeClose(double val);

	void CopyPrevData(int index);

	int FindIndex(double dateTime);
	void OnUpdateDataCount(int oldCount, int newCount);
	virtual void SaveToXml(pugi::xml_node& node);
	virtual std::string LoadFromXml(pugi::xml_node& node);

	int GetLastIndex()
	{
		return Id[GetDataCount() - 1];
	}

	bool Filled() const { return _Filled; } 
	void Filled(bool val) { _Filled = val; }
	std::string keyvalue;
	
	bool Domestic() const { return _Domestic; }
	void Domestic(bool val) { _Domestic = val; }
	int Decimal() const { return _Decimal; }
	void Decimal(int val) { _Decimal = val; }
	VtMainChartType MainChartType() const { return _MainChartType; }
	void MainChartType(VtMainChartType val) { _MainChartType = val; }
	int ColorIndex() const { return _ColorIndex; }
	void ColorIndex(int val) { _ColorIndex = val; }
	std::string Initial() const { return _Initial; }
	void Initial(std::string val) { _Initial = val; }
	std::vector<double>& GetDataArray(std::string dataName);
	VtChartDataItem GetChartData(int index);
	VtChartDataItem GetInputChartData(int index);
	void SetChartData(int index, VtChartDataItem item);
private:
	
	void InitChartData();
	std::map<std::string, std::vector<double>> _DataCol;
	/// <summary>
	/// 데이터를 채울 때 현재 어디까지 데이터가 차 있는지 보여줌
	/// </summary>
	int _CurIndex;
	/// <summary>
	/// 상품코드 풀코드가 들어가 있다.
	/// </summary>
	std::string _SymbolCode;
	int _Cycle = 10;
	VtChartType _ChartType = VtChartType::MIN;
	int _FirstRequestCount;
	int _CyclicRequestCount;
	int _FilledCount;

	std::map<int, VtSystem*> _SystemMap;
	std::map<int, VtChartWindow*> _ChartMap;
	std::map<std::string, std::vector<VtChartWindow*>> _ChartWndMap;
	std::string _Key;
	std::string _Exchange;
	std::string _FullCode;
	VtChartRequestType _RequestType;
	std::string _Currency;
	bool _Selected = false;
	double _RealTimeClose;
	int _DataCount = 0;
	int _LineColor;
	bool _Filled;
	bool _Domestic;
	int _Decimal;
	int _ColorIndex = 0;
	std::string _Initial;

	void SendMessageToChartWnd(bool real);

	VtMainChartType _MainChartType = VtMainChartType::CandleStick;
};

