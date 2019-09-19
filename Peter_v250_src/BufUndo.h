
/***************************************************************************\
*																			*
*								Buffer undo/redo							*
*																			*
\***************************************************************************/

// položka UNDO/REDO bufferu (32 bajtů)
typedef struct UNDOITEM_
{
	BOOL	Stop;					// (4) příznak konce záznamu operace
	long	Oper;					// (4) prováděná operace
	long	BufID;					// (4) ID bufferu, ve kterém se operace provádí
	long	Index;					// (4) index položky, se kterou se operace provádí
	long	DatBufID;				// (4) ID bufferu uschovaných dat (-1 = není)
	long	DatIndex;				// (4) index uschovaných dat (-1 = nejsou data)
	long	Param1;					// (4) parametr 1 (při přesunu původní rodič)
	long	Param2;					// (4) parametr 2 (při přesunu původní předchozí položka)
} UNDOITEM;

#define SIZEOFUNDOITEM	(sizeof(BOOL) + 7*sizeof(long)) // velikost položky

enum UndoOper {
	UNDO_DELETE,					// zrušení položky
	UNDO_INSERT,					// vložení položky
	UNDO_MOVE,						// přesun položky v bufferu
	UNDO_LOCK,						// přepnutí přiznaku LOCK položky
	UNDO_OFF,						// přepnutí příznaku OFF položky
	UNDO_NAME,						// změna jména položky
	UNDO_NAMESET,					// nastavení jména položky
	UNDO_ICON,						// změna ikony položky
	UNDO_SET,						// změna dat
	UNDO_SETFIX,					// změna dat zafixovaná proti minimalizaci
	UNDO_SETICON,					// změna ikony plochy
	UNDO_SETPIC,					// změna obrázku sprajtu
};


/////////////////////////////////////////////////////////////////////////////
// UNDO buffer

class CBufUndo
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné
	UNDOITEM*	m_UndoData;			// ukazatel na buffer UNDO položek
	int			m_UndoNum;			// počet UNDO položek v bufferu
	int			m_UndoMax;			// velikost UNDO bufferu (položek)
	int			m_UndoSize;			// celková velikost dat pro UNDO
	int			m_UndoLimit;		// omezení velikosti dat pro UNDO
	int			m_UndoRecs;			// počet záznamů v UNDO bufferu

	UNDOITEM*	m_RedoData;			// ukazatel na buffer REDO položek
	int			m_RedoNum;			// počet REDO položek v bufferu
	int			m_RedoMax;			// velikost REDO bufferu (položek)
	int			m_RedoRecs;			// počet záznamů v REDO bufferu

	CBufProg	m_Prog;				// (BufEdiID) programový buffer
	CBufReal	m_Real;				// (BufNumID) buffer čísel
	CBufMultiText	m_Text;			// (BufTxtID) buffer textů
	CBufBool	m_Bool;				// (BufLogID) buffer logických hodnot
	CBufIcon	m_Icon;				// (BufIcoID) buffer ikon
	CBufMap		m_Map;				// (BufMapID) buffer ploch
	CBufPic		m_Picture;			// (BufPicID) buffer obrázků
	CBufSprite	m_Sprite;			// (BufSprID) buffer sprajtů
	CBufSound	m_Sound;			// (BufSndID) buffer zvuků
	CBufMusic	m_Music;			// (BufMusID) buffer hudby

	BOOL		m_Play;				// probíhá přehrávání UNDO/REDO záznamu
	BOOL		m_Undo;				// zaznamenávat do UNDO bufferu (jinak REDO)
	BOOL		m_Redraw[PROGBUFNUM]; // příznaky zapnutí překreslení

	int			m_EditMode;			// buffer editačního módu
	int			m_EditIndex;		// index editačního módu
	int			m_SelectMode;		// buffer k výběru
	int			m_SelectIndex;		// index k výběru
	int			m_MapMode;			// mód k nastavení v ploše

// zjištění přibližné velikosti datové položky (jen kvůli kontrole přetečení)
	int _fastcall GetSize(int bufID, int index);

// přičtení velikosti položky (jen pokud je záznam do UNDO)
	void _fastcall AddSize(UNDOITEM* item);

// zrušení datové položky z bufferů
	void _fastcall DelData(int bufID, int index);

// vypnutí příznaků překreslení okna
	void NulRedraw();

// vypnutí/zapnutí překreslování okna
	void SetRedraw(int bufID, BOOL redraw);

// vyprázdnění bufferu REDO (není-li přehrávání)
	void NulRedo();

// test, zda je položka pro změnu dat již v bufferu
	bool TestSet(int bufID, int index);

// označení editované položky
	void SelectEdit(int bufID, int index);


// ---------------------------- veřejné funkce ------------------------------

public:

// konstruktor a destruktor
	CBufUndo();
	~CBufUndo();

// statický konstruktor a destruktor
	void Init();
	void Term();

// zrušení všech položek v bufferu
	void DelAll();

// poskytnutí dat undo
	inline UNDOITEM* DataUndo() const { return m_UndoData; };

// poskytnutí dat redo
	inline UNDOITEM* DataRedo() const { return m_RedoData; };

// poskytnutí počtu položek undo
	inline int NumUndo() const { return m_UndoNum; };

// poskytnutí počtu položek redo
	inline int NumRedo() const { return m_RedoNum; };

// poskytnutí velikosti bufferu undo
	inline int MaxUndo() const { return m_UndoMax; };

// poskytnutí velikosti bufferu redo
	inline int MaxRedo() const { return m_RedoMax; };

// poskytnutí počtu záznamů undo
	inline int RecsUndo() const { return m_UndoRecs; };

// poskytnutí počtu záznamů redo
	inline int RecsRedo() const { return m_RedoRecs; };

// poskytnutí velikosti dat
	inline int Size() const { return m_UndoSize; };

// poskytnutí/nastavení omezení velikosti dat
	inline int Limit() const { return m_UndoLimit; };
	inline void Limit(int limit) { m_UndoLimit = limit; };

// poskytnutí příznaku, že probíhá přehrávání záznamu
	inline BOOL Play() const { return m_Play; };

// kontrola platnosti položky
	inline BOOL IsValidUndo(const int index) const
		{ return ((DWORD)index < (DWORD)m_UndoMax); };

	inline BOOL IsNotValidUndo(const int index) const
		{ return ((DWORD)index >= (DWORD)m_UndoMax); };

	inline BOOL IsValidRedo(const int index) const
		{ return ((DWORD)index < (DWORD)m_RedoMax); };

	inline BOOL IsNotValidRedo(const int index) const
		{ return ((DWORD)index >= (DWORD)m_RedoMax); };

// poskytnutí přístupu k položce (bez kontroly indexu)
	inline UNDOITEM& AtUndo(const int index)
		{ ASSERT(IsValidUndo(index)); return m_UndoData[index]; }

	inline const UNDOITEM& AtUndo(const int index) const
		{ ASSERT(IsValidUndo(index)); return m_UndoData[index]; }

	inline UNDOITEM& AtRedo(const int index)
		{ ASSERT(IsValidRedo(index)); return m_RedoData[index]; }

	inline const UNDOITEM& AtRedo(const int index) const
		{ ASSERT(IsValidRedo(index)); return m_RedoData[index]; }

// zrušení poslední položky z undo/redo
	void DelUndo();
	void DelRedo();

// vytvoření nové položky (vrací NULL=chyba paměti)
	UNDOITEM* New();

// redukce velikosti UNDO bufferu
	void Reduct();

// ukončení záznamu operací (nastaví příznak konce posloupnosti)
	void Stop();

// aktualizace voleb undo a redo
	void UpdateUndoRedo();

// provedení operace UNDO
	void Undo();

// provedení operace REDO
	void Redo();

// záznam o přidání položky do programového bufferu (vrací FALSE=chyba paměti)
	bool AddProgIns(int bufID, int index);

// záznam o zrušení položky z programového bufferu (vrací FALSE=chyba paměti)
	bool AddProgDel(int bufID, int index, PROGITEM* data);

// záznam o přesunu položky v programovém bufferu (vrací FALSE=chyba paměti)
	bool AddProgMove(int bufID, int index, int oldparent, int oldprev);

// záznam o změně příznaku LOCK prvku (vrací FALSE=chyba paměti)
	bool AddProgLock(int bufID, int index);

// záznam o změně příznaku OFF prvku (vrací FALSE=chyba paměti)
	bool AddProgOff(int bufID, int index);

// záznam o změně jména prvku (vrací FALSE=chyba paměti)
	bool AddProgName(int bufID, int index, int name);

// záznam o nastavení jména prvku (vrací FALSE=chyba paměti)
	bool AddProgNameSet(int bufID, int index, const CMultiText& data);
	bool AddProgNameSet(int bufID, int index, const CText& data);

// záznam o změně ikony prvku (vrací FALSE=chyba paměti)
	bool AddProgIcon(int bufID, int index, int icon);

// záznam o přidání položky do bufferu čísel (vrací FALSE=chyba paměti)
	bool AddRealIns(int index);

// záznam o zrušení položky z bufferu čísel (vrací FALSE=chyba paměti)
	bool AddRealDel(int index, double data);
	inline bool AddDel(int index, double data) { return AddRealDel(index, data); }

// záznam o změně dat (vrací FALSE=chyba paměti)
	bool AddRealSet(int index, double data);

// záznam o přidání položky do bufferu textů (vrací FALSE=chyba paměti)
	bool AddTextIns(int index);

// záznam o zrušení položky z bufferu textů (vrací FALSE=chyba paměti)
	bool AddTextDel(int index, const CMultiText& data);
	bool AddTextDel(int index, const CText& data);
	inline bool AddDel(int index, const CMultiText& data) { return AddTextDel(index, data); }
	inline bool AddDel(int index, const CText& data) { return AddTextDel(index, data); }

// záznam o změně dat (vrací FALSE=chyba paměti)
	bool AddTextSet(int index, const CMultiText& data);
	bool AddTextSet(int index, const CText& data);

// záznam o přidání položky do bufferu logických hodnot (vrací FALSE=chyba paměti)
	bool AddBoolIns(int index);

// záznam o zrušení položky z bufferu logických hodnot (vrací FALSE=chyba paměti)
	bool AddBoolDel(int index, BOOL data);
	inline bool AddDel(int index, const BOOL& data) { return AddBoolDel(index, data); }

// záznam o změně dat (vrací FALSE=chyba paměti)
	bool AddBoolSet(int index, BOOL data);

// záznam o přidání položky do bufferu ikon (vrací FALSE=chyba paměti)
	bool AddIconIns(int index);

// záznam o zrušení položky z bufferu ikon (vrací FALSE=chyba paměti)
	bool AddIconDel(int index, const CIcon& data);
	inline bool AddDel(int index, const CIcon& data) { return AddIconDel(index, data); }

// záznam o změně dat (vrací FALSE=chyba paměti)
	bool AddIconSet(int index, const CIcon& data);

// záznam o přidání položky do bufferu ploch (vrací FALSE=chyba paměti)
	bool AddMapIns(int index);

// záznam o zrušení položky z bufferu ploch (vrací FALSE=chyba paměti)
	bool AddMapDel(int index, const CMap& data);
	inline bool AddDel(int index, const CMap& data) { return AddMapDel(index, data); }

// záznam o změně dat (vrací FALSE=chyba paměti)
	bool AddMapSet(int index, const CMap& data, int mode);

// záznam o změně ikony v ploše (při zrušení ikony z plochy) (vrací FALSE=chyba paměti)
	bool AddMapIcoSet(int index, int offs, int icon);

// záznam o přidání položky do bufferu obrázků (vrací FALSE=chyba paměti)
	bool AddPicIns(int index);

// záznam o zrušení položky z bufferu obrázků (vrací FALSE=chyba paměti)
	bool AddPicDel(int index, const CPicture& data);
	inline bool AddDel(int index, const CPicture& data) { return AddPicDel(index, data); }

// záznam o změně dat (vrací FALSE=chyba paměti)
	bool AddPicSet(int index, const CPicture& data);

// záznam o přidání položky do bufferu sprajtů (vrací FALSE=chyba paměti)
	bool AddSprIns(int index);

// záznam o zrušení položky z bufferu sprajtů (vrací FALSE=chyba paměti)
	bool AddSprDel(int index, const CSprite& data);
	inline bool AddDel(int index, const CSprite& data) { return AddSprDel(index, data); }

// záznam o změně dat (vrací FALSE=chyba paměti)
	bool AddSprSet(int index, const CSprite& data);

// záznam o změně dat - obrázek sprajtu (vrací FALSE=chyba paměti)
	bool AddSprPicSet(int index, int pic, CPicture& data);

// záznam o přidání položky do bufferu zvuků (vrací FALSE=chyba paměti)
	bool AddSndIns(int index);

// záznam o zrušení položky z bufferu zvuků (vrací FALSE=chyba paměti)
	bool AddSndDel(int index, const CSound& data);
	inline bool AddDel(int index, const CSound& data) { return AddSndDel(index, data); }

// záznam o změně dat (vrací FALSE=chyba paměti)
	bool AddSndSet(int index, const CSound& data);

// záznam o přidání položky do bufferu hudby (vrací FALSE=chyba paměti)
	bool AddMusIns(int index);

// záznam o zrušení položky z bufferu hudby (vrací FALSE=chyba paměti)
	bool AddMusDel(int index, const CMusic& data);
	inline bool AddDel(int index, const CMusic& data) { return AddMusDel(index, data); }

// záznam o změně dat (vrací FALSE=chyba paměti)
	bool AddMusSet(int index, const CMusic& data);

// zafixování posledního záznamu o změně dat prodi minimalizaci
	void Fixup();

// přehrátí jedné položky
	void Play(UNDOITEM* item);
};
