#include "stdafx.h"
#include "VtOrder.h"

void VtOrder::Save(simple::file_ostream<same_endian_type>& ss)
{
	ss << chartID << requestID << AccountNo << (int)orderType << (int)orderPosition << (int)priceType;
	ss << (int)tradeType << shortCode << fullCode << orderPrice << orderNo << amount << oriOrderNo << tradeDate;
	ss << filledQty << filledPrice << tradeTime << (int)state << productDecimal << SubAccountNo;
	ss << RequestId << SourceId << filledTime << FundName;
}

void VtOrder::Load(simple::file_istream<same_endian_type>& ss)
{

}
