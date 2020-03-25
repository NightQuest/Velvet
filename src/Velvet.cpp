#include "preCompiled.h"

const wchar_t sz_ClassName[] = L"VelvetAppWC";
HWND hwndMain = 0, hwndStatusbar = 0;
HINSTANCE hInst;
HACCEL hAccTable = 0;

// Main program function - called when the program starts
BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	INITCOMMONCONTROLSEX icce;
	WNDCLASSEX	wcx;
	BOOL bRet;
	MSG msg;

	hInst = hInstance;

	// Register which window classes we'll be using
	memset(&icce, 0, sizeof(INITCOMMONCONTROLSEX));
	icce.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icce.dwICC = ICC_BAR_CLASSES | ICC_LISTVIEW_CLASSES;
	if( !InitCommonControlsEx(&icce) )
		return TRUE;

	// Load our Accelerators
	hAccTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MAIN_MENU_ACCELERATOR));
	if( !hAccTable )
	{
		MessageBox(NULL, L"Unable to load Accelerators!", L"Error!", MB_ICONERROR | MB_OK);
		return TRUE;
	}

	// Fill wcx with our main window's class information.
	wcx.cbSize			= sizeof(WNDCLASSEX);
	wcx.style			= 0;
	wcx.lpfnWndProc		= MainWindowProc;
	wcx.cbClsExtra		= 0;
	wcx.cbWndExtra		= 0;
	wcx.hInstance		= hInstance;
	wcx.hIcon			= LoadIcon(hInstance, IDI_APPLICATION);
	wcx.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground	= reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wcx.lpszMenuName	= MAKEINTRESOURCE(IDR_MAIN_MENU);
	wcx.lpszClassName	= sz_ClassName;
	wcx.hIconSm			= reinterpret_cast<HICON>(LoadImage(hInstance, IDI_APPLICATION, IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR));

	// Register our main window's class
	if( !RegisterClassEx(&wcx) )
	{
		MessageBox( NULL, L"Window Registration Failed!", L"Error!", MB_ICONERROR | MB_OK );
		return TRUE;
	}

	int height = 600;
	int width = 900;

	// Create our main window
	hwndMain = CreateWindowEx(NULL,
		sz_ClassName, L"Velvet",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME,
		((GetSystemMetrics(SM_CXSCREEN)/2)-(width/2)), ((GetSystemMetrics(SM_CYSCREEN)/2)-(height/2)), // centered
		width, height,
		NULL, NULL, hInstance, NULL);

	if( !hwndMain )
	{
		MessageBox( NULL, L"Window Creation Failed!", L"Error!", MB_ICONERROR | MB_OK );
		return TRUE;
	}

	// Display and draw our main window
	ShowWindow(hwndMain, nCmdShow);
	UpdateWindow(hwndMain);

	// The main message loop
	while( (bRet = GetMessage(&msg, NULL, 0, 0)) != 0 && bRet != -1 )
	{
		if( !TranslateAccelerator(hwndMain, hAccTable, &msg) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

// Procedure for handling our main window's events
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CREATE:
		{
			hwndStatusbar = CreateWindowEx(NULL,
				STATUSCLASSNAME, NULL,
				WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
				NULL, NULL,
				NULL, NULL,
				hwnd, NULL, hInst, NULL);

			SendMessage(hwndStatusbar, WM_SETFONT, reinterpret_cast<WPARAM>(GetStockObject(DEFAULT_GUI_FONT)), TRUE);
		}
		break;

	case WM_SIZE:
		SendMessage(hwndStatusbar, WM_SIZE, 0, 0);
		break;

	case WM_SHOWWINDOW:
		{
			int statwidths[] = { 100, 200, 300, -1 };

			SendMessage(hwndStatusbar, SB_SETPARTS, 4, reinterpret_cast<LPARAM>(statwidths));
			SendMessage(hwndStatusbar, SB_SETTEXT, 0|SBT_NOBORDERS, reinterpret_cast<LPARAM>(L""));
			SendMessage(hwndStatusbar, SB_SETTEXT, 1|SBT_NOBORDERS, reinterpret_cast<LPARAM>(L""));
			SendMessage(hwndStatusbar, SB_SETTEXT, 2|SBT_NOBORDERS, reinterpret_cast<LPARAM>(L""));
			SendMessage(hwndStatusbar, SB_SETTEXT, 3|SBT_NOBORDERS, reinterpret_cast<LPARAM>(L""));
		}
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDM_FILE_EXIT:
			PostQuitMessage(0);
			break;

		case IDM_FILE_OPEN:
			{
				std::vector<wchar_t> filename(1024);
				OPENFILENAME ofn;
				memset(&ofn, 0, sizeof(OPENFILENAME));
				ofn.lStructSize	= sizeof(OPENFILENAME);
				ofn.hwndOwner	= hwndMain;
				ofn.lpstrFilter	= L"DBC Files (*.dbc)\0*.dbc\0All (*.*)\0*.*\0\0";
				ofn.lpstrFile	= &filename[0];
				ofn.nMaxFile	= filename.size();

				if( GetOpenFileName(&ofn) != 0 )
				{
					DBCFile dbc;
					if( dbc.open(&filename[0]) )
					{
						DBCHeader header = dbc.getHeader();

						SendMessage(hwndStatusbar, SB_SETTEXT, 0|SBT_NOBORDERS, reinterpret_cast<LPARAM>(((L"Rows: " + std::to_wstring(header.rows)).c_str())));
						SendMessage(hwndStatusbar, SB_SETTEXT, 1|SBT_NOBORDERS, reinterpret_cast<LPARAM>(((L"Columns: " + std::to_wstring(header.columns)).c_str())));
						SendMessage(hwndStatusbar, SB_SETTEXT, 2|SBT_NOBORDERS, reinterpret_cast<LPARAM>(((L"Row Size: " + std::to_wstring(header.rowSize) + L"B").c_str())));
						SendMessage(hwndStatusbar, SB_SETTEXT, 3 | SBT_NOBORDERS, reinterpret_cast<LPARAM>(((L"String Table Size: " + std::to_wstring(header.stringTableSize) + L"B").c_str())));
					}
					else
						MessageBox(hwndMain, L"This is not a valid DBC file.", L"Error!", MB_ICONERROR | MB_OK);
				}
			}
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return FALSE;
}
