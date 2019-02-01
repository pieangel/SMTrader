#include "stdafx.h"
#include "VtColorManager.h"
#include <string>
#include <stdio.h>

#define BUILD_ARGB32(a,b,g,r) ( (b) | ( (g)<<8 ) | ( (r) <<16) | ( (a) <<24 ) ) 
#define BUILD_XRGB32(b,g,r) BUILD_ARGB32(255, r, g, b)

VtColorManager::VtColorManager()
{
	_UsedIndex = -1;
	InitColorTable();
}


VtColorManager::~VtColorManager()
{
}

int VtColorManager::GetNextColorIndex()
{
	if (_UsedColorIndex.size() > 0)
	{
		int index = _UsedColorIndex.front();
		_UsedColorIndex.pop();
		return index;
	}
	else
	{
		_UsedIndex++;
		return _UsedIndex >= 45 ? 0 : _UsedIndex;
	}
}

int VtColorManager::AddCustomColorByRGB(COLORREF color)
{
	unsigned int value = color;
	BYTE b, g, r;
	b = (value & 0xFF); //extract first byte
	g = ((value >> 8) & 0xFF); //extract second byte
	r = ((value >> 16) & 0xFF); //extract third byte
	int index = _CustomColorIndex;
	ColorTable[_CustomColorIndex++] = BUILD_XRGB32(b, g, r);
	return index;
}

int VtColorManager::ColorRefToARGB(COLORREF color)
{
	unsigned int value = color;
	BYTE b, g, r;
	b = (value & 0xFF); //extract first byte
	g = ((value >> 8) & 0xFF); //extract second byte
	r = ((value >> 16) & 0xFF); //extract third byte
	return BUILD_XRGB32(b, g, r);
}

COLORREF VtColorManager::ARGBToColorRef(int color)
{
	unsigned int value = color;
	BYTE b, g, r;
	b = (value & 0xFF); //extract first byte
	g = ((value >> 8) & 0xFF); //extract second byte
	r = ((value >> 16) & 0xFF); //extract third byte

	return RGB(r, g, b);
}

std::string VtColorManager::RGBToString(int colorIndex)
{
	int color = ColorTable[colorIndex];
	unsigned int value = color;
	BYTE b, g, r;
	b = (value & 0xFF); //extract first byte
	g = ((value >> 8) & 0xFF); //extract second byte
	r = ((value >> 16) & 0xFF); //extract third byte

	std::string result;

	char rbuf[255];
	sprintf_s(rbuf, _T("%.2X"), r);
	result.append(rbuf);

	char gbuf[255];
	sprintf_s(gbuf, _T("%.2X"), g);
	result.append(gbuf);

	char bbuf[255];
	sprintf_s(bbuf, _T("%.2X"), b);
	result.append(bbuf);

	return result;
}

void VtColorManager::ReturnColorIndex(int index)
{
	_UsedColorIndex.push(index);
}

int VtColorManager::GetColor(int index, bool RGB)
{
	//return ColorTable[index];
	unsigned int value = ColorTable[index]; //4 Bytes value

	BYTE b, g, r, a; //to store byte by byte value


	b = (value & 0xFF); //extract first byte
	g = ((value >> 8) & 0xFF); //extract second byte
	r = ((value >> 16) & 0xFF); //extract third byte
	a = ((value >> 24) & 0xFF); //extract fourth byte

	if (RGB)
		return RGB(r, g, b);
	else
		return value;
}

void VtColorManager::InitColorTable()
{
	ColorTable[0] = 0x00000000;
	ColorTable[1] = 0x00ff0000;
	ColorTable[2] = 0x000033ff;
	ColorTable[3] = 0x00006600;
	ColorTable[4] = 0x00ff00ff;
	ColorTable[5] = 0x009900cc;
	ColorTable[6] = 0x000099ff;
	ColorTable[7] = 0x000000ff;
	ColorTable[8] = 0x009900cc;
	ColorTable[9] = 0x00ff0099;
	//ColorTable[10] = 0x00cc0000;

	ColorTable[10] = 0x00cc3300;
	ColorTable[11] = 0x00ffcc00;
	ColorTable[12] = 0x00009900;
	ColorTable[13] = 0x00006666;
	ColorTable[14] = 0x000066ff;
	ColorTable[15] = 0x000000cc;
	ColorTable[16] = 0x00663399;
	ColorTable[17] = 0x00cc0099;
	ColorTable[18] = 0x00990000;
	ColorTable[19] = 0x00993300;
	ColorTable[20] = 0x00cc9900;
	ColorTable[21] = 0x00006600;
	ColorTable[22] = 0x00336666;
	ColorTable[23] = 0x000033ff;
	ColorTable[24] = 0x00000099;
	ColorTable[25] = 0x00660099;
	ColorTable[26] = 0x00990066;
	ColorTable[27] = 0x00660000;
	ColorTable[28] = 0x00663300;

	ColorTable[29] = 0x00996633;
	ColorTable[30] = 0x00003300;
	ColorTable[31] = 0x00003333;
	ColorTable[32] = 0x00003399;
	ColorTable[33] = 0x00000066;
	ColorTable[34] = 0x00330066;

	ColorTable[35] = 0x00660066;
	ColorTable[36] = 0x00cc3333;
	ColorTable[37] = 0x00ff6633;
	ColorTable[38] = 0x00ffff33;
	ColorTable[39] = 0x0000ff00;
	ColorTable[40] = 0x0000cccc;
	ColorTable[41] = 0x0000ccff;

	ColorTable[42] = 0x003366ff;
	ColorTable[43] = 0x009933ff;
	ColorTable[44] = 0x00ff66ff;

	for (int i = 45; i < ColorTableSize; ++i)
	{
		ColorTable[i] = 0xffffffff;
	}
}
