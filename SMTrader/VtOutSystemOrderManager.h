#pragma once
#include "Global/TemplateSingleton.h"
#include "VtOutSysDef.h"
#include <string>
#include <map>
#include <sstream>

class VtOutSystemOrderManager : public TemplateSingleton<VtOutSystemOrderManager>
{
public:
	VtOutSystemOrderManager();
	~VtOutSystemOrderManager();
	void AddSystem(SharedSystem sys);
	void RemoveSystem(SharedSystem sys);
	SharedSystemVec& GetSysVec()
	{
		return _SystemVec;
	}
	void OnOutSignal(std::string sig);
private:
	void AddSignalOrder(SharedSystem sys);
	void RemoveSignalOrder(SharedSystem sys);
	SharedSystemVec _SystemVec;
	std::map<std::string, SharedSystemMap> _SignalOrderMap;
	/// <summary>
	/// 문자열 쪼개는 함수
	/// </summary>
	/// <param name="s"></param>
	/// <param name="delimiter"></param>
	/// <returns></returns>
	std::vector<std::string> split(const std::string& s, char delimiter) {
		std::vector<std::string> tokens;
		std::string token;
		std::istringstream tokenStream(s);
		while (std::getline(tokenStream, token, delimiter)) {
			tokens.push_back(token);
		}
		return tokens;
	}
	std::string GetTime();
	void PutOrder(std::string sigName, int orderKind);
};

