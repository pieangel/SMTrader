// VtChartProperty.cpp : implementation file
//

#include "stdafx.h"
#include "VtChartProperty.h"
#include "afxdialogex.h"
#include "resource.h"


// VtChartProperty dialog

IMPLEMENT_DYNAMIC(VtChartProperty, CDialogEx)

VtChartProperty::VtChartProperty(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CHART_PROPERTY, pParent)
{

}

VtChartProperty::~VtChartProperty()
{
}

void VtChartProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROP_CHART, _PropCtrl);
}


BEGIN_MESSAGE_MAP(VtChartProperty, CDialogEx)
END_MESSAGE_MAP()


// VtChartProperty message handlers
