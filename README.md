# C++ MFC Map Editor & Runtime

MFC 기반으로 구현한 **2D Map Editor와 WinAPI / Direct3D9 기반 Runtime 연동 프로젝트**입니다.

맵 데이터를 코드에서 직접 작성하는 대신 Editor에서 Tile과 Object를 배치하고 파일로 저장한 뒤, 별도의 Runtime에서 해당 데이터를 로드하여 사용하는 구조를 구현했습니다.

> **MFC Editor → Map Data → Binary Save → Runtime Load → Rendering**

주요 구현 요소는 다음과 같습니다.

* MFC 기반 Map Editor UI
* Direct3D9 렌더링 영역과 MFC UI 연동
* Tile Picking 기반 Map Editing
* Tile / Terrain / Decoration 배치 및 편집
* 리소스 디렉터리 탐색 및 경로 데이터 생성
* Win32 File API 기반 Binary Save / Load
* Editor에서 제작한 Tile 데이터의 Runtime 로드 및 렌더링

---

## 1. 개발 정보

| 항목           | 내용                              |
| ------------ | ------------------------------- |
| 프로젝트 형태      | 개인 학습 프로젝트                      |
| Language     | C++                             |
| Editor       | MFC                             |
| Runtime      | WinAPI                          |
| Graphics API | Direct3D9                       |
| Data         | Binary File, Text Resource Data |
| File API     | Win32 File API                  |
| IDE          | Visual Studio                   |

### 구현 범위

**Editor**

* Tile / Terrain / Decoration 편집
* Scale / Flip / 기준점 조절
* Tile Picking
* Mini View
* Resource Path 생성
* Map Data 저장 / 로드

**Runtime**

* Resource Path Data 로드
* Texture 관리
* Tile Data 로드
* Direct3D9 Tile Rendering
* View 영역 기반 렌더링 범위 계산

Runtime 연동은 **Tile 데이터까지 구현**했으며, Terrain / Decoration 데이터는 Editor의 편집 및 저장 기능까지 구현했습니다.

---

## 2. 시스템 구조

```text
┌──────────────────────────────┐
│          MFC Editor          │
│                              │
│  Resource Select             │
│  Tile Picking                │
│  Map Editing                 │
│  Preview                     │
└──────────────┬───────────────┘
               │ Save
               ▼
┌──────────────────────────────┐
│             Data             │
│                              │
│  ImgPath.txt                 │
│  Tile Data                   │
│  Terrain Data                │
│  Decoration Data             │
└──────────────┬───────────────┘
               │ Load
               ▼
┌──────────────────────────────┐
│       WinAPI / D3D9 Runtime  │
│                              │
│  Resource Load               │
│  Tile Data Load              │
│  Runtime Rendering           │
└──────────────────────────────┘
```

### Editor 구조

```text
CMainFrame
│
├─ CToolView
│   └─ Map Editing / Direct3D9 Rendering
│
├─ CMiniView
│   └─ Map Preview
│
└─ CMyForm
    ├─ CMapTool
    ├─ CPathFind
    └─ CUnitTool
```

`CSplitterWnd`를 이용해 편집 View, Mini View, Control 영역을 분리하고 MFC UI에서 설정한 편집 데이터를 Direct3D9 렌더링 화면에 반영하도록 구성했습니다.

---

## 3. 핵심 구현

### 3.1 MFC 기반 Map Editor

MFC의 `CScrollView`, `CFormView`, `CDialog`, `CSplitterWnd`를 이용하여 맵 편집 UI를 구성했습니다.

Editor에서는 다음 데이터를 구분하여 관리합니다.

```text
CTerrain
│
├─ Tile
├─ Terrain
└─ Decoration
```

주요 편집 기능은 다음과 같습니다.

* Tile Texture 변경 및 초기화
* Terrain 배치 / 삭제
* Decoration 배치 / 삭제
* Object Scale 조절
* 좌우 Flip
* 이미지 기준점 변경
* Tile 표시 / 숨김
* 선택한 Resource의 배치 위치 Preview

MFC Control에서 입력한 값을 실제 렌더링 View에 전달하여 **UI를 통한 데이터 편집 결과가 즉시 화면에 반영되도록 구현**했습니다.

---

### 3.2 Tile Picking

마우스로 직접 Tile을 선택하고 편집하기 위해 마름모 형태의 Tile 영역에 대한 Picking을 구현했습니다.

```text
Mouse Position
      ↓
Get_TileIdx()
      ↓
Picking_Dot()
      ↓
Tile Vertex / Edge 계산
      ↓
Dot Product 판정
      ↓
Selected Tile
```

Tile의 네 꼭짓점을 기준으로 각 변의 방향 및 법선 벡터를 계산하고, 마우스 위치와의 내적을 통해 Tile 내부 여부를 판정합니다.

Picking 결과와 현재 Editor Mode를 조합하여 동일한 입력으로 다음 작업을 처리합니다.

* Tile 변경
* Tile 초기화
* Terrain 배치 / 삭제
* Decoration 배치 / 삭제

마우스 Drag 입력 시에도 Picking을 반복하여 여러 Tile을 연속적으로 편집할 수 있도록 구성했습니다.

---

### 3.3 Resource Path 관리

Resource가 증가할 때 이미지 경로를 코드에 개별 등록하지 않도록 별도의 Resource Path Tool을 구현했습니다.

Resource Directory를 Drag & Drop하면 `CFileInfo`가 디렉터리를 재귀 탐색하여 다음 정보를 생성합니다.

```text
Object Key
State Key
Image Count
Relative Path
```

수집한 정보는 `ImgPath.txt`에 저장합니다.

```text
ObjKey | StateKey | Count | RelativePath
```

이 데이터를 `CTextureMgr`가 읽어 Texture Resource를 등록하도록 구성했습니다.

```text
Resource Directory
        ↓
Directory Search
        ↓
ImgPath.txt
        ↓
CTextureMgr
        ↓
Texture Resource
```

Editor와 Runtime이 동일한 형식의 `ImgPath.txt`를 사용하도록 하여 **리소스 정보를 파일 데이터로 관리하는 구조**를 구성했습니다.

---

### 3.4 Binary Save / Load와 Runtime 연동

Editor에서 편집한 맵 데이터는 Win32 File API를 이용해 Binary Data로 저장합니다.

주요 사용 API는 다음과 같습니다.

```cpp
CreateFile()
WriteFile()
ReadFile()
CloseHandle()
```

Editor에서는 다음 데이터를 각각 저장할 수 있도록 구현했습니다.

```text
Tile
Terrain
Decoration
```

이 중 Tile 데이터는 별도의 Runtime까지 연동했습니다.

```text
MFC Editor
     ↓
Tile Editing
     ↓
Binary Save
     ↓
Tile Data
     ↓
Runtime Load
     ↓
vector<TILE*>
     ↓
Direct3D9 Rendering
```

Runtime의 `CMyTerrain`은 저장된 `TILE` 데이터를 읽고 Position과 Texture ID를 이용해 화면을 구성합니다.

이를 통해 **데이터를 제작하는 Editor와 데이터를 실제 실행에 사용하는 Runtime을 분리한 기본적인 데이터 파이프라인**을 구현했습니다.

---

### 3.5 Runtime Tile Rendering

Runtime은 WinAPI 기반으로 구성하고 `CMainGame`에서 Update / Late Update / Render 흐름을 관리합니다.

```text
WinMain
   ↓
CMainGame
   ↓
CSceneMgr
   ↓
CStage
   ↓
CObjMgr
   ↓
CMyTerrain
```

Tile Rendering 시에는 전체 Tile을 항상 순회하지 않고 현재 Scroll 위치와 Window 크기를 기준으로 출력할 영역을 계산합니다.

```text
Scroll Position + Window Size
              ↓
      Visible Tile Range
              ↓
       Tile Data Lookup
              ↓
     Direct3D9 Rendering
```

현재 View에서 필요한 범위의 Tile만 조회하여 렌더링하도록 구성했습니다.

---

## 4. 주요 클래스

| Class         | 역할                                 |
| ------------- | ---------------------------------- |
| `CMainFrame`  | Editor Window 및 Split View 구성      |
| `CToolView`   | Map Editing / Direct3D9 Rendering  |
| `CMyForm`     | Editor 기능 UI 관리                    |
| `CMapTool`    | Resource 선택 및 Map 편집 옵션 관리         |
| `CTerrain`    | Tile / Terrain / Decoration 데이터 관리 |
| `CPathFind`   | Resource Path Data 생성              |
| `CFileInfo`   | Directory 탐색 및 상대 경로 처리            |
| `CTextureMgr` | Texture Resource 관리                |
| `CMiniView`   | Map 축소 Preview                     |
| `CMainGame`   | Runtime Main Loop                  |
| `CSceneMgr`   | Runtime Scene 관리                   |
| `CObjMgr`     | Runtime Object 관리                  |
| `CMyTerrain`  | Tile Data Load / Rendering         |

---

## 5. Source Code

Repository는 Editor와 Runtime을 분리하여 구성합니다.

```text
src/
├─ Tool/
│  ├─ MainFrame
│  ├─ ToolView
│  ├─ MyForm
│  ├─ MapTool
│  ├─ PathFind
│  ├─ Terrain
│  ├─ Texture
│  └─ Device
│
└─ Client/
   ├─ MainGame
   ├─ Scene
   ├─ ObjManager
   ├─ MyTerrain
   ├─ Texture
   ├─ TimeManager
   └─ Device
```

### Editor

**[`MainFrm`](src/Tool/MainFrame/MainFrm.cpp) / `CToolView`**

* Split View 구성
* Direct3D9 Rendering View
* Mouse Input과 Editor 기능 연결

**`CMapTool` / `CTerrain`**

* Tile / Terrain / Decoration 데이터 편집
* Object Scale / Flip / 기준점 관리
* Binary Data 저장 / 로드

**`CPathFind` / `CFileInfo`**

* Drag & Drop Resource 입력
* Directory 재귀 탐색
* 상대 경로 변환
* Resource Path Data 생성

### Runtime

**`CMainGame`**

* Update / Late Update / Render Main Loop

**`CSceneMgr` / `CObjMgr`**

* Scene 및 Runtime Object 관리 구조

**`CMyTerrain`**

* Tile Binary Data Load
* Scroll 처리
* View 영역 계산
* Direct3D9 Tile Rendering

---

## 6. 개선하고 싶은 부분

현재 코드는 학습 과정에서 제작한 프로토타입으로, 다시 설계한다면 다음 부분을 개선하고 싶습니다.

* Editor와 Runtime에서 사용하는 Data Format 및 파일 경로를 공통 계층으로 분리
* 구조체 메모리를 직접 기록하는 방식 대신 명시적인 Serialization Format 적용
* File Header / Version / Data Size를 이용한 데이터 검증 추가
* Raw Pointer 중심의 메모리 관리를 Smart Pointer와 RAII 기반으로 개선
* Resource Key 및 Metadata 구조 정리
* UI와 Map Data Model 간 의존성 분리
* Terrain / Decoration 데이터를 Runtime까지 동일한 Pipeline으로 확장
