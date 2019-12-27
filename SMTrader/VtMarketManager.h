#pragma once
#include "Global/TemplateSingleton.h"
#include <string>
#include <map>
#include <vector>
#include <set>
#include "SmRunInfo.h"
#include "Xml/pugixml.hpp"
class SmMarket;
class SmProduct;
class VtSymbol;

class VtMarketManager : public TemplateSingleton<VtMarketManager>
{
public:
	VtMarketManager();
	~VtMarketManager();
};

