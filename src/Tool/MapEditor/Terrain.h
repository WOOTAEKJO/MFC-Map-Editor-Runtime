#pragma once

#include "Include.h"

class CToolView;
class CTerrain
{
public:
	CTerrain();
	~CTerrain();

public:
	void		Initialize();
	void		Update();
	void		Render();
	void		Mini_Render();
	void		Release();

public:
	vector<TILE*>&			Get_VecTile() { return m_vecTile; }
	vector<DECORATION*>*	Get_VecDeco() { return m_vecDeco; }
	vector<TERRAIN*>& Get_VecTerrain() { return m_vecTerrain; }

public:
	void		Set_MainView(CToolView* pView) { m_pMainView = pView;  }
	void		Set_Ratio(D3DXMATRIX* pOut, float fRatioX, float fRatioY);

public:
	void		Tile_Change(const D3DXVECTOR3& vPos, const BYTE& byDrawID);

	int			Get_TileIdx(const D3DXVECTOR3& vPos);
	bool		Picking(const D3DXVECTOR3& vPos, const int& iIndex);
	bool		Picking_Dot(const D3DXVECTOR3& vPos, const int& iIndex);

	void		Load_Tile(const TCHAR* pFilePath);
	void		Load_Terrain(const TCHAR* pFilePath);
	void		Load_Dec(const TCHAR* pFilePath);

	void		Insert_Deco(const D3DXVECTOR3& vPos, const BYTE& byDrawID, DECID decId);
	void		Insert_Terrain(const D3DXVECTOR3& vPos, const BYTE& byDrawID);

	void		Set_Scale(float _Scale) { m_Scale = _Scale; };
	void		Set_DecoScale(float _DScale) { m_DecoScale = _DScale; }
	void		Set_Flip(int _Flip) { m_Flip = _Flip; }
	void		Set_CenterBottom(int _Center) { m_Center = _Center; }
	void		Set_TileHiding(bool	_TileHiding) { m_bTileHiding = _TileHiding; }

	void		Scale_Update();

	void		Delete_Terrain(const D3DXVECTOR3& vPos);
	void		Delete_Deco(const D3DXVECTOR3& vPos);
	void		Initial_Tile(const D3DXVECTOR3& vPos);
public:
	void		Tile_Render();
	void		Mini_Tile_Render();

	void		Deco_Render();
	void		Mini_Deco_Render();

	void		Terrain_Render();
	void		Mini_Terrain_Render();

	void		Reset();

private:
	vector<TILE*>			m_vecTile;
	vector<DECORATION*>		m_vecDeco[DECID_END];
	vector<TERRAIN*>		m_vecTerrain;

	CToolView*			m_pMainView = nullptr;

	float	m_Scale=1.f;
	float	m_DecoScale = 1.f;
	int		m_Flip = 1;
	int		m_Center = 0;

	bool	m_bTileHiding = true;
};


