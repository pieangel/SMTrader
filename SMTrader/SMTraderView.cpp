
// SMTraderView.cpp : CSMTraderView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "SMTrader.h"
#endif

#include "SMTraderDoc.h"
#include "SMTraderView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSMTraderView

IMPLEMENT_DYNCREATE(CSMTraderView, CView)

BEGIN_MESSAGE_MAP(CSMTraderView, CView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CSMTraderView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CSMTraderView ����/�Ҹ�

CSMTraderView::CSMTraderView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CSMTraderView::~CSMTraderView()
{
}

BOOL CSMTraderView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CSMTraderView �׸���

void CSMTraderView::OnDraw(CDC* /*pDC*/)
{
	CSMTraderDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CSMTraderView �μ�


void CSMTraderView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CSMTraderView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CSMTraderView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CSMTraderView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}

void CSMTraderView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CSMTraderView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CSMTraderView ����

#ifdef _DEBUG
void CSMTraderView::AssertValid() const
{
	CView::AssertValid();
}

void CSMTraderView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSMTraderDoc* CSMTraderView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSMTraderDoc)));
	return (CSMTraderDoc*)m_pDocument;
}
#endif //_DEBUG


// CSMTraderView �޽��� ó����
