#include "stdafx.h"
#include "FileInfo.h"


CFileInfo::CFileInfo()
{
}


CFileInfo::~CFileInfo()
{
}

CString CFileInfo::ConvertRelativePath(CString strFullPath)
{
	TCHAR	szRelativePath[MAX_PATH] = L"";	// 상대 경로를 저장할 배열
	TCHAR	szCurDirPath[MAX_PATH] = L""; // 현재의 절대 경로를 저장할 배열

	GetCurrentDirectory(MAX_PATH, szCurDirPath);

	//szCurDirPath에서 strFullPath로 가는 상대 경로를 구해서 szRelativePath에 저장, 단, 같은 드라이브 내에서만 가능

	PathRelativePathTo(szRelativePath, 
		szCurDirPath, 
		FILE_ATTRIBUTE_DIRECTORY, 
		strFullPath.GetString(), 
		FILE_ATTRIBUTE_DIRECTORY);
	
	return CString(szRelativePath);
}

void CFileInfo::DirInfoExtration(const wstring& wstrPath, list<IMGPATH*>& rPathList)
{
	CFileFind	Find;
	// 파일에 경로 및 제어 관련 클래스

	wstring		wstrFilePath = wstrPath + L"\\*.*";
	// FindFile : 주어진 경로에 파일이 있는지 파일의 유무를 판단하는 함수, 존재하지 않으면 FALSE 존재하면 TRUE를 리턴
	// 만약 파일이 있다는 것을 알게 되었고 그 파일에 접근하고 싶을 때는 반드시 FindNextFile 함수를 호출해야만 한다.

	BOOL		bContinue = Find.FindFile(wstrFilePath.c_str());

	while (bContinue) {

		bContinue = Find.FindNextFile();
		// FindNextFile : 동일 경로 안에서 다음 파일이 있는지 유무 판단, 더이상 찾을게 없거나 마지막 파일인 경우 자동 0 리턴

		if (Find.IsDots())
			continue;
		//파일의 이름이 점인 경우 다음 파일로

		else if (Find.IsDirectory()) {

			// GetFilePath : 현재 찾은 경로를 얻어오는 함수
			DirInfoExtration(wstring(Find.GetFilePath()), rPathList);
			//다음 경로에 파일이 있을 경우 재귀호출
		}
		else {


			if (Find.IsSystem())
				IContinue;
			//눈에 보이지 않는 시스템 파일인 경우 다음 파일로

			IMGPATH* pImgPath = new IMGPATH;
			TCHAR			szPathBuf[MAX_STR] = L"";

			lstrcpy(szPathBuf, Find.GetFilePath().GetString());
			PathRemoveFileSpec(szPathBuf);

			// L"D:\\유준환\\139기\\Frame139\\Texture\\Stage\\Effect\\BossMultiAttack"
			pImgPath->iCount = DirFileCount(szPathBuf);

			// GetFileTitle : 파일 이름만 얻어오는 함수
			wstring		wstrTextureName = Find.GetFileTitle().GetString();
			// wstrTextureName = L"BossMultiAttack0"

			// substr(시작, 끝) : 시작에서 끝까지 해당하는 문자열을 잘라냄
			wstrTextureName = wstrTextureName.substr(0, wstrTextureName.size() - 1) + L"%d.png";
			// wstrTextureName = L"BossMultiAttack%d.png"

			TCHAR	szBuf[MAX_PATH] = L"";
			lstrcpy(szBuf, Find.GetFilePath().GetString());
			PathRemoveFileSpec(szBuf);
			// L"D:\\유준환\\139기\\Frame139\\Texture\\Stage\\Effect\\BossMultiAttack"

			// PathCombine(1, 2, 3) : 2에 3을 이어붙여 1에 저장, 2와 3사이에 \\가 자동으로 삽입된채 1에 저장
			PathCombine(szBuf, szBuf, wstrTextureName.c_str());
			// L"D:\\유준환\\139기\\Frame139\\Texture\\Stage\\Effect\\BossMultiAttack\\BossMultiAttack%d.png"
			pImgPath->wstrPath = ConvertRelativePath(szBuf); //상대 경로로 바꿔줌
			// L"..\\Texture\\Stage\\Effect\\BossMultiAttack\\BossMultiAttack%d.png"

			PathRemoveFileSpec(szBuf);
			//	PathFindFileName : 파일명을 찾거나 가장 마지막 폴더명을 찾는 함수, 현재 파일이 존재하는 폴더가 STATEKEY값
			pImgPath->wstrStateKey = PathFindFileName(szBuf);

			PathRemoveFileSpec(szBuf);
			pImgPath->wstrObjKey = PathFindFileName(szBuf);

			rPathList.push_back(pImgPath);

			bContinue = false;
		}

	}

}

int CFileInfo::DirFileCount(const wstring& wstrPath)
{
	int				iCount = 0;
	CFileFind		Find;

	wstring		wstrFilePath = wstrPath + L"\\*.*";
	BOOL		bContinue = Find.FindFile(wstrFilePath.c_str());

	while (bContinue)
	{
		bContinue = Find.FindNextFile();

		if (Find.IsDots())
			continue;

		if (Find.IsSystem())
			continue;

		++iCount;
	}

	return iCount;
}
