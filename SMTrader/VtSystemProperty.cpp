// VtSystemProperty.cpp : implementation file
//

#include "stdafx.h"
#include "VtSystemProperty.h"
#include "afxdialogex.h"
#include "resource.h"
#include "System/VtSystem.h"
#include <map>
#include <string>

// VtSystemProperty dialog

IMPLEMENT_DYNAMIC(VtSystemProperty, CDialogEx)

VtSystemProperty::VtSystemProperty(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SYSTEM_PROPERTY, pParent)
{
	_System = nullptr;
}

VtSystemProperty::~VtSystemProperty()
{
}

void VtSystemProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SYSTEM_PROP, _PropCtrl);
}


void VtSystemProperty::InitSystemProperty(VtSystem* sys)
{
	if (!sys)
		return;
	CString title;
	title = sys->Name().c_str();
	title.Append(_T(" 시스템 설정"));
	SetWindowText(title);
	CMFCPropertyGridProperty* pGroup = nullptr;
	CMFCPropertyGridProperty* pSubItem = nullptr;
	CString temp;

	std::vector<VtSystemArgGroup>& argGroup = sys->GetArgGroupVec();
	for (auto it = argGroup.begin(); it != argGroup.end(); ++it)
	{
		VtSystemArgGroup& group = *it;
		pGroup = new CMFCPropertyGridProperty(group.Name().c_str());
		std::vector<VtSystemArg>& args = group.GetArgVec();
		for (auto itn = args.begin(); itn != args.end(); ++itn)
		{
			VtSystemArg& arg = *itn;
			switch (arg.Type)
			{
			case VtParamType::INT:
				temp.Format(_T("%d"), arg.iValue);
				break;
			case VtParamType::FLOAT:
				temp.Format(_T("%.2f"), arg.dValue);
				break;
			case VtParamType::STRING:
				temp = arg.sValue.c_str();
				break;
			case VtParamType::BOOL:
				temp = arg.bValue == true ? _T("예") : _T("아니오");
				break;
			default:
				break;
			} 
			pSubItem = new CMFCPropertyGridProperty(arg.Name.c_str(), temp, arg.Desc.c_str());
			pSubItem->SetData((DWORD_PTR)&arg);
			pGroup->AddSubItem(pSubItem);
		}

		pGroup->Expand(TRUE);
		_PropCtrl.AddProperty(pGroup);
	}
}

BEGIN_MESSAGE_MAP(VtSystemProperty, CDialogEx)
END_MESSAGE_MAP()


// VtSystemProperty message handlers


BOOL VtSystemProperty::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rcClient;
	GetClientRect(rcClient);
	HDITEM hdItem;
	hdItem.mask = HDI_WIDTH; // indicating cxy is width
	hdItem.cxy = rcClient.Width() / 2; // whatever you want the property name column width to be
	hdItem.pszText = _T("속성");
	_PropCtrl.GetHeaderCtrl().SetItem(0, &hdItem);
	_PropCtrl.EnableHeaderCtrl(FALSE);
	_PropCtrl.AlwaysShowUserToolTip(TRUE);
	_PropCtrl.EnableDescriptionArea(FALSE);
	
	l_font.CreateFont(14, 0, 0, 0, FW_NORMAL,
		FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("굴림"));
	//_PropCtrl.SetFont(&l_font);
	// TODO:  Add extra initialization here
	InitSystemProperty(_System);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
