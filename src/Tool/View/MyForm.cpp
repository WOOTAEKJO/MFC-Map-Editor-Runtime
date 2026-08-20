// MyForm.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "MyForm.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "Terrain.h"

// CMyForm

IMPLEMENT_DYNCREATE(CMyForm, CFormView)

CMyForm::CMyForm()
	: CFormView(IDD_MYFORM)
{

}

CMyForm::~CMyForm()
{
}

void CMyForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_Slider);
	DDX_Control(pDX, IDC_CHECK1, m_TileHiding);
}

BEGIN_MESSAGE_MAP(CMyForm, CFormView)
	ON_BN_CLICKED(IDC_BUTTON1, &CMyForm::OnUnitTool)
	ON_BN_CLICKED(IDC_BUTTON4, &CMyForm::OnMapTool)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CMyForm::OnScaleUpdate)
	ON_BN_CLICKED(IDC_BUTTON9, &CMyForm::OnPathFind)
	ON_BN_CLICKED(IDC_BUTTON2, &CMyForm::OnReset)
	ON_BN_CLICKED(IDC_CHECK1, &CMyForm::OnTileHidng)
END_MESSAGE_MAP()


// CMyForm 진단입니다.

#ifdef _DEBUG
void CMyForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMyForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMyForm 메시지 처리기입니다.


void CMyForm::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// CreatePointFont(폰트 크기, 글씨체)
	m_Font.CreatePointFont(180, L"궁서");

	GetDlgItem(IDC_BUTTON1)->SetFont(&m_Font);
	GetDlgItem(IDC_BUTTON4)->SetFont(&m_Font);
	GetDlgItem(IDC_BUTTON9)->SetFont(&m_Font);
	m_Slider.SetRange(1, 10);
	m_Slider.SetPos(4);
}


void CMyForm::OnUnitTool()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// GetSafeHwnd : 현재 다이얼로그의 윈도우 핸들을 반환, 핸들이 null이면 아직 생성되어 있지 않음을 의미

	if(nullptr == m_UnitTool.GetSafeHwnd())
		m_UnitTool.Create(IDD_UNITTOOL);

	m_UnitTool.ShowWindow(SW_SHOW);

}


void CMyForm::OnMapTool()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (nullptr == m_MapTool.GetSafeHwnd())
		m_MapTool.Create(IDD_MAPTOOL);

	m_MapTool.ShowWindow(SW_SHOW);
}


void CMyForm::OnScaleUpdate(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int Position = 0;

	fScale = (float)m_Slider.GetPos();
	//SetDlgItemInt(IDC_SLIDER_EDIT2, iScale);
	CString str;
	str.Format(_T("%0.2f") , fScale);
	SetDlgItemText(IDC_SLIDER_EDIT2, str);
	*pResult = 0;
}


void CMyForm::OnPathFind()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (nullptr == m_PathFind.GetSafeHwnd())
		m_PathFind.Create(IDD_PathFind);

	m_PathFind.ShowWindow(SW_SHOW);
}


void CMyForm::OnReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CToolView* pMainView = dynamic_cast<CToolView*>(pMainFrm->m_MainSplitter.GetPane(0, 1));
	CTerrain* pTerrain = pMainView->m_pTerrain;

	pTerrain->Reset();

}


void CMyForm::OnTileHidng()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	m_bTileHiding =m_TileHiding.GetCheck();

}
