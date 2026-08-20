// MapTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "MapTool.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "Terrain.h"


// CMapTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMapTool, CDialog)

CMapTool::CMapTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MAPTOOL, pParent)
{

}

CMapTool::~CMapTool()
{
	
	Clear_List();

}

void CMapTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_PICTURE, m_Picture);
	DDX_Control(pDX, IDC_SLIDER1, m_Slider);
	DDX_Control(pDX, IDC_CHECK1, m_FlipCheck);
	DDX_Control(pDX, IDC_COMBO1, m_ComboBox);
	DDX_Control(pDX, IDC_CHECK2, m_BottomCenter);
}


BEGIN_MESSAGE_MAP(CMapTool, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST1, &CMapTool::OnListBox)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON4, &CMapTool::OnSaveData)
	ON_BN_CLICKED(IDC_BUTTON2, &CMapTool::OnGetTerrain)
	ON_BN_CLICKED(IDC_BUTTON3, &CMapTool::OnGetDeco)
	ON_BN_CLICKED(IDC_BUTTON8, &CMapTool::OnGetTile)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CMapTool::OnObjScaleUpdate)
	ON_BN_CLICKED(IDC_CHECK1, &CMapTool::OnFlipUpdate)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CMapTool::OnDecSelect)
	ON_BN_CLICKED(IDC_BUTTON5, &CMapTool::OnLoadData)
	ON_BN_CLICKED(IDC_CHECK2, &CMapTool::OnBottomCenter)
END_MESSAGE_MAP()


// CMapTool 메시지 처리기입니다.
BOOL CMapTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_Slider.SetRange(1, 50);
	m_Slider.SetPos(10);

	for (size_t i = 0; i < DECID_END; ++i) 
	{
		switch (i)
		{
		case FLOORDEC:
			m_ComboBox.AddString(L"FloorDec");
			break;
		case BUILDING:
			m_ComboBox.AddString(L"Building");
			break;
		case DEC:
			m_ComboBox.AddString(L"Dec");
			break;
		case WALLDEC:
			m_ComboBox.AddString(L"WallDec");
			break;
		case ROOFDEC:
			m_ComboBox.AddString(L"RoofDec");
			break;
		case EFFECTDEC:
			m_ComboBox.AddString(L"EffectDec");
			break;
		}
	}

	//m_ComboBox.AddString(L"FloorDec");
	//m_ComboBox.AddString(L"Building");
	//m_ComboBox.AddString(L"Dec");
	//m_ComboBox.AddString(L"WallDec");
	//m_ComboBox.AddString(L"RoofDec");
	//m_ComboBox.AddString(L"EffectDec");


	Set_Controls(false);
	GetDlgItem(IDC_COMBO1)->ShowWindow(false);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CMapTool::OnListBox()
{
	UpdateData(TRUE);

	CString		strFindName;

	//GetCurSel : 리스트 박스에서 선택한 목록의 인덱스를 반환

	int		iIndex = m_ListBox.GetCurSel();

	if (LB_ERR == iIndex)
		return;

	// GetText : 해당 인덱스의 문자열을 얻어오는 함수
	m_ListBox.GetText(iIndex, strFindName);

	auto		iter = m_mapPngImg.find(strFindName);

	if (iter == m_mapPngImg.end())
		return;

	m_Picture.SetBitmap(*(iter->second));

	int i = 0;

	for (; i < strFindName.GetLength(); ++i)
	{
		// isdigit : 매개 변수로 넘겨받은 단일 문자가 숫자 형태의 글자인지 문자 형태의 글자인지 판별하는 함수
		// 숫자 형태의 문자라 판별될 경우 0이 아닌 값을 반환

		if (0 != isdigit(strFindName[i]))
			break;
	}
	// Delete(index, count) : 인덱스 위치로부터 카운트만큼 문자를 삭제하는 함수

	strFindName.Delete(0, i);

	m_iDrawID = _tstoi(strFindName);

	UpdateData(FALSE);

}

void CMapTool::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	UpdateData(TRUE);
	
	TCHAR	szFilePath[MAX_PATH] = L"";
	TCHAR	szFileName[MAX_STR] = L"";
	
	// DragQueryFile : 드롭된 파일의 정보를 얻어오는 함수
	// 0xffffffff(-1)을 두 번째 매개 변수로 전달할 경우 드롭된 파일의 개수를 반환

	int	iFileCnt = DragQueryFile(hDropInfo, 0xffffffff, nullptr, 0);

	for (int i = 0; i < iFileCnt; ++i)
	{
		DragQueryFile(hDropInfo, i, szFilePath, MAX_PATH);

		CString	strRelativePath = CFileInfo::ConvertRelativePath(szFilePath);

		// PathFindFileName : 경로 중 파일 이름만 남기는 함수
		CString	strFileName = PathFindFileName(strRelativePath);

		lstrcpy(szFileName, strFileName.GetString());
		
		//PathRemoveExtension : 확장자를 잘라내는 함수
		PathRemoveExtension(szFileName);

		strFileName = szFileName;

		auto	iter = m_mapPngImg.find(strFileName);

		if (iter == m_mapPngImg.end())
		{
			CImage* pPndImg = new CImage;

			pPndImg->Load(strRelativePath);
			

			m_mapPngImg.insert({ strFileName, pPndImg });
			m_ListBox.AddString(strFileName);
		}
	}

	Horizontal_Scroll();

	CDialog::OnDropFiles(hDropInfo);

	UpdateData(FALSE);
}

void CMapTool::Horizontal_Scroll()
{
	CString	strName;
	CSize	Size;

	int	iWidth = 0;

	CDC*	pDC = m_ListBox.GetDC();

	for (int i = 0; i < m_ListBox.GetCount(); ++i)
	{
		m_ListBox.GetText(i, strName);

		// GetTextExtent : 현재 문자열의 길이를 픽셀 단위로 반환
		Size = pDC->GetTextExtent(strName);

		if (Size.cx > iWidth)
			iWidth = Size.cx;
	}

	m_ListBox.ReleaseDC(pDC);

	// GetHorizontalExtent : 리스트 박스가 가로로 스크롤 할 수 있는 최대 범위를 얻어오는 함수
	if (iWidth > m_ListBox.GetHorizontalExtent())
		m_ListBox.SetHorizontalExtent(iWidth);

}

void CMapTool::Clear_List()
{
	for_each(m_mapPngImg.begin(), m_mapPngImg.end(),
		[](auto& MyPair) {

			MyPair.second->Destroy();
			Safe_Delete(MyPair.second);
		});

	m_mapPngImg.clear();
}

void CMapTool::Set_Controls(bool _bool)
{
	int state = _bool ? SW_SHOW : SW_HIDE;

	GetDlgItem(IDC_CHECK1)->ShowWindow(state);
	GetDlgItem(IDC_SLIDER1)->ShowWindow(state);
	GetDlgItem(IDC_STATIC)->ShowWindow(state);
	GetDlgItem(IDC_SLIDER_EDIT)->ShowWindow(state);
	GetDlgItem(IDC_CHECK2)->ShowWindow(state);
}

bool CMapTool::SaveData_Dec()
{
	//CFileDialog	Dlg(FALSE, // TRUE(열기), FALSE(다른 이름으로 저장) 모드 지정
	//	L"dat", // default 확장자명
	//	L"*.dat",  // 대화 상자에 표시할 최초 파일명
	//	OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, // OFN_HIDEREADONLY(읽기 전용 체크박스 숨김), OFN_OVERWRITEPROMPT(중복 저장 시 경고 메세지 띄움)
	//	L"Data File(*.dat) | *.dat||", // "콤보 박스에 출력될 문자열 | 실제 사용할 필터링 문자열"
	//	this);	// 부모 윈도우 주소

	//TCHAR		szPath[MAX_PATH] = L"";
	//GetCurrentDirectory(MAX_PATH, szPath);
	//PathRemoveFileSpec(szPath);
	//lstrcat(szPath, L"\\Data");

	//Dlg.m_ofn.lpstrInitialDir = szPath;

	//if (IDOK == Dlg.DoModal())
	//{
	//	// GetPathName : 선택된 경로를 반환
	//	CString	str = Dlg.GetPathName().GetString();
	//	

	//}

	const TCHAR* pGetPath = L"../Data/Dec.dat";//str.GetString();

	HANDLE hFile = CreateFile(pGetPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return false;

	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CToolView* pMainView = dynamic_cast<CToolView*>(pMainFrm->m_MainSplitter.GetPane(0, 1));
	CTerrain* pTerrain = pMainView->m_pTerrain;

	vector<DECORATION*>* vecDec = pTerrain->Get_VecDeco();

	DWORD	dwByte = 0;
	DWORD	dwStrByte1 = 0;
	DWORD	dwStrByte2 = 0;

	bool bCount = false;

	for (size_t i = 0; i < DECID_END; ++i)
	{
		if (vecDec[i].empty())
		{
			continue;
		}
		else 
		{
			bCount = true;
		}

		for (auto& iter : vecDec[i]) {
			
			dwStrByte1 = sizeof(TCHAR) * (iter->wstrObjKey.length() + 1);

			WriteFile(hFile, &dwStrByte1, sizeof(DWORD), &dwByte, nullptr);
			WriteFile(hFile, iter->wstrObjKey.c_str(), dwStrByte1, &dwByte, nullptr);

			dwStrByte2 = sizeof(TCHAR) * (iter->wstrStateKey.length() + 1);

			WriteFile(hFile, &dwStrByte2, sizeof(DWORD), &dwByte, nullptr);
			WriteFile(hFile, iter->wstrStateKey.c_str(), dwStrByte2, &dwByte, nullptr);

			WriteFile(hFile, &(iter->vPos), sizeof(D3DXVECTOR3), &dwByte, nullptr);
			WriteFile(hFile, &(iter->vSize), sizeof(D3DXVECTOR3), &dwByte, nullptr);
			WriteFile(hFile, &(iter->byOption), sizeof(BYTE), &dwByte, nullptr);
			WriteFile(hFile, &(iter->byDrawID), sizeof(BYTE), &dwByte, nullptr);
			WriteFile(hFile, &(iter->fScale), sizeof(float), &dwByte, nullptr);
			WriteFile(hFile, &(iter->iFlip), sizeof(int), &dwByte, nullptr);
			WriteFile(hFile, &(iter->eDecID), sizeof(DECID), &dwByte, nullptr);
			WriteFile(hFile, &(iter->iTileIndex), sizeof(int), &dwByte, nullptr);
			WriteFile(hFile, &(iter->iCenter), sizeof(int), &dwByte, nullptr);
			//WriteFile(hFile, iter, sizeof(DECORATION), &dwByte, nullptr);
		}

	}

	CloseHandle(hFile);
	return bCount;
}

bool CMapTool::SaveData_Tile()
{
	//CFileDialog	Dlg(FALSE, // TRUE(열기), FALSE(다른 이름으로 저장) 모드 지정
	//	L"dat", // default 확장자명
	//	L"*.dat",  // 대화 상자에 표시할 최초 파일명
	//	OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, // OFN_HIDEREADONLY(읽기 전용 체크박스 숨김), OFN_OVERWRITEPROMPT(중복 저장 시 경고 메세지 띄움)
	//	L"Data File(*.dat) | *.dat||", // "콤보 박스에 출력될 문자열 | 실제 사용할 필터링 문자열"
	//	this);	// 부모 윈도우 주소

	//TCHAR		szPath[MAX_PATH] = L"";
	//GetCurrentDirectory(MAX_PATH, szPath);
	//PathRemoveFileSpec(szPath);
	//lstrcat(szPath, L"\\Data");

	//Dlg.m_ofn.lpstrInitialDir = szPath;

	//if (IDOK == Dlg.DoModal())
	//{
	//	

	//}

	// GetPathName : 선택된 경로를 반환
	//CString	str = Dlg.GetPathName().GetString();
	const TCHAR* pGetPath = L"../Data/Tile.dat";//str.GetString();

	HANDLE hFile = CreateFile(pGetPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return false;

	DWORD	dwByte = 0;

	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CToolView* pMainView = dynamic_cast<CToolView*>(pMainFrm->m_MainSplitter.GetPane(0, 1));
	CTerrain* pTerrain = pMainView->m_pTerrain;

	vector< TILE*>& vecTile = pTerrain->Get_VecTile();

	if (vecTile.empty())
		return false;

	for (auto& iter : vecTile)
		WriteFile(hFile, iter, sizeof(TILE), &dwByte, nullptr);

	CloseHandle(hFile);
	return true;
}

bool CMapTool::SaveData_Terrain()
{
	//CFileDialog	Dlg(FALSE, // TRUE(열기), FALSE(다른 이름으로 저장) 모드 지정
	//	L"dat", // default 확장자명
	//	L"*.dat",  // 대화 상자에 표시할 최초 파일명
	//	OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, // OFN_HIDEREADONLY(읽기 전용 체크박스 숨김), OFN_OVERWRITEPROMPT(중복 저장 시 경고 메세지 띄움)
	//	L"Data File(*.dat) | *.dat||", // "콤보 박스에 출력될 문자열 | 실제 사용할 필터링 문자열"
	//	this);	// 부모 윈도우 주소

	//TCHAR		szPath[MAX_PATH] = L"";
	//GetCurrentDirectory(MAX_PATH, szPath);
	//PathRemoveFileSpec(szPath);
	//lstrcat(szPath, L"\\Data");

	//Dlg.m_ofn.lpstrInitialDir = szPath;

	//if (IDOK == Dlg.DoModal())
	//{
	//	// GetPathName : 선택된 경로를 반환
	//	CString	str = Dlg.GetPathName().GetString();
	//	

	//}

	const TCHAR* pGetPath = L"../Data/Terrain.dat";//str.GetString();

	HANDLE hFile = CreateFile(pGetPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return false;

	DWORD	dwByte = 0;

	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CToolView* pMainView = dynamic_cast<CToolView*>(pMainFrm->m_MainSplitter.GetPane(0, 1));
	CTerrain* pTerrain = pMainView->m_pTerrain;

	vector<TERRAIN*>& vecTerrain = pTerrain->Get_VecTerrain();

	if (vecTerrain.empty())
		return false;

	for (auto& iter : vecTerrain)
		WriteFile(hFile, iter, sizeof(TERRAIN), &dwByte, nullptr);

	CloseHandle(hFile);
	return true;
}


void CMapTool::OnSaveData()
{
	/*switch (m_MapToolID)
	{
	case MID_TILE:
		SaveData_Tile();
		break;
	case MID_TERRAIN:
		SaveData_Terrain();
		break;
	case MID_DECORATION:
		SaveData_Dec();
		break;
	case MID_BACKGROUND:
		break;
	default:
		break;
	}*/
	
	bool TileSaveCheck = SaveData_Tile();
	bool TerrainSaveCheck = SaveData_Terrain();
	bool DecSaveCheck = SaveData_Dec();

	if (TileSaveCheck&& TerrainSaveCheck&& DecSaveCheck) {
		AfxMessageBox(L"저장 성공");
	}
	else {
		if(!TileSaveCheck)
			AfxMessageBox(L"Tile Save Failed");
		if(!TerrainSaveCheck)
			AfxMessageBox(L"Terrain Save Failed");
		if(!DecSaveCheck)
			AfxMessageBox(L"Dec Save Failed");
	}
	
}



void CMapTool::OnGetResource(MAPTOOLID _MapToolID, int _Length, CString _Path)
{
	m_MapToolID = _MapToolID;

	UpdateData(TRUE);

	Clear_List();
	m_ListBox.ResetContent();

	TCHAR	szFilePath[MAX_PATH] = L"";
	TCHAR	szFileName[MAX_STR] = L"";

	for (int i = 0; i < _Length; i++) {
		//szFilePath=
		CString	strRelativePath = _Path;
		swprintf_s(szFilePath, strRelativePath, i);
		CString	strFileName = PathFindFileName(szFilePath);
		lstrcpy(szFileName, strFileName.GetString());

		PathRemoveExtension(szFileName);

		strFileName = szFileName;

		auto	iter = m_mapPngImg.find(strFileName);

		if (iter == m_mapPngImg.end())
		{
			CImage* pPndImg = new CImage;

			pPndImg->Load(szFilePath);

			m_mapPngImg.insert({ strFileName, pPndImg });
			m_ListBox.AddString(strFileName);
		}
	}

	UpdateData(FALSE);

}

void CMapTool::OnGetTerrain()
{
	m_strObj = L"Image";
	m_strState = L"Terrain";
	OnGetResource(MID_TERRAIN, 33, TERRAIN_PATH);
	Set_Controls(true);
	GetDlgItem(IDC_COMBO1)->ShowWindow(false);
}


void CMapTool::OnGetDeco()
{
	Clear_List();
	m_ListBox.ResetContent();
	//m_MapToolID = MID_DECORATION;
	Set_Controls(true);
	GetDlgItem(IDC_COMBO1)->ShowWindow(true);
}

void CMapTool::OnGetTile()
{
	m_strObj = L"Image";
	m_strState = L"Tile";
	OnGetResource(MID_TILE, 8, TILE_PATH);
	Set_Controls(false);
	GetDlgItem(IDC_COMBO1)->ShowWindow(false);
}


void CMapTool::OnObjScaleUpdate(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	

	fScale = (float)m_Slider.GetPos()/10.f;
	//SetDlgItemInt(IDC_SLIDER_EDIT2, iScale);
	CString str;
	str.Format(_T("%0.2f"), fScale);
	str = L"x " + str ;
	SetDlgItemText(IDC_SLIDER_EDIT, str);

	*pResult = 0;
}




void CMapTool::OnFlipUpdate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	iFlip = m_FlipCheck.GetCheck() ? -1 : 1;
}


void CMapTool::OnDecSelect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_DecID = (DECID)m_ComboBox.GetCurSel();

	if (m_DecID == BUILDING) {
		m_strObj = L"Obj";
		m_strState = L"Building";
		OnGetResource(MID_DECORATION, 15, BUILDING_PATH);
	}else if (m_DecID == DEC) {
		m_strObj = L"Obj";
		m_strState = L"Dec";
		OnGetResource(MID_DECORATION, 18, DEC_PATH);
	}
	else if (m_DecID == EFFECTDEC) {
		m_strObj = L"Obj";
		m_strState = L"EffectDec";
		OnGetResource(MID_DECORATION, 17, EFFECTDEC_PATH);
	}
	else if (m_DecID == FLOORDEC) {
		m_strObj = L"Obj";
		m_strState = L"FloorDec";
		OnGetResource(MID_DECORATION, 7, FLOORDEC_PATH);
	}
	else if (m_DecID == ROOFDEC) {
		m_strObj = L"Obj";
		m_strState = L"RoofDec";
		OnGetResource(MID_DECORATION, 3, ROOFDEC_PATH);
	}
	else if (m_DecID == WALLDEC) {
		m_strObj = L"Obj";
		m_strState = L"WallDec";
		OnGetResource(MID_DECORATION, 26, WALLDEC_PATH);
	}
}


void CMapTool::OnLoadData()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CToolView* pMainView = dynamic_cast<CToolView*>(pMainFrm->m_MainSplitter.GetPane(0, 1));
	CTerrain* pTerrain = pMainView->m_pTerrain;

	pTerrain->Load_Tile(LOAD_TILE_PATH);
	pTerrain->Load_Terrain(LOAD_TERRAIN_PATH);
	pTerrain->Load_Dec(LOAD_DEC_PATH);
	
}


void CMapTool::OnBottomCenter()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	iCenter = m_BottomCenter.GetCheck() ? 1 : 0;
}
