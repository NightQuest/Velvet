#include "preCompiled.h"

const TCHAR sz_ClassName[] = _T("VelvetAppWC");
HWND hwndMain = 0;
HACCEL hAccTable = 0;

// Main program function - called when the program starts
BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	INITCOMMONCONTROLSEX icce;
	WNDCLASSEX	wcx;
	BOOL bRet;
	MSG msg;

	// Register which window classes we'll be using
	memset(&icce, 0, sizeof(INITCOMMONCONTROLSEX));
	icce.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icce.dwICC = ICC_STANDARD_CLASSES | ICC_LISTVIEW_CLASSES;
	if( !InitCommonControlsEx(&icce) )
		return TRUE;

	// Load our Accelerators
	hAccTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MAIN_MENU_ACCELERATOR));
	if( !hAccTable )
	{
		MessageBox(NULL, _T("Unable to load Accelerators!"), _T("Error!"), MB_ICONERROR | MB_OK);
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
	wcx.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	wcx.lpszMenuName	= MAKEINTRESOURCE(IDR_MAIN_MENU);
	wcx.lpszClassName	= sz_ClassName;
	wcx.hIconSm			= (HICON)LoadImage(hInstance, IDI_APPLICATION, IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);

	// Register our main window's class
	if( !RegisterClassEx(&wcx) )
	{
		MessageBox( NULL, _T("Window Registration Failed!"), _T("Error!"), MB_ICONERROR | MB_OK );
		return TRUE;
	}

	int height = 200;
	int width = 300;

	// Create our main window
	hwndMain = CreateWindowEx(NULL,
		sz_ClassName, _T("Velvet"),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		((GetSystemMetrics(SM_CXSCREEN)/2)-(width/2)), ((GetSystemMetrics(SM_CYSCREEN)/2)-(height/2)), // centered
		width, height,
		NULL, NULL, hInstance, NULL);

	if( !hwndMain )
	{
		MessageBox( NULL, _T("Window Creation Failed!"), _T("Error!"), MB_ICONERROR | MB_OK );
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
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDM_FILE_EXIT:
			PostQuitMessage(0);
			break;

		case IDM_FILE_OPEN:
			{
				vector<TCHAR> filename(1024);
				OPENFILENAME ofn;
				memset(&ofn, 0, sizeof(OPENFILENAME));
				ofn.lStructSize	= sizeof(OPENFILENAME);
				ofn.hwndOwner	= hwndMain;
				ofn.lpstrFilter	= _T("DBC Files (*.dbc)\0*.dbc\0All (*.*)\0*.*\0\0");
				ofn.lpstrFile	= &filename[0];
				ofn.nMaxFile	= filename.size();

				if( GetOpenFileName(&ofn) != 0 )
				{
					DBCFile dbc;
					if( dbc.open(&filename[0]) )
					{
						tstringstream ss;
						ss	<< _T("Rows: ") << to_tstring(dbc.getHeader().rows) << _T("\r\n")
							<< _T("Columns: ") << to_tstring(dbc.getHeader().columns) << _T("\r\n");

						MessageBox(hwndMain, ss.str().c_str(), _T("DBC Information"), MB_ICONINFORMATION | MB_OK);
					}
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
