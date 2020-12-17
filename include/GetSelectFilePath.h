//************************************
// Method:          GetSelectFilePath
// Access:          public 
// Returns:         string
// CreateTime:      December 18, 2020
// Author:          Patixsier
// Describe:        弹出文件资源管理器，获取选中文件(夹)路径
//************************************
#pragma once
#pragma comment(lib, "Comdlg32.lib")

#include <windows.h>
#include <ShlObj.h>
#include <stdio.h>
#include <stdlib.h>
#include <atlstr.h>
#include <string>
#include <iostream>

// string 转换为路径可用的 char*
char* String2FilePath(std::string src)
{
	char* dst = new char[255];
	int i;
	for (i = 0; i <= src.length(); i++)
		dst[i] = src[i];
	dst[i] = '\0';
	return dst;
}

// 弹出文件资源管理器，获取选中文件路径
std::string GetSelectFilePath_string()
{
	std::string FilePath = "";
	CString path;
	OPENFILENAME open;// 公共对话框结构。
	char file[MAX_PATH];// 用来保存获取文件名称的缓冲区。 
	ZeroMemory(&open, sizeof(OPENFILENAME)); // 初始化选择文件对话框
	open.lStructSize = sizeof(OPENFILENAME);//指定这个结构的大小，以字节为单位。
	open.lpstrFile = file;//打开的文件的全路径
	open.lpstrFile[0] = '\0'; //第一个字符串是过滤器描述的显示字符串
	open.nMaxFile = MAX_PATH;  //指定lpstrFile缓冲的大小，以TCHARs为单位
	open.lpstrFilter = "所有文件(*.*)\0*.*\0\0";  //打开文件类型
	open.nFilterIndex = 1;  //指定在文件类型控件中当前选择的过滤器的索引
	open.lpstrFileTitle = NULL; // 指向接收选择的文件的文件名和扩展名的缓冲（不带路径信息）。这个成员可以是NULL。
	open.nMaxFileTitle = 0;  //指定lpstrFileTitle缓冲的大小，以TCHARs为单位
	open.lpstrInitialDir = NULL;  //指向以空字符结束的字符串，可以在这个字符串中指定初始目录。
	open.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;//位标记的设置，你可以使用来初始化对话框
	//GetOpenFileName (&open) ;//打开文件对话框
	//GetSaveFileName(&open);//保存文件对话框
	if (GetOpenFileName(&open))  // 显示打开选择文件对话框。  
		path = file;
	else {
		std::cout << "没有选择文件。\n\n";
		return FilePath;
	}
	FilePath = path.GetBuffer(0);
	return FilePath;
}

// 弹出文件资源管理器，获取选中文件路径
char* GetSelectFilePath() {
	char* path = String2FilePath(GetSelectFilePath_string());
	return path;
}

// 弹出文件资源管理器，获取选中文件夹路径
std::string GetSelectFolderPath_string()
{
	std::string FolderPath = "";

	TCHAR           szFolderPath[MAX_PATH] = { 0 };
	CString         strFolderPath = TEXT("");

	BROWSEINFO      sInfo;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot = 0;
	sInfo.lpszTitle = _T("请选择一个文件夹：");
	sInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
	sInfo.lpfn = NULL;

	// 显示文件夹选择对话框  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL)
	{
		// 取得文件夹名  
		if (::SHGetPathFromIDList(lpidlBrowse, szFolderPath))
		{
			strFolderPath = szFolderPath;
		}
	}
	else {
		std::cout << "没有选择文件夹。\n\n";
		return FolderPath;
	}

	if (lpidlBrowse != NULL)
	{
		::CoTaskMemFree(lpidlBrowse);
	}

	FolderPath = strFolderPath.GetBuffer(0);
	return FolderPath;

}

// 弹出文件资源管理器，获取选中文件夹路径
char* GetSelectFolderPath() {
	char* path = String2FilePath(GetSelectFolderPath_string());
	return path;
}