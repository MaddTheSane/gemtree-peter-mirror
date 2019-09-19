
#define WIN32_LEAN_AND_MEAN
#define INITGUID
#include "Main.h"

//////////////////////////////////////////////////////////////////////////////
// sekce programu Petříka

PETHEAD*	PetAdr = NULL;				// adresa dat programu
int			PetSize = 0;				// velikost dat programu
CBuf		PetBuf;						// buffer dat programu

char DATAsize[] =									// buffer importů od překladače
{
	"DATAsize"										// sem bude uložena adresa a velikost dat, současně je to identifikátor
};

//////////////////////////////////////////////////////////////////////////////
// definice jazyků

int		JazykDef = JAZYKENG;			// implicitní jazyk
DWORD	DefCharSet = DEFAULT_CHARSET;	// implicitní znaková sada fontů podle systému

int		JazykDefImp = 0;				// implicitní importovaný jazyk

int		Jazyk = JAZYKENG;				// aktuální jazyk
DWORD	LangID = 0x09;					// aktuální identifikátor jazyku
DWORD	CharSet = DEFAULT_CHARSET;		// aktuální znaková sada fontů
DWORD	CodePage = CP_ACP;				// aktuální kódová stránka
char	Carka = '.';					// oddělovač desetinné čárky při zobrazení čísel

int		JazykUkaz0 = 0;					// ukazatel indexu načítaného textu
char*	JazykUkaz[JAZYKNUM];			// ukazatele textů při načítání

JAZYKINFO JazykInfo[JAZYKNUM] =
{
// arabsky
	0x01,	1256,	ARABIC_CHARSET,	NULL,
										_T("Cancel"),	_T("All files"),
										_T("Insufficient Memory"),
										_T("Sorry, this program will be terminated due to the insufficient memory!"),
// bulharsky
	0x02,	1251,	RUSSIAN_CHARSET,	NULL,
										_T("Cancel"),	_T("All files"),
										_T("Insufficient Memory"),
										_T("Sorry, this program will be terminated due to the insufficient memory!"),
// česky
	0x05,	1250,	EASTEUROPE_CHARSET,	NULL,
										_T("Storno"),	_T("Všechny soubory"),
										_T("Nedostatek paměti"),
										_T("Lituji, program bude ukončen z důvodu nedostatku paměti!"),
// dánsky
	0x06,	1252,	ANSI_CHARSET,		NULL,
										_T("Cancel"),	_T("All files"),
										_T("Insufficient Memory"),
										_T("Sorry, this program will be terminated due to the insufficient memory!"),
// německy
	0x07,	1252,	ANSI_CHARSET,		NULL,
										_T("Abbrechen"),_T("Alle Dateien"),
										_T("Speicherplatzmangel"),
										_T("Ich bedauere, das Programm wird wegen Speicherplatzmangel beendet!"),
// řecky
	0x08,	1253,	GREEK_CHARSET,		NULL,
										_T("Cancel"),	_T("All files"),
										_T("Insufficient Memory"),
										_T("Sorry, this program will be terminated due to the insufficient memory!"),
// anglicky
	0x09,	1252,	ANSI_CHARSET,		NULL,
										_T("Cancel"),	_T("All files"),
										_T("Insufficient Memory"),
										_T("Sorry, this program will be terminated due to the insufficient memory!"),
// španělsky
	0x0a,	1252,	ANSI_CHARSET,		NULL,
										_T("Cancel"),	_T("All files"),
										_T("Insufficient Memory"),
										_T("Sorry, this program will be terminated due to the insufficient memory!"),
// finsky
	0x0b,	1257,	BALTIC_CHARSET,		NULL,
										_T("Cancel"),	_T("All files"),
										_T("Insufficient Memory"),
										_T("Sorry, this program will be terminated due to the insufficient memory!"),
// francouzsky
	0x0c,	1252,	ANSI_CHARSET,		NULL,
										_T("Cancel"),	_T("All files"),
										_T("Insufficient Memory"),
										_T("Sorry, this program will be terminated due to the insufficient memory!"),
// hebrejsky
	0x0d,	1255,	HEBREW_CHARSET,		NULL,
										_T("Cancel"),	_T("All files"),
										_T("Insufficient Memory"),
										_T("Sorry, this program will be terminated due to the insufficient memory!"),
// maďarsky
	0x0e,	1250,	EASTEUROPE_CHARSET,	NULL,
										_T("Cancel"),	_T("All files"),
										_T("Insufficient Memory"),
										_T("Sorry, this program will be terminated due to the insufficient memory!"),
// islandsky
	0x0f,	1257,	BALTIC_CHARSET,		NULL,
										_T("Cancel"),	_T("All files"),
										_T("Insufficient Memory"),
										_T("Sorry, this program will be terminated due to the insufficient memory!"),
// italsky
	0x10,	1252,	ANSI_CHARSET,		NULL,
										_T("Cancel"),	_T("All files"),
										_T("Insufficient Memory"),
										_T("Sorry, this program will be terminated due to the insufficient memory!"),
// holansky
	0x13,	1252,	ANSI_CHARSET,		NULL,
										_T("Cancel"),	_T("All files"),
										_T("Insufficient Memory"),
										_T("Sorry, this program will be terminated due to the insufficient memory!"),
// norsky
	0x14,	1257,	BALTIC_CHARSET,		NULL,
										_T("Cancel"),	_T("All files"),
										_T("Insufficient Memory"),
										_T("Sorry, this program will be terminated due to the insufficient memory!"),
// polsky
	0x15,	1250,	EASTEUROPE_CHARSET,	NULL,
										_T("Anuluj"),	_T("Wszystkie pliki"),
										_T("Brak pamięci"),
										_T("Żałuję, program będzie zakończeny z powodu braku pamięci!"),
// portugalsky
	0x16,	1252,	ANSI_CHARSET,		NULL,
										_T("Cancel"),	_T("All files"),
										_T("Insufficient Memory"),
										_T("Sorry, this program will be terminated due to the insufficient memory!"),
// rumunsky
	0x18,	1251,	RUSSIAN_CHARSET,	NULL,
										_T("Cancel"),	_T("All files"),
										_T("Insufficient Memory"),
										_T("Sorry, this program will be terminated due to the insufficient memory!"),
// rusky
	0x19,	1251,	RUSSIAN_CHARSET,	NULL,
										_T("Cancel"),	_T("All files"),
										_T("Insufficient Memory"),
										_T("Sorry, this program will be terminated due to the insufficient memory!"),
// srbochorvátsky
	0x1a,	1250,	EASTEUROPE_CHARSET,	NULL,
										_T("Cancel"),	_T("All files"),
										_T("Insufficient Memory"),
										_T("Sorry, this program will be terminated due to the insufficient memory!"),
// slovensky
	0x1b,	1250,	EASTEUROPE_CHARSET,	NULL,
										_T("Storno"),	_T("Všetky súbory"),
										_T("Nedostatok pamäti"),
										_T("Ľutujem, program bude ukončený z dôvodu nedostatku pamäti!"),
// albánsky
	0x1c,	1253,	GREEK_CHARSET,		NULL,
										_T("Cancel"),	_T("All files"),
										_T("Insufficient Memory"),
										_T("Sorry, this program will be terminated due to the insufficient memory!"),
// švédsky
	0x1d,	1257,	BALTIC_CHARSET,		NULL,
										_T("Cancel"),	_T("All files"),
										_T("Insufficient Memory"),
										_T("Sorry, this program will be terminated due to the insufficient memory!"),
// turecky
	0x1f,	1254,	TURKISH_CHARSET,	NULL,
										_T("Cancel"),	_T("All files"),
										_T("Insufficient Memory"),
										_T("Sorry, this program will be terminated due to the insufficient memory!"),
// vietnamsky
	0x2a,	1258,	VIETNAMESE_CHARSET,	NULL,
										_T("Cancel"),	_T("All files"),
										_T("Insufficient Memory"),
										_T("Sorry, this program will be terminated due to the insufficient memory!"),
};

LPCTSTR ProductKey0 = _T("Software\\Gemtree Software\\Peter");

//////////////////////////////////////////////////////////////////////////////
// inicializační segmenty CRT (konstruktory a destruktory)

typedef void (__cdecl *_PVFV)(void);		// ukazatel na funkci void fnc(void)
typedef void (*PFV)(void);					// funkce void fnc(void)

#pragma data_seg(".CRT$XCA")
_PVFV	__xc_a[] = { NULL };				// konstruktory C++
#pragma data_seg(".CRT$XCZ")
_PVFV	__xc_z[] = { NULL };

#pragma data_seg()							// resetování na běžnou datovou sekci

#pragma comment(linker, "/MERGE:.CRT=.data")	// připojení CRT sekcí do datové sekce

//////////////////////////////////////////////////////////////////////////////
// globální proměnné

CString		CommandLine;				// příkazový řádek
int			VerzeOS;					// verze systému
OSVERSIONINFO	OSVersionInfo;			// informace o systému
HINSTANCE	hInstance = NULL;			// instance programu
int			ScreenWidth;				// šířka klientské oblasti displeje (též v režimu DirectDraw)
int			ScreenHeight;				// výška klientské oblasti displeje (též v režimu DirectDraw)
int			ScreenWidth0;				// šířka videomódu displeje
int			ScreenHeight0;				// výška videomódu displeje
int			ScreenBits;					// počet bitů na bod obrazovky
int			TimerConst;					// konstanta pro časovač (pro 55 ms)

HINSTANCE	KernLibrary = NULL;			// handle modulu KERNEL32.DLL

DWORD		ReturnCode = EXITCODE_OK;	// návratový kód (pro operaci OK)

BYTE*		StdPalImport;				// tabulka importu barev Windows na vlastní barvy
BITMAPINFO* StdBitmapInfo;				// standardní záhlaví BMP
HPALETTE	StdPalette = NULL;			// vlastní logické palety
LOGPALETTE* pStdPalette = NULL;			// tabulka vlastních logický palet (potřebné též pro DirectDraw)

//#ifndef _MINI
//D3DRMPALETTEENTRY* pStdPalette2 = NULL;	// tabulka palet s alfa kanálem (pro Direct3D)
//#endif // _MINI

BYTE*		KonvPal;					// konverzní tabulka palet pro import souborů

PALETTEENTRY* SysPal = NULL;			// tabulka palet pro DirectDraw 256 barev

DWORD*		StdPalExport;				// tabulka exportu vlastních barev na barvy Windows

HWND		PrevWindow = NULL;			// předešlé aktivní okno

bool		Dither = true;				// používat dithering

bool		Release = false;			// je distribuční verze

BYTE*		Shadows;					// tabulka pro generování stínu

CString		ExeFileName;				// jméno tohoto programu

GETDISKFREESPACEEX	pGetDiskFreeSpaceEx = NULL;	// funkce GetDiskFreeSpaceEx (NULL=není)

//////////////////////////////////////////////////////////////////////////////
// prázdné objekty (např. pro návrat neplatné položky z funkce)

CString		EmptyString;				// prázdný řetězec
CIcon		EmptyIcon;					// prázdná ikona
CMap		EmptyMap;					// prázdná plocha
CPicture	EmptyPicture;				// prázdný obrázek
CSprite		EmptySprite;				// prázdný sprajt
CSound		EmptySound;					// prázdný zvuk
CMusic		EmptyMusic;					// prázdná hudba

/////////////////////////////////////////////////////////////////////////////
// buffery

int			TextListBeg;							// index počátku globálních seznamů
int			TextListEnd;							// index konce globálních seznamů

//CBufProg	BufProg[PROGBUFNUM];					// (0 až 5) buffery programu (0=interní)
CBufReal	Real;									// buffer reálných čísel
CBufText	Text;									// buffer textů
CBufBool	Bool;									// buffer logických proměnných
CBufIcon	Icon;									// buffer ikon
CBufMap		Map;									// buffer ploch
CBufPic		Picture;								// buffer obrázků
CBufSprite	Sprite;									// buffer sprajtů
CBufSound	Sound;									// buffer zvuků
CBufMusic	Music;									// buffer hudby
CBufList	List;									// buffer seznamů
CBufWin		Win;									// buffer prvků oken

// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI

//CBufD3DO	D3DO;									// buffer objektů Direct3D
//CBufD3DF	D3DF;									// buffer rámů Direct3D

#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------

//////////////////////////////////////////////////////////////////////////////
// konstanty

const char*	Compiled =	"Gemtree 32-compiler"; // jen tak pro zmatení

// tabulka hladin standardních barev (rozsah 0 až 256)
const int ColLevTab[] = 
{
	64*4,
	54*4,
	44*4,
	33*4,
	22*4,
	11*4,
};

// tabulka odstínů standardních barev (0 až 255) - pořadí B, G, R, F,
const RGBQUAD ColColTab[] = 
{ 
	0,		0,		255,		0,			//  0: červená
	72,		72,		255,		0,			//  1:
	116,	116,	255,		0,			//  2:
	158,	166,	255,		0,			//  3:
	210,	220,	255,		0,			//  4:
	0,		114,	255,		0,			//  5: hnědá
	150,	198,	255,		0,			//  6:
	0,		182,	255,		0,			//  7: oranžová
	96,		192,	255,		0,			//  8:
	0,		255,	255,		0,			//  9: žlutá
	128,	255,	255,		0,			// 10:
	192,	255,	255,		0,			// 11: světle žlutá
	0,		255,	178,		0,			// 12: žlutozelená
	0,		255,	0,			0,			// 13: zelená
	128,	255,	128,		0,			// 14:
	192,	255,	192,		0,			// 15:
	146,	255,	0,			0,			// 16: namodrale zelená
	222,	255,	138,		0,			// 17:
	255,	255,	0,			0,			// 18: modrozelená
	255,	186,	0,			0,			// 19:
	255,	222,	124,		0,			// 20:
	255,	128,	0,			0,			// 21:
	255,	0,		0,			0,			// 22: modrá
	255,	80,		72,			0,			// 23:
	255,	144,	128,		0,			// 24:
	255,	192,	154,		0,			// 25:
	255,	238,	210,		0,			// 26:
	255,	0,		114,		0,			// 27:
	255,	0,		174,		0,			// 28:
	255,	162,	198,		0,			// 29:
	255,	0,		255,		0,			// 30: fialová
	255,	128,	255,		0,			// 31: světle fialová
	138,	0,		255,		0,			// 32: fialově červená
	192,	128,	255,		0,			// 33:
	210,	186,	255,		0,			// 34:

	255,	255,	255,		0,			// 35: bílá (nepoužije se, vygeneruje se)
	96,		96,		96,			0,			// 36: šedá (nepoužije se, vygeneruje se)
};

//const int ColLev = sizeof(ColLevTab)/sizeof(ColLevTab[0]);	// počet hladin barev
//const int ColCol = sizeof(ColColTab)/sizeof(ColColTab[0]);	// počet odstínů barev
//const int StdColors = ResCols + ColCol*ColLev;				// počet vlastních palet (začínají od 0)
const BYTE WhiteCol = StdColors - 2*ColLev;			// bílá barva
const BYTE BlackCol = StdColors - 1;				// černá barva

const BYTE BlueCol = 22*ColLev + ResCols;			// modrá barva
const BYTE RedCol = 0*ColLev + ResCols;				// červená barva
const BYTE YellowCol = 9*ColLev + ResCols;			// žlutá barva
const BYTE GreenCol = 13*ColLev + ResCols;			// zelená barva
const BYTE LtYellowCol = 11*ColLev + ResCols;		// světle žlutá barva
const BYTE OrangeCol = 7*ColLev + ResCols;			// oranžová

const double pi  = 3.14159265358979323846;			// Ludolfovo číslo
const double pi2 = 6.28318530717958647693;			// Ludolfovo číslo * 2
const double pi8  = 25.13274122871834590768;		// Ludolfovo číslo * 8
const double pi2m = -6.28318530717958647693;		// - Ludolfovo číslo * 2
const double pi8m  = -25.13274122871834590768;		// - Ludolfovo číslo * 8
const double eul = 2.71828182845904523536;			// Eulerovo číslo

const double uhel22  = 0.39269908169872415481;		// úhel 22.5 stupňů v radiánech
const double uhel30  = 0.52359877559829887308;		// úhel 30 stupňů v radiánech
const double uhel45  = 0.78539816339744830962;		// úhel 45 stupňů v radiánech
const double uhel60  = 1.04719755119659774615;		// úhel 60 stupňů v radiánech
const double uhel67  = 1.17809724509617246442;		// úhel 67.5 stupňů v radiánech
const double uhel90  = 1.57079632679489661923;		// úhel 90 stupňů v radiánech
const double uhel135 = 2.35619449019234492885;		// úhel 135 stupňů v radiánech
const double uhel180 = 3.14159265358979323846;		// úhel 180 stupňů v radiánech
const double uhel225 = 3.92699081698724154808;		// úhel 225 stupňů v radiánech
const double uhel270 = 4.71238898038468985769;		// úhel 270 stupňů v radiánech
const double uhel315 = 5.49778714378213816731;		// úhel 315 stupňů v radiánech

const double ln10 = 2.3025850929940456840;			// přirozený logaritmus 10

const double degrad = 0.017453292519943295769237;	// konstanta pro převod z DEG na RAD (pi/180)
const double raddeg = 57.2957795130823208768;		// konstanta pro převod z RAD na DEG (180/pi)

double* FactTab;									// tabulka faktoriálů

//////////////////////////////////////////////////////////////////////////////
// lokální proměnné

#ifdef _MT
static	CRITICAL_SECTION	ExitCriticalSection;	// kritická sekce pro konec programu
#endif

unsigned	RandomSeed;					// proměnná pro generátor náhody


//////////////////////////////////////////////////////////////////////////////
// inicializace úseku inicializačních/terminačních funkcí

void InitTerm(_PVFV* beg, _PVFV* end)
{
	for (; beg < end; beg++)
	{
		if ((*beg != NULL) && (*beg != (PFV)-1))
		{
			(**beg)();
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
// inicializace jazyku

void InitLang(int jazyk)
{
	Jazyk = jazyk;
	LangID = JazykInfo[jazyk].LangID;
	CharSet = JazykInfo[jazyk].CharSet;
	CodePage = JazykInfo[jazyk].CodePage;

// desetinná tečka/čárka (tečku pouzívá pouze USA)
	if (jazyk == JAZYKENG)
	{
		Carka = '.';
	}
	else
	{
		Carka = ',';
	}

// implicitní jazyk pro import
	switch (jazyk)
	{
	case JAZYKCZ:
		JazykDefImp = JAZYKSLO;
		break;

	case JAZYKSLO:
	case JAZYKPOL:
		JazykDefImp = JAZYKCZ;
		break;

	default:
		JazykDefImp = JAZYKENG;
	}
}


//////////////////////////////////////////////////////////////////////////////
// inicializace klávesnice

void InitKeyb()
{

/* JAKJAKJAK 051222

	CString keyid;
	int langid = LangID;
	keyid.Hex(langid);

	while (::LoadKeyboardLayout(keyid, KLF_ACTIVATE | KLF_SUBSTITUTE_OK) == NULL)
	{
		langid += 0x400;
		if (langid >= 0x1c00) break;
		keyid.Hex(langid);
	}

*/

}

//////////////////////////////////////////////////////////////////////////////
// aktualizace informací o obrazovce (není-li celoobrazovkový mód)

void AktDispInfo0()
{
	ScreenWidth = ::GetSystemMetrics(SM_CXFULLSCREEN);
	ScreenHeight = ::GetSystemMetrics(SM_CYFULLSCREEN);
	ScreenWidth0 = ::GetSystemMetrics(SM_CXSCREEN);
	ScreenHeight0 = ::GetSystemMetrics(SM_CYSCREEN);

	HDC dc = ::GetDC(MainFrame);
	if (dc)
	{
		ScreenBits = ::GetDeviceCaps(dc, BITSPIXEL);
		::ReleaseDC(MainFrame, dc);
	}
}

void AktDispInfo()
{
	if (!FullScreen) AktDispInfo0();
}

//////////////////////////////////////////////////////////////////////////////
// hlavní start programu

void WinMainCRTStartup()
{
// úschova verze systému
	VerzeOS = (int)::GetVersion();
	OSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&OSVersionInfo);

// handle instance programu
	hInstance = ::GetModuleHandle(NULL);

// detekce implicitní znakové sady
	int acp = ::GetACP();

	switch (acp)
	{
	case 1250:
		DefCharSet = EASTEUROPE_CHARSET;
		goto ACPSET;

	case 1251:
		DefCharSet = RUSSIAN_CHARSET;
		break;

	case 1252:
		DefCharSet = ANSI_CHARSET;
		break;

	case 1253:
		DefCharSet = GREEK_CHARSET;
		goto ACPSET;

	case 1254:
		DefCharSet = TURKISH_CHARSET;
		goto ACPSET;

	case 1255:
		CharSet = HEBREW_CHARSET;
		break;

	case 1256:
		DefCharSet = ARABIC_CHARSET;
ACPSET:
		JazykInfo[JAZYKENG].CodePage = acp;
		JazykInfo[JAZYKENG].CharSet = DefCharSet;
		break;

	case 1257:
		DefCharSet = BALTIC_CHARSET;
		break;

	case 1258:
		CharSet = VIETNAMESE_CHARSET;
		break;

	default:
		DefCharSet = DEFAULT_CHARSET;
	}

// zjištění aktuálního jazyku
	{
		DWORD num = 0;
		HKEY hkey;

		if (::RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,
			ProductKey0,
			0,
			KEY_QUERY_VALUE,
			&hkey) == ERROR_SUCCESS)
		{
			DWORD type = REG_DWORD;
			DWORD bufsize = 4;

			::RegQueryValueEx(
				hkey,
				_T("Language"),
				0,
				&type,
				(BYTE*)&num,
				&bufsize);

			::RegCloseKey(hkey);
		}

		if (num == 0)
		{
			num = (::GetUserDefaultLangID() & 0xff);
		}

		for (int ll = 0; ll < JAZYKNUM; ll++)
		{
			if (JazykInfo[ll].LangID == (int)num)
			{
				JazykDef = ll;
				break;
			}
		}

		InitLang(JazykDef);
	}

// inicializace funkce GetDiskFreeSpaceEx pro OSR2
	if (((OSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
		(LOWORD(OSVersionInfo.dwBuildNumber) > 1000)) ||
		(OSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT))
	{
		KernLibrary = ::LoadLibrary(_T("KERNEL32.DLL"));

		if (KernLibrary != NULL)
		{
#ifdef _UNICODE
			pGetDiskFreeSpaceEx = (GETDISKFREESPACEEX)
				::GetProcAddress(KernLibrary,	"GetDiskFreeSpaceExW");
#else
			pGetDiskFreeSpaceEx = (GETDISKFREESPACEEX)
				::GetProcAddress(KernLibrary,	"GetDiskFreeSpaceExA");
#endif
		}
	}

// úschova předešlého aktivního okna
	PrevWindow = ::GetForegroundWindow();

// inicializace informací o displeji
	AktDispInfo();
	MainFrameX = ScreenWidth/2;
	MainFrameY = ScreenHeight/2;

// inicializace uzamykání ukončení programu
#ifdef _MT
	::InitializeCriticalSection(&ExitCriticalSection);
#endif

// inicializace správce paměti
	if (!MemInit()) 
	{
		Exit(EXITCODE_MEMERR);
	}

// příprava časovače
	if (VerzeOS > 0) 
		TimerConst = 60;			// časovač pro NT
	else
		TimerConst = 30;			// časovač pro 95

// inicializace generátoru náhory
	SYSTEMTIME systime;
	::GetLocalTime(&systime);
	unsigned time = systime.wMilliseconds + 137*systime.wSecond + 2567*systime.wMinute + 358946*systime.wDay;
	RandomSeed = time + 486874*time;	// inicializace generátoru náhody

// inicializace koprocesoru
	FPUDouble();

// inicializace dat prázdné ikony
	EmptyIconData.Data = (BYTE*)MemGet(ICONSIZE);
	MemFill(EmptyIconData.Data, ICONSIZE, BackCol);
	EmptyPictureData.Data = (BYTE*)MemGet(ICONSIZE);
	MemFill(EmptyPictureData.Data, ICONSIZE, BackCol);

// inicializace obsluhy sprajtů
	InitSprite();

// inicializace obsluhy ploch
	InitMap();

// inicializace globálních objektů
	InitTerm(__xc_a, __xc_z);

// načtení jména tohoto programu
	TCHAR buf[_MAX_PATH];
	::GetModuleFileName(hInstance, buf, _MAX_PATH);
	ExeFileName = buf;

// inicializace standardního záhlaví BMP
	StdBitmapInfo = (BITMAPINFO*) MemGet(sizeof(BITMAPINFO) + sizeof(RGBQUAD)*255);
	MemFill(StdBitmapInfo, sizeof(BITMAPINFO) + sizeof(RGBQUAD)*255);	// vynulování
	StdBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);	// velikost záhlaví
	StdBitmapInfo->bmiHeader.biWidth = ICONWIDTH;	// šířka bitmapy
	StdBitmapInfo->bmiHeader.biHeight = ICONHEIGHT;	// výška bitmapy
	StdBitmapInfo->bmiHeader.biPlanes = 1;			// počet barevných rovin
	StdBitmapInfo->bmiHeader.biBitCount = 8;		// počet bitů na bod
	StdBitmapInfo->bmiHeader.biCompression = BI_RGB; // není komprese
	StdBitmapInfo->bmiHeader.biClrImportant = StdColors; // počet důležitých palet

// vytvoření standardních palet
	RGBQUAD* rgb = StdBitmapInfo->bmiColors;		// začátek tabulky palet

// neměnit paletu barvu pozadí - používá se při importu obrázků
	rgb->rgbRed =	BACKCOLOR_RED;					// průhledná barva
	rgb->rgbGreen =	BACKCOLOR_GREEN;
	rgb->rgbBlue =	BACKCOLOR_BLUE;
	rgb++;

	rgb->rgbRed =	SHADCOLOR_RED;					// barva stínu
	rgb->rgbGreen =	SHADCOLOR_GREEN;
	rgb->rgbBlue =	SHADCOLOR_BLUE;
	rgb++;

	int i,j,k;
	for (i = 0; i < (ColCol-2); i++)				// pro všechny barvy bez bílé a šedé
	{
		*rgb = ColColTab[i];						// přenesení základní barvy

		for (j = 1; j < ColLev; j++)				// pro všechny odstíny
		{
			k = ColLevTab[j];						// násobící koeficient
			rgb[j].rgbRed = (BYTE)(rgb->rgbRed*k/256);		// červená
			rgb[j].rgbGreen = (BYTE)(rgb->rgbGreen*k/256);	// zelená
			rgb[j].rgbBlue = (BYTE)(rgb->rgbBlue*k/256);	// modrá
		}
		rgb += ColLev;								// adresa další barvy
	}

	i = WhiteCol;									// index bílé barvy
	for (; i <= BlackCol; i++)						// od bílé barvy až po černou
	{
		k = (BlackCol-i)*255/(2*ColLev-1);			// odstín bílé barvy
		rgb->rgbRed = (BYTE)k;						// červená
		rgb->rgbGreen = (BYTE)k;					// zelená
		rgb->rgbBlue = (BYTE)k;						// modrá
		rgb++;										// zvýšení adresy barvy
	}

// vygenerování tabulky pro import palet Windows do vlatních barev
	StdPalImport = (BYTE*)MemGet(64 * 64 * 64);		// tabulka pro import palet

// načtení tabulky importu palet
	HRSRC hRes = ::FindResource(hInstance, MAKEINTRESOURCE(IDN_PALIMP), _T("LOADER"));
	HGLOBAL hData = ::LoadResource(hInstance, hRes);
	ASSERT((hRes != NULL) && (hData != NULL));
	if ((hRes == NULL) || (hData == NULL)) Exit(EXITCODE_LOADRES);
	DeKomp(StdPalImport, 64*64*64, (BYTE*)::LockResource(hData)+6, ::SizeofResource(hInstance, hRes)-6);

// vytvoření konverzní tabulky palet pro ímport souborů BMP
	KonvPal = (BYTE*)MemGet(256);			// konverzní tabulka palet

// vytvoření vlastních logických palet
	pStdPalette = (LOGPALETTE*) MemGet(sizeof(PALETTEENTRY)*(256-1)+sizeof(LOGPALETTE));
	MemFill(pStdPalette, sizeof(PALETTEENTRY)*(256-1)+sizeof(LOGPALETTE), 0);
	pStdPalette->palVersion = 0x300;
	pStdPalette->palNumEntries = StdColors;
	PALETTEENTRY* pal = pStdPalette->palPalEntry;
	rgb = StdBitmapInfo->bmiColors;
	for (i = StdColors; i > 0; i--)
	{
		pal->peRed = rgb->rgbRed;
		pal->peGreen = rgb->rgbGreen; 
		pal->peBlue = rgb->rgbBlue;
		pal->peFlags = 0;
		pal++;
		rgb++;
	}
	StdPalette = ::CreatePalette(pStdPalette);
	ASSERT(StdPalette != NULL);

// palety pro Direct3D s alfa kanálem
#ifndef _MINI
//	pStdPalette2 = (D3DRMPALETTEENTRY*) MemGet(sizeof(D3DRMPALETTEENTRY)*256);
//	MemCopy(pStdPalette2, pStdPalette->palPalEntry, sizeof(D3DRMPALETTEENTRY)*256);
//	for (i = 0; i < 256; i++)
//	{
//		pStdPalette2[i].flags = 0xff;
//	}
//	pStdPalette2[BackCol].flags = 0;
//	pStdPalette2[ShadCol].flags = 0x7f;
#endif // _MINI

// vygenerování tabulky pro export vlastních barev do palet Windows BGR
	StdPalExport = (DWORD*)MemGet(256*sizeof(DWORD));
	DWORD* exppal = StdPalExport;
	rgb = StdBitmapInfo->bmiColors;

	for (i = 0; i < 256; i++)
	{
		*exppal = *(DWORD*)rgb;

		BYTE c = ((BYTE*)exppal)[0];
		((BYTE*)exppal)[0] = ((BYTE*)exppal)[2];
		((BYTE*)exppal)[2] = c;
		((BYTE*)exppal)[3] = 0;

		if (i == BackCol) *exppal = 0xff000000;
		if (i == ShadCol) *exppal = 0x70000000;
		if (i >= StdColors) *exppal = 0;

		exppal++;
		rgb++;
	}

// vygenerování tabulky faktoriálů (poslední hodnota = přetečení)
	FactTab = (double*)MemGet((MAXFACT+2) * sizeof(double));
	FactTab[0] = 0;
	FactTab[1] = 1;
	for (i = 2; i <= MAXFACT+1; i++)
	{
		FactTab[i] = FactTab[i-1] * i;
	}

// vygenerování tabulky stínů
	Shadows = (BYTE*)MemGet(256);
	for (i = 0; i < 256; i++)
	{
		int col;

		if ((i >= StdColors) ||
			(i < ResCols) ||
			((((i - ResCols + 1) % ColLev) == 0) &&
			(i != (StdColors - ColLev))))
		{
			col = BlackCol;
		}
		else
		{
			col = i+1;
			if ((col > WhiteCol) && (col < BlackCol)) col++;
		}

		Shadows[i] = (BYTE)col;
	}

// úschova příkazového řádku
	CommandLine = ::GetCommandLine();

// inicializace bufferu dat
	PetAdr = (PETHEAD*)((DWORD)hInstance + *(DWORD*)DATAsize);
	PetSize = *((long*)DATAsize + 1);

#if defined(_DEBUG)
//	&& !defined(_OPTIM)
	CFileMap file;
	VERIFY(file.Open(_T("C:\\VC\\Peter\\Program\\Peter.exe")) ||
			file.Open(_T("D:\\VC\\Peter\\Program\\Peter.exe")) ||
			file.Open(_T("E:\\VC\\Peter\\Program\\Peter.exe")));
	IMAGE_NT_HEADERS* hdr = file.NTHeader();
	ASSERT(hdr);
	IMAGE_SECTION_HEADER* sec = file.NTSection(hdr, ".petprg");
	if (sec == NULL) sec = file.NTSection(hdr, ".rtexts");
	ASSERT(sec);
	PetAdr = (PETHEAD*)(file.Adr() + sec->PointerToRawData);
	PetSize = sec->SizeOfRawData;
	ASSERT(file.IsValid(PetAdr, PetSize));
#endif

	PetBuf.Adr((BYTE*)PetAdr);
	PetBuf.Size(PetSize);
	if (PetSize < 500) Exit(EXITCODE_LOADRES);

	if (*(DWORD*)PetAdr == '\1LER')
	{
		Release = true;
	}
	else
	{
		if (*(DWORD*)PetAdr == '\1TEP')
		{
			Release = false;
		}
		else
		{
			Exit(EXITCODE_LOADRES);
		}
	}

// inicializace obsluhy UDP (inicializace bufferů)
#ifndef _MINI
	InitUDP();
#endif // _MINI

// načtení programu
	Load();

// kompilace programu
	Comp();

// vytvoření hlavního okna aplikace
	if (!MainFrameCreate()) 
	{
		Exit(EXITCODE_MAINFRAME);
	}

// inicializace klávesnice (ne pro angličtinu)
	if (LangID != 9)
	{
		InitKeyb();
	}

// provádění programu
	Exec();

#if defined(_DEBUG) && !defined(_OPTIM)
	file.Close();
#endif

// konec programu
	Exit(ReturnCode);
}


//void mainCRTStartup()
//{
//	WinMainCRTStartup();
//}


//////////////////////////////////////////////////////////////////////////////
// ukončení programu

void Exit(DWORD code)
{
// uzamknutí ukončení programu
#ifdef _MT
	::EnterCriticalSection(&ExitCriticalSection);
#endif

// zrušení palet
	::DeleteObject(StdPalette);

// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI
	
// uvolnění knihovny COM
	COMUninit();

// uvolnění knihovny WSA
	WSADeinitialize();
#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------

	if (KernLibrary != NULL)
	{
		::FreeLibrary(KernLibrary);
	}

// ukončení správce paměti
	MemTerm();

// uvolnění uzamykání ukončení programu
#ifdef _MT
	::DeleteCriticalSection(&ExitCriticalSection);
#endif

// nastavení fokusu na předešlé okno
	if (AppActive)
	{
		::SetForegroundWindow(PrevWindow);
	}

// ukončení programu
	::ExitProcess(code);
}


//////////////////////////////////////////////////////////////////////////////
// chyba pro DEBUG OPTIM mód

#if defined(_OPTIM) && defined(_M_IX86) && defined(_DEBUG)
void debugBreak(char* file, int line, char* date)
{
	CString txt("Interní chyba interpreteru PETER. Nahlaste, prosím, dodavateli následující\n"
			  "údaje a popište situaci, za které tato chyba nastala. Děkujeme za pochopení.\n"
			   "\n     soubor = X");

	CString txt2(file);
	txt += txt2.Right(txt2.Length() - txt2.RevFind('\\') - 1);
	txt += "           řádek = ";
	txt2.Int(line);
	txt += txt2;
	txt += "           verze = ";
	txt += date;
	txt += '\n';

	::MessageBox(
		NULL,
		txt,
		"Interní chyba",
		MB_OK | MB_ICONERROR);
}
#endif


//////////////////////////////////////////////////////////////////////////////
// vystředění okna proti jinému oknu

void CenterWindow(HWND child, HWND parent)
{
	RECT rc1, rc2;

	if (parent == NULL)
	{
		::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rc1, NULL);
	}
	else
	{
		::GetWindowRect(parent, &rc1);
	}
	::GetWindowRect(child, &rc2);

	::SetWindowPos(child, NULL, 
		(rc1.left + rc1.right)/2 - (rc2.right - rc2.left)/2,
		(rc1.top + rc1.bottom)/2 - (rc2.bottom - rc2.top)/2, 0, 0,
		SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER);
}

//////////////////////////////////////////////////////////////////////////////
// nastavení vyšší přesnosti procesoru

// Při startu programu: CTRL = 0x027F (53 bitů)
// Vlastní přesnost: CTRL = 0x037F (64 bitů)
// Knihovna Direct3D: CTRL = 0x007F (přesnost float)

// Podle doporučení DirectX7:
//		and ah,not 3 .... přesnost single
//		or  al,0x3f ..... zakázat výjimky
//		and ah,not 0xc0 . zaokrouhlování k nejbližšímu

void FPUDouble()
{

#ifdef _M_IX86

	WORD stat;
	_asm {
		wait						// synchronizace
		fnstcw		stat			// uložení řídicího slova
		wait						// synchronizace
		mov			ax,stat			// stav řídicího slova
		and			ah,not 0xc		// implicitní zaokrouhlování
		or			ah,3			// přesnost 64 bitů
		mov			stat,ax			// nový stav řídicího slova
		fldcw		stat			// nastavení nového řídicího slova
	}
#endif

}

//////////////////////////////////////////////////////////////////////////////
// nastavení nižší přesnosti procesoru

void FPUFloat()
{

#ifdef _M_IX86

	WORD stat;
	_asm {
		wait						// synchronizace
		fnstcw		stat			// uložení řídicího slova
		wait						// synchronizace
		mov			ax,stat			// stav řídicího slova
		and			ah,not 0xf		// implicitní zaokrouhlování
		mov			stat,ax			// nový stav řídicího slova
		fldcw		stat			// nastavení nového řídicího slova
	}
#endif

}

//////////////////////////////////////////////////////////////////////////////
// spuštění programu

DWORD Exec(CString command, BOOL wait)
{
	DWORD result = 0;
	command.TrimLeft();
	if (command.IsEmpty()) return 0;

// inicializace startovací tabulky
	STARTUPINFO si;
	MemFill(&si, sizeof(si));
	si.cb = sizeof(si);

// inicializace tabulky procesu
	PROCESS_INFORMATION pi;
	MemFill(&pi, sizeof(pi));

// spuštění programu
	if (
		::CreateProcess(
		NULL,								// jméno programu
		(LPTSTR)(LPCTSTR)command,			// příkazový řádek
		NULL,								// ochranné atributy procesu
		NULL,								// ochranné atributy vlákna
		FALSE,								// dědičnost handlů
#ifdef _UNICODE
		CREATE_UNICODE_ENVIRONMENT			// parametry
#else
		0
#endif
		| (wait ? 0 : DETACHED_PROCESS)
		| CREATE_DEFAULT_ERROR_MODE,
		NULL,								// prostředí
		NULL,								// výchozí adresář
		&si,								// adresa STARTUPINFO
		&pi) == 0)							// adresa PROCESS_INFORMATION
	{
		if ((command.Length() == 2) && (command[1] == ':'))
		{
			if (::SetCurrentDirectory(command))
			{
				return 0;
			}
			else
			{
				FileError = true;
				return 1;
			}
		}

		if ((command.Length() > 2) && ((command[0] == 'c') || (command[0] == 'C'))
			&& ((command[1] == 'd') || (command[1] == 'D')) &&
			((command[2] == ' ') || (command[2] == 9) || 
			 (command[2] == '.') || (command[2] == '\\')))
		{
			command.Delete(0, 2);
			command.TrimLeft();
			command.TrimRight();
			if (::SetCurrentDirectory(command))
			{
				return 0;
			}
			else
			{
				FileError = true;
				return 1;
			}
		}

		TCHAR* buf = (TCHAR*)MemGet(1024*sizeof(TCHAR));
		int len = ::GetWindowsDirectory(buf, 1024);
		CString windir(buf, len);

		if (VerzeOS > 0)					// NT
		{

			command = windir + _T("\\system32\\cmd.exe /c") + command;
		}
		else
		{
			command = windir + _T("\\command.com /c") + command;
		}
		MemFree(buf);
		
		if (
			::CreateProcess(
			NULL,								// jméno programu
			(LPTSTR)(LPCTSTR)command,			// příkazový řádek
			NULL,								// ochranné atributy procesu
			NULL,								// ochranné atributy vlákna
			FALSE,								// dědičnost handlů
#ifdef _UNICODE
			CREATE_UNICODE_ENVIRONMENT			// parametry
#else
			0
#endif
			| (wait ? 0 : DETACHED_PROCESS)
			| CREATE_DEFAULT_ERROR_MODE,
			NULL,								// prostředí
			NULL,								// výchozí adresář
			&si,								// adresa STARTUPINFO
			&pi) == 0)							// adresa PROCESS_INFORMATION
		{
			FileError = true;
			return 1;
		}
	}

// čekání na ukončení programu
	if (wait)
	{
		::WaitForSingleObject(pi.hProcess, INFINITE);	// čekání na ukončení procesu
		::GetExitCodeProcess(pi.hProcess, &result);		// zjištění návratového kódu
	}

// uzavření handle procesu
	::CloseHandle(pi.hProcess);				// uzavření handle procesu
	::CloseHandle(pi.hThread);				// uzavření handle toku

	return result;
}

/*
int Exec(CString name, CString param, BOOL wait)
{
	int result = 0;

// příprava jména programu
	name.TrimLeft();
	name.TrimRight();
	CString name0 = name;
	bool uvoz = false;

	for (int i = name.Length()-1; i >= 0; i--)
	{
		if (name[i] == _T('"'))
		{
			name = name.Left(i) + _T('"') + name.Right(name.Length() - i);
			uvoz = true;
		}

		if ((name[i] == _T(' ')) || (name[i] == 9)) uvoz = true;
	}
	if (uvoz) name = _T('"') + name + _T('"');

// příprava příkazového řádku
	TCHAR* commandline = (TCHAR*)MemGet(4096 * sizeof(TCHAR));
	TCHAR* dst = commandline;

	int len = name.Length();
	if (len > 2000) len = 2000;
	MemCopy(dst, (LPCTSTR)name, len * sizeof(TCHAR));
	dst += len;

	*dst = _T(' ');
	dst++;

	len = param.Length();
	if (len > 2000) len = 2000;
	MemCopy(dst, (LPCTSTR)param, len * sizeof(TCHAR));
	dst += len;

	*dst = 0;

// inicializace startovací tabulky
	STARTUPINFO si;
	MemFill(&si, sizeof(si));
	si.cb = sizeof(si);

// inicializace tabulky procesu
	PROCESS_INFORMATION pi;
	MemFill(&pi, sizeof(pi));

// spuštění programu
	::CreateProcess(
		name0,								// jméno programu
		commandline,						// příkazový řádek
		NULL,								// ochranné atributy procesu
		NULL,								// ochranné atributy vlákna
		FALSE,								// dědičnost handlů
#ifdef _UNICODE
		CREATE_UNICODE_ENVIRONMENT			// parametry
#else
		0
#endif
		| DETACHED_PROCESS,
		NULL,								// prostředí
		NULL,								// výchozí adresář
		&si,								// adresa STARTUPINFO
		&pi);								// adresa PROCESS_INFORMATION

// čekání na ukončení programu
	if (wait)
	{
		::WaitForSingleObject(pi.hProcess, INFINITE);	// čekání na ukončení procesu
		::GetExitCodeProcess(pi.hProcess, (DWORD*)&result);	// zjištění návratového kódu
	}

// uzavření handle procesu
	::CloseHandle(pi.hProcess);				// uzavření handle procesu
	::CloseHandle(pi.hThread);				// uzavření handle toku

	MemFree(commandline);

	return result;
}
*/

/////////////////////////////////////////////////////////////////////////////
// generování konverzní tabulky barev obrázku

void GenKonvPal(BITMAPINFO* bmp)
{
// lokální proměnné
	int			i;									// čítač barev
	int			palet = bmp->bmiHeader.biClrUsed;	// počet palet v bitmapě
	RGBQUAD*	col = bmp->bmiColors;				// ukazatel barevných složek
	BYTE*		pal = KonvPal;						// ukazatel konverzních palet
	DWORD		BackColData = *(DWORD*)(StdBitmapInfo->bmiColors + BackCol); // paleta pozadí
	DWORD		ShadColData = *(DWORD*)(StdBitmapInfo->bmiColors + ShadCol); // paleta stínu

// příprava počtu palet
	if (palet == 0) palet = (1 << bmp->bmiHeader.biBitCount);
	i = palet;
	if ((palet < 1) || (palet > 256)) return;

// cyklus přes platné barvy
	for (; i > 0; i--)
	{
		if (*(DWORD*)col == BackColData)
		{
			*pal = BackCol;
		}
		else
		{
			if (*(DWORD*)col == ShadColData)
			{
				*pal = ShadCol;
			}
			else
			{
				*pal = PalImport(col->rgbRed, col->rgbGreen, col->rgbBlue);
			}
		}
		pal++;
		col++;
	}

// vymazání zbylých neplatných barev (import na černou barvu)
	MemFill(pal, 256-palet, BlackCol);
}


/////////////////////////////////////////////////////////////////////////////
// vygenerování střední barvy (ze 4 bodů)

BYTE _fastcall ColAvrg(BYTE col1, BYTE col2, BYTE col3, BYTE col4)
{
// lokální proměnné
	int			r = 0;									// červená složka
	int			g = 0;									// zelená složka
	int			b = 0;									// modrá složka
	BYTE		n = 0;									// počet platných bodů
	RGBQUAD*	rgb;									// ukazatel palet
	int			shad = 0;								// čítač stínů

// první bod
	if (col1 != BackCol)
	{
		if (col1 == ShadCol)
		{
			shad++;
		}
		else
		{
			n++;											// zvýšení čítače bodů
			rgb = StdBitmapInfo->bmiColors + col1;			// definice palet bodu
			r = rgb->rgbRed;								// červená složka
			g = rgb->rgbGreen;								// zelená složka
			b = rgb->rgbBlue;								// modrá složka
		}
	}

// druhý bod
	if (col2 != BackCol)
	{
		if (col2 == ShadCol)
		{
			shad++;
		}
		else
		{
			n++;											// zvýšení čítače bodů
			rgb = StdBitmapInfo->bmiColors + col2;			// definice palet bodu
			r += rgb->rgbRed;								// červená složka
			g += rgb->rgbGreen;								// zelená složka
			b += rgb->rgbBlue;								// modrá složka
		}
	}

// třetí bod
	if (col3 != BackCol)
	{
		if (col3 == ShadCol)
		{
			shad++;
		}
		else
		{
			n++;											// zvýšení čítače bodů
			rgb = StdBitmapInfo->bmiColors + col3;			// definice palet bodu
			r += rgb->rgbRed;								// červená složka
			g += rgb->rgbGreen;								// zelená složka
			b += rgb->rgbBlue;								// modrá složka
		}
	}

// čtvrtý bod
	if (col4 != BackCol)
	{
		if (col4 == ShadCol)
		{
			shad++;
		}
		else
		{
			n++;											// zvýšení čítače bodů
			rgb = StdBitmapInfo->bmiColors + col4;			// definice palet bodu
			r += rgb->rgbRed;								// červená složka
			g += rgb->rgbGreen;								// zelená složka
			b += rgb->rgbBlue;								// modrá složka
		}
	}

// bod byl stín
	if (shad > 2)
	{
		return ShadCol;
	}

// bod byl pozadí
	if (n < 2)
	{
		return BackCol;
	}

// jinak vygenerování bodu podle barevných složek
	else
	{
		return PalImport((BYTE)(r/n), (BYTE)(g/n), (BYTE)(b/n));
	}
}


/////////////////////////////////////////////////////////////////////////////
// dekomprese dat

#define MAXLENX 25					// min. délka dlouhého řetězce
#define MAXLEN (MAXLENX+254)		// maximální délka řetězce
#define SUBSTLEN	7				// délka nahrazená dlouhým kódem

void DeKomp(BYTE* dstBuf, int dstNum, BYTE* srcBuf, int srcNum)
{
// načtení jednoho bitu ze stavového slova
#define DekBit		bit = status & 1;				\
					status >>= 1;					\
					if (status == 0)				\
					{								\
						if (srci >= srcNum) break;	\
						status = *(WORD*)src;		\
						src++;						\
						src++;						\
						srci++;						\
						srci++;						\
						bit = status & 1;			\
						status >>= 1;				\
						status |= 0x8000;			\
					}


	BYTE* dst = dstBuf;				// ukazatel cílové adresy
	int dsti = 0;					// čítač cílových dat
	BYTE* src = srcBuf;				// ukazatel zdrojové adresy
	int srci = 0;					// čítač zdrojových dat
	BYTE* src2;						// pomocný ukazatel
	int srci2;

	WORD status = 0;				// střadač stavového slova
	BYTE offsetL, offsetH;			// offset k opakování
	int delka;						// délka k opakování
	int bit;						// 1 = načtený bit

	for (;;)
	{
// načtení prvního bitu příznaku
		DekBit

// přesun bajtu bez komprese
		if (bit == 0)
		{
			if (srci >= srcNum) break;
			if (dsti >= dstNum) break;
			*dst = *src;
			dst++;
			dsti++;
			src++;
			srci++;
		}

// jinak bude opakování řetězce
		else
		{
			offsetH = 0;
			offsetL = 0;

// první bit délky
			DekBit
			delka = bit;

// zvýšení čítače délky
			for (;;)
			{
				delka++;
				delka++;

// načtení příznaku konce kódu
				DekBit

				if (bit == 0)
				{
					break;
				}

				if (delka >= (MAXLENX-4+1))
				{
					delka++;
					delka++;
					break;
				}
			}

// korekce pro náhradní kód
			if (delka >= SUBSTLEN)
			{
				if (delka == SUBSTLEN)
				{
					if (srci >= srcNum) break;
					delka = *src;
					src++;
					srci++;
					if (delka == 255) break;
					delka += MAXLENX;
				}
				else
				{
					delka--;
				}
			}

// stanovení offsetu - vyšší bajt
			if (delka != 2)
			{
				DekBit
				if (bit == 0)
				{
					DekBit
					offsetH <<= 1;
					offsetH |= bit;

					DekBit
					offsetH <<= 1;
					offsetH |= bit;

					DekBit
					offsetH <<= 1;
					offsetH |= bit;

					if (offsetH > 1)
					{
						DekBit
						offsetH <<= 1;
						offsetH |= bit;

						offsetH--;
						if (offsetH > 6)
						{
							DekBit
							offsetH <<= 1;
							offsetH |= bit;

							offsetH -= 7;
							if (offsetH > 13)
							{
								DekBit
								offsetH <<= 1;
								offsetH |= bit;

								offsetH -= 14;
							}
						}
					}
					else
					{
						offsetH++;
					}
				}
			}

// načtení offsetu - nižší bajt
			if (srci >= srcNum) break;
			offsetL = *src;
			src++;
			srci++;

// přenesení řetězce
			srci2 = dsti - (WORD)(offsetL + offsetH*256);
			if (srci2 < 0) break;
			src2 = &(dstBuf[srci2]);

			for (; delka > 0; delka--)
			{
				if (srci2 >= dstNum) break;
				if (dsti >= dstNum) break;
				*dst = *src2;
				dst++;
				dsti++;
				src2++;
				srci2++;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// generátor náhody

int rand()
{
	RandomSeed = RandomSeed * 214013 + 2531011;
	return ((RandomSeed >> 16) & 0x7fff);
}

double Rand()
{
	RandomSeed = RandomSeed * 214013 + 2531011;
	return (double)RandomSeed / (double) 4294967296;	// vydělení rozsahem LONG INT
}


/////////////////////////////////////////////////////////////////////////////
// převod reálného čísla na celé číslo se zaokrouhlením k nejbližší hodnotě

int Round(double num)
{

#ifdef _M_IX86

	DWORD		result;				// výsledek operace

	_asm {
		fld			num				// načtení čísla k provedení operace
		fistp		result			// převod čísla na celé číslo
	}
	return result;

#else

	if (num >= 0)
	{
		return (int)(num + 0.5);
	}
	else
	{
		return (int)(num - 0.5);
	}

#endif
}

/////////////////////////////////////////////////////////////////////////////
// převod reálného čísla na celé číslo s oříznutím k nule

int Round0(double num)
{

#ifdef _M_IX86

	WORD		oldstat;			// starý stav řídicího slova
	WORD		newstat;			// nový stav řídicího slova
	int			result;				// výsledek operace

	_asm {
		fld			num				// načtení čísla k provedení operace
		wait						// synchronizace
		fnstcw		oldstat			// uloženi řídicího slova
		wait						// synchronizace
		mov			ax,oldstat		// starý stav řídicího slova
		or			ah,0xc			// mód zaokrouhlení směrem k nule
		mov			newstat,ax		// nový stav řídicího slova
		fldcw		newstat			// nastavení nového řídicího slova
		fistp		result			// převod čísla na celé číslo
		fldcw		oldstat			// navrácení původního řídicího slova
	}
	return result;

#else

	return (int)num;

#endif
}

/////////////////////////////////////////////////////////////////////////////
// převod reálného čísla na celé číslo se zaokrouhlením nahoru (ke kladné hodnotě)

int RoundP(double num)
{

#ifdef _M_IX86

	WORD		oldstat;			// starý stav řídicího slova
	WORD		newstat;			// nový stav řídicího slova
	int			result;				// výsledek operace

	_asm {
		fld			num				// načtení čísla k provedení operace
		wait						// synchronizace
		fnstcw		oldstat			// uloženi řídicího slova
		wait						// synchronizace
		mov			ax,oldstat		// starý stav řídicího slova
		and			ah,not 0xc;		// vynulování řídicích bitů pro zaokrouhlení
		or			ah,8			// mód zaokrouhlení směrem nahoru
		mov			newstat,ax		// nový stav řídicího slova
		fldcw		newstat			// nastavení nového řídicího slova
		fistp		result			// převod čísla na celé číslo
		fldcw		oldstat			// navrácení původního řídicího slova
	}
	return result;

#else

	if (num >= 0)
	{
		int i = (int)num;
		if ((double)i == num) return i;
		return i+1;
	}
	else
	{
		return (int)num;
	}

#endif
}

/////////////////////////////////////////////////////////////////////////////
// převod reálného čísla na celé číslo se zaokrouhlením dolů (k záporné hodnotě)

int RoundM(double num)
{

#ifdef _M_IX86

	WORD		oldstat;			// starý stav řídicího slova
	WORD		newstat;			// nový stav řídicího slova
	int			result;				// výsledek operace

	_asm {
		fld			num				// načtení čísla k provedení operace
		wait						// synchronizace
		fnstcw		oldstat			// uloženi řídicího slova
		wait						// synchronizace
		mov			ax,oldstat		// starý stav řídicího slova
		and			ah,not 0xc;		// vynulování řídicích bitů pro zaokrouhlení
		or			ah,4			// mód zaokrouhlení směrem dolů
		mov			newstat,ax		// nový stav řídicího slova
		fldcw		newstat			// nastavení nového řídicího slova
		fistp		result			// převod čísla na celé číslo
		fldcw		oldstat			// navrácení původního řídicího slova
	}
	return result;

#else

	if (num >= 0)
	{
		return (int)num;
	}
	else
	{
		int i = (int)num;
		if ((double)i == num) return i;
		return i-1;
	}

#endif
}

/*
/////////////////////////////////////////////////////////////////////////////
// mocnina X^Y

static double NumConst1 = 1;

#pragma warning ( disable: 4035)	// hlášení - není návratová hodnota
#pragma warning ( disable: 4725)	// hlášení - problémová instrukce

double Power(double x, double y)
{
#ifdef _M_IX86


// lokální proměnné
	WORD		oldstat;			// starý stav řídicího slova
	WORD		newstat;			// nový stav řídicího slova - zaokrouhlení k nule
	int			mocn;				// celočíselná část mocnitele

	_asm {

// příprava řídicího slova pro zaokrouhlení k nule
		wait						// synchronizace
		fnstcw		oldstat			// uloženi řídicího slova
		wait						// synchronizace
		mov			ax,oldstat		// starý stav řídicího slova
		or			ah,0xc			// mód zaokrouhlení směrem k nule
		mov			newstat,ax		// nový stav řídicího slova

// příprava celočíselné části mocnitele (mocnitel zůstane v zásobníku)
		fldcw		newstat			// nastavení nového řídicího slova
		fld			y				// načtení mocnitele
		fist		mocn			// uložení celočíselné části mocnitele
		fldcw		oldstat			// navrácení původního řídicího slova

// test, zda je mocnitel celočíselný
		ficom		mocn			// porovnání s celočíselnou částí
		fnstsw		ax				// načtení stavového slova
		test		ah,0x40			// je mocnitel roven celočíselnému číslu?
		jz			P1				// není to celé číslo

// uvolnění mocnitele z vrcholu zásobníku
		fstp		st(0)			// zrušení čísla na vrcholu zásobníku

// příprava mocnitele
		mov			eax,mocn		// celočíselná část mocnitele
		or			eax,eax			// je mocnitel záporný?
		jns			M1				// mocnitel není záporný
		neg			eax				// oprava znaménka mocnitele

// příprava čísla a střadače, test zda je mocnitel = 0
M1:		fld1						// načtení 1 jako střadač -> ST(0)
		or			eax,eax			// je již celý mocnitel?
		jz			M5				// je již celý mocnitel
		fld			x				// načtení mocněnce -> ST(0), 1 -> ST(1)
		jmp			M3

// přidání jednoho prvku
M2:   	fmul		st(0), st(0)	// příprava dalšího prvku
M3:		shr			eax,1			// uplatní se součinitel?
		jnc			M4				// neuplatní se
		fmul		st(1), st(0)	// vynásobení prvkem

// kontrola, zda je výpočet již ukončen
M4:		or			eax,eax			// je již výpočet ukončen?
		jnz			M2				// bude další prvek
		fstp		st(0)			// zrušení prvku v ST(0)		
	
// pro záporný mocnitel převrácená hodnota výsledku

M5:		mov			eax,mocn		// celočíselná část mocnitele
		or			eax,eax			// je mocnitel záporný?
		jns			X9				// mocnitel není záporný
		fdivr		NumConst1		// převrácená hodnota čísla
		jmp			X9				// konec


P1:




X9:
	}
*/	
/*
	int stat;

	_asm {
// načtení mocnitele
		fld		num2		// načtení mocnitele Y -> ST(0)



		fld		num1		// načtení mocněnce X -> ST(0), Y -> ST(1)

// příprava znaménka výsledku
		ftst				// test 



// výpočet Y * log(2)X (dvojkový exponent výsledku)
		fabs				// je povolena pouze kladná hodnota X
		fyl2x				// Y * log(2)X, ST(1) uvolněno

// ohraničení mezivýsledku do intervalu -1 až +1
		fld		st(0)		// duplikace mezivýsledku
		wait				// synchronizace
		fnstcw		stat	// uloženi řídicího slova
		or			byte ptr [stat+1],0xc // zaokrouhlení k nule
		wait				// synchronizace
		fldcw		stat	// nastavení nového řídicího slova
		frndint				// celá část čísla - exponent
		and			byte ptr [stat+1],not 0xc // zaokrouhlení k nejbližšímu
		wait				// synchronizace
		fldcw		stat	// nastavení standardního řídicího slova
		fxch		st(1)	// záměna celé části a mezivýsledku
		fsub		st(0), st(1) // odečtení celé části od mezivýsledku

// výpočet 2^mezivýsledek
		f2xm1				// 2^X - 1
		fld1				// 1 -> ST(0), mezivýsledek -> ST(1)
		faddp	st(1),st(0)	// výsledek -> ST(1), ST(0) uvolněno
		fscale				// vynásobení celočíselnou částí (exponent)
		fstp	num1		// uložení výsledku operace
	}
*/
/*
#else

	return pow(x, y);

#endif
}

#pragma warning ( default: 4725)	// hlášení - problémová instrukce
#pragma warning ( default: 4035)	// hlášení - není návratová hodnota
*/
/////////////////////////////////////////////////////////////////////////////
// funkce pro vypnutou optimalizaci

#ifndef _OPTIM
int abs(int num) 
{ 
	if (num < 0) 
	{
		return -num; 
	} 
	else 
	{ 
		return num; 
	} 
};
#endif

/////////////////////////////////////////////////////////////////////////////
// poskytnutí aktivního okna, je-li typu Edit (NULL = není)

HWND GetEditFocus()
{
	HWND wnd = ::GetFocus();
	if (wnd != NULL)
	{
		TCHAR buf[21];
		::GetClassName(wnd, buf, 20);
		if (EditClassName != buf)
		{
			wnd = NULL;
		}
	}
	return wnd;
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření adresáře (pokud existuje, je vše OK)

BOOL CreateDir(CString txt)
{
// ořezání mezer
	txt.TrimLeft();
	txt.TrimRight();

// ořezání koncových "\"
	while (txt.IsNotEmpty() && (txt[txt.Length() - 1] == _T('\\')))
	{
		txt.Delete(txt.Length() - 1);
		txt.TrimRight();
	}

// vytvoření adresáře
	BOOL res = ::CreateDirectory(txt, NULL);
	if (res) return TRUE;

// při neúspěchu kontrola, zda adresář již existuje
	int attrib = (int)::GetFileAttributes(txt);
	return ((attrib != -1) && (attrib & FILE_ATTRIBUTE_DIRECTORY));
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření cesty (adresáře vícestupňově)

BOOL CreatePath(CString txt)
{
// první pokus o vytvoření adresáře
	if (CreateDir(txt)) return TRUE;

// ořezání mezer
	txt.TrimLeft();
	txt.TrimRight();

// ořezání koncových "\"
	while (txt.IsNotEmpty() && (txt[txt.Length() - 1] == _T('\\')))
	{
		txt.Delete(txt.Length() - 1);
		txt.TrimRight();
	}

// adresář se bude zkracovat
	CString txt2 = txt;
	CString txt3;

// zkracování adresáře
	while (txt2.RevFind(_T('\\')) >= 0)
	{
		txt2.Delete(txt2.RevFind(_T('\\')));
		if (CreateDir(txt2)) break;
	}

// prodlužování adresáře
	while (txt2.Length() < txt.Length())
	{
		txt2 += _T('\\');
		txt3 = txt.Mid(txt2.Length(), txt.Length());
		int pos = txt3.Find(_T('\\'));
		if (pos < 0) break;
		txt2 += txt3.Left(pos);
		if (!CreateDir(txt2)) return FALSE;
	}

// vytvoření posledního stupně (to je poslední pokus)
	return CreateDir(txt);
}
