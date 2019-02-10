#pragma once
enum class VtSystemGroupType {
	/// <summary>
	/// �ְ����� ����
	/// </summary>
	KOSPI200F,
	/// <summary>
	/// �ְ����� �ɼ�
	/// </summary>
	KOSPI200O,
	/// <summary>
	/// �ڽ�Ź150����
	/// </summary>
	KOSDAQ150F,
	/// <summary>
	/// ���޷� ����
	/// </summary>
	USDF
};

enum class TargetType {
	RealAccount,
	SubAccount,
	Fund
};

struct TrailingStop {
	// �ּ� ����
	double MinProfit = 50000;
	// Ʈ���ϸ� �ۼ�Ʈ
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
