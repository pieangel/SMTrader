#pragma once
#include "Global/TemplateSingleton.h"
#include <string>
#include <tuple>
#include <map>
struct VtPosition;
typedef std::map<std::tuple<std::string, std::string, std::string>, VtPosition*> SmSystemPositionMap;
class SmSystemPositionManager : public TemplateSingleton<SmSystemPositionManager>
{
public:
	SmSystemPositionManager();
	~SmSystemPositionManager();
	// 전략이름, 계좌이름, 종목코드
	void AddPosition(std::tuple<std::string, std::string, std::string> key, VtPosition* position);
	VtPosition* FindPosition(std::tuple<std::string, std::string, std::string> key);
private:
	SmSystemPositionMap _PositionMap;
};

