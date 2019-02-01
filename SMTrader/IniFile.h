#pragma once
#include <string>
class CIniWriter
{
public:
	CIniWriter(const char* szFileName); 
	void WriteInteger(const char* szSection, const char* szKey, int iValue);
	void WriteFloat(const char* szSection, const char* szKey, float fltValue);
	void WriteBoolean(const char* szSection, const char* szKey, bool bolValue);
	void WriteString(const char* szSection, const char* szKey, const char* szValue);
private:
	char m_szFileName[255];
};

class CIniReader
{
public:
	CIniReader(const char* szFileName); 
	int ReadInteger(const char* szSection, const char* szKey, int iDefaultValue);
	float ReadFloat(const char* szSection, const char* szKey, float fltDefaultValue);
	bool ReadBoolean(const char* szSection, const char* szKey, bool bolDefaultValue);
	std::string ReadString(const char* szSection, const char* szKey, const char* szDefaultValue);
private:
	char m_szFileName[255];
};
