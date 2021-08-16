#include "Sorter.h"


using namespace TagLib;
using namespace std;

std::wstring MFileOpen(FileName f)
{
	FileRef fo(f);

	TagLib::String artist = fo.tag()->artist();
	TagLib::String album = fo.tag()->album();	
	std::wstring art = artist.toWString();
	return art;
}

DWORD FileLister::pathCreator(LPCTSTR path, LPCTSTR name, LPTSTR Dpath)
{
	
	wcscpy_s(Dpath, MAX_PATH, path);
	wcscat_s(Dpath, MAX_PATH, TEXT("\\"));
	wcscat_s(Dpath, MAX_PATH, name);
	return 0;
}


DWORD FileLister::DirectoryCreate(LPCTSTR path)
{
	if (path == nullptr)
	{
		return NULL;
	}

	int a = CreateDirectory(path, NULL);
	DWORD last_error = GetLastError();

	if (a)
	{
		return 1;
	}
	else
	{
		DWORD errCode = GetLastError();

		if (errCode == ERROR_ALREADY_EXISTS)
		{
			return ERROR_ALREADY_EXISTS;
		}
		else if (errCode == ERROR_PATH_NOT_FOUND)
		{
			return ERROR_PATH_NOT_FOUND;
		}

	}
	return 0;
}

int FileLister::intialiser(std::wstring path[32], std::wstring opath, std::wstring dpath, std::wstring spath, int size)
{
	//Intialisation Of No. of Excluded Files
	n = size;

	//Intialisation Of Origin File Path
	OriginFilePath = opath;
	if (OriginFilePath.size() == 0)
	{
		MessageBox(NULL, TEXT("Could not copy Origin File Path"), TEXT("Error"), MB_OK);
		return -1;
	}

	//Intialisation Of Origin File Path
	DestinationFilePath = dpath;
	if (DestinationFilePath.size() == 0)
	{
		MessageBox(NULL, TEXT("Could not copy Destination File Path"), TEXT("Error"), MB_OK);
		return -1;
	}

	//Intialisation Of Excluded File Path
	for (int i = 0; i < size; i++)
	{
		EFname[i].insert(0, path[i]);
		if (EFname[i].size() == 0)
		{
			MessageBox(NULL, TEXT("Could not copy Excluded File Path"), TEXT("Error"), MB_OK);
			return -1;
		}
	}
	//Intialisation Of Save File Path
	SaveFilePath = spath;
	if (SaveFilePath.size() == 0)
	{
		MessageBox(NULL, TEXT("Could not copy Save File Path"), TEXT("Error"), MB_OK);
		return -1;
	}

	return 0;
}

int FileLister::Exclude(TCHAR* path, int index)
{
	EFname[index].insert(0, path);
	return 0;
}

int FileLister::SaveFile(LPCTSTR spath)
{
	SaveFilePath = spath;
	if (SaveFilePath.size() == 0)
	{
		MessageBox(NULL, TEXT("Could not copy Filename"), TEXT("Error"), MB_OK);
		return 0;
	}
	return 1;
}

int FileLister::OpenFile()
{
	TCHAR path[MAX_PATH];
	StringCchCopy(path, MAX_PATH, SaveFilePath.c_str());
	StringCchCat(path, MAX_PATH, TEXT("\\list"));

	//Open file 
	filelist = _wfsopen(path, TEXT("ab"), _SH_DENYWR);
	if (filelist == NULL)
	{
		MessageBox(NULL, TEXT("Could not open the file!"), TEXT("File Error"), MB_OK);
		exit(1);
	}

	StringCchCopy(path, MAX_PATH, SaveFilePath.c_str());
	StringCchCat(path, MAX_PATH, TEXT("\\MovedList"));

	//Open file 
	movedfiles = _wfsopen(path, TEXT("ab"), _SH_DENYWR);
	if (movedfiles == NULL)
	{
		MessageBox(NULL, TEXT("Could not open the file!"), TEXT("File Error"), MB_OK);
		exit(1);
	}
	return 0;
}

int FileLister::WriteToFile(LPCTSTR str, FILE* f)
{
	if ((fputws(str, f)) == EOF)
	{
		MessageBox(NULL, TEXT("Could not write to the file!"), TEXT("File Error"), MB_OK);
		exit(1);
	}
	if ((fputws(TEXT("\n"), f)) == EOF)
	{
		MessageBox(NULL, TEXT("Could not write to the file!"), TEXT("File Error"), MB_OK);
		exit(1);
	}
	return 0;

}

int FileLister::check(const TCHAR* other_filename)
{
	//Compare the filenames 
	for (int i = 0; i < n; i++)
	{
		if (!EFname[i].compare(other_filename)) //returns 0 if it matches
			return 1;
		//if (EFname[i].find(other_filename) != std::string::npos) //returns -1 if it matches
		//	return 1;
	}
	return 0;
}

int FileLister::ListFiles(LPCTSTR path, LPCTSTR path2)
{
	static long unsigned int fno = 1;
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	int flag = 0;
	DWORD err;

	hFind = FindFirstFile(path, &ffd);


	if (INVALID_HANDLE_VALUE == hFind)
	{
		MessageBox(NULL, TEXT(" Invaild Handle..."), TEXT("Handle Error"), MB_OK);
		return 0;
	}
	else
	{
		//TO skip '.' and '..' file and check if folder is empty
		if (!(wcscmp(ffd.cFileName, TEXT("."))))
		{
			//To check if folder is empty
			if (FindNextFile(hFind, &ffd) == 0)
			{
				FindClose(hFind);
				return 0;
			}
		}

		if (!(wcscmp(ffd.cFileName, TEXT(".."))))
		{
			//To check if folder is empty
			if (FindNextFile(hFind, &ffd) == 0)
			{
				FindClose(hFind);
				return 0;
			}
		}

		do
		{
			//For directory files
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				//To check the excluded directories
				if (check(ffd.cFileName))
				{
					continue;
				}

				TCHAR search_path[MAX_PATH];
				TCHAR prev_path[MAX_PATH];

				//To remove '*' from pathname and add '*' the end
				StringCchCopy(search_path, MAX_PATH, path2);
				StringCchCat(search_path, MAX_PATH, ffd.cFileName);
				StringCchCat(search_path, MAX_PATH, TEXT("\\*"));

				//To copy a pathname without '*'
				StringCchCopy(prev_path, MAX_PATH, path2);
				StringCchCat(prev_path, MAX_PATH, ffd.cFileName);
				StringCchCat(prev_path, MAX_PATH, TEXT("\\"));

				ListFiles(search_path, prev_path);

				//To remove current directory from the pathname



			}

			//For normal files
			else
			{
				std::wstring ext = ffd.cFileName;
				size_t pos;
				pos = ext.find_last_of(TEXT("."));

				if (pos != ext.npos)
				{

					//For .mp3 file
					if (!(ext.compare(pos, ext.npos, TEXT(".mp3"))))
					{
						DisplayFiles(path2, ffd.cFileName, fno);
						fno++;
					}

					//For .flac file
					else if (!(ext.compare(pos, ext.npos, TEXT(".flac"))))
					{
						DisplayFiles(path2, ffd.cFileName, fno);
						fno++;
					}

					//For .wav file
					else if (!(ext.compare(pos, ext.npos, TEXT(".wav"))))
					{
						DisplayFiles(path2, ffd.cFileName, fno);
						fno++;
					}

					//For .m4a file
					else if (!(ext.compare(pos, ext.npos, TEXT(".m4a"))))
					{
						DisplayFiles(path2, ffd.cFileName, fno);
						fno++;
					}

					//For .aac file
					else if (!(ext.compare(pos, ext.npos, TEXT(".aac"))))
					{
						DisplayFiles(path2, ffd.cFileName, fno);
						fno++;
					}

					//For .wma file
					else if (!(ext.compare(pos, ext.npos, TEXT(".wma"))))
					{
						DisplayFiles(path2, ffd.cFileName, fno);
						fno++;
					}

					//For .ogg file
					else if (!(ext.compare(pos, ext.npos, TEXT(".ogg"))))
					{
						DisplayFiles(path2, ffd.cFileName, fno);
						fno++;
					}


				}



			}

		} while (FindNextFile(hFind, &ffd) != 0);
		err = GetLastError();
		if (err == ERROR_NO_MORE_FILES)
		{
			FindClose(hFind);
			return 0;
		}
	}
	return 0;
}

void FileLister::DisplayFiles(LPCTSTR path, TCHAR Filename[], int fno)
{
	TCHAR full_path[MAX_PATH];
	StringCchCopy(full_path, MAX_PATH, path);
	StringCchCat(full_path, MAX_PATH, Filename);
	//system("cls");
	//wcout << "Writing file(" << fno << "): " << full_path << endl;
	WriteToFile(full_path, filelist);
	Sorter(full_path, Filename);
}

DWORD FileLister::FileMover(LPCTSTR orgPath, LPCTSTR destPath)
{
	BOOL result;
	result = MoveFileWithProgress(orgPath, destPath, LpprogressRoutine, NULL, MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH);
	if (!result)
	{
		DWORD err = GetLastError();
		if (err == ERROR_FILE_NOT_FOUND)
		{
			return ERROR_FILE_NOT_FOUND;
		}
		else if (err == ERROR_FILE_EXISTS)
		{
			return ERROR_FILE_EXISTS;
		}
	}
	else
	{
		return S_OK;
	}
	return 0;
}

LPCTSTR FileLister::DirCreate(LPCTSTR fpath, TCHAR dir[MAX_PATH])
{
	if (fpath == nullptr)
	{
		return nullptr;
	}

	FileName f(fpath);
	std::wstring name = TEXT("");
	//if (f.wstr)
	FileRef fo(f);

	TagLib::String artist = fo.tag()->artist();
	TagLib::String album = fo.tag()->album();

	//TCHAR dir[MAX_PATH];

	LPTSTR Dpath = dir; //= nullptr; //Path for moving file
	LPCTSTR temp = nullptr;
	temp = DestinationFilePath.c_str();
	DWORD err;

	if (!artist.isEmpty())
	{
		err = pathCreator(temp, artist.toCWString(), Dpath);
	}
	else
	{
		err = pathCreator(temp, TEXT("Unknown Artist"), Dpath);
	}

	err = DirectoryCreate(Dpath);

	if ((err == ERROR_ALREADY_EXISTS) || err)
	{
		if (!album.isEmpty())
		{
			err = pathCreator(Dpath, album.toCWString(), Dpath);
		}
		else
		{
			err = pathCreator(Dpath, TEXT("Unknown Album"), Dpath);
		}

		DirectoryCreate(Dpath);
		return Dpath;
	}


	//name=MFileOpen(f);

	return Dpath;
}

int FileLister::Sorter(LPCTSTR orgpath, TCHAR Filename[])
{
	if (orgpath == nullptr || Filename == nullptr)
	{
		return -1;
	}
	TCHAR destpath[MAX_PATH];
	TCHAR Dpath[MAX_PATH];
	LPCTSTR Opath = Dpath;
	Opath = DirCreate(orgpath, Dpath);
	if (FAILED(StringCchCopy(destpath, MAX_PATH, Opath)))
	{
		return -1;
	}
	if (FAILED(StringCchCat(destpath, MAX_PATH, TEXT("\\"))))
	{
		return -1;
	}
	if (FAILED(StringCchCat(destpath, MAX_PATH, Filename)))
	{
		return -1;
	}

	DWORD result;
	result = FileMover(orgpath, destpath);
	TCHAR msg[MAX_PATH];
	if (result == S_OK)
	{
		//
	}
	switch (result)
	{
	case S_OK:	WriteToFile(destpath, movedfiles);
		break;
	case ERROR_FILE_NOT_FOUND:
		StringCchCopy(msg, MAX_PATH, TEXT("Could not find the file: \""));
		StringCchCat(msg, MAX_PATH, orgpath);
		StringCchCat(msg, MAX_PATH, TEXT(" \" "));
		MessageBox(NULL, msg, TEXT("File not found"), MB_OK | MB_ICONINFORMATION);
		break;

	case ERROR_FILE_EXISTS:	StringCchCopy(msg, MAX_PATH, TEXT("\""));
		StringCchCat(msg, MAX_PATH, destpath);
		StringCchCat(msg, MAX_PATH, TEXT(" \" already exists."));
		MessageBox(NULL, TEXT("File Exists"), TEXT("File Already exists"), MB_OK | MB_ICONINFORMATION);
		break;
	default: break;
	}

	return 0;
}

int FileLister::start()
{
	OpenFile();

	TCHAR szDir[MAX_PATH];
	TCHAR OrgDir[MAX_PATH];
	std::wstring dir;
	dir = OriginFilePath;
	StringCchCopy(szDir, MAX_PATH, dir.c_str());
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

	StringCchCopy(OrgDir, MAX_PATH, dir.c_str());
	StringCchCat(OrgDir, MAX_PATH, TEXT("\\"));
	ListFiles(szDir, OrgDir);
	return 0;
}

DWORD CALLBACK LpprogressRoutine
(LARGE_INTEGER TotalFileSize,
	LARGE_INTEGER TotalBytesTransferred,
	LARGE_INTEGER StreamSize,
	LARGE_INTEGER StreamBytesTransferred,
	DWORD dwStreamNumber,
	DWORD dwCallbackReason,
	HANDLE hSourceFile,
	HANDLE hDestinationFile,
	LPVOID lpData)
{
	double progressPercent = 0;
	progressPercent = (double)((double)TotalBytesTransferred.QuadPart / (double)TotalFileSize.QuadPart) * 100;

	if (dwCallbackReason == CALLBACK_CHUNK_FINISHED)
	{

		/*system("cls");
		wcout << " Current Progress: " << progressPercent << "% " << endl;*/
		/*wstring per= to_wstring(progressPercent) + TEXT("%");
		LPCTSTR pmsg = (LPCWSTR)&per;
		MessageBox(NULL, pmsg, TEXT("Current percentage"), MB_OK);*/
	}
	else if (dwCallbackReason == CALLBACK_STREAM_SWITCH)
	{
		/*wcout << " Starting the Move\n" << endl;
		wcout << " Current Progress: " << progressPercent << "% " << endl;*/

	}
	return PROGRESS_CONTINUE;
}


