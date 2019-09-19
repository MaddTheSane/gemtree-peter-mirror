
/***************************************************************************\
*																			*
*									Hudba									*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// struktura položky dat hudby - 16 bajtů

typedef struct MUSICDATA_
{
	long	Refer;					// (4) čítač referencí
	long	Size;					// (4) velikost dat (bajtů)
	long	res;					// (4) ... rezerva (pro zarovnání)
	BYTE*	Data;					// (4) ukazatel na data

} MUSICDATA;

// velikost položky dat hudby (bez dat)
#define SIZEOFMUSICDATA	(3*sizeof(long) + sizeof(BYTE*))

extern MUSICDATA EmptyMusicData;		// data prázdné hudby
extern	const CMusic	EmptyMusic;		// prázdná hudba

/////////////////////////////////////////////////////////////////////////////
// statické funkce

// vytvoření dat hudby (při chybě paměti vrací NULL)
MUSICDATA* _fastcall NewMusicData(int size);

// zrušení dat hudby (odděleno kvůli lepší optimalizaci)
void _fastcall DelMusicData(MUSICDATA* data);

/////////////////////////////////////////////////////////////////////////////
// hudba

class CMusic
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné
	MUSICDATA*		pData;					// ukazatel na záhlaví hudby

// připojení dat hudby
	inline void attach(MUSICDATA* data)
	{
		ASSERT(data != NULL);

		pData = data;
		LongIncrement(&(data->Refer));
	}

// odpojení (a zrušení) dat hudby
	inline void detach(MUSICDATA* data)
	{
		ASSERT(data != NULL);
		ASSERT(data->Refer > 0);

		if (LongDecrement(&(data->Refer)))
		{
			DelMusicData(data);
		}
	}


// ---------------------------- veřejné funkce ------------------------------

public:

// konstruktor a destruktor
	CMusic();					// prázdná hudba o délce 0
	CMusic(const CMusic& src);
	CMusic(MUSICDATA* data);
	~CMusic();

// statický konstruktor a destruktor
	void Init();				// prázdná hudba o délce 0
	void _fastcall Init(const CMusic& src);
	void _fastcall Init(MUSICDATA* data);
	bool _fastcall Init(int size);		// při chybě paměti vrací FALSE, hudba není vytvořena
	void Term();

// poskytnutí ukazatele na data
	inline MUSICDATA* Data() const { return pData; };
	inline BYTE* DataData() const { return pData->Data; };

// poskytnutí velikosti dat hudby
	inline int Size() const { return pData->Size; };

// vyprázdnění (uvolnění dat) - připojí se standardní prázdná hudba
	void Empty();

// kopie do vlastního bufferu před modifikací
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool CopyWrite();

// vytvoření nové hudby (připraveno k zápisu, data jsou náhodná)
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool New(int size);

// načtení hudby ze souboru (TRUE=operace OK)
	bool LoadFile(CText jmeno);

// uložení do souboru formátu MID (FALSE=chyba)
	bool SaveFile(CText jmeno) const;

// operátor přiřazení
	const CMusic& operator= (const CMusic& src);
	const CMusic& operator= (MUSICDATA* src);
};

/***************************************************************************\
*																			*
*								Buffer hudby								*
*																			*
\***************************************************************************/

class CBufMusic : public CBuffer<CMusic>
{

// ---------------------------- veřejné funkce ------------------------------

public:

// vytvoření nové položky (prázdné/obsah náhodný) (vrací index položky, <0 = chyba)
// provádí záznam do UNDO bufferu
	inline int New() { return CBuffer<CMusic>::New(); }
	int New(int size);

// přidání položky (vrací index položky, <0 = chyba)
// provádí záznam do UNDO bufferu
	inline int _fastcall Add(const CMusic& data) { return CBuffer<CMusic>::Add(data); }
	int _fastcall Add(MUSICDATA* data);
};

