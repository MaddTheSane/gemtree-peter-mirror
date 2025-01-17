
#include "Main.h"

/***************************************************************************\
*																			*
*							Obsluha editace souboru							*
*																			*
\***************************************************************************/

#include	"ProgInit.inc"							// inicializační tabulka funkcí

/////////////////////////////////////////////////////////////////////////////
// globální a lokální proměnné

BOOL		ProgMode = FALSE;						// příznak módu editace
int			ToolBarProgN;							// počet základních prvků panelu nástrojů

HWND		TreeCls = NULL;							// okno tříd
HWND		TreeObj = NULL;							// okno globálních objektů
HWND		TreeLoc = NULL;							// okno lokálních objektů
HWND		TreeEdi = NULL;							// okno editoru
HWND		TreeStr = NULL;							// okno struktur

#define		NADPISHEIGHT 22							// výška okna nadpisu
int			ProgStatusWidth = 150;					// šířka druhého pole programového řádku

HIMAGELIST	ProgImageList = NULL;					// seznam obrázků
HIMAGELIST	ProgImageListMedium = NULL;				// seznam středních obrázků
HIMAGELIST	ProgImageListSmall = NULL;				// seznam malých obrázků
HIMAGELIST	ProgStateList = NULL;					// seznam stavových ikon
HIMAGELIST	ProgStateListMedium = NULL;				// seznam středních stavových ikon
HIMAGELIST	ProgStateListSmall = NULL;				// seznam malých stavových ikon

BOOL		Modi = FALSE;							// příznak modifikace souboru

//BOOL		NoSelAkt = FALSE;						// neaktualizovat změnu výběru

/////////////////////////////////////////////////////////////////////////////
// obsluha editace jména prvku

BOOL		EditName = FALSE;						// příznak probíhající editace
HWND		EditNameWnd = NULL;						// editační okno

/////////////////////////////////////////////////////////////////////////////
// obsluha editace čísla

HWND		EditNumWnd = NULL;						// editační okno čísla

/////////////////////////////////////////////////////////////////////////////
// editace číselné položky plochy

HWND		EditMapNumWnd = NULL;					// editační okno plochy (NULL=není)

/////////////////////////////////////////////////////////////////////////////
// obsluha editace logických hodnot

HWND		EditLogWnd = NULL;						// editační okno logických hodnot

/////////////////////////////////////////////////////////////////////////////
// obsluha editace textu

HWND		EditTextWnd = NULL;						// editační okno textu

/////////////////////////////////////////////////////////////////////////////
// posuvníky pro editory

HWND	HScroll = NULL;								// horizontální posuvník
HWND	VScroll = NULL;								// vertikální posuvník
BOOL	HScrollDisp = FALSE;						// je horizontální posuvník
BOOL	VScrollDisp = FALSE;						// je vertikální posuvník

/////////////////////////////////////////////////////////////////////////////
// předěly oken

BOOL		Tracking = FALSE;						// je tažení předělů
BOOL		VTracking = FALSE;						// je tažen vertikální předěl
BOOL		LTracking = FALSE;						// je tažen levý předěl

#define	TRACKOFF	3								// odchylka souřadnice pro tažení předělu

int			TrackX1;								// levý vertikální předěl
int			TrackY1;								// levý horizontální předěl
int			TrackX2;								// pravý vertikální předěl
int			TrackY2;								// pravý horizontální předěl

/////////////////////////////////////////////////////////////////////////////
// editační pole

int			FocusBuf = BufObjID;					// okno s fokusem (= číslo bufferu, BufEdiID = editační pole)
int			OldFocusBuf = -1;						// staré okno s fokusem (kvůli překreslení starého kurzoru)

int			EditMode = -1;							// aktivní editační pole (= číslo bufferu)
int			EditBuf = BufEdiID;						// editovaný buffer
int			EditItem = 0;							// editovaná položka v bufferu
int			EditItemSprite = 0;						// sprajt s editovanou ikonou (úschova pro historii)
int			EditItemPic = 0;						// editovaný obrázek sprajtu (úschova pro historii)

int			EditX = 0;								// souřadnice X počátku editačního pole
int			EditY = 0;								// souřadnice Y počátku editačního pole
int			EditWidth = 0;							// šířka editačního pole
int			EditHeight = 0;							// výška editačního pole

int			ToolMode = -1;							// aktivní editační mód Toolbaru (=číslo bufferu)


/////////////////////////////////////////////////////////////////////////////
// hlášení o změně adresáře

HANDLE		FunctionDirChange	= INVALID_HANDLE_VALUE;	// hlášení o změně adresáře - funkce
HANDLE		NumberDirChange		= INVALID_HANDLE_VALUE;	// hlášení o změně adresáře - čísla
HANDLE		TextDirChange		= INVALID_HANDLE_VALUE;	// hlášení o změně adresáře - text
HANDLE		BoolDirChange		= INVALID_HANDLE_VALUE;	// hlášení o změně adresáře - logické hodnoty
HANDLE		IconDirChange		= INVALID_HANDLE_VALUE;	// hlášení o změně adresáře - ikony
HANDLE		MapDirChange		= INVALID_HANDLE_VALUE;	// hlášení o změně adresáře - plochy
HANDLE		PictureDirChange	= INVALID_HANDLE_VALUE;	// hlášení o změně adresáře - obrázky
HANDLE		SpriteDirChange		= INVALID_HANDLE_VALUE;	// hlášení o změně adresáře - sprajty
HANDLE		SoundDirChange		= INVALID_HANDLE_VALUE;	// hlášení o změně adresáře - zvuk
HANDLE		MusicDirChange		= INVALID_HANDLE_VALUE;	// hlášení o změně adresáře - hudba

HANDLE		FunctionDirChange2	= INVALID_HANDLE_VALUE;	// hlášení o změně adresáře - alternativní funkce
HANDLE		NumberDirChange2	= INVALID_HANDLE_VALUE;	// hlášení o změně adresáře - alternativní čísla
HANDLE		TextDirChange2		= INVALID_HANDLE_VALUE;	// hlášení o změně adresáře - alternativní text
HANDLE		BoolDirChange2		= INVALID_HANDLE_VALUE;	// hlášení o změně adresáře - alternativní logické hodnoty
HANDLE		IconDirChange2		= INVALID_HANDLE_VALUE;	// hlášení o změně adresáře - alternativní ikony
HANDLE		MapDirChange2		= INVALID_HANDLE_VALUE;	// hlášení o změně adresáře - alternativní plochy
HANDLE		PictureDirChange2	= INVALID_HANDLE_VALUE;	// hlášení o změně adresáře - alternativní obrázky
HANDLE		SpriteDirChange2	= INVALID_HANDLE_VALUE;	// hlášení o změně adresáře - alternativní sprajty
HANDLE		SoundDirChange2		= INVALID_HANDLE_VALUE;	// hlášení o změně adresáře - alternativní zvuk
HANDLE		MusicDirChange2		= INVALID_HANDLE_VALUE;	// hlášení o změně adresáře - alternativní hudba

#define		ProgDirChangeTimerID 19118			// ID časovače hlášení změny adresáře
UINT		ProgDirChangeTimer = 0;				// časovač pro hlášení změn adresáře
int			ProgDirChangeIgnore = 0;			// čítač pro ignorování hlášení změn po vlastní změně

/////////////////////////////////////////////////////////////////////////////
// přechodně editovaný prvek v bufferu tříd

//CText		EditClassName;							// jméno editovaného souboru
//BOOL		EditClassModi = FALSE;					// příznak nutnosti uložení dat
//int			EditClassBuf = -1;						// editovaný buffer bufferu tříd
//int			EditClassItem = -1;						// editovaná položka v bufferu
//int			EditClassSprite = -1;					// sprajt s editovanou ikonou
//int			EditClassIcon = -1;						// editovaná ikona sprajtu

/////////////////////////////////////////////////////////////////////////////
// obsluha kurzoru myši

#define MOUSEINV -100000				// neplatná souřadnice myši
int		MouseX = MOUSEINV;				// souřadnice X myši
int		MouseY = MOUSEINV;				// souřadnice Y myši


/////////////////////////////////////////////////////////////////////////////
// opožděné nastavení fokusu

#define		FocusTimerID 19128						// ID časovače pro nastavení fokusu
UINT		FocusTimer = 0;							// časovač pro nastavení fokusu
int			FocusTimerN = 0;						// čítač pro nastavení fokusu
int			FocusNextBuf = BufEdiID;				// buffer k nastavení jako aktivní

int			FocusEditModeBuf = -1;					// buffer s editací (-1 = není)
int			FocusEditModeInx = -1;					// položka s editací

/////////////////////////////////////////////////////////////////////////////
// obsluha preview

#define		PreviewTimerID 19135					// ID časovače pro preview
UINT		PreviewTimer = 0;						// časovač pro preview
int			PreviewTimerN = -1;						// čítač pro preview

BOOL		PreviewWav = FALSE;						// probíhá preview WAV
MCIDEVICEID	PreviewMid = 0;							// ID zařízení pro MID
BOOL		PreviewPic = FALSE;						// probíhá preview obrázku

#define		PreviewPicSize 3						// počet položek pro cachování obrázků
#define		PreviewPicAkt 0							// index aktuálního obrázku
#define		PreviewPicNxt 1							// index následujícího obrázku
#define		PreviewPicPre 2							// index předešlého obrázku

CPicture	PreviewPicPic[PreviewPicSize];			// obrázky pro cachování
CText		PreviewPicName[PreviewPicSize];			// jména obrázků pro cachování (prázdný = není)
BOOL		PreviewPicLoad[PreviewPicSize];			// příznaky načítání obrázků (TRUE = byl načítán)

BOOL		PreviewPicVpred = TRUE;					// přednost obrázky směrem vpřed

TCHAR		PreviewTempName[MAX_PATH + 101];		// jméno přechodného souboru při přehrávání MP3
BOOL		IsPreviewTempName = FALSE;				// příznak vytvoření přechodného souboru MP3


/////////////////////////////////////////////////////////////////////////////
// hledání textu

HWND		FindDialog = NULL;						// okno hledání
HWND		FindStringEdit = NULL;					// handle editoru textu k hledání

/////////////////////////////////////////////////////////////////////////////
// combobox panely

HWND		ToolEditIcon = NULL;					// panel nástrojů editoru ikon
HWND		ToolEditIconWidth = NULL;				// panel nástrojů volby šířky
HWND		ToolEditMap = NULL;						// panel nástrojů editoru ploch

/////////////////////////////////////////////////////////////////////////////
// inicializační buffery (připravují se jen jednou při startu programu)
//
// Při používání bufferů se počítá s tím, že položky jsou uloženy po řadě
// od začátku bufferu, počet položek odpovídá počtu (a pořadí) funkcí.

const int FncNum = sizeof(InitTabFunc) / sizeof(InitTabFunc[0]);	// počet funkcí
CBufMultiText	TextFnc;							// texty funkcí
CBufIcon	IconFnc;								// ikony funkcí

/////////////////////////////////////////////////////////////////////////////
// buffery

CBufProg	BufProg[PROGBUFNUM];					// (0 až 5) buffery programu (0=interní)
CBufReal	Real;									// buffer reálných čísel
CBufMultiText	Text;								// buffer textů
CBufBool	Bool;									// buffer logických proměnných
CBufIcon	Icon;									// buffer ikon
CBufIcon	IconState;								// buffer stavových ikon
CBufMap		Map;									// buffer ploch
CBufPic		Picture;								// buffer obrázků
CBufSprite	Sprite;									// buffer sprajtů
CBufSound	Sound;									// buffer zvuků
CBufMusic	Music;									// buffer hudby

CSprite		Sprite0;								// sprajt 0 (Petr)
CSprite		Sprite1;								// sprajt 1 (Petra)
CBufIcon	Icon1;									// buffer s 1 ikonou k zobrazení

//CBufIcon	Icon16;									// buffer ikon s velikostí 16x16
//CBufIndex	Icon16List;								// buffer indexů ikon s velikostí 16x16
//int			Icon16Next = 0;							// příští ikona pro zápis
//CBufIcon	Icon32;									// buffer ikon s velikostí 32x32
//CBufIndex	Icon32List;								// buffer indexů ikon s velikostí 32x32
//int			Icon32Next = 0;							// příští ikona pro zápis

CBufUndo	Undo;									// buffer undo a redo

int			GroupIndex = -1;						// index do bufferu BufStr na IDF_GROUP

/////////////////////////////////////////////////////////////////////////////
// definice základních tlačítek pro toolbar Program

TBBUTTON ToolBarProgram[] = {
	{	ButtonStart,		IDN_START,			TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonClose,		IDN_CLOSE,			TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonSave,			IDN_SAVE,			TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	0,					0,					TBSTATE_ENABLED, TBSTYLE_SEP,		0, 0},
	{	ButtonUndo,			IDN_UNDO,			TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonRedo,			IDN_REDO,			TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonCut,			IDN_CUT,			TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonCopy,			IDN_COPY,			TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonPaste,		IDN_PASTE,			TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonClear,		IDN_CLEAR,			TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonAll,			IDN_ALL,			TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	0,					0,					TBSTATE_ENABLED, TBSTYLE_SEP,		0, 0},
	{	ButtonIn,			IDN_IN,				TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonOut,			IDN_OUT,			TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonHistPrev,		IDN_HIST_PREV,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonHistNext,		IDN_HIST_NEXT,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	0,					0,					TBSTATE_ENABLED, TBSTYLE_SEP,		0, 0},
};

#define ToolBarProgramNum (sizeof(ToolBarProgram)/sizeof(TBBUTTON))

// editor programu
TBBUTTON ToolBarEditProg[] = {
	{	ButtonLock,			IDN_LOCK,			TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonOff,			IDN_OFF,			TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonEditName,		IDN_EDIT_NAME,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonDefName,		IDN_DEF_NAME,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonEditIcon,		IDN_EDIT_ICON,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonDefIcon,		IDN_DEF_ICON,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	0,					0,					TBSTATE_ENABLED, TBSTYLE_SEP,		0, 0},
	{	ButtonRefDef,		IDN_REF_DEF,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonRefPrev,		IDN_REF_PREV,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonRefNext,		IDN_REF_NEXT,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	0,					0,					TBSTATE_ENABLED, TBSTYLE_SEP,		0, 0},
	{	ButtonFind,			IDN_FIND,			TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonFindPrev,		IDN_FIND_PREV,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonFindNext,		IDN_FIND_NEXT,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
};

#define ToolBarEditProgNum (sizeof(ToolBarEditProg)/sizeof(TBBUTTON))

// editor ikon
TBBUTTON ToolBarEditIcon[] = {
	{	ButtonRastr,		IDN_RASTR,			TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_CHECK,	0, 0},
	{	ButtonEditBlok,		IDN_EDITBLOK,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonOtisk,		IDN_OTISK,			TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
};

#define ToolBarEditIconNum (sizeof(ToolBarEditIcon)/sizeof(TBBUTTON))

// editor plochy
TBBUTTON ToolBarEditMap[] = {
	{	ButtonDimen,		IDN_DIMEN,			TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonEditMap,		IDN_EDITMAP,		TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_CHECK,	0, 0},
	{	ButtonFillMap,		IDN_FILLMAP,		TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_CHECK,	0, 0},
	{	ButtonRastr,		IDN_RASTR,			TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_CHECK,	0, 0},
//	{	ButtonSelect,		IDN_SELECT,			TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_CHECK,	0, 0},
};

#define ToolBarEditMapNum (sizeof(ToolBarEditMap)/sizeof(TBBUTTON))

// editor sprajtu
TBBUTTON ToolBarEditSprite[] = {
	{	ButtonSpritePar,	IDN_SPRITEPAR,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonTest,			IDN_TEST,			TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
};

#define ToolBarEditSpriteNum (sizeof(ToolBarEditSprite)/sizeof(TBBUTTON))

// editor obrázků
TBBUTTON ToolBarEditPic[] = {
	{	ButtonDimen,		IDN_DIMEN,			TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonRastr,		IDN_RASTR,			TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_CHECK,	0, 0},
	{	ButtonEditBlok,		IDN_EDITBLOK,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{	ButtonOtisk,		IDN_OTISK,			TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
};

#define ToolBarEditPicNum (sizeof(ToolBarEditPic)/sizeof(TBBUTTON))

// editor zvuku
TBBUTTON ToolBarEditSound[] = {
	{	ButtonPlay,			IDN_PLAY,			TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_CHECK,	0, 0},
	{	ButtonPause,		IDN_PAUSE,			TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_CHECK,	0, 0},
	{	ButtonStop,			IDN_STOP,			TBSTATE_ENABLED, TBSTYLE_BUTTON,					0, 0},
	{	ButtonLoop,			IDN_LOOP,			TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_CHECK,	0, 0},
	{	ButtonRecord,		IDN_RECORD,			TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_CHECK,	0, 0},
};

#define ToolBarEditSoundNum (sizeof(ToolBarEditSound)/sizeof(TBBUTTON))

// editor hudby
TBBUTTON ToolBarEditMusic[] = {
	{	ButtonPlay,			IDN_PLAY,			TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_CHECK,	0, 0},
	{	ButtonPause,		IDN_PAUSE,			TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_CHECK,	0, 0},
	{	ButtonStop,			IDN_STOP,			TBSTATE_ENABLED, TBSTYLE_BUTTON,					0, 0},
	{	ButtonLoop,			IDN_LOOP,			TBSTATE_ENABLED, TBSTYLE_BUTTON | TBSTYLE_CHECK,	0, 0},
	{	ButtonRew,			IDN_REW,			TBSTATE_ENABLED, TBSTYLE_BUTTON,					0, 0},
	{	ButtonFrw,			IDN_FRW,			TBSTATE_ENABLED, TBSTYLE_BUTTON,					0, 0},
};

#define ToolBarEditMusicNum (sizeof(ToolBarEditMusic)/sizeof(TBBUTTON))

// volba nástroje pro kreslení
int ToolBarEditPicTools[] = {
	ButtonPen,			IDN_PEN,
	ButtonLine,			IDN_LINE,
	ButtonBox,			IDN_BOX,
	ButtonFillBox,		IDN_FILLBOX,
	ButtonCircle,		IDN_CIRCLE,
	ButtonFillCircle,	IDN_FILLCIRCLE,
	ButtonEllipse,		IDN_ELLIPSE,
	ButtonFillEllipse,	IDN_FILLELLIPSE,
	ButtonSphere,		IDN_SPHERE,
	ButtonSelect,		IDN_SELECT,
	ButtonSpray,		IDN_SPRAY,
	ButtonPipe,			IDN_PIPE,
	ButtonFill,			IDN_FILL,
	ButtonText,			IDN_TEXT,
	-1
};

// volba šířky pera pro kreslení
int ToolBarEditPicWidths[] = {
	ButtonWidth1,		IDN_WIDTH1,
	ButtonWidth2,		IDN_WIDTH2,
	ButtonWidth3,		IDN_WIDTH3,
	ButtonWidth4,		IDN_WIDTH4,
	ButtonWidth5,		IDN_WIDTH5,
	-1
};

// volba módu pro editor plochy
int ToolBarEditMapTools[] = {
	ButtonMap,			IDN_MAP,
	ButtonSwc1,			IDN_SWC1,
	ButtonSwc2,			IDN_SWC2,
	ButtonSwc3,			IDN_SWC3,
	ButtonSwc4,			IDN_SWC4,
	ButtonSwc5,			IDN_SWC5,
	ButtonSwcN1,		IDN_SWCN1,
	ButtonSwcN2,		IDN_SWCN2,
	ButtonSwcN3,		IDN_SWCN3,
	-1
};

/////////////////////////////////////////////////////////////////////////////
// definice menu

// program
MENUITEM* MenuPrg[] =
{
	MenuSoubor2,
	MenuEditace,
	MenuZobraz,
	MenuProgram,
	MenuHlp,
	NULL
};

// číslo, přepínač, text
MENUITEM* MenuNum[] =
{
	MenuSoubor2,
	MenuEditace,
	MenuZobraz,
	MenuHlp,
	NULL
};
#define MenuTxt MenuNum
#define MenuLog MenuNum

// ikona
MENUITEM* MenuIco[] =
{
	MenuSoubor2,
	MenuEditace,
	MenuZobraz,
	MenuIkona,
	MenuNastroje,
	MenuTloustka,
	MenuHlp,
	NULL
};

// obrázek
MENUITEM* MenuPic[] =
{
	MenuSoubor2,
	MenuEditace,
	MenuZobraz,
	MenuObrazek,
	MenuNastroje,
	MenuTloustka,
	MenuHlp,
	NULL
};

// plocha
MENUITEM* MenuMap[] =
{
	MenuSoubor2,
	MenuEditace,
	MenuZobraz,
	MenuPlocha,
	MenuHlp,
	NULL
};

// sprajt
MENUITEM* MenuSpr[] =
{
	MenuSoubor2,
	MenuEditace,
	MenuZobraz,
	MenuSprajt,
	MenuHlp,
	NULL
};

// zvuk
MENUITEM* MenuSnd[] =
{
	MenuSoubor2,
	MenuEditace,
	MenuZobraz,
	MenuSound,
	MenuHlp,
	NULL
};

// hudba
MENUITEM* MenuMus[] =
{
	MenuSoubor2,
	MenuEditace,
	MenuZobraz,
	MenuMusic,
	MenuHlp,
	NULL
};

/////////////////////////////////////////////////////////////////////////////
// nastavení menu programu a fontů (lze i opakovaně)

void ProgSetMenu()
{
	MENUITEM** oldmenu = MenuAkt;

	BufCls.AktFont();
	BufObj.AktFont();
	BufLoc.AktFont();
	BufStr.AktFont();
	BufEdi.AktFont();

	if (FocusBuf != BufEdiID)
	{
		SetNewMenu(MenuPrg);
	}
	else
	{
		switch (EditMode)
		{
		case BufEdiID:
			SetNewMenu(MenuPrg);
			break;

		case BufNumID:
			SetNewMenu(MenuNum);
			break;

		case BufTxtID:
			SetNewMenu(MenuTxt);
			break;

		case BufLogID:
			SetNewMenu(MenuLog);
			break;

		case BufIcoID:
			SetNewMenu(MenuIco);
			break;

		case BufPicID:
			SetNewMenu(MenuPic);
			break;

		case BufMapID:
			SetNewMenu(MenuMap);
			break;

		case BufSprID:
			SetNewMenu(MenuSpr);
			break;

		case BufSndID:
			SetNewMenu(MenuSnd);
			break;

		case BufMusID:
			SetNewMenu(MenuMus);
			break;
		}
	}

	if (oldmenu != MenuAkt)
	{
		ProgUpdateInOut();
		ProgUpdateUndoRedo();
		ProgUpdateClipboard();
		ProgAktItem();
	}
}

/////////////////////////////////////////////////////////////////////////////
// aktualizace po změně jazyku

void ProgReloadBuf(CBufProg* buf)
{
	int disp = buf->Disp();
	int sel = buf->Select();

	buf->Disp(-2);

	buf->Disp(disp);
	buf->Select(sel);
}

void ProgReloadJazyk()
{
// načtení nadpisů oken
	CText text;
	text.Load(IDN_NADPISOBJ);
	BufObj.Nadpis(text);
	text.Load(IDN_NADPISLOC);
	BufLoc.Nadpis(text);
	text.Load(IDN_NADPISSTR);
	BufStr.Nadpis(text);
	text.Load(IDN_NADPISCLS);
	BufCls.Nadpis(text);

// aktualizace oken
	if (ProgMode)
	{
		ProgReloadBuf(&BufStr);
		ProgReloadBuf(&BufCls);
		ProgReloadBuf(&BufObj);
		ProgReloadBuf(&BufLoc);
		ProgReloadBuf(&BufEdi);

		if (EditMode == BufTxtID)
		{
			EditText::BegEdit(EditText::Index);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// první inicializace při startu programu (hlavní okno i SELECT je již vytvořeno)

void ProgStartInit()
{
// inicializace bufferů programu
	int i;
	for (i = 0; i < PROGBUFNUM; i++)
	{
		BufProg[i].BufID(i);
	}

// načtení nadpisů oken
	CText text;
	text.Load(IDN_NADPISOBJ);
	BufObj.Nadpis(text);
	text.Load(IDN_NADPISLOC);
	BufLoc.Nadpis(text);
	text.Load(IDN_NADPISSTR);
	BufStr.Nadpis(text);
	text.Load(IDN_NADPISCLS);
	BufCls.Nadpis(text);

// inicializace statických objektů
	Sprite0.New(5, 4);				// sprajt 0 - Petr
	Sprite1.New(5, 4);				// sprajt 1 - Petra
	Sprite1.Level(900);

// kontrola tabulky jmen importu (zda jsou jména jedinečná)
#ifdef	_DEBUG
	{
		int i, j, n;
		char* txt;

		for (i = 0; i < FncNum-1; i++)
		{
			txt = InitTabFunc[i].Import;
			n = ::lstrlenA(txt) + 1;

			for (j = i+1; j < FncNum; j++)
			{
				ASSERT(!MemCompare(txt, InitTabFunc[j].Import, n));
			}
		}
	}
#endif

// načtení textů funkcí
	int index;
	for (i = IDF; i < IDF + FncNum; i++)		// cyklus přes všechny ID
	{
		index = TextFnc.Load(i);				// načtení textu s resource
		ASSERT(index == (i - IDF));				// ukládání musí být spojité

#ifdef _DEBUG
		for (int j = 0; j < JAZYKNUM; j++)
		{
			ASSERT(TextFnc[index].MultiText(j).IsNotEmpty());	// musí to být platný text
		}
#endif
	}

#ifdef _DEBUG
	ASSERT(TextFnc.Num() == FncNum);			// počet textů teď musí přesně souhlasit
#endif

// načtení ikon funkcí
	IconFnc.Load(IDN_DEF_ICON, FncNum);			// načtení ikon z resource
	ASSERT(IconFnc.Num() == FncNum);			// musí přesně souhlasit počet

// vygenerování ikon pro HELP (odpoznámkovat a spustit vždy po přidání dalších funkcí, před doplňováním HELPu)
/*
	StdBitmapInfo->bmiColors[BackCol].rgbRed = 255;
	StdBitmapInfo->bmiColors[BackCol].rgbGreen = 255;
	StdBitmapInfo->bmiColors[BackCol].rgbBlue = 230;
	CPicture ppp;
	for (i = IDF; i < IDF+FncNum; i++)
	{
		ppp.New(32, 32);
		IconFnc[i-IDF].DeComp();
		ppp.CopyData(IconFnc[i-IDF].DataData());
		CText n;
		n.Int(i);
		ppp.SaveFile("HLP\\IDF\\IDF" + n + ".BMP");
	}
*/

// načtení stavových ikon
	IconState.IconSize(12, 32);					// nastaveni velikosti ikon
	IconState.Load(IDN_STATE_ICON, 4);			// načtení stavových ikon
	ASSERT(IconState.Num() == 4);

// inicializace bufferu seznamu ikon
//	Icon16.DelAll();
//	Icon32.DelAll();
//	Icon16List.NumClear(ICONNUM);
//	Icon32List.NumClear(ICONNUM);
//	for (i = 0; i < ICONNUM; i++)
//	{
//		Icon16.New();
//		Icon32.New();
//	}

// inicializace bufferu interních funkcí
	PROGITEM* item;
	for (i = IDF; i < IDF + FncNum; i++)
	{
		index = BufInt.New();						// vytvoření nové položky
		ASSERT(index == (i - IDF));					// ukládání musí být spojité
		item = &BufInt[index];						// adresa nové položky
		item->Func = i;								// identifikační číslo funkce
		item->SrcMask = InitTabFunc[index].SrcMask;	// zdrojové vlastnosti
		item->DstMask = InitTabFunc[index].DstMask;	// cílové vlastnosti
		item->Param = InitTabFunc[index].Param;		// parametry
		item->Icon = index;							// index ikony
		item->Name = index;							// index jména
	}
	ASSERT(BufInt.Num() == FncNum);					// počet funkcí musí souhlasit

// inicializace bufferu struktur
	BufStr.InitTab(InitTabStr, sizeof(InitTabStr)/sizeof(InitTabStr[0]));
	ASSERT(BufStr.Num() == sizeof(InitTabStr)/sizeof(InitTabStr[0]));

// nalezení indexu skupiny
	for (int inx = BufStr.Max()-1; inx >= 0; inx--)
	{
		if ((BufStr.IsValid(inx)) &&
			(BufStr[inx].Func == IDF_GROUP))
		{
			GroupIndex = inx;
			break;
		}
	};

// inicializace editorů
	EditIcon::StartInit();
	EditNum::StartInit();
	EditLog::StartInit();
	EditText::StartInit();
	EditMap::StartInit();
	EditSound::StartInit();
	EditMusic::StartInit();
	EditSprite::StartInit();
	ProgClip::StartInit();
	ProgLib::StartInit();
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření okna stromu

HWND CreateTree(int x, int y, int width, int height, int id)
{
	HWND wnd = ::CreateWindowEx(
		WS_EX_CLIENTEDGE,
		WC_TREEVIEW,
		NULL,
		WS_CHILD | WS_TABSTOP | TVS_HASLINES | TVS_HASBUTTONS 
			| TVS_LINESATROOT | TVS_EDITLABELS | TVS_SHOWSELALWAYS,
		x,
		y,
		width,
		height,
		MainFrame,
		(HMENU)id,
		hInstance,
		NULL);

	ASSERT(wnd != NULL);
	return wnd;
}


/////////////////////////////////////////////////////////////////////////////
// zahájení režimu editace souboru

void ProgInit()
{
// kontrola, zda není režim editace souboru
	if (ProgMode) return;

	Modi = FALSE;
	EditName = FALSE;
	EditNameWnd = NULL;
	EditMapNumWnd = NULL;

	EditMode = BufEdiID;						// editace zatím není platná
	EditBuf = BufEdiID;
	EditItem = 0;

// nastavení tlačítek pro režim programování
	ToolBarProgN = ToolBarProgramNum;
	ToolBarAdd(ToolBarProgram, ToolBarProgramNum); // přidání tlačítek

// nastavení nového menu
	ProgSetMenu();
	EnableCommand(IDN_START, TRUE);

// inicializace bufferů
	Icon = IconFnc;
	Text = TextFnc;

//	Icon16List.NumClear(ICONNUM);
//	Icon32List.NumClear(ICONNUM);

// buffer interních funkcí musí zůstat stále stejný a nezměněn
	ASSERT(BufInt.Num() == FncNum);
	ASSERT(Icon.Num() == FncNum);
	ASSERT(Text.Num() == FncNum);
	ASSERT(IconState.Num() == 4);

// buffer struktur musí také zůstat nezměněn
	ASSERT(BufStr.Num() == sizeof(InitTabStr)/sizeof(InitTabStr[0]));

// vynulování čítačů referencí v bufferu struktur
	int inx;
	for (inx = BufStr.Max()-1; inx >= 0; inx--)
	{
		if (BufStr.IsValid(inx))
		{
			BufStr[inx].Refer = 0;
		}
	}

// inicializace bufferu tříd
	BufCls.InitTab(InitTabCls, sizeof(InitTabCls)/sizeof(InitTabCls[0]));

// načtení programu
	ProgFile::Load();

// kontrola programu
	ProgramCheck();

// aktualizace LOCK a OFF prvků v buferech
	UpdateLock();

// vytvoření seznamu obrázků
	ProgImageList = ImageList_Create(32, 32, ILC_COLORDDB | ILC_MASK,1,1);
	ASSERT(ProgImageList);
	ProgImageListMedium = ImageList_Create(24, 24, ILC_COLORDDB | ILC_MASK,1,1);
	ASSERT(ProgImageListMedium);
	ProgImageListSmall = ImageList_Create(16, 16, ILC_COLORDDB | ILC_MASK,1,1);
	ASSERT(ProgImageListSmall);
	ProgStateList = IconState.GenerList(FALSE, SI_BIG);
	ASSERT(ProgStateList);
	ProgStateListMedium = IconState.GenerList(FALSE, SI_MEDIUM);
	ASSERT(ProgStateListMedium);
	ProgStateListSmall = IconState.GenerList(FALSE, SI_SMALL);
	ASSERT(ProgStateListSmall);

// vytvoření oken stromů
	TrackX1 = ClientRect.left + ClientWidth/5;
	TrackX2 = ClientRect.left + ClientWidth/5*4;
	TrackY1 = ClientRect.top + ClientHeight/3*2;
	TrackY2 = ClientRect.top + ClientHeight/3*2;

	EditX = TrackX1;
	EditY = ClientRect.top + NADPISHEIGHT;
	EditWidth = TrackX2 - TrackX1;
	EditHeight = ClientHeight;

	TreeObj = CreateTree(ClientRect.left, ClientRect.top + NADPISHEIGHT, TrackX1 - ClientRect.left, TrackY1 - ClientRect.top - NADPISHEIGHT, IDN_TREE_OBJ);
	TreeLoc = CreateTree(ClientRect.left, TrackY1 + NADPISHEIGHT, TrackX1 - ClientRect.left, ClientRect.bottom - TrackY1 - NADPISHEIGHT, IDN_TREE_LOC);
	TreeEdi = CreateTree(EditX, EditY, EditWidth, EditHeight - NADPISHEIGHT, IDN_TREE_EDI);
	TreeStr = CreateTree(TrackX2, ClientRect.top + NADPISHEIGHT, ClientRect.right - TrackX2, TrackY2 - ClientRect.top - NADPISHEIGHT, IDN_TREE_STR);
	TreeCls = CreateTree(TrackX2, TrackY2 + NADPISHEIGHT, ClientRect.right - TrackX2, ClientRect.bottom - TrackY2 - NADPISHEIGHT, IDN_TREE_CLS);

// horizontální posuvník
	HScroll = ::CreateWindow(_T("SCROLLBAR"), NULL, WS_CHILD | SBS_HORZ,
		0, 0, TRACKSIRKA, TRACKSIRKA, MainFrame, NULL, hInstance, 0);

// vertikální posuvník
	VScroll = ::CreateWindow(_T("SCROLLBAR"), NULL, WS_CHILD | SBS_VERT,
		0, 0, TRACKSIRKA, TRACKSIRKA, MainFrame, NULL, hInstance, 0);

// napojení stromů na buffery
	BufObj.Tree(TreeObj);
	BufLoc.Tree(TreeLoc);
	BufEdi.Tree(TreeEdi);
	BufCls.Tree(TreeCls);
	BufStr.Tree(TreeStr);

// nastavení ikon pro stromy
	BufObj.IconList(BufZoom[BufObjID]);
	BufLoc.IconList(BufZoom[BufLocID]);
	BufEdi.IconList(BufZoom[BufEdiID]);
	BufStr.IconList(BufZoom[BufStrID]);
	BufCls.IconList(BufZoom[BufClsID]);

// nastavení fontů pro stromy
	BufCls.AktFont();
	BufObj.AktFont();
	BufLoc.AktFont();
	BufStr.AktFont();
	BufEdi.AktFont();

// zobrazení stromů
	BufObj.Disp(-1);
	BufLoc.Disp(0);
	BufEdi.Disp(0);
	BufStr.Disp(-1);
	BufCls.Disp(-1);

// zapnutí módu programování
	ProgMode = TRUE;

// zobrazení oken stromů
	ProgOnSize();
	::ShowWindow(TreeObj, SW_SHOW);
	::ShowWindow(TreeLoc, SW_SHOW);
	::ShowWindow(TreeEdi, SW_SHOW);
	::ShowWindow(TreeStr, SW_SHOW);
	::ShowWindow(TreeCls, SW_SHOW);

// vyprázdnění bufferu historie editace
	HistEdit::DelAll();
	HistEdit::Update();

// nastavení fokusu na okno objektů
	EditBuf = -1;
	SetEditMode(BufEdiID, 0);						// zapnutí editačního pole programu

// nastavení fokusu na okno objektů s opožděním (chyba v obsluze stromu v COMMCTL32.DLL)
// !!!!!!!! ProgSetFocus() havaruje (adresa 56AE90 se zneplatní - položka stromu struktur) !!!!!!!!
	FocusBuf = BufObjID;
	FocusNextBuf = BufObjID;
	FocusEditModeBuf = -1;
	FocusTimerN = 2;
	FocusTimer = ::SetTimer(MainFrame, FocusTimerID, TimerConst, NULL);
//	ProgSetFocus();

// nadpis editačního okna
	BufEdi.Nadpis(Text.GetTxt(BufObj.GetText(BufObj.SrcDat(BufLocID, 0))));

// zahájení hlášení změn adresářů
	BeginDirChange(&FunctionDirChange, &FunctionDirChange2, FunctionPath, FunctionPath2);
	BeginDirChange(&NumberDirChange, &NumberDirChange2, NumberPath, NumberPath2);
	BeginDirChange(&TextDirChange, &TextDirChange2, TextPath, TextPath2);
	BeginDirChange(&BoolDirChange, &BoolDirChange2, BoolPath, BoolPath2);
	BeginDirChange(&IconDirChange, &IconDirChange2, IconPath, IconPath2);
	BeginDirChange(&MapDirChange, &MapDirChange2, MapPath, MapPath2);
	BeginDirChange(&PictureDirChange, &PictureDirChange2, PicturePath, PicturePath2);
	BeginDirChange(&SpriteDirChange, &SpriteDirChange2, SpritePath, SpritePath2);
	BeginDirChange(&SoundDirChange, &SoundDirChange2, SoundPath, SoundPath2);
	BeginDirChange(&MusicDirChange, &MusicDirChange2, MusicPath, MusicPath2);

	ProgDirChangeTimer = ::SetTimer(MainFrame, ProgDirChangeTimerID, 2000, NULL);
	ProgDirChangeIgnore = 1;

// zahájení registrace UNDO
	BufCls.UndoOn();
	BufObj.UndoOn();
	BufLoc.UndoOn();
	BufEdi.UndoOn();
	BufStr.UndoOn();

	Real.UndoOn();
	Text.UndoOn();
	Bool.UndoOn();
	Icon.UndoOn();
	Map.UndoOn();
	Picture.UndoOn();
	Sprite.UndoOn();
	Sound.UndoOn();
	Music.UndoOn();

// zahájení čítání preview
	PreviewStop();

// aktualizace přepínačů pro vybranou položku
	ProgAktItem();
}


/////////////////////////////////////////////////////////////////////////////
// zahájení sledování změn adresáře

void BeginDirChange(HANDLE* dir1, HANDLE* dir2, CText& path1, CText& path2)
{
	if (*dir1 == INVALID_HANDLE_VALUE)
	{
		*dir1 = ::FindFirstChangeNotification(
			path1,							// cesta k adresáři
			TRUE,							// hlásit podadredáře
			FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES);
	}

	if ((*dir2 == INVALID_HANDLE_VALUE) && path2.IsNotEmpty())
	{
		*dir2 = ::FindFirstChangeNotification(
			path2,							// cesta k adresáři
			TRUE,							// hlásit podadredáře
			FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES);
	}
}


/////////////////////////////////////////////////////////////////////////////
// ukončení režimu editace souboru

void ProgTerm()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;
	ProgMode = FALSE;

// ukončení preview
	PreviewStop();

	if (PreviewTimer)
	{
		::KillTimer(MainFrame, PreviewTimer);
		PreviewTimer = 0;
	}
	PreviewTimerN = -1;
	PreviewPicOff();

// ukončení registrace UNDO
	BufCls.UndoOff();
	BufObj.UndoOff();
	BufLoc.UndoOff();
	BufEdi.UndoOff();
	BufStr.UndoOff();

	Real.UndoOff();
	Text.UndoOff();
	Bool.UndoOff();
	Icon.UndoOff();
	Map.UndoOff();
	Picture.UndoOff();
	Sprite.UndoOff();
	Sound.UndoOff();
	Music.UndoOff();

// zrušení hlášení o změně adresáře
	if (ProgDirChangeTimer)
	{
		::KillTimer(MainFrame, ProgDirChangeTimer);
		ProgDirChangeTimer = 0;
	}

	if (FunctionDirChange != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(FunctionDirChange);
		FunctionDirChange = INVALID_HANDLE_VALUE;
	}

	if (NumberDirChange != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(NumberDirChange);
		NumberDirChange = INVALID_HANDLE_VALUE;
	}

	if (TextDirChange != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(TextDirChange);
		TextDirChange = INVALID_HANDLE_VALUE;
	}

	if (BoolDirChange != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(BoolDirChange);
		BoolDirChange = INVALID_HANDLE_VALUE;
	}

	if (IconDirChange != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(IconDirChange);
		IconDirChange = INVALID_HANDLE_VALUE;
	}

	if (MapDirChange != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(MapDirChange);
		MapDirChange = INVALID_HANDLE_VALUE;
	}

	if (PictureDirChange != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(PictureDirChange);
		PictureDirChange = INVALID_HANDLE_VALUE;
	}

	if (SpriteDirChange != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(SpriteDirChange);
		SpriteDirChange = INVALID_HANDLE_VALUE;
	}

	if (SoundDirChange != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(SoundDirChange);
		SoundDirChange = INVALID_HANDLE_VALUE;
	}

	if (MusicDirChange != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(MusicDirChange);
		MusicDirChange = INVALID_HANDLE_VALUE;
	}



	if (FunctionDirChange2 != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(FunctionDirChange2);
		FunctionDirChange2 = INVALID_HANDLE_VALUE;
	}

	if (NumberDirChange2 != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(NumberDirChange2);
		NumberDirChange2 = INVALID_HANDLE_VALUE;
	}

	if (TextDirChange2 != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(TextDirChange2);
		TextDirChange2 = INVALID_HANDLE_VALUE;
	}

	if (BoolDirChange2 != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(BoolDirChange2);
		BoolDirChange2 = INVALID_HANDLE_VALUE;
	}

	if (IconDirChange2 != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(IconDirChange2);
		IconDirChange2 = INVALID_HANDLE_VALUE;
	}

	if (MapDirChange2 != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(MapDirChange2);
		MapDirChange2 = INVALID_HANDLE_VALUE;
	}

	if (PictureDirChange2 != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(PictureDirChange2);
		PictureDirChange2 = INVALID_HANDLE_VALUE;
	}

	if (SpriteDirChange2 != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(SpriteDirChange2);
		SpriteDirChange2 = INVALID_HANDLE_VALUE;
	}

	if (SoundDirChange2 != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(SoundDirChange2);
		SoundDirChange2 = INVALID_HANDLE_VALUE;
	}

	if (MusicDirChange2 != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(MusicDirChange2);
		MusicDirChange2 = INVALID_HANDLE_VALUE;
	}


// vypnutí automatického focusu
	FocusTimerN = 0;
	FocusEditModeBuf = -1;
	if (FocusTimer)
	{
		::KillTimer(MainFrame, FocusTimer);
		FocusTimer = 0;
	}

// vypnutí módu editace
	SetToolMode(-1);

// skrytí okna struktur
//	::ShowWindow(TreeStr, SW_HIDE);

// zrušení okna globálních objektů
	BufObj.Disp(-2);
	BufObj.Tree(NULL);
	::DestroyWindow(TreeObj);
	TreeObj = NULL;

// zrušení okna lokálních objektů
	BufLoc.Disp(-2);
	BufLoc.Tree(NULL);
	::DestroyWindow(TreeLoc);
	TreeLoc = NULL;

// zrušení okna editoru
	BufEdi.Disp(-2);
	BufEdi.Tree(NULL);
	::DestroyWindow(TreeEdi);
	TreeEdi = NULL;

// zrušení okna struktur
	BufStr.Disp(-2);
	BufStr.Tree(NULL);
	::DestroyWindow(TreeStr);
	TreeStr = NULL;
	ASSERT(BufStr.Num() == sizeof(InitTabStr)/sizeof(InitTabStr[0]));

// zrušení okna tříd
	BufCls.Disp(-2);
	BufCls.Tree(NULL);
	::DestroyWindow(TreeCls);
	TreeCls = NULL;

// zrušení posuvných lišt
	::DestroyWindow(HScroll);
	HScroll = NULL;
	::DestroyWindow(VScroll);
	VScroll = NULL;

	if (ToolEditIcon)
	{
		::DestroyWindow(ToolEditIcon);
		ToolEditIcon = NULL;
	}

	if (ToolEditIconWidth)
	{
		::DestroyWindow(ToolEditIconWidth);
		ToolEditIconWidth = NULL;
	}

	if (ToolEditMap)
	{
		::DestroyWindow(ToolEditMap);
		ToolEditMap = NULL;
	}

// skrytí okna editoru čísla
	::ShowWindow(EditNumWnd, SW_HIDE);

// skrytí okna editoru logických hodnot
	::ShowWindow(EditLogWnd, SW_HIDE);

// skrytí okna editoru textu
	::ShowWindow(EditTextWnd, SW_HIDE);

// skrytí okna editoru zvuku
	EditSound::OnStop();
	::ShowWindow(EditSound::DispWnd, SW_HIDE);

// skrytí okna editoru hudby
	EditMusic::OnStop();
	::ShowWindow(EditMusic::DispWnd, SW_HIDE);

// zrušení okna editoru plochy
	EditMap::EndEditSwcNum();

// zrušení seznamu obrázků
	::ImageList_Destroy(ProgImageList);
	ProgImageList = NULL;
	::ImageList_Destroy(ProgImageListMedium);
	ProgImageListMedium = NULL;
	::ImageList_Destroy(ProgImageListSmall);
	ProgImageListSmall = NULL;
	::ImageList_Destroy(ProgStateList);
	ProgStateList = NULL;
	::ImageList_Destroy(ProgStateListMedium);
	ProgStateListMedium = NULL;
	::ImageList_Destroy(ProgStateListSmall);
	ProgStateListSmall = NULL;

// vyprázdnění bufferu historie editace (přes zrušením bufferů!)
	HistEdit::DelAll();
	HistEdit::Update();

// vyprázdnění bufferů programu (jen pracovních)
	ASSERT(BufInt.Num() == FncNum);
	BufObj.DelAll();
	BufLoc.DelAll();
	BufEdi.DelAll();
	BufCls.DelAll();

// vyprázdnění pracovních bufferů
	Real.DelAll();								// buffer reálných čísel
	Text.DelAll();								// buffer textů
	Bool.DelAll();								// buffer logických proměnných
	Icon.DelAll();								// buffer ikon
	Map.DelAll();								// buffer ploch
	Sprite.DelAll();							// buffer sprajtů
	Sound.DelAll();								// buffer zvuků
	Picture.DelAll();							// buffer obrázků
	Music.DelAll();								// buffer hudby

	Undo.DelAll();								// buffer undo a redo

// zrušení všech tlačítek v panelu nástrojů
	ToolBarClear(0);
}


/////////////////////////////////////////////////////////////////////////////
// nastavení fokusu na aktivní okno

void ProgSetFocus()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// načtení okna s fokusem
	HWND wndOld = ::GetFocus();

// vypnutí automatického focusu
	FocusTimerN = 0;
	FocusEditModeBuf = -1;
	if (FocusTimer)
	{
		::KillTimer(MainFrame, FocusTimer);
		FocusTimer = 0;
	}

// příprava nového okna
	HWND wndNew = MainFrame;

	if ((FocusBuf != BufEdiID) || (EditMode == BufEdiID))
	{
		wndNew = BufProg[FocusBuf].Tree();
	}

	if ((FocusBuf == BufEdiID) && (EditMode == BufNumID))
	{
		wndNew = EditNumWnd;
	}

	if ((FocusBuf == BufEdiID) && (EditMode == BufLogID))
	{
		wndNew = EditLogWnd;
	}

	if ((FocusBuf == BufEdiID) && (EditMode == BufTxtID))
	{
		wndNew = EditTextWnd;
	}

	if ((FocusBuf == BufEdiID) && (EditMode == BufMapID) && EditMapNumWnd)
	{
		wndNew = EditMapNumWnd;
	}

// nastavení fokusu na nové okno
	if (wndNew != wndOld)
	{
//		NoSelAkt = TRUE;
		::SetFocus(wndNew);
		ProgOnSetFocus(wndNew);
//		NoSelAkt = FALSE;

// zahájení čítání preview
		PreviewStop();
	}
}


/////////////////////////////////////////////////////////////////////////////
// update voleb bloku

void ProgUpdateClipboard()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// aktualizace voleb při editaci jména
	if (EditName)
	{
		long int start, end;
		::SendMessage(EditNameWnd, EM_GETSEL, (WPARAM)&start, (WPARAM)&end);
		BOOL enable = (end != start);
		int len = ::GetWindowTextLength(EditNameWnd);

		EnableCommand(IDN_CUT, enable);
		EnableCommand(IDN_COPY, enable);
		EnableCommand(IDN_PASTE, ::IsClipboardFormatAvailable(CF_TEXT));
		EnableCommand(IDN_CLEAR, enable || (start < len));
		EnableCommand(IDN_ALL, len > 0);
		return;
	}

// nastavení panelu nástrojů
	if (FocusBuf == BufEdiID)
	{
		switch(EditMode)
		{
		case BufEdiID:
			ProgClip::UpdateClipboard();
			break;

		case BufNumID:
			EditNum::UpdateClipboard();
			break;

		case BufTxtID:
			EditText::UpdateClipboard();
			break;

		case BufLogID:
			SetStatusText2(EmptyText);
			EnableCommand(IDN_CUT, FALSE);
			EnableCommand(IDN_COPY, FALSE);
			EnableCommand(IDN_PASTE, FALSE);
			EnableCommand(IDN_CLEAR, FALSE);
			EnableCommand(IDN_ALL, FALSE);
			break;

		case BufIcoID:
		case BufPicID:
			EditIcon::UpdateClipboard();
			break;

		case BufMapID:
			EditMap::UpdateClipboard();
			break;

		case BufSprID:
			EnableCommand(IDN_CUT, FALSE);
			EnableCommand(IDN_COPY, FALSE);
			EnableCommand(IDN_PASTE, FALSE);
			EnableCommand(IDN_CLEAR, FALSE);
			EnableCommand(IDN_ALL, FALSE);
			break;

		case BufSndID:
			EditSound::UpdateClipboard();
			break;

		case BufMusID:
			EditMusic::UpdateClipboard();
			break;

		default:
			ASSERTERROR;
		}
	}
	else
	{
		ProgClip::UpdateClipboard();
	}
}


/////////////////////////////////////////////////////////////////////////////
// update UNDO a REDO

void ProgUpdateUndoRedo()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// aktualizace voleb při editaci jména
	if (EditName)
	{
		BOOL enable = ::SendMessage(EditNameWnd, EM_CANUNDO, 0, 0);
		EnableCommand(IDN_UNDO, enable);
		EnableCommand(IDN_REDO, enable);
		return;
	}

// aktualizace voleb při editaci čísla plochy
	if (EditMapNumWnd)
	{
		BOOL enable = ::SendMessage(EditMapNumWnd, EM_CANUNDO, 0, 0);
		EnableCommand(IDN_UNDO, enable);
		EnableCommand(IDN_REDO, enable);
		return;
	}

// nastavení panelu nástrojů
	Undo.UpdateUndoRedo();
}


/////////////////////////////////////////////////////////////////////////////
// update menu

void ProgUpdateMenu()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// nastavení menu
	ProgSetMenu();

// nastavení panelu nástrojů
	if (FocusBuf == BufEdiID)
	{
		switch(EditMode)
		{
		case BufIcoID:
		case BufPicID:
			EditIcon::UpdateMenu();
			break;

		case BufSprID:
			EditSound::UpdateMenu();
			break;

		case BufSndID:
			EditSound::UpdateMenu();
			break;

		case BufMusID:
			EditMusic::UpdateMenu();
			break;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// update voleb pro ZOOM

void ProgUpdateInOut()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// aktualizace voleb při editaci jména
	if (EditName)
	{
		EnableCommand(IDN_IN, FALSE);
		EnableCommand(IDN_OUT, FALSE);
		return;
	}

// nastavení panelu nástrojů
	if (FocusBuf == BufEdiID)
	{
		switch(EditMode)
		{
		case BufEdiID:
			EnableCommand(IDN_IN, BufZoom[BufEdiID] != SI_BIG);
			EnableCommand(IDN_OUT, BufZoom[BufEdiID] != SI_SMALL);
			break;

		case BufIcoID:
		case BufPicID:
			EditIcon::UpdateZoomInOut();
			break;

		default:				
			EnableCommand(IDN_IN, FALSE);
			EnableCommand(IDN_OUT, FALSE);
		}
	}
	else
	{
		EnableCommand(IDN_IN, BufZoom[FocusBuf] != SI_BIG);
		EnableCommand(IDN_OUT, BufZoom[FocusBuf] != SI_SMALL);
	}
}


/////////////////////////////////////////////////////////////////////////////
// nastavení fokusu na okno

void ProgOnSetFocus(HWND hWnd)
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// výběr textu v editoru čísla
	if (hWnd == EditNumWnd)
	{
		::SendMessage(EditNumWnd, EM_SETSEL, 0, (LPARAM)-1);
	}

// ukončení editace číselného parametru plochy
	if (hWnd != EditMapNumWnd)
	{
		EditMap::EndEditSwcNum();
	}

// rozlišení zdrojového okna
	int bufID = BufProgIDFromHandle(hWnd);
	if (bufID < 0) return;
	FocusBuf = bufID;

// překreslení starého i nového okna
	TimeRepaint(FocusBuf);
	TimeRepaint(OldFocusBuf);
	OldFocusBuf = FocusBuf;

// aktivace okna stromu editoru
//	if ((hWnd == TreeEdi) && (EditMode != BufEdiID))
//	{
//		SetEditMode(BufEdiID, 0);
//	}

// nastavení panelu nástrojů
	if (bufID == BufEdiID)
	{
		SetToolMode(EditMode);

		switch(EditMode)
		{
		case BufSndID:
		case BufMusID:
		case BufLogID:
			SetStatusText2(EmptyText);
			break;
		}
	}
	else
	{
		SetToolMode(bufID);
	}

	ProgUpdateClipboard();
	ProgUpdateUndoRedo();
	ProgUpdateInOut();
	ProgUpdateMenu();

// aktualizace přepínačů pro vybranou položku
	ProgDispNadpis();
	ProgAktItem();

// zahájení čítání preview
	PreviewStop();
}


/////////////////////////////////////////////////////////////////////////////
// změna velikosti okna

void ProgOnSize()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// posun panelu nástrojů
	ToolBarResize();

// zahájení přesouvání oken
	HDWP hdwp = ::BeginDeferWindowPos(30);

// načtení výšky stavové lišty (i když to není potřeba)
	RECT rcStat;
	::GetWindowRect(StatusBar, &rcStat);

// posun stavového okna
	hdwp = ::DeferWindowPos(hdwp, StatusBar, NULL, ClientRect.left, ClientRect.bottom, ClientWidth, rcStat.bottom - rcStat.top,
			SWP_DRAWFRAME | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);

// inicializace klientské oblasti
	InitClientRect();

// aktualizace předělů stavového okna
	SetStatusWidth(ProgStatusWidth);

// příprava předělů
	TrackX1 = ClientRect.left + (ClientWidth*LeftWidth + Scale/2)/Scale;
	if (TrackX1 > (ClientRect.right - 8)) TrackX1 = ClientRect.right - 8;
	if (TrackX1 < ClientRect.left + 4) TrackX1 = ClientRect.left + 4;

	TrackX2 = ClientRect.right - (ClientWidth*RightWidth + Scale/2)/Scale;
	if (TrackX2 > (ClientRect.right - 4)) TrackX2 = ClientRect.right - 4;
	if (TrackX2 < ClientRect.left + 8) TrackX2 = ClientRect.left + 8;

	TrackY1 = ClientRect.top + (ClientHeight*LeftHeight + Scale/2)/Scale;
	if (TrackY1 > (ClientRect.bottom - NADPISHEIGHT - 4)) TrackY1 = ClientRect.bottom - NADPISHEIGHT - 4;
	if (TrackY1 < ClientRect.top + NADPISHEIGHT + 4) TrackY1 = ClientRect.top + NADPISHEIGHT + 4;

	TrackY2 = ClientRect.top + (ClientHeight*RightHeight + Scale/2)/Scale;
	if (TrackY2 > (ClientRect.bottom - NADPISHEIGHT - 4)) TrackY2 = ClientRect.bottom - NADPISHEIGHT - 4;
	if (TrackY2 < ClientRect.top + NADPISHEIGHT + 4) TrackY2 = ClientRect.top + NADPISHEIGHT + 4;

// rozměry editačního pole
	EditX = TrackX1;
	EditY = ClientRect.top + NADPISHEIGHT;
	EditWidth = TrackX2 - TrackX1;
	EditHeight = ClientHeight - NADPISHEIGHT;

// změna velikosti okna globálních objektů
	hdwp = ::DeferWindowPos(hdwp, TreeObj, NULL, ClientRect.left, ClientRect.top + NADPISHEIGHT, TrackX1 - ClientRect.left, TrackY1 - ClientRect.top - NADPISHEIGHT,
			SWP_DRAWFRAME | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);

// změna velikosti okna lokálních objektů
	hdwp = ::DeferWindowPos(hdwp, TreeLoc, NULL, ClientRect.left, TrackY1 + NADPISHEIGHT, TrackX1 - ClientRect.left, ClientRect.bottom - TrackY1 - NADPISHEIGHT,
			SWP_DRAWFRAME | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);

// změna velikosti okna editoru (i když není zobrazeno)
	hdwp = ::DeferWindowPos(hdwp, TreeEdi, NULL, EditX, EditY, EditWidth, EditHeight,
			SWP_DRAWFRAME | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);

// změna velikosti okna struktur
	hdwp = ::DeferWindowPos(hdwp, TreeStr, NULL, TrackX2, ClientRect.top + NADPISHEIGHT, ClientRect.right - TrackX2, TrackY2 - ClientRect.top - NADPISHEIGHT,
			SWP_DRAWFRAME | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);

// změna velikosti okna tříd
	hdwp = ::DeferWindowPos(hdwp, TreeCls, NULL, TrackX2, TrackY2 + NADPISHEIGHT, ClientRect.right - TrackX2, ClientRect.bottom - TrackY2 - NADPISHEIGHT,
			SWP_DRAWFRAME | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);

// nastavení posuvníků
	switch (EditMode)
	{
	case BufMapID:
		hdwp = EditMap::MoveScroll(hdwp);
		break;

	case BufSprID:
		hdwp = EditSprite::MoveScroll(hdwp);
		break;

	case BufIcoID:
	case BufPicID:
		hdwp = EditIcon::MoveScroll(hdwp);
		break;
	
	default:;
		hdwp = ::DeferWindowPos(hdwp, HScroll, NULL, 0, 0, TRACKSIRKA, TRACKSIRKA,
			SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER);
		hdwp = ::DeferWindowPos(hdwp, VScroll, NULL, 0, 0, TRACKSIRKA, TRACKSIRKA,
			SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER);
	}

// posun editačního pole editoru čísel
	hdwp = EditNum::MoveEdit(hdwp);	

// posun editačního pole editoru logických hodnot
	hdwp = EditLog::MoveEdit(hdwp);

// posun editačního pole editoru textu
	hdwp = EditText::MoveEdit(hdwp);	

// posun displeje editoru zvuku
	hdwp = EditSound::MoveDisp(hdwp);

// posun displeje editoru hudby
	hdwp = EditMusic::MoveDisp(hdwp);

// konec přesouvání oken
	::EndDeferWindowPos(hdwp);

// překreslení okna
	::UpdateWindow(MainFrame);
}


/////////////////////////////////////////////////////////////////////////////
// zjištění bufferu programu z handle okna (NULL = není)

CBufProg* BufProgFromHandle(HWND hWnd)
{
	if ((hWnd == TreeEdi) || 
		(hWnd == MainFrame) || 
		(hWnd == EditNumWnd) ||
		(hWnd == EditLogWnd) ||
		(hWnd == EditTextWnd) ||
		(hWnd == EditMapNumWnd) ||
		(hWnd == ToolEditIcon) || 
		(hWnd == ToolEditIconWidth) ||
		(hWnd == ToolEditMap) || 
		(hWnd == HScroll) ||
		(hWnd == VScroll)
		)
	{
		return &BufEdi;
	}

	if (hWnd == TreeStr)
	{
		return &BufStr;
	}

	if (hWnd == TreeObj)
	{
		return &BufObj;
	}

	if (hWnd == TreeLoc)
	{
		return &BufLoc;
	}

	if (hWnd == TreeCls)
	{
		return &BufCls;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění ID bufferu programu z handle okna (-1 = není)

int BufProgIDFromHandle(HWND hWnd)
{
	if ((hWnd == TreeEdi) || 
		(hWnd == MainFrame) || 
		(hWnd == EditNumWnd) ||
		(hWnd == EditLogWnd) ||
		(hWnd == EditTextWnd) ||
		(hWnd == EditMapNumWnd) ||
		(hWnd == ToolEditIcon) || 
		(hWnd == ToolEditIconWidth) ||
		(hWnd == ToolEditMap) || 
		(hWnd == HScroll) ||
		(hWnd == VScroll)
		)
	{
		return BufEdiID;
	}

	if (hWnd == TreeStr)
	{
		return BufStrID;
	}

	if (hWnd == TreeObj)
	{
		return BufObjID;
	}

	if (hWnd == TreeLoc)
	{
		return BufLocID;
	}

	if (hWnd == TreeCls)
	{
		return BufClsID;
	}

	return -1;
}


/////////////////////////////////////////////////////////////////////////////
// zahájení rozvinutí položky (vrací TRUE=zákaz změny)
// (pozor - vyvolá se po dvojkliku v editačním okně u zrušené položky,
//  bez zákazu rozvinutí vznikne chyba paměti! Bez této obsluhy havaruje!)

BOOL ProgOnExpanding(HWND hWnd, TV_ITEM* tvi, BOOL expand)
{
// kontrola, zda je režim editace
	if (!ProgMode) return FALSE;

// rozlišení zdrojového okna
	CBufProg* buf = BufProgFromHandle(hWnd);
	if (buf == NULL) return TRUE;

// nalezení položky
	int index = buf->Find(tvi->hItem);
	if (index < 0) return TRUE;

// obsluha okna tříd
	if (buf->BufID() == BufClsID)
	{
		if ((buf->At(index).Func != IDF_GROUP) &&
			(buf->At(index).Parent >= 0))
		{
			return TRUE;
		}

		if (!expand) return FALSE;

		if ((buf->At(index).Child < 0) || (buf->At(index).Param & PR_CD2))
		{
			ProgLib::Load(index);
		}
		return (buf->At(index).Child < 0);
	}

// položky bez dat lze rozvinout kdekoliv
	if ((buf->At(index).DatBlok < 0) && 
		(buf->At(index).RefBlok != BufObjID) &&
		(buf->At(index).RefBlok != BufLocID))
	{
		return FALSE;
	}

// při dvojkliku je nutno zkontrolovat místo kliknutí
	if (LMouseDClick || RMouseDClick)
	{
		TV_HITTESTINFO tvh;
		tvh.pt = MouseScreen;
		::ScreenToClient(buf->Tree(), &tvh.pt);
		::SendMessage(buf->Tree(), TVM_HITTEST, 0, (LPARAM)&tvh);
//		ASSERT(tvh.hItem == tvi->hItem);
		return ((tvh.flags & TVHT_ONITEMBUTTON) == 0);
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// změna rozvinutí položky

void ProgOnExpand(HWND hWnd, TV_ITEM* tvi, BOOL expand)
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// rozlišení zdrojového okna
	CBufProg* buf = BufProgFromHandle(hWnd);
	if (buf == NULL) return;

// nalezení položky
	int index = buf->Find(tvi->hItem);
	if (index < 0) return;

// nastavení nového příznaku
	if (expand)
	{
		(*buf)[index].Param |= PR_EXP;
	}
	else
	{
		(*buf)[index].Param &= ~PR_EXP;
	}

	TimeRepaint(buf->BufID());
};


/////////////////////////////////////////////////////////////////////////////
// změna vybrané položky

void ProgOnSelChanged(HWND hWnd, NM_TREEVIEW* tv)
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// aktualizace přepínačů pro vybranou položku
	ProgAktItem();

// zahájení čítání preview
	PreviewStop();
}

/////////////////////////////////////////////////////////////////////////////
// zvětšení měřítka zobrazení

void ProgOnZoomIn()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// nastavení ikon pro strom
	if ((FocusBuf != BufEdiID) || (EditMode == BufEdiID))
	{
		SMALLICON zoom;
		switch (BufZoom[FocusBuf])
		{
		case SI_BIG:	return;
		case SI_MEDIUM:	zoom = SI_BIG; break;
		default: zoom = SI_MEDIUM;
		}
		BufZoom[FocusBuf] = zoom;
		BufProg[FocusBuf].IconList(zoom);
		EnableCommand(IDN_IN, (zoom == SI_MEDIUM));
		EnableCommand(IDN_OUT, TRUE);
		return;
	}

// nastavení pro editační pole
	if (FocusBuf == BufEdiID)
	{
		switch (EditMode)
		{
		case BufIcoID:
		case BufPicID:
			EditIcon::ZoomIn();
			return;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// zmenšení měřítka zobrazení

void ProgOnZoomOut()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// nastavení ikon pro strom
	if ((FocusBuf != BufEdiID) || (EditMode == BufEdiID))
	{
		SMALLICON zoom;
		switch (BufZoom[FocusBuf])
		{
		case SI_SMALL:	return;
		case SI_MEDIUM:	zoom = SI_SMALL; break;
		default: zoom = SI_MEDIUM;
		}
		BufZoom[FocusBuf] = zoom;
		BufProg[FocusBuf].IconList(zoom);
		EnableCommand(IDN_IN, TRUE);
		EnableCommand(IDN_OUT, (zoom == SI_MEDIUM));
		return;
	}

// nastavení pro editační pole
	if (FocusBuf == BufEdiID)
	{
		switch (EditMode)
		{
		case BufIcoID:
		case BufPicID:
			EditIcon::ZoomOut();
			return;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// nastavení editačního módu Toolbaru (podle ID bufferu, -1 = vypnout)

void SetToolMode(int bufID)
{
// korekce bufferu
	if ((DWORD)bufID < (DWORD)PROGBUFNUM)
	{
		bufID = BufEdiID;
	}

// kontrola, zda je mód již nastaven
	if (bufID == ToolMode) return;

	if (ToolEditIcon)
	{
		::DestroyWindow(ToolEditIcon);
		ToolEditIcon = NULL;
	}

	if (ToolEditIconWidth)
	{
		::DestroyWindow(ToolEditIconWidth);
		ToolEditIconWidth = NULL;
	}

	if (ToolEditMap)
	{
		::DestroyWindow(ToolEditMap);
		ToolEditMap = NULL;
	}

// aktualizace menu
	ProgSetMenu();

// nulování panelu nástrojů
	ToolBarClear(ToolBarProgramNum);

// nastavení panelu nástrojů podle editačního módu
	switch (bufID)
	{
	case BufEdiID:	ToolBarAdd(ToolBarEditProg, ToolBarEditProgNum);
					break;

	case BufNumID:	
					break;

	case BufTxtID:	
					break;

	case BufLogID:	
					break;

	case BufIcoID:	ToolBarAdd(ToolBarEditIcon, ToolBarEditIconNum);

					if (ToolEditIcon == NULL)
					{
						ToolEditIcon = CreateToolCombo(IDD_PEN, ToolBarEditPicTools, 0);
					}

					if (ToolEditIconWidth == NULL)
					{
						ToolEditIconWidth = CreateToolCombo(IDD_WIDTH, ToolBarEditPicWidths, 1);
					}

					EditIcon::UpdateMenu();
					break;

	case BufMapID:	ToolBarAdd(ToolBarEditMap, ToolBarEditMapNum);

					if (ToolEditMap == NULL)
					{
						ToolEditMap = CreateToolCombo(IDD_MAP, ToolBarEditMapTools, 0);
					}

					EditMap::UpdateMenu();
					break;

	case BufSprID:	ToolBarAdd(ToolBarEditSprite, ToolBarEditSpriteNum);
//					EditSprite::UpdateMenu();
					break;

	case BufSndID:	ToolBarAdd(ToolBarEditSound, ToolBarEditSoundNum);
					EditSound::UpdateMenu();
					break;

	case BufPicID:	ToolBarAdd(ToolBarEditPic, ToolBarEditPicNum);

					if (ToolEditIcon == NULL)
					{
						ToolEditIcon = CreateToolCombo(IDD_PEN, ToolBarEditPicTools, 0);
					}

					if (ToolEditIconWidth == NULL)
					{
						ToolEditIconWidth = CreateToolCombo(IDD_WIDTH, ToolBarEditPicWidths, 1);
					}

					EditIcon::UpdateMenu();
					break;

	case BufMusID:	ToolBarAdd(ToolBarEditMusic, ToolBarEditMusicNum);
					EditMusic::UpdateMenu();
					break;
	}

	ToolMode = bufID;

// aktualizace zobrazení panelu nástrojů
	ProgOnSize();
}


/////////////////////////////////////////////////////////////////////////////
// vypnutí editačních oken (-1 = vše)

void HideEditWnd(int bufID)
{
// skrytí editoru programu
	if (bufID != BufEdiID)
	{
		::ShowWindow(TreeEdi, SW_HIDE);
	}

// skrytí editoru čísel
	if (bufID != BufNumID)
	{
		::ShowWindow(EditNumWnd, SW_HIDE);
	}

// skrytí editoru logických hodnot
	if (bufID != BufLogID)
	{
		::ShowWindow(EditLogWnd, SW_HIDE);
	}

// skrytí editoru textu
	if (bufID != BufTxtID)
	{
		::ShowWindow(EditTextWnd, SW_HIDE);
	}

// skrytí okna editoru zvuku
	if (bufID != BufSndID)
	{
		::ShowWindow(EditSound::DispWnd, SW_HIDE);
	}

// skrytí okna editoru hudby
	if (bufID != BufMusID)
	{
		::ShowWindow(EditMusic::DispWnd, SW_HIDE);
	}
}


/////////////////////////////////////////////////////////////////////////////
// nastavení editačního módu edítačního panelu (TRUE=OK) (BufPicID a -2 = editace obrázku sprajtu)

BOOL SetEditMode(int bufID, int index)
{
// příprava indexu dat pro hledání v buferech
	int datblok = bufID;
	int datindex = index;
	if ((bufID == BufPicID) && (datindex == -2))
	{
		datblok = BufSprID;
		datindex = EditSprite::Index;
	}

// nalezení dat v globálním bufferu
	int editinx = BufObj.SrcDat(datblok, datindex);
	if (editinx >= 0)
	{
		BufObj.Def(editinx);
		BufLoc.Def(-1);
		BufEdi.Nadpis(Text.GetTxt(BufObj.GetText(editinx)));
		if (datblok == BufLocID)
		{
			BufLoc.Disp(datindex);
		}
	}
	else

// nalezení dat v lokálním bufferu
	{
		editinx = BufLoc.SrcDat(datblok, datindex);
		if (editinx >= 0)
		{
			BufLoc.Def(editinx);
			BufEdi.Nadpis(Text.GetTxt(BufLoc.GetText(editinx)));
			while (BufLoc[editinx].Parent >= 0)
			{
				editinx = BufLoc[editinx].Parent;
			}
			BufLoc.Disp(editinx);
			BufObj.Def(BufObj.SrcDat(BufLocID, editinx));
		}
		else
		{
			if ((bufID != BufIcoID) || (Icon.IsNotValid(index)))
			{
				return FALSE;
			}

			if (bufID == BufIcoID)
			{
				for (int i = BufEdi.Max()-1; i >= 0; i--)
				{
					if ((BufEdi.IsValid(i)) && 
						(BufEdi[i].Icon == index) &&
						(BufEdi[i].Func == IDF_COLOR))
					{
						bufID = BufEdiID;
						while (BufEdi[i].Parent >= 0) i = BufEdi[i].Parent;
						index = i;
						break;
					}
				}
			}
		}
	}

// korekce bufferu
	int bufID0 = bufID;
	if ((DWORD)bufID < (DWORD)PROGBUFNUM)
	{
		bufID = BufEdiID;
	}

// nastavení panelu nástrojů
	if (FocusBuf == BufEdiID)
	{
		SetToolMode(bufID);
	}

// vypnutí posuvníků
	if (bufID != EditMode)
	{
		HScrollDisp = FALSE;
		VScrollDisp = FALSE;
		::ShowWindow(HScroll, SW_HIDE);
		::ShowWindow(VScroll, SW_HIDE);
	}

// vypnutí editací
	EditSound::OnStop();
	EditMusic::OnStop();
	EditMap::EndEditSwcNum();

// zafixování změn UNDO
	Undo.Fixup();

// nastavení editačního pole podle editačního módu
	switch (bufID0)
	{
	case BufLocID:	if (BufLoc.IsValid(index) && (BufLoc[index].DatBlok == BufEdiID))
					{
						HistEdit::Push();
						EditMode = bufID;
						EditBuf = bufID0;
						EditItem = index;
						index = BufLoc[index].DatIndex;
						::ShowWindow(TreeEdi, SW_SHOW);
						BufEdi.Disp(index);
						HideEditWnd(bufID);
						ProgUpdateInOut();
						ProgOnSize();
						return TRUE;
					}
					break;

	case BufEdiID:	if (BufEdi.IsValid(index))
					{
						HistEdit::Push();
						EditMode = bufID;
						EditBuf = bufID0;
						EditItem = index;
						::ShowWindow(TreeEdi, SW_SHOW);
						BufEdi.Disp(index);
						HideEditWnd(bufID);
						ProgUpdateInOut();
						ProgOnSize();
						return TRUE;
					}
					break;

	case BufNumID:	if (Real.IsValid(index))
					{
						HistEdit::Push();
						EditMode = bufID;
						EditBuf = bufID0;
						EditItem = index;
						EditNum::BegEdit(index);
						HideEditWnd(bufID);
						ProgUpdateInOut();
						ProgOnSize();
						return TRUE;
					}
					break;

	case BufTxtID:	if (Text.IsValid(index))
					{
						HistEdit::Push();
						EditMode = bufID;
						EditBuf = bufID0;
						EditItem = index;
						EditText::BegEdit(index);
						HideEditWnd(bufID);
						ProgUpdateInOut();
						ProgOnSize();
						return TRUE;
					}
					break;

	case BufLogID:	if (Bool.IsValid(index))
					{
						HistEdit::Push();
						EditMode = bufID;
						EditBuf = bufID0;
						EditItem = index;
						EditLog::BegEdit(index);
						HideEditWnd(bufID);
						ProgUpdateInOut();
						ProgOnSize();
						return TRUE;
					}
					break;

	case BufIcoID:	if (Icon.IsValid(index))
					{
						HistEdit::Push();
						EditMode = bufID;
						EditBuf = bufID0;
						EditItem = index;
						EditIcon::BegEdit(BufIcoID, index);
						HideEditWnd(bufID);
						ProgUpdateInOut();
						ProgOnSize();
						return TRUE;
					}
					break;

	case BufMapID:	if (Map.IsValid(index))
					{
						HistEdit::Push();
						EditMode = bufID;
						EditBuf = bufID0;
						EditItem = index;
						EditMap::BegEdit(index);
						HideEditWnd(bufID);
						ProgUpdateInOut();
						ProgOnSize();
						return TRUE;
					}
					break;

	case BufPicID:	if ((index == -2) || Picture.IsValid(index))
					{
						HistEdit::Push();
						EditMode = bufID;
						EditBuf = bufID0;
						EditItem = index;
						EditIcon::BegEdit(BufPicID, index);
						HideEditWnd(bufID);
						ProgUpdateInOut();
						ProgOnSize();
						return TRUE;
					}
					break;

	case BufSprID:	if (Sprite.IsValid(index))
					{
						HistEdit::Push();
						EditMode = bufID;
						EditBuf = bufID0;
						EditItem = index;
						EditSprite::BegEdit(index);
						HideEditWnd(bufID);
						ProgUpdateInOut();
						ProgOnSize();
						return TRUE;
					}
					break;

	case BufSndID:	if (Sound.IsValid(index))
					{
						HistEdit::Push();
						EditMode = bufID;
						EditBuf = bufID0;
						EditItem = index;
						EditSound::BegEdit(index);
						HideEditWnd(bufID);
						ProgUpdateInOut();
						ProgOnSize();
						return TRUE;
					}
					break;

	case BufMusID:	if (Music.IsValid(index))
					{
						HistEdit::Push();
						EditMode = bufID;
						EditBuf = bufID0;
						EditItem = index;
						EditMusic::BegEdit(index);
						HideEditWnd(bufID);
						ProgUpdateInOut();
						ProgOnSize();
						return TRUE;
					}
					break;
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// vykreslení prázdného editačního pole

void ProgDispEmpty()
{
// otevření kontextu displeje
	HDC dc = ::GetDC(MainFrame);

// oblast okna
	RECT rc;
	rc.left = EditX + 2;
	rc.top = EditY + 2;
	rc.right = EditX + EditWidth - 2;
	rc.bottom = EditY + EditHeight - 2;

// vyplnění okna
	::FillRect(dc, &rc, (HBRUSH)::GetStockObject(LTGRAY_BRUSH));

// uvolnění kontextu displeje
	::ReleaseDC(MainFrame, dc);
}


/////////////////////////////////////////////////////////////////////////////
// vykreslení rámu kolem editačního pole

void ProgDispFrame()
{
// otevření kontextu displeje
	HDC dc = ::GetDC(MainFrame);

// oblast okna
	RECT rc;
	rc.left = EditX;
	rc.top = EditY;
	rc.right = EditX + EditWidth;
	rc.bottom = EditY + EditHeight;

// vykreslení rámečku
	::DrawEdge(dc, &rc, EDGE_SUNKEN, BF_RECT);

// uvolnění kontextu displeje
	::ReleaseDC(MainFrame, dc);
}


/////////////////////////////////////////////////////////////////////////////
// příprava souřadnic nadpisu okna

void ProgNadpisRect(RECT* rc, int bufID)
{
	switch (bufID)
	{
	case BufObjID:
		rc->left = ClientRect.left;
		rc->top = ClientRect.top;
		rc->right = TrackX1;
		break;

	case BufLocID:
		rc->left = ClientRect.left;
		rc->top = TrackY1;
		rc->right = TrackX1;
		break;

	case BufStrID:
		rc->left = TrackX2;
		rc->top = ClientRect.top;
		rc->right = ClientRect.right;
		break;

	case BufClsID:
		rc->left = TrackX2;
		rc->top = TrackY2;
		rc->right = ClientRect.right;
		break;

	default:
		rc->left = TrackX1;
		rc->top = EditY - NADPISHEIGHT;
		rc->right = TrackX2;
	}
	rc->bottom = rc->top + NADPISHEIGHT;
}

/////////////////////////////////////////////////////////////////////////////
// vykreslení nadpisů oken

void ProgDispNadpis()
{
// otevření kontextu displeje
	HDC dc = ::GetDC(MainFrame);

// nastavení nového fontu
	CFont fnt;
	fnt.Bold = TRUE;
	HFONT newfont = GetFont(&fnt);
	HFONT oldfont = (HFONT)::SelectObject(dc, newfont);

// cyklus přes všechny buffery
	for (int bufID = 0; bufID < PROGBUFNUM; bufID++)
	{
		if (bufID != BufIntID)
		{

// adresa bufferu
			CBufProg* buf = BufProg + bufID;
			
// kontrola, zda je okno zobrazeno
			if (buf->Tree() != NULL)
			{
// text k zobrazení
				CText txt = buf->Nadpis();
				if ((bufID == BufEdiID) && PreviewPic && PreviewPicName[PreviewPicAkt].IsNotEmpty())
				{
					int index = BufCls.Select();
					if (BufCls.IsValid(index))
					{
						txt = Text.GetTxt(BufCls.GetText(index));
					}
				}

// příprava souřadnic nadpisu okna
				RECT rc;
				ProgNadpisRect(&rc, bufID);

// zobrazení rámečku kolem nadpisu
				::DrawEdge(dc, &rc, EDGE_BUMP, BF_RECT);

// posun souřadnic na vnitřek okna
				rc.left += 2;
				rc.top += 2;
				rc.right -= 2;
				rc.bottom -= 2;

// příprava barvy podkladu a barvy písma
				COLORREF bcol;					// barva podkladu
				COLORREF tcol;					// barva písma
				if (bufID == FocusBuf)
				{
					bcol = ::GetSysColor(COLOR_ACTIVECAPTION);
					tcol = ::GetSysColor(COLOR_CAPTIONTEXT);
				}
				else
				{
					bcol = ::GetSysColor(COLOR_INACTIVECAPTION);
					tcol = ::GetSysColor(COLOR_INACTIVECAPTIONTEXT);
				}

// příprava módu k zobrazení textu
				::SetBkColor(dc, bcol);
				::SetBkMode(dc, OPAQUE);
				::SetTextColor(dc, tcol);

// zjištění délky textu
				RECT rc2 = rc;
				::DrawText(dc, txt, txt.Length(), &rc2, DT_CALCRECT);

// nastavení zarovnávání textu
				int left;
				if ((rc2.right - rc2.left) <= (rc.right - rc.left))
				{
					left = (rc.left + rc.right)/2;
					::SetTextAlign(dc, TA_CENTER | TA_BASELINE);
				}
				else
				{
					left = rc.left;
					::SetTextAlign(dc, TA_LEFT | TA_BASELINE);
				}

// zobrazení textu s podkladem
				::ExtTextOut(dc, left, (rc.top + rc.bottom)/2 + 4, 
						ETO_CLIPPED | ETO_OPAQUE, &rc, 
						txt, txt.Length(), NULL);
			}
		}
	}

// návrat původního fontu
	::SelectObject(dc, oldfont);
	DelFont(newfont);

// uvolnění kontextu displeje
	::ReleaseDC(MainFrame, dc);
}


/////////////////////////////////////////////////////////////////////////////
// vykreslení okna

void ProgOnPaint()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// obsluha režimu preview obrázku
	if (PreviewPic)
	{
// zobrazení neplatného obrázku
		if (PreviewPicPic[PreviewPicAkt].IsEmpty())
		{
			ProgDispEmpty();
		}
		else
		{
// parametry plochy
			int widthc = EditWidth - 4;
			int heightc = EditHeight - 4;
			int leftc = EditX + 2;
			int topc = EditY + 2;

// parametry obrázku
			int width = PreviewPicPic[PreviewPicAkt].Width();
			int height = PreviewPicPic[PreviewPicAkt].Height();

			int widtho = width;
			if (widtho > widthc) widtho = widthc;
			int heighto = height;
			if (heighto > heightc) heighto = heightc;

			int lefto = (widthc - widtho)/2 + leftc;
			int topo = (heightc - heighto)/2 + topc;

			int widthbyte = (widtho + 3) & ~3;

// otevření kontextu displeje
			HDC dc = ::GetDC(MainFrame);

// nastavení vlastních palet
			HPALETTE OldPal;					// úschova původních palet
			OldPal = ::SelectPalette(dc,		// výběr vlastních palet
				StdPalette, FALSE);
			::RealizePalette(dc);				// realizace palet

// buffer k dekódování obrázku
			BYTE* buf = (BYTE*)MemGet(widthbyte * heighto);
			if (buf)
			{
				BYTE* dst = buf;
				PreviewPicPic[PreviewPicAkt].DeComp();
				BYTE* src = PreviewPicPic[PreviewPicAkt].DataData() + (height - heighto)/2*width + (width - widtho)/2;

// dekódování obrázku
				for (int i = heighto; i > 0; i--)
				{
					MemCopy(dst, src, widtho);
					dst += widthbyte;
					src += width;
				}

// přednastavení parametrů záhlaví bitmapy
				StdBitmapInfo->bmiHeader.biWidth = widtho;
				StdBitmapInfo->bmiHeader.biHeight = heighto;

// zobrazení obrázku z bufferu
				::StretchDIBits(dc, lefto, topo, widtho, heighto,
					0, 0, widtho, heighto, buf,
					StdBitmapInfo, DIB_RGB_COLORS, SRCCOPY);

// zrušení bufferu k dekódování obrázku
				MemFree(buf);
			}

// příprava štětce k vymazání podkladu
			HBRUSH brush = (HBRUSH)::GetStockObject(LTGRAY_BRUSH); // štětec k vymazání plochy

// vymazání plochy nahoře nad obrázkem
			RECT rc;
			rc.left = leftc;
			rc.right = leftc + widthc;
			rc.top = topc;
			rc.bottom = topo;
			if (rc.top < rc.bottom)
			{
				::FillRect(dc, &rc, brush);
			}

// vymazání plochy dole pod obrázkem (left a right je nastaveno)
			rc.top = topo + heighto;
			rc.bottom = topc + heightc;
			if (rc.top < rc.bottom)
			{
				::FillRect(dc, &rc, brush);
			}

// vymazání plochy vlevo od obrázku (left je nastaveno)
			rc.right = lefto;
			rc.top = topo;
			rc.bottom = topo + heighto;
			if (rc.left < rc.right)
			{
				::FillRect(dc, &rc, brush);
			}

// vymazání plochy vpravo od obrázku (top a bottom je nastaveno)
			rc.left = lefto + widtho;
			rc.right = leftc + widthc;
			if (rc.left < rc.right)
			{
				::FillRect(dc, &rc, brush);
			}

// zrušení štětce podkladu (i když podle dokumentace rušení není nutné)
			::DeleteObject(brush);

// návrat původních palet
			::SelectPalette(dc,OldPal,FALSE);

// uvolnění kontextu displeje
			::ReleaseDC(MainFrame, dc);
		}

// zobrazení rámu kolem plochy
		ProgDispFrame();

// zobrazení nadpisů oken
		ProgDispNadpis();
		return;
	}

// rozlišení podle módu
	switch (EditMode)
	{
	case BufEdiID:
					break;

	case BufTxtID:
					break;

	case BufLogID:	EditLog::Disp();
					ProgDispFrame();
					break;

	case BufNumID:	EditNum::Disp();
					ProgDispFrame();
					break;

	case BufMapID:	EditMap::Disp();
					ProgDispFrame();
					break;

	case BufSprID:	EditSprite::Disp();
					ProgDispFrame();
					break;

	case BufSndID:	EditSound::Disp();
					ProgDispFrame();
					break;

	case BufMusID:	EditMusic::Disp();
					ProgDispFrame();
					break;

	case BufIcoID:
	case BufPicID:	EditIcon::Disp();
					ProgDispFrame();
					break;


	default:	ProgDispEmpty();
				ProgDispFrame();
	}

// zobrazení nadpisů oken
	ProgDispNadpis();
}


/////////////////////////////////////////////////////////////////////////////
// dvojklik myší - editace prvku

void ProgOnDblClk(HWND hWnd)
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// rozlišení zdrojového okna
	int bufID = BufProgIDFromHandle(hWnd);
	if (bufID < 0) return;
	CBufProg* buf = &BufProg[bufID];

// zjištění aktivní položky
	TV_HITTESTINFO hti;					// struktura pro nalezení položky
	hti.pt = MouseScreen;
	::ScreenToClient(buf->Tree(), &hti.pt);
	int index = buf->Find((HTREEITEM) ::SendMessage(buf->Tree(), TVM_HITTEST, 0, (LPARAM)&hti));
	if (index < 0) index = buf->Select();
	if (index < 0) return;
	PROGITEM* item = &(*buf)[index];

// kontrola, zda není klik na rozbalovací křížek
	RECT rc;
	*(HTREEITEM*)(&rc) = item->HTree;
	if (::SendMessage(buf->Tree(), TVM_GETITEMRECT, TRUE, (LPARAM)&rc))
	{
		int pozx = rc.left - buf->IconWidth() - 3;
		if ((hti.pt.x < pozx) && (hti.pt.x > pozx - buf->IconWidth() - 4)) return;
	}

// editační buffer
	if (bufID == BufEdiID)
	{

// volba barvy
		if (item->Func == IDF_COLOR)
		{
			ProgCol::SelectColor(index);
			return;
		}

// přesměrování z editoru do okna bufferů
		bufID = item->RefBlok;
		index = item->RefIndex;
		if (((bufID != BufObjID) && (bufID != BufLocID)) || (index < 0)) return;
		buf = &BufProg[bufID];
		ASSERT(buf->IsValid(index));
		item = &(*buf)[index];
	}

// nastavení editačního módu
	item->Param &= ~PR_NEW;				// zrušení příznaku nové položky

	if ((item->DatBlok == BufEdiID) || (item->DatBlok == BufLocID))
	{
		SetFocusEditMode(item->DatBlok, item->DatIndex);
	}
	else
	{
		if (!SetEditMode(item->DatBlok, item->DatIndex)) return;
		SetFocusEdit();
	}
}


/////////////////////////////////////////////////////////////////////////////
// Enter - editace prvku

void ProgOnEnter(HWND hWnd)
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// rozlišení zdrojového okna
	int bufID = BufProgIDFromHandle(hWnd);
	if (bufID < 0) return;
	CBufProg* buf = &BufProg[bufID];

// zjištění aktivní položky
	int index = buf->Select();
	if (index < 0) return;
	PROGITEM* item = &(*buf)[index];

	if (bufID == BufEdiID)
	{

// volba barvy
		if (item->Func == IDF_COLOR)
		{
			ProgCol::SelectColor(index);
			return;
		}

// přesměrování z editoru do okna bufferů
		bufID = item->RefBlok;
		index = item->RefIndex;
		if (((bufID != BufObjID) && (bufID != BufLocID)) || (index < 0)) return;
		buf = &BufProg[bufID];
		ASSERT(buf->IsValid(index));
		item = &(*buf)[index];
	}

// zahájení preview v okně tříd
	if (FocusBuf == BufClsID) PreviewStop();

// nastavení editačního módu
	item->Param &= ~PR_NEW;				// zrušení příznaku nové položky
	if (!SetEditMode(item->DatBlok, item->DatIndex)) return;
	ProgOnSize();						// zapnutí posuvníků

// zahájení přehrávání WAV a MID
	if ((FocusBuf == BufObjID) || (FocusBuf == BufLocID))
	{
		ProgOnPlay();
	}
}


/////////////////////////////////////////////////////////////////////////////
// zjištění informace o ikoně k zobrazení

void ProgOnGetDispInfo(HWND hWnd, TV_DISPINFO* tvd)
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// kontrola, zda jsou vytvořeny platné seznamy obrázků
	if ((ProgImageList == NULL) || (ProgImageListMedium == NULL) || (ProgImageListSmall == NULL)) return;

// rozlišení zdrojového okna
	CBufProg* buf = BufProgFromHandle(hWnd);
	if (buf == NULL) return;

// nalezení položky
	int index = buf->Find(tvd->item.hItem);
	if (index < 0) return;

// načtení ikony
	int icon = buf->GetIcon(index);
	if (icon < 0) icon = 0;
	ASSERT(Icon.IsValid(icon));
	Icon1.DelAll();
	Icon1.Add(Icon.Get(icon));

// vygenerování nového seznamu ikon
	if (buf->IconHeight() > (32+24)/2)
	{
		::ImageList_Remove(ProgImageList, -1);
		Icon1.GenerList(FALSE, SI_BIG, ProgImageList);
/*
		for (int i = ICONNUM-1; i >= 0; i--)
		{
			if (Icon32List[i] == icon)
			{
				tvd->item.iImage = i;
				tvd->item.iSelectedImage = i;
				return;
			}
		}

		Icon32[Icon32Next] = Icon.Get(icon);
		Icon32List[Icon32Next] = icon;
		tvd->item.iImage = Icon32Next;
		tvd->item.iSelectedImage = Icon32Next;
		Icon32Next++;
		if (Icon32Next >= ICONNUM) Icon32Next = 0;
		::ImageList_Remove(ProgImageList, -1);
		Icon32.GenerList(FALSE, FALSE, ProgImageList);
		return;
*/
	}
	else
	{
		if (buf->IconHeight() > (24+16)/2)
		{
			::ImageList_Remove(ProgImageListMedium, -1);
			Icon1.GenerList(FALSE, SI_MEDIUM, ProgImageListMedium);
	/*
			for (int i = ICONNUM-1; i >= 0; i--)
			{
				if (Icon32List[i] == icon)
				{
					tvd->item.iImage = i;
					tvd->item.iSelectedImage = i;
					return;
				}
			}

			Icon32[Icon32Next] = Icon.Get(icon);
			Icon32List[Icon32Next] = icon;
			tvd->item.iImage = Icon32Next;
			tvd->item.iSelectedImage = Icon32Next;
			Icon32Next++;
			if (Icon32Next >= ICONNUM) Icon32Next = 0;
			::ImageList_Remove(ProgImageList, -1);
			Icon32.GenerList(FALSE, FALSE, ProgImageList);
			return;
	*/
		}
		else
		{
			::ImageList_Remove(ProgImageListSmall, -1);
			Icon1.GenerList(FALSE, SI_SMALL, ProgImageListSmall);
	/*
			for (int i = ICONNUM-1; i >= 0; i--)
			{
				if (Icon16List[i] == icon)
				{
					tvd->item.iImage = i;
					tvd->item.iSelectedImage = i;
					return;
				}
			}

			Icon16[Icon16Next] = Icon.Get(icon);
			Icon16List[Icon16Next] = icon;
			tvd->item.iImage = Icon16Next;
			tvd->item.iSelectedImage = Icon16Next;
			Icon16Next++;
			if (Icon16Next >= ICONNUM) Icon16Next = 0;
			::ImageList_Remove(ProgImageListSmall, -1);
			Icon16.GenerList(FALSE, TRUE, ProgImageListSmall);
			return;
	*/
		}
	}

// nastavení ikony k zobrazení
	tvd->item.iImage = 0;
	tvd->item.iSelectedImage = 0;
}


/////////////////////////////////////////////////////////////////////////////
// obsluha modifikace

// Pozor na překryv obrázků tlačítek - při uložení klávesou Ctrl+S se obrázky 
// překreslí bez vymazání podkladu, šedá barva přitom zůstane nepřekreslena.

// zapnutí modifikace
void SetModi()
{
	if (!Modi)
	{
		Modi = TRUE;
		ToolBarBitmap(IDN_SAVE, ButtonSaveModi);
	}
}

// vypnutí modifikace
void ResModi()
{
	if (Modi)
	{
		Modi = FALSE;
		ToolBarBitmap(IDN_SAVE, ButtonSave);
	}
}


// podmíněné uložení při modifikaci (TRUE=pokračovat)
BOOL TestModi()
{
	if (Modi)
	{
		CText text;
		CText nadpis;
		text.Load(IDN_FILEMODI);
		nadpis.Load(IDN_FILEMODICAP);
/*
		MSGBOXPARAMS par;
		par.cbSize = sizeof(MSGBOXPARAMS);
		par.hwndOwner = MainFrame;
		par.hInstance = hInstance;
		par.lpszText = text;
		par.lpszCaption = nadpis;
		par.dwStyle = MB_USERICON | MB_YESNOCANCEL | MB_APPLMODAL | MB_SETFOREGROUND;
		par.lpszIcon = MAKEINTRESOURCE(IDN_MAINFRAME);
		par.dwContextHelpId = 0;
		par.lpfnMsgBoxCallback = NULL;
		par.dwLanguageId = MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT);

		int volba = ::MessageBoxIndirect(&par);
*/
		int volba = ::MessageBox(MainFrame, text, nadpis,
				MB_YESNOCANCEL | MB_ICONQUESTION);

		if (volba == IDCANCEL)
			return FALSE;

		if (volba == IDNO)
		{
			return TRUE;
		}

		if (volba == IDYES)
		{
			ProgFile::SaveMaxi();
			if (!Modi) 
			{
				return TRUE;
			}
		}
		return FALSE;			// pro CANCEL nebo něco jiného
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// test kurzoru myši na předělu (v klientských souřadnicích)

BOOL TestTrack(int x, int y, BOOL vert, BOOL left)
{
// úschova typu předělu
	VTracking = vert;
	LTracking = left;

// příprava souřadnic taženého obdélníku
	RECT TrackRect;

	if (vert)
	{
		TrackRect.top = ClientRect.top;
		TrackRect.bottom = ClientRect.bottom;

		if (left)
		{
			TrackRect.left = TrackX1 - TRACKOFF;
			TrackRect.right = TrackX1 + TRACKOFF;
		}
		else
		{
			TrackRect.left = TrackX2 - TRACKOFF;
			TrackRect.right = TrackX2 + TRACKOFF;
		}
	}
	else
	{
		if (left)
		{
			TrackRect.top = TrackY1 - TRACKOFF;
			TrackRect.bottom = TrackY1 + TRACKOFF;
			TrackRect.left = ClientRect.left;
			TrackRect.right = TrackX1;
		}
		else
		{
			TrackRect.top = TrackY2 - TRACKOFF;
			TrackRect.bottom = TrackY2 + TRACKOFF;
			TrackRect.left = TrackX2;
			TrackRect.right = ClientRect.right;
		}
	}

// test, zda je bod nad předělem
	return ((x >= TrackRect.left) &&
			(x < TrackRect.right) &&
			(y >= TrackRect.top) &&
			(y < TrackRect.bottom));
}


/////////////////////////////////////////////////////////////////////////////
// obsluha zobrazení kurzoru myši (vrací TRUE=nastaveno)

BOOL ProgOnSetCursor()
{
// kontrola, zda je režim editace
	if (!ProgMode) return FALSE;

// tažení má vlastní obsluhu
	if (Draging) return FALSE;

// nastavení kurzoru myši, je-li tažení předělu
	if (Tracking)
	{
		if (VTracking)
		{
			CurAkt = CurSplitH;		// tažení vertikálního předělu
		}
		else
		{
			CurAkt = CurSplitV;		// tažení horizontálního předělu
		}

		::SetCursor(CurAkt);		// nastavení kurzoru
		return TRUE;
	}

// příprava ke zjištění, zda je předěl k tažení
//	::GetCursorPos(&MouseScreen);	// načtení souřadnic myši
//	MouseMain = MouseScreen;
//	::ScreenToClient(MainFrame, &MouseMain); // převod na klientské souřadnice

	POINT pt = MouseMain;

// test vertikálních předělů
	if (TestTrack(pt.x, pt.y, TRUE, TRUE) ||
		TestTrack(pt.x, pt.y, TRUE, FALSE))
	{
		CurAkt = CurSplitH;		// tažení vertikálního předělu
		::SetCursor(CurAkt);	// nastavení kurzoru
		return TRUE;
	}

// test horizontálních předělů
	if (TestTrack(pt.x, pt.y, FALSE, TRUE) ||
		TestTrack(pt.x, pt.y, FALSE, FALSE))
	{
		CurAkt = CurSplitV;		// tažení horizontálního předělu
		::SetCursor(CurAkt);	// nastavení kurzoru
		return TRUE;
	}

// obsluha zobrazení kurzoru v editoru
	if ((pt.x >= EditX) && 
		(pt.x < EditX + EditWidth) &&
		(pt.y >= EditY) &&
		(pt.y < EditY + EditHeight) &&
		!PreviewPic)
	{
		switch (EditMode)
		{
		case BufMapID:
			if (EditMap::OnSetCursor(pt.x, pt.y))
				return TRUE;
			break;

		case BufIcoID:
		case BufPicID:
			if (EditIcon::OnSetCursor(pt.x, pt.y))
				return TRUE;
			break;
		}
	}

// jinak není obsluha
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// zahájení tažení předělu (TRUE=obslouženo)

BOOL BegTrack(int x, int y, BOOL vert, BOOL left)
{
// test, zda je předěl
	if (TestTrack(x, y, vert, left))
	{

// příznak zahájení taźení
		Tracking = TRUE;				// příznak obsluhy tažení

// zahájení zachytávání myši
		::SetCapture(MainFrame);
		return TRUE;
	}
	
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// ukončení tažení předělu

void EndTrack()
{
// test, zda probíhá taźení předělu
	if (Tracking)
	{

// uvolnění zachytávání myši
		::ReleaseCapture();			// ukončení zachytávání myši

// vypnutí příznaku tažení
		Tracking = FALSE;			// vypnutí příznaku tažení
	}
}


/////////////////////////////////////////////////////////////////////////////
// stisk tlačítka myši (TRUE = obslouženo)

BOOL ProgOnButtonDown(UINT flags, int x, int y, BOOL right, BOOL dclick)
{
// kontrola, zda je režim editace
	if (!ProgMode) return FALSE;

// kontrola, zda není tažení
	if (!Draging && !Tracking)
	{

// přepnutí aktivního panelu
		RECT rc;
		POINT pt;
		pt.x = x;
		pt.y = y;

		for (int bufID = 0; bufID < PROGBUFNUM; bufID++)
		{
			if ((bufID != BufIntID) && (bufID != FocusBuf))
			{
				ProgNadpisRect(&rc, bufID);
				switch (bufID)
				{
				case BufLocID:
					rc.top += TRACKOFF;
				case BufObjID:
					rc.right -= TRACKOFF;
					break;

				case BufEdiID:
					rc.right -= TRACKOFF;
					rc.bottom = EditY + EditHeight;
					rc.left += TRACKOFF;
					break;

				case BufClsID:
					rc.top += TRACKOFF;
				default:
					rc.left += TRACKOFF;
				}

				if (PtInRect(&rc, pt))
				{
					FocusBuf = bufID;

					if (PreviewPic)
					{
						ProgSetFocus();
						return TRUE;
					}

					ProgSetFocus();
				}
			}
		}

// zahájení tažení předělu
		if (BegTrack(x, y, TRUE, TRUE) ||
			BegTrack(x, y, TRUE, FALSE) ||
			BegTrack(x, y, FALSE, TRUE) ||
			BegTrack(x, y, FALSE, FALSE))
		{
			return TRUE;
		}
	}

// přerušení tažení
	BreakDrag();
	EndTrack();

// obsluha editačního pole
	if ((x >= EditX) &&
		(x < EditX + EditWidth) &&
		(y >= EditY) &&
		(y < EditY + EditHeight) &&
		!PreviewPic)
	{
		switch(EditMode)
		{
		case BufMapID:
			if (EditMap::OnButtonDown(flags, x, y, right, dclick)) return TRUE;
			break;

		case BufSprID:
			if (EditSprite::OnButtonDown(flags, x, y, right, dclick)) return TRUE;
			break;

		case BufPicID:
		case BufIcoID:
			if (EditIcon::OnButtonDown(flags, x, y, right, dclick)) return TRUE;
			break;
		}
	}


	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// posun myši při tažení

void ProgOnMouseMove(UINT flags, int x, int y)
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// přerušení tažení
	if ((flags & (MK_LBUTTON | MK_RBUTTON)) == 0)
	{
		BreakDrag();
		EndTrack();
	}

// obsluha tažení
	if (Draging)
	{
		ProgOnMoveDrag(flags, x, y);
		return;
	}

	if ((EditMode == BufMapID) && (EditMap::MDraging))
	{
		EditMap::OnMouseMove(flags, x, y);
		return;
	}

	if ((EditMode == BufSprID) && (EditSprite::MDraging))
	{
		EditSprite::OnMouseMove(flags, x, y);
		return;
	}

// obsluha tažení předělu
	if (Tracking)
	{

// vertikální předěl
		if (VTracking)
		{

// původní souřadnice X
			int oldx;

			if (LTracking)
			{
				oldx = TrackX1;
			}
			else
			{
				oldx = TrackX2;
			}

// nová souřadnice X
			int newx = x;
			if (newx < ClientRect.left + 4) newx = ClientRect.left + 4;
			if (newx > ClientRect.right - 4) newx = ClientRect.right - 4;

			if (LTracking)
			{
				if (newx > TrackX2 - 2*TRACKOFF) newx = TrackX2 - 2*TRACKOFF;
			}
			else
			{
				if (newx < TrackX1 + 2*TRACKOFF) newx = TrackX1 + 2*TRACKOFF;
			}

// kontrola, zda se souřadnice mění
			if (newx != oldx)
			{

// nastavení nové souřadnice
				if (LTracking)
				{
					TrackX1 = newx;
					LeftWidth = ((newx - ClientRect.left)*Scale + ClientWidth/2)/ClientWidth;
				}
				else
				{
					TrackX2 = newx;
					RightWidth = Scale - ((newx - ClientRect.left)*Scale + ClientWidth/2)/ClientWidth;
				}

// aktualizace zobrazení oken
				ProgOnSize();
				::UpdateWindow(MainFrame);
			}
		}

// horizontální předěl
		else
		{

// původní souřadnice Y
			int oldy;

			if (LTracking)
			{
				oldy = TrackY1;
			}
			else
			{
				oldy = TrackY2;
			}

// nová souřadnice Y 
			int newy = y;
			if (newy < ClientRect.top + NADPISHEIGHT + 4) newy = ClientRect.top + NADPISHEIGHT + 4;
			if (newy > ClientRect.bottom - NADPISHEIGHT - 4) newy = ClientRect.bottom - NADPISHEIGHT - 4;

// kontrola, zda se souřadnice mění
			if (newy != oldy)
			{

// nastavení nové souřadnice
				if (LTracking)
				{
					TrackY1 = newy;
					LeftHeight = ((newy - ClientRect.top)*Scale + ClientHeight/2)/ClientHeight;
				}
				else
				{
					TrackY2 = newy;
					RightHeight = ((newy - ClientRect.top)*Scale + ClientHeight/2)/ClientHeight;
				}

// aktualizace zobrazení oken
				ProgOnSize();
				::UpdateWindow(MainFrame);
			}
		}
	}


// obsluha editačního pole
	if ((x >= EditX) &&
		(x < EditX + EditWidth) &&
		(y >= EditY) &&
		(y < EditY + EditHeight) &&
		!PreviewPic)
	{	
		switch(EditMode)
		{
		case BufMapID:
			EditMap::OnMouseMove(flags, x, y);
			break;

		case BufSprID:
			EditSprite::OnMouseMove(flags, x, y);
			break;

		case BufPicID:
		case BufIcoID:
			EditIcon::OnMouseMove(flags, x, y);
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// uvolnění tlačítka myši

void ProgOnButtonUp(UINT keys, BOOL right)
{
// kontrola, zda je režim editace
	if (!ProgMode) return;
	
// ukončení tažení
	ProgOnEndDrag();
	EndTrack();

	if (EditName)
	{
		ProgUpdateClipboard();
		return;
	}

// obsluha editačního pole
	switch(EditMode)
	{
	case BufNumID:
		EditNum::UpdateClipboard();
		break;

	case BufMapID:
		EditMap::OnButtonUp(keys, right);
		EditMap::UpdateClipboard();
		break;

	case BufSprID:
		EditSprite::OnButtonUp(keys, right);
		break;

	case BufTxtID:
		EditText::UpdateClipboard();
		break;

	case BufPicID:
	case BufIcoID:
		EditIcon::OnButtonUp(keys, right);
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// přepnutí stavu LOCK prvku pod kurzorem

void ProgOnLock()
{

// kontrola, zda je režim editace
	if (!ProgMode) return;

// ukončení preview
	PreviewStop();

// test, zda je okno stromu
	if ((FocusBuf != BufEdiID) ||
		(EditMode == BufEdiID))
	{

// nalezení aktivního prvku
		CBufProg* buf = BufProg + FocusBuf;
		int index = buf->Select();
		if (index >= 0)
		{

// adresa položky
			PROGITEM* item = &((*buf)[index]);

// prvek nesmí být závisle uzamknutý
			if ((item->Param & PR_LOCK_DEP) || (item->Param & PR_CD & PR_CD2)) return;

// přepnutí stavu pro okno tříd
			if ((FocusBuf == BufClsID) &&
				(item->Parent >= 0))
			{
				ProgLib::InitName(index);

				int atr = (int) :: GetFileAttributes(ProgLib::m_Name);
				if (atr == -1) return;
				atr &= ~(FILE_ATTRIBUTE_READONLY + FILE_ATTRIBUTE_DIRECTORY);
				if ((item->Param & PR_LOCK) == 0)
				{
					atr |= FILE_ATTRIBUTE_READONLY;
				}
				if (!::SetFileAttributes(ProgLib::m_Name, atr)) return;

				if (ProgLib::m_IconName.IsNotEmpty())
				{
					int atr = (int) :: GetFileAttributes(ProgLib::m_IconName);
					if (atr != -1)
					{
						atr &= ~(FILE_ATTRIBUTE_READONLY + FILE_ATTRIBUTE_DIRECTORY);
						if ((item->Param & PR_LOCK) == 0)
						{
							atr |= FILE_ATTRIBUTE_READONLY;
						}
						::SetFileAttributes(ProgLib::m_IconName, atr);
					}
				}

				ProgDirChangeIgnore = 2;
			}

// změna příznaku uzamčení
			item->Param ^= PR_LOCK;

// aktualizace zobrazení prvku ve stromu
			TV_ITEM tvi;
			tvi.mask = TVIF_STATE;
			tvi.hItem = item->HTree;
			tvi.stateMask = TVIS_BOLD;
			tvi.state = (item->Param & (PR_LOCK | PR_LOCK_DEP)) ? TVIS_BOLD : 0;
			::SendMessage(buf->Tree(), TVM_SETITEM, 0, (LPARAM)&tvi);

// úschova UNDO
			Undo.AddProgLock(FocusBuf, index);
			Undo.Stop();

// aktualizace prvků v buferech
			UpdateLock();

// aktualizace přepínačů pro vybranou položku
			ProgAktItem();

// nastavení příznaku modifikace
			SetModi();

			if (FocusBuf == BufClsID)
			{
				Undo.DelAll();
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// přepnutí stavu OFF prvku pod kurzorem

void ProgOnOff()
{

// kontrola, zda je režim editace
	if (!ProgMode) return;

// ukončení preview
	PreviewStop();

// test, zda je okno stromu
	if ((FocusBuf != BufEdiID) ||
		(EditMode == BufEdiID))
	{

// nalezení aktivního prvku
		CBufProg* buf = BufProg + FocusBuf;
		int index = buf->Select();
		if (index >= 0)
		{

// adresa položky
			PROGITEM* item = &((*buf)[index]);

// prvek nesmí být uzamknutý
			if (item->Param & (PR_LOCK | PR_LOCK_DEP)) return;

// přepnutí stavu pro okno tříd
			if ((FocusBuf == BufClsID) &&
				(item->Parent >= 0))
			{
				ProgLib::InitName(index);

				int atr = (int) :: GetFileAttributes(ProgLib::m_Name);
				if (atr == -1) return;
				atr &= ~(FILE_ATTRIBUTE_HIDDEN + FILE_ATTRIBUTE_DIRECTORY);
				if ((item->Param & PR_OFF) == 0)
				{
					atr |= FILE_ATTRIBUTE_HIDDEN;
				}
				if (!::SetFileAttributes(ProgLib::m_Name, atr)) return;

				if (ProgLib::m_IconName.IsNotEmpty())
				{
					int atr = (int) :: GetFileAttributes(ProgLib::m_IconName);
					if (atr != -1)
					{
						atr &= ~(FILE_ATTRIBUTE_HIDDEN + FILE_ATTRIBUTE_DIRECTORY);
						if ((item->Param & PR_OFF) == 0)
						{
							atr |= FILE_ATTRIBUTE_HIDDEN;
						}
						::SetFileAttributes(ProgLib::m_IconName, atr);
					}
				}

				ProgDirChangeIgnore = 2;
			}

// změna příznaku vypnutí
			item->Param ^= PR_OFF;

// aktualizace zobrazení prvku ve stromu
			TV_ITEM tvi;
			tvi.mask = TVIF_STATE;
			tvi.hItem = item->HTree;
			tvi.stateMask = TVIS_CUT;
			tvi.state = (item->Param & (PR_OFF | PR_OFF_DEP)) ? TVIS_CUT : 0;
			::SendMessage(buf->Tree(), TVM_SETITEM, 0, (LPARAM)&tvi);

// úschova UNDO
			Undo.AddProgOff(FocusBuf, index);
			Undo.Stop();

// aktualizace prvků v buferech
			UpdateLock();

// aktualizace přepínačů pro vybranou položku
			ProgAktItem();

// nastavení příznaku modifikace
			SetModi();

			if (FocusBuf == BufClsID)
			{
				Undo.DelAll();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// aktualizace zobrazení jména závislých položek

void AktNameTree(int bufID, CText& txt, int refBlok, int refIndex)
{

// kontrola platnosti závislosti
	switch (refBlok)
	{
	case BufStrID:
		switch (bufID)
		{
		case BufObjID:
		case BufLocID:
		case BufClsID:
		case BufEdiID:
			break;
		default:
			return;
		}
		break;

	case BufClsID:
		switch (bufID)
		{
		case BufObjID:
		case BufLocID:
			break;
		default:
			return;
		}
		break;

	case BufLocID:
	case BufObjID:
		switch (bufID)
		{
		case BufEdiID:
			break;
		default:
			return;
		}
		break;

	default:
		return;
	}

// lokální proměnné
	CBufProg* buf = BufProg + bufID;
	int index = buf->Max() - 1;
	PROGITEM* item = buf->Data() + index;
	TV_ITEM tvi;

// cyklus přes všechny položky
	for (; index >= 0; index--)
	{

// test, zda je položka platná, zda referuje na hledanou položku a zda má implicitní text
		if ((buf->IsValid(index)) &&
			(item->RefIndex == refIndex) &&
			(item->RefBlok == refBlok) &&
			(item->Name < 0))
		{

// oprava textu položky ve stromu
			if (item->HTree)
			{
				tvi.hItem = item->HTree;
				tvi.mask = TVIF_TEXT;
				tvi.pszText = (LPTSTR)(LPCTSTR)txt;
				::SendMessage(buf->Tree(), TVM_SETITEM, 0, (LPARAM)&tvi);
			}

// aktualizace zobrazení v závislých bufferech
			AktNameTree(BufObjID, txt, bufID, index);
			AktNameTree(BufLocID, txt, bufID, index);
			AktNameTree(BufEdiID, txt, bufID, index);
			AktNameTree(BufClsID, txt, bufID, index);
		}

// posun adresy položky
		item--;
	}
}


/////////////////////////////////////////////////////////////////////////////
// přepnutí stavu DEF_NAME prvku pod kurzorem

void ProgOnDefName()
{

// kontrola, zda je režim editace
	if (!ProgMode) return;

// ukončení preview
	PreviewStop();

// test, zda je okno stromu
	if ((FocusBuf != BufEdiID) ||
		(EditMode == BufEdiID))
	{

// nalezení aktivního prvku
		CBufProg* buf = BufProg + FocusBuf;
		int index = buf->Select();
		if (index >= 0)
		{

// adresa prvku
			PROGITEM* item = &((*buf)[index]);

// prvek nesmí být uzamknutý
			if (item->Param & (PR_LOCK | PR_LOCK_DEP)) return;

// nelze přepnout u potomka v okně tříd
			if ((FocusBuf == BufClsID) && (item->Parent >= 0)) return;

// duplikace textu, je-li implicitní
			if (item->Name < 0)
			{
				CMultiText txt;
				txt.MultiText(Text.GetTxt(buf->GetText(index)));
				item->Name = Text.Add(txt);
				Undo.AddProgName(FocusBuf, index, -1);
			}

// jinak zrušení původního textu
			else
			{
				Undo.AddProgName(FocusBuf, index, item->Name);

				Text.Del(item->Name);
				item->Name = -1;
				int oldinx = buf->GetText(index);
				CMultiText txt;
				txt.MultiText(Text.GetTxt(oldinx));

// kontrola, zda je to jméno objektu
				if (((FocusBuf == BufObjID) || (FocusBuf == BufLocID))
					&& (buf->TestObj(item->Func)))
				{

// příprava počáteční a koncové položky k hledání v lokálním bufferu
					int frst = -1;
					int last = -1;
					if (FocusBuf == BufLocID)
					{
						frst = index;
						while (buf->At(frst).Parent >= 0)
						{
							frst = buf->At(frst).Parent;
						}
						last = buf->At(frst).Next;
					}

// kontrola, zda je jméno jedinečné
					if ((BufLoc.FindObj(txt, index, frst, last) >= 0) ||
						(BufObj.FindObj(txt, index, -1, -1) >= 0))
					{

// nalezení jedinečného jména
						int n = 0;
						do
						{
							n++;
							txt.SetNumObj(n);
						} while ((BufLoc.FindObj(txt, index, frst, last) >= 0) ||
								(BufObj.FindObj(txt, index, -1, -1) >= 0));

// nastavení nového jména položky
						item->Name = Text.Add(txt);
					}
				}

// nastavení textu položky ve stromu
				if (item->HTree)
				{
					TV_ITEM tvi;
					tvi.mask = TVIF_TEXT;
					tvi.hItem = item->HTree;
					tvi.pszText = (LPTSTR)(LPCTSTR)txt.MultiText();
					::SendMessage(buf->Tree(), TVM_SETITEM, 0, (LPARAM)&tvi);
				}

// aktualizace textů v závislých bufferech
				if (item->Refer)
				{
					CText txt2(txt.MultiText());
					AktNameTree(BufObjID, txt2, buf->BufID(), index);
					AktNameTree(BufLocID, txt2, buf->BufID(), index);
					AktNameTree(BufEdiID, txt2, buf->BufID(), index);
					AktNameTree(BufClsID, txt2, buf->BufID(), index);
				}
			}

// ukončení záznamu UNDO
			Undo.Stop();

// aktualizace přepínačů pro vybranou položku
			ProgAktItem();

// nastavení příznaku modifikace
			SetModi();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// překreslení okna

void UpdateWnd(HWND wnd)
{
	if (wnd)
	{
		::InvalidateRect(wnd, NULL, FALSE);
		::UpdateWindow(wnd);
	}
}


/////////////////////////////////////////////////////////////////////////////
// překreslení stromů

void UpdateAllTree()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// aktualizace oken
	UpdateWnd(TreeObj);
	UpdateWnd(TreeLoc);
	UpdateWnd(TreeEdi);
	UpdateWnd(TreeStr);
	UpdateWnd(TreeCls);
}

/////////////////////////////////////////////////////////////////////////////
// přepnutí stavu DEF_ICON prvku pod kurzorem

void ProgOnDefIcon()
{

// kontrola, zda je režim editace
	if (!ProgMode) return;

// ukončení preview
	PreviewStop();

// test, zda je okno stromu
	if ((FocusBuf != BufEdiID) ||
		(EditMode == BufEdiID))
	{

// nalezení aktivního prvku
		CBufProg* buf = BufProg + FocusBuf;
		int index = buf->Select();
		if (index >= 0)
		{

// adresa prvku
			PROGITEM* item = &((*buf)[index]);

// prvek nesmí být uzamknutý a nesmí to být okno tříd ani struktur
			if ((item->Param & (PR_LOCK | PR_LOCK_DEP)) || 
					(FocusBuf == BufClsID) || (FocusBuf == BufStrID)) return;

// duplikace ikony, je-li implicitní
			if (item->Icon < 0)
			{
				item->Icon = Icon.Dup(buf->GetIcon(index));
				Undo.AddProgIcon(FocusBuf, index, -1);
			}

// jinak zrušení původní ikony
			else
			{
				if (item->Icon != 0)
				{
					if (!HistEdit::Del(BufIcoID, item->Icon, -1, -1)) return;

					Undo.AddProgIcon(FocusBuf, index, item->Icon);
				
					Icon.Del(item->Icon);
					item->Icon = -1;

					if (item->DatBlok == BufIcoID)
					{
						item->Icon = Icon.Dup(0);
						item->DatIndex = item->Icon;
					}
				}

// aktualizace ikon ve stromech
				UpdateAllTree();
			}

// ukončení záznamu UNDO
			Undo.Stop();

// aktualizace přepínačů pro vybranou položku
			ProgAktItem();

// nastavení příznaku modifikace
			SetModi();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// aktualizace přepínačů pro vybranou položku

void ProgAktItem()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// obsluha stromu
	if ((FocusBuf != BufEdiID) ||
		(EditMode == BufEdiID))
	{

// nalezení aktivního prvku
		CText txt;
		CText txt2;

		CBufProg* buf = BufProg + FocusBuf;
		int index = buf->Select();
		if (index >= 0)
		{
			PROGITEM* item = &((*buf)[index]);

// povolení tlačítek (zákaz při uzamčení)
			BOOL enable = ((item->Param & (PR_LOCK | PR_LOCK_DEP)) == 0);
			EnableCommand(IDN_LOCK, ((item->Param & (PR_LOCK_DEP | PR_CD | PR_CD2)) == 0));
			EnableCommand(IDN_OFF, enable);
			EnableCommand(IDN_DEF_NAME, enable && !EditName && (FocusBuf != BufStrID)
					&& (FocusBuf != BufClsID));
			EnableCommand(IDN_EDIT_NAME, enable && (FocusBuf != BufStrID)
					&& ((FocusBuf != BufClsID) || (item->Parent >= 0)));
			EnableCommand(IDN_DEF_ICON, enable && !EditName && (item->Icon != 0) && 
						(FocusBuf != BufClsID) && (FocusBuf != BufStrID));
			EnableCommand(IDN_EDIT_ICON, enable && !EditName &&	
						(FocusBuf != BufClsID) && (FocusBuf != BufStrID));

// zapnutí tlačítek
			CheckCommand(IDN_LOCK, (item->Param & PR_LOCK));
			CheckCommand(IDN_OFF, (item->Param & PR_OFF));
			CheckCommand(IDN_EDIT_NAME, EditName);
			CheckCommand(IDN_DEF_NAME, (item->Name < 0));
			CheckCommand(IDN_EDIT_ICON, (FocusBuf == BufEdiID) && (EditMode == BufIcoID));
			CheckCommand(IDN_DEF_ICON, (item->Icon < 0));

// příprava stavového textu
			txt.Load(IDN_STAT_REF);
			txt.Add(_T(": "));

			int ref = buf->GetRefer(index);

			txt2.Int(ref);
			txt.Add(txt2);
		}

// tlačítka jsou nedefinovaná - jejich zákaz
		else
		{
			EnableCommand(IDN_LOCK, FALSE);
			CheckCommand(IDN_LOCK, FALSE);

			EnableCommand(IDN_OFF, FALSE);
			CheckCommand(IDN_OFF, FALSE);

			EnableCommand(IDN_DEF_NAME, FALSE);
			CheckCommand(IDN_DEF_NAME, FALSE);

			EnableCommand(IDN_EDIT_NAME, FALSE);
			CheckCommand(IDN_EDIT_NAME, FALSE);

			EnableCommand(IDN_DEF_ICON, FALSE);
			CheckCommand(IDN_DEF_ICON, FALSE);

			EnableCommand(IDN_EDIT_ICON, FALSE);
			CheckCommand(IDN_EDIT_ICON, FALSE);
		}

// nastavení stavového textu
		SetStatusText2(txt);
	}

	ProgUpdateClipboard();
}

/////////////////////////////////////////////////////////////////////////////
// aktualizace příznaků LOCK a OFF jedné položky (state = TVIS_BOLD, TVIS_CUT)

void UpdateLockItem(CBufProg* buf, PROGITEM* item, int state)
{
// lokální proměnné
	int* param;							// adresa parametrů položky
	int oldstate;						// starý stav položky
	int index;							// ukazatel indexu potomků
	TV_ITEM tvi;						// položka stromu
	
// oprava nového stavu OFF podle deklarační položky
	if ((item->RefBlok >= 0) && (item->RefIndex >= 0))
	{
		if (BufProg[item->RefBlok][item->RefIndex].Param & (PR_OFF | PR_OFF_DEP))
		{
			state |= TVIS_CUT;
		}
	}

// příprava starého stavu položky
	oldstate = 0;
	param = &(item->Param);
	if (*param & (PR_LOCK | PR_LOCK_DEP)) oldstate = TVIS_BOLD;
	if (*param & (PR_OFF | PR_OFF_DEP)) oldstate |= TVIS_CUT;

// nastavení závislých příznaků
	*param &= ~(PR_LOCK_DEP | PR_OFF_DEP);
	if (state & TVIS_BOLD) *param |= PR_LOCK_DEP;
	if (state & TVIS_CUT) *param |= PR_OFF_DEP;

// oprava stavu položky
	if (*param & (PR_LOCK | PR_LOCK_DEP)) state |= TVIS_BOLD;
	if (*param & (PR_OFF | PR_OFF_DEP)) state |= TVIS_CUT;

// oprava stavu položky ve stromu
	if ((state != oldstate) && (item->HTree != NULL))
	{
		tvi.mask = TVIF_STATE;
		tvi.hItem = item->HTree;
		tvi.stateMask = TVIS_CUT | TVIS_BOLD;
		tvi.state = state;
		::SendMessage(buf->Tree(), TVM_SETITEM, 0, (LPARAM)&tvi);
	}

// nastavení parametrů datové položky
	if (((DWORD)(item->DatBlok) < (DWORD)PROGBUFNUM) && (item->DatIndex >= 0))
	{
		UpdateLockItem(BufProg + item->DatBlok, &(BufProg[item->DatBlok][item->DatIndex]), state);
	}

// nastavení potomků
	index = item->Child;
	BOOL iscomment = ((item->Func == IDF_COMMENT) && (item->Parent >= 0));
	while (buf->IsValid(index))
	{
		item = &((*buf)[index]);
		if (iscomment && (item->Func != IDF_COMMENT))
		{
			UpdateLockItem(buf, item, state | TVIS_CUT);
		}
		else
		{
			UpdateLockItem(buf, item, state);
		}
		index = item->Next;
	}
}


/////////////////////////////////////////////////////////////////////////////
// aktualizace příznaků LOCK a OFF jednoho bufferu

void UpdateLockBuf(int bufID)
{
// lokální proměnné
	CBufProg* buf = BufProg + bufID;	// buffer
	int index = buf->First();			// index položky
	PROGITEM* item;						// adresa položky

// cyklus přes všechny položky ROOT
	while (buf->IsValid(index))
	{

// adresa položky
		item = &((*buf)[index]);

// aktualizace položky
		UpdateLockItem(buf, item, 0);

// index další položky
		index = item->Next;
	}
}

/////////////////////////////////////////////////////////////////////////////
// aktualizace příznaků LOCK a OFF bufferů (LOCAL a EDIT jsou aktualizovány závisle)

void UpdateLock()
{
	UpdateLockBuf(BufStrID);
	UpdateLockBuf(BufClsID);
	UpdateLockBuf(BufObjID);

// pro správné provázání zopakování návazností
// (jinak nesprávná funkce např. je-li globální proměnná ve skupině
//  a přepíná se vypnutí celé skupiny -> v editoru se aktualizuje špatně)

	UpdateLockBuf(BufStrID);
	UpdateLockBuf(BufClsID);
	UpdateLockBuf(BufObjID);
}


/////////////////////////////////////////////////////////////////////////////
// zahájení editace ikony prvku

void ProgOnEditIcon()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// ukončení preview
	PreviewStop();

// test, zda je okno stromu
	if (((FocusBuf != BufEdiID) ||
		(EditMode == BufEdiID)) &&
		(FocusBuf != BufStrID) &&
		(FocusBuf != BufClsID))
	{
		CBufProg* buf = BufProg + FocusBuf;

		int index = buf->Select();
		if (index >= 0)
		{
			PROGITEM* item = &((*buf)[index]);

			if ((FocusBuf == BufEdiID) && (item->Func == IDF_COLOR))
			{
				ProgCol::SelectColor(index);
				return;
			}

			if ((item->Icon < 0) && 
					((buf->BufID() != BufEdiID) || 
						((item->Func != IDF_ICON) && 
						(item->Func != IDF_NONE))))
			{
				item->Icon = Icon.Dup(buf->GetIcon(index));
				Undo.AddProgIcon(FocusBuf, index, -1);
				ProgAktItem();
				SetModi();
			}

			int icon = buf->GetIcon(index);

			if (Icon.IsValid(icon))
			{
				if (!SetEditMode(BufIcoID, icon)) return;
				BufEdi.Nadpis(Text.GetTxt(buf->GetText(index)));
				FocusBuf = BufEdiID;
				ProgSetFocus();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// zahájení/ukončení editace jména prvku

void ProgOnEditName()
{

// kontrola, zda je režim editace
	if (!ProgMode) return;

// ukončení preview
	PreviewStop();

// test, zda je okno stromu
	if (((FocusBuf != BufEdiID) ||
		(EditMode == BufEdiID)) &&
		(FocusBuf != BufStrID))
	{
		CBufProg* buf = BufProg + FocusBuf;

// ukončení editace
		if (EditName)
		{
			::SendMessage(buf->Tree(), TVM_ENDEDITLABELNOW, FALSE, 0);
		}

// zahájení editace
		else
		{
			int index = buf->Select();
			if (index >= 0)
			{
				PROGITEM* item = &((*buf)[index]);
				if (item->HTree)
				{
					::SendMessage(buf->Tree(), TVM_EDITLABEL, 0, (LPARAM)item->HTree);
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// zahájení editace jména prvku (vrací TRUE=přerušit editaci)

BOOL ProgOnBeginLabelEdit(HWND hWnd, HTREEITEM hItem)
{

// kontrola, zda je režim editace a zda není editace prvku
	if (!ProgMode || EditName) return TRUE;

// ukončení preview
	PreviewStop();

// rozlišení zdrojového okna
	CBufProg* buf = BufProgFromHandle(hWnd);
	if ((buf == NULL) || (buf->Tree() == NULL)) return TRUE;

// nalezení položky
	int index = buf->Find(hItem);
	if (index < 0) return TRUE;

// načtení handle editačního okna
	EditNameWnd = (HWND)::SendMessage(buf->Tree(), TVM_GETEDITCONTROL, 0, 0);
	if (EditNameWnd == NULL) return TRUE;

// příznak zahájení editace
	EditName = TRUE;

// aktualizace přepínačů pro vybranou položku
	ProgAktItem();

// update voleb clipboardu
	ProgUpdateClipboard();
	EnableCommand(IDN_CUT, TRUE);
	EnableCommand(IDN_COPY, TRUE);

// příznak povolení editace
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// ukončení editace jména prvku

// pomocná proměnná k podržení textu pro prvek
CText LabelText;

void ProgOnEndLabelEdit(HWND hWnd, TV_ITEM* tvi)
{

// kontrola, zda je režim editace a zda je editace prvku
	if (!ProgMode || !EditName) return;

// test, zda je text platný
	if (tvi->pszText)
	{

// rozlišení zdrojového okna
		int bufID = BufProgIDFromHandle(hWnd);
		if (bufID >= 0)
		{
			CBufProg* buf = BufProg + bufID;

// nalezení položky
			int index = buf->Find(tvi->hItem);
			if (index >= 0)
			{
				PROGITEM* item = &((*buf)[index]);

// úschova starého jména
				int oldindex = buf->GetText(index);
				CMultiText oldtxt;
				if (buf->At(index).Name >= 0)
				{
					oldtxt = Text.Get(oldindex);
				}
				else
				{
					oldtxt.MultiText(Text.GetTxt(oldindex));
				}
				CMultiText newtxt;
				newtxt = oldtxt;

// příprava nového jména
				CText txt(tvi->pszText);
				if (bufID == BufClsID)
				{
					txt.TrimLeft();
					txt.TrimRight();
				}

// kontrola, zda je text změněn .... vyřazeno, aby bylo aktualizováno vždy jméno souboru v knihovně
//				if (txt != oldtxt.MultiText())
				{

// prázdný text bude nahrazen implicitním textem
					if (txt.IsEmpty())
					{
						oldtxt.MultiText(Jazyk, EmptyText);
						txt = oldtxt.MultiText();
						LabelText = txt;
						tvi->pszText = (LPTSTR)(LPCTSTR)LabelText;
					}

// pro buffer tříd úschova starého jména souboru
					CText oldname;
					CText oldicon;
					CText oldkey;

					if ((bufID == BufClsID) && (item->Parent >= 0))
					{
						ProgLib::InitName(index);
						oldname = ProgLib::m_Name;
						oldicon = ProgLib::m_IconName;
						oldkey = ProgLib::m_NameKey;
					}

// nastavení nového textu
					newtxt.MultiText(txt);

// vytvoření nového textu nebo nastavení existujícího
					ASSERT(FocusBuf == bufID);	// proč je zde najednou použito FocusBuf ??? !!!!!!
					if (item->Name < 0)
					{
						item->Name = Text.Add(newtxt);
						Undo.AddProgName(FocusBuf, index, -1);
					}
					else
					{
						Undo.AddProgNameSet(FocusBuf, index, Text.Get(item->Name));
						Text.Set(item->Name, newtxt);
					}

// v bufferu tříd kontrola, zda jméno již existuje
					if ((bufID == BufClsID) && (item->Parent >= 0))
					{

// příprava nového jména souboru
						CText newname = newtxt.MultiText();
						newname.KorigShort();

						if (item->Func == IDF_GROUP)
						{
							newtxt.MultiText(JAZYK000, newname);
						}
						else
						{
							if (ProgLib::m_AltExt)
							{
								newtxt.MultiText(JAZYK000, newname + _T('.') + ProgLib::m_Ext2);
							}
							else
							{
								newtxt.MultiText(JAZYK000, newname + _T('.') + ProgLib::m_Ext);
							}
						}
						Text.Set(item->Name, newtxt);

						ProgLib::InitName(index);

// alternativní jméno
						CText altname;
						if (ProgLib::m_AltExt)
						{
							altname = ProgLib::m_Name.Left(ProgLib::m_Name.Length() - 3) + ProgLib::m_Ext;
						}
						else
						{
							altname = ProgLib::m_Name.Left(ProgLib::m_Name.Length() - 3) + ProgLib::m_Ext2;
						}

// existuje soubor nebo adresář tohoto jména?
						CText oldname2(oldname);
						oldname2.UpperCase();
						CText newname2;

						int nn = 1;
						while (((int)::GetFileAttributes(ProgLib::m_Name) != -1) ||
								(ProgLib::m_Ext2.IsNotEmpty() && ((int)::GetFileAttributes(altname) != -1)))
						{
// je to stejné jméno - OK
							newname2 = ProgLib::m_Name;
							newname2.UpperCase();
							if (newname2 == oldname2) break;

//							if (ProgLib::m_Name == oldname) break;

// zvýšení čítače indexu jména
							nn++;
							if (nn >= 10000) break;

// sestavení nového jména položky
							CText txt0;

							if (ShortName)
							{
								CText txtint;
								txtint.Int(nn);

								while ((newname.Length() + txtint.Length()) > 8)
								{
									newname.Delete(newname.Length() - 1);
								}
								txt0 = newname + txtint;
							}
							else
							{
								txt0 = newname;
								txt0.SetNumObj(nn);
								newtxt.SetNumObj(nn);
							}

// změna textu pro položku
							if (item->Func == IDF_GROUP)
							{
								newtxt.MultiText(JAZYK000, txt0);
							}
							else
							{
								if (ProgLib::m_AltExt)
								{
									newtxt.MultiText(JAZYK000, txt0 + _T('.') + ProgLib::m_Ext2);
								}
								else
								{
									newtxt.MultiText(JAZYK000, txt0 + _T('.') + ProgLib::m_Ext);
								}
							}
							Text.Set(item->Name, newtxt);

							ProgLib::InitName(index);

							if (ProgLib::m_AltExt)
							{
								altname = ProgLib::m_Name.Left(ProgLib::m_Name.Length() - 3) + ProgLib::m_Ext;
							}
							else
							{
								altname = ProgLib::m_Name.Left(ProgLib::m_Name.Length() - 3) + ProgLib::m_Ext2;
							}
						}

// nastavení nového textu
						LabelText = Text.GetTxt(item->Name);
						tvi->pszText = (LPTSTR)(LPCTSTR)LabelText;

// přejmenování souboru nebo adresáře
						if (::MoveFile(oldname, ProgLib::m_Name) && 
							ProgLib::m_IconName.IsNotEmpty())
						{
							::MoveFile(oldicon, ProgLib::m_IconName);
						}

// nastavení aliases jména
						for (int lang = 0; lang < JAZYKNUM; lang++)
						{
							if (lang != JAZYK000)
							{
								if (oldkey != ProgLib::m_NameKey)
								{
									::WritePrivateProfileString(JazykInfo[lang].LangIDini, oldkey, NULL, ProgLib::m_Aliases);
								}

								if ((newtxt.MultiText(lang)).IsEmpty())
								{
									::WritePrivateProfileString(JazykInfo[lang].LangIDini, ProgLib::m_NameKey, NULL, ProgLib::m_Aliases);
								}
								else
								{
									::WritePrivateProfileString(JazykInfo[lang].LangIDini, ProgLib::m_NameKey, newtxt.MultiText(lang), ProgLib::m_Aliases);
								}
							}
						}
						
						Undo.DelAll();

						ProgDirChangeIgnore = 2;
					}

// kontrola, zda je to jméno objektu
					if (((bufID == BufObjID) || (bufID == BufLocID))
						&& (buf->TestObj(item->Func)))
					{

// odstranění mezer ze začátku a konce jména
						txt.TrimLeft();						// odstranění mezer z počátku jména
						txt.TrimRight();					// odstranění mezer z konce jména
						if (txt.IsEmpty() || (txt.Length() != Text.GetTxt(item->Name).Length()))
						{
							if (txt.IsEmpty()) txt = _T('#');	// náhradní jméno
							Text.SetTxt(item->Name, txt);			// nastavení nového jména
							LabelText = Text.GetTxt(item->Name);
							tvi->pszText = (LPTSTR)(LPCTSTR)LabelText;
						}
						newtxt.MultiText(txt);

// příprava počáteční a koncové položky k hledání v lokálním bufferu
						int frst = -1;
						int last = -1;
						if (bufID == BufLocID)
						{
							frst = index;
							while (buf->At(frst).Parent >= 0)
							{
								frst = buf->At(frst).Parent;
							}
							last = buf->At(frst).Next;
						}

// kontrola, zda je jméno jedinečné
						if ((BufLoc.FindObj(newtxt, index, frst, last) >= 0) ||
							(BufObj.FindObj(newtxt, index, -1, -1) >= 0))
						{

// nalezení jedinečného jména
							int n = 1;
							do {
								n++;
								newtxt.SetNumObj(n);
							} while ((BufLoc.FindObj(newtxt, index, frst, last) >= 0) ||
									(BufObj.FindObj(newtxt, index, -1, -1) >= 0));

// nastavení nového jména položky
							Text.Set(item->Name, newtxt);

// nastavení nového textu
							txt = Text.GetTxt(item->Name);
							LabelText = txt;
							tvi->pszText = (LPTSTR)(LPCTSTR)LabelText;
						}
					}

// při shodě s implicitním jazykem se použije implicitní text
					oldtxt = Text.Get(item->Name);
					if ((Jazyk != JazykDef) && (oldtxt.MultiText(Jazyk) == oldtxt.MultiText(JazykDef)))
					{
						oldtxt.MultiText(EmptyText);
						Text.Set(item->Name, oldtxt);
					}

// nastavení příznaku modifikace
					if (bufID != BufClsID) SetModi();

// aktualizace textů v závislých bufferech
					if (item->Refer)
					{
						AktNameTree(BufObjID, txt, buf->BufID(), index);
						AktNameTree(BufLocID, txt, buf->BufID(), index);
						AktNameTree(BufEdiID, txt, buf->BufID(), index);
						AktNameTree(BufClsID, txt, buf->BufID(), index);
					}
				}
			}
		}
	}

// příznak ukončení editace
	EditName = FALSE;
	EditNameWnd = NULL;

	Undo.Stop();

// aktualizace přepínačů pro vybranou položku
	ProgAktItem();

// update voleb clipboardu
	ProgUpdateClipboard();
	ProgUpdateInOut();
	ProgUpdateUndoRedo();
}

/////////////////////////////////////////////////////////////////////////////
// obsluha klávesy (vrací TRUE=zrušení klávesy)

BOOL ProgOnKeyDown(HWND hWnd, int key, int data)
{
// lokální proměnné
	CBufProg* buf;					// buffer programu
	int index;						// index položky
	PROGITEM* item;					// adresa položky

// kontrola, zda je režim editace
	if (!ProgMode) return FALSE;

// obsluha okna stromu
	buf = BufProgFromHandle(hWnd);
	if (buf)
	{

// přepínání oken TAB
		if (key == VK_TAB)
		{
			if ((::GetKeyState(VK_MENU) >= 0) && (::GetKeyState(VK_CONTROL) >= 0))
			{
				if (::GetKeyState(VK_SHIFT) < 0)
				{
					switch (FocusBuf)
					{
					case BufObjID:	FocusBuf = BufLocID; break;
					case BufEdiID:	FocusBuf = BufObjID; break;
					case BufStrID:	FocusBuf = BufEdiID; break;
					case BufClsID:	FocusBuf = BufStrID; break;
					case BufLocID:	FocusBuf = BufClsID; break;
					}
				}
				else
				{
					switch (FocusBuf)
					{
					case BufObjID:	FocusBuf = BufEdiID; break;
					case BufEdiID:	FocusBuf = BufStrID; break;
					case BufStrID:	FocusBuf = BufClsID; break;
					case BufClsID:	FocusBuf = BufLocID; break;
					case BufLocID:	FocusBuf = BufObjID; break;
					}
				}
				ProgSetFocus();
				return TRUE;
			}
		}

// editační pole
		if (FocusBuf == BufEdiID)
		{
			switch(EditMode)
			{
			case BufLogID:
				if (key == VK_RETURN) 
				{	EditLog::Switch();
					return TRUE;
				}
				break;

			case BufMapID:
				if (EditMap::OnKeyDown(key)) return TRUE;
				break;

			case BufIcoID:
			case BufPicID:
				if (EditIcon::OnKeyDown(key)) return TRUE;
				break;
			}

			if ((key == VK_RETURN) || (key == VK_ESCAPE))
			{
				switch(EditMode)
				{
				case BufNumID:
					EditNum::ReLoad();
					break;
				}
			}
		}

		switch (key)
		{
		case VK_RETURN:

// nalezení aktivního prvku
			index = buf->Select();
			if (index >= 0)
			{
				item = &((*buf)[index]);

// zahájení editace jména prvku
				if (!EditName)
				{
					if ((::GetKeyState(VK_MENU) < 0) &&
						((item->Param &  (PR_LOCK | PR_LOCK_DEP)) == 0) &&
						(buf->BufID() != BufStrID) &&
						((buf->BufID() != BufClsID) || (item->Parent >= 0)))
					{
						::PostMessage(buf->Tree(), TVM_EDITLABEL, 0, (LPARAM)item->HTree);
					}
				}
			}
			break;
		}
	}

// klávesy při editaci jména prvku (pozor na opakované vnoření)
	if (EditName)
	{
		buf = BufProg + FocusBuf;

		switch (key)
		{
		case VK_UP:
		case VK_DOWN:
		case VK_PRIOR:
		case VK_NEXT:
			{
				::SendMessage(buf->Tree(), TVM_ENDEDITLABELNOW, FALSE, 0);
				ASSERT(!EditName);
				EditName = FALSE;
				::SendMessage(buf->Tree(), WM_KEYDOWN, key, data);
				index = buf->Select();
				if (index >= 0)
				{
					item = &((*buf)[index]);
					::PostMessage(buf->Tree(), TVM_EDITLABEL, 0, (LPARAM)item->HTree);
				}
			}
			return TRUE;
		}
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// vstup znaku z klávesnice (TRUE=obslouženo)

BOOL ProgOnChar(HWND hWnd, TCHAR znak)
{
// kontrola, zda je režim editace
	if (!ProgMode) return FALSE;

// rozlišení podle editačního módu
	if (FocusBuf == BufEdiID)
	{
		switch (EditMode)
		{
		case BufNumID:
			return EditNum::OnChar(znak);

		case BufIcoID:
		case BufPicID:
			return EditIcon::OnChar(znak);
		}
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// aktualizace definičních ukazatelů

//void AktAllDef()
//{
//
//}

/////////////////////////////////////////////////////////////////////////////
// zobrazení položky (např. po hledání)

void DispItem(int bufID, int index)
{
// adresa bufferu, kontrola zadání
	if (((DWORD)bufID >= (DWORD)PROGBUFNUM) || (bufID == BufIntID)) return;
	CBufProg* buf = BufProg + bufID;
	if (buf->IsNotValid(index)) return;

// pro EDIT a LOCAL se bude nastavovat první položka
	if ((bufID == BufEdiID) || (bufID == BufLocID))
	{

// nalezení ROOT položky
		int first = index;
		while (buf->At(first).Parent >= 0)
		{
			first = buf->At(first).Parent;
		}

// buffer EDIT
		if (bufID == BufEdiID)
		{
			first = BufLoc.SrcDat(BufEdiID, first);
			ASSERT(first >= 0);
			if (first < 0) return;

			if (BufLoc[first].Parent >= 0)
			{
				SetEditMode(BufEdiID, BufLoc[first].DatIndex);
			}
			else
			{
				SetEditMode(BufLocID, first);
			}
		}
		else

// buffer LOCAL
		{
			SetEditMode(BufLocID, first);
		}
	}

// přepnutí na okno
	::SetFocus(buf->Tree());

// přepnutí na aktivní položku
	buf->Select(index);
}


/////////////////////////////////////////////////////////////////////////////
// aktualizace položky s požadovanou referencí

BOOL AktRef(int bufID, int RefBlok, int RefIndex, BOOL next)
{
// nalezení položky v bufferu
	int index = BufProg[bufID].SrcRef(RefBlok, RefIndex, next);

// zobrazení nalezené položky
	if (index >= 0)
	{
		DispItem(bufID, index);
		return TRUE;
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// skok na použití položky v požadovaném směru

void ProgOnRefNextPrev(BOOL next)
{
// kontrola, zda je režim editace
	if (!ProgMode) return;
	
// aktuální buffer
	int bufID = FocusBuf;
	CBufProg* buf = BufProg + bufID;

// aktuální položka
	int index = buf->Select();
	if (index < 0) return;
	PROGITEM* item = &((*buf)[index]);

// nalezení položky
	switch (bufID)
	{
	case BufEdiID:
		AktRef(BufEdiID, item->RefBlok, item->RefIndex, next);
		break;

	case BufClsID:
		if (!AktRef(BufLocID, bufID, index, next))
		{
			AktRef(BufObjID, bufID, index, next);
		}
		break;

	default:
		AktRef(BufEdiID, bufID, index, next);
	}
}


/////////////////////////////////////////////////////////////////////////////
// následující deklarace

void ProgOnRefNext()
{
	ProgOnRefNextPrev(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// předešlá deklarace

void ProgOnRefPrev()
{
	ProgOnRefNextPrev(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// definice deklarace

void ProgOnRefDef()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;
	
// aktuální buffer
	int bufID = FocusBuf;
	CBufProg* buf = BufProg + bufID;

// aktuální položka
	int index = buf->Select();
	if (index < 0) return;
	PROGITEM* item = &((*buf)[index]);

// skok na deklarační položku
	DispItem(item->RefBlok, item->RefIndex);
}


/////////////////////////////////////////////////////////////////////////////
// obsluha dialogu hledání textu

BOOL CALLBACK FindDialogProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_INITDIALOG:
		{
			CenterWindow(hWnd, MainFrame);
			CText txt;

			txt.Load(IDN_FIND_DIALOG);
			txt.SetWindowTextFont(hWnd);

			DialogCustomText(hWnd, IDN_FINDTEXT);

			txt.Load(IDN_CANCEL0);
			txt.SetDialogTextFont(hWnd, IDCANCEL);

			FindStringEdit = ::GetDlgItem(hWnd, IDN_FIND_EDIT);
			FindString.SetWindowTextFont(FindStringEdit);
		}
		return TRUE;

	case WM_DRAWITEM:				// 0x02B
		if (DialogDraw(hWnd, lParam)) return TRUE;
		return FALSE;

	case WM_COMMAND:
		switch (HIWORD(wParam))
		{
		case 0:
		case 1:
			switch (LOWORD(wParam))
			{
			case IDOK:
			case IDCANCEL:
				FindString.GetWindowText(FindStringEdit);
				::EndDialog(hWnd, LOWORD(wParam));
				return TRUE;
			}
			break;
		}
		break;

	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// hledání textu

void ProgOnFind()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// vymazání textů
//	SetStatusText(EmptyText);
	SetStatusText2(EmptyText);

// zadání textu k hledání
	if (::DialogBoxParam(
				hInstance,
				MAKEINTRESOURCE(IDN_FIND_DIALOG),
				MainFrame,
				(DLGPROC)FindDialogProc,
				(LPARAM)IDN_FIND_DIALOG) != IDOK) return;

// nalezení dalšího výskytu textu
	ProgOnFindNext();
}

void ProgOnFindNextPrev(BOOL next)
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// příprava textu k hledání
	CText txt(FindString);
	txt.UpperCase();
	if (txt.IsEmpty()) return;
	CText name;
	int nameN;

// příprava aktivního bufferu
	int bufID = FocusBuf;
	CBufProg* buf = BufProg + bufID;

// příprava počáteční položky
	int index = buf->Select();
	PROGITEM* item;

// cyklus přes všechny buffery
	for (int i = 5+1; i > 0; i--)
	{

// úschova první položky
		int first = buf->First();
		if (first >= 0)
		{

// cyklus přes všechny položky
			for (;;)
			{

// index další položky
				if (next)
				{
					if (index < 0)
					{
						index = buf->NextItem(index);
					}
					else
					{
						index = buf->NextItem(index);
						if (index == first) break;
					}
				}
				else
				{
					if (index == first) break;
					index = buf->PrevItem(index);
				}

// adresa položky
				item = &((*buf)[index]);

// nekontroluje se ROOT položka LOCAL a EDIT
				if (((bufID != BufEdiID) && (bufID != BufLocID)) ||
					(item->Parent >= 0))
				{

// test textu v položce
					nameN = buf->GetText(index);
					if (nameN >= 0)
					{
						name = Text.GetTxt(nameN);
						name.UpperCase();
						if (name.Find(txt) >= 0)
						{
							DispItem(bufID, index);
							return;
						}
					}
				}
			}
		}

// index dalšího bufferu
		if (next)
		{
			switch (bufID)
			{
			case BufEdiID: bufID = BufStrID; break;
			case BufStrID: bufID = BufClsID; break;
			case BufClsID: bufID = BufLocID; break;
			case BufLocID: bufID = BufObjID; break;
			case BufObjID: bufID = BufEdiID; break;
			}
		}
		else
		{
			switch (bufID)
			{
			case BufEdiID: bufID = BufObjID; break;
			case BufObjID: bufID = BufLocID; break;
			case BufLocID: bufID = BufClsID; break;
			case BufClsID: bufID = BufStrID; break;
			case BufStrID: bufID = BufEdiID; break;
			}
		}
		buf = BufProg + bufID;
		index = -1;
	}
}

void ProgOnFindNext()
{
	ProgOnFindNextPrev(TRUE);
}

void ProgOnFindPrev()
{
	ProgOnFindNextPrev(FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// horizontální posuvník editačního pole

void ProgOnHScroll(int code, int pos)
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// načtení pozice pro jiné kódy než tažení
	if ((code != SB_THUMBPOSITION) && (code != SB_THUMBTRACK))
	{
		pos = ::GetScrollPos(HScroll, SB_CTL);
	}

// rozlišení podle editačního módu
	switch (EditMode)
	{
	case BufMapID:
		EditMap::OnHScroll(code, pos);
		break;

	case BufSprID:
		EditSprite::OnHScroll(code, pos);
		break;

	case BufIcoID:
	case BufPicID:
		EditIcon::OnHScroll(code, pos);
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// vertikální posuvník editačního pole

void ProgOnVScroll(int code, int pos)
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// načtení pozice pro jiné kódy než tažení
	if ((code != SB_THUMBPOSITION) && (code != SB_THUMBTRACK))
	{
		pos = ::GetScrollPos(VScroll, SB_CTL);
	}

// rozlišení podle editačního módu
	switch (EditMode)
	{
	case BufMapID:
		EditMap::OnVScroll(code, pos);
		break;

	case BufSprID:
		EditSprite::OnVScroll(code, pos);
		break;

	case BufIcoID:
	case BufPicID:
		EditIcon::OnVScroll(code, pos);
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// výběr všeho

void ProgOnAll()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// editace jména prvku
	if (EditName)
	{
		::SendMessage(EditNameWnd, EM_SETSEL, 0, (LPARAM)-1);
	}

// rozlišení podle editačního módu
	if (FocusBuf == BufEdiID)
	{
		switch (EditMode)
		{
		case BufNumID:
			EditNum::SelectAll();
			break;

		case BufMapID:
			EditMap::SelectAll();
			break;

		case BufTxtID:
			EditText::SelectAll();
			break;

		case BufIcoID:
		case BufPicID:
			EditIcon::SelectAll();
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// zrušení bloku

void ProgOnDelete()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// editace jména prvku
	if (EditName)
	{
		long int start, end;
		::SendMessage(EditNameWnd, EM_GETSEL, (WPARAM)&start, (WPARAM)&end);

		if (start == end)
		{
			::SendMessage(EditNameWnd, WM_KEYDOWN,	VK_DELETE, 0x01530001);
			::SendMessage(EditNameWnd, WM_KEYUP,	VK_DELETE, 0xC1530001);
		}
		else
		{
			::SendMessage(EditNameWnd, WM_CLEAR, 0, 0);
		}
		return;
	}

// rozlišení podle editačního módu
	if (FocusBuf == BufEdiID)
	{
		switch (EditMode)
		{
		case BufEdiID:
			ProgClip::Delete(BufEdiID);
			break;

		case BufNumID:
			EditNum::Delete();
			break;

		case BufMapID:
			EditMap::Delete();
			break;

		case BufTxtID:
			EditText::Delete();
			break;

		case BufIcoID:
		case BufPicID:
			EditIcon::Delete();
			break;
		}
	}
	else
	{
		ProgClip::Delete(FocusBuf);
	}
}

/////////////////////////////////////////////////////////////////////////////
// kopie bloku

void ProgOnCopy()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// editace jména prvku
	if (EditName)
	{
		::SendMessage(EditNameWnd, WM_COPY, 0, 0);
		return;
	}

// rozlišení podle editačního módu
	if (FocusBuf == BufEdiID)
	{
		switch (EditMode)
		{
		case BufEdiID:
			ProgClip::Copy(BufEdiID);
			break;

		case BufNumID:
			EditNum::Copy();
			break;

		case BufMapID:
			EditMap::Copy();
			break;

		case BufSndID:
			EditSound::Copy();
			break;

		case BufMusID:
			EditMusic::Copy();
			break;

		case BufTxtID:
			EditText::Copy();
			break;

		case BufIcoID:
		case BufPicID:
			EditIcon::Copy();
			break;
		}
	}
	else
	{
		ProgClip::Copy(FocusBuf);
	}
}

/////////////////////////////////////////////////////////////////////////////
// vystřižení bloku

void ProgOnCut()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// editace jména prvku
	if (EditName)
	{
		::SendMessage(EditNameWnd, WM_CUT, 0, 0);
		return;
	}

// rozlišení podle editačního módu
	if (FocusBuf == BufEdiID)
	{
		switch (EditMode)
		{
		case BufEdiID:
			ProgClip::Cut(BufEdiID);
			break;

		case BufNumID:
			EditNum::Cut();
			break;

		case BufMapID:
			EditMap::Cut();
			break;

		case BufTxtID:
			EditText::Cut();
			break;

		case BufIcoID:
		case BufPicID:
			EditIcon::Cut();
			break;
		}
	}
	else
	{
		ProgClip::Cut(FocusBuf);
	}
}

/////////////////////////////////////////////////////////////////////////////
// vložení bloku

void ProgOnPaste()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// editace jména prvku
	if (EditName)
	{
		::SendMessage(EditNameWnd, WM_PASTE, 0, 0);
		return;
	}

// rozlišení podle editačního módu
	if (FocusBuf == BufEdiID)
	{
		switch (EditMode)
		{
		case BufEdiID:
			ProgClip::Paste(BufEdiID);
			break;

		case BufNumID:
			EditNum::Paste();
			break;

		case BufMapID:
			EditMap::Paste();
			break;

		case BufSndID:
			EditSound::Paste();
			break;

		case BufMusID:
			EditMusic::Paste();
			break;

		case BufTxtID:
			EditText::Paste();
			break;

		case BufIcoID:
		case BufPicID:
			EditIcon::Paste();
			break;
		}
	}
	else
	{
		ProgClip::Paste(FocusBuf);
	}
}

/////////////////////////////////////////////////////////////////////////////
// UNDO

void ProgOnUndo()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// editace jména prvku
	if (EditName)
	{
		::SendMessage(EditNameWnd, EM_UNDO, 0, 0);
		return;
	}

// editace čísla plochy
	if (EditMapNumWnd)
	{
		::SendMessage(EditMapNumWnd, EM_UNDO, 0, 0);
		return;
	}

	Undo.Undo();
}

/////////////////////////////////////////////////////////////////////////////
// REDO

void ProgOnRedo()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// editace jména prvku
	if (EditName)
	{
		::SendMessage(EditNameWnd, EM_UNDO, 0, 0);
		return;
	}

// editace čísla plochy
	if (EditMapNumWnd)
	{
		::SendMessage(EditMapNumWnd, EM_UNDO, 0, 0);
		return;
	}

	Undo.Redo();
}

/////////////////////////////////////////////////////////////////////////////
// nulování příznaků načtení obrázků pro preview

void PreviewPicNul()
{
 	for (int k = PreviewPicSize - 1; k >= 0; k--)
	{
		PreviewPicLoad[k] = FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// test změny adresáře

void TestDirChange(HANDLE* dir1, HANDLE* dir2, CText& path1, CText& path2, int idf)
{
	HANDLE olddir1 = *dir1;
	HANDLE olddir2 = *dir2;

// zahájení sledování
//	BeginDirChange(dir1, dir2, path1, path2);

// běžný adresář
	if (*dir1 != INVALID_HANDLE_VALUE)
	{
		while ((olddir1 == INVALID_HANDLE_VALUE) || (::WaitForSingleObject(*dir1, 0) == WAIT_OBJECT_0))
		{
			if (ProgDirChangeIgnore <= 0)
			{
				int i = BufCls.First();
				while (BufCls.IsValid(i))
				{
					if (BufCls[i].Func == idf)
					{
						while (BufCls.IsValid(BufCls[i].Child))
						{
							BufCls.Del(BufCls[i].Child);
						}

						BufCls[i].Param &= ~PR_EXP;

						if (BufCls[i].HTree != NULL)
						{
							TV_ITEM tvi;
							tvi.hItem = BufCls[i].HTree;
							tvi.mask = TVIF_CHILDREN | TVIF_STATE;
							tvi.cChildren = 1;
							tvi.stateMask = TVIS_EXPANDED;
							tvi.state = 0;
							::SendMessage(BufCls.Tree(), TVM_SETITEM, 0, (LPARAM)&tvi);
						}

						if (idf == IDF_PIC)
						{
							PreviewPicNul();
						}

						Undo.DelAll();
						break;
					}
					i = BufCls[i].Next;
				}
			}

			if (olddir1 != INVALID_HANDLE_VALUE)
			{
				::FindNextChangeNotification(*dir1);
			}
			olddir1 = *dir1;
		}
	}

// alternativní adresář
	if (*dir2 != INVALID_HANDLE_VALUE)
	{
		while ((olddir2 == INVALID_HANDLE_VALUE) || (::WaitForSingleObject(*dir2, 0) == WAIT_OBJECT_0))
		{
			if (ProgDirChangeIgnore <= 0)
			{
				int i = BufCls.First();
				while (BufCls.IsValid(i))
				{
					if (BufCls[i].Func == idf)
					{
						i = BufCls[i].Child;
						while (BufCls.IsValid(i))
						{
							if ((BufCls[i].Func == IDF_GROUP) && (BufCls[i].Param & (PR_CD | PR_CD2)))
							{
								while (BufCls.IsValid(BufCls[i].Child))
								{
									BufCls.Del(BufCls[i].Child);
								}

								BufCls[i].Param &= ~PR_EXP;

								if (BufCls[i].HTree != NULL)
								{
									TV_ITEM tvi;
									tvi.hItem = BufCls[i].HTree;
									tvi.mask = TVIF_CHILDREN | TVIF_STATE;
									tvi.cChildren = 1;
									tvi.stateMask = TVIS_EXPANDED;
									tvi.state = 0;
									::SendMessage(BufCls.Tree(), TVM_SETITEM, 0, (LPARAM)&tvi);
								}

								if (idf == IDF_PIC)
								{
									PreviewPicNul();
								}

								Undo.DelAll();
								break;
							}

							i = BufCls[i].Next;
						}
						break;
					}
					i = BufCls[i].Next;
				}
			}

			if (olddir2 != INVALID_HANDLE_VALUE)
			{
				::FindNextChangeNotification(*dir2);
			}
			olddir2 = *dir2;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// obsluha časovače (TRUE=obslouženo)

BOOL ProgOnTimer(UINT timerID)
{
// kontrola, zda je režim editace
	if (!ProgMode) return FALSE;

// nastavení automatického fokusu
	if (timerID == FocusTimerID)
	{
		if (FocusTimerN > 0)
		{
			if (LMouseDown || RMouseDown)
			{
				if (FocusTimerN < 2) FocusTimerN = 2;
			}
			else
			{
				FocusTimerN--;
				if (FocusTimerN == 0)
				{
					if (FocusEditModeBuf >= 0)
					{
						SetEditMode(FocusEditModeBuf, FocusEditModeInx);
						FocusEditModeBuf = -1;
					}

					FocusBuf = FocusNextBuf;
					ProgSetFocus();
				}
			}
			return TRUE;
		}
	}

// hlášení o změně adresáře
	if (timerID == ProgDirChangeTimerID)
	{
		TestDirChange(&FunctionDirChange, &FunctionDirChange2, FunctionPath, FunctionPath2, IDF_FNC);
		TestDirChange(&NumberDirChange, &NumberDirChange2, NumberPath, NumberPath2, IDF_REAL);
		TestDirChange(&TextDirChange, &TextDirChange2, TextPath, TextPath2, IDF_TEXT);
		TestDirChange(&BoolDirChange, &BoolDirChange2, BoolPath, BoolPath2, IDF_BOOL);
		TestDirChange(&IconDirChange, &IconDirChange2, IconPath, IconPath2, IDF_ICON);
		TestDirChange(&MapDirChange, &MapDirChange2, MapPath, MapPath2, IDF_MAP);
		TestDirChange(&PictureDirChange, &PictureDirChange2, PicturePath, PicturePath2, IDF_PIC);
		TestDirChange(&SpriteDirChange, &SpriteDirChange2, SpritePath, SpritePath2, IDF_SPRITE);
		TestDirChange(&SoundDirChange, &SoundDirChange2, SoundPath, SoundPath2, IDF_SND);
		TestDirChange(&MusicDirChange, &MusicDirChange2, MusicPath, MusicPath2, IDF_MUS);

		if (ProgDirChangeIgnore > 0)
		{
			ProgDirChangeIgnore--;
		}
		return TRUE;
	}

// zahájení preview
	if (timerID == PreviewTimerID)
	{
		PreviewTimerN--;
		if (PreviewTimerN == 0)
		{
			PreviewStart();
		}
	}

// rozlišení podle editačního módu
	switch (EditMode)
	{
//	case BufIcoID:
//	case BufPicID:
//		return EditIcon::OnTimer(timerID);
	case BufSndID:
		return EditSound::OnTimer(timerID);

	case BufMusID:
		return EditMusic::OnTimer(timerID);
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// nastavení fokusu na okno editoru s opožděním

void SetFocusEdit()
{
	FocusNextBuf = BufEdiID;

	FocusEditModeBuf = -1;

	FocusTimerN = 2;
	if (FocusTimer == 0)
	{
		FocusTimer = ::SetTimer(MainFrame, FocusTimerID, TimerConst, NULL);
	}
}

void SetFocusEditMode(int buf, int inx)
{
	FocusNextBuf = BufEdiID;

	FocusEditModeBuf = buf;
	FocusEditModeInx = inx;

	FocusTimerN = 3;
	if (FocusTimer == 0)
	{
		FocusTimer = ::SetTimer(MainFrame, FocusTimerID, TimerConst, NULL);
	}
}

/////////////////////////////////////////////////////////////////////////////
// zobrazení souřadnic grafického editoru

void DispMouseXY()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

	if ((FocusBuf != BufEdiID) || 
		((EditMode != BufIcoID) && 
		(EditMode != BufPicID) && 
		(EditMode != BufMapID) &&
		(EditMode != BufSprID)))
		return;

	CText txt;
	CText txt2;
	int i;

	if ((MouseX != MOUSEINV) && (MouseY != MOUSEINV))
	{
		if ((EditMode == BufPicID) && !EditIcon::NewDimPoint)
		{
			txt = _T(" X=");
			txt2.Double((double)Round((double)MouseX/32*100 + 0.5)/100);
			txt2 += _T(',');
			for (i = Round(1.75*(9-txt2.Length())); i > 0; i--) txt2 += _T(' ');
			txt.Add(txt2);
			txt.Add(_T("Y="));
			txt2.Double((double)Round((double)MouseY/32*100 + 0.5)/100);
			txt.Add(txt2);
		}
		else
		{
			txt = _T(" X=");
			txt2.Int(MouseX);
			txt2 += _T(',');
			for (i = Round(1.7*(9-txt2.Length())); i > 0; i--) txt2 += _T(' ');
			txt.Add(txt2);
			txt.Add(_T("Y="));
			txt2.Int(MouseY);
			txt.Add(txt2);
		}
	}

	SetStatusText2(txt);
}


/////////////////////////////////////////////////////////////////////////////
// vykreslení prvku (vrací TRUE=obslouženo)

BOOL ProgOnDrawItem(DRAWITEMSTRUCT* di)
{
// kontrola, zda je režim editace
	if (!ProgMode) return FALSE;

// příprava lokálních proměnných
	HWND wnd = di->hwndItem;
	HDC dc = di->hDC;
	RECT* rc = &di->rcItem;
	int index = di->itemID;
	int* tab = NULL;
	DWORD state = di->itemState;

// nalezení tabulky comboboxu
	if ((wnd == ToolEditIcon) && ((DWORD)index < (DWORD)30))
	{
		tab = ToolBarEditPicTools;
	}

	if ((wnd == ToolEditIconWidth) && ((DWORD)index < (DWORD)30))
	{
		tab = ToolBarEditPicWidths;
	}

	if ((wnd == ToolEditMap) && ((DWORD)index < (DWORD)30))
	{
		tab = ToolBarEditMapTools;
	}

// obsluha COMBO boxu
	if (di->CtlType == ODT_COMBOBOX)
	{

// vymazání podkladu
		::FillRect(dc, rc, (HBRUSH)::GetStockObject(LTGRAY_BRUSH));

// kontrola, zda je to platný prvek
		if (tab)
		{

// vytvoření kompatibilního DC
			HDC dc2 = ::CreateCompatibleDC(dc);

// výběr bitmapy toolbaru
			::SelectObject(dc2, ToolBarBmp);

// vykreslení ikony
			::BitBlt(dc, (rc->left+rc->right)/2-8, 
				(rc->top+rc->bottom)/2-8, 16, 16, dc2, 16*tab[index*2], 0, SRCCOPY);

// zrušení DC
			::DeleteDC(dc2);

// vykreslení rámečku kolem vybraného prvku
			if ((state & ODS_SELECTED) && ((state & ODS_COMBOBOXEDIT) == 0))
			{
				::DrawEdge(dc, rc, EDGE_RAISED, BF_RECT);

// zobrazení nápovědy k vybranému prvku
				CText txt;
				txt.Load(di->itemData);
				if (txt.IsNotEmpty())
				{
					txt.Delete(txt.Find(_T('\n')));
					SetStatusText(txt);
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// nastavení rozměrů obrázku/plochy

void ProgOnDimen()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// rozlišení podle editačního módu
	switch (EditMode)
	{
	case BufMapID:
		EditMap::OnDimen();
		break;

	case BufSprID:
		EditSprite::OnDimen();
		break;

	case BufIcoID:
	case BufPicID:
		EditIcon::OnDimen();
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// zapnutí/vypnutí rastru

void ProgOnRastr()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// rozlišení podle editačního módu
	switch (EditMode)
	{
	case BufMapID:
		EditMap::OnRastr();
		break;

	case BufIcoID:
	case BufPicID:
		EditIcon::OnRastr();
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// start přehrávání

void ProgOnPlay()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// ukončení preview
	PreviewStop();

// rozlišení podle editačního módu
	switch (EditMode)
	{
	case BufSndID:
		EditSound::OnPlay();
		break;

	case BufMusID:
		EditMusic::OnPlay();
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// zastavení nahrávání i přehrávání

void ProgOnStop()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// rozlišení podle editačního módu
	switch (EditMode)
	{
	case BufSndID:
		EditSound::OnStop();
		break;

	case BufMusID:
		EditMusic::OnStop();
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// přepnutí příznaku opakování

void ProgOnLoop()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// rozlišení podle editačního módu
	switch (EditMode)
	{
	case BufSndID:
		EditSound::OnLoop();
		break;

	case BufMusID:
		EditMusic::OnLoop();
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// start nahrávání

void ProgOnRecord()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// ukončení preview
	PreviewStop();

// rozlišení podle editačního módu
	switch (EditMode)
	{
	case BufSndID:
		EditSound::OnRecord();
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// pozastavení nahrávání, přehrávání

void ProgOnPause()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// ukončení preview
	PreviewStop();

// rozlišení podle editačního módu
	switch (EditMode)
	{
	case BufSndID:
		EditSound::OnPause();
		break;

	case BufMusID:
		EditMusic::OnPause();
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// převíjení zpět

void ProgOnRew()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// rozlišení podle editačního módu
	switch (EditMode)
	{
	case BufMusID:
		EditMusic::OnRew();
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// převíjení vpřed

void ProgOnFrw()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// rozlišení podle editačního módu
	switch (EditMode)
	{
	case BufMusID:
		EditMusic::OnFrw();
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// start programu

void ProgOnStart()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// ukončení preview
	PreviewStop();

// uložení programu
	if (Modi) ProgFile::SaveMaxi();
	if (Modi) return;

// nastavení kódu jazyku do registrů Windows
	SetJazykReg(LangID);

// spuštění programu
	Exec(CText(_T('"')) + JmenoLoad + _T('"'), ProgramPath + Cesta, FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// test indexu, zda to je obrázek v bufferu knihovny (vrací TRUE=je ok, v knihovně je jméno)

BOOL PreviewPicTest(int index)
{
// test, zda to je platný index bufferu
	if (BufCls.IsValid(index))
	{

// adresa položky
		PROGITEM* item = &BufCls[index];

// test, zda to není ROOT položka a zda to je soubor
		if ((item->Parent >= 0) && (item->Func != IDF_GROUP))
		{

// příprava jména souboru
			ProgLib::InitName(index);

// test, zda to je obrázek
			if ((ProgLib::m_Ext == _T("BMP")) || 
				(ProgLib::m_Ext == _T("JPG")))
			{
				return TRUE;
			}
		}
	}

// není to platný obrázek
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// aktualizace cachování obrázku (vrací TRUE=pod kurzorem je obrázek)

BOOL PreviewPicCache()
{
// test, zda je aktivní okno tříd a zda není editace jména položky
	if ((FocusBuf == BufClsID) && !EditName)
	{

// index aktivní položky
		int index = BufCls.Select();

// test indexu, zda to je obrázek
		if (PreviewPicTest(index))
		{

// úschova aktuálního stavu položek cachování, vynulování položek
			CPicture	pic[PreviewPicSize];			// obrázky pro cachování
			CText		name[PreviewPicSize];			// jména obrázků pro cachování (prázdný = není)
			BOOL		load[PreviewPicSize];			// příznaky načítání obrázků (TRUE = byl načítán)

			for (int i = PreviewPicSize - 1; i >= 0; i--)
			{
				pic[i] = PreviewPicPic[i];
				name[i] = PreviewPicName[i];
				load[i] = PreviewPicLoad[i];

				PreviewPicPic[i].Empty();
				PreviewPicName[i].Empty();
				PreviewPicLoad[i] = FALSE;
			}

// indexy položek k načtení
			int inx[PreviewPicSize];
			inx[PreviewPicAkt] = index;
			inx[PreviewPicNxt] = BufCls.NextItem(index);
			inx[PreviewPicPre] = BufCls.PrevItem(index);

// načtení položek
			for (i = PreviewPicSize - 1; i >= 0; i--)
			{
				if (PreviewPicTest(inx[i]))
				{
					PreviewPicName[i] = ProgLib::m_Name;

					for (int j = PreviewPicSize - 1; j >= 0; j--)
					{
						if (name[j] == ProgLib::m_Name)
						{
							if (i == PreviewPicAkt)
							{
								if (j == PreviewPicNxt)	PreviewPicVpred = TRUE;
								if (j == PreviewPicPre)	PreviewPicVpred = FALSE;
							}

							PreviewPicPic[i] = pic[j];
							PreviewPicLoad[i] = load[j];
							break;
						}
					}
				}
			}

			return TRUE;
		}
	}

// pod kurzorem není obrázek
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// zapnutí režimu preview obrázku

void PreviewPicOn()
{
// test, zda je režim preview obrázku již zapnut
	if (PreviewPic) return;

// příznak zapnutí režimu preview obrázku
	PreviewPic = TRUE;

// vypnutí posuvníků
	if (HScrollDisp) ::ShowWindow(HScroll, SW_HIDE);
	if (VScrollDisp) ::ShowWindow(VScroll, SW_HIDE);

// vypnutí editací
	EditSound::OnStop();
	EditMusic::OnStop();
	EditMap::EndEditSwcNum();
	HideEditWnd(-1);

// překreslení okna
	ProgOnSize();
}

/////////////////////////////////////////////////////////////////////////////
// vypnutí režimu preview obrázku

void PreviewPicOff()
{
// test, zda je režimu preview obrázku již vypnut
	if (!PreviewPic) return;

// příznak vypnutí režimu preview obrázku
	PreviewPic = FALSE;

// zapnutí posuvníků
	if (HScrollDisp) ::ShowWindow(HScroll, SW_SHOW);
	if (VScrollDisp) ::ShowWindow(VScroll, SW_SHOW);

// zapnutí editací
	switch (EditMode)
	{
	case BufEdiID:
		::ShowWindow(TreeEdi, SW_SHOW);
		break;

	case BufNumID:
		::ShowWindow(EditNumWnd, SW_SHOW);
		break;

	case BufLogID:
		::ShowWindow(EditLogWnd, SW_SHOW);
		break;

	case BufTxtID:
		::ShowWindow(EditTextWnd, SW_SHOW);
		break;

	case BufSndID:
		::ShowWindow(EditSound::DispWnd, SW_SHOW);
		break;

	case BufMusID:
		::ShowWindow(EditMusic::DispWnd, SW_SHOW);
		break;
	}

// překreslení okna
	ProgOnSize();
}


/////////////////////////////////////////////////////////////////////////////
// zastavení preview, zahájení nového čítání času

void PreviewStop()
{
// ukončení probíhajícího preview
	if (PreviewWav)
	{
		::PlaySound(NULL, NULL, 0);
		PreviewWav = FALSE;
	}

	if (IsPreviewTempName)
	{
		::DeleteFile(PreviewTempName);
		IsPreviewTempName = FALSE;
	}

	if (PreviewMid)
	{
		::mciSendCommand(PreviewMid, MCI_STOP, 0, NULL);
		::mciSendCommand(PreviewMid, MCI_CLOSE, 0, NULL);
		PreviewMid = 0;
	}

// test, zda je nové preview povoleno
	if ((FocusBuf == BufClsID) && !EditName)
	{

// vytvoření časovače
		if (PreviewTimer == 0)
		{
			PreviewTimer = ::SetTimer(MainFrame, PreviewTimerID, TimerConst, NULL);
		}
		PreviewTimerN = 3;

// zobrazení preview obrázku (je-li již načten)
		if (PreviewPicCache())
		{
			if (PreviewPicLoad[PreviewPicAkt])
			{
				if (PreviewPic)
				{
					ProgOnSize();
				}
				else
				{
					PreviewPicOn();
				}
				PreviewTimerN = 1;
			}
		}
	}

// není okno tříd - ukončení časovače
	else
	{
		PreviewPicOff();

		if (PreviewTimer)
		{
			::KillTimer(MainFrame, PreviewTimer);
			PreviewTimer = 0;
		}
		PreviewTimerN = -1;
	}
}

// tabulka testovaných jmen pro opakování (malá písmena!)
LPCTSTR TestRepeatTab[] = 
{
	NULL,
	NULL,
	_T("(opakování)"),
	_T("(opakovaně)"),
	_T("(repeat)"),
	_T("(repeate)"),
	_T("(repeated)"),
	_T("(repeating)"),
	_T("(loop)"),
	NULL
};

// test příznaku opakování ve jméně zvuku
BOOL TestRepeat(CText name)
{
	name.LowerCase();

	CText txt1;
	txt1.Load(IDN_OPAKOVANI);
	txt1 = _T('(') + txt1 + _T(')');
	TestRepeatTab[0] = txt1;

	CText txt2;
	txt2.Load(IDN_OPAKOVANE);
	txt2 = _T('(') + txt2 + _T(')');
	TestRepeatTab[1] = txt2;

	CText txt;

	LPCTSTR* tab = TestRepeatTab;
	while (*tab != NULL)
	{
		txt = *tab;
		if (name.Find(txt) >= 0) return true;
		tab++;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// zahájení preview přehrávání souboru pod kurzorem

void PreviewStart()
{
// nulování čítače proti opětovnému vyvolání
	PreviewTimerN = -1;

// zastavení probíhajícího přehrávání
	if (PreviewWav)
	{
		::PlaySound(NULL, NULL, 0);
		PreviewWav = FALSE;
	}

	if (IsPreviewTempName)
	{
		::DeleteFile(PreviewTempName);
		IsPreviewTempName = FALSE;
	}

	if (PreviewMid)
	{
		::mciSendCommand(PreviewMid, MCI_STOP, 0, NULL);
		::mciSendCommand(PreviewMid, MCI_CLOSE, 0, NULL);
		PreviewMid = 0;
	}

// test, zda je aktivní okno tříd a zda není editace jména položky
	if ((FocusBuf == BufClsID) && !EditName)
	{

// zjištění aktivní položky
		int index = BufCls.Select();
		if (index >= 0)
		{

// adresa položky
			PROGITEM* item = &BufCls[index];

// test, zda to není ROOT položka a zda to je soubor
			if ((item->Parent >= 0) && (item->Func != IDF_GROUP))
			{

// příprava jména souboru
				ProgLib::InitName(index);
				CText text = Text.GetTxt(item->Name);

// přehrávání souboru WAV
				if ((ProgLib::m_Ext == _T("WAV")) || (ProgLib::m_Ext == _T("MP3")))
				{
					CText name = ProgLib::m_Name;

					if (ProgLib::m_AltExt)
					{
						CSound snd;
						snd.LoadFile(name);

						TCHAR tmppath[101];
						tmppath[0] = 0;
						::GetTempPath(100, tmppath);
						PreviewTempName[0] = 0;
						::GetTempFileName(tmppath, _T("PET"), 0, PreviewTempName);
						name = PreviewTempName;
						snd.SaveFile(name);
						IsPreviewTempName = TRUE;
					}

					PreviewWav = ::PlaySound(name, NULL,
						SND_ASYNC | SND_FILENAME | SND_NODEFAULT | SND_NOSTOP | SND_NOWAIT
						| (TestRepeat(text) ? SND_LOOP : 0));

					return;
				}

// přehrávání souboru MID
				if (ProgLib::m_Ext == _T("MID"))
				{

// otevření výstupního zařízení MID
					MCI_OPEN_PARMS mop;

					mop.lpstrDeviceType = _T("sequencer");
					mop.lpstrElementName = ProgLib::m_Name;

					MCIERROR res = ::mciSendCommand(NULL, MCI_OPEN,
					    MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD) &mop);

					if (res == 0)
					{
						PreviewMid = mop.wDeviceID;

// přehrání souboru
						MCI_PLAY_PARMS mpp;
						res = ::mciSendCommand(PreviewMid, MCI_PLAY, 0, (DWORD) &mpp);

// při chybě uzavření zařízení
						if (res)
						{
							::mciSendCommand(PreviewMid, MCI_STOP, 0, NULL);
						    ::mciSendCommand(PreviewMid, MCI_CLOSE, 0, NULL);
							PreviewMid = 0;
						}
					}
					return;
				}

// preview obrázku
				if ((ProgLib::m_Ext == _T("BMP")) || (ProgLib::m_Ext == _T("JPG")))
				{
					PreviewPicCache();

					if (!PreviewPicLoad[PreviewPicAkt])
					{
						PreviewPicPic[PreviewPicAkt].LoadFile(PreviewPicName[PreviewPicAkt], FALSE);
						PreviewPicLoad[PreviewPicAkt] = TRUE;
						PreviewTimerN = 1;
					}
					else
					{
						int inx = (PreviewPicVpred) ? PreviewPicNxt : PreviewPicPre;

						if (!PreviewPicLoad[inx])
						{
							PreviewPicPic[inx].LoadFile(PreviewPicName[inx], FALSE);
							PreviewPicLoad[inx] = TRUE;
							PreviewTimerN = 1;
						}
						else
						{
							inx ^= (PreviewPicNxt ^ PreviewPicPre);

							if (!PreviewPicLoad[inx])
							{
								PreviewPicPic[inx].LoadFile(PreviewPicName[inx], FALSE);
								PreviewPicLoad[inx] = TRUE;
							}
						}
					}

					if (PreviewPic)
					{
						ProgOnSize();
					}
					else
					{
						PreviewPicOn();
					}

					return;
				}
			}
		}
	}

// pro neplatnou položku vypnutí režimu preview obrázku
	PreviewPicOff();
}
