#pragma once
#include <string>

enum class VtIndexType
{
	SimpleMa,
	DataVolume,
	BollingerBand,
	PriceChannel,
	Macd,
	Rsi,
	Stochastic,
	Trix,
	Atr
};


enum class VtBandType
{
	Ndaily,
	Daily,
	Entire,
	ORB
};

enum class VtStressType
{
	VRobo1,
	VRobo2
};


struct VtIndexBase
{
	VtIndexType Type;
	std::string Name;
	std::string Desc;
};


struct VtBandBase
{
	VtBandType Type;
	std::string Name;
	std::string Desc;
};


struct VtStressBase
{
	VtStressType Type;
	std::string Name;
	std::string Desc;
};

