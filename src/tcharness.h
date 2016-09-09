#include <tchar.h>
#include <iterator>

#define _TLLFMT	_T("%I64")
#define _TOTSTRING(buf, fmt, val)	\
	_stprintf_s(buf, sizeof (buf) / sizeof (TCHAR), fmt, val)

namespace std
{
	typedef basic_string<TCHAR, char_traits<TCHAR>, allocator<TCHAR> > tstring;

	typedef basic_ios<TCHAR, char_traits<TCHAR> > tios;
	typedef basic_streambuf<TCHAR, char_traits<TCHAR> > tstreambuf;
	typedef basic_istream<TCHAR, char_traits<TCHAR> > tistream;
	typedef basic_ostream<TCHAR, char_traits<TCHAR> > tostream;
	typedef basic_iostream<TCHAR, char_traits<TCHAR> > tiostream;
	typedef basic_stringbuf<TCHAR, char_traits<TCHAR>, allocator<TCHAR> > tstringbuf;
	typedef basic_istringstream<TCHAR, char_traits<TCHAR>, allocator<TCHAR> > tistringstream;
	typedef basic_ostringstream<TCHAR, char_traits<TCHAR>, allocator<TCHAR> > tostringstream;
	typedef basic_stringstream<TCHAR, char_traits<TCHAR>, allocator<TCHAR> > tstringstream;
	typedef basic_filebuf<TCHAR, char_traits<TCHAR> > tfilebuf;
	typedef basic_ifstream<TCHAR, char_traits<TCHAR> > tifstream;
	typedef basic_ofstream<TCHAR, char_traits<TCHAR> > tofstream;
	typedef basic_fstream<TCHAR, char_traits<TCHAR> > tfstream;

	inline tstring to_tstring(int _Val)
	{	// convert int to tstring
		TCHAR _Buf[2 * _MAX_INT_DIG];

		_CSTD _TOTSTRING(_Buf, _T("%d"), _Val);
		return (tstring(_Buf));
	}

	inline tstring to_tstring(unsigned int _Val)
	{	// convert unsigned int to tstring
		TCHAR _Buf[2 * _MAX_INT_DIG];

		_CSTD _TOTSTRING(_Buf, _T("%u"), _Val);
		return (tstring(_Buf));
	}

	inline tstring to_tstring(long _Val)
	{	// convert long to tstring
		TCHAR _Buf[2 * _MAX_INT_DIG];

		_CSTD _TOTSTRING(_Buf, _T("%ld"), _Val);
		return (tstring(_Buf));
	}

	inline tstring to_tstring(unsigned long _Val)
	{	// convert unsigned long to tstring
		TCHAR _Buf[2 * _MAX_INT_DIG];

		_CSTD _TOTSTRING(_Buf, _T("%lu"), _Val);
		return (tstring(_Buf));
	}

	inline tstring to_tstring(_Longlong _Val)
	{	// convert long long to tstring
		TCHAR _Buf[2 * _MAX_INT_DIG];

		_CSTD _TOTSTRING(_Buf, _TLLFMT _T("d"), _Val);
		return (tstring(_Buf));
	}

	inline tstring to_tstring(_ULonglong _Val)
	{	// convert unsigned long long to tstring
		TCHAR _Buf[2 * _MAX_INT_DIG];

		_CSTD _TOTSTRING(_Buf, _TLLFMT _T("u"), _Val);
		return (tstring(_Buf));
	}

	inline tstring to_tstring(long double _Val)
	{	// convert long double to tstring
		typedef back_insert_iterator<tstring> _Iter;
		typedef num_put<TCHAR, _Iter> _Nput;
		const _Nput& _Nput_fac = use_facet<_Nput>(locale());
		ostream _Ios((streambuf *)0);
		tstring _Str;

		_Ios.setf(ios_base::fixed);
		_Nput_fac.put(_Iter(_Str), _Ios, _T(' '), _Val);
		return (_Str);
	}

	inline tstring to_tstring(double _Val)
	{	// convert double to tstring
		return (to_tstring((long double)_Val));
	}

	inline tstring to_tstring(float _Val)
	{	// convert float to tstring
		return (to_tstring((long double)_Val));
	}
};
