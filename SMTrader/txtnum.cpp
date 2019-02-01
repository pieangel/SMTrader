// Copyright ?2006 Shaun Harrington, All Rights Reserved.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is
// not sold for profit without the authors written consent, and
// providing that this notice and the authors name is included. If
// the source code in this file is used in any commercial application
// then a simple email would be nice.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to your
// computer or anything else for that matter.
//

// MFC apps use the following include:
#include "stdafx.h"
// Non-MFC apps use the following includes and comment out the include above:
//#include <assert.h>
//#include <winnt.h>

#include "txtnum.h"

/*static*/ LPNUMBERFMT CTextNumber::s_lpNumberFormat = NULL;

#define INT8BUFSIZE  10
#define INT16BUFSIZE 20
#define INT32BUFSIZE 20
#define INT64BUFSIZE 40

void CTextNumber::operator=(signed __int8& i8Val) {
	TCHAR numBuffer[INT8BUFSIZE+1] = {0};
	_itot_s(i8Val, numBuffer, INT8BUFSIZE, 10 );
	GetNumberFormat(LOCALE_USER_DEFAULT, 0,  numBuffer, s_lpNumberFormat, m_lpcBuffer, INT8BUFSIZE);
}

void CTextNumber::operator=(unsigned __int8& u8Val) {
	TCHAR numBuffer[INT8BUFSIZE+1] = {0};
	_itot_s(u8Val, numBuffer, INT8BUFSIZE, 10 );
	GetNumberFormat(LOCALE_USER_DEFAULT, 0,  numBuffer, s_lpNumberFormat, m_lpcBuffer, INT8BUFSIZE);
}

void CTextNumber::operator=(signed __int16& i16Val) {
	TCHAR numBuffer[INT16BUFSIZE+1] = {0};
	_itot_s(i16Val, numBuffer, INT16BUFSIZE, 10 );
	GetNumberFormat(LOCALE_USER_DEFAULT, 0,  numBuffer, s_lpNumberFormat, m_lpcBuffer, INT16BUFSIZE);
}

void CTextNumber::operator=(unsigned __int16& u16Val) {
	TCHAR numBuffer[INT16BUFSIZE+1] = {0};
	_itot_s(u16Val, numBuffer, INT16BUFSIZE, 10 );
	GetNumberFormat(LOCALE_USER_DEFAULT, 0,  numBuffer, s_lpNumberFormat, m_lpcBuffer, INT16BUFSIZE);
}

void CTextNumber::operator=(signed __int32& i32Val) {
	TCHAR numBuffer[INT32BUFSIZE+1] = {0};
	_itot_s(i32Val, numBuffer, INT32BUFSIZE, 10 );
	GetNumberFormat(LOCALE_USER_DEFAULT, 0,  numBuffer, s_lpNumberFormat, m_lpcBuffer, INT32BUFSIZE);
}

void CTextNumber::operator=(unsigned __int32& u32Val) {
	TCHAR numBuffer[INT32BUFSIZE+1] = {0};
#ifdef _UNICODE
	_ultow_s(u32Val, numBuffer, 10 );
#else
	_ultoa_s(u32Val, numBuffer, 10);
#endif
	GetNumberFormat(LOCALE_USER_DEFAULT, 0,  numBuffer, s_lpNumberFormat, m_lpcBuffer, INT32BUFSIZE);
}

void CTextNumber::operator=(signed __int64& i64Val) {
	TCHAR numBuffer[INT64BUFSIZE+1] = {0};
	_i64tot_s(i64Val, numBuffer, INT64BUFSIZE, 10 );
	GetNumberFormat(LOCALE_USER_DEFAULT, 0,  numBuffer, s_lpNumberFormat, m_lpcBuffer, INT64BUFSIZE);
}

void CTextNumber::operator=(unsigned __int64& u64Val) {
	TCHAR numBuffer[INT64BUFSIZE+1] = {0};
	_ui64tot_s(u64Val, numBuffer, INT64BUFSIZE, 10 );
	GetNumberFormat(LOCALE_USER_DEFAULT, 0,  numBuffer, s_lpNumberFormat, m_lpcBuffer, INT64BUFSIZE);
}

void CTextNumber::operator=(float& fVal) {
	TCHAR numBuffer[_CVTBUFSIZE+1] = {0};
	TCHAR bufFormatter[20] = TEXT("%f");
	if(s_lpNumberFormat) {
		_stprintf_s(bufFormatter, 20, TEXT("%%.%df"), s_lpNumberFormat->NumDigits);
	}
	_stprintf_s(numBuffer, _CVTBUFSIZE, bufFormatter, fVal);
	GetNumberFormat(LOCALE_USER_DEFAULT, 0,  numBuffer, s_lpNumberFormat, m_lpcBuffer, _CVTBUFSIZE);
}

void CTextNumber::operator=(double& dbVal) {
	TCHAR numBuffer[(_CVTBUFSIZE*2)+1] = {0};
	TCHAR bufFormatter[20] = TEXT("%.02f");
	if(s_lpNumberFormat) {
		_stprintf_s(bufFormatter, 20, TEXT("%%.%df"), s_lpNumberFormat->NumDigits);
	}
	if (dbVal != 0.00)
	{
		_stprintf_s(numBuffer, (_CVTBUFSIZE * 2), bufFormatter, dbVal);
		GetNumberFormat(LOCALE_USER_DEFAULT, 0, numBuffer, s_lpNumberFormat, m_lpcBuffer, (_CVTBUFSIZE * 2));
	}
	else
	{
		_stprintf_s(m_lpcBuffer, (_CVTBUFSIZE * 2), _T("%.2f"), dbVal);
	}
}
