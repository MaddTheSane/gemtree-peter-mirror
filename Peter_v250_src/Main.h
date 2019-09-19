
//////////////////////////////////////////////////////////////////////////////
// přepínače preprocesoru:
//	_DEBUG ............. je debugger
//	_OPTIM ............. je optimalizace
//	_MT ................ vícetokový režim
//	_UNICODE ........... kódování znaků UNICODE
//
//	_M_IX86 ............ procesor Intel 86
//	_M_ALPHA ........... procesor DEC Alpha
//	_M_MPPC ............ procesor Power Macintosh
//	_M_MRX000 .......... procesor MIPS
//	_M_PPC ............. procesor PowerPC

//#define _MT

// Pro překladač nastavit úroveň varování 4 (přepínač /W4)
// pro inline funkce nepoužívat "bool" ale raději "BOOL" - lépe optimalizováno


#define VerzeMaj		2			// verze - hlavní číslice (jednotky)
#define VerzeMin		5			// verze - vedlejší číslice (desetiny)
#define VerzeRel		0			// verze - číslice vydání (setiny)
#define VerzeCom		0			// verze - číslice kompilace (tisíciny)
#define VerzeTxt		_T("2.50")	// verze - text
#define VerzeTxt0		_T("250")	// verze - text zkrácený (pro clipboard)

//////////////////////////////////////////////////////////////////////////////
// obsluha debuggeru

#ifdef _DEBUG
#ifdef _M_IX86
#ifdef _OPTIM
#define debugbreak debugBreak(__FILE__, __LINE__, __DATE__)
void debugBreak(char* file, int line, char* date);
#else
#define debugbreak _asm{int 3}
#endif	// _OPTIM
#else	// _M_IX86
#define debugbreak DebugBreak()
#endif	// _M_IX86
#endif	// _DEBUG

#if defined(_DEBUG)
#define ASSERT(f)			{ if (!(f)) debugbreak; }
#define VERIFY(f)			ASSERT(f)
#define ASSERTERROR			debugbreak
#else	// _DEBUG
#define ASSERT(f)			((void)0)
#define VERIFY(f)			((void)(f))
#define ASSERTERROR			((void)0)
#endif	// _DEBUG

#ifdef _UNICODE
#ifndef UNICODE
#define UNICODE
#endif	// UNICODE
#endif	// _UNICODE

//////////////////////////////////////////////////////////////////////////////
// standardní vnořené sekce

#pragma warning ( disable: 4201)		// hlášení - nepojmenovaná struktura

#include <windows.h>					// základní definice WINDOWS
#include <math.h>						// matematické operace
//#include <alphaops.h>					// matematické konstanty
#include <tchar.h>						// obsluha znaků UNICODE/MB
#include <commctrl.h>					// doplňkové ovládací prvky
#include <richedit.h>					// RichEdit
#include <mmreg.h>						// multimediální definice
#include "resource.h"

#pragma warning ( default: 4201)		// hlášení - nepojmenovaná struktura

#pragma warning ( disable: 4100)		// hlášení - nepoužitý formální parametr
#pragma warning ( disable: 4710)		// hlášení - funkce není inline

#ifndef SPI_GETLISTBOXSMOOTHSCROLLING
#define SPI_GETLISTBOXSMOOTHSCROLLING 0x1006
#define SPI_SETLISTBOXSMOOTHSCROLLING 0x1007
#endif

#define DOUBLE_INFINITY_VALUE       ((ULONGLONG)0x7ff0000000000000)

//////////////////////////////////////////////////////////////////////////////
// předdefinice tříd

class CIcon; class CMap; class CMusic; class CPicture; class CSound; class CSprite;
class CText; class CFont; class CMultiText;
class CBufBool; class CBufIcon; class CBufInt; class CBufMap; class CBufMusic;
class CBufPic; class CBufReal; class CBufSound; class CBufSprite; class CBufText;
class CBufProg; class CBufIndex; class CBufUndo;
class CFileMap; class CBuf;

//////////////////////////////////////////////////////////////////////////////
// globální proměnné

extern	TCHAR*		CommandLine;		// příkazový řádek
extern	int			VerzeOS;			// verze systému
extern	HINSTANCE	hInstance;			// instance programu
extern	int			ScreenWidth;		// šířka klientské oblasti displeje
extern	int			ScreenHeight;		// výška klientské oblasti displeje
extern	int			TimerConst;			// konstanta pro časovač (pro 55 ms)

extern	HACCEL		Accel;				// akcelerační tabulka

extern	BYTE*		StdPalImport;		// tabulka importu palet
extern	BITMAPINFO* StdBitmapInfo;		// standardní záhlaví BMP
extern	HPALETTE	StdPalette;			// vlastní logické palety
extern	BYTE*		KonvPal;			// konverzní tabulka palet
extern	bool		KonvPalOK;			// příznak shodné konverzní tabulky (lze provést kopii)

extern	bool		Dither;				// používat dithering

extern	bool		ShortName;			// zkracovat jména souborů

//extern	bool		Profi;				// profesionální verze programu

#ifdef _UNICODE
typedef	BOOL (WINAPI *GETDISKFREESPACEEX) (LPCWSTR, __int64*, __int64*, __int64*);
#else
typedef	BOOL (WINAPI *GETDISKFREESPACEEX) (LPCSTR, __int64*, __int64*, __int64*);
#endif
extern GETDISKFREESPACEEX	pGetDiskFreeSpaceEx; // funkce GetDiskFreeSpaceEx (NULL=není)
extern	__int64	DiskSize;			// velikost disku (z funkce GetDiskSpace)
extern	__int64	DiskFree;			// volné místo disku (z funkce GetDiskSpace)
extern	__int64	DiskFreeUser;		// volné místo uživatele (z funkce GetDiskSpace)

extern	CMultiText	Jmeno;				// jméno editovaného souboru (bez cesty)
extern	CText		JmenoLoad;			// plné jméno souboru pro čtení (s cestou - vzorový adresář)
extern	CText		JmenoSave;			// plné jméno souboru pro zápis
extern	CText		Cesta;				// cesta k editovanému souboru (s příp. "\" na konci)
extern	CText		AktDir;				// aktivní adresář uživatele (s "\" na konci)
extern	CText		HomeDir;			// cesta do domovského adresáře s EXE (s "\" na konci)
extern	CText		ExeFileName;		// jméno programu
extern	CText		IniFileName;		// jméno konfiguračního souboru
//extern	CText		HelpFileName;		// jméno souboru nápovědy
extern	CText		TextExe;			// text ".exe"
extern	CText		TextExe2;			// text ".EXE"
extern	CText		CDRom;				// cesta k CD-ROM (prázdný = není)

extern	HWND		PrevWindow;			// předešlé aktivní okno

extern	CText		MemErrNadpis;		// nadpis okna chybového hlášení nedostatku paměti
extern	LPCTSTR		MemErrNadpisP;
extern	CText		MemErrText;			// text okna chybového hlášení nedostatku paměti
extern	LPCTSTR		MemErrTextP;

//////////////////////////////////////////////////////////////////////////////
// cesty do adresářů programu (všechny jsou zakončeny s "\")

extern	CText		ProgramPath;		// cesta do adresáře programů "Program" (*.exe)

extern	CText		BackupPath;			// cesta do adresáře záloh "Backup"

extern	CText		FunctionPath;		// cesta do adresáře funkcí "Function" (*.fnc)
extern	CText		NumberPath;			// cesta do adresáře čísel "Number" (*.num)
extern	CText		TextPath;			// cesta do adresáře textů "Text" (*.txt)
extern	CText		BoolPath;			// cesta do adresáře logických hodnot "Bool" (*.log)
extern	CText		IconPath;			// cesta do adresáře ikon "Icon" (*.ico)
extern	CText		MapPath;			// cesta do adresáře ploch "Map" (*.map)
extern	CText		PicturePath;		// cesta do adresáře obrázků "Picture" (*.bmp)
extern	CText		SpritePath;			// cesta do adresáře sprajtů "Sprite" (*.spr)
extern	CText		SoundPath;			// cesta do adresáře zvuků "Sound" (*.wav)
extern	CText		MusicPath;			// cesta do adresáře hudby "Music" (*.mid)

// alternativní knihovny - implicitně v podadresáři programu Petr
extern	CText		ProgramPath2;		// cesta do adresáře programů "Program" (*.exe)

extern	CText		FunctionPath2;		// cesta do adresáře funkcí "Function" (*.fnc)
extern	CText		NumberPath2;		// cesta do adresáře čísel "Number" (*.num)
extern	CText		TextPath2;			// cesta do adresáře textů "Text" (*.txt)
extern	CText		BoolPath2;			// cesta do adresáře logických hodnot "Bool" (*.log)
extern	CText		IconPath2;			// cesta do adresáře ikon "Icon" (*.ico)
extern	CText		MapPath2;			// cesta do adresáře ploch "Map" (*.map)
extern	CText		PicturePath2;		// cesta do adresáře obrázků "Picture" (*.bmp)
extern	CText		SpritePath2;		// cesta do adresáře sprajtů "Sprite" (*.spr)
extern	CText		SoundPath2;			// cesta do adresáře zvuků "Sound" (*.wav)
extern	CText		MusicPath2;			// cesta do adresáře hudby "Music" (*.mid)

// alternativní knihovny - implicitně v ROOT CD-ROM
extern	CText		ProgramPath3;		// cesta do adresáře programů "Program" (*.exe)

extern	CText		FunctionPath3;		// cesta do adresáře funkcí "Function" (*.fnc)
extern	CText		NumberPath3;		// cesta do adresáře čísel "Number" (*.num)
extern	CText		TextPath3;			// cesta do adresáře textů "Text" (*.txt)
extern	CText		BoolPath3;			// cesta do adresáře logických hodnot "Bool" (*.log)
extern	CText		IconPath3;			// cesta do adresáře ikon "Icon" (*.ico)
extern	CText		MapPath3;			// cesta do adresáře ploch "Map" (*.map)
extern	CText		PicturePath3;		// cesta do adresáře obrázků "Picture" (*.bmp)
extern	CText		SpritePath3;		// cesta do adresáře sprajtů "Sprite" (*.spr)
extern	CText		SoundPath3;			// cesta do adresáře zvuků "Sound" (*.wav)
extern	CText		MusicPath3;			// cesta do adresáře hudby "Music" (*.mid)

//////////////////////////////////////////////////////////////////////////////
// jazyk

// textové konstanty
typedef struct TEXTCONST_
{
	short		TextID;					// identifikační kód textu
	const char*	Text;					// text
} TEXTCONST;

extern const int TextConstNum;			// počet textových konstant

#define	JAZYKAUT -1						// automatický jazyk

enum JAZYKID {							// (identifikátor jazyku - hledej LANGID)
		JAZYK000,						// neutrální jazyk (0) - obsahuje jména souborů
		JAZYKARA,						// arabsky (1)
		JAZYKBUL,						// bulharsky (2)
		JAZYKCZ,						// česky (5)
		JAZYKDAN,						// dánsky (6)
		JAZYKGER,						// německy (7)
		JAZYKREC,						// řecky (8)
		JAZYKENG,						// anglicky (9)
		JAZYKSPA,						// španělsky (10)
		JAZYKFIN,						// finsky (11)
		JAZYKFRA,						// francouzsky (12)
		JAZYKHEB,						// hebrejsky (13)
		JAZYKMAD,						// maďarsky (14)
		JAZYKISL,						// islandsky (15)
		JAZYKITA,						// italsky (16)
		JAZYKHOL,						// holandsky (19)
		JAZYKNOR,						// norsky (20)
		JAZYKPOL,						// polsky (21)
		JAZYKPOR,						// portugalsky (22)
		JAZYKRUM,						// rumunsky (24)
		JAZYKRUS,						// rusky (25)
		JAZYKSRB,						// srbochorvatsky (26)
		JAZYKSLO,						// slovensky (27)
		JAZYKALB,						// albánsky (28)
		JAZYKSWE,						// šwédsky (29)
		JAZYKTUR,						// turecky (31)    = 0x1f
		JAZYKVIE,						// vietnamsky (42) = 0x2a

		JAZYKNUM						// počet jazyků
};

extern int	JazykUser;					// uživatelem zvolený jazyk
extern int	Jazyk;						// skutečně nastavený jazyk - musí mít platný rozsah!
extern int	JazykDef;					// implicitní jazyk (podle systému)
extern int	JazykPrev;					// předcházející jazyk

extern	DWORD	CharSet;				// aktuální znaková sada fontů (1=implicitní)
extern	DWORD	CodePage;				// aktuální kódová stránka (0=implicitní)
extern	DWORD	LangID;					// identifikátor jazyku LANGID (0=jiný)
extern	BOOL	MultiJazyk;				// multijazyčnost
extern	char	Carka;					// oddělovač desetinné čárky při zobrazení čísel

// jazykové informace
typedef struct JAZYKINFO_
{
	const TEXTCONST*	TextConst;		// tabulka textů
	int					LangID;			// identifikátor jazyku
	LPCTSTR				LangIDini;		// identifikátor jazyku pro ALIASES.INI
	int					CodePage;		// kódová stránka znaků
	int					CharSet;		// znaková sada fontů
	int					Button;			// kód tlačítka
	LPCTSTR				Name;			// jméno jazyku pro INI
	int					MenuID;			// identifikační kód pro menu
	BOOL				InstOK;			// jazyk je instalován
	LPCTSTR				HelpName;		// jméno souboru nápovědy (2 písmena: CZ, EN, ..)
	BOOL				RightToLeft;		// Right-To-Left, píše se zprava doleva
} JAZYKINFO;

extern const JAZYKINFO JazykInfo[JAZYKNUM]; // tabulka definic jazyků

//////////////////////////////////////////////////////////////////////////////
// nastavení identifikátoru jazyku do registrů Windows (0=implicitní)

void SetJazykReg(int langID);

//////////////////////////////////////////////////////////////////////////////
// změna jazyku

void SetJazyk(int jazyk);

//////////////////////////////////////////////////////////////////////////////
// standardní fonty (pro tažení ikon ve stromu)

extern CBufIcon StdFonts;			// buffer standardních fontů (indexy 0 až 255)
extern CBufIndex StdFontsWidth;		// šířky znaků standardních fontů (indexy 0 až 255)


//////////////////////////////////////////////////////////////////////////////
// zásobník objektů

extern	HBRUSH		HalfToneBrush;		// půltónový štětec

//////////////////////////////////////////////////////////////////////////////
// konfigurace

extern int Scale;					// měřítko pro relativní rozměry okna

extern int MainLeft;				// relativní pozice hlavního okna vlevo
extern int MainTop;					// relativní pozice hlavního okna nahoře
extern int MainWidth;				// relativní šířka hlavního okna
extern int MainHeight;				// relativní výška hlavního okna

extern int LeftWidth;				// relativní šířka levého pole (objekty)
extern int LeftHeight;				// relativní výška levého horního okna (globální objekty)
extern int RightWidth;				// relativní šířka pravého pole (třídy)
extern int RightHeight;				// relativní výška pravého horního okna (struktury)

extern BOOL Maximized;				// příznak maximalizace okna

extern CText FindString;			// hledaný text

// velikost ikony
typedef enum _SMALLICON {
	SI_SMALL = 0,					// malá, 16x16
	SI_BIG = 1,						// velká, 32x32
	SI_MEDIUM = 2,					// střední, 24x24
} SMALLICON;

extern SMALLICON BufZoom[];			// příznaky zoom oken (TRUE=zvětšeno)

//////////////////////////////////////////////////////////////////////////////
// globální konstanty

#define BackCol 0					// průhledná barva (barva pozadí)
#define ShadCol 1					// poloprůhledná barva (stín)

#define ResCols 2					// počet rezervovaných barev na počátku barev (=pozadí a stín)

#define BACKCOLOR_RED	149			// červená složka barvy pozadí
#define BACKCOLOR_GREEN	34			// zelená složka barvy pozadí
#define BACKCOLOR_BLUE	140			// modrá složka barvy pozadí

#define SHADCOLOR_RED	97			// červená složka barvy stínu
#define SHADCOLOR_GREEN	30			// zelená složka barvy stínu
#define SHADCOLOR_BLUE	111			// modrá složka barvy stínu

extern const int ColLev;			// počet úrovní standardních palet
extern const int ColCol;			// počet barev standardních palet
extern const int StdColors;			// počet vlastních palet (začínají od 0)
extern const BYTE WhiteCol;			// bílá barva
extern const BYTE BlackCol;			// černá barva
extern const BYTE BlueCol;			// modrá barva
extern const BYTE RedCol;			// červená barva
extern const BYTE YellowCol;		// žlutá barva
extern const BYTE GreenCol;			// zelená barva
extern const BYTE LtYellowCol;		// světle žlutá barva
extern const BYTE OrangeCol;		// oranžová

extern const long double pi;		// Ludolfovo číslo
extern const long double pi2;		// Ludolfovo číslo * 2
extern const long double eul;		// Eulerovo číslo

extern const long double uhel22;	// úhel 22.5 stupňů v radiánech
extern const long double uhel30;	// úhel 30 stupňů v radiánech
extern const long double uhel45;	// úhel 45 stupňů v radiánech
extern const long double uhel60;	// úhel 60 stupňů v radiánech
extern const long double uhel67;	// úhel 67.5 stupňů v radiánech
extern const long double uhel90;	// úhel 90 stupňů v radiánech
extern const long double uhel135;	// úhel 135 stupňů v radiánech
extern const long double uhel180;	// úhel 180 stupňů v radiánech
extern const long double uhel225;	// úhel 215 stupňů v radiánech
extern const long double uhel270;	// úhel 270 stupňů v radiánech
extern const long double uhel315;	// úhel 315 stupňů v radiánech

extern const long double ln10;		// přirozený logaritmus 10

extern const long double degrad;	// konstanta pro převod z DEG na RAD (pi/180)
extern const long double raddeg;	// konstanta pro převod z RAD na DEG (180/pi)


//////////////////////////////////////////////////////////////////////////////
// příznaky typu ikony a obrázku
enum PICPARAM {
	PicParamPic,			// pouze obrázek bez pozadí
	PicParamMix,			// obrázek mixovaný s pozadím
	PicParamBack,			// pouze pozadí (prázdný obrázek)
	PicParamNone,			// neznámý obsah
	PicParamComp,			// komprimovaná data
};


//////////////////////////////////////////////////////////////////////////////
// ukončení aplikace

void	Exit(int code);					// ukončení programu

#define	EXITCODE_MEMERRINI	125			// chyba paměti při startu programu
#define	EXITCODE_MEMINIT	120			// chyba paměti při inicializaci
#define EXITCODE_MAINFRAME	115			// chyba vytvoření hlavního okna
#define EXITCODE_LOADRES	110			// chyba čtení resource

#define EXITCODE_INIT		100			// hranice inicializačních kódů

#define	EXITCODE_MEMERR		80			// chyba paměti při běhu programu
#define EXITCODE_OK			0			// řádné ukončení programu


/////////////////////////////////////////////////////////////////////////////
// test licence

BOOL TestLic();


//////////////////////////////////////////////////////////////////////////////
// vystředění okna proti jinému oknu

void CenterWindow(HWND child, HWND parent = NULL);


//////////////////////////////////////////////////////////////////////////////
// spuštění programu

int Exec(CText command, CText aktdir, BOOL wait);


/////////////////////////////////////////////////////////////////////////////
// generování konverzní tabulky barev obrázku

void GenKonvPal(BITMAPINFO* bmp);


/////////////////////////////////////////////////////////////////////////////
// vygenerování střední barvy (ze 4 bodů)

BYTE ColAvrg(BYTE col1, BYTE col2, BYTE col3, BYTE col4);
BYTE ColAvrgDither(BYTE col1, BYTE col2, BYTE col3, BYTE col4, int x, int y);


/////////////////////////////////////////////////////////////////////////////
// zjištění editačního okna s fokusem (NULL = není)

HWND GetEditFocus();


/////////////////////////////////////////////////////////////////////////////
// vytvoření adresáře (pokud existuje, je vše OK)

BOOL CreateDir(CText txt);


/////////////////////////////////////////////////////////////////////////////
// vytvoření adresáře (i vícestupňově - včetně cesty)

BOOL CreatePath(CText txt);


/////////////////////////////////////////////////////////////////////////////
// dekomprese dat

void DeKomp(BYTE* dstBuf, int dstNum, BYTE* srcBuf, int srcNum);


/////////////////////////////////////////////////////////////////////////////
// generátor náhody

int rand();
double Rand();

inline int sqr(int num) { return num*num; }

/////////////////////////////////////////////////////////////////////////////
// zaokrouhlení čísel

// převod reálného čísla na celé číslo se zaokrouhlením k nejbližší hodnotě
int Round(double num);

// převod reálného čísla na celé číslo s oříznutím k nule
int Round0(double num);

// převod reálného čísla na celé číslo se zaokrouhlením nahoru (ke kladné hodnotě)
int RoundP(double num);

// převod reálného čísla na celé číslo se zaokrouhlením dolů (k záporné hodnotě)
int RoundM(double num);


/////////////////////////////////////////////////////////////////////////////
// zjištění informací o velikosti a volném místu disku

void GetDiskSpace(CText path);


/////////////////////////////////////////////////////////////////////////////
// mód editace ikony (* = je v editoru ikony, + = je v editoru obrázku))

enum EditMode
{ 
	EditModePen,					// * + pero
	EditModePaint,					// *   štětec

	EditModeLine,					// * + čára
	EditModeRect,					// * + obdélník
	EditModeRectFill,				//   + vyplněný obdélník
	EditModeRound,					//   + kružnice
	EditModeRoundFill,				// * + kruh
	EditModeElip,					//   + elipsa
	EditModeElipFill,				// * + ovál

	EditModeWidth1,					//   + pero 1
	EditModeWidth2,					//   + pero 2
	EditModeWidth3,					//   + pero 3
	EditModeWidth5,					//   + pero 5
	EditModeWidth9,					//   + pero 9
	EditModeWidth20,				//   + pero 21

	EditModePipe,					// * + kapátko
	EditModeFill,					// * + výplň
	EditModeSpray,					// * + sprej
	EditModeText,					//   + text

	EditModeZoomIn,					//   + měřítko zvětšit
	EditModeZoomOut,				//   + měřítko zmenšit

	EditModeSelect,					// * + blok
	EditModeXFlip,					// * + horizontální převrácení
	EditModeYFlip,					// * + vertikální převrácení
	EditModeRRot,					// * + otočení R
	EditModeLRot,					// * + otočení L
	EditModeXCol,					// * + záměna barev
	EditModeSCol,					// * + nastavení barvy

	EditModeMul,					// * + zvětšení
	EditModeDiv,					// * + zmenšení

	EditMode45,						// * + otočení o 45 stupňů
	EditMode30,						// * + otočení o 30 stupňů
	EditMode22,						// * + otočení o 22.5 stupňů
};


//////////////////////////////////////////////////////////////////////////////
// pomocné konstanty

#define ICONWIDTH 32								// šířka ikon
#define ICONHEIGHT 32								// výška ikon
#define ICONSIZE (ICONWIDTH*ICONHEIGHT)				// velikost ikon v bajtech

#define MOUSEINV -100000							// neplatná souřadnice myši

//////////////////////////////////////////////////////////////////////////////
// vlastní vnořené sekce

#include "Memory.h"						// obsluha paměti
#include "Bitmap.h"						// obsluha bitmap a ikon
#include "Select.h"						// výběr souboru k editaci
#include "Buffer.h"						// šablona pro buffery
#include "BufIcon.h"					// buffer ikon, data ikon
#include "BufInt.h"						// buffer celých čísel a logických proměnných
#include "BufMap.h"						// buffer mapy plochy
#include "BufMus.h"						// buffer hudby
#include "BufPic.h"						// buffer obrázků (pozadí)
#include "JPEG.h"						// obsluha JPEG souborů
#include "BufReal.h"					// buffer reálných čísel
#include "BufSnd.h"						// buffer zvuku
#include "BufSprt.h"					// buffer sprajtů
#include "BufText.h"					// buffer textů, textové řetězce
#include "MainFrm.h"					// hlavní okno aplikace


//////////////////////////////////////////////////////////////////////////////
// fonty

// definice parametrů fontu
class CFont
{
public:
	BOOL	Standard;					// příznak standardního fontu
	BOOL	Bold;						// příznak tučného fontu
	BOOL	Italic;						// příznak kurzívy (pro standardní font se ignoruje)
	BOOL	Underline;					// příznak podtržení (pro standardní font se ignoruje)
	BOOL	Strikeout;					// příznak přeškrtnutí (pro standardní font se ignoruje)
	BOOL	Serif;						// příznak patičkového písma (pro standardní font se ignoruje)
	BOOL	Fixed;						// příznak písma s pevnou roztečí
	int		Height;						// výška fontů, 0=běžná (pro standardní font se ignoruje)
	int		Height0;					// skutečná výška fontů (vypočtená při vytváření fontu)
	double	Width;						// relativní šířka fontů, 0=běžná (pro standardní font se ignoruje)
	int		Width0;						// skutečná šířka fontů (vypočtená při vytváření fontu)
	CText	User;						// uživatelsky definovaný font (SERIF i FIXED nastaveny)

// konstruktor a destruktor
	inline CFont()
	{ 
		Standard = TRUE;				// příznak standardního fontu
		Bold = FALSE;					// příznak tučného fontu
		Italic = FALSE;					// příznak kurzívy
		Underline = FALSE;				// příznak podtržení
		Strikeout = FALSE;				// příznak přeškrtnutí
		Serif = FALSE;					// příznak patiček
		Fixed = FALSE;					// příznak pevných roztečí
		Height = 0;						// výška fontů
		Height0 = 15;					// skutečná výška fontů
		Width = 0;						// relativní šířka fontů
		Width0 = 8;						// skutečná šířka fontů
	};

	inline void Init()
	{ 
		Standard = TRUE;				// příznak standardního fontu
		Bold = FALSE;					// příznak tučného fontu
		Italic = FALSE;					// příznak kurzívy
		Underline = FALSE;				// příznak podtržení
		Strikeout = FALSE;				// příznak přeškrtnutí
		Serif = FALSE;					// příznak patiček
		Fixed = FALSE;					// příznak pevných roztečí
		Height = 0;						// výška fontů
		Height0 = 15;					// skutečná výška fontů
		Width = 0;						// relativní šířka fontů
		Width0 = 8;						// skutečná šířka fontů
		User.Init();					// uživatelský font
	};

	inline void Term() { User.Term(); };

	inline const CFont& operator= (const CFont& src)
	{ 
		Standard	= src.Standard;
		Bold		= src.Bold;
		Italic		= src.Italic;
		Underline	= src.Underline;
		Strikeout	= src.Strikeout;
		Serif		= src.Serif;
		Fixed		= src.Fixed;
		Height		= src.Height;
		Height0		= src.Height0;
		Width		= src.Width;
		Width0		= src.Width0;
		User		= src.User;
		return *this; 
	}

	inline friend BOOL operator==(const CFont& fnt1, const CFont& fnt2)
	{
		return (
			(fnt1.Standard	== fnt2.Standard) &&
			(fnt1.Bold		== fnt2.Bold) &&
			(fnt1.Italic	== fnt2.Italic) &&
			(fnt1.Underline	== fnt2.Underline) &&
			(fnt1.Strikeout	== fnt2.Strikeout) &&
			(fnt1.Serif		== fnt2.Serif) &&
			(fnt1.Fixed		== fnt2.Fixed) &&
			(fnt1.Height	== fnt2.Height) &&
			(fnt1.Width		== fnt2.Width) &&
			(fnt1.User		== fnt2.User));
	};
};

extern HFONT	FontBezny;				// běžný font pro dialogy a stavový řádek
extern HFONT	FontBold;				// tučný font pro dialogy a nadpisy oken
extern HFONT	FontFixed;				// font s pevnou roztečí pro dialogy

extern CText	SetupVzor;				// vzor textu pro nastavení písma

extern CFont	SelectFont;				// font výběru programu
extern CFont	TreeFont;				// font editoru programu
extern CFont	TextFont;				// font editoru textu
extern CFont	MenuFont;				// font nabídky
extern CFont	StatusFont;				// font stavové lišty
extern CFont	GraphicFont;			// font grafického editoru

/////////////////////////////////////////////////////////////////////////////
// vytvoření fontu

HFONT GetFont(CFont* fnt);


/////////////////////////////////////////////////////////////////////////////
// zrušení fontu

void DelFont(HFONT font);


//////////////////////////////////////////////////////////////////////////////
// nastavení fontů v okně

void SetFontBezny(HWND wnd);
void SetFontBold(HWND wnd);
void SetFontFixed(HWND wnd);
void SetFontFixedBold(HWND wnd);


#include "BufProg.h"					// buffer programu
#include "BufInx.h"						// buffer indexů
#include "BufUndo.h"					// buffer undo a redo
#include "EditIcon.h"					// editor ikon a obrázků
#include "EditLog.h"					// editor logických hodnot
#include "EditMap.h"					// editor ploch
#include "EditMus.h"					// editor hudby
#include "EditNum.h"					// editor čísel
#include "EditSnd.h"					// editor zvuků
#include "EditSprt.h"					// editor sprajtů
#include "EditText.h"					// editor textů
#include "File.h"						// obsluha souborů
#include "Compress.h"					// obsluha komprese dat
#include "Prog.h"						// obsluha editace souboru
#include "ProgDrag.h"					// tažení ikon programu
#include "ProgFile.h"					// obsluha souborů
#include "ProgClip.h"					// obsluha clipboardu
#include "ProgExp.h"					// export a import programu
#include "ProgCol.h"					// výběr barvy
#include "ProgHist.h"					// historie editace
#include "ProgLib.h"					// knihovny


//////////////////////////////////////////////////////////////////////////////
// import palet

// konverze barev
inline void KonvCopy(BYTE* dst, BYTE* src, int num)
{
	if (KonvPalOK)
	{
		MemCopy(dst, src, num);
	}
	else
	{
		for (; num > 0; num--)
		{
			dst[0] = KonvPal[src[0]];
			dst++;
			src++;
		}
	}
}

// import palety
inline BYTE PalImport(BYTE red, BYTE green, BYTE blue)
{
	return StdPalImport[(red/4) | (green/4 * 64) | (blue/4 * 64 * 64)];
}

inline BYTE PalImport(int color)
{
	return StdPalImport[
		((color & (63*4))/4) |							// červená složka
		((color & (63*4*256))/(256*4/64)) |				// zelená složka
		((color & (63*4*256*256))/(256*256*4/64/64))];	// modrá složka
}


/////////////////////////////////////////////////////////////////////////////
// načtení registru DWORD (-1 = chyba)

//int GetRegUserDWORD(CText key, CText name);


/////////////////////////////////////////////////////////////////////////////
// nastavení registru DWORD (nevytváří klíč)

//void SetRegUserDWORD(CText key, CText name, int data);

