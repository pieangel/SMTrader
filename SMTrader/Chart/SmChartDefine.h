#pragma once
#include <string>
const size_t SMChartDataSize = 1500;
enum class SmChartType
{
	NONE = 0,
	TICK,
	MIN,
	DAY,
	WEEK,
	MON,
	YEAR
};

enum class SmChartDataReqestType {
	FIRST = 0, // ���� �䱸
	CYCLE // �ֱ� ������ �䱸
};

struct SmSiseDataRequest {
	std::string user_id;
	std::string symbol_code;
};

struct SmHogaDataRequest {
	std::string user_id;
	std::string symbol_code;
};

struct SmSymbolMasterRequest {
	std::string user_id;
	std::string symbol_code;
};

struct SmChartDataRequest
{
	SmChartDataReqestType reqType = SmChartDataReqestType::FIRST;
	int service_req_id;
	int session_id;
	std::string chart_id;
	std::string user_id;
	std::string symbolCode;
	SmChartType chartType;
	int cycle = 0;
	int count = 0;
	int next = 0;
	int reqKey = 0;
	// ������ 0 : ������, 1 : �����
	int seq = 0;
	bool domestic = false;
	std::string GetDataKey() {
		std::string key = symbolCode;
		key.append(":");
		key.append(std::to_string((int)chartType));
		key.append(":");
		key.append(std::to_string(cycle));
		return key;
	}
};

struct SmChartDataItem {
	std::string symbolCode;
	SmChartType chartType;
	int cycle = 0;
	std::string date;
	std::string time;
	std::string date_time;
	int o = 0;
	int h = 0;
	int l = 0;
	int c = 0;
	int v = 0;
	std::string GetDataKey() {
		std::string key = symbolCode;
		key.append(":");
		key.append(std::to_string((int)chartType));
		key.append(":");
		key.append(std::to_string(cycle));
		return key;
	}
};

struct SmSimpleChartDataItem {
	std::string date_time;
	int o = 0;
	int h = 0;
	int l = 0;
	int c = 0;
	int v = 0;
};

