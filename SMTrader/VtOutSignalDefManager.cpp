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
	for (int i = 0; i < 5; ++i) {
		outSig = std::make_shared<VtOutSignalDef>();
		outSig->SignalName = _T("OS_CLI");
		outSig->SignalName.append(std::to_string(i + 1));
		outSig->Desc = _T("OS_CLI");
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
