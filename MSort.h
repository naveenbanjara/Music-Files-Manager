#ifndef UNICODE
#define UNICODE
#endif

#include "Sorter.h"
#include <xkeycheck.h>


using namespace std;

template <class DERIVED_TYPE>
class BaseWindow
{
public:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		DERIVED_TYPE* pThis = NULL;

		if (uMsg == WM_NCCREATE)
		{
			CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
			pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

			pThis->m_hwnd = hwnd;
		}
		else
		{
			pThis = (DERIVED_TYPE*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		}
		if (pThis)
		{
			return pThis->HandleMessage(uMsg, wParam, lParam);
		}
		else
		{
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}

	BaseWindow() : m_hwnd(NULL) { }

	BOOL Create(
		PCWSTR lpWindowName,
		DWORD dwStyle,
		DWORD dwExStyle = 0,
		int x = CW_USEDEFAULT,
		int y = CW_USEDEFAULT,
		int nWidth = CW_USEDEFAULT,
		int nHeight = CW_USEDEFAULT,
		HWND hWndParent = 0,
		HMENU hMenu = 0
	)
	{
		WNDCLASS wc = { 0 };

		wc.lpfnWndProc = DERIVED_TYPE::WindowProc;
		wc.hInstance = GetModuleHandle(NULL);
		wc.lpszClassName = ClassName();

		RegisterClass(&wc);


		m_hwnd = CreateWindowEx(
			dwExStyle, ClassName(), lpWindowName, dwStyle, x, y,
			nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this
		);

		return (m_hwnd ? TRUE : FALSE);
	}

	HWND Window() const { return m_hwnd; }

protected:

	virtual PCWSTR  ClassName() const = 0;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

	HWND m_hwnd;
};


class MainWindow : public BaseWindow<MainWindow>
{
	std::wstring Fpath;
	std::wstring Spath;
	std::wstring Dpath;
	std::wstring Epath[32];
	int n;
	LPTSTR Tpath;
	FileLister fl;
	HMENU hMenu;
	HWND sLabel, dLabel, fLabel, eLabel;
	HWND sText, dText, fText, eText;
	HWND sCntrl, dCntrl, fCntrl, eCntrl;
	HWND sortBTN, cnclBTN;
public:
	PCWSTR  ClassName() const { return L"Main Window"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	int ShowDialog(int flag = 0);
	void AddMenus();
	void AddControls();

	wstring FilePath() { return Fpath; }
	MainWindow() : Fpath(TEXT("")), Spath(TEXT("")), Dpath(TEXT("")), Tpath(nullptr),
		sCntrl(nullptr), dCntrl(nullptr), fCntrl(nullptr), eCntrl(nullptr), n(0)
	{
		for (int i = 0; i < 32; i++)
		{
			Epath[i] = TEXT("");
		}
	}

};

class StaticClass : public BaseWindow<StaticClass>
{
	TCHAR text[MAX_PATH];
public:
	PCWSTR  ClassName() const { return L"Static"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);


	StaticClass() : text(TEXT("")) { }

};

class ButtonClass : public BaseWindow<ButtonClass>
{
	TCHAR text[MAX_PATH];
public:
	PCWSTR  ClassName() const { return L"Button"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);


	ButtonClass() : text(TEXT("")) { }

};

class EditClass : public BaseWindow<EditClass>
{
	TCHAR text[MAX_PATH];
public:
	PCWSTR  ClassName() const { return L"Edit"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);


	EditClass() : text(TEXT("")) { }

};