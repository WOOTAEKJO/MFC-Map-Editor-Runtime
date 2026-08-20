#include "stdafx.h"
#include "Terrain.h"
#include "TextureMgr.h"
#include "Device.h"
#include "ToolView.h"

CTerrain::CTerrain()
{
	m_vecTile.reserve(TILEX * TILEY);
}

CTerrain::~CTerrain()
{
	Release();
}

void CTerrain::Initialize()
{

	if (FAILED(CTextureMgr::Get_Instance()->ReadImgPath(L"../Data/ImgPath.txt")))
		AfxMessageBox(L"TILE IMG FAILED");

	for (int i = 0; i < TILEY; ++i)
	{
		for (int j = 0; j < TILEX; ++j)
		{
			TILE* pTile = new TILE;

			float fX = (TILECX * j) + (i % 2) * (TILECX / 2.f);
			float fY = i * (TILECY / 2.f);

			pTile->vPos = { fX, fY , 0.f };
			pTile->vSize = { (float)TILECX, (float)TILECY, 0.f };
			pTile->byOption = 0;
			pTile->byDrawID = 0;

			m_vecTile.push_back(pTile);
		}
	}

}

void CTerrain::Update()
{

}

void CTerrain::Render()
{
	

	

	if (!m_vecTerrain.empty()) {
		Terrain_Render();
	}

	for (size_t i = 0; i < DECID_END; i++) {
		if (!m_vecDeco[i].empty()) {
			Deco_Render();
		}
	}

	if(!m_bTileHiding)
		Tile_Render();

}

void CTerrain::Mini_Render()
{
	Mini_Tile_Render();
}

void CTerrain::Release()
{
	for_each(m_vecTile.begin(), m_vecTile.end(), CDeleteObj());
	m_vecTile.clear();
	m_vecTile.shrink_to_fit();

	for (size_t i = 0; i < DECID_END; i++) {
		for_each(m_vecDeco[i].begin(), m_vecDeco[i].end(), CDeleteObj());
		/*for_each(m_vecDeco[i].begin(), m_vecDeco[i].end(), [](auto& dec) {
			
			if (dec) {
				delete dec;
				dec = nullptr;
				}
			
			});*/
		m_vecDeco[i].clear();
		m_vecDeco[i].shrink_to_fit();
	}

	for_each(m_vecTerrain.begin(), m_vecTerrain.end(), CDeleteObj());
	m_vecTerrain.clear();
	m_vecTerrain.shrink_to_fit();
}

void CTerrain::Set_Ratio(D3DXMATRIX* pOut, float fRatioX, float fRatioY)
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

void CTerrain::Tile_Change(const D3DXVECTOR3 & vPos, const BYTE & byDrawID)
{
	int		iIndex = Get_TileIdx(vPos);

	if (-1 == iIndex)
		return;

	m_vecTile[iIndex]->byDrawID = byDrawID;
	m_vecTile[iIndex]->byOption = 1;
}

int CTerrain::Get_TileIdx(const D3DXVECTOR3 & vPos)
{
	for (size_t index = 0; index < m_vecTile.size(); ++index)
	{
		if (Picking_Dot(vPos, index))
			return index;
	}

	return -1;
}

bool CTerrain::Picking(const D3DXVECTOR3 & vPos, const int & iIndex)
{
	// y = ax + b;

	float	fGradient[4]{

		(TILECY / 2.f) / (TILECX / 2.f) * -1.f,
		(TILECY / 2.f) / (TILECX / 2.f),
		(TILECY / 2.f) / (TILECX / 2.f) * -1.f,
		(TILECY / 2.f) / (TILECX / 2.f)  

	};

	// 12, 3, 6, 9
	D3DXVECTOR3		vPoint[4]{

		{ m_vecTile[iIndex]->vPos.x, m_vecTile[iIndex]->vPos.y + (TILECY / 2.f), 0.f },
		{ m_vecTile[iIndex]->vPos.x + (TILECX / 2.f), m_vecTile[iIndex]->vPos.y, 0.f },
		{ m_vecTile[iIndex]->vPos.x, m_vecTile[iIndex]->vPos.y - (TILECY / 2.f), 0.f },
		{ m_vecTile[iIndex]->vPos.x - (TILECX / 2.f), m_vecTile[iIndex]->vPos.y, 0.f }

	};

	// y = ax + b;
	// -b = ax - y
	// b = y - ax 	

	float	fB[4]{

		vPoint[0].y - fGradient[0] * vPoint[0].x,
		vPoint[1].y - fGradient[1] * vPoint[1].x,
		vPoint[2].y - fGradient[2] * vPoint[2].x,
		vPoint[3].y - fGradient[3] * vPoint[3].x

	};

	// 0 == ax + b - y		// 점이 직선 상에 놓여 있는 경우
	// 0 > ax + b - y		// 점이 직선보다 위 쪽에 놓여 있는 경우
	// 0 < ax + b - y		// 점이 직선보다 아래 쪽에 놓여 있는 경우

	bool		bCheck[4]{ false };

	// 12 -> 3
	if (0 < fGradient[0] * vPos.x + fB[0] - vPos.y)
		bCheck[0] = true;

	// 3 -> 6
	if (0 >= fGradient[1] * vPos.x + fB[1] - vPos.y)
		bCheck[1] = true;

	// 6 -> 9
	if (0 >= fGradient[2] * vPos.x + fB[2] - vPos.y)
		bCheck[2] = true;

	// 9 -> 12
	if (0 < fGradient[3] * vPos.x + fB[3] - vPos.y)
		bCheck[3] = true;
	
	return bCheck[0] && bCheck[1] && bCheck[2] && bCheck[3];
}

bool CTerrain::Picking_Dot(const D3DXVECTOR3& vPos, const int& iIndex)
{
	// 12, 3, 6, 9
	D3DXVECTOR3		vPoint[4]{

		{ m_vecTile[iIndex]->vPos.x*m_Scale, m_vecTile[iIndex]->vPos.y * m_Scale + (TILECY*m_Scale/ 2.f), 0.f },
		{ m_vecTile[iIndex]->vPos.x * m_Scale + (TILECX * m_Scale / 2.f), m_vecTile[iIndex]->vPos.y * m_Scale, 0.f },
		{ m_vecTile[iIndex]->vPos.x * m_Scale, m_vecTile[iIndex]->vPos.y * m_Scale - (TILECY * m_Scale / 2.f), 0.f },
		{ m_vecTile[iIndex]->vPos.x * m_Scale - (TILECX * m_Scale / 2.f), m_vecTile[iIndex]->vPos.y * m_Scale, 0.f }

	};

	D3DXVECTOR3		vDir[4]{

		vPoint[1] - vPoint[0],
		vPoint[2] - vPoint[1],
		vPoint[3] - vPoint[2],
		vPoint[0] - vPoint[3]
	};

	D3DXVECTOR3		vNormal[4]{

		{ -vDir[0].y, vDir[0].x, 0.f },
		{ -vDir[1].y, vDir[1].x, 0.f },
		{ -vDir[2].y, vDir[2].x, 0.f },
		{ -vDir[3].y, vDir[3].x, 0.f }
	};

	D3DXVECTOR3		vMouseDir[4]{

		vPos - vPoint[0],
		vPos - vPoint[1],
		vPos - vPoint[2],
		vPos - vPoint[3]

	};
	
	for (int i = 0; i < 4; ++i)
	{
		D3DXVec3Normalize(&vNormal[i], &vNormal[i]);
		D3DXVec3Normalize(&vMouseDir[i], &vMouseDir[i]);
	}

	for (int i = 0; i < 4; ++i)
	{
		if (0.f < D3DXVec3Dot(&vMouseDir[i], &vNormal[i]))
			return false;
	}

	return true;
}

void CTerrain::Insert_Deco(const D3DXVECTOR3& vPos, const BYTE& byDrawID, DECID decId)
{
	int		iIndex = Get_TileIdx(vPos);

	if (-1 == iIndex)
		return;

	const TEXINFO* pTexInfo=nullptr;
	CString strState = L"";
	
	DECORATION* pDeco = new DECORATION;

	switch (decId)
	{
	case BUILDING:
		pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"Obj", L"Building", byDrawID);
		strState = L"Building";
		pDeco->wstrStateKey = L"Building";
		break;
	case DEC:
		pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"Obj", L"Dec", byDrawID);
		strState = L"Dec";
		pDeco->wstrStateKey = L"Dec";
		break;
	case EFFECTDEC:
		pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"Obj", L"EffectDec", byDrawID);
		strState = L"EffectDec";
		pDeco->wstrStateKey = L"EffectDec";
		break;
	case FLOORDEC:
		pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"Obj", L"FloorDec", byDrawID);
		strState = L"FloorDec";
		pDeco->wstrStateKey = L"FloorDec";
		break;
	case ROOFDEC:
		pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"Obj", L"RoofDec", byDrawID);
		strState = L"RoofDec";
		pDeco->wstrStateKey = L"RoofDec";
		break;
	case WALLDEC:
		pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"Obj", L"WallDec", byDrawID);
		strState = L"WallDec";
		pDeco->wstrStateKey = L"WallDec";
		break;
	}
	
	

	pDeco->vPos = { m_vecTile[iIndex]->vPos.x,m_vecTile[iIndex]->vPos.y,m_vecTile[iIndex]->vPos.z };
	pDeco->vSize = { (FLOAT)pTexInfo->tImgInfo.Width,(FLOAT)pTexInfo->tImgInfo.Height,0.f };
	pDeco->byDrawID = byDrawID;
	pDeco->fScale = m_DecoScale;
	pDeco->iFlip = m_Flip;
	pDeco->wstrObjKey = L"Obj";
	pDeco->eDecID = decId;
	pDeco->iTileIndex = iIndex;
	pDeco->iCenter = m_Center;

	m_vecDeco[decId].push_back(pDeco);

	m_vecTile[iIndex]->byDrawID = 4;
}

void CTerrain::Load_Tile(const TCHAR* pFilePath)
{
	for_each(m_vecTile.begin(), m_vecTile.end(), CDeleteObj());
	m_vecTile.clear();
	m_vecTile.shrink_to_fit();

	HANDLE		hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte = 0;
	TILE* pTile = nullptr;

	while (true)
	{
		pTile = new TILE;
		
		ReadFile(hFile, pTile, sizeof(TILE), &dwByte, nullptr);


		if (0 == dwByte)
		{
			Safe_Delete(pTile);
			break;
		}


		m_vecTile.push_back(pTile);

	}

	CloseHandle(hFile);
}

void CTerrain::Load_Terrain(const TCHAR* pFilePath)
{

	for_each(m_vecTerrain.begin(), m_vecTerrain.end(), CDeleteObj());
	m_vecTerrain.clear();
	m_vecTerrain.shrink_to_fit();

	HANDLE		hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte = 0;
	TERRAIN* pTerrain = nullptr;

	while (true)
	{
	
		pTerrain = new TERRAIN;


		ReadFile(hFile, pTerrain, sizeof(TERRAIN), &dwByte, nullptr);

		if (0 == dwByte)
		{
			Safe_Delete(pTerrain);
			break;
		}

		m_vecTerrain.push_back(pTerrain);

	}

	CloseHandle(hFile);
}

void CTerrain::Load_Dec(const TCHAR* pFilePath)
{
	for (size_t i = 0; i < DECID_END; i++) {
		for_each(m_vecDeco[i].begin(), m_vecDeco[i].end(), CDeleteObj());
		m_vecDeco[i].clear();
		m_vecDeco[i].shrink_to_fit();
	}

	HANDLE		hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD	dwByte = 0;
	DWORD	dwStrByte1=0;
	DWORD	dwStrByte2=0;
	DECORATION tData{};
	DECORATION* pDec = nullptr;
	while (true)
	{

		ReadFile(hFile, &dwStrByte1, sizeof(DWORD), &dwByte, nullptr);

		TCHAR* pName1 = new TCHAR[dwStrByte1];

		ReadFile(hFile, pName1, dwStrByte1, &dwByte, nullptr);

		ReadFile(hFile, &dwStrByte2, sizeof(DWORD), &dwByte, nullptr);

		TCHAR* pName2 = new TCHAR[dwStrByte2];

		ReadFile(hFile, pName2, dwStrByte2, &dwByte, nullptr);

		ReadFile(hFile, &(tData.vPos), sizeof(D3DXVECTOR3), &dwByte, nullptr);
		ReadFile(hFile, &(tData.vSize), sizeof(D3DXVECTOR3), &dwByte, nullptr);
		ReadFile(hFile, &(tData.byOption), sizeof(BYTE), &dwByte, nullptr);
		ReadFile(hFile, &(tData.byDrawID), sizeof(BYTE), &dwByte, nullptr);
		ReadFile(hFile, &(tData.fScale), sizeof(float), &dwByte, nullptr);
		ReadFile(hFile, &(tData.iFlip), sizeof(int), &dwByte, nullptr);
		ReadFile(hFile, &(tData.eDecID), sizeof(DECID), &dwByte, nullptr);
		ReadFile(hFile, &(tData.iTileIndex), sizeof(int), &dwByte, nullptr);
		ReadFile(hFile, &(tData.iCenter), sizeof(int), &dwByte, nullptr);

		pDec = new DECORATION;

		if (0 == dwByte)
		{
			Safe_Delete(pDec);
			delete[]pName1;
			delete[]pName2;
			
			break;
		}
		//21908240 bytes
		pDec->wstrObjKey = pName1;
		delete[] pName1;
		pDec->wstrStateKey = pName2;
		delete[] pName2;

		pDec->vPos = tData.vPos;
		pDec->vSize = tData.vSize;
		pDec->byOption = tData.byOption;
		pDec->byDrawID = tData.byDrawID;
		pDec->fScale = tData.fScale;
		pDec->iFlip = tData.iFlip;
		pDec->eDecID = tData.eDecID;
		pDec->iTileIndex = tData.iTileIndex;
		pDec->iCenter = tData.iCenter;

		m_vecDeco[pDec->eDecID].push_back(pDec);

		/*pDec = new DECORATION;

		ReadFile(hFile, pDec, sizeof(DECORATION), &dwByte, nullptr);

		if (0 == dwByte)
		{
			Safe_Delete(pDec);
			break;
		}

		m_vecDeco[pDec->eDecID].push_back(pDec);*/

	}
	CloseHandle(hFile);
}

void CTerrain::Insert_Terrain(const D3DXVECTOR3& vPos, const BYTE& byDrawID)
{
	int		iIndex = Get_TileIdx(vPos);

	if (-1 == iIndex)
		return;

	const TEXINFO* pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"Image", L"Terrain", byDrawID);

	TERRAIN* pTerrain = new TERRAIN;

	pTerrain->vPos = { m_vecTile[iIndex]->vPos.x,m_vecTile[iIndex]->vPos.y,m_vecTile[iIndex]->vPos.z };
	pTerrain->vSize = { (FLOAT)pTexInfo->tImgInfo.Width,(FLOAT)pTexInfo->tImgInfo.Height,0.f };
	pTerrain->byDrawID = byDrawID;
	pTerrain->fScale = m_DecoScale;
	pTerrain->iFlip = m_Flip;
	pTerrain->iTileIndex = iIndex;
	pTerrain->iCenter = m_Center;

	m_vecTerrain.push_back(pTerrain);
	m_vecTile[iIndex]->byDrawID = 4;
}

void CTerrain::Scale_Update()
{

	for (int i = 0; i < TILEY; ++i)
	{
		for (int j = 0; j < TILEX; ++j)
		{
			float cx = TILECX * m_Scale;
			float cy = TILECY * m_Scale;

			float fX = (cx * j) + (i % 2) * (cx / 2.f);
			float fY = i * (cy / 2.f);

			m_vecTile[i * TILEX + j]->vPos = { fX, fY , 0.f };
			m_vecTile[i * TILEX + j]->vSize = { (float)cx, (float)cy, 0.f };
		}
	}
}

void CTerrain::Delete_Terrain(const D3DXVECTOR3& vPos)
{
	int		iIndex = Get_TileIdx(vPos);

	if (-1 == iIndex)
		return;

	for (int i = m_vecTerrain.size()-1; i >= 0;--i) {
		if (m_vecTerrain[i]->iTileIndex == iIndex) {
			Safe_Delete(m_vecTerrain[i]);
			m_vecTerrain.erase(m_vecTerrain.begin()+i);
			m_vecTile[iIndex]->byDrawID = 0;
			break;
		}
	}

	
}

void CTerrain::Delete_Deco(const D3DXVECTOR3& vPos)
{
	int		iIndex = Get_TileIdx(vPos);

	if (-1 == iIndex)
		return;


	for (size_t i = 0; i < DECID_END; i++) {
		for (int j = m_vecDeco[i].size() - 1; j >= 0; --j) {
			if (m_vecDeco[i][j]->iTileIndex == iIndex) {
				Safe_Delete(m_vecDeco[i][j]);
				m_vecDeco[i].erase(m_vecDeco[i].begin() + j);
				m_vecTile[iIndex]->byDrawID = 0;
				break;
			}
		}
	}
	
}

void CTerrain::Initial_Tile(const D3DXVECTOR3& vPos)
{
	int		iIndex = Get_TileIdx(vPos);

	if (-1 == iIndex)
		return;

	m_vecTile[iIndex]->byDrawID = 0;
}

void CTerrain::Tile_Render()
{
	D3DXMATRIX	matWorld, matScale, matTrans;

	TCHAR		szBuf[MIN_STR] = L"";
	int			iIndex = 0;

	for (auto& iter : m_vecTile)
	{
		const TEXINFO* pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"Image", L"Tile", iter->byDrawID);

		D3DXMatrixIdentity(&matWorld);
		D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
		D3DXMatrixTranslation(&matTrans, iter->vPos.x - m_pMainView->GetScrollPos(0),
			iter->vPos.y - m_pMainView->GetScrollPos(1),
			iter->vPos.z);
		/*D3DXMatrixIdentity(&matWorld);
		D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
		D3DXMatrixTranslation(&matTrans, iter->vPos.x + CToolView::m_vScroll.x,
			iter->vPos.y + CToolView::m_vScroll.y,
			iter->vPos.z);*/

		matWorld = matScale * matTrans;

		RECT		rc{};

		// GetClientRect : 현재 클라이언트 영역의 렉트 정보를 얻어옴
		GetClientRect(m_pMainView->m_hWnd, &rc);

		float fX = WINCX / float(rc.right - rc.left);
		float fY = WINCY / float(rc.bottom - rc.top);

		Set_Ratio(&matWorld, fX * m_Scale, fY * m_Scale);

		float		fCenterX = pTexInfo->tImgInfo.Width / 2.f;
		float		fCenterY = pTexInfo->tImgInfo.Height / 2.f;

		//Set_Ratio(&matWorld, 2.f, 2.f);

		int iAlp = 0;

		if (iter->byDrawID != 0)
			iAlp = 150;
		else
			iAlp = 255;

		CDevice::Get_Instance()->Get_Sprite()->SetTransform(&matWorld);

		CDevice::Get_Instance()->Get_Sprite()->Draw(pTexInfo->pTexture,	// 텍스처 객체 주소
			nullptr, // 출력할 이미지 영역에 대한 렉트 구조체 주소, null인경우 이미지의 0, 0기준으로 출력
			&D3DXVECTOR3(fCenterX, fCenterY, 0.f), // 출력할 이미지의 중심 축에 대한 vector3 구조체 포인터, nullptr인 경우 0, 0이 중심 좌표
			nullptr, // 위치 좌표에 따른 vector3 구조체 포인어
			D3DCOLOR_ARGB(iAlp, 255, 255, 255));	// 출력할 원본 이미지와 섞을 색상 값, 출력 시 섞은 색이 반영, 0xffffffff를 넘겨주면 원본 색상 유지

		swprintf_s(szBuf, L"%d", iIndex);

		CDevice::Get_Instance()->Get_Font()->DrawTextW(CDevice::Get_Instance()->Get_Sprite(),
			szBuf,
			lstrlen(szBuf),
			nullptr,	// 출력할 렉트의 주소
			0,			// 정렬 옵션
			D3DCOLOR_ARGB(255, 255, 255, 255));

		++iIndex;
	}
}

void CTerrain::Mini_Tile_Render()
{
	D3DXMATRIX	matWorld, matScale, matTrans;

	for (auto& iter : m_vecTile)
	{
		const TEXINFO* pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"Image", L"Tile", iter->byDrawID);

		D3DXMatrixIdentity(&matWorld);
		D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
		D3DXMatrixTranslation(&matTrans,
			iter->vPos.x,
			iter->vPos.y,
			iter->vPos.z);

		matWorld = matScale * matTrans;

		float		fCenterX = pTexInfo->tImgInfo.Width / 2.f;
		float		fCenterY = pTexInfo->tImgInfo.Height / 2.f;

		Set_Ratio(&matWorld, 0.3f, 0.3f);

		CDevice::Get_Instance()->Get_Sprite()->SetTransform(&matWorld);

		CDevice::Get_Instance()->Get_Sprite()->Draw(pTexInfo->pTexture,	// 텍스처 객체 주소
			nullptr, // 출력할 이미지 영역에 대한 렉트 구조체 주소, null인경우 이미지의 0, 0기준으로 출력
			&D3DXVECTOR3(fCenterX, fCenterY, 0.f), // 출력할 이미지의 중심 축에 대한 vector3 구조체 포인터, nullptr인 경우 0, 0이 중심 좌표
			nullptr, // 위치 좌표에 따른 vector3 구조체 포인어
			D3DCOLOR_ARGB(255, 255, 255, 255));	// 출력할 원본 이미지와 섞을 색상 값, 출력 시 섞은 색이 반영, 0xffffffff를 넘겨주면 원본 색상 유지
	}
}

void CTerrain::Deco_Render()
{
	D3DXMATRIX	matWorld, matScale, matTrans;

	for (size_t i = 0; i < DECID_END; i++) {
		for (auto& iter : m_vecDeco[i])
		{
			const TEXINFO* pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(iter->wstrObjKey.c_str(), iter->wstrStateKey.c_str(), iter->byDrawID);

			D3DXMatrixIdentity(&matWorld);
			D3DXMatrixScaling(&matScale, iter->fScale * iter->iFlip, iter->fScale, 1.f);
			D3DXMatrixTranslation(&matTrans, iter->vPos.x - m_pMainView->GetScrollPos(0),
				iter->vPos.y - m_pMainView->GetScrollPos(1),
				iter->vPos.z);
			/*D3DXMatrixIdentity(&matWorld);
			D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
			D3DXMatrixTranslation(&matTrans, iter->vPos.x + CToolView::m_vScroll.x,
				iter->vPos.y + CToolView::m_vScroll.y,
				iter->vPos.z);*/

			matWorld = matScale * matTrans;

			RECT		rc{};

			// GetClientRect : 현재 클라이언트 영역의 렉트 정보를 얻어옴
			GetClientRect(m_pMainView->m_hWnd, &rc);

			float fX = WINCX / float(rc.right - rc.left);
			float fY = WINCY / float(rc.bottom - rc.top);

			Set_Ratio(&matWorld, fX * m_Scale, fY * m_Scale);

			float		fCenterX = pTexInfo->tImgInfo.Width / 2.f;
			float		fCenterY = pTexInfo->tImgInfo.Height / 2.f * (1 + iter->iCenter);


			//Set_Ratio(&matWorld, 2.f, 2.f);

			CDevice::Get_Instance()->Get_Sprite()->SetTransform(&matWorld);

			CDevice::Get_Instance()->Get_Sprite()->Draw(pTexInfo->pTexture,	// 텍스처 객체 주소
				nullptr, // 출력할 이미지 영역에 대한 렉트 구조체 주소, null인경우 이미지의 0, 0기준으로 출력
				&D3DXVECTOR3(fCenterX, fCenterY, 0.f), // 출력할 이미지의 중심 축에 대한 vector3 구조체 포인터, nullptr인 경우 0, 0이 중심 좌표
				nullptr, // 위치 좌표에 따른 vector3 구조체 포인어
				D3DCOLOR_ARGB(255, 255, 255, 255));	// 출력할 원본 이미지와 섞을 색상 값, 출력 시 섞은 색이 반영, 0xffffffff를 넘겨주면 원본 색상 유지

			if (m_vecTile[iter->iTileIndex]->byDrawID != 4)
				m_vecTile[iter->iTileIndex]->byDrawID = 4;
		}
	}
}

void CTerrain::Mini_Deco_Render()
{
	D3DXMATRIX	matWorld, matScale, matTrans;

	for (size_t i = 0; i < DECID_END; i++) {
		for (auto& iter : m_vecDeco[i])
		{
			const TEXINFO* pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(iter->wstrObjKey.c_str(), iter->wstrStateKey.c_str(), iter->byDrawID);

			D3DXMatrixIdentity(&matWorld);
			D3DXMatrixScaling(&matScale, iter->fScale * iter->iFlip, iter->fScale, 1.f);
			D3DXMatrixTranslation(&matTrans,
				iter->vPos.x,
				iter->vPos.y,
				iter->vPos.z);

			matWorld = matScale * matTrans;

			float		fCenterX = pTexInfo->tImgInfo.Width / 2.f;
			float		fCenterY = pTexInfo->tImgInfo.Height / 2.f * (1 + iter->iCenter);

			Set_Ratio(&matWorld, 0.3f, 0.3f);

			CDevice::Get_Instance()->Get_Sprite()->SetTransform(&matWorld);

			CDevice::Get_Instance()->Get_Sprite()->Draw(pTexInfo->pTexture,	// 텍스처 객체 주소
				nullptr, // 출력할 이미지 영역에 대한 렉트 구조체 주소, null인경우 이미지의 0, 0기준으로 출력
				&D3DXVECTOR3(fCenterX, fCenterY, 0.f), // 출력할 이미지의 중심 축에 대한 vector3 구조체 포인터, nullptr인 경우 0, 0이 중심 좌표
				nullptr, // 위치 좌표에 따른 vector3 구조체 포인어
				D3DCOLOR_ARGB(255, 255, 255, 255));	// 출력할 원본 이미지와 섞을 색상 값, 출력 시 섞은 색이 반영, 0xffffffff를 넘겨주면 원본 색상 유지
		}
	}
}

void CTerrain::Terrain_Render()
{
	D3DXMATRIX	matWorld, matScale, matTrans;

	for (auto& iter : m_vecTerrain)
	{
		const TEXINFO* pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"Image", L"Terrain", iter->byDrawID);

		D3DXMatrixIdentity(&matWorld);
		D3DXMatrixScaling(&matScale, iter->fScale * iter->iFlip, iter->fScale, 1.f);
		D3DXMatrixTranslation(&matTrans, iter->vPos.x - m_pMainView->GetScrollPos(0),
			iter->vPos.y - m_pMainView->GetScrollPos(1),
			iter->vPos.z);
		/*D3DXMatrixIdentity(&matWorld);
		D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
		D3DXMatrixTranslation(&matTrans, iter->vPos.x + CToolView::m_vScroll.x,
			iter->vPos.y + CToolView::m_vScroll.y,
			iter->vPos.z);*/

		matWorld = matScale * matTrans;

		RECT		rc{};

		// GetClientRect : 현재 클라이언트 영역의 렉트 정보를 얻어옴
		GetClientRect(m_pMainView->m_hWnd, &rc);

		float fX = WINCX / float(rc.right - rc.left);
		float fY = WINCY / float(rc.bottom - rc.top);

		Set_Ratio(&matWorld, fX * m_Scale, fY * m_Scale);

		float		fCenterX = pTexInfo->tImgInfo.Width / 2.f;
		float		fCenterY = pTexInfo->tImgInfo.Height / 2.f * (1 + iter->iCenter);


		//Set_Ratio(&matWorld, 2.f, 2.f);

		CDevice::Get_Instance()->Get_Sprite()->SetTransform(&matWorld);

		CDevice::Get_Instance()->Get_Sprite()->Draw(pTexInfo->pTexture,	// 텍스처 객체 주소
			nullptr, // 출력할 이미지 영역에 대한 렉트 구조체 주소, null인경우 이미지의 0, 0기준으로 출력
			&D3DXVECTOR3(fCenterX, fCenterY, 0.f), // 출력할 이미지의 중심 축에 대한 vector3 구조체 포인터, nullptr인 경우 0, 0이 중심 좌표
			nullptr, // 위치 좌표에 따른 vector3 구조체 포인어
			D3DCOLOR_ARGB(255, 255, 255, 255));	// 출력할 원본 이미지와 섞을 색상 값, 출력 시 섞은 색이 반영, 0xffffffff를 넘겨주면 원본 색상 유지

		/*if (m_vecTile[iter->iTileIndex]->byDrawID != 4)
			m_vecTile[iter->iTileIndex]->byDrawID = 4;*/
	}
}

void CTerrain::Mini_Terrain_Render()
{
	D3DXMATRIX	matWorld, matScale, matTrans;

	for (auto& iter : m_vecTerrain)
	{
		const TEXINFO* pTexInfo = CTextureMgr::Get_Instance()->Get_Texture(L"Image", L"Terrain", iter->byDrawID);

		D3DXMatrixIdentity(&matWorld);
		D3DXMatrixScaling(&matScale, iter->fScale * iter->iFlip, iter->fScale, 1.f);
		D3DXMatrixTranslation(&matTrans, iter->vPos.x - m_pMainView->GetScrollPos(0),
			iter->vPos.y - m_pMainView->GetScrollPos(1),
			iter->vPos.z);
		/*D3DXMatrixIdentity(&matWorld);
		D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.f);
		D3DXMatrixTranslation(&matTrans, iter->vPos.x + CToolView::m_vScroll.x,
			iter->vPos.y + CToolView::m_vScroll.y,
			iter->vPos.z);*/

		matWorld = matScale * matTrans;

		Set_Ratio(&matWorld, 0.3f, 0.3f);

		float		fCenterX = pTexInfo->tImgInfo.Width / 2.f;
		float		fCenterY = pTexInfo->tImgInfo.Height / 2.f * (1 + iter->iCenter);

		CDevice::Get_Instance()->Get_Sprite()->SetTransform(&matWorld);

		CDevice::Get_Instance()->Get_Sprite()->Draw(pTexInfo->pTexture,	// 텍스처 객체 주소
			nullptr, // 출력할 이미지 영역에 대한 렉트 구조체 주소, null인경우 이미지의 0, 0기준으로 출력
			&D3DXVECTOR3(fCenterX, fCenterY, 0.f), // 출력할 이미지의 중심 축에 대한 vector3 구조체 포인터, nullptr인 경우 0, 0이 중심 좌표
			nullptr, // 위치 좌표에 따른 vector3 구조체 포인어
			D3DCOLOR_ARGB(255, 255, 255, 255));	// 출력할 원본 이미지와 섞을 색상 값, 출력 시 섞은 색이 반영, 0xffffffff를 넘겨주면 원본 색상 유지
	}
}

void CTerrain::Reset()
{
	for (size_t i = 0; i < DECID_END; i++) {
		for_each(m_vecDeco[i].begin(), m_vecDeco[i].end(), CDeleteObj());
		/*for_each(m_vecDeco[i].begin(), m_vecDeco[i].end(), [](auto& dec) {

			if (dec) {
				delete dec;
				dec = nullptr;
				}

			});*/
		m_vecDeco[i].clear();
		m_vecDeco[i].shrink_to_fit();
	}

	for_each(m_vecTerrain.begin(), m_vecTerrain.end(), CDeleteObj());
	m_vecTerrain.clear();
	m_vecTerrain.shrink_to_fit();

	for (auto& iter : m_vecTile) {
		iter->byDrawID = 0;
	}
}
