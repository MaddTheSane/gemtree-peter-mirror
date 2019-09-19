
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

//////////////////////////////////////////////////////////////////////////////
// obsluha debuggeru

#ifdef _DEBUG
#ifdef _M_IX86
#define debugbreak _asm{int 3}
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
#include <shlobj.h>
#include <objbase.h>
#include "resource.h"

#define DOUBLE_INFINITY_VALUE       ((ULONGLONG)0x7ff0000000000000)

#pragma warning ( default: 4201)		// hlášení - nepojmenovaná struktura

#pragma warning ( disable: 4100)		// hlášení - nepoužitý formální parametr
#pragma warning ( disable: 4710)		// hlášení - funkce není inline

//////////////////////////////////////////////////////////////////////////////
// předdefinice tříd

class CText;


//////////////////////////////////////////////////////////////////////////////
// globální proměnné

extern	int			VerzeOS;			// verze systému
extern	HINSTANCE	hInstance;			// instance programu
extern	int			ScreenWidth;		// šířka klientské oblasti displeje
extern	int			ScreenHeight;		// výška klientské oblasti displeje

extern	HWND		MainFrame;			// hlavní okno aplikace

extern	int			Waiting;			// příznak zobrazení kurzoru čekání

#ifdef _UNICODE
typedef	BOOL (WINAPI *GETDISKFREESPACEEX) (LPCWSTR, __int64*, __int64*, __int64*);
#else
typedef	BOOL (WINAPI *GETDISKFREESPACEEX) (LPCSTR, __int64*, __int64*, __int64*);
#endif
extern GETDISKFREESPACEEX	pGetDiskFreeSpaceEx; // funkce GetDiskFreeSpaceEx (NULL=není)
extern	__int64	DiskSize;			// velikost disku (z funkce GetDiskSpace)
extern	__int64	DiskFree;			// volné místo disku (z funkce GetDiskSpace)
extern	__int64	DiskFreeUser;		// volné místo uživatele (z funkce GetDiskSpace)

//////////////////////////////////////////////////////////////////////////////
// struktura instalačních dat

#define GROUPSNUM 2							// počet skupin

// definice jednoho souboru v seznamu (9 B + text)
typedef struct INSTFILE_ {
	long			Size;					// (4) velikost souboru v bajtech (po dekompresi)
	long			Check;					// (4) kontrolní součet dat souboru (výchozí 0, přičten bajt, rotace vlevo s přenosem)
	BYTE			NameN;					// (1) délka jména souboru včetně podcesty - ve znacích
	char			Name[1];				// (n) jméno souboru (včetně podcesty)
} INSTFILE;

// definice jedné skupiny (16 B)
typedef struct INSTGROUP_ {
	long			Files;					// (4) počet souborů ve skupině
	long			Size;					// (4) velikost skupiny v KB (po nainstalování) - soubory zaokrouhleny na alokační bloky 8 KB
	long			SizeFiles;				// (4) velikost seznamu souborů (bajtů)
	long			SizeGroup;				// (4) velikost komprimovaných dat (bajtů)
} INSTGROUP;

// záhlaví instalačních dat (16 B + skupiny)
typedef struct INSTHEAD_ {
	char			Ident[4];				// (4) identifikace (text "SET" + bínárně počet sekcí)
	long			Check;					// (4) kontrolní součet zbytku záhlaví včetně seznamu souborů
	FILETIME		DateTime;				// (8) datum a čas souborů
	INSTGROUP		Groups[GROUPSNUM];		// definice skupin
} INSTHEAD;

//////////////////////////////////////////////////////////////////////////////
// pomocné konstanty

#define ICONWIDTH 32								// šířka ikon
#define ICONHEIGHT 32								// výška ikon
#define ICONSIZE (ICONWIDTH*ICONHEIGHT)				// velikost ikon v bajtech

//////////////////////////////////////////////////////////////////////////////
// stránky instalátoru

extern	int			AktPage;		// aktivní stránka instalátoru

enum PAGES {
	PAGESELECT,						// stránka výběru prvků k instalaci
	PAGEINSTAL,						// stránka probíhající instalace
	PAGEOK,							// instalace ukončena OK
};

//////////////////////////////////////////////////////////////////////////////
// ukončení aplikace

void	Exit();						// ukončení programu

/////////////////////////////////////////////////////////////////////////////
// obsluha zprávy před rozesláním do oken (TRUE = zpráva zpracována)

BOOL PreTranslateMessage(MSG* msg);

/////////////////////////////////////////////////////////////////////////////
// nastavení stránky instalátoru

void SetPage(int page);

/////////////////////////////////////////////////////////////////////////////
// načtení registru

CText GetReg(CText key, CText name);

/////////////////////////////////////////////////////////////////////////////
// nastavení registru

void SetReg(CText key, CText name, CText data);

/////////////////////////////////////////////////////////////////////////////
// zrušení klíče

void DelReg(CText key, CText name);

/////////////////////////////////////////////////////////////////////////////
// vytvoření hlavního okna aplikace

bool MainFrameCreate();

/////////////////////////////////////////////////////////////////////////////
// zobrazení kurzoru čekání (zahájení a ukončení musí být do páru!)

void BeginWaitCursor();


/////////////////////////////////////////////////////////////////////////////
// načtení informací o souborech (vrací FALSE=přerušit)

BOOL OpenSetup();

/////////////////////////////////////////////////////////////////////////////
// vypnutí kurzoru čekání (zahájení a ukončení musí být do páru!)

void EndWaitCursor();

//////////////////////////////////////////////////////////////////////////////
// vlastní vnořené sekce

#include "Memory.h"						// obsluha paměti
#include "BufText.h"					// buffer textů, textové řetězce
#include "File.h"						// soubory, buffery a resource
#include "Compress.h"					// komprese
