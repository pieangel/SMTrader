// VtSystemSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SMTrader.h"
#include "VtSystemSetDlg.h"
#include "afxdialogex.h"
#include "System/VtSystem.h"


// VtSystemSetDlg dialog

IMPLEMENT_DYNAMIC(VtSystemSetDlg, CDialogEx)

VtSystemSetDlg::VtSystemSetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SYS_PROP, pParent)
{

}

VtSystemSetDlg::~VtSystemSetDlg()
{
	if (CMFCVisualManager::GetInstance() != nullptr)
	{
		delete CMFCVisualManager::GetInstance();
	}
}

void VtSystemSetDlg::InitSystemProperty(VtSystem* sys)
{

	if (!sys)
		return;
	std::string systemTitle = _T("시스템 설정 - ");
	systemTitle.append(sys->Name());
	SetWindowText(systemTitle.c_str());

	CMFCPropertyGridProperty* pGroup = nullptr;
	CMFCPropertyGridProperty* pSubItem = nullptr;
	
	std::vector<VtSystemArgGroup>& argGroup = sys->GetArgGroupVec();
	CString temp;
	for (auto it = argGroup.begin(); it != argGroup.end(); ++it) {
		VtSystemArgGroup& saG = *it;
		pGroup = new CMFCPropertyGridProperty(saG.Name().c_str());
		pGroup->SetData((DWORD_PTR)sys);

		std::vector<VtSystemArg>& argMap = saG.GetArgVec();
		for (auto itn = argMap.begin(); itn != argMap.end(); ++itn) {
			VtSystemArg& arg = *itn;
			switch (arg.Type) {
			case VtParamType::STRING: {
				pSubItem = new CMFCPropertyGridProperty(arg.Name.c_str(), arg.sValue.c_str(), arg.Desc.c_str());
				pSubItem->SetData((DWORD_PTR)&arg);
				pGroup->AddSubItem(pSubItem);
			}
				break;
			case VtParamType::INT: {
				temp.Format(_T("%d"), arg.iValue);
				pSubItem = new CMFCPropertyGridProperty(arg.Name.c_str(), temp, arg.Desc.c_str());
				pSubItem->SetData((DWORD_PTR)&arg);
				pGroup->AddSubItem(pSubItem);
			}
				break;
			case VtParamType::FLOAT: {
				temp.Format(_T("%.0f"), arg.dValue);
				pSubItem = new CMFCPropertyGridProperty(arg.Name.c_str(), temp, arg.Desc.c_str());
				pSubItem->SetData((DWORD_PTR)&arg);
				pGroup->AddSubItem(pSubItem);
			}
				break;
			case VtParamType::BOOL: {
				if (arg.bValue)
					temp = _T("True");
				else
					temp = _T("False");
				pSubItem = new CMFCPropertyGridProperty(arg.Name.c_str(), temp, arg.Desc.c_str());
				pSubItem->SetData((DWORD_PTR)&arg);
				pGroup->AddSubItem(pSubItem);
			}
				break;
			default:
				break;
			}
			pGroup->Expand(TRUE);
		}
		_PropCtrl.AddProperty(pGroup);
	}
}

void VtSystemSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SYS_PROP, _PropCtrl);
}


BEGIN_MESSAGE_MAP(VtSystemSetDlg, CDialogEx)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, &VtSystemSetDlg::OnAfxWmPropertyChanged)
END_MESSAGE_MAP()


// VtSystemSetDlg message handlers


afx_msg LRESULT VtSystemSetDlg::OnAfxWmPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	CMFCPropertyGridProperty* prop = (CMFCPropertyGridProperty*)lParam;
	VtSystemArg* arg = (VtSystemArg*)prop->GetData();
	CString val;
	if (arg) {
		val = prop->GetValue();
		switch (arg->Type) {
		case VtParamType::STRING: {
			arg->sValue = (LPCTSTR)val;
		}
		break;
		case VtParamType::INT: {
			arg->iValue = _ttoi(val);
		}
		break;
		case VtParamType::FLOAT: {
			arg->dValue = _ttof(val);
		}
		break;
		case VtParamType::BOOL: {
			if (val.Compare(_T("True")) == 0)
				arg->bValue = true;
			else
				arg->bValue = false;
		}
		break;
		default:
			break;
		}
	}
	return 0;
}
