#pragma once
#include <string>
#include <stdarg.h>
#include <iostream>
#include <sstream>

using namespace std;

#pragma warning( disable : 4996)

static std::string format_arg_list(const char *fmt, va_list args)
{
	if (!fmt) return "";
	int result = -1, length = 256;
	char *buffer = 0;
	while (result == -1)
	{
		if (buffer) delete[] buffer;
		buffer = new char[length + 1];

		CString msg;
		msg.Format(_T("buffer = %x\n"), buffer);
		//TRACE(msg);

		memset(buffer, 0, length + 1);
		result = _vsnprintf_s(buffer, length, _TRUNCATE, fmt, args);
		length *= 2;
	} 
	
	std::string s(buffer);
	delete[] buffer;
	return s;
}


static std::string format(const char *fmt, ...) 
{ 
	va_list args; 
	va_start(args, fmt); 
	std::string s = format_arg_list(fmt, args); 
	va_end(args); return s; 
}

static unsigned int GetDigit(const char* ptrFormat, int &nIdx, int nFmtLen)
{
	if ((ptrFormat == NULL) || (nIdx >= nFmtLen))
		return 0;

	ostringstream oDigit;

	while (isdigit(ptrFormat[nIdx]) > 0)
	{
		oDigit << ptrFormat[nIdx];
		nIdx++;

		if (nIdx >= nFmtLen)
			break;
	}

	return atoi(oDigit.str().c_str());
}


static string::size_type strprintf(string& rstr, const char *pFmt, ...)
{
	bool bFMT = false;
	char* psz = NULL;
	int nExpChars = 0, nIdx = 0, nFmtLen = strlen(pFmt);

	if (nFmtLen <= 0)
		return 0;

	va_list arg;
	va_start(arg, pFmt);

	// 1. vsprintf 를 사용하기 위해서 포멧 문자열 길이 계산	
	while (pFmt[nIdx])
	{
		switch (pFmt[nIdx])
		{
		case 's':	//string 
			if (bFMT)
			{
				nExpChars += strlen((char*)va_arg(arg, char*));
				bFMT = false;
			}
			break;
		case 'f':	// Decimal floating point
		case 'd':	// Signed decimal integer
		case 'i':	// Signed decimal integer
		case 'g':	// Use shorter %e or %f
		case 'G':	// Use shorter %E or %F
		case 'o':	// Signed octal
		case 'x':	// Unsigned hexadecimal integer
		case 'X':	// Unsigned hexadecimal integer (capital letters)
		case 'p':	// Prints the address of the argument in hexadecimal digits.
			nExpChars += 16;
			bFMT = false;
			break;
		case 'c': 	// char 
			nExpChars++;
			bFMT = false;
			break;

		case '.':
			nIdx++;
			nExpChars += GetDigit(pFmt, nIdx, nFmtLen);
			nIdx--;
			break;
		case '%':
			if (nIdx > 0)
			{
				bFMT = (pFmt[nIdx - 1] == '%') ? false : true;
			}
			else
			{
				bFMT = true;
			}

			nIdx++;
			nExpChars += GetDigit(pFmt, nIdx, nFmtLen);
			nIdx--;
			break;
		default:
			nExpChars++;
		}

		nIdx++;
		if (nIdx < 0 || nIdx >= nFmtLen)
		{
			break;
		}
	}

	va_end(arg);

	// 2. 버퍼 크기를 계산된 포멧 문자열의 길이만큼 할당..
	rstr.resize(nExpChars, ' ');

	va_start(arg, pFmt);

	// 3. vsprintf 사용
	int nPrnChars = vsprintf((char*)rstr.c_str(), pFmt, arg);

	if (nPrnChars >= 0 && nPrnChars <= nExpChars)
	{
		rstr.erase(nPrnChars);
	}

	va_end(arg);
	return rstr.length();
}


static std::string pad_right(std::string const& str, size_t s, char withChar )
{
	if (str.size() < s)
		return str + std::string(s - str.size(), withChar);
	else
		return str;
}

static std::string pad_left(std::string const& str, size_t s, char withChar)
{
	if (str.size() < s)
		return std::string(s - str.size(), withChar) + str;
	else
		return str;
}