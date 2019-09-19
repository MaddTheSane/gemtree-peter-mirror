
/***************************************************************************\
*																			*
*								Ikonové proměnné							*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// struktura záhlaví ikony - 16 bajtů
// Při komprimaci udává první dvojslovo dat délku komprimovaných dat
// Dekomprimace i komprimace se provádí s daty společnými pro všechny ikony,
// je proto nutno v případě potřeby zajistit vlastnictví ikony!
// Komprimace se provádí pouze u ikon 32x32!

typedef struct ICONDATA_
{
	long	Refer;					// (4) čítač referencí na ikonu
	long	Param;					// (4) parametry (průhlednost, komprese)
	long	Size;					// (4) velikost nezkomprimovaných dat ikony (typicky 32x32 = 1024)
	BYTE*	Data;					// (4) ukazatel na data ikony
} ICONDATA;

//////////////////////////////////////////////////////////////////////////////
// příznaky typu ikony a obrázku
//enum PICPARAM {
//	PicParamPic,			// pouze obrázek bez pozadí
//	PicParamMix,			// obrázek mixovaný s pozadím
//	PicParamBack,			// pouze pozadí (prázdný obrázek)
//	PicParamNone,			// neznámý obsah
//	PicParamComp,			// komprimovaná data
//};

#define SIZEOFICONDATA	(3*sizeof(long) + sizeof(BYTE*)) // velikost záhlaví

extern ICONDATA EmptyIconData;				// data prázdné ikony
extern	const CIcon		EmptyIcon;			// prázdná ikona

/////////////////////////////////////////////////////////////////////////////
// statické funkce

// statická inicializace ikon (při chybě paměti vrací FALSE)
bool InitIcon();

// vytvoření dat ikony (při chybě paměti vrací NULL)
ICONDATA* _fastcall NewIconData(int size);

// zrušení dat ikony (odděleno kvůli lepší optimalizaci)
void _fastcall DelIconData(ICONDATA* data);


/////////////////////////////////////////////////////////////////////////////
// ikona

class CIcon
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné
	ICONDATA*		pData;			// ukazatel na záhlaví ikony (NULL=není nic)

// připojení dat ikony
	inline void attach(ICONDATA* data)
	{
		ASSERT(data != NULL);

		pData = data;
		LongIncrement(&(data->Refer));
	}

// odpojení (a zrušení) dat ikony
	inline void detach(ICONDATA* data)
	{
		ASSERT(data != NULL);
		ASSERT(data->Refer > 0);

		if (LongDecrement(&(data->Refer)))
		{
			DelIconData(data);
		}
	}


// ---------------------------- veřejné funkce ------------------------------

public:

// konstruktor a destruktor
	CIcon();					// prázdná ikona o velikosti ICONSIZE
	CIcon(const CIcon& src);
	CIcon(ICONDATA* data);
	~CIcon();

// statický konstruktor a destruktor
	void Init();				// prázdná ikona o velikosti ICONSIZE
	void _fastcall Init(const CIcon& src);
	void _fastcall Init(ICONDATA* data);
	bool _fastcall Init(int size);	// při chybě paměti vrací FALSE, ikona není vytvořena
	void Term();

// poskytnutí ukazatele na data
	inline ICONDATA* Data() const { return pData; };
	inline BYTE* DataData() const { return pData->Data; };

// poskytnutí velikosti nezkomprimovaných dat ikony
	inline int Size() const { return pData->Size; };

// poskytnutí/nastavení parametrů ikony
	inline int Param() const { return pData->Param; };
	inline void Param(int param) { pData->Param = param; };

// vyprázdnění (uvolnění dat) - připojí se standardní prázdná ikona ICONSIZE
	void Empty();

// test, zda je ikona prázdná (zda ukazuje na standardní prázdnou ikonu ICONSIZE)
	inline BOOL IsEmpty() { return ((DWORD)pData == (DWORD)&EmptyIconData); };
	inline BOOL IsNotEmpty() { return ((DWORD)pData != (DWORD)&EmptyIconData); };

// vymazání obsahu ikony (naplnění průhlednou barvou), zajistí přivlastnění (a dekomprimaci) bufferu,
// při chybě paměti vrací FALSE, původní obsah nezměněn
	bool Clear();

// vymazání obsahu ikony s nastavením velikosti (naplnění průhlednou barvou), 
// zajistí přivlastnění (a dekomprimaci) bufferu, při chybě paměti vrací FALSE, původní obsah nezměněn
	bool _fastcall Clear(int size);

// kopie do vlastního bufferu před modifikací (komprimovaná data zůstanou komprimovaná)
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool CopyWrite();

// vytvoření nové ikony se stejnou velikostí (připraveno k zápisu, data jsou náhodná)
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool New();

// vytvoření nové ikony (připraveno k zápisu, data jsou náhodná)
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool _fastcall New(int size);

// kontrola platnosti offsetu bodu
	inline BOOL IsValid(const int off) const 
		{ return ((DWORD)off < (DWORD)pData->Size); };

	inline BOOL IsNotValid(const int off) const 
		{ return ((DWORD)off >= (DWORD)pData->Size); };

// poskytnutí přístupu k bodu ikony (bez kontroly offsetu) - ikona nesmí být komprimovaná!
// v případě zápisu musí být zajištěno přivlastnění bufferu!
	inline BYTE& operator[] (const int off) 
		{ ASSERT(IsValid(off)); return pData->Data[off]; }

	inline const BYTE& operator[] (const int off) const
		{ ASSERT(IsValid(off)); return pData->Data[off]; }

	inline BYTE& At(const int off) 
		{ ASSERT(IsValid(off)); return pData->Data[off]; }

	inline const BYTE& At(const int off) const
		{ ASSERT(IsValid(off)); return pData->Data[off]; }

// poskytnutí bodu (s kontrolou platnosti offsetu) - ikona nesmí být komprimovaná!
// pro neplatný offset vrací barvu pozadí
	BYTE _fastcall Get(const int off) const;

// nastavení bodu (s kontrolou platnosti offsetu) - ikona nesmí být komprimovaná!
// před zápisem je nutno zajistit přivlastnění bufferu!
	void _fastcall Set(const int off, const BYTE data);

// komprese dat ikony 32*32, vrací velikost dat (při chybě paměti vrací <0, data jsou nezměněna)
// komprese se provádí ve společném bufferu pro všechny proměnné!
// (v případě potřeby je nutno zajistit přivlastnění bufferu)
// původní buffer s daty je zrušen
// (velikost komprimovaných dat je udávána bez dvojslova s velikostí, buffer s daty
// je tedy o 4 větší, data začínají na offsetu 4 v bufferu)
	int Comp() const;

// dekomprimace dat ikony 32*32 (jsou-li komprimována), při chybě vrací FALSE, data jsou nezměněna
// dekomprese se provádí ve společném bufferu pro všechny proměnné
// (v případě potřeby je nutno zajistit přivlastnění bufferu)
// původní buffer s komprimovanými daty je zrušen
	bool DeComp() const;

// kopie nových dat ikony (rozměry zůstanou nezměněny) - zajistí přivlastnění (a dekomprimaci) bufferu,
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool CopyData(BYTE* src);

// kopie komprimovaných nových dat ikony (rozměry zůstanou nezměněny) - zajistí odpojení dat
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool CompCopy(BYTE* src);

// kopie nových dat ikony s konverzí (rozměry zůstanou nezměněny) - zajistí odpojení dat
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool CopyKonvData(BYTE* src);

// extrakt ikony ze souboru formátu PET (při chybě vrátí FALSE, obsah nezměněn)
	bool Extract(CText jmeno);

// načtení ikony ze souboru formátu ICO (FALSE=chyba, obsah nezměněn)
	bool LoadFile(CText jmeno);

// uložení ikony do souboru formátu ICO (komprimovanou ikonu dekomprimuje) (FALSE=chyba)
	bool SaveFile(CText jmeno) const;

// operátor přiřazení
	const CIcon& operator= (const CIcon& src);
	const CIcon& operator= (ICONDATA* src);
};


/***************************************************************************\
*																			*
*								Buffer ikon									*
*																			*
\***************************************************************************/

class CBufIcon : public CBuffer<CIcon>
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné
	int			m_Width;		// šířka jedné ikony
	int			m_Height;		// výška jedné ikony
	int			m_WidthByte;	// délka jedné linky v bajtech
	int			m_IconSize;		// velikost ikony v bajtech

// ---------------------------- veřejné funkce ------------------------------

public:

// konstruktor
	CBufIcon();

// statický konstruktor
	void Init();

// poskytnutí šířky ikony
	inline int Width() const { return m_Width; };

// poskytnutí výšky ikony
	inline int Height() const { return m_Height; };

// poskytnutí/nastavení velikosti ikony (případné existující ikony zruší!)
	void _fastcall IconSize(const int width, const int height);
	inline int IconSize() { return m_IconSize; };

// přidání položky (vrací index položky, <0 = chyba)
// provádí záznam do UNDO bufferu
	inline int _fastcall Add(const CIcon& icon) { return CBuffer<CIcon>::Add(icon); }
	int _fastcall Add(ICONDATA* data);

// načtení ikon z resource (vynulovat buffer, aby bylo ukládáno po řadě!), vrací TRUE=operace OK
	bool Load(const int nID, int num);

// vygenerování dat bitmapy (vrací ukazatel na buffer s rezervou 8 bajtů na konci, při chybě vrací NULL)
	BYTE* GenerBitmap(SMALLICON smallicon);

// vygenerování masky ikon z dat barevné bitmapy (smallicon = poloviční velikost)
// barevná bitmapa musí mít na konci rezervu pro zarovnání linky na 8 bajtů !
// vrací adresu na buffer, při chybě vrací NULL
	BYTE* GenerMask(BYTE* bitmapa, SMALLICON smallicon);

// vygenerování seznamu ikon (drag = tažení, smallicon = poloviční velikost,
//		himg = použitý seznam, pro himg=NULL vytvoří nový seznam)
// při chybě vrací NULL, původní seznam ikon přitom zůstává nezměněn
	HIMAGELIST GenerList(bool drag, SMALLICON smallicon, HIMAGELIST himg = NULL);
};
