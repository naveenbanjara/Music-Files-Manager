#include "MSort.h"
#define choose_source_path 123
#define choose_destination_path 124
#define choose_logfile_path 125
#define choose_exclude_path 126
#define start_sort 150
#define cancel_sort 158

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	MainWindow win;

	if (!win.Create(TEXT("Music Sorter"), WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 100, 100, 600, 300))
	{
		return 0;
	}	

	ShowWindow(win.Window(), nCmdShow);

	
	MSG msg = {};

	// Run the message loop.
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}


LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int response = IDNO;

	switch (uMsg)
	{
	case WM_COMMAND:
		switch (wParam)
		{
		case choose_source_path:
		{
			Tpath = nullptr;
			ShowDialog();
			if (Tpath != nullptr)
			{
				Spath = Tpath;
				SetWindowText(sText, Tpath);
				MessageBox(NULL, Tpath, L"Source Path", MB_OK);
			}

			Tpath = nullptr;
		}
		break;

		case choose_destination_path:
		{
			Tpath = nullptr;
			ShowDialog();
			if (Tpath != nullptr)
			{
				Dpath = Tpath;
				SetWindowText(dText, Tpath);
				MessageBox(NULL, Tpath, L"Destination Path", MB_OK);
			}

			Tpath = nullptr;
		}
		break;

		case choose_logfile_path:
		{
			Tpath = nullptr;
			ShowDialog();
			if (Tpath != nullptr)
			{
				Fpath = Tpath;
				SetWindowText(fText, Tpath);
				MessageBox(NULL, Tpath, L"Choosen File Path", MB_OK);
			}

			Tpath = nullptr;
		}
		break;

		case choose_exclude_path:
		{
			Tpath = nullptr;
			do {
				ShowDialog(1);
				if (Tpath != nullptr)
				{
					Epath[n++] = Tpath;
					SetWindowText(eText, Tpath);
					MessageBox(NULL, Tpath, L"Exclude Path", MB_OK);
					response = MessageBox(NULL, L"Want to add more?", L"Exclude Path", MB_YESNO | MB_ICONQUESTION);
				}

				Tpath = nullptr;

			} while (response == IDYES);
		}
		break;

		case start_sort:
		{
			MessageBox(NULL, L"Starting sorting", L"Sorter", MB_OK);
			fl.intialiser(Epath, Spath, Dpath, Fpath, n);
			fl.start();
			MessageBox(NULL, L"Sorting over", L"End", MB_OK);
			return 0;
		}
		break;

		case cancel_sort:
		{
			PostQuitMessage(0);
			return 0;
		}
		break;

		case 1:	MessageBox(NULL, Tpath, L"New", MB_OK);
			break;
		case 2:	MessageBox(NULL, Tpath, L"Open", MB_OK);
			break;
		case 3:	DestroyWindow(m_hwnd);
			break;
		}
		break;

	case WM_CREATE:
		AddMenus();
		AddControls();
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(m_hwnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(m_hwnd, &ps);
	}
	return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
	}
	return TRUE;
}

int MainWindow::ShowDialog(int flag)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog* pFileOpen = nullptr;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			pFileOpen->SetOptions(FOS_PICKFOLDERS);
			// Show the Open dialog box.
			hr = pFileOpen->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{

				IShellItem* pItem = nullptr;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					LPTSTR pszFilePath = nullptr;

					//To copy filename 
					if (flag)
					{
						hr = pItem->GetDisplayName(SIGDN_NORMALDISPLAY, &pszFilePath);
					}
					//To copy filepath
					else
					{
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					}

					// Display the file name to the user.
					if (SUCCEEDED(hr))
					{
						//MessageBox(NULL, pszFilePath, L"File Path", MB_OK);
						//StringCchCopy(Tpath, MAX_PATH, pszFilePath);
						Tpath = pszFilePath;
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}

			}
			else if (FAILED(hr))
			{
				MessageBox(NULL, L"Unable to read path", L"Error", MB_OK);
			}
			if (pFileOpen != nullptr)
			{
				pFileOpen->Release();
			}
		}
		CoUninitialize();
	}
	return 0;
}

void MainWindow::AddMenus()
{
	hMenu = CreateMenu();
	HMENU fileMenu = CreateMenu();

	AppendMenu(fileMenu, MF_STRING, 1, TEXT("New"));
	AppendMenu(fileMenu, MF_STRING, 2, TEXT("Open"));
	AppendMenu(fileMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(fileMenu, MF_STRING, 3, TEXT("Exit"));

	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)fileMenu, TEXT("File"));
	AppendMenu(hMenu, MF_STRING, NULL, TEXT("Help"));

	SetMenu(m_hwnd, hMenu);
}

void MainWindow::AddControls()
{
	//Source File
	sLabel = CreateWindow(TEXT("static"), TEXT("Source File Path"), WS_VISIBLE | WS_CHILD | WS_BORDER | SS_SUNKEN, 50, 20, 200, 20, m_hwnd, NULL, NULL, NULL);
	sText = CreateWindow(TEXT("edit"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 50, 45, 200, 20, m_hwnd, NULL, NULL, NULL);
	sCntrl = CreateWindow(TEXT("button"), TEXT("..."), WS_VISIBLE | WS_CHILD | WS_BORDER, 250, 45, 30, 20, m_hwnd, (HMENU)choose_source_path, NULL, NULL);

	//Destination File
	dLabel = CreateWindow(TEXT("static"), TEXT("Destination File Path"), WS_VISIBLE | WS_CHILD | WS_BORDER, 50, 70, 200, 20, m_hwnd, NULL, NULL, NULL);
	dText = CreateWindow(TEXT("edit"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 50, 95, 200, 20, m_hwnd, NULL, NULL, NULL);
	dCntrl = CreateWindow(TEXT("button"), TEXT("..."), WS_VISIBLE | WS_CHILD | WS_BORDER, 250, 95, 30, 20, m_hwnd, (HMENU)choose_destination_path, NULL, NULL);

	//Log File
	fLabel = CreateWindow(TEXT("static"), TEXT("Log File Path"), WS_VISIBLE | WS_CHILD | WS_BORDER, 50, 120, 200, 20, m_hwnd, NULL, NULL, NULL);
	fText = CreateWindow(TEXT("edit"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 50, 145, 200, 20, m_hwnd, NULL, NULL, NULL);
	fCntrl = CreateWindow(TEXT("button"), TEXT("..."), WS_VISIBLE | WS_CHILD | WS_BORDER, 250, 145, 30, 20, m_hwnd, (HMENU)choose_logfile_path, NULL, NULL);

	//Exclude Folders
	eLabel = CreateWindow(TEXT("static"), TEXT("Exclude File Path"), WS_VISIBLE | WS_CHILD | WS_BORDER, 50, 170, 200, 20, m_hwnd, NULL, NULL, NULL);
	eText = CreateWindow(TEXT("edit"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 50, 195, 200, 20, m_hwnd, NULL, NULL, NULL);
	eCntrl = CreateWindow(TEXT("button"), TEXT("..."), WS_VISIBLE | WS_CHILD | WS_BORDER, 250, 195, 30, 20, m_hwnd, (HMENU)choose_exclude_path, NULL, NULL);

	//Buttons
	sortBTN = CreateWindow(TEXT("button"), TEXT("Sort"), WS_VISIBLE | WS_CHILD | WS_BORDER, 500, 20, 80, 20, m_hwnd, (HMENU)start_sort, NULL, NULL);
	cnclBTN = CreateWindow(TEXT("button"), TEXT("Exit"), WS_VISIBLE | WS_CHILD | WS_BORDER, 500, 50, 80, 20, m_hwnd, (HMENU)cancel_sort, NULL, NULL);
}

LRESULT StaticClass::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		SetWindowText(m_hwnd, TEXT("Source File change"));
		break;

		/*case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(m_hwnd, &ps);
			FillRect(hdc, &ps.rcPaint, CreateSolidBrush(0x0000FFFF));
			EndPaint(m_hwnd, &ps);
			return 0;
		}*/
	default:
		return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
		break;
	}
	return TRUE;
}

LRESULT EditClass::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	default:
		return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
		break;
	}
	return TRUE;
}

LRESULT ButtonClass::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	default:
		return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
		break;
	}
	return TRUE;
}
