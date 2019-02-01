#include "stdafx.h"
#include "IniFile.h"
#include <iostream>
#include <Windows.h> 
CIniWriter::CIniWriter(const char* szFileName)
{
	memset(m_szFileName, 0x00, 255);
	memcpy(m_szFileName, szFileName, strlen(szFileName));
}
void CIniWriter::WriteInteger(const char* szSection, const char* szKey, int iValue)
{
	char szValue[255];
	sprintf_s(szValue, "%d", iValue);
	WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName); 
}
void CIniWriter::WriteFloat(const char* szSection, const char* szKey, float fltValue)
{
	char szValue[255];
	sprintf_s(szValue, "%f", fltValue);
	WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName); 
}
void CIniWriter::WriteBoolean(const char* szSection, const char* szKey, bool bolValue)
{
	char szValue[255];
	sprintf_s(szValue, "%s", bolValue ? "True" : "False");
	WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName); 
}
void CIniWriter::WriteString(const char* szSection, const char* szKey, const char* szValue)
{
	WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName);
}

CIniReader::CIniReader(const char* szFileName)
{
	memset(m_szFileName, 0x00, 255);
	memcpy(m_szFileName, szFileName, strlen(szFileName));
}
int CIniReader::ReadInteger(const char* szSection, const char* szKey, int iDefaultValue)
{
	int iResult = GetPrivateProfileInt(szSection,  szKey, iDefaultValue, m_szFileName); 
	return iResult;
}
float CIniReader::ReadFloat(const char* szSection, const char* szKey, float fltDefaultValue)
{
	char szResult[255];
	char szDefault[255];
	float fltResult;
	sprintf_s(szDefault, "%f",fltDefaultValue);
	GetPrivateProfileString(szSection,  szKey, szDefault, szResult, 255, m_szFileName); 
	fltResult =  static_cast<float>(atof(szResult));
	return fltResult;
}
bool CIniReader::ReadBoolean(const char* szSection, const char* szKey, bool bolDefaultValue)
{
	char szResult[255];
	char szDefault[255];
	bool bolResult;
	sprintf_s(szDefault, "%s", bolDefaultValue? "True" : "False");
	GetPrivateProfileString(szSection, szKey, szDefault, szResult, 255, m_szFileName); 
	bolResult =  (strcmp(szResult, "True") == 0 || 
		strcmp(szResult, "true") == 0) ? true : false;
	return bolResult;
}
std::string CIniReader::ReadString(const char* szSection, const char* szKey, const char* szDefaultValue)
{
	char szResult[255];
	memset(szResult, 0x00, 255);
	GetPrivateProfileString(szSection,  szKey, 
		szDefaultValue, szResult, 255, m_szFileName); 

	std::string result(szResult);
	return result;
}