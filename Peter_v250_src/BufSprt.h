
/***************************************************************************\
*																			*
*									Sprajt									*
*																			*
\***************************************************************************/

#define	SPRITEDEFDELAY	55			// implicitní prodleva
#define	SPRITEDEFLEVEL	1000		// implicitní hladina
#define	SPRITEDEFKROKU	8			// implicitní počet kroků

#define SPRITEMAXKLID  1024			// maximální počet klidových fází
#define SPRITEMAXPOHYB 1024			// maximální počet fází pro pohyb
#define SPRITEMAXFAZE  (SPRITEMAXKLID+SPRITEMAXPOHYB) // max. počet fází
#define SPRITEMAXSMER  1024			// maximální počet směrů
#define SPRITEMAXDELAY 3600000		// maximální prodleva mezi fázemi
#define SPRITEMAXKROKU 1000000		// maximální počet kroků na jednotku

/////////////////////////////////////////////////////////////////////////////
// struktura položky dat sprajtu (32 bajtů + data)
// Všechny obrázky mají stejnou velikost, musí být minimálně 1 obrázek,
// - velikost obrázku se zjišťuje z prvního obrázku

typedef struct SPRITEDATA_
{
	long		Refer;				// (4) čítač referencí na sprajt
	long		Faze;				// (4) počet fází celkem
	long		Smer;				// (4) počet směrů
	long		Klid;				// (4) z toho počet klidových fází
	long		Delay;				// (4) prodleva mezi dvěma fázemi (v milisekundách)
	long		Level;				// (4) hladina k překreslování (0=předměty)
	double		Kroku;				// (8) počet fází na jednotkovou vzdálenost (0=sleduje ihned)
	CPicture	Data[1];			// obrázky sprajtu (v pořadí fáze/směr)
} SPRITEDATA;

#define SIZEOFSPRITEDATA	(6*sizeof(long) + sizeof(double)) // velikost položky dat sprajtu (bez dat)

/////////////////////////////////////////////////////////////////////////////
// popisovač sprajtu v souboru (40 B)

typedef struct SPRITEFILE_
{
	char	Ident[4];				// (4) identifikátor (= "PSPR")
	long	Faze;					// (4) počet fází celkem
	long	Smer;					// (4) počet směrů
	long	Klid;					// (4) z toho počet klidových fází
	WORD	Width;					// (2) šířka obrázku (max. 32768)
	WORD	Height;					// (2) výška obrázku (max. 32768)
	long	Delay;					// (4) prodleva mezi fázemi v milisekundách
	long	Level;					// (4) hladina k vykreslování (0=předměty)
	double	Kroku;					// (8) počet fází na jednotkovou vzdálenost (0=ihned)
	short	Colors;					// (2) počet palet v tabulce
	short	Format;					// (2) formát dat a parametry
									//			0 = nekomprimováno (starý formát)
									//			1 = komprese
	RGBQUAD	ColorTab[1];			// (4) tabulka palet
} SPRITEFILE;

// Za popisovačem následuje tabulka palet (formát RGBQUAD) a data obrázků
// V komprimovaném formátu následuje za tabulkou palet dvojslovo=velikost zkomprimovaných
// dat obrázků (komprimováno vše najednou), následují zkomprimovaná data obrázků

#define SIZEOFSPRITEFILE (4*sizeof(char) + 5*sizeof(long) + 2*sizeof(short) + 2*sizeof(WORD) + sizeof(double))

/////////////////////////////////////////////////////////////////////////////
// inicializační prázdný sprajt (modifikuje se počet referencí!)

extern SPRITEDATA* EmptySpriteData;				// data prázdného sprajtu
extern	const CSprite		EmptySprite;		// prázdný sprajt

/////////////////////////////////////////////////////////////////////////////
// statické funkce

// statická inicializace sprajtů (při chybě paměti vrací FALSE)
bool InitSprite();

// vytvoření dat sprajtu (při chybě paměti vrací NULL)
SPRITEDATA* _fastcall NewSpriteData(int faze, int smer);

// vytvoření dat sprajtu bez inicializace obrázků a nastavení parametrů (při chybě paměti vrací NULL)
SPRITEDATA* _fastcall NewSpriteData0(int faze, int smer);

// zrušení dat sprajtu (odděleno kvůli lepší optimalizaci)
void _fastcall DelSpriteData(SPRITEDATA* data);


/////////////////////////////////////////////////////////////////////////////
// sprajt

class CSprite
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné
	SPRITEDATA*		pData;					// ukazatel na záhlaví sprajtu

// připojení dat sprajtu
	inline void attach(SPRITEDATA* data)
	{
		ASSERT(data != NULL);

		pData = data;
		LongIncrement(&(data->Refer));
	}

// odpojení (a zrušení) dat sprajtu
	inline void detach(SPRITEDATA* data)
	{
		ASSERT(data != NULL);
		ASSERT(data->Refer > 0);

		if (LongDecrement(&(pData->Refer)))
		{
			DelSpriteData(data);
		}
	}


// ---------------------------- veřejné funkce ------------------------------

public:

// konstruktor a destruktor
	CSprite();						// prázdný sprajt o velikosti 1x1 (prázdný obrázek ICONSIZE)
	CSprite(const CSprite& src);
	CSprite(SPRITEDATA* data);
	~CSprite();

// konstruktor a destruktor
	void Init();					// prázdný sprajt o velikosti 1x1 (prázdný obrázek ICONSIZE)
	void _fastcall Init(const CSprite& src);
	void _fastcall Init(SPRITEDATA* data);
	bool _fastcall Init(int faze, int smer); // při chybě paměti vrátí FALSE, sprajt není vytvořen
	void Term();

// poskytnutí ukazatele na data sprajtu
	inline SPRITEDATA* Data() const { return pData; };
	inline CPicture* DataData() const { return pData->Data; };

// poskytnutí počtu fází sprajtu
	inline int Faze() const { return pData->Faze; };

// poskytnutí počtu směrů sprajtu
	inline int Smer() const { return pData->Smer; };

// poskytnutí/nastavení (bez změny fází celkem) počtu klidových fází sprajtu
	inline int Klid() const { return pData->Klid; };
	inline void Klid(int klid) { pData->Klid = klid; };

// poskytnutí/nastavení prodlevy mezi dvěma fázemi (v milisekundách)
	inline int Delay() const { return pData->Delay; };
	inline void Delay(int delay) { pData->Delay = delay; };

// poskytnutí/nastavení hladiny zobrazení sprajtu (0=předměty)
	inline int Level() const { return pData->Level; };
	inline void Level(int level) { pData->Level = level; };

// poskytnutí/nastavení počtu kroků na jednotku (0=ihned)
	inline double Kroku() const { return pData->Kroku; };
	inline void Kroku(double kroku) { pData->Kroku = kroku; };

// poskytnutí šířky obrázku sprajtu
	inline int Width() const { return pData->Data[0].Width(); };

// poskytnutí výšky obrázku sprajtu
	inline int Height() const { return pData->Data[0].Height(); };

// poskytnutí velikosti dat sprajtu - data obrázků bez záhlaví
	inline int Size() const { return pData->Faze * pData->Smer * pData->Data[0].Width() * pData->Data[0].Height(); };

// vyprázdnění (uvolnění dat) - připojí se standardní prázdný sprajt o velikosti 1x1
	void Empty();

// test, zda je sprajt prázdná (zda ukazuje na standardní prázdný sprajt 1x1)
	inline BOOL IsEmpty() { return ((DWORD)pData == (DWORD)EmptySpriteData); };
	inline BOOL IsNotEmpty() { return ((DWORD)pData != (DWORD)EmptySpriteData); };

// kopie do vlastního bufferu před modifikací (pozor - obrázky tímto nejsou přivlastněny!)
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool CopyWrite();

// vytvoření nového sprajtu (připraveno k zápisu, data jsou náhodná)
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool _fastcall New(int faze, int smer);

// nastavení nových rozměrů sprajtu (počet fází pro klid a pohyb, počet směrů)
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool ReSize(int klid, int pohyb, int smer);

// kontrola platnosti offsetu prvku
	inline BOOL IsValid(const int off) const 
		{ return ((DWORD)off < (DWORD)(pData->Faze * pData->Smer)); };

	inline BOOL IsNotValid(const int off) const 
		{ return ((DWORD)off >= (DWORD)(pData->Faze * pData->Smer)); };

// kontrola platnosti indexu prvku
	inline BOOL IsValid(const int faze, const int smer) const
		{ return (((DWORD)faze < (DWORD)pData->Faze) && ((DWORD)smer < (DWORD)pData->Smer)); };

	inline BOOL IsNotValid(const int faze, const int smer) const
		{ return (((DWORD)faze >= (DWORD)pData->Faze) || ((DWORD)smer >= (DWORD)pData->Smer)); };

// poskytnutí přístupu k prvku (bez kontroly offsetu/indexu)
// v případě zápisu musí být zajištěno přivlastnění bufferu!
	inline CPicture& operator[] (const int off) 
		{ ASSERT(IsValid(off)); return pData->Data[off]; }

	inline const CPicture& operator[] (const int off) const
		{ ASSERT(IsValid(off)); return pData->Data[off]; }

	inline CPicture& At(const int off) 
		{ ASSERT(IsValid(off)); return pData->Data[off]; }

	inline const CPicture& At(const int off) const
		{ ASSERT(IsValid(off)); return pData->Data[off]; }

	inline CPicture& At(const int faze, const int smer)
		{ ASSERT(IsValid(faze, smer)); return pData->Data[faze + smer*pData->Faze]; }

	inline const CPicture& At(const int faze, const int smer) const
		{ ASSERT(IsValid(faze, smer)); return pData->Data[faze + smer*pData->Faze]; }

// poskytnutí prvku (s kontrolou platnosti offsetu/indexu)
// pro neplatný prvek vrací přístup na prvek prázdného sprajtu
	const CPicture& _fastcall Get(const int off) const;
	const CPicture& _fastcall Get(const int faze, const int smer) const;

// nastavení prvku (s kontrolou platnosti offsetu/indexu)
// před zápisem je nutno zajistit přivlastnění bufferu!
	void _fastcall Set(const int off, const CPicture& data);
	void _fastcall Set(const int faze, const int smer, const CPicture& data);

// načtení sprajtu ze souboru
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool LoadFile(CText jmeno);

// uložení do souboru (FALSE=chyba)
	bool SaveFile(CText jmeno) const;

// operátor přiřazení
	const CSprite& operator= (const CSprite& src);
	const CSprite& operator= (SPRITEDATA* src);
};

/***************************************************************************\
*																			*
*								Buffer sprajtů								*
*																			*
\***************************************************************************/

class CBufSprite : public CBuffer<CSprite>
{

// ---------------------------- veřejné funkce ------------------------------

public:

// vytvoření nového sprajtu (vrací index položky, <0 = chyba)
// provádí záznam do UNDO bufferu
	inline int New() { return CBuffer<CSprite>::New(); } // prázdný sprajt o velikosti 1x1
	int _fastcall New(int faze, int smer);			// data jsou náhodná
};

