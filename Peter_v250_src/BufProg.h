
/***************************************************************************\
*																			*
*							Buffer programu									*
*																			*
\***************************************************************************/

#define		PROGBUFNUM	6				// počet programových bufferů

// identifikace bufferů (neměnit pořadí - muselo by se změnit při ukládání souboru!)
#define		BufIntID	0				// interní buffer seznamu funkcí (nemá strom)
#define		BufClsID	1				// buffer tříd objektů
#define		BufObjID	2				// buffer globálních objektů
#define		BufLocID	3				// buffer lokálních objektů
#define		BufEdiID	4				// buffer editace
#define		BufStrID	5				// buffer struktur

// datové buffery (musí odpovídat indexům v SaveHead !!!!)
#define		BufNumID	PROGBUFNUM		// (6) buffer čísel
#define		BufTxtID	PROGBUFNUM+1	// (7) buffer textů
#define		BufLogID	PROGBUFNUM+2	// (8) buffer logických proměnných
#define		BufIcoID	PROGBUFNUM+3	// (9) buffer ikon
#define		BufMapID	PROGBUFNUM+4	// (10) buffer ploch
#define		BufPicID	PROGBUFNUM+5	// (11) buffer obrázků
#define		BufSprID	PROGBUFNUM+6	// (12) buffer sprajtů
#define		BufSndID	PROGBUFNUM+7	// (13) buffer zvuků
#define		BufMusID	PROGBUFNUM+8	// (14) buffer hudby
#define		BufPalID	PROGBUFNUM+9	// (15) buffer palet

// Při změně změnit definice i v Loader !!!!!

#define	PR_LITE		0x0000		// prvek vypnut pro LITE verzi

/////////////////////////////////////////////////////////////////////////////
// definice stavů ikon

enum STATE {
	STATE_NONE = 0,
	STATE_DEF,
	STATE_SET,						// nastavení
	STATE_SET_OPT,					// nepovinné nastavení
};


/////////////////////////////////////////////////////////////////////////////
// struktura položky programu - 64 bajtů

typedef struct PROGITEM_
{
	int			Func;				// identifikační číslo funkce (= IDF_...)

	int			SrcMask;			// maska zdrojových vlastností
	int			DstMask;			// maska cílových vlastností
	int			Param;				// parametry položky

	int			RefBlok;			// referenční blok (-1 = není)
	int			RefIndex;			// referenční index
	int			DatBlok;			// datový blok (-1 = není)
	int			DatIndex;			// datový index

	int			Parent;				// index rodiče (-1 = není)
	int			Next;				// index následující položky (-1 = není)
	int			Prev;				// index předcházející položky (-1 = není)
	int			Child;				// index prvniho potomka (-1 = není)

	int			Icon;				// index ikony (-1 = implicitní)
	int			Name;				// index jména (-1 = ímplicitní)

	HTREEITEM	HTree;				// handle položky stromu (NULL=není)

	int			Refer;				// čítač referencí na položku
} PROGITEM;

#define	SIZEOFPROGITEM	(15*sizeof(int) + sizeof(HTREEITEM))	// velikost položky

// zdrojové a cílové vlastnosti:
#define	PR_COMMAND	0x0001		// je příkazem/přijímá příkazy
#define	PR_EXPRES	0x0002		// je číselným výrazem/přijímá číselný výraz
#define	PR_TEXT		0x0004		// je textem/přijímá text
#define	PR_BOOL		0x0008		// je logickým výrazem/přijímá logický výraz
#define	PR_ICON		0x0010		// je ikonou/přijímá ikonu
#define	PR_MAP		0x0020		// je plochou/přijímá plochu
#define	PR_PIC		0x0040		// je obrázkem/přijímá obrázky
#define	PR_SPRITE	0x0080		// je sprajtem/přijímá sprajty
#define	PR_SOUND	0x0100		// je zvukem/přijímá zvuky
#define	PR_MUSIC	0x0200		// je hudbou/přijímá hudbu

#define	PR_DIGIT	0x0400		// je číslicí/přijímá číslice

#define PR_ALLDATA	(	PR_EXPRES |	\
						PR_TEXT |	\
						PR_BOOL |	\
						PR_ICON |	\
						PR_MAP |	\
						PR_PIC |	\
						PR_SPRITE |	\
						PR_SOUND |	\
						PR_MUSIC)	// všechny možnosti dat (kromě příkazu)

#define	PR_ALL		(PR_ALLDATA | PR_COMMAND) // všechny možnosti vlastností (funkce, skupina)

// parametry a vlastnosti
#define	PR_EXP		0x0001		// potomci prvku jsou rozvinuti
#define	PR_LOCK		0x0002		// prvek uzamknut
#define	PR_LOCK_DEP	0x0004		// prvek závisle uzamknut
#define	PR_OFF		0x0008		// prvek vypnut
#define	PR_OFF_DEP	0x0010		// prvek závisle vypnut
#define	PR_ONE		0x0020		// prvek může mít jen 1 potomka
#define	PR_INTERN	0x0040		// zákaz rušení a přesunu
#define	PR_NOMOVE	0x0080		// zákaz rušení, přesunu a kopie
#define PR_NEW		0x0100		// nová položka (bude přepsána z cliboard::paste)
#define PR_SETPAR	0x0200		// je-li možný parametr, je nutně požadován
#define PR_PARPAR	0x0400		// má parametr při použití jako příkaz i jako parametr
#define PR_CD		0x0800		// je alternativní knihovna
#define PR_ALTEXT	0x1000		// je alternativní přípona souboru (JPG, MP3, RMI)
#define PR_CD2		0x2000		// je alternativní knihovna 2

#define PR_NOREFER	0x4000		// nedělat reference na prvek

#define NOREFER		0x4000		// příznak v tabulce struktur

/////////////////////////////////////////////////////////////////////////////
// položka inicializační tabulky stromů

typedef struct INITTREE_ {
	short	Level;			// hladina (0 = ROOT, NOREFER=nedělat reference)
	short	Func;			// identifikace funkce (IDF_...)
} INITTREE;


/////////////////////////////////////////////////////////////////////////////
// buffer programu

class CBufProg
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné
	PROGITEM*	m_Data;			// buffer dat
	bool*		m_Valid;		// příznaky platnosti položek
	int			m_Num;			// počet platných položek v bufferu
	int			m_Max;			// velikost bufferu (položek)
	int			m_Next;			// příští volná položka (-1 = není)
	bool		m_Undo;			// požadavek registrace změn pro UNDO

	int			m_Redraw;		// čítač pro vypnutí překreslování okna (0=zapnuto)
	int			m_First;		// index první (ROOT) položky (-1 = není)
	HWND		m_Tree;			// připojený strom (NULL = není)
	int			m_Disp;			// rodič zobrazení položek (-1 = vše, -2 = nic)
	int			m_IconWidth;	// šířka ikony (0 = není)
	int			m_IconHeight;	// výška ikony (0 = není)
	int			m_BufID;		// identifikace tohoto bufferu
	int			m_Def;			// definovaná položka (-1 = není)
	CText		m_Nadpis;		// nadpis okna

	CFont		m_FontSet;		// uschované nastavení fontu
	DWORD		m_CharSet;		// znaková sada fontu
	HFONT		m_Font;			// font (NULL=není)

// vytvoření nových dat (vrací TRUE=operace OK) (odděleno kvůli lepší optimalizaci)
	bool NewData();

// vytvoření nové položky (vrací index položky, při chybě vrací -1)
	int NewItem();

// zrušení položky - zařazení do volných položek (nekontroluje index)
	void _fastcall DelItem(const int index);

// příprava indexu stavové ikony (upravit s INDEXTOSTATEIMAGEMASK() !)
	int GetStateImage(int index);

// nové zobrazení položky
	void DispNewItem(int index);

// zajištění nových dat u kopírované položky (voláno též z Clipboard::Paste)
	void CopyNewData(PROGITEM* item);

// duplikace dat u kopírované položky
	void CopyDupData(PROGITEM* item);

// ---------------------------- veřejné funkce ------------------------------

public:

// konstruktor a destruktor
	CBufProg();
	~CBufProg();

// statický konstruktor a destruktor
	void Init();
	void Term();

// zrušení všech položek v bufferu (ukládání začne opět po řadě od začátku, nemění obsah okna)
	void DelAll();

// poskytnutí bufferu dat
	inline PROGITEM* Data() const { return m_Data; };

// poskytnutí bufferu platnosti (hodnoty TRUE a FALSE)
	inline bool* Valid() const { return m_Valid; };

// poskytnutí počtu platných položek v bufferu
	inline int Num() const { return m_Num; };

// poskytnutí velikosti bufferu (včetně zrušených položek)
	inline int Max() const { return m_Max; };

// nastavení požadavku registrace změn pro UNDO
	inline void UndoOn() { m_Undo = true; };
	inline void UndoOff() { m_Undo = false; };
	inline void UndoSet(const bool undo) { m_Undo = undo; };

// kontrola platnosti položky
	inline BOOL IsValid(const int index) const
		{ return (((DWORD)index < (DWORD)m_Max) && m_Valid[index]); };

	inline BOOL IsNotValid(const int index) const
		{ return (((DWORD)index >= (DWORD)m_Max) || !m_Valid[index]); };

// poskytnutí přístupu k položce (bez kontroly indexu)
	inline PROGITEM& operator[] (const int index)
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline const PROGITEM& operator[] (const int index) const
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline PROGITEM& At(const int index)
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline const PROGITEM& At(const int index) const
		{ ASSERT(IsValid(index)); return m_Data[index]; }

// vypnutí/zapnutí překreslování (musí být spárováno!)
	void RedrawOff();
	void RedrawOn();

// poskytnutí indexu první ROOT položky (-1 = není)
	inline int First() const { return m_First; };

// poskytnutí/nastavení handle stromu (neaktualizuje zobrazení, zneplatní ikony)
	inline HWND Tree() const { return m_Tree; };
	inline void Tree(const HWND tree)
	{ 
		if (tree != m_Tree)
		{
			m_Tree = tree; 
			m_IconWidth = 0; 
			m_IconHeight = 0; 
			m_FontSet.Height = -1;
			AktFont();
		}
	};

// aktualizace fontu v okně
	void AktFont();

// poskytnutí/nastavení rodiče zobrazení položek (aktualizuje zobrazení, -1 = vše, -2 = nic)
	inline int Disp() const { return m_Disp; };
	void Disp(int disp);

// poskytnutí/nastavení rozměrů ikon (zoom: TRUE = ikony 32*32)
	inline int IconWidth() const { return m_IconWidth; };
	inline void IconWidth(int width) { m_IconWidth = width; };
	inline int IconHeight() const { return m_IconHeight; };
	inline void IconHeight(int height) { m_IconHeight = height; };
	void IconList(SMALLICON zoom);

// poskytnutí/nastavení ID bufferu
	inline int BufID() const { return m_BufID; };
	inline void BufID(int id) { m_BufID = id; };

// poskytnutí/nastavení definované položky (-1 = odznačení)
	inline int Def() const { return m_Def; };
	void Def(int index);

// poskytnutí/nastavení nadpisu okna
	const CText& Nadpis() const { return m_Nadpis; };
	void Nadpis(const CText& text);

// poskytnutí/nastavení první zobrazené položky (-1 = není)
	int Top();
	void Top(int index);

// poskytnutí/nastavení vybrané položky (-1 = není)
	int Select();
	void Select(int index);

// aktualizace zobrazení stavové ikony položky pro okno editoru programu
	void _fastcall AktStateImage(int index);

// index následující položky ve stromu (cyklicky, -1 = navrátí se první položka)
	int _fastcall NextItem(int index);

// index předcházející položky ve stromu (cyklicky, -1 = navrátí se poslední položka)
	int _fastcall PrevItem(int index);

// navrácení položky v UNDO operaci (vrací TRUE=operace OK)
// provádí záznam do UNDO bufferu
	bool UndoIns(const PROGITEM* item, const int index);

// vložení položky bez navázání dat (pro UNDO buffer)
// vrací index položky, při chybě paměti vrací -1
	int Insert0(const PROGITEM* item);

// zrušení položky bez navázání dat (pro UNDO buffer)
	void Del0(int index);

// vložení položky (vrací index položky) - parametry nemusí být platné
//  parent = rodič (-1=ROOT), after=předcházející položka 
//		(-1=první, -2=poslední, -3=abecedně, -4=abecedně s předností skupin)
//  Položky nastavené funkcí: Parent, Next, Prev, Child, HTree, Refer,
	int Insert(const PROGITEM* item, int parent, int after);

// načtení inicializační tabulky bufferu (starý obsah bufferu se neruší - lze přidávat)
// při chybě paměti vrací FALSE
	bool InitTab(const INITTREE* tab, const int num);

// vytvoření prázdné položky (vrací index položky, -1=chyba paměti)
	int New();

// nalezení položky podle handle (-1 = není)
	int Find(HTREEITEM htree);

// nalezení položky s ukazatelem na data (-1 = není)
	int SrcDat(int bufID, int index);

// nalezení referující položky od aktuální položky cyklicky (-1 = nenalezeno)
	int SrcRef(int RefBlok, int RefIndex, BOOL next);

// nalezení indexu textu k položce (-1 = neplatný)
	int _fastcall GetText(const int index);

// nalezení indexu ikony k položce (-1 = neplatná)
	int _fastcall GetIcon(const int index);

// poskytnutí počtu referencí pro uživatele
	int GetRefer(int index);

// test povolení zrušení větve z hlediska referencí na položky (TRUE=OK)
	bool TestRefer(int index);

// překreslení ikony ve stromu
	void ReDispIcon(int icon);

// test čísla funkce, zda je to objekt (s jedinečným jménem)
	bool _fastcall TestObj(int func);

// nalezení objektu daného jména (-1 = nenalezeno) - rozlišuje velká/malá písmena
//		name ..... hledaný text
//		over ..... přeskočená položka (-1 = není)
//		first .... první položka (-1 = od začátku)
//		next ..... příští položka pro zastavení (-1 = po konec)
	int FindObj(const CMultiText& name, int over = -1, int first = -1, int next = -1);

// zrušení větve stromu (při chybě paměti vrací FALSE - stav není navrácen!)
	bool Del(int index);

// přesun větve stromu (vrací nový index položky)
// (parent: -1 = ROOT, after: -1 = začátek, -2 = konec)
	int Move(int parent, int after, int src);

// kopie větve stromu - vrací cílový index položky (při chybě vrací <0)
// (parent: -1 = ROOT, pos: -1 = začátek, -2 = konec)
	int Copy(int parent, int pos, int bufID, int src);
};
