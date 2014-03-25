#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <tchar.h>

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
using namespace std;

#include "resource.h"

typedef std::basic_string<TCHAR, char_traits<TCHAR>, allocator<TCHAR> > tstring;
typedef std::basic_stringstream<TCHAR,  char_traits<TCHAR>, allocator<TCHAR> > tstringstream;
#ifdef UNICODE
#define to_tstring to_wstring
#else
#define to_tstring to_string
#endif

#include "DBCFile.h"

#include "Velvet.h"
