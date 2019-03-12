#include "stdafx.h"
#include "VtOutSignalDefManager.h"
#include "VtOutSignalDef.h"
#include <algorithm>

VtOutSignalDefManager::VtOutSignalDefManager()
{
	InitDefaultOutSignalDefs();
}


VtOutSignalDefManager::~VtOutSignalDefManager()
{
}

void VtOutSignalDefManager::AddOutSignalDef(SharedOutSigDef outSigDef)
{
	_SignalDefVec.push_back(outSigDef);
}

void VtOutSignalDefManager::RemoveOutSignalDef(std::string name)
{
	auto it = std::find_if(_SignalDefVec.begin(), _SignalDefVec.end(), [&](const SharedOutSigDef& x) { return x->SignalName.compare(name); });
	if (it != _SignalDefVec.end()) {
		_SignalDefVec.erase(it);
	}
}

void VtOutSignalDefManager::InitDefaultOutSignalDefs()
{
	SharedOutSigDef outSig(nullptr);
	for (int i = 0; i < 20; ++i) {
		outSig = std::make_shared<VtOutSignalDef>();
		outSig->SignalName = _T("T");
		outSig->SignalName.append(std::to_string(i + 1));
		outSig->Desc = _T("");
		outSig->Desc.append(std::to_string(i + 1));
		_SignalDefVec.push_back(outSig);
	}
}

SharedOutSigDef VtOutSignalDefManager::FindOutSigDef(std::string name)
{
	auto it = std::find_if(_SignalDefVec.begin(), _SignalDefVec.end(), [&](const SharedOutSigDef& x) { return x->SignalName.compare(name) == 0; });
	if (it != _SignalDefVec.end()) {
		return *it;
	}

	return nullptr;
}

std::string VtOutSignalDefManager::GetOutSigDefString()
{
	std::string result;
	for (auto it = _SignalDefVec.begin(); it != _SignalDefVec.end(); ++it) {
		SharedOutSigDef& sig = *it;
		result.append(sig->SignalName);
		result.append(_T("\n"));
	}
	return result;
}

void VtOutSignalDefManager::Save(simple::file_ostream<same_endian_type>& ss)
{
	size_t count = _SignalDefVec.size();
	ss << count; // 시스템 갯수

	// 각 시스템을 저장한다.
	for (auto it = _SignalDefVec.begin(); it != _SignalDefVec.end(); ++it) {
		SharedOutSigDef sig = *it;
		sig->Save(ss);
	}
}

void VtOutSignalDefManager::Load(simple::file_istream<same_endian_type>& ss)
{
	int count = 0;
	ss >> count;
	if (count == 0)
		return;

	for (int i = 0; i < count; ++i) {
		SharedOutSigDef sig = std::make_shared<VtOutSignalDef>();
		sig->Load(ss);
		AddOutSignalDef(sig);
	}
}
