#include "stdafx.h"
#include "ZmConfigManager.h"
#include "Poco/UnicodeConverter.h"
#include "Poco/Exception.h"
#include "Poco/File.h"
#include "IniFile.h"

using Poco::SystemException;
using Poco::Path;
using Poco::File;

void ZmConfigManager::Begin()
{
	loadConfiguration();
}

void ZmConfigManager::End()
{

}

ZmConfigManager::ZmConfigManager()
{
	loadConfiguration();
}


ZmConfigManager::~ZmConfigManager()
{
	
}

void ZmConfigManager::getApplicationPath(Poco::Path& appPath) const
{
#if defined(POCO_WIN32_UTF8) && !defined(POCO_NO_WSTRING)
	wchar_t path[1024];
	int n = GetModuleFileNameW(0, path, sizeof(path) / sizeof(wchar_t));
	if (n > 0)
	{
		std::string p;
		Poco::UnicodeConverter::toUTF8(path, p);
		appPath = p;
	}
	else throw SystemException("Cannot get application file name.");
#else
	char path[1024];
	int n = GetModuleFileNameA(0, path, sizeof(path));
	if (n > 0)
		appPath = path;
	else
		throw SystemException("Cannot get application file name.");
#endif
}

int ZmConfigManager::loadConfiguration()
{
	int n = 0;
	Path appPath;
	getApplicationPath(appPath);
	Path confPath;

	if (findAppConfigFile(appPath.getBaseName(), "ini", confPath))
	{
		_filename = confPath.parent().toString() + confPath.getFileName();
	}
	return n;
}

bool ZmConfigManager::findAppConfigFile(const std::string& appName, const std::string& extension, Path& path) const
{
	poco_assert(!appName.empty());

	Path p(appName);
	p.setExtension(extension);
	bool found = findFile(p);
	if (!found)
	{
#if defined(_DEBUG)
		if (appName[appName.length() - 1] == 'd')
		{
			p.setBaseName(appName.substr(0, appName.length() - 1));
			found = findFile(p);
		}
#endif
	}
	if (found)
		path = p;
	return found;
}


bool ZmConfigManager::findFile(Poco::Path& path) const
{
	if (path.isAbsolute()) return true;

	Path appPath;
	getApplicationPath(appPath);
	Path base = appPath.parent();
	do
	{
		Path p(base, path);
		File f(p);
		if (f.exists())
		{
			path = p;
			return true;
		}
		if (base.depth() > 0) base.popDirectory();
	} while (base.depth() > 0);
	return false;
}


std::string ZmConfigManager::getString(const std::string &section, const std::string &name)
{
	CIniReader iniReader(_filename.c_str());

	return iniReader.ReadString(section.c_str(), name.c_str(), _T(""));
}

int ZmConfigManager::getInt(const std::string &section, const std::string &name)
{
	CIniReader iniReader(_filename.c_str());
	return iniReader.ReadInteger(section.c_str(), name.c_str(), 0);
}

bool ZmConfigManager::getBool(const std::string &section, const std::string &name)
{
	CIniReader iniReader(_filename.c_str());
	return iniReader.ReadBoolean(section.c_str(), name.c_str(), 0);
}

float ZmConfigManager::getFloat(const std::string &section, const std::string &name)
{
	CIniReader iniReader(_filename.c_str());
	return iniReader.ReadFloat(section.c_str(), name.c_str(), 0);
}

void ZmConfigManager::setString(const std::string &section, const std::string &name, std::string& value)
{
	CIniWriter iniWriter(_filename.c_str());
	iniWriter.WriteString(section.c_str(), name.c_str(), value.c_str());
}

void ZmConfigManager::setInteger(const std::string &section, const std::string &name, int& value)
{
	CIniWriter iniWriter(_filename.c_str());
	iniWriter.WriteInteger(section.c_str(), name.c_str(), value);
}

void ZmConfigManager::setBool(const std::string &section, const std::string &name, bool& value)
{
	CIniWriter iniWriter(_filename.c_str());
	iniWriter.WriteBoolean(section.c_str(), name.c_str(), value);
}

void ZmConfigManager::setFloat(const std::string &section, const std::string &name, float& value)
{
	CIniWriter iniWriter(_filename.c_str());
	iniWriter.WriteFloat(section.c_str(), name.c_str(), value);
}

std::string ZmConfigManager::GetAppPath()
{
	std::string curPath;
	char path[1024] = {};
	int n = GetModuleFileNameA(0, path, sizeof(path));
	if (n > 0)
		curPath = path;
	else
	{
		throw SystemException("Cannot get application file name.");
	}

	std::string::size_type found = curPath.find_last_of("/\\");
	curPath = curPath.substr(0, found);

	return curPath;
}

