
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
	long	Level;					// (4) hladina k vykreslování (0 = předměty)
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

// statická inicializace sprajtů (při chybě paměti vrací FALSE)
BOOL InitSprite();


/////////////////////////////////////////////////////////////////////////////
// sprajt

class CSprite
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné
	SPRITEDATA*		pData;					// (4) ukazatel na záhlaví sprajtu

	bool			m_Visible;				// (1) příznak viditelnosti sprajtu
	bool			m_Moving;				// (1) příznak pohybu sprajtu
	bool			m_DispReq;				// (1) příznak požadavku překreslení
	bool			m_KlidMove;				// (1) je pohyb v klidu

	int				m_X;					// (4) aktuální souřadnice X (v bodech)
	int				m_Y;					// (4) aktuální souřadnice Y (v bodech)

	int				m_Timer;				// (4) čítač hodin pro další fázi
	int				m_KrokCit;				// (4) čítač fází pro celý pohyb
	int				m_FazeN;				// (4) aktuální fáze (pozice ve sprajtu)
	int				m_SmerN;				// (4) aktuální směr (řádek ve sprajtu)

	double			m_CilX;					// (8) cílová souřadnice X (v políčkách)
	double			m_CilY;					// (8) cílová souřadnice Y (v políčkách)

	double			m_AktX;					// (8) aktuální souřadnice X (v políčkách)
	double			m_AktY;					// (8) aktuální souřadnice Y (v políčkách)

	double			m_dX;					// (8) přírustek souřadnice X (v políčkách)
	double			m_dY;					// (8) přírustek souřadnice Y (v políčkách)

	double			m_Smer;					// (8) aktuální směr (v úhlové míře)
	int				m_FazeNext;				// (4) příští fáze (určená uživatelem), mimo rozsah=implicitní

	bool			m_StatusVis;			// (1) příznak zobrazení stavového indikátoru
	bool			m_res1;
	bool			m_res2;
	bool			m_res3;

	double			m_Status;				// (8) stav * 0x40000000 (0 až 1, <= -1 vypnuto)

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
		SPRITEDATA* data = (SPRITEDATA*)MemGet(faze*smer*sizeof(CIcon) + SIZEOFSPRITEDATA);
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

// poskytnutí/nastavení hladiny zobrazení sprajtu (0 = předměty)
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

// poskytnutí/nastavení příznaku viditelnosti
	inline bool Visible() const { return m_Visible; };
	inline void Visible(bool visible) { m_Visible = visible; };

// poskytnutí/nastavení příznaku pohybu
	inline bool Moving() const { return m_Moving; };
	inline void Moving(bool moving) { m_Moving = moving; };

// poskytnutí/nastavení příznaku požadavku překreslení
	inline bool DispReq() const { return m_DispReq; };
	inline void DispReq(bool dispreq) { m_DispReq = dispreq; };

// poskytnutí/nastavení příznaku pohybu v klidu
	inline bool KlidMove() const { return m_KlidMove; };
	inline void KlidMove(bool klidmove) { m_KlidMove = klidmove; };

// poskytnutí/nastavení/dekrementace čítače hodin pro další fázi
	inline int Timer() const { return m_Timer; };
	inline void Timer(int timer) { m_Timer = timer; };
	inline BOOL TimerDec() { m_Timer--; return (m_Timer <= 0); };

// poskytnutí/nastavení/dekrementace čítače fází pro celý pohyb
	inline int KrokCit() const { return m_KrokCit; };
	inline void KrokCit(int krokcit) { m_KrokCit = krokcit; };
	inline BOOL KrokDec() { m_KrokCit--; return (m_KrokCit < 0); };

// poskytnutí/nastavení aktuální fáze
	inline int FazeN() const { return m_FazeN; };
	inline void FazeN(int fazen) { m_FazeN = fazen; };

// poskytnutí/nastavení příští fáze (mimo rozsah=implicitní)
	inline int FazeNext() const { return m_FazeNext; };
	inline void FazeNext(int fazenext) { m_FazeNext = fazenext; };

// poskytnutí/nastavení stavu (rozsah 0 až 1, <0=vypnuto)
	inline double Status() const { return m_Status; };
	inline bool IsStatus() const { return m_StatusVis; };
	inline void Status(double status) { m_Status = status; m_StatusVis = (m_Status > -1); };

// poskytnutí/nastavení aktuálního směru
	inline int SmerN() const { return m_SmerN; };
	inline void SmerN(int smern) { m_SmerN = smern; };

// poskytnutí dat aktuálního obrázku (podle aktuálního směru a fáze, bez kontroly)
	PICTUREDATA* AktPicData();

// poskytnutí/nastavení cílové souřadnice X (v políčkách)
	inline double CilX() const { return m_CilX; };
	inline void CilX(double cilx) { m_CilX = cilx; };
	inline double* PCilX() { return &m_CilX; };

// poskytnutí/nastavení cílové souřadnice Y (v políčkách)
	inline double CilY() const { return m_CilY; };
	inline void CilY(double cily) { m_CilY = cily; };
	inline double* PCilY() { return &m_CilY; };

// poskytnutí/nastavení aktuální souřadnice X (v políčkách)
	inline double AktX() const { return m_AktX; };
	inline void AktX(double aktx) { m_AktX = aktx; };

// poskytnutí/nastavení aktuální souřadnice Y (v políčkách)
	inline double AktY() const { return m_AktY; };
	inline void AktY(double akty) { m_AktY = akty; };

// poskytnutí/nastavení aktuální souřadnice X (v bodech)
	inline int X() const { return m_X; };
	inline void X(int x) { m_X = x; };

// poskytnutí/nastavení aktuální souřadnice Y (v bodech)
	inline int Y() const { return m_Y; };
	inline void Y(int y) { m_Y = y; };

// poskytnutí/nastavení přirustku souřadnice X (v políčkách)
	inline double dX() const { return m_dX; };
	inline void dX(double dx) { m_dX = dx; };

// poskytnutí/nastavení přirustku souřadnice Y (v políčkách)
	inline double dY() const { return m_dY; };
	inline void dY(double dy) { m_dY = dy; };

// poskytnutí/nastavení aktuálního směru (v úhlové míře)
	inline double SmerR() const { return m_Smer; };
	inline void SmerR(double smerr) { m_Smer = smerr; };

// kopie do vlastního bufferu před modifikací
	void CopyWrite();

// vyprázdnění sprajtu (uvolnění dat)
	void Empty();

// test, zda je sprajt prázdný
	inline BOOL IsEmpty() { return ((DWORD)pData == (DWORD)EmptySpriteData); };
	inline BOOL IsNotEmpty() { return ((DWORD)pData != (DWORD)EmptySpriteData); };

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

// načtení sprajtu ze souboru
	void LoadFile();

// uložení do souboru
	void SaveFile() const;

// operátor přiřazení
	const CSprite& operator= (const CSprite& src);

// operátory porovnání
	friend inline BOOL operator==(const CSprite& s1, const CSprite& s2)
		{ return (DWORD)s1.pData == (DWORD)s2.pData; };

	friend inline BOOL operator!=(const CSprite& s1, const CSprite& s2)
		{ return (DWORD)s1.pData != (DWORD)s2.pData; };
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
	CBufSprite();
	~CBufSprite();

// statický konstruktor a destruktor
	void Init();			// statický konstruktor
	void Term();			// statický destruktor

// zrušení všech položek v bufferu (ukládání začne opět po řadě od začátku)
	void DelAll();

// poskytnutí bufferu sprajtů
	inline CSprite* Data() const { return m_Data; };

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

// vyprázdnění položky (bez jejího zrušení - jen pro uvolnění dat)
	void _fastcall Empty(const int index);

// zrušení položek z konce bufferu
	void _fastcall Del(int num);

// vytvoření prázdné položky (vrací index položky)
	int New();
	int New(int faze, int smer);

// přidání položky (vrací index položky)
	int _fastcall Add(const CSprite& data);

// duplikace položky (s kontrolou platnosti indexu, vrací index první položky)
	int _fastcall Dup(const int index);
	int _fastcall Dup(const int index, int num);

// operátor přiřazení
	const CBufSprite& operator= (const CBufSprite& src);
};

