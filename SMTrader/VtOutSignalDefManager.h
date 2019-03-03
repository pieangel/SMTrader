#pragma once
#include "Global/TemplateSingleton.h"
#include <map>
#include <string>
#include <memory>
#include <vector>
class VtOutSignalDef;
typedef std::vector<std::shared_ptr<VtOutSignalDef>> OutSigDefVec;
typedef std::shared_ptr<VtOutSignalDef> SharedOutSigDef;
class VtOutSignalDefManager : public TemplateSingleton<VtOutSignalDefManager>
{
public:
	VtOutSignalDefManager();
	~VtOutSignalDefManager();
	void AddOutSignalDef(SharedOutSigDef outSigDef);
	void RemoveOutSignalDef(std::string name);
	void InitDefaultOutSignalDefs();
	SharedOutSigDef FindOutSigDef(std::string name);
	OutSigDefVec& GetSignalDefVec()
	{
		return _SignalDefVec;
	}
	std::string GetOutSigDefString();
private:
	OutSigDefVec _SignalDefVec;
};

