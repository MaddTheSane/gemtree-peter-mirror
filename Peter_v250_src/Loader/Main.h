
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

//#define _MT
//#undef _M_IX86

// Pro překladač nastavit úroveň varování 4 (přepínač /W4)

// U "inline" funkcí nepoužívat "bool" ale raději "BOOL" - efektivnější překlad !

//#pragma comment(linker, "/SUBSYSTEM:CONSOLE")


//////////////////////////////////////////////////////////////////////////////
// u verze MINI chybí:
//  - ukládání a načítání obrázků, zvuků, hudby, ikon

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
#endif  // _DEBUG

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

typedef unsigned __int64 QWORD;

#define MAXDWORD	0xffffffff

//////////////////////////////////////////////////////////////////////////////
// standardní vnořené sekce

#include <windows.h>					// základní definice WINDOWS
#pragma warning ( disable: 4201)		// hlášení - nepojmenovaná struktura
#include <mmsystem.h>					// multimédia
#include <aviriff.h>					// definice pro AVI
#include <winsock.h>					// WinSock
#include <windowsx.h>					// rozšířená definice WINDOWS
#include <math.h>						// matematické operace
//#include <alphaops.h>					// matematické konstanty
#include <tchar.h>						// obsluha znaků UNICODE/MB
#include <commctrl.h>					// doplňkové ovládací prvky
#include <mmreg.h>						// multimediální definice
#include "dx\ddraw.h"					// DirectDraw

#if (DIRECT3D_VERSION < 0x0500)
typedef DWORD D3DCOLORMODEL;
#endif

#if (DIRECT3D_VERSION >= 0x0800)
#include "dx\d3d8.h"					// Direct3D
#else
#include "dx\d3d.h"						// Direct3D
#endif

//#include <d3dx8.h>
#include "dx\dplay.h"					// DirectPlay
#include "dx\dsound.h"					// DirectSound
#include "dx\gl.h"						// obsluha OpenGL
//#include <glu.h>						// utility OpenGL
#include <richedit.h>					// RichEdit
#include <msacm.h>						// konverze zvuku ACM
#include "resource.h"

#define DOUBLE_INFINITY_VALUE       ((ULONGLONG)0x7ff0000000000000)

#pragma warning ( default: 4201)		// hlášení - nepojmenovaná struktura

#pragma warning ( disable: 4100)		// hlášení - nepoužitý formální parametr
#pragma warning ( disable: 4710)		// hlášení - funkce není inline

#ifndef D3DVECTOR_DEFINED
typedef struct _D3DVECTOR {
    float x;
    float y;
    float z;
} D3DVECTOR;
#define D3DVECTOR_DEFINED
#endif

typedef struct _D3DXVECTOR3
{
	float	x;
	float	y;
	float	z;
}D3DXVECTOR3;

typedef struct _D3DXVECTOR4
{
	float	x;
	float	y;
	float	z;
	float	w;
}D3DXVECTOR4;

#ifndef D3DMATRIX_DEFINED
typedef struct _D3DMATRIX {
//    union {
//        struct {
            float        _11, _12, _13, _14;
            float        _21, _22, _23, _24;
            float        _31, _32, _33, _34;
            float        _41, _42, _43, _44;

//        };
  //      float m[4][4];
//    };
} D3DMATRIX;
#define D3DMATRIX_DEFINED
#endif

#ifndef D3DCOLORVALUE_DEFINED
typedef struct _D3DCOLORVALUE {
    float r;
    float g;
    float b;
    float a;
} D3DCOLORVALUE;
#define D3DCOLORVALUE_DEFINED
#endif

//////////////////////////////////////////////////////////////////////////////
// předdefinice tříd

class CIcon; class CMap; class CMusic; class CPicture; class CSound; class CSprite;
class CString;
class CBufBool; class CBufIcon; class CBufInt; class CBufMap; class CBufMusic;
class CBufPic; class CBufReal; class CBufSound; class CBufSprite; class CBufText;
class CBufIndex; class CBuf; class CBufList;

struct PETHEAD_;

//////////////////////////////////////////////////////////////////////////////
// globální proměnné

extern	PETHEAD_*	PetAdr;				// adresa dat programu
extern	int			PetSize;			// velikost dat programu
extern	CBuf		PetBuf;				// buffer dat programu
extern	char		DATAsize[];			// buffer importů od překladače

extern	CString		CommandLine;		// příkazový řádek
extern	int			VerzeOS;			// verze systému
extern	OSVERSIONINFO	OSVersionInfo;	// informace o systému
extern	HINSTANCE	hInstance;			// instance programu
extern	int			ScreenWidth;		// šířka klientské oblasti displeje (též v režimu DirectDraw)
extern	int			ScreenHeight;		// výška klientské oblasti displeje (též v režimu DirectDraw)
extern	int			ScreenWidth0;		// šířka videomódu displeje
extern	int			ScreenHeight0;		// výška videomódu displeje
extern	int			ScreenBits;			// počet bitů na bod obrazovky
extern	int			TimerConst;			// konstanta pro časovač (pro 55 ms)

extern	DWORD		ReturnCode;			// návratový kód (pro operaci OK)

extern	BYTE*		StdPalImport;		// tabulka importu palet
extern	BITMAPINFO* StdBitmapInfo;		// standardní záhlaví BMP
extern	HPALETTE	StdPalette;			// vlastní logické palety
extern	LOGPALETTE* pStdPalette;		// tabulka vlastních logický palet (potřebné též pro DirectDraw)
extern	BYTE*		KonvPal;			// konverzní tabulka palet

//#ifndef _MINI
//extern	D3DRMPALETTEENTRY* pStdPalette2;	// tabulka palet s alfa kanálem (pro Direct3D)
//#endif // _MINI

extern	PALETTEENTRY* SysPal;			// tabulka palet pro DirectDraw 256 barev

#define	ResSysPal	20					// počet rezervovaných systémových palet (na začátku tabulky)

extern	DWORD*		StdPalExport;		// tabulka exportu vlastních barev na barvy Windows

extern	BYTE*		Shadows;			// tabulka pro generování stínu

extern	bool		Dither;				// používat dithering

extern	bool		Release;			// je distribuční verze

extern	CString		ExeFileName;		// jméno programu

#ifdef _UNICODE
typedef	BOOL (WINAPI *GETDISKFREESPACEEX) (LPCWSTR, __int64*, __int64*, __int64*);
#else
typedef	BOOL (WINAPI *GETDISKFREESPACEEX) (LPCSTR, __int64*, __int64*, __int64*);
#endif
extern GETDISKFREESPACEEX	pGetDiskFreeSpaceEx; // funkce GetDiskFreeSpaceEx (NULL=není)

extern	HWND		PrevWindow;			// předešlé aktivní okno

//////////////////////////////////////////////////////////////////////////////
// definice jazyků

enum JAZYKID {							// (identifikátor jazyku - hledej LANGID)
		JAZYKARA,						// arabsky (0x401)
		JAZYKBUL,						// bulharsky (0x402)
		JAZYKCZ,						// česky (0x405)
		JAZYKDAN,						// dánsky (0x406)
		JAZYKGER,						// německy (0x407)
		JAZYKREC,						// řecky (0x408)
		JAZYKENG,						// anglicky (0x409)
		JAZYKSPA,						// španělsky (0x40A)
		JAZYKFIN,						// finsky (0x40b)
		JAZYKFRA,						// francouzsky (0x40C)
		JAZYKHEB,						// hebrejsky (0x40d)
		JAZYKMAD,						// maďarsky (0x40e)
		JAZYKISL,						// islandsky (0x40f)
		JAZYKITA,						// italsky (0x410)
		JAZYKHOL,						// holandsky (0x413)
		JAZYKNOR,						// norsky (0x414)
		JAZYKPOL,						// polsky (0x415)
		JAZYKPOR,						// portugalsky (0x816)
		JAZYKRUM,						// rumunsky (0x418)
		JAZYKRUS,						// rusky (0x419)
		JAZYKSRB,						// srbochorvatsky (0x41a)
		JAZYKSLO,						// slovensky (0x41b)
		JAZYKALB,						// albánsky (0x41c)
		JAZYKSWE,						// šwédsky (0x41d)
		JAZYKTUR,						// turecky (0x41f)
		JAZYKVIE,						// vietnamsky (0x42a)

		JAZYKNUM						// počet jazyků
};

// jazykové informace
typedef struct JAZYKINFO_
{
	int					LangID;			// identifikátor jazyku
	int					CodePage;		// kódová stránka znaků
	int					CharSet;		// znaková sada fontů
	char*				TextTab;		// adresa začátku textů v programu (NULL=není)
	LPCTSTR				CancelTxt;		// text "Storno"
	LPCTSTR				AllFilesTxt;	// text "všechny soubory"
	LPCTSTR				MemErrNadp;		// text nadpisu okna hlášení o nedostatku paměti
	LPCTSTR				MemErrText;		// text okna hlášení o nedostatku paměti
} JAZYKINFO;

extern	int		JazykDef;				// implicitní jazyk
extern	DWORD	DefCharSet;				// implicitní znaková sada fontů podle systému

extern	int		JazykDefImp;			// implicitní importovaný jazyk

extern	int		Jazyk;					// aktuální jazyk
extern	DWORD	LangID;					// aktuální identifikátor jazyku
extern	DWORD	CharSet;				// aktuální znaková sada fontů
extern	DWORD	CodePage;				// aktuální kódová stránka
extern	char	Carka;					// oddělovač desetinné čárky při zobrazení čísel

extern	int		JazykUkaz0;				// ukazatel indexu načítaného textu
extern	char*	JazykUkaz[JAZYKNUM];	// ukazatele textů při načítání

extern JAZYKINFO JazykInfo[JAZYKNUM];	// tabulka definic jazyků

//////////////////////////////////////////////////////////////////////////////
// prázdné objekty (např. pro návrat neplatné položky z funkce)

extern	CString		EmptyString;		// prázdný řetězec
extern	CIcon		EmptyIcon;			// prázdná ikona
extern	CMap		EmptyMap;			// prázdná plocha
extern	CPicture	EmptyPicture;		// prázdný obrázek
extern	CSprite		EmptySprite;		// prázdný sprajt
extern	CSound		EmptySound;			// prázdný zvuk
extern	CMusic		EmptyMusic;			// prázdná hudba

//////////////////////////////////////////////////////////////////////////////
// globální konstanty

#define ResCols 2					// počet rezervovaných barev na počátku barev (=pozadí a stín)

//extern const int ColLev;			// počet úrovní standardních palet
#define	ColLev 6					// počet úrovní standardních palet
//extern const int ColCol;			// počet barev standardních palet
#define	ColCol 37					// počet barev standardních palet
//extern const int StdColors;			// počet vlastních palet (začínají od 0)
#define	StdColors (ResCols+ColCol*ColLev)	// počet vlastních palet (začínají od 0)

#define BackCol 0					// průhledná barva (barva pozadí)
#define ShadCol 1					// poloprůhledná barva (stín)

#define BACKCOLOR_RED	149			// červená složka barvy pozadí
#define BACKCOLOR_GREEN	34			// zelená složka barvy pozadí
#define BACKCOLOR_BLUE	140			// modrá složka barvy pozadí

#define SHADCOLOR_RED	97			// červená složka barvy stínu
#define SHADCOLOR_GREEN	30			// zelená složka barvy stínu
#define SHADCOLOR_BLUE	111			// modrá složka barvy stínu

extern const BYTE WhiteCol;			// bílá barva
extern const BYTE BlackCol;			// černá barva
extern const BYTE BlueCol;			// modrá barva
extern const BYTE RedCol;			// červená barva
extern const BYTE YellowCol;		// žlutá barva
extern const BYTE GreenCol;			// zelená barva
extern const BYTE LtYellowCol;		// světle žlutá barva
extern const BYTE OrangeCol;		// oranžová

extern const double pi;				// Ludolfovo číslo
extern const double pi2;			// Ludolfovo číslo * 2
extern const double pi8;			// Ludolfovo číslo * 8
extern const double pi2m;			// - Ludolfovo číslo * 2
extern const double pi8m;			// - Ludolfovo číslo * 8
extern const double eul;			// Eulerovo číslo

extern const double uhel22;			// úhel 22.5 stupňů v radiánech
extern const double uhel30;			// úhel 30 stupňů v radiánech
extern const double uhel45;			// úhel 45 stupňů v radiánech
extern const double uhel60;			// úhel 60 stupňů v radiánech
extern const double uhel67;			// úhel 67.5 stupňů v radiánech
extern const double uhel90;			// úhel 90 stupňů v radiánech
extern const double uhel135;		// úhel 135 stupňů v radiánech
extern const double uhel180;		// úhel 180 stupňů v radiánech
extern const double uhel225;		// úhel 215 stupňů v radiánech
extern const double uhel270;		// úhel 270 stupňů v radiánech
extern const double uhel315;		// úhel 315 stupňů v radiánech
extern const double uhel360;		// úhel 360 stupňů v radiánech

extern const double ln10;			// přirozený logaritmus 10

extern const double degrad;			// konstanta pro převod z DEG na RAD (pi/180)
extern const double raddeg;			// konstanta pro převod z RAD na DEG (180/pi)

#define BigInt 0x40000000			// velké číslo INT

extern double* FactTab;				// tabulka faktoriálů
#define	MAXFACT	170					// maximální hodnota faktoriálu

#define	CR	13						// ASCII znak CR
#define	LF	10						// ASCII znak LF

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

inline BYTE PalImport(DWORD color)
{
	if (color >= 0x60000000)
	{
		if (color >= 0x80000000)
		{
			return BackCol;
		}
		return ShadCol;
	}

	return StdPalImport[
		((color & (63*4))/4) |							// červená složka
		((color & (63*4*256))/(256*4/64)) |				// zelená složka
		((color & (63*4*256*256))/(256*256*4/64/64))];	// modrá složka
}

// export palety (zajistit rozsah barvy 0 až 255 !!!)
inline DWORD PalExport(int col)
{
	return StdPalExport[col];
}

//////////////////////////////////////////////////////////////////////////////
// ukončení aplikace

void	Exit(DWORD code);				// ukončení programu

#define	EXITCODE_MEMERR		253			// chyba paměti při běhu programu
#define EXITCODE_MAINFRAME	252			// chyba vytvoření hlavního okna
#define EXITCODE_LOADRES	251			// chyba čtení resource

#define EXITCODE_OK			0			// řádné ukončení programu


//////////////////////////////////////////////////////////////////////////////
// aktualizace informací o obrazovce (není-li celoobrazovkový mód)

void AktDispInfo0();
void AktDispInfo();


//////////////////////////////////////////////////////////////////////////////
// nastavení vyšší přesnosti procesoru

void FPUDouble();

//////////////////////////////////////////////////////////////////////////////
// nastavení nižší přesnosti procesoru

void FPUFloat();


//////////////////////////////////////////////////////////////////////////////
// vystředění okna proti jinému oknu

void CenterWindow(HWND child, HWND parent = NULL);


//////////////////////////////////////////////////////////////////////////////
// inicializace jazyku

void InitLang(int jazyk);


//////////////////////////////////////////////////////////////////////////////
// inicializace klávesnice

void InitKeyb();


//////////////////////////////////////////////////////////////////////////////
// spuštění programu

//int Exec(CString name, CString param, BOOL wait);
DWORD Exec(CString command, BOOL wait);


/////////////////////////////////////////////////////////////////////////////
// generování konverzní tabulky barev obrázku

void GenKonvPal(BITMAPINFO* bmp);


/////////////////////////////////////////////////////////////////////////////
// vygenerování střední barvy (ze 4 bodů)

BYTE _fastcall ColAvrg(BYTE col1, BYTE col2, BYTE col3, BYTE col4);


/////////////////////////////////////////////////////////////////////////////
// zjištění editačního okna s fokusem (NULL = není)

HWND GetEditFocus();


/////////////////////////////////////////////////////////////////////////////
// vytvoření adresáře (pokud existuje, je vše OK)

BOOL CreateDir(CString txt);

/////////////////////////////////////////////////////////////////////////////
// vytvoření cesty (adresáře vícestupňově)

BOOL CreatePath(CString txt);


/////////////////////////////////////////////////////////////////////////////
// dekomprese dat

void DeKomp(BYTE* dstBuf, int dstNum, BYTE* srcBuf, int srcNum);


/////////////////////////////////////////////////////////////////////////////
// generátor náhody

int rand();
double Rand();

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
// mocnina X^Y

//double Power(double x, double y);


//////////////////////////////////////////////////////////////////////////////
// pomocné konstanty

#define ICONWIDTH 32								// šířka ikon
#define ICONHEIGHT 32								// výška ikon
#define ICONSIZE (ICONWIDTH*ICONHEIGHT)				// velikost ikon v bajtech

#define MOUSEINV -100000							// neplatná souřadnice myši


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

// Při změně změnit definice i v Peter.EXE !!!!!

// příznaky typu ikony a obrázku
enum PICPARAM {
	PicParamPic,			// pouze obrázek bez pozadí
	PicParamMix,			// obrázek mixovaný s pozadím (nebo stín)
	PicParamBack,			// pouze pozadí (prázdný obrázek)
	PicParamNone,			// neznámý obsah
	PicParamComp,			// komprimovaná data (R/O - nerušit buffer!)
};

//////////////////////////////////////////////////////////////////////////////
// vlastní vnořené sekce - základní deklarace

#include "Memory.h"						// obsluha paměti
#include "Bitmap.h"						// obsluha bitmap a ikon
#include "BufText.h"					// buffer textů, textové řetězce
#include "BufIcon.h"					// buffer ikon, data ikon
#include "BufPic.h"						// buffer obrázků (pozadí)
#include "BufWin.h"						// buffer prvků oken
#include "MainFrm.h"					// hlavní okno aplikace
#include "BufInt.h"						// buffer celých čísel a logických proměnných
#include "BufList.h"					// buffer seznamů
#include "BufMap.h"						// buffer mapy plochy
#include "BufMus.h"						// buffer hudby
#include "JPEG.h"						// obsluha JPEG
#include "BufReal.h"					// buffer reálných čísel
#include "BufSnd.h"						// buffer zvuku
#include "BufSprt.h"					// buffer sprajtů
#include "BufD3D.h"						// buffer Direct3D
#include "BufXFile.h"					// buffer objektů X souboru
#include "File.h"						// obsluha souborů
#include "Compress.h"					// obsluha komprese dat
#include "Load.h"						// načítání programu

#include "D3NO.h"						// ovladač 3D - není
#include "D3GL0.h"						// ovladač 3D - OpenGL 1.0
#include "D3GL1.h"						// ovladač 3D - OpenGL 1.1
#include "D3GL2.h"						// ovladač 3D - OpenGL 2.0
#include "D3DX4.h"						// ovladač 3D - DirectX 4
#include "D3DX5.h"						// ovladač 3D - DirectX 5
#include "D3DX6.h"						// ovladač 3D - DirectX 6
#include "D3DX7.h"						// ovladač 3D - DirectX 7
#include "D3DX8.h"						// ovladač 3D - DirectX 8

//////////////////////////////////////////////////////////////////////////////
// modely funkcí

typedef void	(			*PROCCOM)	();					// příkaz
typedef double	(			*PROCNUM)	();					// číslo
typedef void	(_fastcall	*PROCTXT)	(CString& text);	// text
typedef bool	(			*PROCLOG)	();					// logický výraz
typedef void	(_fastcall	*PROCICO)	(CIcon& icon);		// ikona
typedef void	(_fastcall	*PROCMAP)	(CMap& map);		// plocha
typedef void	(_fastcall	*PROCPIC)	(CPicture& pic);	// obrázek
typedef void	(_fastcall	*PROCSPR)	(CSprite& sprite);	// sprajt
typedef	void	(_fastcall	*PROCSND)	(CSound& sound);	// zvuk
typedef void	(_fastcall	*PROCMUS)	(CMusic& music);	// hudba


/////////////////////////////////////////////////////////////////////////////
// položka bufferu programu

#pragma warning ( disable: 4201)		// hlášení - nepojmenovaná struktura

typedef struct EXECITEM_ {
	union 
	{
		PROCCOM		ExecCom;		// příkaz
		PROCNUM		ExecNum;		// číslo
		PROCTXT		ExecTxt;		// text
		PROCLOG		ExecLog;		// logický výraz
		PROCICO		ExecIco;		// ikona
		PROCMAP		ExecMap;		// plocha
		PROCPIC		ExecPic;		// obrázek
		PROCSPR		ExecSpr;		// sprajt
		PROCSND		ExecSnd;		// zvuk
		PROCMUS		ExecMus;		// hudba
	};

	union 
	{
		double		Double;			// reálná data (číselná konstanta)

		struct
		{	
			int		Data;			// data
			int		List;			// seznam
		};
	};

	int			Jump;				// offset ke zrychlenému přeskočení prvku ve skupině
} EXECITEM;

#pragma warning ( default: 4201)	// hlášení - nepojmenovaná struktura

extern	EXECITEM*	ExecItem;		// ukazatel prováděného programu


//////////////////////////////////////////////////////////////////////////////
// vlastní vnořené sekce - překlad programu

#include "Comp.h"						// kompilace programu
#include "CompCom.h"					// kompilace programu - příkaz
#include "CompIco.h"					// kompilace programu - ikona
#include "CompLog.h"					// kompilace programu - logický výraz
#include "CompMap.h"					// kompilace programu - plocha
#include "CompMus.h"					// kompilace programu - hudba
#include "CompNum.h"					// kompilace programu - číselný výraz
#include "CompPic.h"					// kompilace programu - obrázek
#include "CompSnd.h"					// kompilace programu - zvuk
#include "CompSpr.h"					// kompilace programu - sprajt
#include "CompTxt.h"					// kompilace programu - text

#include "ExecCom.h"					// provádění programu - příkaz
#include "ExecIco.h"					// provádění programu - ikona
#include "ExecLog.h"					// provádění programu - logický výraz
#include "ExecMap.h"					// provádění programu - plocha
#include "ExecMus.h"					// provádění programu - hudba
#include "ExecNum.h"					// provádění programu - číselný výraz
#include "ExecPic.h"					// provádění programu - obrázek
#include "ExecSnd.h"					// provádění programu - zvuk
#include "ExecSpr.h"					// provádění programu - sprajt
#include "ExecTxt.h"					// provádění programu - text
#include "Exec.h"						// provádění programu


/////////////////////////////////////////////////////////////////////////////
// buffery

extern	int			TextListBeg;					// index počátku globálních seznamů
extern	int			TextListEnd;					// index konce globálních seznamů

//extern	CBufProg	BufProg[PROGBUFNUM];			// buffery programu
extern	CBufReal	Real;							// buffer reálných čísel
extern	CBufText	Text;							// buffer textů
extern	CBufBool	Bool;							// buffer logických proměnných
extern	CBufIcon	Icon;							// buffer ikon
extern	CBufMap		Map;							// buffer ploch
extern	CBufPic		Picture;						// buffer obrázků
extern	CBufSprite	Sprite;							// buffer sprajtů
extern	CBufSound	Sound;							// buffer zvuků
extern	CBufMusic	Music;							// buffer hudby
extern	CBufList	List;							// buffer seznamů
extern	CBufWin		Win;							// buffer prvků oken
//extern	CBufD3DO	D3DO;							// buffer objektů Direct3D
//extern	CBufD3DF	D3DF;							// buffer rámů Direct3D

//#define	BufInt		BufProg[BufIntID]				// interní buffer
//#define	BufCls		BufProg[BufClsID]				// buffer tříd
//#define	BufObj		BufProg[BufObjID]				// globální buffer
//#define BufLoc		BufProg[BufLocID]				// lokální buffer
//#define	BufEdi		BufProg[BufEdiID]				// editační buffer
//#define	BufStr		BufProg[BufStrID]				// buffer struktur

