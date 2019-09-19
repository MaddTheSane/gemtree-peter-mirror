
/***************************************************************************\
*																			*
*									Plocha									*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// struktura jednoho prvku dat plochy - 8 bajtů

typedef struct MAPITEM_
{
	CIcon	Icon;					// (4) ikona (při provádění programu)
	DWORD	Param;					// (4) parametry políčka
									//		bit 0 až bit 9 ........ (10 bitů) hodnota 1 (0 až 1023)
									//		bit 10 až bit 19 ...... (10 bitů) hodnota 2 (0 až 1023)
									//		bit 20 až bit 26 ...... (7 bitů) hodnota 3 (0 až 127)
									//		bit 27 až bit 31 ...... (5 bitů) příznaky 1 až 5
} MAPITEM;

#define SIZEOFMAPITEM	(sizeof(long) + sizeof(CIcon))	// velikost jednoho prvku plochy

/////////////////////////////////////////////////////////////////////////////
// struktura položky dat plochy - 12 bajtů + data

typedef struct MAPDATA_
{
	long		Refer;					// (4) čítač referencí na plochu
	long		Width;					// (4) šířka plochy v políčkách
	long		Height;					// (4) výška plochy v políčkách
	MAPITEM		Data[1];				// data plochy (zleva doprava a zdola nahoru)
} MAPDATA;

// parametry:
#define	MAP_VALUE1_MASK		0x000003FF		// maska hodnoty 1 políčka
#define MAP_VALUE1_ROT		0				// počet rotací pro hodnotu 1 políčka
#define MAP_VALUE1_MAX		1023			// maximální hodnota pro hodnotu 1 políčka
#define MAP_VALUE2_MASK		0x000FFC00		// maska hodnoty 2 políčka
#define MAP_VALUE2_ROT		10				// počet rotací pro hodnotu 2 políčka
#define MAP_VALUE2_MAX		1023			// maximální hodnota pro hodnotu 2 políčka
#define MAP_VALUE3_MASK		0x07F00000		// maska hodnoty 3 políčka
#define MAP_VALUE3_ROT		20				// počet rotací pro hodnotu 3 políčka
#define MAP_VALUE3_MAX		127				// maximální hodnota pro hodnotu 3 políčka

// zachovat pořadí hodnot přepínačů - používá se při zobrazení
#define MAP_FLAG1			0x08000000		// příznak 1
#define MAP_FLAG2			0x10000000		// příznak 2
#define MAP_FLAG3			0x20000000		// příznak 3
#define MAP_FLAG4			0x40000000		// příznak 4
#define MAP_FLAG5			0x80000000		// příznak 5

#define SIZEOFMAPDATA	(3*sizeof(long))	// velikost položky dat plochy (bez dat)

//#define MAPMAXWIDTH 0x1000					// maximální šířka plochy
//#define MAPMAXHEIGHT 0x1000					// maximální výška plochy


/////////////////////////////////////////////////////////////////////////////
// popisovač plochy v souboru (20 B)

typedef struct MAPFILE_
{
	char	Ident[4];				// (4) identifikátor (= "PMAP")
	DWORD	Param;					// (4) parametry - nastaveno na 0
	long	Width;					// (4) šířka plochy
	long	Height;					// (4) výška plochy
	long	Colors;					// (4) počet palet v tabulce
	RGBQUAD	ColorTab[1];			// (4) tabulka palet
} MAPFILE;

// Za popisovačem následuje:
//		- tabulka palet (formát RGBQUAD)
//		- (4) počet definovaných ikon
//		- definice ikon ve formátu:
//				- (4) délka textu ve znacích
//				- text jména ikony
//				- (ICONSIZE) ikona
//		- definice plochy ve formátu MAPITEM

#define SIZEOFMAPFILE (4*sizeof(char) + sizeof(DWORD) + 3*sizeof(long))

// statická inicializace plch

void InitMap();
extern MAPDATA* EmptyMapData;				// data prázdné plochy


/////////////////////////////////////////////////////////////////////////////
// plocha

class CMap
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné - pouze ukazatel na data
	MAPDATA*		pData;			// ukazatel na záhlaví plochy

// připojení dat
	inline void Attach(MAPDATA* data)
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
			MAPITEM* item = pData->Data;
			for (int i = pData->Width * pData->Height; i > 0; i--)
			{
				item->Icon.Term();
				item++;
			}
			MemFree(pData);
#ifdef _DEBUG
			pData = NULL;
#endif
		}
	}

// vytvoření nového bufferu - starý buffer musí být odpojen!
	inline void NewBuffer(int width, int height)
	{
		ASSERT((width > 0) && (height > 0));
		if (width <= 0) width = 1;
		if (height <= 0) height = 1;
		MAPDATA* data = (MAPDATA*)MemGet(SIZEOFMAPDATA + width*height*SIZEOFMAPITEM);
		data->Refer = 1;				// počet referencí
		data->Width = width;			// šířka
		data->Height = height;			// výška

		MAPITEM* item = data->Data;
		for (int i = width*height; i > 0; i--)
		{
			item->Icon.Init();
			item++;
		}
		pData = data;					// adresa dat
	}

// ---------------------------- veřejné funkce ------------------------------

public:

// konstruktor a destruktor
	CMap();								// standardní konstruktor
	CMap(const CMap& src);				// kopírovací konstruktor
	CMap(int width, int height);		// konstruktor s vytvořením obrázku
	~CMap();							// standardní destruktor

// statický konstruktor a destruktor
	void Init();						// statický konstruktor
	void Init(MAPDATA* data);			// statický konstruktor se zadáním dat
	void Init(int width, int height);	// statický konstruktor s vytvořením obrázku
	void Term();						// statický destruktor

// poskytnutí ukazatele na data
	inline MAPDATA* Data() const { return pData; };
	inline MAPITEM* DataData() const { return pData->Data; };

// poskytnutí šířky plochy
	inline int Width() const { return pData->Width; };

// poskytnutí výšky plochy
	inline int Height() const { return pData->Height; };

// poskytnutí velikosti dat plochy (bez záhlaví)
	inline int Size() const { return (pData->Height * pData->Width * SIZEOFMAPITEM); };

// vymazání obsahu plochy (naplnění ikonami 0, vynulované parametry)
	void Clear();

// kopie do vlastního bufferu před modifikací
	void CopyWrite();

// vyprázdnění plochy (uvolnění dat)
	void Empty();

// test, zda je plocha prázdná
	inline BOOL IsEmpty() { return ((DWORD)pData == (DWORD)EmptyMapData); };
	inline BOOL IsNotEmpty() { return ((DWORD)pData != (DWORD)EmptyMapData); };

// vytvoření nové plochy (připraveno k zápisu, data jsou náhodná)
	void New(int width, int height);

// kontrola platnosti offsetu prvku
	inline BOOL IsValid(const int off) const 
		{ return ((DWORD)off < (DWORD)(pData->Width * pData->Height)); };

	inline BOOL IsNotValid(const int off) const 
		{ return ((DWORD)off >= (DWORD)(pData->Width * pData->Height)); };

// kontrola platnosti indexu prvku
	inline BOOL IsValid(const int x, const int y) const
		{ return (((DWORD)x < (DWORD)pData->Width) && ((DWORD)y < (DWORD)pData->Height)); };

	inline BOOL IsNotValid(const int x, const int y) const
		{ return (((DWORD)x >= (DWORD)pData->Width) || ((DWORD)y >= (DWORD)pData->Height)); };

// poskytnutí přístupu k prvku (bez kontroly offsetu/indexu)
	inline MAPITEM& operator[] (const int off) 
		{ ASSERT(IsValid(off)); return pData->Data[off]; }

	inline const MAPITEM& operator[] (const int off) const
		{ ASSERT(IsValid(off)); return pData->Data[off]; }

	inline MAPITEM& At(const int off) 
		{ ASSERT(IsValid(off)); return pData->Data[off]; }

	inline const MAPITEM& At(const int off) const
		{ ASSERT(IsValid(off)); return pData->Data[off]; }

	inline MAPITEM& At(const int x, const int y) 
		{ ASSERT(IsValid(x, y)); return pData->Data[x + y*pData->Width]; }

	inline const MAPITEM& At(const int x, const int y) const
		{ ASSERT(IsValid(x, y)); return pData->Data[x + y*pData->Width]; }

// poskytnutí prvku (s kontrolou platnosti offsetu/indexu)
	const MAPITEM& _fastcall Get(const int off) const;
	const MAPITEM& _fastcall Get(const int x, const int y) const;

// nastavení prvku (s kontrolou platnosti offsetu/indexu)
	void _fastcall Set(const int off, const MAPITEM& data);
	void _fastcall Set(const int x, const int y, const MAPITEM& data);

// operátor přiřazení
	const CMap& operator= (const CMap& src);

// operátory porovnání
	friend inline BOOL operator==(const CMap& s1, const CMap& s2)
		{ return (DWORD)s1.pData == (DWORD)s2.pData; };

	friend inline BOOL operator!=(const CMap& s1, const CMap& s2)
		{ return (DWORD)s1.pData != (DWORD)s2.pData; };
};

/***************************************************************************\
*																			*
*								Buffer ploch								*
*																			*
\***************************************************************************/

class CBufMap
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné
	CMap*		m_Data;		// ukazatel na data
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
	CBufMap();
	~CBufMap();

// statický konstruktor a destruktor
	void Init();			// statický konstruktor
	void Term();			// statický destruktor

// zrušení všech položek v bufferu (ukládání začne opět po řadě od začátku)
	void DelAll();

// poskytnutí bufferu dat
	inline CMap* Data() const { return m_Data; };

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
	inline CMap& operator[] (const int index)
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline const CMap& operator[] (const int index) const
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline CMap& At(const int index)
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline const CMap& At(const int index) const
		{ ASSERT(IsValid(index)); return m_Data[index]; }

// poskytnutí položky (s kontrolou platnosti indexu)
	const CMap& _fastcall Get(const int index) const;

// nastavení položky (s kontrolou platnosti indexu)
	void _fastcall Set(const int index, const CMap& data);

// vyprázdnění položky (bez jejího zrušení - jen pro uvolnění dat)
	void _fastcall Empty(const int index);

// zrušení položek z konce bufferu
	void _fastcall Del(int num);

// vytvoření prázdné položky (vrací index položky)
	int New();
	int New(int width, int height); // plochu vymaže ikonami 0

// přidání položky (vrací index položky)
	int _fastcall Add(const CMap& data);
	int _fastcall Add(MAPDATA* data);

// duplikace položky (s kontrolou platnosti indexu, vrací index první položky)
	int _fastcall Dup(const int index);
	int _fastcall Dup(const int index, int num);

// operátor přiřazení
	const CBufMap& operator= (const CBufMap& src);
};

