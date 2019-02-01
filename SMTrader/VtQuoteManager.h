#pragma once
#include "Global/TemplateSingleton.h"
#include <map>
#include <string>
#include "VtQuoteEvent.h"
#include "Poco/BasicEvent.h"

using Poco::BasicEvent;

struct VtRealtimeQuote;
struct VtTickQuote;
struct VtQuote;
class VtQuoteManager : public TemplateSingleton<VtQuoteManager>
{
public:
	VtQuoteManager();
	~VtQuoteManager();

	std::map<std::string, VtRealtimeQuote*> RealtimeQuoteMap;
	void AddRealtimeQuote(std::string fullCode, VtRealtimeQuote* realQuote);
	void RemoveRealtimeQuote(std::string fullCode);
	void ReceiveRealtimeQuote(std::string fullCode, LPCTSTR packet);
	void ReceiveRealtimeQuote(VtQuote* quote);
	VtRealtimeQuote* FindRealtimeQuote(std::string fullCode);
	VtRealtimeQuote* FindAddRealQuote(std::string fullCode);

	std::map<std::string, VtTickQuote*> TickQuoteMap;
	void AddTickQuote(std::string fullCode, VtTickQuote* realQuote);
	void RemoveTickQuote(std::string fullCode);
	void ReceiveTickQuote(std::string fullCode, LPCTSTR packet);
	VtTickQuote* FindTickQuote(std::string fullCode);
	VtTickQuote* FindAddTickQuote(std::string fullCode);

	std::map<std::string, VtQuote*> QuoteMap;
	void AddQuote(std::string fullCode, VtQuote* realQuote);
	void RemoveQuote(std::string fullCode);
	void ReceiveQuote(std::string fullCode, LPCTSTR packet);
	void ReceiveQuote(VtQuote* quote);
	VtQuote* FindQuote(std::string fullCode);
	VtQuote* FindAddQuote(std::string fullCode);

	BasicEvent<VtRealtimeQuoteEventArgs> _RealtimeQuoteReceivedEvent;
	void FireRealtimeQuoteReceived(VtRealtimeQuoteEventArgs&& arg)
	{
		_RealtimeQuoteReceivedEvent(this, arg);
	}
};

