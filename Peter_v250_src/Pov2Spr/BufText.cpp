
#include "Main.h"

/***************************************************************************\
*																			*
*								Textové řetězce								*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// inicializační prázdný řetězec (modifikuje se počet referencí!)

STRINGDATA	EmptyStringData		= { 1, 0, 0 };


/////////////////////////////////////////////////////////////////////////////
// konstruktor a destruktor

CString::CString() 
{ 
	Attach(&EmptyStringData); 
};

CString::CString(const CString& src) 
{ 
	Attach(src.pData); 
};

CString::CString(const TCHAR chr)
{
	NewBuffer(1);
	pData->Data[0] = chr;
}

CString::CString(LPCTSTR text, const int length)
{
	int len = 0;
	if (text)
	{
		len = length;
		if (len < 0)
		{
			len = lstrlen(text);
		}
	}
	NewBuffer(len);	
	MemCopy(pData->Data, text, len*sizeof(TCHAR));
}

#ifdef _UNICODE

CString::CString(LPCSTR text, const int length)
{
	int len = 0;
	if (text)
	{
		len = length;
		if (len < 0)
		{
			len = lstrlenA(text);
		}
	}
	NewBuffer(len);
	::MultiByteToWideChar(CP_ACP, 0, text, len, pData->Data, len+1);
}

#else //_UNICODE

CString::CString(LPCWSTR text, const int length)
{
	int len = 0;
	if (text)
	{
		len = length;
		if (len < 0)
		{
			len = lstrlenW(text);
		}
	}
	NewBuffer(len);
	::WideCharToMultiByte(CP_ACP, 0, text, len, pData->Data, len+1, NULL, NULL);
}

#endif //_UNICODE

CString::~CString() 
{ 
	Detach(); 
};


/////////////////////////////////////////////////////////////////////////////
// statický konstruktor a destruktor

void CString::Init()
{ 
	Attach(&EmptyStringData); 
};

void CString::Init(STRINGDATA* data)
{ 
	Attach(data); 
};

void CString::Init(const CString& src) 
{ 
	Attach(src.pData); 
};

void CString::Init(LPCTSTR text, const int length)
{
	int len = 0;
	if (text)
	{
		len = length;
		if (len < 0)
		{
			len = lstrlen(text);
		}
	}
	NewBuffer(len);	
	MemCopy(pData->Data, text, len*sizeof(TCHAR));
}

#ifdef _UNICODE

void CString::Init(LPCSTR text, const int length)
{
	int len = 0;
	if (text)
	{
		len = length;
		if (len < 0)
		{
			len = lstrlenA(text);
		}
	}
	NewBuffer(len);
	::MultiByteToWideChar(CP_ACP, 0, text, len, pData->Data, len+1);
}

#else //_UNICODE

void CString::Init(LPCWSTR text, const int length)
{
	int len = 0;
	if (text)
	{
		len = length;
		if (len < 0)
		{
			len = lstrlenW(text);
		}
	}
	NewBuffer(len);
	::WideCharToMultiByte(CP_ACP, 0, text, len, pData->Data, len+1, NULL, NULL);
}

#endif //_UNICODE

void CString::Term()
{ 
	Detach(); 
};


/////////////////////////////////////////////////////////////////////////////
// zápis do bufferu (bez koncové nuly!)

void CString::Write(TCHAR* buf) const
{
	MemCopy(buf, pData->Data, pData->Length*sizeof(TCHAR));
}

#ifdef _UNICODE

void CString::Write(LPSTR buf) const
{
	::WideCharToMultiByte(CP_ACP, 0, pData->Data, pData->Length, buf, pData->Length+1, NULL, NULL);
}

#else // _UNICODE

void CString::Write(LPWSTR buf) const
{
	::MultiByteToWideChar(CP_ACP, 0, pData->Data, pData->Length, buf, pData->Length+1);
}

#endif // _UNICODE


/////////////////////////////////////////////////////////////////////////////
// nastavení délky textu (při prodloužení textu obsah nedefinovaný)

void CString::Length(int len)
{
	CopyWrite();
	SizeBuffer(len);
}


/////////////////////////////////////////////////////////////////////////////
// hledání textu v řetězci (-1 = nenalezeno)

int _fastcall CString::Find(const CString& str) const
{
// hledaný řetězec prázdný - bude pozice 0
	int lenstr = str.pData->Length;
	if (lenstr == 0) return 0;

// hledaný řetězec má délku 1 - převod na hledání znaku
	TCHAR* datastr = str.pData->Data;
	TCHAR chr = datastr[0];
	if (lenstr == 1) return Find(chr);

// příprava ukazatelů
	int i = pData->Length - lenstr;		// počet testovaných pozic - 1
	datastr++;							// druhý znak hledaného řetězce
	lenstr--;							// délka hledaného řetězce - 1
	TCHAR* data = pData->Data;			// začátek prohledávaného řetězce

// cyklus hledání
	for (; i >= 0; i--)
	{
		if (*(data++) == chr)			// shoduje se první znak?
		{
			if (MemCompare(data, datastr, lenstr*sizeof(TCHAR)))	// shoduje se zbytek řetězce?
			{
				return (data - pData->Data - 1);	// návrat pozice začátku řetězce
			}
		}
	}

	return -1;
}


/////////////////////////////////////////////////////////////////////////////
// hledání textu v řetězci (-1 = nenalezeno)

int _fastcall CString::Find(LPCTSTR txt) const
{
// hledaný řetězec prázdný - bude pozice 0
	int lenstr = 0;
	if (txt)
	{
		lenstr = lstrlen(txt);
	}
	if (lenstr == 0) return 0;

// hledaný řetězec má délku 1 - převod na hledání znaku
	const TCHAR* datastr = txt;
	TCHAR chr = datastr[0];
	if (lenstr == 1) return Find(chr);

// příprava ukazatelů
	int i = pData->Length - lenstr;		// počet testovaných pozic - 1
	datastr++;							// druhý znak hledaného řetězce
	lenstr--;							// délka hledaného řetězce - 1
	TCHAR* data = pData->Data;			// začátek prohledávaného řetězce

// cyklus hledání
	for (; i >= 0; i--)
	{
		if (*(data++) == chr)			// shoduje se první znak?
		{
			if (MemCompare(data, datastr, lenstr*sizeof(TCHAR)))	// shoduje se zbytek řetězce?
			{
				return (data - pData->Data - 1);	// návrat pozice začátku řetězce
			}
		}
	}

	return -1;
}


/////////////////////////////////////////////////////////////////////////////
// hledání znaku v řetězci (-1 = nenalezeno)

int _fastcall CString::Find(const TCHAR chr) const
{
	TCHAR* data = pData->Data;
	while ((*data) && (*data != chr))
	{
		data++;
	}

	if (*data == 0)
	{
		return -1;
	}
	
	return data - pData->Data;
}


/////////////////////////////////////////////////////////////////////////////
// hledání znaku od konce řetězce zpět (-1 = nenalezeno)

int _fastcall CString::RevFind(const TCHAR chr) const
{
	int i = pData->Length;
	TCHAR* data = pData->Data + i;
	i--;

	for (; i >= 0; i--)
	{
		data--;
		if (*data == chr) break;
	}

	return i;
}


/////////////////////////////////////////////////////////////////////////////
// vyprázdnění řetězce (uvolnění dat)

void CString::Empty()
{ 
	Detach(); 
	Attach(&EmptyStringData); 
}


/////////////////////////////////////////////////////////////////////////////
// konverze na velká písmena

void CString::UpperCase()
{
	CopyWrite();
	::CharUpper(pData->Data);
}


/////////////////////////////////////////////////////////////////////////////
// konverze na malá písmena

void CString::LowerCase()
{
	CopyWrite();
	::CharLower(pData->Data);
}


/////////////////////////////////////////////////////////////////////////////
// levá část řetězce

CString _fastcall CString::Left(int count) const
{
	int len = pData->Length;

	if ((DWORD)count > (DWORD)len)
	{
		if (count < 0)
		{
			count = 0;
		}
		else
		{
			count = len;
		}
	}
	return CString(pData->Data, count);
}


/////////////////////////////////////////////////////////////////////////////
// pravá část řetězce

CString _fastcall CString::Right(int count) const
{
	int len = pData->Length;

	if ((DWORD)count > (DWORD)len)
	{
		if (count < 0)
		{
			count = 0;
		}
		else
		{
			count = len;
		}
	}
	return CString(pData->Data + len - count, count);
}


/////////////////////////////////////////////////////////////////////////////
// střední část řetězce

CString _fastcall CString::Mid(int first, int count) const
{
	int len = pData->Length;

	if ((DWORD)first > (DWORD)len)
	{
		if (first < 0)
		{
			first = 0;
		}
		else
		{
			first = len;
		}
	}

	if (count < 0) count = 0;
	if ((DWORD)(first + count) > (DWORD)len) count = len - first;
	
	return CString(pData->Data + first, count);
}


/////////////////////////////////////////////////////////////////////////////
// zrušení textu z konce řetězce

void CString::Delete(int first, int count)
{
	int len = pData->Length;

	if ((DWORD)first > (DWORD)len)
	{
		if (first < 0)
		{
			first = 0;
		}
		else
		{
			first = len;
		}
	}

	if (count < 0) count = 0;
	if ((DWORD)(first + count) > (DWORD)len) count = len - first;

	if (count)
	{
		CopyWrite();
		int firstcount = first + count;
		MemCopy(pData->Data + first, pData->Data + firstcount, len - firstcount + 1);
		pData->Length -= count;
	}
}


/////////////////////////////////////////////////////////////////////////////
// odstranění mezer ze začátku/konce řetězce

void CString::TrimLeft()
{
	CopyWrite();					// přivlastnění bufferu

	TCHAR* data = pData->Data;		// ukazatel textu

#ifdef _UNICODE
	while ((*data) && ((WORD)*data <= (WORD)' '))
#else
	while ((*data) && ((BYTE)*data <= (BYTE)' '))
#endif
	{
		data++;
	}

	int dif = data - pData->Data;

	if (dif)
	{
		pData->Length -= dif;
		MemCopy(pData->Data, data, (pData->Length+1)*sizeof(TCHAR));
	}
}

void CString::TrimRight()
{
	CopyWrite();					// přivlastnění bufferu

	int i = pData->Length;			// délka textu
	TCHAR* data = pData->Data + i;	// adresa za koncem textu

	for (; i != 0; i--)				// pro všechny znaky textu
	{
		data--;						// posun adresy
#ifdef _UNICODE
		if ((WORD)*data > (WORD)' ') break;	// pro platný znak konec
#else
		if ((BYTE)*data > (BYTE)' ') break;	// pro platný znak konec
#endif
	}

	pData->Length = i;				// nová délka textu
	pData->Data[i] = 0;				// označení konce textu
}


/////////////////////////////////////////////////////////////////////////////
// načtení textu z resource

void CString::Load(const int nID)
{
	TCHAR buf[0x201];
	buf[0] = 0;
	int len = ::LoadString(hInstance, nID, buf, 0x200);
	Detach();
	NewBuffer(len);	
	MemCopy(pData->Data, buf, len*sizeof(TCHAR));
}


/////////////////////////////////////////////////////////////////////////////
// načtení ze souboru formátu TXT (false=chyba)
	
bool CString::LoadFile(CString jmeno)
{
// otevření souboru
	HANDLE hFile = ::CreateFile(jmeno, GENERIC_READ, 
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, NULL);

// test, zda byl soubor nalezen
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

// zjištění velikosti souboru
	int size = (int)::SetFilePointer(hFile, 0, NULL, FILE_END);
	::SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	if (size < 0) size = 0;

// úschova původního obsahu pro případ chyby
	CString txt(*this);

// vytvoření nového bufferu
	Detach();
	NewBuffer(size/sizeof(TCHAR));

#ifdef _UNICODE

// buffer k načtení souboru
	char* buf = (char*)MemGet(size);

// načtení souboru
	DWORD read;
	::ReadFile(hFile, buf, size, &read, NULL);

// načtení textu
	::MultiByteToWideChar(CP_ACP, 0, buf, pData->Length, pData->Data, pData->Length+1);

// zrušení bufferu
	MemFree(buf);

#else

// načtení souboru
	DWORD read;
	::ReadFile(hFile, pData->Data, size, &read, NULL);

#endif

// uzavření souboru
	::CloseHandle(hFile);		// uzavření souboru

// kontrola správnosti operace
	if (read != (DWORD)size)
	{
		Detach();
		Attach(txt.Data());
		return false;
	}

// ochrana proti načtení nuly
	int len = ::lstrlen(pData->Data);
	if ((len < pData->Length) && (len >= 0))
	{
		pData->Length = len;
		pData->Data[len] = 0;
	}

// příznak - načteno OK
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// uložení textu do souboru formátu TXT (false=chyba)

bool CString::SaveFile(CString jmeno) const
{
// otevření souboru
	HANDLE hFile = ::CreateFile(jmeno, GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

// test, zda byl soubor vytvořen
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

#ifdef _UNICODE
	
// buffer k dekódování souboru
	char* buf = (char*) MemGet(pData->Length + 100);
	Write(buf);
	
// uložení souboru
	DWORD write;
	BOOL result = ::WriteFile(hFile, buf, pData->Length, &write, NULL);

// zrušení bufferu
	MemFree(buf);

#else

// uložení souboru
	DWORD write;
	BOOL result = ::WriteFile(hFile, pData->Data, pData->Length, &write, NULL);

#endif

// uzavření souboru
	::CloseHandle(hFile);

// při chybě zrušení souboru
	if (!result || (write != (DWORD)(pData->Length)))
	{
		::DeleteFile(jmeno);
		return false;
	}

// příznak - uloženo OK
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// příprava jména přechodného souboru

void CString::TempName()
{
 	TCHAR* pth = (TCHAR*)MemGet(256);
	TCHAR* fil = (TCHAR*)MemGet(512);
	pth[0] = 0;
	fil[0] = 0;
	::GetTempPath(255, pth);
	::GetTempFileName(pth, _T("PET"), 0, fil);
	*this = fil;
	MemFree(pth);
	MemFree(fil);
}


/////////////////////////////////////////////////////////////////////////////
// kopie do vlastního bufferu před modifikací

void CString::CopyWrite()
{
	STRINGDATA* data = pData;		// adresa starých dat
	long* refer = &(data->Refer);	// počet referencí

	if (*refer > 1)					// je nějaký jiný majitel?
	{
		NewBuffer(data->Length);	// vytvoření nového bufferu
		MemCopy(pData->Data, data->Data, data->Length*sizeof(TCHAR));

// odpojení starých dat - v multithread může nastat i zrušení
		if (LongDecrement(refer))
		{
#ifdef _MT
			MemFree(data);			// případné zrušení řetězce
#endif	// _MT
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
// poskytnutí znaku na pozici s kontrolou

TCHAR _fastcall CString::Get(const int index) const
{ 
	if (IsValid(index))
	{
		return pData->Data[index];
	}
	return 0;
}


//////////////////////////////////////////////////////////////////////////////
// nastavení znaku na pozici s kontrolou (zajistí přivlastnění bufferu)

void _fastcall CString::Set(const int index, const TCHAR chr)
{
	if (IsValid(index))
	{
		CopyWrite();
		pData->Data[index] = chr;
	}
}


//////////////////////////////////////////////////////////////////////////////
// načtení textu okna

void CString::GetWindowText(const HWND wnd)
{
	Detach();
	NewBuffer(::GetWindowTextLength(wnd));
	::GetWindowText(wnd, pData->Data, pData->Length+1);
}


//////////////////////////////////////////////////////////////////////////////
// nastavení textu okna

void _fastcall CString::SetWindowText(const HWND wnd) const
{
	::SetWindowText(wnd, pData->Data);
}


//////////////////////////////////////////////////////////////////////////////
// načtení textu dialogového prvku

void CString::GetDialogText(const HWND wnd, int id)
{
	HWND item = ::GetDlgItem(wnd, id);
	if (item)
	{
		Detach();
		NewBuffer(::GetWindowTextLength(item));
		::GetWindowText(item, pData->Data, pData->Length+1);
	}
	else
	{
		Empty();
	}
}


//////////////////////////////////////////////////////////////////////////////
// nastavení textu dialogového prvku

void _fastcall CString::SetDialogText(const HWND wnd, int id) const
{
	HWND item = ::GetDlgItem(wnd, id);
	if (item)
	{
		::SetWindowText(item, pData->Data);
	}
}


//////////////////////////////////////////////////////////////////////////////
// načtení aktivního adresáře

void CString::GetAktDir()
{
	TCHAR buf[_MAX_PATH];
	buf[0] = 0;
	int len = (int)::GetCurrentDirectory(_MAX_PATH, buf);
	if (len < 0) len = 0;
	if (len > _MAX_PATH) len = _MAX_PATH;
	
	Detach();
	NewBuffer(len);
	MemCopy(pData->Data, buf, len*sizeof(TCHAR));
}


//////////////////////////////////////////////////////////////////////////////
// korekce textu na jméno souboru (vypuštění zakázaných znaků, vrací opravenou pozici kurzoru)

int CString::FileName(int curs)
{
	for (int i = pData->Length-1; i >= 0; i--)
	{
		TCHAR znak = pData->Data[i];
		if ((BYTE)znak < 32) znak = 0;

		switch(znak)
		{
		case 0:
		case _T('*'):
		case _T('|'):
		case _T('\\'):
		case _T(':'):
		case _T('"'):
		case _T('<'):
		case _T('>'):
		case _T('?'):
		case _T('/'):
			Delete(i,1);
			if (curs > i) curs--;
		}
	}
	return curs;
}


//////////////////////////////////////////////////////////////////////////////
// korekce textu na jméno cesty (vypuštění zakázaných znaků, vrací opravenou pozici kurzoru)

int CString::PathName(int curs)
{
	for (int i = pData->Length-1; i >= 0; i--)
	{
		TCHAR znak = pData->Data[i];
		if ((BYTE)znak < 32) znak = 0;

		switch(znak)
		{
		case 0:
		case _T('*'):
		case _T('|'):
		case _T('"'):
		case _T('<'):
		case _T('>'):
		case _T('?'):
		case _T('/'):
			Delete(i,1);
			if (curs > i) curs--;
		}
	}
	return curs;
}


//////////////////////////////////////////////////////////////////////////////
// konverze čísla INT na text

void CString::Int(int num)
{
// příprava bufferu
	TCHAR buf[15];
	TCHAR* dst = buf+15;
	int len = 0;

// příprava znaménka
	bool sign = false;
	if (num < 0)
	{
		sign = true;
		num = -num;
	}

// konverze číslic
	do {
		dst--;
		*dst = (TCHAR)(((DWORD)num % 10) + '0');
		*(DWORD*)(&num) /= 10;
		len++;
	} while (num != 0);

// uložení znaménka
	if (sign)
	{
		dst--;
		*dst = '-';
		len++;
	}

// kopie do bufferu
	Detach();
	NewBuffer(len);
	MemCopy(pData->Data, dst, len*sizeof(TCHAR));
}


//////////////////////////////////////////////////////////////////////////////
// konverze textu na číslo INT

int Int(LPCTSTR txt)
{
// příprava proměnných
	DWORD result = 0;
	bool sign = false;
	TCHAR chr;

// načtení znaménka
	while ((chr = *txt) != 0)
	{
		if (chr == '-')
		{
			sign = !sign;
		}
		else
		{
			if ((chr != '+') && (chr != ' ') && (chr != 9))
			{
				break;
			}
		}
		txt++;
	}			
	
// načtení číslic
	while ((chr = *txt) != 0)
	{
		if ((chr >= '0') && (chr <= '9'))
		{
			if (result <= (DWORD)(0x7fffffff / 10))
			{
				result *= 10;
				result += (DWORD)(chr - '0');
			}
			else
			{
				result = 0x80000000;
			}

			if (result > 0x80000000)
			{
				result = 0x80000000;
			}
		}
		else
		{
			break;
		}
		txt++;
	}

// návrat s korekcí znaménka
	if (sign)
	{
		return -(int)result;
	}

	if (result >= 0x80000000)
	{
		return 0x7fffffff;
	}

	return result;
}	


//////////////////////////////////////////////////////////////////////////////
// přidání 2 číslic

void _fastcall CString::Add2Dig(char num)
{
// úschova indexu
	int len = pData->Length;

// přivlastnění bufferu pro zápis
	CopyWrite();			// přivlastnění bufferu

// zvýšení velikosti bufferu
	SizeBuffer(len+2);		// změna velikosti bufferu

// dekódování první číslice
	pData->Data[len] = (TCHAR)(num/10 + _T('0'));

// dekódování druhé číslice
	pData->Data[len+1] = (TCHAR)(num % 10 + _T('0'));
}


//////////////////////////////////////////////////////////////////////////////
// přidání 4 číslic

void _fastcall CString::Add4Dig(short num)
{
	Add2Dig((char)(num/100));
	Add2Dig((char)(num % 100));
}


//////////////////////////////////////////////////////////////////////////////
// přidání 2 číslic se zarovnáním mezerou

void _fastcall CString::Add2DigSpc(char num)
{
// dekódování číslic
	Add2Dig(num);

// adresa předposledního znaku
	TCHAR* data = pData->Data + pData->Length - 2;

// náhrada první nuly mezerou
	if (*data == _T('0')) *data = _T(' ');
}


//////////////////////////////////////////////////////////////////////////////
// přidání textu se známou délkou

void _fastcall CString::Add(LPCTSTR text, int num)
{
// kontrola délky textu
	if (num <= 0) return;

// přivlastnění bufferu pro zápis
	CopyWrite();

// úschova staré délky textu
	int len = pData->Length;

// zvýšení velikosti bufferu
	SizeBuffer(len + num);

// přidání textu
	MemCopy(pData->Data + len, text, num*sizeof(TCHAR));
}


//////////////////////////////////////////////////////////////////////////////
// konverze čísla DWORD na HEX (8 číslic)

void CString::Hex(DWORD num)
{
// příprava bufferu
	Detach();
	NewBuffer(8);
	TCHAR* dst = pData->Data + 8;

// dekódování číslic
	TCHAR chr;
	for (int i = 8; i != 0; i--)
	{
		dst--;
		chr = (TCHAR)(num & 0xf);
		if (chr <= 9)
		{
			*dst = (TCHAR)(chr + '0');
		}
		else
		{
			*dst = (TCHAR)(chr - 10 + 'A');
		}

		num >>= 4;
	}
}

//////////////////////////////////////////////////////////////////////////////
// konverze čísla DOUBLE na text

void CString::Double(double num)
{
// přetečení "1#INF"
	if (*(ULONGLONG*)(&num) == DOUBLE_INFINITY_VALUE)
	{
		Detach();
		NewBuffer(5);
		pData->Data[0] = '1';
		pData->Data[1] = '#';
		pData->Data[2] = 'I';
		pData->Data[3] = 'N';
		pData->Data[4] = 'F';
		return;
	}

// dekódování nuly
	if (num == 0)
	{
		Detach();
		NewBuffer(1);
		pData->Data[0] = '0';
		return;
	}

// příprava znaménka
	bool sign = false;
	if (num < 0)
	{
		sign = true;
		num = -num;
	}

// rozdělení čísla na exponent a mantisu
	int expI = 0;
	double mantD = num;

	if (mantD >= 1)
	{
		if (mantD >= 1e256) { mantD *= 1e-256; expI += 256; };
		if (mantD >= 1e128) { mantD *= 1e-128; expI += 128; };
		if (mantD >= 1e64)  { mantD *= 1e-64;  expI += 64;  };
		if (mantD >= 1e32)  { mantD *= 1e-32;  expI += 32;  };
		if (mantD >= 1e16)  { mantD *= 1e-16;  expI += 16;  };
		if (mantD >= 1e8)   { mantD *= 1e-8;   expI += 8;   };
		if (mantD >= 1e4)   { mantD *= 1e-4;   expI += 4;   };
		if (mantD >= 1e2)   { mantD *= 1e-2;   expI += 2;   };
		if (mantD >= 1e1)   { mantD *= 1e-1;   expI += 1;   };
	}
	else
	{
		if (mantD < 1e-256) { mantD *= 1e257;  expI -= 257; };
		if (mantD < 1e-128) { mantD *= 1e129;  expI -= 129; };
		if (mantD < 1e-64)  { mantD *= 1e65;   expI -= 65;  };
		if (mantD < 1e-32)  { mantD *= 1e33;   expI -= 33;  };
		if (mantD < 1e-16)  { mantD *= 1e17;   expI -= 17;  };
		if (mantD < 1e-8)   { mantD *= 1e9;    expI -= 9;   };
		if (mantD < 1e-4)   { mantD *= 1e5;    expI -= 5;   };
		if (mantD < 1e-2)   { mantD *= 1e3;    expI -= 3;   };
		if (mantD < 1e-1)   { mantD *= 1e2;    expI -= 2;   };
		if (mantD < 1e0)    { mantD *= 1e1;    expI -= 1;   };
	}

	mantD *= 1e7;
	mantD += 6e-8;

	if (mantD >= 1e8)
	{
		mantD /= 10;
		expI++;
	}

	int mantH = (int)(mantD);
	int mantL = (int)((mantD - mantH)*1e7);

// dekódování mantisy
	int i;
	TCHAR mantT[15];
	TCHAR* mantP = mantT+15;
	for (i = 7; i > 0; i--)
	{
		mantP--;
		*mantP = (TCHAR)((mantL % 10) + '0');
		mantL /= 10;
	}
	for (i = 8; i > 0; i--)
	{
		mantP--;
		*mantP = (TCHAR)((mantH % 10) + '0');
		mantH /= 10;
	}
	
// zjištění počtu platných číslic
	TCHAR* dst = mantT+15;
	int digits = 15;
	for (; digits > 1; digits--)
	{
		dst--;
		if (*dst != '0') break;
	}

// příprava znaménka do výchozího bufferu
	TCHAR buf[30];
	dst = buf;
	if (sign)
	{
		*dst = '-';
		dst++;
	}

// dekódování čísla bez exponentu, číslo >= 1
	if ((expI < 15) && (expI >= 0))
	{
		for (;;)
		{
			*dst = *mantP;
			dst++;
			mantP++;
			digits--;
			if ((expI <= 0) && (digits <= 0)) break;
			if (expI == 0)
			{
				*dst = '.';
				dst++;
			}
			expI--;
		}
	}
	else

// dekódování čísla bez exponentu, číslo < 1
	if ((expI < 0) && (expI >= -3))
	{
		*dst = '0';
		dst++;
		*dst = '.';
		dst++;

		expI++;
		for (;expI < 0; expI++)
		{
			*dst = '0';
			dst++;
		}

		for (;digits > 0; digits--)
		{
			*dst = *mantP;
			dst++;
			mantP++;
		}
	}
	else

// dekódování čísla s exponentem
	{
		*dst = *mantP;
		dst++;
		mantP++;
		digits--;

		if (digits > 0)
		{
			*dst = '.';
			dst++;

			for (; digits > 0; digits--)
			{
				*dst = *mantP;
				dst++;
				mantP++;
			}
		}

		*dst = 'e';
		dst++;

		if (expI < 0)
		{
			*dst = '-';
			dst++;
			expI = -expI;
		}

		if (expI > 99)
		{
			*dst = (TCHAR)(expI/100 + '0');
			expI = expI % 100;
			dst++;

			*dst = (TCHAR)(expI/10 + '0');
			expI = expI % 10;
			dst++;
		}
		else
			if (expI > 9)
			{
				*dst = (TCHAR)(expI/10 + '0');
				expI = expI % 10;
				dst++;
			}

		*dst = (TCHAR)(expI + '0');
		dst++;
	}

// uložení čísla do bufferu
	int len = dst - buf;
	Detach();
	NewBuffer(len);
	MemCopy(pData->Data, buf, len*sizeof(TCHAR));
}


//////////////////////////////////////////////////////////////////////////////
// konverze textu na číslo DOUBLE

double Double(LPCTSTR txt)
{
// příprava proměnných
	double result = 0;			// výsledek
	int expN = 0;				// exponent
	double zlomek = 1;			// zlomek desetinné části
	bool sign = false;			// příznak záporného znaménka
	bool signE = false;			// znaménko exponentu
	TCHAR chr = *txt;			// načtený znak

// načtení znaménka
	while (chr != 0)
	{
		if (chr == '-')
		{
			sign = !sign;
		}
		else
		{
			if ((chr != '+') && (chr != ' ') && (chr != 9))
			{
				break;
			}
		}
		txt++;
		chr = *txt;
	}			
	
// načtení číslic celé části
	while (chr != 0)
	{
		if ((chr >= '0') && (chr <= '9'))
		{
			result *= 10;
			result += chr - '0';
		}
		else
		{
			break;
		}
		txt++;
		chr = *txt;
	}

// načtení číslic desetinné části
	if ((chr == '.') || (chr == ','))
	{
		txt++;
		chr = *txt;

		while (chr != 0)
		{
			if ((chr >= '0') && (chr <= '9'))
			{
				zlomek *= 10;
				result += (chr - '0')/zlomek;
			}
			else
			{
				break;
			}
			txt++;
			chr = *txt;
		}
	}

// vypuštění mezer
	while ((chr == ' ') || (chr == 9))
	{
		txt++;
		chr = *txt;
	}

// test, zda je exponent
	if ((chr == 'E') || (chr == 'e'))
	{
		txt++;
		chr = *txt;

// vypuštění mezer a určení znaménka
		while ((chr == ' ') || (chr == 9) || (chr == '+') || (chr == '-'))
		{
			if (chr == '-')
			{
				signE = !signE;
			}
			txt++;
			chr = *txt;
		}

// načtení číslic exponentu
		while (chr != 0)
		{
			if ((chr >= '0') && (chr <= '9'))
			{
				expN *= 10;
				expN += chr - '0';
			}
			else
			{
				break;
			}
			txt++;
			chr = *txt;
		}

// vynásobení čísla exponentem
		if (expN != 0)
		{
			if (signE)
			{
				if (expN >= 256) { result *= 1e-256; expN -= 256; };
				if (expN >= 128) { result *= 1e-128; expN -= 128; };
				if (expN >=  64) { result *=  1e-64; expN -=  64; };
				if (expN >=  32) { result *=  1e-32; expN -=  32; };
				if (expN >=  16) { result *=  1e-16; expN -=  16; };
				if (expN >=   8) { result *=   1e-8; expN -=   8; };
				if (expN >=   4) { result *=   1e-4; expN -=   4; };
				if (expN >=   2) { result *=   1e-2; expN -=   2; };
				if (expN >=   1) { result *=   1e-1;              };
			}
			else
			{
				if (expN >= 256) { result *= 1e256; expN -= 256; };
				if (expN >= 128) { result *= 1e128; expN -= 128; };
				if (expN >=  64) { result *=  1e64; expN -=  64; };
				if (expN >=  32) { result *=  1e32; expN -=  32; };
				if (expN >=  16) { result *=  1e16; expN -=  16; };
				if (expN >=   8) { result *=   1e8; expN -=   8; };
				if (expN >=   4) { result *=   1e4; expN -=   4; };
				if (expN >=   2) { result *=   1e2; expN -=   2; };
				if (expN >=   1) { result *=   1e1;              };
			}
		}
	}

// příznak přetečení
	if (chr == '#')
	{
		if ((txt[1] == 'I') &&
			(txt[2] == 'N') &&
			(txt[3] == 'F') &&
			((result == 1) || (result == 0)))
		{
			*(ULONGLONG*)(&result) = DOUBLE_INFINITY_VALUE;
			return result;
		}
	}

// návrat s korekcí znaménka
	if (sign)
	{
		return -result;
	}
	return result;
}	


/////////////////////////////////////////////////////////////////////////////
// operátor přiřazení

const CString& _fastcall CString::operator= (const CString& str)
{
	Detach();				// zrušení starých dat
	Attach(str.pData);		// přiřazení nových dat
	return *this;
}

const CString& _fastcall CString::operator= (LPCTSTR txt)
{
	int len = 0;
	if (txt) len = lstrlen(txt);
	Detach();
	NewBuffer(len);	
	MemCopy(pData->Data, txt, len*sizeof(TCHAR));
	return *this;
}

#ifdef _UNICODE

const CString& _fastcall CString::operator= (LPCSTR txt)
{
	int len = 0;
	if (txt) len = lstrlenA(txt);
	Detach();
	NewBuffer(len);
	::MultiByteToWideChar(CP_ACP, 0, txt, len, pData->Data, len+1);
	return *this;
}

#else //_UNICODE

const CString& _fastcall CString::operator= (LPCWSTR txt)
{
	int len = 0;
	if (txt) len = lstrlenW(txt);
	Detach();
	NewBuffer(len);
	::WideCharToMultiByte(CP_ACP, 0, txt, len, pData->Data, len+1, NULL, NULL);
	return *this;
}

#endif //_UNICODE

const CString& _fastcall CString::operator= (const TCHAR chr)
{
	Detach();
	NewBuffer(1);
	pData->Data[0] = chr;
	return *this;
}


/////////////////////////////////////////////////////////////////////////////
// operátor přičtení

const CString& _fastcall CString::operator+= (const CString& str)
{
	int len1 = pData->Length;
	int len2 = str.pData->Length;
	CopyWrite();			// přivlastnění bufferu
	SizeBuffer(len1+len2);	// změna velikosti bufferu
	MemCopy(pData->Data + len1, str.pData->Data, len2*sizeof(TCHAR));
	return *this;
}

const CString& _fastcall CString::operator+= (LPCTSTR txt)
{
	int len1 = pData->Length;
	int len2 = 0;
	if (txt) len2 = lstrlen(txt);
	CopyWrite();			// přivlastnění bufferu
	SizeBuffer(len1+len2);	// změna velikosti bufferu
	MemCopy(pData->Data + len1, txt, len2*sizeof(TCHAR));
	return *this;
}

const CString& _fastcall CString::operator+= (const TCHAR chr)
{
	int len = pData->Length;
	CopyWrite();			// přivlastnění bufferu
	SizeBuffer(len+1);		// změna velikosti bufferu
	pData->Data[len] = chr;
	return *this;
}


//////////////////////////////////////////////////////////////////////////////
// operátory součtu

CString _fastcall operator+ (const CString& str1, const CString& str2)
{
	return CString(str1, str2);
}

CString _fastcall operator+ (LPCTSTR txt, const CString& str)
{
	return CString(txt, str);
}

CString _fastcall operator+ (const CString& str, LPCTSTR txt)
{
	return CString(str, txt);
}

CString _fastcall operator+ (const TCHAR chr, const CString& str)
{
	return CString(chr, str);
}

CString _fastcall operator+ (const CString& str, const TCHAR chr)
{
	return CString(str, chr);
}


/////////////////////////////////////////////////////////////////////////////
// operátor porovnání

bool _fastcall operator==(const CString& str1, const CString& str2)
{ 
	int delka = str1.Length();
	return ((delka == str2.Length()) &&
		MemCompare(str1.DataData(), str2.DataData(), delka*sizeof(TCHAR)));
};

bool _fastcall operator==(LPCTSTR txt, const CString& str)
{ 
	int delka = 0;
	if (txt) delka = lstrlen(txt);
	return ((delka == str.Length()) &&
		MemCompare(str.DataData(), txt, delka*sizeof(TCHAR)));
};

bool _fastcall operator==(const CString& str, LPCTSTR txt)
{ 
	int delka = 0;
	if (txt) delka = lstrlen(txt);
	return ((delka == str.Length()) &&
		MemCompare(str.DataData(), txt, delka*sizeof(TCHAR)));
};

bool _fastcall operator==(const CString& str, const TCHAR chr)
{ 
	return ((str.Length() == 1) && (str[0] == chr));
};

bool _fastcall operator==(const TCHAR chr, const CString& str)
{ 
	return ((str.Length() == 1) && (str[0] == chr));
};

bool _fastcall operator!=(const CString& str1, const CString& str2)
{ 
	int delka = str1.Length();
	return ((delka != str2.Length()) || !MemCompare(str1.DataData(),
			str2.DataData(), delka*sizeof(TCHAR)));
};

bool _fastcall operator!=(LPCTSTR txt, const CString& str)
{ 
	int delka = 0;
	if (txt) delka = lstrlen(txt);
	return ((delka != str.Length()) || !MemCompare(str.DataData(),
			txt, delka*sizeof(TCHAR)));
};

bool _fastcall operator!=(const CString& str, LPCTSTR txt)
{ 
	int delka = 0;
	if (txt) delka = lstrlen(txt);
	return ((delka != str.Length()) || !MemCompare(str.DataData(),
			txt, delka*sizeof(TCHAR)));
};

bool _fastcall operator!=(const CString& str, const TCHAR chr)
{ 
	return ((str.Length() != 1) || (str[0] != chr));
};

bool _fastcall operator!=(const TCHAR chr, const CString& str)
{ 
	return ((str.Length() != 1) || (str[0] != chr));
};

bool _fastcall operator< (const CString& str1, const CString& str2)
{
	return (::CompareString(
				LOCALE_USER_DEFAULT, 
				0, 
				str1.DataData(),
				str1.Length(),
				str2.DataData(),
				str2.Length()) < 2);

//	return (::lstrcmpi(str1.DataData(), str2.DataData()) < 0);
}

bool _fastcall operator<=(const CString& str1, const CString& str2)
{
	return (::CompareString(
				LOCALE_USER_DEFAULT, 
				0, 
				str1.DataData(),
				str1.Length(),
				str2.DataData(),
				str2.Length()) <= 2);

//	return (::lstrcmpi(str1.DataData(), str2.DataData()) <= 0);
}

bool _fastcall operator> (const CString& str1, const CString& str2)
{
	return (::CompareString(
				LOCALE_USER_DEFAULT, 
				0, 
				str1.DataData(),
				str1.Length(),
				str2.DataData(),
				str2.Length()) > 2);

//	return (::lstrcmpi(str1.DataData(), str2.DataData()) > 0);
}

bool _fastcall operator>=(const CString& str1, const CString& str2)
{
	return (::CompareString(
				LOCALE_USER_DEFAULT, 
				0, 
				str1.DataData(),
				str1.Length(),
				str2.DataData(),
				str2.Length()) >= 2);

//	return (::lstrcmpi(str1.DataData(), str2.DataData()) >= 0);
}


/***************************************************************************\
*																			*
*								Buffer textů								*
*																			*
\***************************************************************************/

////////////////////////////////////////////////////////////////////
// konstruktor a destruktor

CBufText::CBufText()
{
	m_Data = NULL;			// není buffer dat
	m_Num = 0;				// není žádná platná položka
	m_Max = 0;				// není buffer položek
}

CBufText::~CBufText()
{
	DelAll();				// zrušení všech položek
}


////////////////////////////////////////////////////////////////////
// statický konstruktor a destruktor

void CBufText::Init()
{
	m_Data = NULL;			// není buffer dat
	m_Num = 0;				// není žádná platná položka
	m_Max = 0;				// není buffer položek
}

void CBufText::Term()
{
	DelAll();				// zrušení všech položek
}


////////////////////////////////////////////////////////////////////
// vytvoření nových dat

void CBufText::NewData()
{
	m_Max += 0x400;			// zvýšení počtu položek (o 4 KB)
	MemBuf(m_Data, m_Max);	// zvýšení velikosti bufferu
};


////////////////////////////////////////////////////////////////////
// zrušení všech položek v bufferu

void CBufText::DelAll()
{
	Del(m_Num);					// zrušení položek
	MemBuf(m_Data, 0);			// zrušení bufferu dat
	m_Max = 0;					// není žádná položka v bufferu
}

////////////////////////////////////////////////////////////////////
// poskytnutí položky (s kontrolou platnosti indexu)

const CString& _fastcall CBufText::Get(const int index) const
{
	if (IsValid(index))			// je index platný?
	{
		return m_Data[index];	// položka na daném indexu
	}
	return EmptyString;		// pro neplatný index vrátí prázdný řetězec
}

////////////////////////////////////////////////////////////////////
// nastavení položky (s kontrolou platnosti indexu)

void _fastcall CBufText::Set(const int index, const CString& data)
{
	if (IsValid(index))			// je index platný?
	{
		m_Data[index] = data;
	}
}


/////////////////////////////////////////////////////////////////////////////
// vyprázdnění položky (bez jejího zrušení - jen pro uvolnění dat)

void _fastcall CBufText::Empty(const int index)
{
	if (IsValid(index))					// je index platný?
	{
		m_Data[index].Empty();
	}
}


////////////////////////////////////////////////////////////////////
// zrušení položek z konce bufferu

void _fastcall CBufText::Del(int num)
{
	if (num > m_Num) num = m_Num;

	CString* str = m_Data + m_Num - 1;
	m_Num -= num;

	for (; num > 0; num--)
	{
		str->Term();
		str--;
	}
}


////////////////////////////////////////////////////////////////////
// vytvoření položky (vrací index položky)

int CBufText::New()
{
	int result = NewItem();		// vytvoření nové položky
	m_Data[result].Init();		// inicializace položky
	return result;
}


////////////////////////////////////////////////////////////////////
// přidání položky (vrací index položky)

int _fastcall CBufText::Add(const CString& data)
{
	int result = NewItem();		// vytvoření nové položky
	m_Data[result].Init(data.Data());	// inicializace položky
	return result;
}


int _fastcall CBufText::Add(const WCHAR* data, int len)
{
	int result = NewItem();		// vytvoření nové položky
	m_Data[result].Init(data, len);	// inicializace položky
	return result;
}


int _fastcall CBufText::Add(const char* data, int len)
{
	int result = NewItem();		// vytvoření nové položky
	m_Data[result].Init(data, len);	// inicializace položky
	return result;
}


////////////////////////////////////////////////////////////////////
// duplikace položky (s kontrolou platnosti indexu, vrací index první položky)

int _fastcall CBufText::Dup(const int index)
{
	int result = NewItem();		// vytvoření nové položky

	if (IsValid(index))			// je index platný?
	{
		m_Data[result].Init(m_Data[index].Data());	// kopie položky
	}
	else
	{
		m_Data[result].Init();		// inicializace neplatné položky
	}
	return result;
}

int _fastcall CBufText::Dup(const int index, int num)
{
	int result = NewItem();		// vytvoření nové položky

	if (IsValid(index))					// je index platný?
	{
		STRINGDATA* data = m_Data[index].Data();

		m_Data[result].Init(data);		// kopie položky

		for (num--; num > 0; num--)
		{
			m_Data[NewItem()].Init(data);
		}
	}
	else
	{
		m_Data[result].Init();		// inicializace neplatné položky

		for (num--; num > 0; num--)
		{
			m_Data[NewItem()].Init();
		}
	}
	return result;
}


////////////////////////////////////////////////////////////////////
// načtení textu z resource (vrací index položky)

int _fastcall CBufText::Load(const int nID)
{
	int result = NewItem();		// vytvoření nové položky
	m_Data[result].Init();		// inicializace položky
	m_Data[result].Load(nID);	// načtení textu z resource
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// operátor přiřazení

const CBufText& CBufText::operator= (const CBufText& src)
{
	Del(m_Num);					// zrušení starých dat

	int index = 0;				// index načítané položky
	int i = src.m_Num;			// velikost zdrojového bufferu

	for (; i > 0; i--)			// pro všechny položky v bufferu
	{
		Add(src[index]);	// kopie položky
		index++;				// inkrementace čtecího indexu
	}
	ASSERT(m_Num == src.m_Num);
	return *this;
}
