#pragma once
#include <string>
struct HdProductInfo
{
	// 품목 코드
	std::string code;
	// 소수점
	int decimal;
	// 정수 틱크기
	int intTickSize;
	// 틱크기 - 문자열
	std::string tickSize;
	// 틱가치 - 정수
	int tickValue;
	// 승수
	int tradeWin;
	// 호가 단위 정수
	int hogaUnit;
	// 품목 이름
	std::string name;
};