
/***************************************************************************\
*																			*
*							Obsluha editace souboru							*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// položka inicializační tabulky funkcí

typedef struct INITFUNC_ {
	short	SrcMask;		// maska zdrojových vlastností
	short	DstMask;		// maska cílových vlastností
	short	Param;			// parametry
	char*	Import;			// text pro import funkce
} INITFUNC;

// inicializační tabulka funkcí
extern	const INITFUNC InitTabFunc[];

/////////////////////////////////////////////////////////////////////////////
// editační pole

extern	int			FocusBuf;						// okno s fokusem (= číslo bufferu, BufEdiID = editační pole)

extern	int			EditMode;						// aktivní editační pole (= číslo bufferu)
extern	int			EditBuf;						// editovaný buffer
extern	int			EditItem;						// editovaná položka v bufferu
extern	int			EditItemSprite;					// sprajt s editovanou ikonou (úschova pro historii)
extern	int			EditItemPic;					// editovaný obrázek sprajtu (úschova pro historii)

extern	int			EditX;							// souřadnice X počátku editačního pole
extern	int			EditY;							// souřadnice Y počátku editačního pole
extern	int			EditWidth;						// šířka editačního pole
extern	int			EditHeight;						// výška editačního pole

extern	int			ToolMode;						// aktivní editační mód Toolbaru (=číslo bufferu)

extern	UINT		FocusTimer;						// časovač pro nastavení fokusu
extern	int			FocusTimerN;					// čítač pro nastavení fokusu

/////////////////////////////////////////////////////////////////////////////
// hlášení o změně adresáře

extern	HANDLE		FunctionDirChange;				// hlášení o změně adresáře - funkce
extern	HANDLE		NumberDirChange;				// hlášení o změně adresáře - čísla
extern	HANDLE		TextDirChange;					// hlášení o změně adresáře - text
extern	HANDLE		BoolDirChange;					// hlášení o změně adresáře - logické hodnoty
extern	HANDLE		IconDirChange;					// hlášení o změně adresáře - ikony
extern	HANDLE		MapDirChange;					// hlášení o změně adresáře - plochy
extern	HANDLE		PictureDirChange;				// hlášení o změně adresáře - obrázky
extern	HANDLE		SpriteDirChange;				// hlášení o změně adresáře - sprajty
extern	HANDLE		SoundDirChange;					// hlášení o změně adresáře - zvuk
extern	HANDLE		MusicDirChange;					// hlášení o změně adresáře - hudba

extern	HANDLE		FunctionDirChange2;				// hlášení o změně adresáře - alternativní funkce
extern	HANDLE		NumberDirChange2;				// hlášení o změně adresáře - alternativní čísla
extern	HANDLE		TextDirChange2;					// hlášení o změně adresáře - alternativní text
extern	HANDLE		BoolDirChange2;					// hlášení o změně adresáře - alternativní logické hodnoty
extern	HANDLE		IconDirChange2;					// hlášení o změně adresáře - alternativní ikony
extern	HANDLE		MapDirChange2;					// hlášení o změně adresáře - alternativní plochy
extern	HANDLE		PictureDirChange2;				// hlášení o změně adresáře - alternativní obrázky
extern	HANDLE		SpriteDirChange2;				// hlášení o změně adresáře - alternativní sprajty
extern	HANDLE		SoundDirChange2;				// hlášení o změně adresáře - alternativní zvuk
extern	HANDLE		MusicDirChange2;				// hlášení o změně adresáře - alternativní hudba

extern	int			ProgDirChangeIgnore;			// čítač pro ignorování hlášení změn po vlastní změně

extern	int			ProgStatusWidth;				// šířka druhého pole programového řádku

//#define MOUSEINV -100000							// neplatná souřadnice myši
//extern	int			mX;								// souřadnice X myši
//extern	int			mY;								// souřadnice Y myši

extern	BOOL		Modi;							// příznak modifikace souboru

/////////////////////////////////////////////////////////////////////////////
// obsluha kurzoru myši

extern	int		MouseX;								// souřadnice X myši
extern	int		MouseY;								// souřadnice Y myši


/////////////////////////////////////////////////////////////////////////////
// obsluha editace čísla

extern	HWND	EditNumWnd;						// editační okno čísla

/////////////////////////////////////////////////////////////////////////////
// editace číselné položky plochy

extern	HWND	EditMapNumWnd;					// editační okno plochy (NULL=není)

/////////////////////////////////////////////////////////////////////////////
// obsluha editace logických hodnot

extern	HWND	EditLogWnd;						// editační okno logických hodnot

/////////////////////////////////////////////////////////////////////////////
// obsluha editace textu

extern	HWND	EditTextWnd;					// editační okno textu

/////////////////////////////////////////////////////////////////////////////
// posuvníky pro editory

extern	HWND	HScroll;						// horizontální posuvník
extern	HWND	VScroll;						// vertikální posuvník
extern	BOOL	HScrollDisp;					// je horizontální posuvník
extern	BOOL	VScrollDisp;					// je vertikální posuvník
#define	TRACKSIRKA 16							// šířka posuvníku

/////////////////////////////////////////////////////////////////////////////
// globální a lokální proměnné

extern	BOOL		ProgMode;						// příznak módu editace
//extern	HWND		EditName;						// editor jména prvku
//extern	HWND		EditFind;						// editor textu k hledání
extern	int			ToolBarProgN;					// počet základních prvků panelu nástrojů

extern	HWND		TreeCls;						// okno tříd
extern	HWND		TreeObj;						// okno globálních objektů
extern	HWND		TreeLoc;						// okno lokálních objektů
extern	HWND		TreeEdi;						// okno editoru
extern	HWND		TreeStr;						// okno struktur

extern	HIMAGELIST	ProgImageList;					// seznam obrázků
extern	HIMAGELIST	ProgImageListMedium;			// seznam středních obrázků
extern	HIMAGELIST	ProgImageListSmall;				// seznam malých obrázků
extern	HIMAGELIST	ProgStateList;					// seznam stavových ikon
extern	HIMAGELIST	ProgStateListMedium;			// seznam středních stavových ikon
extern	HIMAGELIST	ProgStateListSmall;				// seznam malých stavových ikon


#define CLASSFNCINDEX		0		// index FNC v bufferu tříd
#define CLASSICONINDEX		4		// index ICON v bufferu tříd
#define CLASSMAPINDEX		5		// index MAP v bufferu tříd
#define CLASSPICINDEX		6		// index BACK v bufferu tříd
#define CLASSSPRITEINDEX	9		// index SPRITE v bufferu tříd

/////////////////////////////////////////////////////////////////////////////
// obsluha editace jména prvku

extern	BOOL		EditName;						// příznak probíhající editace

/////////////////////////////////////////////////////////////////////////////
// inicializační buffery (připravují se jen jednou při startu programu)
//
// Při používání bufferů se počítá s tím, že položky jsou uloženy po řadě
// od začátku bufferu, počet položek odpovídá počtu (a pořadí) funkcí.

extern	const int	FncNum;							// počet funkcí
extern	CBufIcon	IconFnc;						// ikony funkcí

extern	BOOL		PreviewPic;						// probíhá preview obrázku

/////////////////////////////////////////////////////////////////////////////
// buffery

extern	CBufProg	BufProg[PROGBUFNUM];			// buffery programu
extern	CBufReal	Real;							// buffer reálných čísel
extern	CBufMultiText	Text;							// buffer textů
extern	CBufBool	Bool;							// buffer logických proměnných
extern	CBufIcon	Icon;							// buffer ikon
extern	CBufIcon	IconState;						// buffer stavových ikon
extern	CBufMap		Map;							// buffer ploch
extern	CBufPic		Picture;						// buffer obrázků
extern	CBufSprite	Sprite;							// buffer sprajtů
extern	CBufSound	Sound;							// buffer zvuků
extern	CBufMusic	Music;							// buffer hudby

extern	CSprite		Sprite0;						// sprajt 0 (Petr)
extern	CSprite		Sprite1;						// sprajt 1 (Petra)
extern	CBufIcon	Icon1;							// buffer s 1 ikonou k zobrazení

//#define		ICONNUM 31								// velikost bufferu ikon (pro TRUECOLOR max. 63!)
//extern	CBufIcon	Icon16;							// buffer ikon s velikostí 16x16
//extern	CBufIndex	Icon16List;						// buffer indexů ikon s velikostí 16x16
//extern	int			Icon16Next;						// příští ikona pro zápis
//extern	CBufIcon	Icon32;							// buffer ikon s velikostí 32x32
//extern	CBufIndex	Icon32List;						// buffer indexů ikon s velikostí 32x32
//extern	int			Icon32Next;						// příští ikona pro zápis

#define	BufInt		BufProg[BufIntID]				// interní buffer
#define	BufCls		BufProg[BufClsID]				// buffer tříd
#define	BufObj		BufProg[BufObjID]				// globální buffer
#define BufLoc		BufProg[BufLocID]				// lokální buffer
#define	BufEdi		BufProg[BufEdiID]				// editační buffer
#define	BufStr		BufProg[BufStrID]				// buffer struktur

extern	CBufUndo	Undo;							// buffer undo a redo

extern	int			GroupIndex;						// index do bufferu BufStr na IDF_GROUP

/////////////////////////////////////////////////////////////////////////////
// combobox panely

extern	HWND		ToolEditIcon;					// panel nástrojů editoru ikon
extern	HWND		ToolEditIconWidth;				// panel nástrojů volby šířky
extern	HWND		ToolEditMap;					// panel nástrojů editoru ploch

/////////////////////////////////////////////////////////////////////////////
// tabulky příkazů v COMBO boxu

extern	int ToolBarEditPicTools[];					// nástroje EDITICON
extern	int ToolBarEditPicWidths[];					// šířky EDITICON
extern	int ToolBarEditMapTools[];					// nástroje EDITMAP


/////////////////////////////////////////////////////////////////////////////
// nastavení menu programu

void ProgSetMenu();

/////////////////////////////////////////////////////////////////////////////
// aktualizace po změně jazyku

void ProgReloadJazyk();


/////////////////////////////////////////////////////////////////////////////
// první inicializace při startu programu (hlavní okno i SELECT je již vytvořeno)

void ProgStartInit();


/////////////////////////////////////////////////////////////////////////////
// zahájení režimu editace souboru

void ProgInit();


/////////////////////////////////////////////////////////////////////////////
// ukončení režimu editace souboru

void ProgTerm();


/////////////////////////////////////////////////////////////////////////////
// zahájení sledování změn adresáře

void BeginDirChange(HANDLE* dir1, HANDLE* dir2, CText& path1, CText& path2);


/////////////////////////////////////////////////////////////////////////////
// update voleb pro ZOOM

void ProgUpdateInOut();


/////////////////////////////////////////////////////////////////////////////
// nastavení editačního módu Toolbar (podle ID bufferu, -1 = vypnout)

void SetToolMode(int bufID);


/////////////////////////////////////////////////////////////////////////////
// nastavení editačního módu edítačního panelu (TRUE=OK)

BOOL SetEditMode(int bufID, int index);


/////////////////////////////////////////////////////////////////////////////
// nastavení fokusu na okno editace

void ProgSetFocus();

/////////////////////////////////////////////////////////////////////////////
// update voleb bloku

void ProgUpdateClipboard();


/////////////////////////////////////////////////////////////////////////////
// update UNDO a REDO

void ProgUpdateUndoRedo();


/////////////////////////////////////////////////////////////////////////////
// překreslení okna

void UpdateWnd(HWND wnd);


/////////////////////////////////////////////////////////////////////////////
// vykreslení nadpisů oken

void ProgDispNadpis();


/////////////////////////////////////////////////////////////////////////////
// aktualizace zobrazení jména závislých položek

void AktNameTree(int bufID, CText& txt, int refBlok, int refIndex);


/////////////////////////////////////////////////////////////////////////////
// překreslení stromů

void UpdateAllTree();


/////////////////////////////////////////////////////////////////////////////
// nastavení fokusu na okno

void ProgOnSetFocus(HWND hWnd);


/////////////////////////////////////////////////////////////////////////////
// ztráta fokusu z okna

//void ProgOnKillFocus(HWND hWnd);


/////////////////////////////////////////////////////////////////////////////
// změna velikosti okna

void ProgOnSize();


/////////////////////////////////////////////////////////////////////////////
// vykreslení nadpisů oken

void ProgDispNadpis();


/////////////////////////////////////////////////////////////////////////////
// vykreslení okna

void ProgOnPaint();


/////////////////////////////////////////////////////////////////////////////
// zjištění informace o ikoně k zobrazení

void ProgOnGetDispInfo(HWND hWnd, TV_DISPINFO* tvd);


/////////////////////////////////////////////////////////////////////////////
// zobrazení položky (např. po hledání)

void DispItem(int bufID, int index);


/////////////////////////////////////////////////////////////////////////////
// zjištění bufferu programu z handle okna (NULL = není)

CBufProg* BufProgFromHandle(HWND hWnd);


/////////////////////////////////////////////////////////////////////////////
// zjištění ID bufferu programu z handle okna (-1 = není)

int BufProgIDFromHandle(HWND hWnd);


/////////////////////////////////////////////////////////////////////////////
// zahájení rozvinutí položky (vrací TRUE=zákaz změny)

BOOL ProgOnExpanding(HWND hWnd, TV_ITEM* tvi, BOOL expand);


/////////////////////////////////////////////////////////////////////////////
// změna rozvinutí položky

void ProgOnExpand(HWND hWnd, TV_ITEM* tvi, BOOL expand);


/////////////////////////////////////////////////////////////////////////////
// změna vybrané položky

void ProgOnSelChanged(HWND hWnd, NM_TREEVIEW* tv);


/////////////////////////////////////////////////////////////////////////////
// posun myši

void ProgOnMouseMove(UINT flags, int x, int y);


/////////////////////////////////////////////////////////////////////////////
// uvolnění tlačítka myši

void ProgOnButtonUp(UINT keys, BOOL right);


/////////////////////////////////////////////////////////////////////////////
// stisk tlačítka myši (vrací TRUE=obslouženo)

BOOL ProgOnButtonDown(UINT flags, int x, int y, BOOL right, BOOL dclick);


/////////////////////////////////////////////////////////////////////////////
// zvětšení měřítka zobrazení

void ProgOnZoomIn();


/////////////////////////////////////////////////////////////////////////////
// zmenšení měřítka zobrazení

void ProgOnZoomOut();


//void ProgOnReIcon();


/////////////////////////////////////////////////////////////////////////////
// dvojklik myší - editace prvku

void ProgOnDblClk(HWND hWnd);


/////////////////////////////////////////////////////////////////////////////
// Enter - editace prvku

void ProgOnEnter(HWND hWnd);


/////////////////////////////////////////////////////////////////////////////
// obsluha modifikace

// zapnutí modifikace
void SetModi();

// vypnutí modifikace
void ResModi();

// podmíněné uložení při modifikaci (TRUE=pokračovat)
BOOL TestModi();


/////////////////////////////////////////////////////////////////////////////
// obsluha zobrazení kurzoru myši (vrací TRUE=nastaveno)

BOOL ProgOnSetCursor();


/////////////////////////////////////////////////////////////////////////////
// přepnutí stavu LOCK prvku pod kurzorem

void ProgOnLock();

/////////////////////////////////////////////////////////////////////////////
// přepnutí stavu OFF prvku pod kurzorem

void ProgOnOff();

/////////////////////////////////////////////////////////////////////////////
// přepnutí stavu DEF_NAME prvku pod kurzorem

void ProgOnDefName();

/////////////////////////////////////////////////////////////////////////////
// přepnutí stavu DEF_ICON prvku pod kurzorem

void ProgOnDefIcon();

/////////////////////////////////////////////////////////////////////////////
// aktualizace přepínačů pro vybranou položku

void ProgAktItem();

/////////////////////////////////////////////////////////////////////////////
// aktualizace příznaků LOCK a OFF bufferů

void UpdateLock();

/////////////////////////////////////////////////////////////////////////////
// zahájení editace ikony prvku

void ProgOnEditIcon();

/////////////////////////////////////////////////////////////////////////////
// zahájení/ukončení editace jména prvku

void ProgOnEditName();

/////////////////////////////////////////////////////////////////////////////
// zahájení editace jména prvku (vrací TRUE=přerušit editaci)

BOOL ProgOnBeginLabelEdit(HWND hWnd, HTREEITEM hItem);


/////////////////////////////////////////////////////////////////////////////
// ukončení editace jména prvku

void ProgOnEndLabelEdit(HWND hWnd, TV_ITEM* tvi);


/////////////////////////////////////////////////////////////////////////////
// obsluha klávesy (vrací TRUE=zrušení klávesy)

BOOL ProgOnKeyDown(HWND hWnd, int key, int data);


/////////////////////////////////////////////////////////////////////////////
// následující deklarace

void ProgOnRefNext();


/////////////////////////////////////////////////////////////////////////////
// předešlá deklarace

void ProgOnRefPrev();


/////////////////////////////////////////////////////////////////////////////
// definice deklarace

void ProgOnRefDef();

/////////////////////////////////////////////////////////////////////////////
// hledání textu

void ProgOnFind();
void ProgOnFindNext();
void ProgOnFindPrev();


/////////////////////////////////////////////////////////////////////////////
// horizontální posuvník

void ProgOnHScroll(int code, int pos);

/////////////////////////////////////////////////////////////////////////////
// vertikální posuvník

void ProgOnVScroll(int code, int pos);

/////////////////////////////////////////////////////////////////////////////
// obsluha časovače (TRUE=obslouženo)

BOOL ProgOnTimer(UINT timerID);

/////////////////////////////////////////////////////////////////////////////
// vstup znaku z klávesnice (TRUE=obslouženo)

BOOL ProgOnChar(HWND hWnd, TCHAR znak);

/////////////////////////////////////////////////////////////////////////////
// výběr všeho

void ProgOnAll();

/////////////////////////////////////////////////////////////////////////////
// zrušení bloku

void ProgOnDelete();

/////////////////////////////////////////////////////////////////////////////
// kopie bloku

void ProgOnCopy();

/////////////////////////////////////////////////////////////////////////////
// vystřížení bloku

void ProgOnCut();

/////////////////////////////////////////////////////////////////////////////
// vložení bloku

void ProgOnPaste();

/////////////////////////////////////////////////////////////////////////////
// UNDO

void ProgOnUndo();

/////////////////////////////////////////////////////////////////////////////
// REDO

void ProgOnRedo();

/////////////////////////////////////////////////////////////////////////////
// nastavení fokusu na okno editoru s opožděním

void SetFocusEdit();
void SetFocusEditMode(int buf, int inx);

/////////////////////////////////////////////////////////////////////////////
// zobrazení souřadnic grafického editoru

void DispMouseXY();

/////////////////////////////////////////////////////////////////////////////
// vykreslení prvku (vrací TRUE=obslouženo)

BOOL ProgOnDrawItem(DRAWITEMSTRUCT* di);

/////////////////////////////////////////////////////////////////////////////
// nastavení rozměrů obrázku/plochy

void ProgOnDimen();

/////////////////////////////////////////////////////////////////////////////
// zapnutí/vypnutí rastru

void ProgOnRastr();

/////////////////////////////////////////////////////////////////////////////
// start přehrávání

void ProgOnPlay();

/////////////////////////////////////////////////////////////////////////////
// pozastavení nahrávání, přehrávání

void ProgOnPause();

/////////////////////////////////////////////////////////////////////////////
// zastavení nahrávání i přehrávání

void ProgOnStop();

/////////////////////////////////////////////////////////////////////////////
// přepnutí příznaku opakování

void ProgOnLoop();

/////////////////////////////////////////////////////////////////////////////
// start nahrávání

void ProgOnRecord();

/////////////////////////////////////////////////////////////////////////////
// převíjení zpět

void ProgOnRew();

/////////////////////////////////////////////////////////////////////////////
// převíjení vpřed

void ProgOnFrw();

/////////////////////////////////////////////////////////////////////////////
// start programu

void ProgOnStart();

/////////////////////////////////////////////////////////////////////////////
// zapnutí režimu preview obrázku

void PreviewPicOn();

/////////////////////////////////////////////////////////////////////////////
// vypnutí režimu preview obrázku

void PreviewPicOff();

/////////////////////////////////////////////////////////////////////////////
// zastaveňí preview, zahájení nového čítání času

void PreviewStop();

/////////////////////////////////////////////////////////////////////////////
// zahájení preview přehrávání souboru pod kurzorem

void PreviewStart();

/////////////////////////////////////////////////////////////////////////////
// nulování příznaků načtení obrázků pro preview

void PreviewPicNul();
