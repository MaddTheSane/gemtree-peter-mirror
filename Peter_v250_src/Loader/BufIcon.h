
/***************************************************************************\
*																			*
*						Ikonové proměnné (velikost 32x32)					*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// struktura záhlaví ikony - 24 bajtů

typedef struct ICONDATA_
{
	long	Refer;					// (4) čítač referencí na ikonu
	long	Param;					// (4) parametry (průhlednost ikony)
	HICON	HIcon;					// (4) handle ikony Windows (NULL = nevytvořena)
	HCURSOR	Cursor;					// (4) handle kurzoru myši (NULL = nevytvořen)
	long	res;					// (4) ...rezerva pro zarovnání
	BYTE*	Data;					// (4) ukazatel na data ikony
} ICONDATA;

#define SIZEOFICONDATA	(5*sizeof(long) + sizeof(BYTE*)) // velikost záhlaví

extern ICONDATA EmptyIconData;		// data prázdné ikony (kurzor myši = implicitní šipka)


/////////////////////////////////////////////////////////////////////////////
// ikona

class CIcon
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné - pouze ukazatel na data
	ICONDATA*		pData;			// ukazatel na záhlaví ikony

// připojení dat
	inline void Attach(ICONDATA* data)
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
			if (pData->HIcon != NULL)
			{
				::DestroyIcon(pData->HIcon);
			}

			if (pData->Cursor != NULL)
			{
				::DestroyCursor(pData->Cursor);
			}

			ASSERT(pData->Data);
			MemFree(pData->Data);		// zrušení dat ikony
			MemFree(pData);				// zrušení záhlaví ikony
#ifdef _DEBUG
			pData = NULL;
#endif
		}
	}

// vytvoření nového záhlaví a bufferu dat ikony - staré záhlaví musí být odpojeno!
	inline void NewBuffer()
	{
		ICONDATA* data = (ICONDATA*)MemGet(SIZEOFICONDATA); // vytvoření záhlaví
		pData = data;
		data->Refer = 1;						// počet referencí
		data->Param = PicParamNone;				// parametry - neznámé
		data->HIcon = NULL;						// není ikona
		data->Cursor = NULL;					// není kurzor
		data->Data = (BYTE*)MemGet(ICONSIZE);	// vytvoření bufferu dat ikony
	}

// ---------------------------- veřejné funkce ------------------------------

public:

// konstruktor a destruktor
	CIcon();
	CIcon(const CIcon& src);
	~CIcon();

// statický konstruktor a destruktor
	void Init();
	void Init(ICONDATA* data);
	void InitNew();
	void Term();

// poskytnutí ukazatele na data
	inline ICONDATA* Data() const { return pData; };
	inline BYTE* DataData() const { return pData->Data; };

// poskytnutí parametrů ikony (při provádění programu)
	inline int Param() const { return pData->Param; };

// nastavení parametrů ikony
	inline void Param(int param) { pData->Param = param; };

// kopie nových dat ikony - zajistí odpojení dat
	void CopyData(BYTE* src);

// kopie nových dat ikony s konverzí - zajistí odpojení dat
	void CopyKonvData(BYTE* src);

// kopie do vlastního bufferu před modifikací
	void CopyWrite();

// vyprázdnění ikony (uvolnění dat)
	void Empty();

// test, zda je ikona prázdná
	inline BOOL IsEmpty() { return ((DWORD)pData == (DWORD)&EmptyIconData); };
	inline BOOL IsNotEmpty() { return ((DWORD)pData != (DWORD)&EmptyIconData); };

// vytvoření nové ikony (připraveno k zápisu, data jsou náhodná)
	void New();

// načtení ikony ze souboru
	void LoadFile();

// uložení ikony do souboru
	bool SaveFile(CString jmeno);
	void SaveFile();

// vytvoření ikony WINDOWS
	HICON HIcon();

// vytvoření kurzoru myši
	HCURSOR Cursor();

// dekomprimace dat ikony (jsou-li komprimována)
	void DeComp();

// operátor přiřazení
	const CIcon& operator= (const CIcon& src);
	const CIcon& operator= (ICONDATA* src);

// operátory porovnání
	friend inline BOOL operator==(const CIcon& s1, const CIcon& s2)
		{ return (DWORD)s1.pData == (DWORD)s2.pData; };

	friend inline BOOL operator!=(const CIcon& s1, const CIcon& s2)
		{ return (DWORD)s1.pData != (DWORD)s2.pData; };
};


/***************************************************************************\
*																			*
*								Buffer ikon									*
*																			*
\***************************************************************************/

class CBufIcon
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné
	CIcon*		m_Data;			// ukazatel na data
	int			m_Num;			// počet platných položek v bufferu
	int			m_Max;			// velikost bufferu (položek)

// vytvoření nové položky
	inline int NewItem()
	{
		int i = m_Num;
		if (i >= m_Max)
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
	CBufIcon();
	~CBufIcon();

// statický konstruktor a destruktor
	void Init();
	void Term();

// zrušení všech položek v bufferu
	void DelAll();

// poskytnutí bufferu ikon
	inline CIcon* Data() const { return m_Data; };

// poskytnutí počtu platných položek v bufferu
	inline int Num() const { return m_Num; };

// poskytnutí velikosti bufferu
	inline int Max() const { return m_Max; };

// kontrola platnosti položky
	inline BOOL IsValid(const int index) const
		{ return ((DWORD)index < (DWORD)m_Num); };

	inline BOOL IsNotValid(const int index) const
		{ return ((DWORD)index >= (DWORD)m_Num); };

// poskytnutí přístupu k položce (bez kontroly indexu)
	inline CIcon& operator[] (const int index)
		{ ASSERT(IsValid(index)); return m_Data[index]; };

	inline const CIcon& operator[] (const int index) const
		{ ASSERT(IsValid(index)); return m_Data[index]; };

	inline CIcon& At(const int index)
		{ ASSERT(IsValid(index)); return m_Data[index]; };

	inline const CIcon& At(const int index) const
		{ ASSERT(IsValid(index)); return m_Data[index]; };

// poskytnutí položky (s kontrolou platnosti indexu)
	const CIcon& _fastcall Get(const int index) const;

// nastavení položky (s kontrolou platnosti indexu)
	void _fastcall Set(const int index, const CIcon& data);

// vyprázdnění položky (bez jejího zrušení - jen pro uvolnění dat)
	void _fastcall Empty(const int index);

// zrušení položek z konce bufferu
	void _fastcall Del(int num);

// operátor přiřazení
	const CBufIcon& _fastcall operator= (const CBufIcon& src);

// vytvoření prázdné položky - data neinicializovaná (vrací index položky)
	int New();

// přidání položky (vrací index položky)
	int _fastcall Add(const CIcon& data);
	int _fastcall Add(ICONDATA* data);

// duplikace položky (s kontrolou platnosti indexu, vrací index první položky)
	int _fastcall Dup(const int index);
	int _fastcall Dup(const int index, int num);
};
