
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
	long		Level;				// (4) hladina k překreslování
	double		Kroku;				// (8) počet fází na jednotkovou vzdálenost (0=sleduje ihned)
	CPicture	Data[1];			// obrázky sprajtu (v pořadí fáze/směr)
} SPRITEDATA;

#define SIZEOFSPRITEDATA	(6*sizeof(long) + sizeof(double)) // velikost položky dat sprajtu (bez dat)

extern SPRITEDATA* EmptySpriteData;				// data prázdného sprajtu

/////////////////////////////////////////////////////////////////////////////
// popisovač sprajtu v souboru (40 B)

typedef struct SPRITEFILE_
{
	char	Ident[4];				// (4) identifikátor (= "PSPR")
	long	Faze;					// (4) počet fází celkem
	long	Smer;					// (4) počet směrů
	long	Klid;					// (4) z toho počet klidových fází
	WORD	Width;					// (2) šířka obrázku
	WORD	Height;					// (2) výška obrázku
	long	Delay;					// (4) prodleva mezi fázemi v milisekundách
	long	Level;					// (4) hladina k vykreslování
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

// statická inicializace sprajtů
void InitSprite();


/////////////////////////////////////////////////////////////////////////////
// sprajt

class CSprite
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné - pouze ukazatel na data
	SPRITEDATA*		pData;					// ukazatel na záhlaví sprajtu

// připojení dat
	inline void Attach(SPRITEDATA* data)
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
			CPicture* pic = pData->Data;
			for (int i = pData->Faze * pData->Smer; i > 0; i--)
			{
				pic->Term();
				pic++;
			}
			MemFree(pData);
#ifdef _DEBUG
			pData = NULL;
#endif
		}
	}

// vytvoření nového bufferu - starý buffer musí být odpojen! (obrázky nastaveny na prázdné)
	inline void NewBuffer(int faze, int smer)
	{
		ASSERT((faze > 0) && (smer > 0));
		if (faze <= 0) faze = 1;
		if (smer <= 0) smer = 1;
		SPRITEDATA* data = (SPRITEDATA*)MemGet(faze*smer*sizeof(CPicture) + SIZEOFSPRITEDATA);
		data->Refer = 1;				// počet referencí
		data->Faze = faze;				// počet fází
		data->Smer = smer;				// počet směrů
		data->Klid = 1;					// počet klidových fází
		data->Delay = SPRITEDEFDELAY;	// prodleva mezi fázemi
		data->Level = SPRITEDEFLEVEL;	// hladina k překreslování
		data->Kroku = SPRITEDEFKROKU;	// počet fází na jednotku
		pData = data;					// adresa dat

		CPicture* pic = data->Data;
		for (int i = faze*smer; i > 0; i--)
		{
			pic->Init();
			pic++;
		}
	}

// ---------------------------- veřejné funkce ------------------------------

public:

// konstruktor a destruktor
	CSprite();								// standardní konstruktor
	CSprite(const CSprite& src);			// kopírovací konstruktor
	CSprite(int faze, int smer);			// konstruktor s vytvořením sprajtu
	~CSprite();								// standardní destruktor

// konstruktor a destruktor
	void Init();							// statický konstruktor
	void Init(SPRITEDATA* data);			// statický konstruktor se zadáním dat
	void Init(int faze, int smer);			// statický konstruktor s vytvořením sprajtu
	void Term();							// statický destruktor

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

// poskytnutí/nastavení prodlevy mezi dvěma fázemi
	inline int Delay() const { return pData->Delay; };
	inline void Delay(int delay) { pData->Delay = delay; };

// poskytnutí/nastavení hladiny zobrazení sprajtu
	inline int Level() const { return pData->Level; };
	inline void Level(int level) { pData->Level = level; };

// poskytnutí/nastavení počtu kroků na jednotku (0 = ihned)
	inline double Kroku() const { return pData->Kroku; };
	inline void Kroku(double kroku) { pData->Kroku = kroku; };

// poskytnutí šířky obrázku sprajtu
	inline int Width() const { return pData->Data[0].Width(); };

// poskytnutí výšky obrázku sprajtu
	inline int Height() const { return pData->Data[0].Height(); };

// poskytnutí velikosti dat sprajtu - data obrázků bez záhlaví
	inline int Size() const { return Faze() * Smer() * Width() * Height(); };

// kopie do vlastního bufferu před modifikací
	void CopyWrite();

// vytvoření nového sprajtu (připraveno k zápisu, data jsou náhodná)
	void New(int faze, int smer);

// nastavení nových rozměrů sprajtu (počet fází pro klid a pohyb, počet směrů)
	void ReSize(int klid, int pohyb, int smer);

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
	const CPicture& _fastcall Get(const int off) const;
	const CPicture& _fastcall Get(const int faze, const int smer) const;

// nastavení prvku (s kontrolou platnosti offsetu/indexu)
	void _fastcall Set(const int off, const CPicture& data);
	void _fastcall Set(const int faze, const int smer, const CPicture& data);

// načtení sprajtu ze souboru (TRUE=operace OK)
	BOOL LoadFile(CString jmeno);

// uložení do souboru (FALSE=chyba)
	BOOL SaveFile(CString jmeno) const;

// operátor přiřazení
	const CSprite& operator= (const CSprite& src);
};

/***************************************************************************\
*																			*
*								Buffer sprajtů								*
*																			*
\***************************************************************************/

class CBufSprite
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné
	CSprite*	m_Data;		// ukazatel na data
	BYTE*		m_Valid;	// příznaky platnosti položek
	int			m_Num;		// počet platných položek v bufferu
	int			m_Max;		// velikost bufferu (položek)
	int			m_Next;		// příští volná položka (-1=není)

// vytvoření nové položky
	inline int NewItem()
	{
		if (m_Next < 0)				// není další položka?
		{
			NewData();				// vytvoření nových dat
		}

		int i = m_Next;				// příští volna položka
		m_Next = *(int*)(m_Data + i); // další položka
		m_Valid[i] = TRUE;			// nastavení příznaku platnosti položky
		m_Num++;					// zvýšení čítače platných položek
		return i;
	};

// vytvoření nových dat (odděleno kvůli lepší optimalizaci)
	void NewData();

// ---------------------------- veřejné funkce ------------------------------

public:

// konstruktor a destruktor
	CBufSprite();
	~CBufSprite();

// statický konstruktor a destruktor
	void Init();			// statický konstruktor
	void Term();			// statický destruktor

// zrušení všech položek v bufferu (ukládání začne opět po řadě od začátku)
	void DelAll();

// poskytnutí bufferu sprajtů
	inline CSprite* Data() const { return m_Data; };

// poskytnutí tabulky platnosti (hodnoty TRUE a FALSE)
	inline BYTE* Valid() const { return m_Valid; };

// poskytnutí počtu platných položek v bufferu
	inline int Num() const { return m_Num; };

// poskytnutí velikosti bufferu (včetně zrušených položek)
	inline int Max() const { return m_Max; };

// kontrola platnosti položky
	inline BOOL IsValid(const int index) const
		{ return (((DWORD)index < (DWORD)m_Max) && m_Valid[index]); };

	inline BOOL IsNotValid(const int index) const
		{ return (((DWORD)index >= (DWORD)m_Max) || !m_Valid[index]); };

// poskytnutí přístupu k položce (bez kontroly indexu)
	inline CSprite& operator[] (const int index)
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline const CSprite& operator[] (const int index) const
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline CSprite& At(const int index)
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline const CSprite& At(const int index) const
		{ ASSERT(IsValid(index)); return m_Data[index]; }

// poskytnutí položky (s kontrolou platnosti indexu)
	const CSprite& _fastcall Get(const int index) const;

// nastavení položky (s kontrolou platnosti indexu)
	void _fastcall Set(const int index, const CSprite& data);

// zrušení položky (s kontrolou platnosti indexu)
	void _fastcall Del(const int index);

// vytvoření prázdné položky (vrací index položky)
	int New();
	int New(int faze, int smer);

// přidání položky (vrací index položky)
	int _fastcall Add(const CSprite& data);

// duplikace položky (s kontrolou platnosti indexu, vrací index položky)
	int _fastcall Dup(const int index);

// operátor přiřazení
	const CBufSprite& operator= (const CBufSprite& src);
};

