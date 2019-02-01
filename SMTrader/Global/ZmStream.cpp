#include "stdafx.h"

#include "ZmStream.h"
#include <string>
#include "Poco/NumberParser.h"
#include "Poco/NumberFormatter.h"
#include "Poco/String.h"

using Poco::NumberParser;
using Poco::NumberFormatter;
using Poco::trim;
using Poco::trimLeft;
using Poco::trimRight;
using Poco::trimRightInPlace;

ZmStream::ZmStream(VOID)
{
	_BufferPointer	= NULL;
	_Length			= 0;
}

ZmStream::~ZmStream(VOID)
{
}

void ZmStream::SetOffset(int offset)
{
	_Length = offset;
}

BOOL ZmStream::SetBuffer(BYTE *buffer)
{
	if (!buffer)
		return FALSE;

	_BufferPointer	= buffer;
	_Length			= 0;

	return TRUE;
}

BOOL ZmStream::SetBuffer()
{
	if (!_BufferPointer)
		return FALSE;

	_Length			= 0;

	return TRUE;
}

BOOL ZmStream::SetBuffer(char* buffer)
{
	if (!buffer)
		return FALSE;

	_BufferPointer = (BYTE*)buffer;
	_Length = 0;

	return TRUE;
}

void ZmStream::WriteString(char* input, int len)
{
	memcpy(_BufferPointer + _Length, input , len);
	_Length += len;
}

void ZmStream::WriteString(std::string input, int len)
{
	char buffer[StringBufferLen] = {};
	std::strcpy(buffer, input.c_str());
	memcpy(_BufferPointer + _Length, buffer, len);
	_Length += len;
}

void ZmStream::WriteStringPadRight(char* input, int len, char pad /*= ' '*/)
{
	char buffer[StringBufferLen];
	memset(buffer, pad, sizeof(buffer));
	std::strncpy(buffer, input, std::strlen(input));
	memcpy(_BufferPointer + _Length, buffer, len);
	_Length += len;
}

void ZmStream::WriteStringPadRight(std::string input, int len, char pad /*= ' '*/)
{
	char buffer[StringBufferLen];
	memset(buffer, pad, sizeof(buffer));
	std::strncpy(buffer, input.c_str(), input.length());
	memcpy(_BufferPointer + _Length, buffer, len);
	_Length += len;
}

BOOL ZmStream::ReadInt32(INT *data)
{
	memcpy(data, _BufferPointer + _Length, sizeof(INT));

	_Length += sizeof(INT);

	return TRUE;
}

int ZmStream::ReadInt(int len)
{
	int number;
	char buffer[NumberBufferLen] = { };
	memcpy(buffer, _BufferPointer + _Length, len);
	try 
	{
		std::string temp = buffer;
		if (!NumberParser::tryParse(temp, number))
		{
			number = 0;
		}
	}
	catch (std::exception const &e) 
	{
		throw(e);
	}

	_Length += len;

	return number;
}

short ZmStream::ReadShort(int len)
{
	int number;
	char buffer[NumberBufferLen] = {};
	memcpy(buffer, _BufferPointer + _Length, len);
	try
	{
		std::string temp = buffer;
		if (!NumberParser::tryParse(temp, number))
		{
			number = 0;
		}
	}
	catch (std::exception const &e)
	{
		throw(e);
	}

	_Length += len;

	return (short)number;
}

BOOL ZmStream::ReadDWORD(DWORD *data)
{
	memcpy(data, _BufferPointer + _Length, sizeof(DWORD));

	_Length += sizeof(DWORD);

	return TRUE;
}

BOOL ZmStream::ReadDWORD_PTR(DWORD_PTR *data)
{
	memcpy(data, _BufferPointer + _Length, sizeof(DWORD_PTR));

	_Length += sizeof(DWORD_PTR);

	return TRUE;
}

BOOL ZmStream::ReadByte(BYTE *data)
{
	memcpy(data, _BufferPointer + _Length, sizeof(BYTE));

	_Length += sizeof(BYTE);

	return TRUE;
}

BOOL ZmStream::ReadBytes(BYTE *data, DWORD length)
{
	memcpy(data, _BufferPointer + _Length, length);

	_Length += length;

	return TRUE;
}

BOOL ZmStream::ReadFloat(FLOAT *data)
{
	memcpy(data, _BufferPointer + _Length, sizeof(FLOAT));

	_Length += sizeof(FLOAT);

	return TRUE;
}

float ZmStream::ReadFloat(int len)
{
	double value;
	char buffer[NumberBufferLen] = {};
	memcpy(buffer, _BufferPointer + _Length, len);
	try
	{
		std::string temp = buffer;
		if (!NumberParser::tryParseFloat(temp, value))
		{
			value = 0;
		}
	}
	catch (std::exception const &e)
	{
		throw(e);
	}

	_Length += len;
	return (float)value;
}

double ZmStream::ReadDouble(int len)
{
	double value;

	char buffer[NumberBufferLen] = {};
	memcpy(buffer, _BufferPointer + _Length, len);
	try
	{
		std::string temp = buffer;
		if (!NumberParser::tryParseFloat(temp, value))
		{
			value = 0;
		}
	}
	catch (std::exception const &e)
	{
		throw(e);
	}

	_Length += len;
	return value;
}

std::string ZmStream::ReadString(int len)
{
	std::string value;
	char buffer[StringBufferLen] = {};
	memcpy(buffer, _BufferPointer + _Length, len);
	try
	{
		value = buffer;
	}
	catch (std::exception const &e)
	{
		throw(e);
	}
	_Length += len;
	return value;
}

void ZmStream::ReadString(char* des, int len)
{
	try
	{
		memset(des, 0x00, len);
		memcpy(des, _BufferPointer + _Length, len);
	}
	catch (std::exception const &e)
	{
		throw(e);
	}
	_Length += len;
}

void ZmStream::ReadStringPadRight(char* des, int len, char padding)
{
	try
	{
		memset(des, padding, len);
		memcpy(des, _BufferPointer + _Length, len);
	}
	catch (std::exception const &e)
	{
		throw(e);
	}
	_Length += len;
}

void ZmStream::WriteInt(int input, int len)
{
	std::string value = std::to_string(input);
	char buffer[StringBufferLen] = {};
	std::strcpy(buffer, value.c_str());
	memcpy(_BufferPointer + _Length, buffer, len);
	_Length += len;
}


void ZmStream::WriteIntPadRight(int input, int len, char pad /*= '0'*/)
{
	std::string value = std::to_string(input);
	char buffer[StringBufferLen];
	memset(buffer, pad, sizeof(buffer));
	std::strcpy(buffer, value.c_str());
	memcpy(_BufferPointer + _Length, buffer, len);
	_Length += len;
}

BOOL ZmStream::ReadInt64(INT64 *data)
{
	memcpy(data, _BufferPointer + _Length, sizeof(INT64));

	_Length += sizeof(INT64);

	return TRUE;
}

BOOL ZmStream::ReadSHORT(SHORT *data)
{
	memcpy(data, _BufferPointer + _Length, sizeof(SHORT));

	_Length += sizeof(SHORT);

	return TRUE;
}

BOOL ZmStream::ReadUSHORT(USHORT *data)
{
	memcpy(data, _BufferPointer + _Length, sizeof(USHORT));

	_Length += sizeof(USHORT);

	return TRUE;
}

BOOL ZmStream::ReadBOOL(BOOL *data)
{
	memcpy(data, _BufferPointer + _Length, sizeof(BOOL));

	_Length += sizeof(BOOL);

	return TRUE;
}

BOOL ZmStream::ReadWCHAR(WCHAR *data)
{
	memcpy(data, _BufferPointer + _Length, sizeof(WCHAR));

	_Length += sizeof(WCHAR);

	return TRUE;
}

BOOL ZmStream::ReadWCHARs(LPWSTR data, DWORD length)
{
	memcpy(data, _BufferPointer + _Length, length * sizeof(WCHAR));

	_Length += length * sizeof(WCHAR);

	return TRUE;
}

BOOL ZmStream::WriteInt32(INT data)
{
	memcpy(_BufferPointer + _Length, &data, sizeof(INT));

	_Length += sizeof(INT);

	return TRUE;
}

BOOL ZmStream::WriteDWORD(DWORD data)
{
	memcpy(_BufferPointer + _Length, &data, sizeof(DWORD));

	_Length += sizeof(DWORD);

	return TRUE;
}

BOOL ZmStream::WriteDWORD_PTR(DWORD_PTR data)
{
	memcpy(_BufferPointer + _Length, &data, sizeof(DWORD_PTR));

	_Length += sizeof(DWORD_PTR);

	return TRUE;
}

BOOL ZmStream::WriteByte(BYTE data)
{
	memcpy(_BufferPointer + _Length, &data, sizeof(BYTE));

	_Length += sizeof(BYTE);

	return TRUE;
}

BOOL ZmStream::WriteBytes(BYTE *data, DWORD length)
{
	memcpy(_BufferPointer + _Length, data, length);

	_Length += length;

	return TRUE;
}

BOOL ZmStream::WriteFloat(FLOAT data)
{
	memcpy(_BufferPointer + _Length, &data, sizeof(FLOAT));

	_Length += sizeof(FLOAT);

	return TRUE;
}

void ZmStream::WriteFloat(float input, int len)
{
	std::string value = std::to_string(input);
	char buffer[256] = {};
	std::strcpy(buffer, value.c_str());
	memcpy(_BufferPointer + _Length, buffer, len);
	_Length += len;
}

void ZmStream::WriteFloatPadRight(float input, int len, char pad /*= '0'*/)
{
	std::string value = std::to_string(input);
	char buffer[256];
	memset(buffer, pad, sizeof(buffer));
	std::strcpy(buffer, value.c_str());
	memcpy(_BufferPointer + _Length, buffer, len);
	_Length += len;
}

void ZmStream::WriteDouble(double input, int len)
{
	std::string value = std::to_string(input);
	char buffer[NumberBufferLen] = {};
	std::strcpy(buffer, value.c_str());
	memcpy(_BufferPointer + _Length, buffer, len);
	_Length += len;
}

void ZmStream::WriteDoublePadRight(double input, int len, char pad /*= '0'*/)
{
	std::string value = std::to_string(input);
	char buffer[NumberBufferLen];
	memset(buffer, pad, sizeof(buffer));
	std::strcpy(buffer, value.c_str());
	memcpy(_BufferPointer + _Length, buffer, len);
	_Length += len;
}

void ZmStream::WriteDoublePadLeft(double input, int decimal, int len, char pad /*= '0'*/)
{
	std::string value = NumberFormatter::format(input, len, decimal);
	std::string tvalue = trimLeft(value);
	int valueLen = std::strlen(tvalue.c_str());
	char buffer[NumberBufferLen];
	memset(buffer, pad, sizeof(buffer));
	memcpy(buffer + (len - valueLen), tvalue.c_str(), valueLen);
	memcpy(_BufferPointer + _Length, buffer, len);
	_Length += len;
}

BOOL ZmStream::WriteInt64(INT64 data)
{
	memcpy(_BufferPointer + _Length, &data, sizeof(INT64));

	_Length += sizeof(INT64);

	return TRUE;
}

BOOL ZmStream::WriteSHORT(SHORT data)
{
	memcpy(_BufferPointer + _Length, &data, sizeof(SHORT));

	_Length += sizeof(SHORT);

	return TRUE;
}

BOOL ZmStream::WriteUSHORT(USHORT data)
{
	memcpy(_BufferPointer + _Length, &data, sizeof(USHORT));

	_Length += sizeof(USHORT);

	return TRUE;
}

BOOL ZmStream::WriteBOOL(BOOL data)
{
	memcpy(_BufferPointer + _Length, &data, sizeof(BOOL));

	_Length += sizeof(BOOL);

	return TRUE;
}

BOOL ZmStream::WriteWCHAR(WCHAR data)
{
	memcpy(_BufferPointer + _Length, &data, sizeof(WCHAR));

	_Length += sizeof(WCHAR);

	return TRUE;
}

BOOL ZmStream::WriteWCHARs(LPCWSTR data, DWORD length)
{
	memcpy(_BufferPointer + _Length, data, length * sizeof(WCHAR));

	_Length += length * sizeof(WCHAR);

	return TRUE;
}

DWORD ZmStream::GetLength(VOID)
{
	return _Length;
}
