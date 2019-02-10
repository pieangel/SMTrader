#pragma once
enum class VtSystemGroupType {
	/// <summary>
	/// 주가지수 선물
	/// </summary>
	KOSPI200F,
	/// <summary>
	/// 주가지수 옵션
	/// </summary>
	KOSPI200O,
	/// <summary>
	/// 코스탁150선물
	/// </summary>
	KOSDAQ150F,
	/// <summary>
	/// 원달러 선물
	/// </summary>
	USDF
};

enum class TargetType {
	RealAccount,
	SubAccount,
	Fund
};

struct TrailingStop {
	// 최소 이익
	double MinProfit = 50000;
	// 트레일링 퍼센트
	double TrailingPercent = 10;
};

struct ArgDataSource {
	std::string SymbolCode;
	std::string SellSource;
	std::string SellData;
	std::string BuySource;
	std::string BuyData;
};

enum class ArgNameType {
	None, // None
	KbsMiKas, // Kbs - Kas
	KbcGtKac, // Kbc > Kac
	QbcGtQac, // Qbc > Qac
	UacGtUbc, // Uac > Ubc
	KasMiKbs, // Kas - Kbs
	KacGtKbc, // Kac > Kbc
	QacGtQbc, // Qac > Qbc
	UbcGtUac, // Ubc > Uac
	QbsGtQas, // Qbs > Qas
	QasGtQbs, // Qas > Qbs
	UbsGtUas, // Ubs > Uas
	UasGtUbs,  // Uas > Ubs
	KasGtKbs, // Kas > Kbs
	KbsGtKas // Kbs > Kas
};

enum class ValueType {
	Tick,
	Percent
};
