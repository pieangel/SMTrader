#pragma once
#include <map>
#include <string>
#include "SimpleBinStream.h"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;
class VtProductOrderManager;
class VtProductOrderManagerSelector
{
public:
	VtProductOrderManagerSelector();
	~VtProductOrderManagerSelector();

	// The key value is the fullcode of the product.
	std::map<std::string, VtProductOrderManager*> _OrderManagerMap;

	VtProductOrderManager* FindAdd(std::string symbolCode);
	VtProductOrderManager* Find(std::string symbolCode);

	void ClearAllAccepted();
	void ClearAllFilled();
	void ClearAccepted(VtProductOrderManager* prdtOrderMgr);
	void ClearFilled(VtProductOrderManager* prdtOrderMgr);
	bool GetInit();

	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);
};

