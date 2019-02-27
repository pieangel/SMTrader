#pragma once
#include "Global/TemplateSingleton.h"
#include "HdWindowEvent.h"
#include <map>
#include <string>
#include "SimpleBinStream.h"
using same_endian_type = std::is_same<simple::LittleEndian, simple::LittleEndian>;

typedef std::map<CWnd*, std::pair<HdWindowType, CWnd*>> WindowMap;

class CMainFrame;
class HdWindowManager : public TemplateSingleton<HdWindowManager>
{
public:
	HdWindowManager();
	~HdWindowManager();
	WindowMap& GetWindowMap()
	{
		return _WindowMap;
	}
	void DestroyAllWindow();
	void OnWindowEvent(HdWindowEventArgs& arg);
	void Save(simple::file_ostream<same_endian_type>& ss);
	void Load(simple::file_istream<same_endian_type>& ss);
	void Save(simple::file_ostream<same_endian_type>& ss, HdWindowType wndType, CWnd* wnd);
	void Load(simple::file_istream<same_endian_type>& ss, HdWindowType wndType);
	CMainFrame* MainFrm() const { return _MainFrm; }
	void MainFrm(CMainFrame* val) { _MainFrm = val; }
private:
	std::map<CWnd*, std::pair<HdWindowType, CWnd*>> _WindowMap;
	void AddWindow(HdWindowType wndType, CWnd* wnd);
	void RemoveWindow(CWnd* wnd);
	void RestoreDialog(HdWindowType type, CRect rcWnd, std::string info);
	CMainFrame* _MainFrm = nullptr;
};

