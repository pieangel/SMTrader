#include "stdafx.h"
#include "VtOutSystemOrderManager.h"
#include "System/VtSystem.h"
#include <algorithm>
#include "VtOutSignalDef.h"
#include <sstream>
#include <string>
#include "File/path.h"
#include "File/resolver.h"
#include "VtPosition.h"

VtOutSystemOrderManager::VtOutSystemOrderManager()
{
}


VtOutSystemOrderManager::~VtOutSystemOrderManager()
{
}

void VtOutSystemOrderManager::AddSystem(SharedSystem sys)
{
	if (!sys)
		return;
	sys->RegisterRealtimeSymbolEvent();
	_SystemVec.push_back(sys);
	AddSignalOrder(sys);
}

void VtOutSystemOrderManager::RemoveSystem(SharedSystem sys)
{
	if (!sys) return;

	auto it = std::find_if(_SystemVec.begin(), _SystemVec.end(), [&](const SharedSystem& x) { return x->Id() == sys->Id(); });
	if (it != _SystemVec.end()) {
		RemoveSignalOrder(sys);
		_SystemVec.erase(it);
	}
}

#pragma warning ( push )
#pragma warning ( disable : 4996 )
void VtOutSystemOrderManager::OnOutSignal(std::string sig)
{
	std::vector<std::string> result = split(sig, ',');
// 	for (auto it = result.begin(); it != result.end(); /* NOTHING */)
// 	{
// 		if ((*it).empty())
// 			it = result.erase(it);
// 		else
// 			++it;
// 	}
	if (result.size() > 5 && result[0].length() > 0) {
		filesystem::path full_path(result[0]);
		std::string signame = full_path.filename();
		auto pos = signame.find_first_of(_T("-"));
		signame = signame.substr(0, pos);
		CString strOrderKind(result[4].c_str());
		int order_kind = _ttoi(strOrderKind);
		PutOrder(signame, order_kind);
	}
}

#pragma  warning ( pop )

void VtOutSystemOrderManager::AddSignalOrder(SharedSystem sys)
{
	if (!sys || !sys->OutSignal()) return;

	auto it = _SignalOrderMap.find(sys->OutSignal()->SignalName);
	if (it != _SignalOrderMap.end()) {
		SharedSystemMap& orderMap = it->second;
		orderMap[sys->Id()] = sys;
	}
	else {
		SharedSystemMap orderMap;
		orderMap[sys->Id()] = sys;
		_SignalOrderMap[sys->OutSignal()->SignalName] = orderMap;
	}
}

void VtOutSystemOrderManager::RemoveSignalOrder(SharedSystem sys)
{
	if (!sys || !sys->OutSignal()) return;

	auto it = _SignalOrderMap.find(sys->OutSignal()->SignalName);
	if (it != _SignalOrderMap.end()) {
		SharedSystemMap& orderMap = it->second;
		auto iit = orderMap.find(sys->Id());
		if (iit != orderMap.end()) {
			orderMap.erase(iit);
		}
	}
}

std::string VtOutSystemOrderManager::GetTime()
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80] = { 0 };

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, 80, " %H:%M:%S", timeinfo);
	puts(buffer);
	std::string curTime = buffer;
	return curTime;
}

void VtOutSystemOrderManager::PutOrder(std::string sigName, int orderKind)
{
	CString msg;
	auto it = _SignalOrderMap.find(sigName);
	if (it != _SignalOrderMap.end()) {
		SharedSystemMap& orderMap = it->second;
		for (auto itn = orderMap.begin(); itn != orderMap.end(); ++itn) {
			SharedSystem sys = itn->second;
			// ��Ȱ��ȭ �ý����� �׳� �Ѿ��.
			if (!sys->Enable()) continue;

			// Ȱ��ȭ�� �ý��ۿ� ���Ͽ� �ֹ��� �ִ´�.
			switch (orderKind)
			{
			case 1: // Buy
				//msg.Format(_T("��ȣ�̸� : %s, ��ȣ ���� : %d, �ֹ� : �ż�\n"), sigName.c_str(), orderKind);
				//TRACE(msg);
				LOG_F(INFO, _T("��ȣ�̸� : %s, ��ȣ ���� : %d, �ֹ� : �ż�"), sigName.c_str(), orderKind);
				sys->PutEntranceOrder(VtPositionType::Buy);
				
				break;
			case 2: { // ExitLong -> Sell
				VtPosition posi = sys->GetPosition();
				if (std::abs(posi.OpenQty) > 0) {
					LOG_F(INFO, _T("��ȣ�̸� : %s, ��ȣ ���� : %d, �ֹ� : �ŵ�"), sigName.c_str(), orderKind);
					sys->PutEntranceOrder(VtPositionType::Sell);
					//msg.Format(_T("��ȣ�̸� : %s, ��ȣ ���� : %d, �ֹ� : �ŵ�\n"), sigName.c_str(), orderKind);
					//TRACE(msg);
					
				}
			}
				break;
			case 3: // Sell
				LOG_F(INFO, _T("��ȣ�̸� : %s, ��ȣ ���� : %d, �ֹ� : �ŵ�"), sigName.c_str(), orderKind);
				sys->PutEntranceOrder(VtPositionType::Sell);
				//msg.Format(_T("��ȣ�̸� : %s, ��ȣ ���� : %d, �ֹ� : �ŵ�\n"), sigName.c_str(), orderKind);
				//TRACE(msg);
				
				break;
			case 4: { // ExitShort -> Buy
				VtPosition posi = sys->GetPosition();
				if (std::abs(posi.OpenQty) > 0) {
					LOG_F(INFO, _T("��ȣ�̸� : %s, ��ȣ ���� : %d, �ֹ� : �ż�"), sigName.c_str(), orderKind);
					sys->PutEntranceOrder(VtPositionType::Buy);
					//msg.Format(_T("��ȣ�̸� : %s, ��ȣ ���� : %d, �ֹ� : �ż�\n"), sigName.c_str(), orderKind);
					//TRACE(msg);
					
				}
			}
				break;
			default:
				break;
			}
		}
	}
}
