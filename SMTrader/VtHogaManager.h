#pragma once
#include "Global/TemplateSingleton.h"
#include <map>
#include <string>

struct VtRealtimeHoga;
struct VtHoga;
class VtHogaManager : public TemplateSingleton<VtHogaManager>
{
public:
	VtHogaManager();
	~VtHogaManager();

	std::map<std::string, VtRealtimeHoga*> RealtimeHogaMap;
	void AddRealtimeHoga(std::string fullCode, VtRealtimeHoga* realtimeHoga);
	void ReceiveRealtimeHoga(std::string fullCode, LPCTSTR packet);
	void ReceiveRealtimeHoga(VtRealtimeHoga* hoga);
	void ReceiveRealtimeHoga(VtHoga* hoga);
	void RemoveRealtimeHoga(std::string fullCode);
	VtRealtimeHoga* FindAddRealtimeHoga(std::string fullCode);

	std::map<std::string, VtHoga*> HogaMap;
	void AddHoga(std::string fullCode, VtHoga* realtimeHoga);
	void ReceiveHoga(std::string fullCode, LPCTSTR packet);
	void ReceiveHoga(VtHoga* hoga);
	void RemoveHoga(std::string fullCode);
	VtHoga* FindAddHoga(std::string fullCode);
	VtHoga* FindHoga(std::string fullCode);

	void RequestHoga(std::string fullCode);
};

