#pragma once
#include "Global/TemplateSingleton.h"
#include <string>
#include <map>
class VtWindowManager : public TemplateSingleton<VtWindowManager>
{
public:
	VtWindowManager();
	~VtWindowManager();

	std::map<std::string, CWnd*> WindowMap;
	void AddWindow(std::string windowName, CWnd* wnd);
	CWnd* FindWindow(std::string windowName);
	void RemoveWindow(std::string windowName);
};

