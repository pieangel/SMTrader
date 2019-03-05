#pragma once
#include "Global/TemplateSingleton.h"
#include <map>
#include "VtOutSysDef.h"
#include "SimpleBinStream.h"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;

class VtOutSystemManager : public TemplateSingleton<VtOutSystemManager>
{
public:
	void AddSystem(SharedSystem sys);
	void RemoveSystem(int sysId);
	VtOutSystemManager();
	~VtOutSystemManager();
	SharedSystemVec& GetSysMap()
	{
		return _SystemVec;
	}
	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);
private:
	SharedSystemVec _SystemVec;
};

