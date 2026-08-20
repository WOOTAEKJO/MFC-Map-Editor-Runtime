#pragma once
#include "afxwin.h"
#include "FileInfo.h"

#include <atlimage.h>

// CMapTool 대화 상자입니다.

class CMapTool : public CDialog
{
	DECLARE_DYNAMIC(CMapTool)

public:
	CMapTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMapTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAPTOOL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnListBox();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnSaveData();

	afx_msg void OnGetResource(MAPTOOLID _MapToolID, int _Length, CString _Path);
	afx_msg void OnGetTerrain();
	afx_msg void OnGetDeco();

private:
	void	Horizontal_Scroll();
	void	Clear_List();
	void	Set_Controls(bool _bool);

	bool	SaveData_Dec();
	bool	SaveData_Tile();
	bool	SaveData_Terrain();
public:
	CListBox m_ListBox;
	CStatic m_Picture;
	CSliderCtrl m_Slider;
	CButton m_FlipCheck;

	float	fScale = 1.f;
	int		iFlip = 1.f;
	int		m_iDrawID = 0;
	int		iCenter = 0;

	CString m_strObj = L"";
	CString m_strState = L"";

	MAPTOOLID	m_MapToolID = MAPTOOLID_END;
	DECID		m_DecID = DECID_END;

	map<CString, CImage*>	m_mapPngImg;
	
	virtual BOOL OnInitDialog();
	
	
	afx_msg void OnGetTile();
	afx_msg void OnObjScaleUpdate(NMHDR* pNMHDR, LRESULT* pResult);
	

	afx_msg void OnFlipUpdate();
	afx_msg void OnDecSelect();
	CComboBox m_ComboBox;
	afx_msg void OnLoadData();
	afx_msg void OnBottomCenter();
	CButton m_BottomCenter;
};
