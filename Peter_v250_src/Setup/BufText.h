
/***************************************************************************\
*																			*
*								Textové řetězce								*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// struktura položky dat řetězce (8 bajtů + data + koncová 0)

typedef struct TEXTDATA_
{
	long	Refer;					// (4) čítač referencí na textový řetězec
	long	Length;					// (4) délka řetězce ve znacích (bez koncové 0)
	TCHAR	Data[1];				// (x*1 nebo x*2) textová data + koncová 0
} TEXTDATA;

#define SIZEOFTEXTDATA	(2*sizeof(long)) // velikost záhlaví datové položky (bez dat)

extern TEXTDATA EmptyTextData;	// data prázdného řetězce
extern	const CText	EmptyText;	// prázdný řetězec

/////////////////////////////////////////////////////////////////////////////
// statické funkce

// vytvoření dat textu - délka zadána ve znacích (při chybě paměti vrací NULL)
TEXTDATA* _fastcall NewTextData(int length);

// zrušení dat textu (odděleno kvůli lepší optimalizaci)
void _fastcall DelTextData(TEXTDATA* data);


/////////////////////////////////////////////////////////////////////////////
// textový řetězec

class CText
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné
	TEXTDATA*		pData;			// ukazatel na data textu

// připojení dat textu
	inline void attach(TEXTDATA* data)
	{
		ASSERT(data != NULL);

		pData = data;
		LongIncrement(&(data->Refer));
	}

// odpojení (a zrušení) dat textu
	inline void detach(TEXTDATA* data)
	{
		ASSERT(data != NULL);
		ASSERT(data->Refer > 0);

		if (LongDecrement(&(data->Refer)))
		{
			DelTextData(data);
		}
	}

// změna velikosti bufferu (délka zadána ve znacích) - zajistí přivlastnění bufferu
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool SizeBuffer(int length);

// ---------------------------- veřejné funkce ------------------------------

public:

// konstruktor a destruktor
	CText();
	CText(const CText& src);
	CText(TEXTDATA* data);
	CText(const TCHAR chr);					// při chybě paměti bude řetězec prázdný
	CText(LPCTSTR text);						// při chybě paměti bude řetězec prázdný
	CText(LPCTSTR text, const int length);	// při chybě paměti bude řetězec prázdný
#ifdef _UNICODE
	CText(LPCSTR text);						// při chybě paměti bude řetězec prázdný
	CText(LPCSTR text, const int length);		// při chybě paměti bude řetězec prázdný
#else //_UNICODE
	CText(LPCWSTR text);						// při chybě paměti bude řetězec prázdný
	CText(LPCWSTR text, const int length);	// při chybě paměti bude řetězec prázdný
#endif //_UNICODE
	~CText();

// slučovací konstruktury
	CText(const CText& str1, const CText& str2);	// při chybě paměti bude řetězec prázdný
	CText(LPCTSTR txt, const CText& str);			// při chybě paměti bude řetězec prázdný
	CText(const CText& str, LPCTSTR txt);			// při chybě paměti bude řetězec prázdný
	CText(const TCHAR chr, const CText& str);		// při chybě paměti bude řetězec prázdný
	CText(const CText& str, const TCHAR chr);		// při chybě paměti bude řetězec prázdný

// statický konstruktor a destruktor
	void Init();
	void _fastcall Init(const CText& src);
	void _fastcall Init(TEXTDATA* data);
	bool _fastcall Init(const TCHAR chr);		// při chybě paměti vrátí FALSE, text není platný
	bool _fastcall Init(LPCTSTR text);			// při chybě paměti vrátí FALSE, text není platný
	bool _fastcall Init(LPCTSTR text, const int length); // při chybě paměti vrátí FALSE, text není platný
#ifdef _UNICODE
	bool _fastcall Init(LPCSTR text);			// při chybě paměti vrátí FALSE, text není platný
	bool _fastcall Init(LPCSTR text, const int length); // při chybě paměti vrátí FALSE, text není platný
#else //_UNICODE
	bool _fastcall Init(LPCWSTR text);			// při chybě paměti vrátí FALSE, text není platný
	bool _fastcall Init(LPCWSTR text, const int length); // při chybě paměti vrátí FALSE, text není platný
#endif //_UNICODE
	void Term();

// poskytnutí adresy dat
	inline TEXTDATA* Data() const { return pData; };
	inline TCHAR* DataData() const { return pData->Data; };

// poskytnutí délky řetězce
	inline int Length() const { return pData->Length; }

// kontrola, zda je řetězec prázdný
	inline BOOL IsEmpty() const { return pData->Length == 0; };
	inline BOOL IsNotEmpty() const { return pData->Length != 0; };

// zápis do bufferu (bez koncové nuly)
	void Write(TCHAR* buf) const;
#ifdef _UNICODE
	void Write(LPSTR buf) const;
#else
	void Write(LPWSTR buf) const;
#endif

// zápis do bufferu (s koncovou nulou)
	void WriteNull(TCHAR* buf) const;
#ifdef _UNICODE
	void WriteNull(LPSTR buf) const;
#else
	void WriteNull(LPWSTR buf) const;
#endif


// ------- hledání

// hledání (<0 = nenalezeno, pos=výchozí pozice)
	int _fastcall Find(const CText& str) const;
	int _fastcall Find(const CText& str, int pos) const;
	int _fastcall Find(LPCTSTR txt) const;
	int _fastcall Find(LPCTSTR txt, int pos) const;
	int _fastcall Find(const TCHAR chr) const;
	int _fastcall Find(const TCHAR chr, int pos) const;
	
// hledání znaku od konce řetězce zpět (<0 = nenalezeno, pos=výchozí pozice)
	int _fastcall RevFind(const TCHAR chr) const;
	int _fastcall RevFind(const TCHAR chr, int pos) const;

// -------- modifikace textu

// vyprázdnění řetězce (připojí prázdný řetězec)
	void Empty();

// konverze malá/velká písmena
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool UpperCase();
	bool LowerCase();

// levá část řetězce (při chybě paměti vrátí prázdný řetězec)
	CText _fastcall Left(int count) const;

// pravá část řetězce (při chybě paměti vrátí prázdný řetězec)
	CText _fastcall Right(int count) const;

// střední část řetězce (při chybě paměti vrátí prázdný řetězec)
	CText _fastcall Mid(int first, int count = 0x7fffffff) const;

// zrušení textu z konce řetězce
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool Delete(int first, int count = 0x7fffffff);

// zrušení posledního znaku z konce řetězce
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool DeleteLast();

// odstranění mezer ze začátku/konce řetězce
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool TrimLeft();
	bool TrimRight();

// korekce výskytu nuly v textu (provádí se ve společném bufferu všech proměnných)
	void KorigNul();

// nastavení čísla jména objektu (1, 2, ...)
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool _fastcall SetNumObj(const int num);

// přidání textu
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool _fastcall Add(const CText& str);
	bool _fastcall Add(LPCTSTR txt);
	bool _fastcall Add(const TCHAR chr);

// přidání textu čísla
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool _fastcall AddInt(const int num);
	bool _fastcall AddDouble(const double num);

// součet textů
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool _fastcall Add(const CText& str1, const CText& str2);
	bool _fastcall Add(LPCTSTR txt, const CText& str);
	bool _fastcall Add(const CText& str, LPCTSTR txt);
	bool _fastcall Add(const TCHAR chr, const CText& str);
	bool _fastcall Add(const CText& str, const TCHAR chr);

// nastavení textu
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	void _fastcall Set(const CText& str);
	bool _fastcall Set(LPCTSTR txt);
	bool _fastcall Set(LPCTSTR txt, const int length);
#ifdef _UNICODE
	bool _fastcall Set(LPCSTR txt);
	bool _fastcall Set(LPCSTR txt, const int length);
#else //_UNICODE
	bool _fastcall Set(LPCWSTR txt);
	bool _fastcall Set(LPCWSTR txt, const int length);
#endif //_UNICODE
	bool _fastcall Set(const TCHAR chr);

// načtení textu z resource (při chybě vrací FALSE, původní obsah nezměněn)
	bool Load(const int nID);

// načtení ze souboru formátu TXT (FALSE=chyba, obsah nezměněn)
//	bool LoadFile(CText jmeno);

// uložení textu do souboru formátu TXT (FALSE=chyba)
//	bool SaveFile(CText jmeno) const;

// ------- přístup ke znakům v bufferu

// kopie do vlastního bufferu před modifikací
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool CopyWrite();

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

// poskytnutí znaku na pozici s kontrolou (pro neplatný znak vrátí 0)
	TCHAR _fastcall Get(const int index) const;

// nastavení znaku na pozici s kontrolou (zajistí přivlastnění bufferu)
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool _fastcall Set(const int index, const TCHAR chr);

// poskytnutí posledního znaku řetězce (pro prázdný řetězec vrací 0)
	TCHAR _fastcall LastChar() const;

// ------- text oken

// načtení textu okna (při chybě paměti vrátí FALSE, obsah bude nezměněn)
	bool GetWindowText(const HWND wnd);

// nastavení textu okna
	void _fastcall SetWindowText(const HWND wnd) const;

// načtení textu dialogového prvku (při chybě paměti vrátí FALSE, obsah bude nezměněn)
	bool GetDialogText(const HWND wnd, int id);

// nastavení textu dialogového prvku
	void _fastcall SetDialogText(const HWND wnd, int id) const;

// -------- adresáře a soubory

// načtení aktivního adresáře (při chybě paměti vrátí FALSE, obsah bude nezměněn)
	bool GetAktDir();

// korekce textu na jméno souboru (vypuštění zakázaných znaků, 
//	vrací opravenou pozici kurzoru)
//  ignoruje chybu paměti při přivlastnění bufferu (přivlastnit předem)
	int FileName(int curs);

// korekce textu na jméno cesty (vypuštění zakázaných znaků, 
//	vrací opravenou pozici kurzoru)
//  ignoruje chybu paměti při přivlastnění bufferu (přivlastnit předem)
	int PathName(int curs);

// -------- konverze čísel

// konverze čísla INT na text (při chybě paměti vrací FALSE)
	bool Int(int num);

// konverze textu na číslo INT
	friend int Int(LPCTSTR text);

// konverze čísla DWORD na HEX (8 číslic) (při chybě paměti vrací FALSE)
	bool Hex(DWORD num);

// konverze čísla DOUBLE na text (při chybě paměti vrací false)
	bool Double(double num);

// konverze textu na číslo DOUBLE
	friend double Double(LPCTSTR txt);

// -------- operátory

// konverze na řetězec LPCTSTR
	inline operator LPCTSTR() const { return pData->Data; }

// operátory přiřazení (nepoužívat - nehlásí chybu operace)
	inline const CText& _fastcall operator= (const CText& str) { Set(str); return *this; }
	inline const CText& _fastcall operator= (LPCTSTR txt) { Set(txt); return *this; }
#ifdef _UNICODE
	inline const CText& _fastcall operator= (LPCSTR txt) { Set(txt); return *this; }
#else //_UNICODE
	inline const CText& _fastcall operator= (LPCWSTR txt) { Set(txt); return *this; }
#endif //_UNICODE
	inline const CText& _fastcall operator= (const TCHAR chr) { Set(chr); return *this; };

// operátory přičtení (nepoužívat - nehlásí chybu operace)
	inline const CText& _fastcall operator+=(const CText& str) { Add(str); return *this; }
	inline const CText& _fastcall operator+=(LPCTSTR txt) { Add(txt); return *this; }
	inline const CText& _fastcall operator+=(const TCHAR chr) { Add(chr); return *this; }

// operátory součtu (nepoužívat - nehlásí chybu operace)
	inline friend CText _fastcall operator+ (const CText& str1, const CText& str2) { return CText(str1, str2); }
	inline friend CText _fastcall operator+ (LPCTSTR txt, const CText& str) { return CText(txt, str); }
	inline friend CText _fastcall operator+ (const CText& str, LPCTSTR txt) { return CText(str, txt); }
	inline friend CText _fastcall operator+ (const TCHAR chr, const CText& str) { return CText(chr, str); }
	inline friend CText _fastcall operator+ (const CText& str, const TCHAR chr) { return CText(str, chr); }

// operátory porovnání
	friend BOOL _fastcall operator==(const CText& str1, const CText& str2);
	friend BOOL _fastcall operator==(LPCTSTR txt, const CText& str);
	friend BOOL _fastcall operator==(const CText& str, LPCTSTR txt);
	friend BOOL _fastcall operator==(const CText& str, const TCHAR chr);
	friend BOOL _fastcall operator==(const TCHAR chr, const CText& str);
	friend BOOL _fastcall operator!=(const CText& str1, const CText& str2);
	friend BOOL _fastcall operator!=(LPCTSTR txt, const CText& str);
	friend BOOL _fastcall operator!=(const CText& str, LPCTSTR txt);
	friend BOOL _fastcall operator!=(const CText& str, const TCHAR chr);
	friend BOOL _fastcall operator!=(const TCHAR chr, const CText& str);
	friend bool _fastcall operator< (const CText& str1, const CText& str2);
	friend bool _fastcall operator<=(const CText& str1, const CText& str2);
	friend bool _fastcall operator> (const CText& str1, const CText& str2);
	friend bool _fastcall operator>=(const CText& str1, const CText& str2);
};


