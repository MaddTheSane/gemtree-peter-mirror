
/***************************************************************************\
*																			*
*								Prvky oken									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// typy prvků oken

enum WINTYP {
	WINITEM_WINDOW,					// okno
	WINITEM_WINDOWRO,				// okno bez uzavření
	WINITEM_BUTTON,					// tlačítko
	WINITEM_DEFBUTTON,				// implicitní tlačítko
	WINITEM_BUTTONICON,				// ikonový spínač
	WINITEM_BUTTONCHECK,			// zaškrtávací tlačítko
	WINITEM_BUTTONCHECK2,			// tlačítkový přepínač
	WINITEM_BUTTON3,				// 3-stavový přepínač
	WINITEM_BUTTONRADIO,			// přepínací tlačítko
	WINITEM_BUTTONRADIO2,			// přepínací tlačítko = začátek skupiny
	WINITEM_EDIT,					// textový řádek
	WINITEM_MEMO,					// textové pole
	WINITEM_GROUP,					// skupina
	WINITEM_LISTBOX,				// seznam
	WINITEM_LISTEDIT,				// seznam s editací
	WINITEM_COMBOBOX,				// rozbalovací seznam
	WINITEM_COMBOEDIT,				// rozbalovací seznam s editací
	WINITEM_WINTEXT,				// text
	WINITEM_WINICON,				// ikona
	WINITEM_WINPIC,					// obrázek
	WINITEM_RICHEDIT,				// textové pole Richedit
	WINITEM_RICHMEMO,				// textové pole bez zalamování
	WINITEM_PROGRESS,				// indikátor průběhu
	WINITEM_HTRACKBAR,				// horizontální táhlo
	WINITEM_VTRACKBAR,				// vertikální táhlo
	WINITEM_TABLE,					// tabulka
	WINITEM_HSCROLLBAR,				// horizontální posuvník
	WINITEM_VSCROLLBAR,				// vertikální posuvník
	WINITEM_TABS,					// záložky
};


/////////////////////////////////////////////////////////////////////////////
// struktura položky prvku okna

typedef struct WINITEM_
{
	HWND	Wnd;					// handle prvku
	int		Parent;					// index rodiče prvku (0=hlavní okno, -1=nic)
	int		Typ;					// typ prvku
	int		Prev;					// předcházející prvek okna (včetně samotného okna)
	int		Next;					// následující prvek okna (včetně samotného okna)
	CString	Text;					// text prvku
	CIcon	Icon;					// ikona
	CPicture Picture;				// obrázek
	BYTE*	PictureData;			// cachovaná data obrázku se zarovnáním na 4 bajty (NULL=není)
	int		Left;					// pozice prvku zleva (v bodech)
	int		Top;					// pozice prvku shora (v bodech)
	int		Width;					// šířka prvku (v bodech)
	int		Height;					// výška prvku (v bodech)
	int		ClientWidth;			// klientská šířka prvku (v bodech)
	int		ClientHeight;			// klientská výška prvku (v bodech)
	bool	HasCaption;				// má titulek
	bool	HasBorder;				// má okraj
	bool	HasSize;				// okno s měnitelnou velikostí (nebo další varianta okraje dialog. prvku)
	bool	AlwaysTop;				// vždy nahoře
	bool	MayClose;				// možné uzavření okna
	bool	TabStop;				// zarážka pro TAB
	bool	Show;					// příznak zobrazení prvku
	bool	Disable;				// příznak zákazu prvku
	bool	Click;					// kliknuto na tlačítko
	bool	TextValid;				// text je platný
	bool	Check;					// přepínač zapnut
	bool	Check3;					// nedefinovaný stav přepínače
	bool	Bold;					// zvýrazněný text
	bool	Italic;					// kurzíva
	bool	Underline;				// podtržení
	bool	Strikeout;				// přeškrtnutí
	bool	Serif;					// patičky
	bool	Fixed;					// text s pevnou roztečí
	int		FontHeight;				// bodová výška znaků (0=běžná)
	double	FontWidth;				// relativní šířka znaků (0=běžná, 1=základní)
	double	Angle;					// úhel sklonu (0=běžný)
	int		FontCol;				// barva textu
	int		FontBack;				// barva pozadí
	HBRUSH	FontBrush;				// štětec pozadí
	HFONT	Font;					// aktuální font
  // tabulka
	bool	TabHead;				// tabulka má záhlaví
	int		TabCol;					// aktivní sloupec tabulky
	int		TabRow;					// aktivní řádek tabulky
	int		TabRowF;				// aktivní řádek tabulky se zaměřením
	int		TabCols;				// počet sloupců tabulky
	int		TabRows;				// počet řádků tabulky
	CString* TabData;				// data tabulky (po řádcích)
	BYTE*	TabAlign;				// zarovnání sloupců tabulky
	int		TabDataMax;				// velikost bufferu seznamu dat tabulky (počet položek)
  // posuvník
	int		ScrollMax;				// maximální hodnota pozice posuvníku (0 až SCROLLRANGE-PAGE)
	double	ScrollPage;				// délka stránky (0.0001 až 1)
	double	ScrollPos;				// aktuální pozice (0 až 1)
} WINITEM;

#define SCROLLRANGE	10000			// rozsah posuvníku

/////////////////////////////////////////////////////////////////////////////
// prvky oken (index 0 = hlavní okno)

class CBufWin
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné
	WINITEM*		m_Data;			// ukazatel na data
	bool*			m_Valid;		// příznaky platnosti položek
	int				m_Num;			// počet platných položek v bufferu
	int				m_Max;			// velikost bufferu (položek)
	int				m_Next;			// příští volná položka (-1 = není)

// vytvoření nové položky
	inline int NewItem()
	{
		if (m_Next < 0)				// není další položka?
		{
			NewData();				// vytvoření nových dat
		}

		int i = m_Next;				// příští volna položka
		m_Next = *(int*)(m_Data + i); // další položka
		m_Valid[i] = true;			// nastavení příznaku platnosti položky
		WINITEM* item = m_Data + i;
		MemFill(item, sizeof(WINITEM), 0);
		item->Text.Init();			// inicializace textu
		item->Icon.Init();
		item->Picture.Init();
		item->PictureData = NULL;
		item->FontBrush = NULL;
		item->ScrollMax = (SCROLLRANGE*9 + 5)/10;
		item->ScrollPage = 0.1;
		item->Prev = i;
		item->Next = i;
		m_Num++;					// zvýšení čítače platných položek
		return i;
	};

// vytvoření nových dat (odděleno kvůli lepší optimalizaci)
	void NewData();

// ---------------------------- veřejné funkce ------------------------------

public:

// konstruktor a destruktor
	CBufWin();
	~CBufWin();

// statický konstruktor a destruktor
	void Init();			// statický konstruktor
	void Term();			// statický destruktor

// zrušení všech položek v bufferu (ukládání začne opět po řadě od začátku)
	void DelAll();

// poskytnutí bufferu dat
	inline WINITEM* Data() const { return m_Data; };

// poskytnutí pole platností
	inline bool* Valid() const { return m_Valid; };

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
	inline WINITEM& operator[] (const int index)
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline const WINITEM& operator[] (const int index) const
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline WINITEM& At(const int index)
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline const WINITEM& At(const int index) const
		{ ASSERT(IsValid(index)); return m_Data[index]; }

// zrušení položky (s kontrolou platnosti indexu)
	void _fastcall Del(const int index);

// vytvoření prázdné položky (vrací index položky)
	int _fastcall New();
};
