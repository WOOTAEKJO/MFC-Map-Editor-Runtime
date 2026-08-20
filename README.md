# C++ MFC Map Editor & Runtime Data Pipeline

## 1. 프로젝트 소개

이 프로젝트는 **MFC 기반 2D Map Editor와 WinAPI / Direct3D9 기반 Runtime을 분리하여 구성한 편집 도구 프로토타입**입니다.

맵 데이터를 코드에 직접 작성하는 대신 별도의 편집 Tool에서 Tile과 오브젝트를 배치하고 데이터를 저장할 수 있도록 구성했으며, 저장된 데이터와 리소스 정보를 별도의 Runtime에서 다시 사용할 수 있는 구조를 구현했습니다.

본 포트폴리오에서는 화면에 출력되는 결과보다 다음과 같은 **Windows 애플리케이션 및 데이터 처리 구조**에 초점을 맞춥니다.

* MFC 기반 편집 UI 구성
* Direct3D9 렌더링 영역과 MFC UI 연동
* 마우스 Picking 기반 Map Editing
* 파일 시스템을 이용한 리소스 경로 관리
* Win32 File API 기반 Binary Save / Load
* Editor와 별도 Runtime 간 데이터 사용 구조

Runtime 연동은 **Tile 데이터의 로드 및 렌더링 단계까지 구현**했으며, Terrain / Decoration과 일부 객체 기능은 Tool에 구현된 이후 Runtime까지 확장되지 않은 상태입니다.

---

## 2. 개발 환경

| 구분        | 내용                                          |
| --------- | ------------------------------------------- |
| Language  | C++                                         |
| Editor UI | MFC                                         |
| Runtime   | WinAPI                                      |
| Graphics  | Direct3D9 / D3DX Sprite                     |
| Data      | Binary File, Text Resource Path             |
| File API  | Win32 `CreateFile`, `ReadFile`, `WriteFile` |
| IDE       | Visual Studio                               |

---

## 3. 전체 시스템 구조

프로젝트는 **Editor / Data / Runtime**의 역할을 분리하는 형태로 구성했습니다.

```text
┌─────────────────────────────────────────┐
│              MFC Editor Tool            │
│                                         │
│  UI Input                               │
│     ↓                                   │
│  Map Editing                            │
│     ↓                                   │
│  Tile / Terrain / Decoration Data       │
│                                         │
│  Resource Directory Scan                │
└──────────────┬──────────────────────────┘
               │
               │ Save
               ▼
┌─────────────────────────────────────────┐
│                  Data                   │
│                                         │
│  ImgPath.txt                            │
│  Tile Binary Data                       │
│  Terrain Binary Data                    │
│  Decoration Binary Data                 │
└──────────────┬──────────────────────────┘
               │
               │ Load
               ▼
┌─────────────────────────────────────────┐
│       WinAPI / Direct3D9 Runtime        │
│                                         │
│  Resource Load                          │
│       ↓                                 │
│  Tile Data Load                         │
│       ↓                                 │
│  Runtime Rendering                      │
└─────────────────────────────────────────┘
```

Tool에서는 Tile, Terrain, Decoration 데이터를 편집하고 저장하며, Runtime에서는 동일한 리소스 경로 정보를 읽고 **TILE 구조의 바이너리 데이터를 로드하여 화면에 출력하는 단계까지 구현**했습니다.

---

# 4. 핵심 구현

## 4.1 MFC 기반 Map Editor UI

### 개요

맵 편집 화면과 컨트롤 UI를 하나의 Window에 구성하기 위해 MFC의 View / FormView / Dialog 구조를 사용했습니다.

### 구조

```text
CMainFrame
│
├─ CToolView
│   └─ Direct3D9 기반 Map Rendering / Editing
│
├─ CMiniView
│   └─ 전체 Map 축소 Preview
│
└─ CMyForm
    │
    ├─ CMapTool
    │   └─ Map Resource 선택 및 편집 옵션
    │
    ├─ CPathFind
    │   └─ Resource Path 관리
    │
    └─ CUnitTool
        └─ 별도 Data Editing 기능
```

`CMainFrame`에서 `CSplitterWnd`를 이용해 화면을 분할하고, 메인 편집 영역인 `CToolView`와 Mini View, Tool Control 영역을 배치했습니다.

Tool의 UI에서 변경한 Scale, Flip, Resource 정보 등의 편집 값은 `CToolView`의 렌더링 과정에 반영되도록 구성했습니다.

### 결과

MFC 컨트롤만 사용하는 형태에서 벗어나 **MFC UI와 Direct3D9 렌더링 화면을 하나의 편집 프로그램 안에서 함께 운용하는 구조**를 구현했습니다.

---

## 4.2 Tile Picking 기반 Map Editing

### 개요

사용자가 마우스로 맵을 직접 편집할 수 있도록 화면 좌표를 Tile 데이터와 연결하는 Picking 기능을 구현했습니다.

### 문제

맵에서 사용한 Tile은 일반적인 사각형 형태가 아니라 마름모 형태이기 때문에 단순한 사각형 영역 비교만으로는 정확한 Tile 선택이 어렵습니다.

### 해결

각 Tile의 네 꼭짓점을 기준으로 변의 방향 벡터와 법선 벡터를 계산하고, 마우스 위치와의 내적 결과를 이용해 Tile 내부 여부를 판정했습니다.

```text
Mouse Position
      │
      ▼
Get_TileIdx()
      │
      ▼
Picking_Dot()
      │
      ├─ Tile Vertex 계산
      ├─ Edge Direction 계산
      ├─ Normal Vector 계산
      └─ Dot Product 판정
      │
      ▼
Selected Tile
```

선택된 Tile을 기준으로 Tool Mode에 따라 서로 다른 편집 작업을 수행합니다.

* Tile Texture 변경
* Tile 초기화
* Terrain 배치 / 삭제
* Decoration 배치 / 삭제

마우스 드래그 입력을 이용해 여러 Tile을 연속적으로 수정할 수도 있도록 구성했습니다.

### 결과

화면에 렌더링된 맵 위에서 사용자가 직접 Tile과 Object를 선택하고 수정할 수 있는 **인터랙티브 Map Editing 기능**을 구현했습니다.

---

## 4.3 Map Object Editing

Editor에서는 Map 데이터를 세 종류로 분리하여 관리했습니다.

```text
CTerrain
│
├─ vector<TILE*>
│
├─ vector<TERRAIN*>
│
└─ vector<DECORATION*>[]
```

### Tile

맵의 기본 Grid 데이터를 관리합니다.

* Texture ID
* Position
* Size
* Option

### Terrain

Tile 위에 배치되는 지형 이미지를 관리합니다.

* Position
* Image ID
* Scale
* Flip
* Tile Index
* Image 기준점

### Decoration

건물이나 장식 요소를 종류별로 분류하여 관리합니다.

* Building
* Decoration
* Effect Decoration
* Floor Decoration
* Roof Decoration
* Wall Decoration

Editor에서는 UI를 통해 다음 값을 실시간으로 조절할 수 있도록 구성했습니다.

* Resource 선택
* Scale
* 좌우 Flip
* 이미지 중심 기준 변경
* Object 배치 / 삭제
* Tile 표시 여부

배치하기 전 선택된 Resource를 마우스 위치에 반투명하게 출력하여 **실제 배치 결과를 미리 확인할 수 있도록 Preview 기능**도 구현했습니다.

---

## 4.4 Resource Path 자동 생성 및 관리

### 문제

여러 상태와 이미지 시퀀스를 코드에 하나씩 등록하면 Resource가 늘어날수록 경로 관리 코드가 반복적으로 증가합니다.

### 해결

별도의 `CPathFind` Tool을 두고 Resource Directory를 Drag & Drop하면 디렉터리를 재귀 탐색하여 필요한 Resource 정보를 자동으로 수집하도록 구현했습니다.

수집하는 정보는 다음과 같습니다.

```text
Object Key
State Key
Image Count
Relative File Path
```

수집된 데이터는 `ImgPath.txt`에 다음 형식으로 저장됩니다.

```text
ObjKey | StateKey | Count | RelativePath
```

예를 들어 Directory 구조를 기반으로 Object와 State를 구분하고, 이미지 시퀀스의 개수를 계산하여 Texture Manager에서 사용할 수 있는 데이터로 변환합니다.

### 데이터 흐름

```text
Resource Directory
        │
        │ Drag & Drop
        ▼
    CPathFind
        │
        ▼
    CFileInfo
        │
        ├─ Recursive Directory Search
        ├─ Relative Path Conversion
        └─ File Count
        │
        ▼
    ImgPath.txt
```

### Tool / Runtime 공유

`ImgPath.txt`는 Tool뿐만 아니라 Runtime의 `CTextureMgr`에서도 동일한 형식으로 읽도록 구현했습니다.

```text
               ImgPath.txt
                    │
            ┌───────┴───────┐
            ▼               ▼
    Tool CTextureMgr   Runtime CTextureMgr
            │               │
            ▼               ▼
       Tool Render       Runtime Render
```

이를 통해 **Resource 정보를 코드에 직접 작성하는 방식 대신 파일 데이터를 기준으로 Texture를 등록하는 구조**를 구성했습니다.

---

## 4.5 Binary Data Save / Load

### 개요

Editor에서 제작한 Map 데이터를 다시 사용할 수 있도록 Win32 File API 기반의 바이너리 저장 / 로드 기능을 구현했습니다.

### Editor

Tool에서는 다음 데이터를 각각 파일로 저장합니다.

```text
Tile        → Tile.dat
Terrain     → Terrain.dat
Decoration  → Dec.dat
```

사용한 주요 API는 다음과 같습니다.

```cpp
CreateFile()
WriteFile()
ReadFile()
CloseHandle()
```

Tile과 Terrain은 구조체 데이터를 바이너리 형태로 저장하고, Decoration은 문자열 데이터와 구조체 멤버를 분리하여 저장하도록 구성했습니다.

### Runtime

Runtime의 `CMyTerrain`에서는 동일한 `TILE` 구조의 바이너리 데이터를 읽어 `vector<TILE*>` 형태로 복원합니다.

```text
Binary Tile Data
       │
       ▼
   ReadFile()
       │
       ▼
 vector<TILE*>
       │
       ▼
 Position / DrawID
       │
       ▼
 Direct3D9 Rendering
```

현재 소스 기준으로 Editor의 Tile 저장 경로는 `Tile.dat`, Runtime의 테스트 로드 경로는 `test4.dat`로 남아 있어 최종 파일 경로까지 통합된 상태는 아닙니다.

다만 Editor에서 저장하는 `TILE` 구조와 Runtime에서 읽는 `TILE` 구조를 동일한 방식으로 사용하여 **Editor 데이터가 별도의 실행 프로그램에서 다시 사용될 수 있는 기본적인 데이터 파이프라인**을 구성했습니다.

---

## 4.6 Runtime Tile Rendering

Runtime은 MFC와 분리된 WinAPI 프로그램으로 구성했습니다.

```text
WinMain
   │
   ▼
CMainGame
   │
   ├─ Update
   ├─ Late_Update
   └─ Render
       │
       ▼
   CSceneMgr
       │
       ▼
     CStage
       │
       ▼
    CObjMgr
       │
       ▼
  CMyTerrain
```

`CMainGame`이 전체 Update / Render 루프를 담당하고, `CSceneMgr`와 `CObjMgr`를 통해 Runtime Object를 관리하도록 구성했습니다.

### View 영역 기반 Tile Rendering

전체 맵의 모든 Tile을 항상 순회하는 대신 현재 Scroll 위치와 화면 크기를 이용하여 렌더링할 Tile 범위를 계산했습니다.

```cpp
iCullX
iCullY
iMaxX
iMaxY
```

계산된 범위에 포함된 Tile만 조회하여 Direct3D9 Sprite로 출력합니다.

```text
Scroll Position
      +
Screen Size
      │
      ▼
Visible Tile Range
      │
      ▼
TILE Data Lookup
      │
      ▼
D3D9 Sprite Rendering
```

이를 통해 Runtime 화면에서 현재 View 영역을 기준으로 Tile을 출력하도록 구성했습니다.

---

# 5. 주요 클래스

| Class         | 역할                                 |
| ------------- | ---------------------------------- |
| `CMainFrame`  | MFC Editor Window 및 Split View 구성  |
| `CToolView`   | 메인 Map 편집 및 Direct3D9 Rendering    |
| `CMyForm`     | Tool 기능 Dialog 관리                  |
| `CMapTool`    | Map Resource 선택 및 편집 옵션 관리         |
| `CTerrain`    | Tile / Terrain / Decoration 데이터 관리 |
| `CPathFind`   | Resource Directory 정보 생성           |
| `CFileInfo`   | Directory 탐색 및 상대 경로 변환            |
| `CTextureMgr` | Resource Key 기반 Texture 관리         |
| `CMiniView`   | Map 축소 Preview                     |
| `CMainGame`   | Runtime Main Loop                  |
| `CSceneMgr`   | Runtime Scene 관리                   |
| `CObjMgr`     | Runtime Object 관리                  |
| `CMyTerrain`  | Tile Data Load 및 Runtime Rendering |

---

# 6. 구현 범위

Editor와 Runtime의 구현 범위는 다음과 같습니다.

| 기능                        | Editor | Runtime |
| ------------------------- | :----: | :-----: |
| Resource Path Data        |    O   |    O    |
| Texture Loading           |    O   |    O    |
| Tile Editing              |    O   |    -    |
| Tile Data Save / Load     |    O   |    O    |
| Tile Rendering            |    O   |    O    |
| Terrain Editing / Save    |    O   |   미연동   |
| Decoration Editing / Save |    O   |   미연동   |
| Mini Map Preview          |    O   |    -    |
| Scene / Object 관리         |    -   |  기본 구조  |
| Player                    |    -   |   미완성   |

Editor에서는 Terrain 및 Decoration 편집과 저장까지 구현했지만 Runtime 연동은 Tile 데이터까지 진행했습니다.

Player와 Loading Scene 등 이후 확장을 위해 작성했던 일부 구조는 구현이 완료되지 않아 본 포트폴리오의 주요 기능에서는 제외했습니다.

---

# 7. 일반 소프트웨어 개발 관점의 의미

이 프로젝트는 2D 콘텐츠 제작을 위해 구현했지만, 다음과 같은 일반적인 Windows 소프트웨어 개발 경험으로 연결됩니다.

### Windows GUI Application

MFC의 View, FormView, Dialog, Splitter Window 및 다양한 Control을 조합하여 하나의 편집 프로그램 UI를 구성했습니다.

### UI와 Rendering 영역 연동

MFC에서 입력받은 상태를 Direct3D9 기반 View에 전달하여 편집 결과가 즉시 화면에 반영되는 구조를 경험했습니다.

### Data Serialization

메모리에서 관리하는 데이터를 Binary File로 저장하고 다시 복원하는 데이터 저장 / 로드 과정을 구현했습니다.

### File System 처리

Directory 재귀 탐색, 상대 경로 변환, Drag & Drop 입력을 이용하여 외부 Resource 정보를 프로그램 데이터로 변환했습니다.

### Tool / Runtime 역할 분리

데이터를 제작하는 Editor와 데이터를 사용하는 Runtime을 별도의 프로젝트로 구성하여 **제작 환경과 실행 환경을 분리하는 구조**를 경험했습니다.

---

# 8. 개선 가능성

현재 코드는 학습 과정에서 제작한 프로토타입이기 때문에 개선할 부분이 존재합니다.

### Editor / Runtime 공통 Data Format 정리

현재 Tool과 Runtime이 동일한 구조체 형식을 기반으로 데이터를 저장 / 로드하지만 파일 경로와 데이터 관리 코드가 각각 존재합니다.

공통 Data Library와 Serialization 계층을 분리하면 Editor와 Runtime 간 데이터 규격을 보다 명확하게 관리할 수 있습니다.

### Binary Format 안정성 개선

구조체 메모리를 직접 저장하는 방식 대신 명시적인 Header, Version, Data Size 등의 정보를 포함하는 Serialization Format을 적용할 수 있습니다.

### Resource System 개선

현재 문자열 Key와 파일 경로를 중심으로 관리하는 Resource 구조를 별도의 Resource ID 또는 Asset Metadata 구조로 개선할 수 있습니다.

### Runtime 연동 확장

현재 Runtime은 Tile 데이터까지 연동되어 있으며 Terrain / Decoration 데이터까지 동일한 파이프라인으로 확장할 수 있습니다.

### 메모리 관리 현대화

Raw Pointer와 수동 `new / delete` 중심의 자원 관리를 Smart Pointer와 RAII 기반 구조로 개선할 수 있습니다.

---

# 9. 프로젝트를 통해 경험한 내용

이 프로젝트를 통해 MFC Control 사용 자체뿐만 아니라 **Windows 기반 편집 프로그램을 구성하고 내부 데이터를 별도의 Runtime과 연결하는 전체 흐름**을 경험했습니다.

특히 UI에서 입력된 값을 화면에 반영하는 데서 끝나지 않고,

```text
사용자 입력
    ↓
Editor Data 변경
    ↓
File 저장
    ↓
Runtime Load
    ↓
실행 데이터로 활용
```

하는 구조를 구현하면서 **UI, 데이터 관리, 파일 입출력, 렌더링 영역이 서로 어떻게 연결되는지** 경험할 수 있었습니다.

현재 관점에서는 데이터 규격이나 메모리 관리 등 개선할 부분도 확인할 수 있지만, 이후 여러 프로젝트에서 편집 Tool과 데이터 기반 구조를 설계하는 기초 경험이 되었습니다.

