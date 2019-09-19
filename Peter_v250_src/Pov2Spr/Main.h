
//////////////////////////////////////////////////////////////////////////////
// přepínače preprocesoru:
//	_DEBUG ............. je debugger
//	_OPTIM ............. je optimalizace
//	_MT ................ vícevláknový režim
//	_UNICODE ........... kódování znaků UNICODE
//
//	_M_IX86 ............ procesor Intel 86
//	_M_ALPHA ........... procesor DEC Alpha
//	_M_MPPC ............ procesor Power Macintosh
//	_M_MRX000 .......... procesor MIPS
//	_M_PPC ............. procesor PowerPC


// Pro překladač nastavit úroveň varování 4 (přepínač /W4)

//////////////////////////////////////////////////////////////////////////////
// obsluha debuggeru

#ifdef _DEBUG
#ifdef _M_IX86
#define debugbreak _asm{int 3}
#else	// _M_IX86
#define debugbreak DebugBreak()
#endif	// _M_IX86
#endif	// _DEBUG

#if defined(_DEBUG) && !defined(_OPTIM)
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
#endif
#endif

//////////////////////////////////////////////////////////////////////////////
// standardní vnořené sekce

#pragma warning ( disable: 4201)		// hlášení - nepojmenovaná struktura

#include <windows.h>					// základní definice WINDOWS
#include <math.h>						// matematické operace
//#include <alphaops.h>					// matematické konstanty
#include <tchar.h>						// obsluha znaků UNICODE/MB
#include <commctrl.h>					// doplňkové ovládací prvky
//#include <mmreg.h>						// multimediální definice
#include "resource.h"

#define DOUBLE_INFINITY_VALUE       ((ULONGLONG)0x7ff0000000000000)

#pragma warning ( default: 4201)		// hlášení - nepojmenovaná struktura

#pragma warning ( disable: 4100)		// hlášení - nepoužitý formální parametr
#pragma warning ( disable: 4710)		// hlášení - funkce není inline

//////////////////////////////////////////////////////////////////////////////
// předdefinice tříd

class CPicture; class CSprite;
class CString;
class CBufPic; class CBufSprite; class CBufText;
class CFileMap;

//////////////////////////////////////////////////////////////////////////////
// globální proměnné

extern	CString		CommandLine;		// příkazový řádek
extern	int			VerzeOS;			// verze systému
extern	HINSTANCE	hInstance;			// instance programu

extern	BYTE*		StdPalImport;		// tabulka importu palet
extern	BYTE*		StdPalImportDither;	// tabulka importu palet s ditheringem
extern	BITMAPINFO* StdBitmapInfo;		// standardní záhlaví BMP
extern	HPALETTE	StdPalette;			// vlastní logické palety
extern	BYTE*		KonvPal;			// konverzní tabulka palet

extern	bool	Dither;					// použít dithering

//////////////////////////////////////////////////////////////////////////////
// prázdné objekty (např. pro návrat neplatné položky z funkce)

extern	CString		EmptyString;		// prázdný řetězec
extern	CPicture	EmptyPicture;		// prázdný obrázek
extern	CSprite		EmptySprite;		// prázdný sprajt

//////////////////////////////////////////////////////////////////////////////
// globální konstanty

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
//extern const BYTE BackCol;			// průhledná barva

//////////////////////////////////////////////////////////////////////////////
// import palet

// konverze barev
inline void KonvCopy(BYTE* dst, BYTE* src, int num)
{
	for (; num > 0; num--)
	{
		dst[0] = KonvPal[src[0]];
		dst++;
		src++;
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

// import palety s ditheringem
inline BYTE PalImportDither(BYTE red, BYTE green, BYTE blue, int x, int y)
{
	return StdPalImportDither[(red & ~3) | ((green & ~3) * 64) | ((blue & ~3) * 64 * 64) | ((x & 1) + (y & 1)*2)];
}

inline BYTE PalImportDither(int color, int x, int y)
{
	return StdPalImportDither[
		(color & (63*4)) |								// červená složka
		((color & (63*4*256))/(256/64)) |				// zelená složka
		((color & (63*4*256*256))/(256*256/64/64)) |	// modrá složka
		((x & 1) + (y & 1)*2)];
}

//////////////////////////////////////////////////////////////////////////////
// ukončení aplikace

void	Exit(int code);					// ukončení programu

#define	EXITCODE_MEMINIT	120			// chyba paměti při inicializaci
#define EXITCODE_MAINFRAME	115			// chyba vytvoření hlavního okna
#define EXITCODE_LOADRES	110			// chyba čtení resource

#define EXITCODE_INIT		100			// hranice inicializačních kódů

#define	EXITCODE_MEMERR		80			// chyba paměti při běhu programu
#define EXITCODE_OK			0			// řádné ukončení programu


/////////////////////////////////////////////////////////////////////////////
// generování konverzní tabulky barev obrázku

void GenKonvPal(BITMAPINFO* bmp);


/////////////////////////////////////////////////////////////////////////////
// vygenerování střední barvy (ze 4 bodů)

BYTE _fastcall ColAvrg(BYTE col1, BYTE col2, BYTE col3, BYTE col4);
BYTE ColAvrgDither(BYTE col1, BYTE col2, BYTE col3, BYTE col4, int x, int y);


/////////////////////////////////////////////////////////////////////////////
// dekomprese dat

void DeKomp(BYTE* dstBuf, int dstNum, BYTE* srcBuf, int srcNum);


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


//////////////////////////////////////////////////////////////////////////////
// pomocné konstanty

#define ICONWIDTH 32								// šířka ikon
#define ICONHEIGHT 32								// výška ikon
#define ICONSIZE (ICONWIDTH*ICONHEIGHT)				// velikost ikon v bajtech

//////////////////////////////////////////////////////////////////////////////
// vlastní vnořené sekce

#include "Memory.h"						// obsluha paměti
#include "Bitmap.h"						// obsluha bitmap a ikon
#include "BufPic.h"						// buffer obrázků (pozadí)
#include "BufSprt.h"					// buffer sprajtů
#include "BufText.h"					// buffer textů, textové řetězce
#include "File.h"						// obsluha souborů
#include "Compress.h"					// obsluha komprese dat
