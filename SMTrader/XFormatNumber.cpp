// XFormatNumber.cpp  Version 1.0
//
// Author:  Hans Dietrich
//          hdietrich2@hotmail.com
//
// This software is released into the public domain.
// You are free to use it in any way you like.
//
// This software is provided "as is" with no expressed
// or implied warranty.  I accept no liability for any
// damage or loss of business that this software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XFormatNumber.h"

// nFracDigits = -1 : use no. of frac. digits from string
CString XFormatNumber(LPCTSTR lpszNumber, int nFracDigits)
{
	ASSERT(lpszNumber);
	ASSERT(lpszNumber[0] != 0);
	
	CString str = lpszNumber;

	NUMBERFMT nf;
	memset(&nf, 0, sizeof(nf));


	_TCHAR szBuffer[10];
	_TCHAR szDecSep[10];
	_TCHAR szThousandsSep[10];

	// set decimal separator string from locale default
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL,
		szDecSep, sizeof(szDecSep)-1);
	nf.lpDecimalSep = szDecSep;

	// set thousand separator string from locale default
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND,
		szThousandsSep, sizeof(szThousandsSep)-1);
	nf.lpThousandSep = szThousandsSep;

	// set leading zeroes from locale default
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ILZERO,
		szBuffer, sizeof(szBuffer)-1);
	nf.LeadingZero = _ttoi(szBuffer);

	// set grouping of digits from locale default
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SGROUPING,
		szBuffer, sizeof(szBuffer)-1);
	nf.Grouping = _ttoi(szBuffer);

	// set negative number mode from locale default
	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_INEGNUMBER,
		szBuffer, sizeof(szBuffer)-1);
	nf.NegativeOrder = _ttoi(szBuffer);

	int dp = nFracDigits;

	if (dp == -1)
	{
		// set number of fractional digits from input string
		CString strDecSep = szDecSep;
		dp = str.Find(szDecSep);
		dp = (dp == -1) ? 0 : str.GetLength() - dp - strDecSep.GetLength();
	}
	nf.NumDigits = dp;

	int nSize = (_tcslen(lpszNumber) * 3) + 100;
	_TCHAR *pszFormattedNumber = new _TCHAR [nSize];
	ASSERT(pszFormattedNumber);

	if (GetNumberFormat(LOCALE_USER_DEFAULT, 
						0, 
						lpszNumber, 
						&nf, 
						pszFormattedNumber, 
						nSize-1))
	{
		str = pszFormattedNumber;
	}

	if (pszFormattedNumber)
		delete [] pszFormattedNumber;
	if (str.GetAt(0) == '.') {
		CString result = _T("0");
		result.Append(str);
		return result;
	}
	return str;
}
