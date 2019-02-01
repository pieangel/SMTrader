#pragma once
#include <windows.h>
#include <string>
const int StringBufferLen = 512;
const int NumberBufferLen = 128;
class ZmStream 
{
public:
	ZmStream(VOID);
	~ZmStream(VOID);

private:
	BYTE	*_BufferPointer;
	DWORD	_Length;

public:
	void    SetOffset(int offset);
	BOOL	SetBuffer(BYTE *buffer);
	BOOL    SetBuffer(char* buffer);
	BOOL	SetBuffer();

	int ReadInt(int len);
	short ReadShort(int len);
	float ReadFloat(int len);
	double ReadDouble(int len);
	std::string ReadString(int len);
	void ReadString(char* des, int len);
	void ReadStringPadRight(char* des, int len, char padding = ' ');

	void WriteInt(int input, int len);
	void WriteIntPadRight(int input, int len, char pad = '0');
	void WriteFloat(float input, int len);
	void WriteFloatPadRight(float input, int len, char pad = '0');
	void WriteDouble(double input, int len);
	void WriteDoublePadRight(double input, int len, char pad = '0');
	void WriteDoublePadLeft(double input, int decimal, int len, char pad = '0');
	void WriteString(char* input, int len);
	void WriteStringPadRight(char* input, int len, char pad = ' ');
	void WriteString(std::string input, int len);
	void WriteStringPadRight(std::string input, int len, char pad = ' ');


	BOOL	ReadInt32(INT *data);
	BOOL	ReadDWORD(DWORD *data);
	BOOL	ReadDWORD_PTR(DWORD_PTR *data);
	BOOL	ReadByte(BYTE *data);
	BOOL	ReadBytes(BYTE *data, DWORD length);
	BOOL	ReadFloat(FLOAT *data);
	BOOL	ReadInt64(INT64 *data);
	BOOL	ReadUSHORT(USHORT *data);
	BOOL	ReadSHORT(SHORT *data);
	BOOL	ReadBOOL(BOOL *data);
	BOOL	ReadWCHAR(WCHAR *data);
	BOOL	ReadWCHARs(LPWSTR data, DWORD length);

	BOOL	WriteInt32(INT data);
	BOOL	WriteDWORD(DWORD data);
	BOOL	WriteDWORD_PTR(DWORD_PTR data);
	BOOL	WriteByte(BYTE data);
	BOOL	WriteBytes(BYTE *data, DWORD length);
	BOOL	WriteFloat(FLOAT data);
	BOOL	WriteInt64(INT64 data);
	BOOL	WriteUSHORT(USHORT data);
	BOOL	WriteSHORT(SHORT data);
	BOOL	WriteBOOL(BOOL data);
	BOOL	WriteWCHAR(WCHAR data);
	BOOL	WriteWCHARs(LPCWSTR data, DWORD length);

	DWORD	GetLength(VOID);
};

class ZmStreamSP
{
public:
	ZmStreamSP(VOID){Stream = new ZmStream();}
	~ZmStreamSP(VOID){delete Stream;}

	ZmStream* operator->(VOID){return Stream;}
	operator ZmStream*(VOID){return Stream;}

private:
	ZmStream	*Stream;
};
