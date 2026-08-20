#pragma once
#include "Include.h"
#include <d3dx9.h>
#include "TextureMgr.h"
#include "Device.h"
#include "stdafx.h"


class CToolMouse 
{
public:
	CToolMouse() {};
	~CToolMouse() {};

public:
	void Update()
	{
		D3DXMATRIX	matWorld, matScale, matTrans;

		D3DXMatrixIdentity(&matWorld);
		D3DXMatrixScaling(&matScale, fMapScale*fToolScale * iFlip, fMapScale * fToolScale, 1.f);
		D3DXMatrixTranslation(&matTrans,
			::Get_Mouse().x,
			::Get_Mouse().y,
			0);
		matWorld = matScale * matTrans;

		RECT		rc{};

		GetClientRect(m_pMainView->m_hWnd, &rc);

		float fX = WINCX / float(rc.right - rc.left);
		float fY = WINCY / float(rc.bottom - rc.top);

		Set_Ratio(&matWorld, fX, fY);

		CDevice::Get_Instance()->Get_Sprite()->SetTransform(&matWorld);
	};



	void Render(CString _ObjKey, CString _StateKey, int _Index)
	{
		const TEXINFO* pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(_ObjKey, _StateKey, _Index);

		if (!pTexInfo)
			return;

		fCenterX = pTexInfo->tImgInfo.Width / 2.f;
		fCenterY = pTexInfo->tImgInfo.Height / 2.f * (1 + iCenter);

		CDevice::Get_Instance()->Get_Sprite()->Draw(pTexInfo->pTexture,	// 텍스처 객체 주소
			nullptr,
			&D3DXVECTOR3(fCenterX, fCenterY, 0.f),
			nullptr,
			D3DCOLOR_ARGB(100, 255, 255, 255));
	};

	void Set_Ratio(D3DXMATRIX* pOut, float fRatioX, float fRatioY)
	{
		pOut->_11 *= fRatioX;
		pOut->_21 *= fRatioX;
		pOut->_31 *= fRatioX;
		pOut->_41 *= fRatioX;

		pOut->_12 *= fRatioY;
		pOut->_22 *= fRatioY;
		pOut->_32 *= fRatioY;
		pOut->_42 *= fRatioY;
	}

	void		Set_MainView(CToolView* pView) { m_pMainView = pView; }
	void		Set_MapScale(float _Scale) { fMapScale = _Scale; }
	void		Set_ToolScale(float _Scale) { fToolScale = _Scale; }
	void		Set_ToolFlip(int _Flip) { iFlip = _Flip; }
	void		Set_ToolCenterBottom(int _Center) { iCenter = _Center; }
public:
	
	//D3DXVECTOR3 pos;
	float		fCenterX;
	float		fCenterY;

	float		fMapScale=1.f;
	float		fToolScale = 1.f;
	int			iFlip = 1;
	int			iCenter = 0;

	CToolView* m_pMainView = nullptr;
};
