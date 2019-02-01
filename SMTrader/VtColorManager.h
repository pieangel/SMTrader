#pragma once
#include <queue>
const int ColorTableSize = 1000;
class VtColorManager 
{
public:
	VtColorManager();
	~VtColorManager();
	void ReturnColorIndex(int index);
	int GetColor(int index, bool RGB = false);
	int GetNextColorIndex();
	int AddCustomColorByRGB(COLORREF color);
	static int ColorRefToARGB(COLORREF color);
	static COLORREF ARGBToColorRef(int color);
	std::string RGBToString(int colorIndex);
private:
	unsigned int ColorTable[ColorTableSize];

	void InitColorTable();

	std::queue<int> _UsedColorIndex;

	int _UsedIndex;
	int _CustomColorIndex = 45;
};

