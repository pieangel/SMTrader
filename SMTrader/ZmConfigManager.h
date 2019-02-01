#pragma once

#include "Global/TemplateSingleton.h"
#include <string>
#include "Poco/Path.h"
#include <map>
#include "Poco/Util/IniFileConfiguration.h"
#include "Poco/AutoPtr.h"

using Poco::Path;
using Poco::Util::IniFileConfiguration;
using Poco::AutoPtr;

class ZmConfigManager : public TemplateSingleton<ZmConfigManager>
{
public:
	void Begin();
	void End();
	ZmConfigManager();
	~ZmConfigManager();
	std::string getString(const std::string &section, const std::string& name);
	int getInt(const std::string &section, const std::string& name);
	bool getBool(const std::string &section, const std::string& name);
	float getFloat(const std::string &section, const std::string& name);

	void setString(const std::string &section, const std::string &name, std::string& value);
	void setInteger(const std::string &section, const std::string &name, int& value);
	void setBool(const std::string &section, const std::string &name, bool& value);
	void setFloat(const std::string &section, const std::string &name, float& value);
	std::string GetAppPath();
private:
	void getApplicationPath(Poco::Path& path) const;
	bool findAppConfigFile(const std::string& appName, const std::string& extension, Path& path) const;
	bool findFile(Poco::Path& path) const;
	int loadConfiguration();
	std::string _filename;
};

