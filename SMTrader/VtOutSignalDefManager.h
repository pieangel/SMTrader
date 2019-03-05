#pragma once
#include "Global/TemplateSingleton.h"
#include <map>
#include <string>
#include <memory>
#include <vector>
#include "SimpleBinStream.h"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;

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
	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);
private:
	OutSigDefVec _SignalDefVec;
};

