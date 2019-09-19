
/***************************************************************************\
*																			*
*								Textové řetězce								*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// struktura položky dat řetězce - 8 bajtů + data

typedef struct STRINGDATA_
{
	long	Refer;					// (4) čítač referencí na textový řetězec
	long	Length;					// (4) délka řetězce ve znacích
	long	Orig;					// (4) index originálního textu v programu (-1 není)
	TCHAR	Data[1];				// (x*1 nebo x*2) textová data
} STRINGDATA;

#define SIZEOFSTRINGDATA	(3*sizeof(long)) // velikost záhlaví datové položky (bez dat)

extern STRINGDATA EmptyStringData;	// data prázdného řetězce


/////////////////////////////////////////////////////////////////////////////
// textový řetězec

class CString
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné - pouze ukazatel na data
	STRINGDATA*		pData;			// ukazatel na data řetězce

// připojení dat
	inline void Attach(STRINGDATA* data)
	{
		ASSERT(data);
		pData = data;
		LongIncrement(&(data->Refer));
	}

// odpojení (a zrušení) dat
	inline void Detach()
	{
		ASSERT(pData);
		ASSERT(pData->Refer > 0);

		if (LongDecrement(&(pData->Refer)))
		{
			MemFree(pData);
#ifdef _DEBUG
			pData = NULL;
#endif
		}
	}

// vytvoření nového bufferu (délka zadána ve znacích) - starý buffer musí být odpojen!
	inline void NewBuffer(int length)
	{
		ASSERT(length >= 0);
		if (length < 0) length = 0;
		STRINGDATA* data = (STRINGDATA*)MemGet(
				(length+1)*sizeof(TCHAR) + SIZEOFSTRINGDATA);
		pData = data;					// adresa dat
		data->Refer = 1;				// počet referencí
		data->Length = length;			// délka
		data->Orig = -1;				// index originálního textu
		data->Data[length] = 0;			// označení konce textu
	}

// změna velikosti bufferu (délka zadána ve znacích) - buffer musí být přivlastněn!
	inline void SizeBuffer(int length)
	{
		ASSERT(pData->Refer == 1);
		ASSERT(length >= 0);
		if (length < 0) length = 0;
		STRINGDATA* data = (STRINGDATA*)MemSize(pData, 
				(length+1)*sizeof(TCHAR) + SIZEOFSTRINGDATA);
		pData = data;					// adresa dat
		data->Length = length;			// délka
		data->Orig = -1;				// index originálního textu
		data->Data[length] = 0;			// označení konce textu
	}

// slučovací konstruktory (určeno pro operátor +)
	inline CString(const CString& str1, const CString& str2) 
	{ 
		int len1 = str1.pData->Length;
		int len2 = str2.pData->Length;
		NewBuffer(len1+len2);
		MemCopy(pData->Data, str1.pData->Data, len1*sizeof(TCHAR));
		MemCopy(pData->Data + len1, str2.pData->Data, len2*sizeof(TCHAR));
	}

	inline CString(LPCTSTR txt, const CString& str) 
	{ 
		int len1 = 0;
		if (txt) len1 = lstrlen(txt);
		int len2 = str.pData->Length;
		NewBuffer(len1+len2);
		MemCopy(pData->Data, txt, len1*sizeof(TCHAR));
		MemCopy(pData->Data + len1, str.pData->Data, len2*sizeof(TCHAR));
	}

	inline CString(const CString& str, LPCTSTR txt) 
	{ 
		int len1 = str.pData->Length;
		int len2 = 0;
		if (txt) len2 = lstrlen(txt);
		NewBuffer(len1+len2);
		MemCopy(pData->Data, str.pData->Data, len1*sizeof(TCHAR));
		MemCopy(pData->Data + len1, txt, len2*sizeof(TCHAR));
	}

	inline CString(const TCHAR chr, const CString& str) 
	{ 
		int len = str.pData->Length;
		NewBuffer(1+len);
		pData->Data[0] = chr;
		MemCopy(pData->Data + 1, str.pData->Data, len*sizeof(TCHAR));
	}

	inline CString(const CString& str, const TCHAR chr) 
	{ 
		int len = str.pData->Length;
		NewBuffer(len+1);
		MemCopy(pData->Data, str.pData->Data, len*sizeof(TCHAR));
		pData->Data[len] = chr;
	}

// ---------------------------- veřejné funkce ------------------------------

public:

// konstruktor a destruktor
	CString();
	CString(const CString& src);
	CString(const TCHAR chr);
	CString(LPCTSTR text, const int length = -1);
#ifdef _UNICODE
	CString(LPCSTR text, const int length = -1);
#else //_UNICODE
	CString(LPCWSTR text, const int length = -1);
#endif //_UNICODE
	~CString();

// statický konstruktor a destruktor
	void Init();
	void Init(STRINGDATA* data);
	void Init(const CString& src);
	void Init(LPCTSTR text, const int length = -1);
#ifdef _UNICODE
	void Init(LPCSTR text, const int length = -1);
#else //_UNICODE
	void Init(LPCWSTR text, const int length = -1);
#endif //_UNICODE
	void Term();

// poskytnutí adresy dat
	inline STRINGDATA* Data() const { return pData; };
	inline TCHAR* DataData() const { return pData->Data; };

// poskytnutí/nastavení délky řetězce
	inline int Length() const { return pData->Length; }
	void Length(int len);

// poskytnutí/nastavení indexu originálního textu (-1=není)
	inline int Orig() const { return pData->Orig; }
	inline void Orig(int orig) { pData->Orig = orig; }

// kontrola, zda je řetězec prázdný
	inline BOOL IsEmpty() const { return pData->Length == 0; };
	inline BOOL IsNotEmpty() const { return pData->Length != 0; };

// zápis do bufferu (bez koncové nuly!)
	void Write(TCHAR* buf) const;
#ifdef _UNICODE
	void Write(LPSTR buf) const;
#else
	void Write(LPWSTR buf) const;
#endif

// ------- hledání

// hledání (-1 = nenalezeno)
	int _fastcall Find(const CString& str) const;
	int _fastcall Find(LPCTSTR txt) const;
	int _fastcall Find(const TCHAR chr) const;
	
// hledání znaku od konce řetězce zpět (-1 = nenalezeno)
	int _fastcall RevFind(const TCHAR chr) const;

// počet řádků textu
	int LineNum() const;
	CString _fastcall GetLine(int radek) const;

// blok textu (-1=konec řádku nebo textu)
	CString GetBlock(int begline, int begpos, int endline, int endpos) const;

// -------- modifikace textu

// vyprázdnění řetězce (uvolnění dat)
	void Empty();

// konverze malá/velká písmena
	void UpperCase();
	void LowerCase();

// levá část řetězce
	CString _fastcall Left(int count) const;

// pravá část řetězce
	CString _fastcall Right(int count) const;

// střední část řetězce
	CString _fastcall Mid(int first, int count = 0x7fffffff) const;

// zrušení textu z konce řetězce
	void Delete(int first, int count = 0x7fffffff);

// odstranění mezer ze začátku/konce řetězce
	void TrimLeft();
	void TrimRight();

// korekce výskytu nuly v textu (provádí se ve společném bufferu všech proměnných)
	void KorigNul();

// nastavení čísla jména objektu (1, 2, ...)
	void _fastcall SetNumObj(const int num);

// načtení textu z resource
	void Load(const int nID);

// načtení ze souboru formátu TXT (false=chyba)
//	bool LoadFile(CString jmeno);

// uložení textu do souboru formátu TXT (false=chyba)
//	bool SaveFile(CString jmeno) const;

// ------- přístup ke znakům v bufferu

// kopie do vlastního bufferu před modifikací
	void CopyWrite();

// kontrola, zda je index znaku platný
	inline BOOL IsValid(const int index) const
		{ return ((DWORD)index < (DWORD)pData->Length); };

	inline BOOL IsNotValid(const int index) const
		{ return ((DWORD)index >= (DWORD)pData->Length); };

// poskytnutí přístupu ke znaku na pozici (bez kontroly a bez přivlastnění bufferu!)
	inline TCHAR& operator[] (int index)
		{ ASSERT(IsValid(index)); return pData->Data[index]; }

	inline const TCHAR& operator[] (int index) const
		{ ASSERT(IsValid(index)); return pData->Data[index]; }

	inline TCHAR& At(int index)
		{ ASSERT(IsValid(index)); return pData->Data[index]; }

	inline const TCHAR& At(int index) const
		{ ASSERT(IsValid(index)); return pData->Data[index]; }

// poskytnutí znaku na pozici s kontrolou
	TCHAR _fastcall Get(const int index) const;

// nastavení znaku na pozici s kontrolou (zajistí přivlastnění bufferu)
	void _fastcall Set(const int index, const TCHAR chr);

// poskytnutí posledního znaku řetězce (pro prázdný řetězec vrací 0)
	TCHAR _fastcall LastChar() const;

// ------- text oken

// načtení textu okna
	void GetWindowText(const HWND wnd);

// nastavení textu okna
	void _fastcall SetWindowText(const HWND wnd) const;

// načtení textu dialogového prvku
	void GetDialogText(const HWND wnd, int id);

// nastavení textu dialogového prvku
	void _fastcall SetDialogText(const HWND wnd, int id) const;

// -------- adresáře a soubory

// načtení aktivního adresáře
	void GetAktDir();

// korekce textu na jméno souboru (vypuštění zakázaných znaků, 
//	vrací opravenou pozici kurzoru)
	int FileName(int curs);

// korekce textu na jméno cesty (vypuštění zakázaných znaků, 
//	vrací opravenou pozici kurzoru)
	int PathName(int curs);

// jméno přechodného souboru
	void TempName();


// -------- konverze čísel

// konverze čísla INT na text
	void Int(int num);

// konverze textu na číslo INT
	friend int Int(LPCTSTR text);

// přidání 2 číslic
	void _fastcall Add2Dig(char num);

// přidání 4 číslic
	void _fastcall Add4Dig(short num);

// přidání 2 číslic se zarovnáním mezerou
	void _fastcall Add2DigSpc(char num);

// přidání textu se známou délkou
	void _fastcall Add(LPCTSTR text, int num);
#ifdef _UNICODE
	void _fastcall Add(char* text, int num);
#else
	void _fastcall Add(WCHAR* text, int num);
#endif

// konverze čísla DWORD na HEX (8 číslic)
	void Hex(DWORD num);

// konverze čísla DOUBLE na text
	void Double(double num);

// konverze textu na číslo DOUBLE
	friend double Double(LPCTSTR txt);

// -------- operátory

// konverze na řetězec LPCTSTR
	inline operator LPCTSTR() const { return pData->Data; }

// operátory přiřazení
	const CString& _fastcall operator= (const CString& str);
	const CString& _fastcall operator= (LPCTSTR txt);
#ifdef _UNICODE
	const CString& _fastcall operator= (LPCSTR txt);
#else //_UNICODE
	const CString& _fastcall operator= (LPCWSTR txt);
#endif //_UNICODE
	const CString& _fastcall operator= (const TCHAR chr);

// operátory přičtení
	const CString& _fastcall operator+=(const CString& str);
	const CString& _fastcall operator+=(LPCTSTR txt);
	const CString& _fastcall operator+=(const TCHAR chr);

// operátory součtu
	friend CString _fastcall operator+ (const CString& str1, const CString& str2);
	friend CString _fastcall operator+ (LPCTSTR txt, const CString& str);
	friend CString _fastcall operator+ (const CString& str, LPCTSTR txt);
	friend CString _fastcall operator+ (const TCHAR chr, const CString& str);
	friend CString _fastcall operator+ (const CString& str, const TCHAR chr);

// operátory porovnání
	friend bool _fastcall operator==(const CString& str1, const CString& str2);
	friend bool _fastcall operator==(LPCTSTR txt, const CString& str);
	friend bool _fastcall operator==(const CString& str, LPCTSTR txt);
	friend bool _fastcall operator==(const CString& str, const TCHAR chr);
	friend bool _fastcall operator==(const TCHAR chr, const CString& str);
	friend bool _fastcall operator!=(const CString& str1, const CString& str2);
	friend bool _fastcall operator!=(LPCTSTR txt, const CString& str);
	friend bool _fastcall operator!=(const CString& str, LPCTSTR txt);
	friend bool _fastcall operator!=(const CString& str, const TCHAR chr);
	friend bool _fastcall operator!=(const TCHAR chr, const CString& str);
	friend bool _fastcall operator< (const CString& str1, const CString& str2);
	friend bool _fastcall operator<=(const CString& str1, const CString& str2);
	friend bool _fastcall operator> (const CString& str1, const CString& str2);
	friend bool _fastcall operator>=(const CString& str1, const CString& str2);
};


/***************************************************************************\
*																			*
*								Buffer textů								*
*																			*
\***************************************************************************/

class CBufText
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné
	CString*	m_Data;		// ukazatel na data
	int			m_Num;		// počet platných položek v bufferu
	int			m_Max;		// velikost bufferu (položek)

// vytvoření nové položky
	inline int NewItem()
	{
		int i = m_Num;				// počet položek
		if (i >= m_Max)				// není další položka?
		{
			NewData();				// vytvoření nových dat
		}

		m_Num = i + 1;
		return i;
	};

// vytvoření nových dat (odděleno kvůli lepší optimalizaci)
	void NewData();

// ---------------------------- veřejné funkce ------------------------------

public:

// konstruktor a destruktor
	CBufText();
	~CBufText();

// statický konstruktor a destruktor
	void Init();
	void Term();

// zrušení všech položek v bufferu
	void DelAll();

// poskytnutí bufferu dat
	inline CString* Data() const { return m_Data; };

// poskytnutí počtu platných položek v bufferu
	inline int Num() const { return m_Num; };

// poskytnutí velikosti bufferu (včetně zrušených položek)
	inline int Max() const { return m_Max; };

// kontrola platnosti položky
	inline BOOL IsValid(const int index) const
		{ return ((DWORD)index < (DWORD)m_Num); };

	inline BOOL IsNotValid(const int index) const
		{ return ((DWORD)index >= (DWORD)m_Num); };

// poskytnutí přístupu k položce (bez kontroly indexu)
	inline CString& operator[] (const int index)
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline const CString& operator[] (const int index) const
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline CString& At(const int index)
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline const CString& At(const int index) const
		{ ASSERT(IsValid(index)); return m_Data[index]; }

// poskytnutí položky (s kontrolou platnosti indexu)
	const CString& _fastcall Get(const int index) const;

// nastavení položky (s kontrolou platnosti indexu)
	void _fastcall Set(const int index, const CString& data);

// vyprázdnění položky (bez jejího zrušení - jen pro uvolnění dat)
	void _fastcall Empty(const int index);

// zrušení položek z konce bufferu
	void _fastcall Del(int num);

// vytvoření prázdné položky (vrací index položky)
	int New();

// přidání položky (vrací index položky)
	int _fastcall Add(const CString& data);
	int _fastcall Add(const WCHAR* data, const int len = -1);
	int _fastcall Add(const char* data, const int len = -1);

// duplikace položky (s kontrolou platnosti indexu, vrací index první položky)
	int _fastcall Dup(const int index);
	int _fastcall Dup(const int index, int num);

// načtení textu z resource (vrací index položky)
	int _fastcall Load(const int nID);

// operátor přiřazení
	const CBufText& operator= (const CBufText& src);
};

