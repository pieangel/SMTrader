
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

// The class takes a number in the form of an int and returns a string with thousands seperators.
// To use: Declare an instance, assign a value, call GetString().
class CTextNumber
{
public:
	static LPNUMBERFMT s_lpNumberFormat;

public:
	CTextNumber(){};
	void operator=(signed __int8&);
	void operator=(signed __int16&);
	void operator=(signed __int32&);
	void operator=(signed __int64&);
	void operator=(unsigned __int8&);
	void operator=(unsigned __int16&);
	void operator=(unsigned __int32&);
	void operator=(unsigned __int64&);
	void operator=(float&);
	void operator=(double&);
	
	LPCTSTR GetString(void){return m_lpcBuffer;}

private:
	TCHAR m_lpcBuffer[(_CVTBUFSIZE*2)+1];
};
