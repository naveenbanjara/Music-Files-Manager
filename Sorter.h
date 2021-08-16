#pragma once
//#define TAGLIB_STATIC
//#define ZLIB_WINAPI
//#define _ITERATOR_DEBUG_LEVEL 0

//Windows
#include<Windows.h>
#include<fileapi.h>
#include<strsafe.h>
#include<iostream>
#include<conio.h>
#include<tchar.h>
#include<fcntl.h>
#include<io.h>
#include<stdio.h>
#include<share.h>
#include<string>

#include<windowsx.h>
#include <shobjidl.h>
#include<sal.h>

//TagLib
#include "taglib/tag.h"
#include "taglib/fileref.h"
#include "taglib/audioproperties.h"
#include "taglib/taglib_export.h"
#include "taglib/tfile.h"
#include "taglib/tfilestream.h"

//Others


class FileLister
{
	std::wstring EFname[32];
	int n; //no. of exclude filenames // MAX=32
	FILE* filelist;
	FILE* movedfiles;
	TCHAR Wname[MAX_PATH];
	/*LPCTSTR OriginFilePath;
	LPCTSTR DestinationFilePath;
	LPCTSTR SaveFilePath;*/
	std::wstring OriginFilePath;
	std::wstring DestinationFilePath;
	std::wstring SaveFilePath;

public:

	FileLister() : OriginFilePath(TEXT("")), DestinationFilePath(TEXT("")), SaveFilePath(TEXT("")), n(0), filelist(nullptr), movedfiles(nullptr)
	{
		for (int i = 0; i < 32; i++)
		{
			EFname[i] = TEXT("");
		}
	}

	~FileLister()
	{
		if ((filelist != nullptr) && (movedfiles != nullptr))
		{
			fclose(filelist);
			fclose(movedfiles);

		}

	}

	//FileLister(TCHAR *path, LPCTSTR opath = nullptr, LPCTSTR spath = nullptr, LPCTSTR dpath = nullptr);
	int intialiser(std::wstring path[32], std::wstring opath, std::wstring dpath, std::wstring spath, int size);

	int Exclude(TCHAR* path, int index);

	int SaveFile(LPCTSTR spath);

	int OpenFile();

	int WriteToFile(LPCTSTR str, FILE* f);

	int check(const TCHAR* other_filename);

	int ListFiles(LPCTSTR path, LPCTSTR path2);

	void DisplayFiles(LPCTSTR path, TCHAR Filename[], int fno);

	DWORD FileMover(LPCTSTR orgPath, LPCTSTR destPath);

	LPCTSTR DirCreate(LPCTSTR fpath, TCHAR dir[MAX_PATH]);

	int Sorter(LPCTSTR orgpath, TCHAR Filename[]);

	int start();

	DWORD DirectoryCreate(LPCTSTR path);

	DWORD pathCreator(LPCTSTR path, LPCTSTR name, LPTSTR Dpath);

	LPCTSTR FileOpener(LPCTSTR Spath, LPCTSTR Fpath, LPCTSTR Destpath);

};

std::wstring MFileOpen(TagLib::FileName f);





DWORD CALLBACK LpprogressRoutine(
	LARGE_INTEGER TotalFileSize,
	LARGE_INTEGER TotalBytesTransferred,
	LARGE_INTEGER StreamSize,
	LARGE_INTEGER StreamBytesTransferred,
	DWORD dwStreamNumber,
	DWORD dwCallbackReason,
	HANDLE hSourceFile,
	HANDLE hDestinationFile,
	LPVOID lpData
);