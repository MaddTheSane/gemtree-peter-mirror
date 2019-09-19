
#include "Main.h"

// Upozornění:
// -----------
// Obrázky musí mít všechny shodné palety, nelze proto používat
// editor VC, obrázky se konvertují pomocí ALCHEMY spuštěním !KONV.BAT

/***************************************************************************\
*																			*
*							Hlavní modul aplikace							*
*																			*
\***************************************************************************/

/*=========================================================================*\
+																			+
+						Globální proměnné a konstanty						+
+																			+
\*=========================================================================*/

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

TCHAR*		CommandLine = NULL;			// příkazový řádek
int			VerzeOS;					// verze systému
OSVERSIONINFO	OSVersionInfo;			// informace o systému
HINSTANCE	hInstance = NULL;			// instance programu
int			ScreenWidth;				// šířka klientské oblasti displeje
int			ScreenHeight;				// výška klientské oblasti displeje

// všechny obrázky musí mít rozměr jako plocha okna
CPicture	Picture0;					// obrázek 0 (bez volby)
CPicture	Picture1;					// obrázek 1 (menu 1 - instalovat)
CPicture	Picture2;					// obrázek 2 (menu 2 - přidat/debrat)
CPicture	Picture3;					// obrázek 3 (menu 3 - odinstalovat)
CPicture	Picture4;					// obrázek 4 (menu 4 - konec)
CPicture	Shadow;						// stín
CPicture	InstalOK;					// obrázek instalace OK
CPicture	DInstalOK;					// obrázek odinstalace OK
CPicture	DInstal;					// obrázek odinstalace

// pozor - vlajky musí mít šířku násobky 8 !!!! (kvůli zobrazení)
CPicture	FlagCZ;						// česká vlajka
CPicture	FlagENG;					// anglická vlajka

int			KeyDisp = 0;				// zobrazený obrázek volby hlavního okna (0 až 4)

int			GemtreeFaze = 0;			// fáze aktivního obrázku
CPicture	GemLogo;					// obrázek znaku Gemtree (90 obrázků horizontálně s rozměry 40x40)

BITMAPINFO* StdBitmapInfo;				// standardní záhlaví BMP
HPALETTE	StdPalette = NULL;			// vlastní logické palety

DWORD		BackColCol = 0;				// barva pozadí
HBRUSH		BackBrush = NULL;			// štětec pozadí

TCHAR		MainFrameClass[] = _T("PeterSetupClass");	// název třídy hlavního okna
HWND		MainFrame = NULL;			// hlavní okno aplikace

CText		PeterMainFrameClass(_T("PeterMainFrameClass"));	// název třídy hlavního okna editoru Petr
CText		PeterSetupClass(MainFrameClass);	// název třídy instalátoru Petra

BOOL		Dither = FALSE;

int			MainFrameWidth = 620;		// šířka hlavního okna
int			MainFrameHeight = 420;		// výška hlavního okna
#define		ClientWidth 608				// šířka klientské oblasti (musí to být násobek 8 a stejné jako Picture)
#define		ClientHeight 384			// výška klientské oblasti (musí být stejné jako Picture)
BYTE*		ClientBuf = NULL;			// buffer k zobrazení obrázku v klientské oblasti

int			OffX = MOUSEINV;			// offset stínu X (MOUSEINV = neplatné)
int			OffY = MOUSEINV;			// offset stínu Y (MOUSEINV = neplatné)

int			RelX = 0;					// kurzor myši relativně v okně X
int			RelY = 0;					// kurzor myši relativně v okně Y

int			Waiting = 0;				// příznak zobrazení kurzoru čekání

#define		TimerID 1234				// ID časovače
int			Timer = 0;					// časovač

HCURSOR		CurArrow;					// (standardní) šipka
HCURSOR		CurWait;					// čekání
HCURSOR		CurPeter;					// kurzor Petr

int			KeySelect = -1;				// výběr volby na hlavní stránce klávesnicí (-1 = není)

int			AktPage = PAGELANG;			// aktivní stránka instalátoru

__int64	DiskSize = 0;					// velikost disku (z funkce GetDiskSpace)
__int64	DiskFree = 0;					// volné místo disku (z funkce GetDiskSpace)
__int64	DiskFreeUser = 0;				// volné místo uživatele (z funkce GetDiskSpace)

int			DiskFre = 0;				// volné místo cílového disku v MB
int			DiskReq = 0;				// požadované volné místo v MB

GETDISKFREESPACEEX	pGetDiskFreeSpaceEx = NULL;	// funkce GetDiskFreeSpaceEx (NULL=není)

CText		DelExe;						// spuštění programu při ukončení (Empty=není)

CText	InstPath;						// instalační cesta

//////////////////////////////////////////////////////////////////////////////
// jazyky

#define	JAZYKENG	0					// anglicky
#define	JAZYKCZ		1					// česky

int		Jazyk = JAZYKENG;				// vybraný jazyk

//////////////////////////////////////////////////////////////////////////////
// prvky okna instalátoru

HIMAGELIST CheckList = NULL;			// obrázky tlačítek

HWND	ButtonBack = NULL;				// tlačítko zpět
HWND	ButtonNext = NULL;				// tlačítko další
HWND	ButtonExit = NULL;				// tlačítko ukončení
HWND	LineWnd = NULL;					// čára nad tlačítky
HWND	CheckWnd = NULL;				// okno přepínačů
HWND	CheckNadp = NULL;				// nadpis přepínačů
HWND	SmlouvaWnd = NULL;				// okno licenční smlouvy
HWND	SmlouvaNadp = NULL;				// nadpis nad smlouvou
HWND	PathWndNadp = NULL;				// instalační cesta - nadpis
HWND	PathWnd = NULL;					// instalační cesta
HWND	ButtonList = NULL;				// tlačítko Procházet
HWND	UserRam = NULL;					// rám uživatele
HWND	LicTextNadp = NULL;				// licenční text - nadpis
HWND	LicTextNum = NULL;				// licenční text - číslo
HWND	LicTextName = NULL;				// licenční text - jméno
HWND	LicTextAdr = NULL;				// licenční text - adresa
HWND	LicTextMest = NULL;				// licenční text - město
HWND	LicTextIco = NULL;				// licenční text - IČO
HWND	DiskReqNadp = NULL;				// požadované volné místo - text
HWND	DiskReqNum = NULL;				// požadované volné místo - číslo
HWND	DiskFreeNadp = NULL;			// volné místo cílového disku - text
HWND	DiskFreeNum = NULL;				// volné místo cílového disku - číslo
HWND	ButtonCanc = NULL;				// tlačítko přerušení
HWND	IndWnd = NULL;					// indikátor instalace
HWND	IndWndNadp = NULL;				// indikátor instalace - nadpis
HWND	IndWndFile = NULL;				// indikátor instalace - soubor
HWND	IndWnd000 = NULL;				// indikátor - 0 %
HWND	IndWnd025 = NULL;				// indikátor - 25 %
HWND	IndWnd050 = NULL;				// indikátor - 50 %
HWND	IndWnd075 = NULL;				// indikátor - 75 %
HWND	IndWnd100 = NULL;				// indikátor - 100 %
HWND	FlagCZWnd = NULL;				// tlačítko Česky
HWND	FlagENGWnd = NULL;				// tlačítko Anglicky

enum {
	IDN_BUTTONBACK = 13000,				// tlačítko Zpět
	IDN_BUTTONNEXT,						// tlačítko Další
	IDN_BUTTONEXIT,						// tlačítko Konec
	IDN_LINEWND,						// čára nad tlačítky
	IDN_CHECKWND,						// okno přepínačů
	IDN_CHECKNADP,						// nadpis přepínačů
	IDN_SMLOUVAWND,						// licenční smlouva
	IDN_SMLOUVANADP,					// nadpis licenční smlouvy
	IDN_PATHWNDNADP,					// instalační cesta - nadpis
	IDN_PATHWND,						// instalační cesta
	IDN_BUTTONLIST,						// tlačítko Procházet
	IDN_USERRAM,						// rám uživatele
	IDN_LICTEXTNADP,					// licenční text - nadpis
	IDN_LICTEXTNUM,						// licenční text - číslo
	IDN_LICTEXTNAME,					// licenční text - jméno
	IDN_LICTEXTADR,						// licenční text - adresa
	IDN_LICTEXTMEST,					// licenční text - město
	IDN_LICTEXTICO,						// licenční text - IČO
	IDN_DISKREQNADP,					// požadované volné místo - text
	IDN_DISKREQNUM,						// požadované volné místo - číslo
	IDN_DISKFREENADP,					// volné místo cílového disku - text
	IDN_DISKFREENUM,					// volné místo cílového disku - číslo
	IDN_BUTTONCANC,						// tlačítko přerušení
	IDN_INDWND,							// indikátor instalace
	IDN_INDWNDNADP,						// indikátor instalace - nadpis
	IDN_INDWNDFILE,						// indikátor instalace - soubor
	IDN_INDWND000,						// indikátor instalace - 0%
	IDN_INDWND025,						// indikátor instalace - 25%
	IDN_INDWND050,						// indikátor instalace - 50%
	IDN_INDWND075,						// indikátor instalace - 75%
	IDN_INDWND100,						// indikátor instalace - 100%
	IDN_FLAGWNDCZ,						// tlačítko Česky
	IDN_FLAGWNDENG,						// tlačítko Anglicky
};

//////////////////////////////////////////////////////////////////////////////
// přepínače instalovaných částí

CText		ExeFileName;				// jméno tohoto programu SETUP.EXE
HANDLE		HFile = NULL;				// handle souboru SETUP.EXE
INSTHEAD	Head;						// záhlaví instalačních dat

BOOL		Uninst = FALSE;				// je odinstalátor Uninstal.exe

int			RawData = 0;				// offset začátku dat v souboru
int			RawSize = 0;				// velikost dat v souboru
int			DataOff = 0;				// offset začátku dat
int			VirtAdr = 0;				// virtuální adresa k načtení dat

int			SizeOff = 0;				// offset velikosti souboru v záhlaví
int			SizeImageOff = 0;			// offset velikosti obrazu programu v záhlaví
int			VirtSizeOff = 0;			// offset virtuální velikosti dat v záhlaví
int			GDIModeOff = 0;				// offset příznaku GDI módu programu v záhlaví

#define		PROGMAX		128				// rozsah indikátoru
int			DataSize = 0;				// výchozí velikost instalovaných dat
int			DataSizeOK = 0;				// velikost zpracovaných instalovaných dat
BOOL		Storno = FALSE;				// požadavek přerušení operace instalace

typedef struct GROUPITEM_ {
	LPCTSTR name;						// jméno přepínače volby
	bool	initcheck;					// inicializační příznak volby části
	bool	check;						// příznak volby části
	bool	instal;						// příznak, že prvek je nainstalován
	BYTE*	files;						// seznam souborů
} GROUPITEM;

#ifdef MINI
GROUPITEM GroupsCZ[GROUPSNUM] =
{
	_T("hlavní program aplikace Petr"),		TRUE,	TRUE,	FALSE,	NULL,
	_T("příklady vytvořených programů"),	TRUE,	TRUE,	FALSE,	NULL,
	_T("ukázkové knihovny"),				TRUE,	TRUE,	FALSE,	NULL,
};

GROUPITEM GroupsEN[GROUPSNUM] =
{
	_T("main program of the Peter application"),TRUE,	TRUE,	FALSE,	NULL,
	_T("examples of created programs"),		TRUE,	TRUE,	FALSE,	NULL,
	_T("example libraries"),				TRUE,	TRUE,	FALSE,	NULL,
};
#else // MINI
GROUPITEM GroupsCZ[GROUPSNUM] =
{
	_T("hlavní program aplikace Petr"),		TRUE,	TRUE,	FALSE,	NULL,
	_T("příklady vytvořených programů"),	TRUE,	TRUE,	FALSE,	NULL,
	_T("knihovna předmětů (ikon)"),			TRUE,	TRUE,	FALSE,	NULL,
	_T("knihovna obrázků"),					TRUE,	TRUE,	FALSE,	NULL,
	_T("knihovna sprajtů"),					TRUE,	TRUE,	FALSE,	NULL,
	_T("knihovna zvuků"),					TRUE,	TRUE,	FALSE,	NULL,
	_T("knihovna hudeb"),					TRUE,	TRUE,	FALSE,	NULL,
	_T("ostatní knihovny"),					TRUE,	TRUE,	FALSE,	NULL,
	_T("zdroje pro generování sprajtů"),	TRUE,	TRUE,	FALSE,	NULL,
};

GROUPITEM GroupsEN[GROUPSNUM] =
{
	_T("main program of the Peter application"),TRUE,	TRUE,	FALSE,	NULL,
	_T("examples of created programs"),		TRUE,	TRUE,	FALSE,	NULL,
	_T("item library (icons)"),				TRUE,	TRUE,	FALSE,	NULL,
	_T("picture library"),					TRUE,	TRUE,	FALSE,	NULL,
	_T("sprite library"),					TRUE,	TRUE,	FALSE,	NULL,
	_T("sound library"),					TRUE,	TRUE,	FALSE,	NULL,
	_T("music library"),					TRUE,	TRUE,	FALSE,	NULL,
	_T("other libraries"),					TRUE,	TRUE,	FALSE,	NULL,
	_T("sources to generate sprites"),		TRUE,	TRUE,	FALSE,	NULL,
};
#endif // MINI

GROUPITEM*	Groups = GroupsEN;

CText ProductKey0(_T("Software\\Gemtree Software\\Peter"));
CText ProductKey(_T("Software\\Gemtree Software\\Peter\\" VerzeTxt));
CText UninstalKey0(_T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall"));
CText UninstalKey(_T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Peter" VerzeTxt));

BOOL	FirstInst = TRUE;				// je první instalace

/*=========================================================================*\
+																			+
+							Hlavní start programu							+
+																			+
\*=========================================================================*/

//////////////////////////////////////////////////////////////////////////////
// rozlišení textu podle jazyku

LPCTSTR X(LPCTSTR cesky, LPCTSTR anglicky)
{
	switch (Jazyk)
	{
	case JAZYKCZ:
		return cesky;

	default:
		return anglicky;
	}
}

CText X(const CText& cesky, const CText& anglicky)
{
	switch (Jazyk)
	{
	case JAZYKCZ:
		return cesky;

	default:
		return anglicky;
	}
}

int X(int cesky, int anglicky)
{
	switch (Jazyk)
	{
	case JAZYKCZ:
		return cesky;

	default:
		return anglicky;
	}
}

//////////////////////////////////////////////////////////////////////////////
// sestavení obrázků pro aktivní jazyk

void InitPic()
{
// příprava skupiny přepínačů
	switch (Jazyk)
	{
	case JAZYKCZ:
		Groups = GroupsCZ;
		break;

	default:
		Groups = GroupsEN;
	}

// nastavení titulku okna
	CText txt(X(_T("Instalátor aplikace Petr "), _T("Installator of the Peter Application ")));
	txt += VerzeTxt;
	txt.SetWindowText(MainFrame);

// příprava prázdného obrázku plochy
	Picture0.Clear(ClientWidth, ClientHeight);

// přidání obrázku Petra
	Picture0.AddCopy(IDN_PETR, 25, 237);

// přidání titulku
	Picture0.AddCopy(X(IDN_TITLE_CZ, IDN_TITLE_EN), 170, 230);

// dekódování čísla verze (x = 496, y = 326, šířka číslice = 12. výška číslice = 16)
#define VERZEX 496
#define VERZEY 325
	CPicture pic;
	pic.Load(IDN_DIGIT);
	Picture0.AddCopy(pic, VERZEX,			VERZEY, 12, 16, VerzeMaj*12, 0);
	Picture0.AddCopy(pic, VERZEX+12,		VERZEY, 12, 16,	      10*12, 0);
	Picture0.AddCopy(pic, VERZEX+12+8,		VERZEY, 12, 16, VerzeMin*12, 0);
	Picture0.AddCopy(pic, VERZEX+12+8+12,	VERZEY, 12, 16, VerzeRel*12, 0);

// přidání linky
	Picture0.AddCopy(IDN_LINKA, 0, 213);

// přidání textu Gemtree
	Picture0.AddCopy(IDN_GEMTREE, 30, 160);

// přidání adresy
	Picture0.AddCopy(X(IDN_ADR_CZ, IDN_ADR_EN), 0, 74);

// přidání telefonu
	Picture0.AddCopy(X(IDN_PHONE_CZ, IDN_PHONE_EN), 0, 0);

// přidání voleb
#define MENX	230
#define MENY	22
#define	MENDY	50
	switch (Jazyk)
	{
	case JAZYKCZ:
		Picture0.AddCopy(FirstInst ? IDN_INST0_CZ : IDN_INST2_CZ, MENX, MENY + 3*MENDY);
		Picture0.AddCopy(FirstInst ? IDN_ADD2_CZ  : IDN_ADD0_CZ,  MENX, MENY + 2*MENDY);
		Picture0.AddCopy(FirstInst ? IDN_UINS2_CZ : IDN_UINS0_CZ, MENX, MENY + 1*MENDY);
		Picture0.AddCopy(IDN_END0_CZ,  MENX, MENY + 0*MENDY);
		break;

	default:
		Picture0.AddCopy(FirstInst ? IDN_INST0_EN : IDN_INST2_EN, MENX, MENY + 3*MENDY);
		Picture0.AddCopy(FirstInst ? IDN_ADD2_EN  : IDN_ADD0_EN,  MENX, MENY + 2*MENDY);
		Picture0.AddCopy(FirstInst ? IDN_UINS2_EN : IDN_UINS0_EN, MENX, MENY + 1*MENDY);
		Picture0.AddCopy(IDN_END0_EN,  MENX, MENY + 0*MENDY);
	}

// konverze stínu
	{
		Shadow.New((int)(Picture0.Width()*1.052+0.5),(int)(Picture0.Height()*1.052+0.5));

		for (int yd = Shadow.Height()-1; yd >= 0; yd--)
		{
			int ys = yd*Picture0.Height()/Shadow.Height();
			if (ys < 0) ys = 0;
			if (ys >= Picture0.Height()) ys = Picture0.Height()-1;

			for (int xd = Shadow.Width()-1; xd >= 0; xd--)
			{
				int xs = xd*Picture0.Width()/Shadow.Width();
				if (xs < 0) xs = 0;
				if (xs >= Picture0.Width()) xs = Picture0.Width()-1;

				BYTE* src = Picture0.DataData() + (xs + ys*Picture0.Width());
				BYTE* dst = Shadow.DataData() + (xd + yd*Shadow.Width());

				BYTE col = *src;
				if (col == BackCol)
				{
					*dst = col;
				}
				else
				{
					*dst = ShadCol;
				}
			}
		}
	}

// vypnuté přepínače
#define SWCX	193
#define SWCY	19
#define SWCDY	50
	Picture0.AddCopy(IDN_SWCOFF, SWCX, SWCY + 0*SWCDY);
	Picture0.AddCopy(IDN_SWCOFF, SWCX, SWCY + 1*SWCDY);
	Picture0.AddCopy(IDN_SWCOFF, SWCX, SWCY + 2*SWCDY);
	Picture0.AddCopy(IDN_SWCOFF, SWCX, SWCY + 3*SWCDY);

// kopie do ostatních obrázků menu
	Picture1 = Picture0;
	Picture2 = Picture0;
	Picture3 = Picture0;
	Picture4 = Picture0;

// přepínače ostatních obrázků menu
	if (FirstInst) Picture1.AddCopy(IDN_SWCON, SWCX, SWCY + 3*SWCDY);
	if (!FirstInst) Picture2.AddCopy(IDN_SWCON, SWCX, SWCY + 2*SWCDY);
	if (!FirstInst) Picture3.AddCopy(IDN_SWCON, SWCX, SWCY + 1*SWCDY);
	Picture4.AddCopy(IDN_SWCON, SWCX, SWCY + 0*SWCDY);

// volby ostatních obrázků menu
	switch (Jazyk)
	{
	case JAZYKCZ:
		Picture1.AddCopy(FirstInst ? IDN_INST1_CZ : IDN_INST3_CZ, MENX, MENY + 3*MENDY);
		Picture2.AddCopy(FirstInst ? IDN_ADD3_CZ  : IDN_ADD1_CZ,  MENX, MENY + 2*MENDY);
		Picture3.AddCopy(FirstInst ? IDN_UINS3_CZ : IDN_UINS1_CZ, MENX, MENY + 1*MENDY);
		Picture4.AddCopy(IDN_END1_CZ,  MENX, MENY + 0*MENDY);
		break;

	default:
		Picture1.AddCopy(FirstInst ? IDN_INST1_EN : IDN_INST3_EN, MENX, MENY + 3*MENDY);
		Picture2.AddCopy(FirstInst ? IDN_ADD3_EN  : IDN_ADD1_EN,  MENX, MENY + 2*MENDY);
		Picture3.AddCopy(FirstInst ? IDN_UINS3_EN : IDN_UINS1_EN, MENX, MENY + 1*MENDY);
		Picture4.AddCopy(IDN_END1_EN,  MENX, MENY + 0*MENDY);
	}

// přidání výběrových kurzorů
#define SELX	173
#define SELY	11
#define SELDY	50
	Picture1.AddCopy(IDN_SELECT, SELX,	SELY + 3*SELDY);
	Picture2.AddCopy(IDN_SELECT, SELX,	SELY + 2*SELDY);
	Picture3.AddCopy(IDN_SELECT, SELX,	SELY + 1*SELDY);
	Picture4.AddCopy(IDN_SELECT, SELX,	SELY + 0*SELDY);

// nápověda k volbám
#define HELPX	473
	Picture1.AddCopy(X(IDN_SEL1_CZ, IDN_SEL1_EN), HELPX, 157);
	Picture2.AddCopy(X(IDN_SEL2_CZ, IDN_SEL2_EN), HELPX, 99);
	Picture3.AddCopy(X(IDN_SEL3_CZ, IDN_SEL3_EN), HELPX, 58);
	Picture4.AddCopy(X(IDN_SEL4_CZ, IDN_SEL4_EN), HELPX, 13);

// obrázek "Instalace dokončena"
	InstalOK.Load(IDN_INSOK);
	InstalOK.AddCopy(X(IDN_INSOK_CZ, IDN_INSOK_EN), 0, 0);

// obrázek "Chcete odinstalovat?"
	DInstal.Load(IDN_DINST);
	DInstal.AddCopy(X(IDN_DINST_CZ, IDN_DINST_EN), 0, 0);

// obrázek "Odinstalování dokončeno"
	DInstalOK.Load(IDN_DINOK);
	DInstalOK.AddCopy(X(IDN_DINOK_CZ, IDN_DINOK_EN), 0, 0);
}


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

/////////////////////////////////////////////////////////////////////////////
// načtení cesty Program Files

CText GetProgramFiles()
{
// text výsledné hodnoty
	CText txt;

// otevření klíče
	HKEY hkey;
	if (::RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion"),
		0,
		KEY_QUERY_VALUE,
		&hkey) == ERROR_SUCCESS)
	{

// načtení hodnoty klíče
		DWORD type = REG_SZ;
#define REGBUFSIZE 300
		BYTE buf[REGBUFSIZE+1];
		DWORD bufsize = REGBUFSIZE;

		if ((::RegQueryValueEx(
			hkey,
			_T("ProgramFilesDir"),
			0,
			&type,
			buf,
			&bufsize) == ERROR_SUCCESS) &&
			(type == REG_SZ) &&
			(bufsize > 0) &&
			(bufsize <= REGBUFSIZE))
		{
			txt.Set((char*)buf, bufsize-1);
			txt.KorigNul();
			txt.TrimLeft();
			txt.TrimRight();
		}

// uzavření klíče
		::RegCloseKey(hkey);
	}

	return txt;
}

//////////////////////////////////////////////////////////////////////////////
// hlášení chyby

#ifdef _DEBUG
void Hlaseni(LPCTSTR text)
{
	::MessageBox(MainFrame, text, NULL, MB_OK);
}
#else
#define Hlaseni(f) ((void)0)
#endif

//////////////////////////////////////////////////////////////////////////////
// obsluha enumerace oken

BOOL PeterRun = FALSE;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	TCHAR buf[64];
	CText name(buf, ::GetClassName(hwnd, buf, 64));

	if ((name == PeterMainFrameClass) || (name == PeterSetupClass))
	{
		PeterRun = TRUE;
		return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// hlavní start programu

void WinMainCRTStartup()
{
// úschova adresy příkazového řádku
	CommandLine = ::GetCommandLine();

// úschova verze systému
	VerzeOS = (int)::GetVersion();
	OSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&OSVersionInfo);

// handle instance programu
	hInstance = ::GetModuleHandle(NULL);

// nastavení chybového módu
	::SetErrorMode(::SetErrorMode(0) | SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX); 

// příprava implicitního jazyku
	switch (::GetUserDefaultLangID() & 0xff)
	{
	case 5:				// česky
	case 21:			// polsky
	case 27:			// slovensky
		Jazyk = JAZYKCZ;
	}

// inicializace informací o displeji
	ScreenWidth = ::GetSystemMetrics(SM_CXFULLSCREEN);
	ScreenHeight = ::GetSystemMetrics(SM_CYFULLSCREEN);

// inicializace správce paměti
	if (!MemInit()) 
	{
MEMERROR:
		::MessageBox(NULL, 
			X(_T("Nedostatek paměti ke spuštění instalátoru."),
				_T("Insufficient memory to run installator.")),
			X(_T("Chyba paměti"), _T("Memory Error")),
			MB_OK | MB_ICONSTOP);
		Exit();
	}

// inicializace funkce GetDiskFreeSpaceEx pro OSR2
	if (((OSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
		(LOWORD(OSVersionInfo.dwBuildNumber) > 1000)) ||
		(OSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT))
	{
		HINSTANCE hinst = ::LoadLibrary(_T("KERNEL32.DLL"));

		if (hinst)
		{
#ifdef _UNICODE
			pGetDiskFreeSpaceEx = (GETDISKFREESPACEEX)
				::GetProcAddress(hinst,	"GetDiskFreeSpaceExW");
#else
			pGetDiskFreeSpaceEx = (GETDISKFREESPACEEX)
				::GetProcAddress(hinst,	"GetDiskFreeSpaceExA");
#endif
		}
	}

// inicializace koprocesoru
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

// inicializace obsluhy obrázků
	if (!InitPicture()) goto MEMERROR;

// inicializace globálních objektů
	InitTerm(__xc_a, __xc_z);

// test, zda není spuštěn editor Petr (např. při zobrazení knihovny na CD)
	::EnumWindows(EnumWindowsProc, 0);
	if (PeterRun) Exit();

// upřesnění jazyku
//	CText txt = GetReg(ProductKey0, _T("SetupLang"));
//	if ((txt == "5") || (txt == "21") || (txt == "27")) Jazyk = JAZYKCZ;
//	if ((txt == "7") || (txt == "9") || (txt == "12")) Jazyk = JAZYKENG;

	CText txt = GetReg(ProductKey, _T("SetupLang"));
	if ((txt == "5") || (txt == "21") || (txt == "27")) Jazyk = JAZYKCZ;
	if ((txt == "7") || (txt == "9") || (txt == "12")) Jazyk = JAZYKENG;

// načtení knihovny RichEdit
	::LoadLibrary(_T("RICHED32.DLL"));

// vytvoření bufferu k zobrazení obrázků
	ClientBuf = (BYTE*)MemGet(ClientWidth * ClientHeight);

// standardní záhlaví bitmap
	CResource pic;
	pic.Open(IDN_PETR, RT_BITMAP);
	StdBitmapInfo = (BITMAPINFO*)MemGet(sizeof(BITMAPINFO) + sizeof(RGBQUAD)*255);
	MemCopy(StdBitmapInfo, pic.Adr(), sizeof(BITMAPINFO) + sizeof(RGBQUAD)*255);
	StdBitmapInfo->bmiHeader.biCompression = 0;

	BackColCol = StdBitmapInfo->bmiColors[BackCol].rgbRed + StdBitmapInfo->bmiColors[BackCol].rgbGreen*256
				+ StdBitmapInfo->bmiColors[BackCol].rgbBlue*256*256;
	BackBrush = ::CreateSolidBrush(BackColCol);

// vytvoření vlastních logických palet
	LOGPALETTE* pStdPalette = (LOGPALETTE*) MemGet(sizeof(PALETTEENTRY)*(256-1)+sizeof(LOGPALETTE));
	MemFill(pStdPalette, sizeof(PALETTEENTRY)*(256-1)+sizeof(LOGPALETTE), 0);
	pStdPalette->palVersion = 0x300;
	pStdPalette->palNumEntries = 250;
	PALETTEENTRY* pal = pStdPalette->palPalEntry;
	RGBQUAD* rgb = StdBitmapInfo->bmiColors;
	int i;
	for (i = 255; i >= 0; i--)
	{
		pal->peRed = rgb->rgbRed;
		pal->peGreen = rgb->rgbGreen; 
		pal->peBlue = rgb->rgbBlue;
		pal->peFlags = 0;
		pal++;
		rgb++;
	}
	StdPalette = ::CreatePalette(pStdPalette);

// kurzory
	CurPeter = ::LoadCursor(hInstance, MAKEINTRESOURCE(IDC_PETER));
	CurArrow = ::LoadCursor(0, MAKEINTRESOURCE(IDC_ARROW));
	CurWait = ::LoadCursor(0, MAKEINTRESOURCE(IDC_WAIT));

// zajištění knihovny rozšířených ovládacích prvků
	::InitCommonControls();

// obrázek checkboxů
	CheckList = ::ImageList_LoadImage(hInstance, MAKEINTRESOURCE(IDN_CHECKBOX), 20, 10, CLR_NONE, IMAGE_BITMAP, LR_DEFAULTCOLOR);

// vytvoření hlavního okna aplikace
	if (!MainFrameCreate()) 
	{
//RESERROR:
		::MessageBox(NULL, 
			X(_T("Nedostatek systémových prostředků ke spuštění instalátoru."),
				_T("Insufficient system resources to run installator.")),
			X(_T("Chyba systému"), _T("System Error")),
			MB_OK | MB_ICONSTOP);
		Exit();
	}

// načtení cesty
	InstPath = GetReg(ProductKey, _T("Path"));

// test, zda je první instalace
	if (InstPath.IsNotEmpty())
	{
		FirstInst = FALSE;

// načtení přepínačů
		CText grpnum;
		CText res;
		for (i = 0; i < GROUPSNUM; i++)
		{
			grpnum.Int(i);
			bool val = ((GetReg(ProductKey, _T("Group") + grpnum) == _T('1')) != FALSE);
			GroupsCZ[i].check = val;
			GroupsCZ[i].instal = val;

			GroupsEN[i].check = val;
			GroupsEN[i].instal = val;
		}

// úschova instalační cesty
		SetReg(ProductKey0, _T("LastPath"), InstPath);
	}
	else
	{
		// cesta k programům
		InstPath = GetProgramFiles();
		if (InstPath.IsEmpty()) InstPath = _T("C:\\Program Files\\");
		if (InstPath.LastChar() != _T('\\')) InstPath += _T('\\');

		InstPath += _T("Peter");
	}


// příprava obrázků
	FlagCZ.Load(IDN_FLAGCZ);
	FlagENG.Load(IDN_FLAGENG);

	GemLogo.Clear(90*40, 40);
	GemLogo.AddCopy(IDN_GEMLOGO1, 0*18*40, 0);
	GemLogo.AddCopy(IDN_GEMLOGO2, 1*18*40, 0);
	GemLogo.AddCopy(IDN_GEMLOGO3, 2*18*40, 0);
	GemLogo.AddCopy(IDN_GEMLOGO4, 3*18*40, 0);
	GemLogo.AddCopy(IDN_GEMLOGO5, 4*18*40, 0);

	InitPic();

// příprava jména programu
	TCHAR buf[_MAX_PATH];
	buf[0] = 0;
	::GetModuleFileName(hInstance, buf, _MAX_PATH);
	ExeFileName = buf;
	if ((int)::GetFileAttributes(ExeFileName) == -1)
	{
		ExeFileName = _T("Setup.exe");
	}

#ifdef _DEBUG
	ExeFileName = "c:\\vc\\Peter\\Gener\\Setup.exe";

	if ((int)::GetFileAttributes(ExeFileName) == -1)
	{
		Hlaseni("Nenalezen soubor c:\\vc\\Peter\\Gener\\Setup.exe");
	}

#endif // _DEBUG

// rozlišení instalátoru podle jména (to je pěkná blbost!)
	Uninst = FALSE;

	CText ttt;

	ttt = ExeFileName.Right(10);
	ttt.UpperCase();
	if (ttt == _T("UNINST.EXE")) Uninst = TRUE;
	if (ttt == _T("DEINST.EXE")) Uninst = TRUE;
	if (ttt == _T("REMOVE.EXE")) Uninst = TRUE;

	ttt = ExeFileName.Right(12);
	ttt.UpperCase();
	if (ttt == _T("UNINSTAL.EXE")) Uninst = TRUE;
	if (ttt == _T("DEINSTAL.EXE")) Uninst = TRUE;
	if (ttt == _T("DINSTALL.EXE")) Uninst = TRUE;
	if (ttt == _T("UINSTALL.EXE")) Uninst = TRUE;

// test, zda nebyla chyba paměti
	if (MemoryError) goto MEMERROR;

// vytvoření časovače
	Timer = ::SetTimer(MainFrame, TimerID, 55, NULL);

// zapnutí hlavní stránky
	if (Uninst)
	{
		if (FirstInst)
		{
			SetPage(PAGEDINSTOK);
		}
		else
		{
			SetPage(PAGEISDINST);

			if (!OpenSetup())
			{
				Exit();
			}
		}
	}
	else
	{
		if (FirstInst)
		{
			SetPage(PAGELANG);
		}
		else
		{
			SetPage(PAGEMAIN);
		}
	}

// hlavní obslužná smyčka aplikace
	MSG msg;

	while (::GetMessage(&msg, NULL, 0, 0))
	{
		MemoryError = FALSE;

		if (!PreTranslateMessage(&msg))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

// zrušení časovače
	::KillTimer(MainFrame, Timer);

// konec programu
	Exit();
};

//////////////////////////////////////////////////////////////////////////////
// ukončení programu

void Exit()
{
	if (DelExe.IsNotEmpty())
	{
		Exec(DelExe, EmptyText, FALSE);
	}

// zrušení palet
	::DeleteObject(StdPalette);

// ukončení správce paměti
	MemTerm();

// ukončení programu
	ExitProcess(0);
}


//////////////////////////////////////////////////////////////////////////////
// spuštění programu

int Exec(CText command, CText aktdir, BOOL wait)
{
	int result = 0;
	command.TrimLeft();
	if (command.IsEmpty()) return 255;

// korekce aktivního adresáře
	if (aktdir.Length() > 1)
	{
		if (aktdir.LastChar() == '\\')
		{
			aktdir.Delete(aktdir.Length()-1);
		}
	}

// inicializace startovací tabulky
	STARTUPINFO si;
	MemFill(&si, sizeof(si), 0);
	si.cb = sizeof(si);

// inicializace tabulky procesu
	PROCESS_INFORMATION pi;
	MemFill(&pi, sizeof(pi), 0);

// spuštění programu
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
		| CREATE_DEFAULT_ERROR_MODE,
		NULL,								// prostředí
		aktdir.IsEmpty() ? NULL : (LPCTSTR)aktdir, // výchozí adresář
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

	return result;
}


//////////////////////////////////////////////////////////////////////////////
// průběžné zpracování zpráv

void PumpMessage()
{
	MSG msg;

	while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		MemoryError = FALSE;

		if (!PreTranslateMessage(&msg))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// hlášení chyby čtení ze souboru SETUP.EXE (TRUE=přerušit)

BOOL ReadError(CText jmeno)
{
	CText txt;
	if (Uninst)
	{
		txt = X(_T(	"Chyba čtení odinstalačních dat.\n"
					"Soubor UNINSTAL.EXE je poškozen."),

				_T(	"Error reading uninstallation data.\n"
					"File UNINSTAL.EXE is damaged."));
	}
	else
	{
		txt = X(_T(	"Chyba čtení instalačních dat ze souboru ") + jmeno + _T(".\n"
					"Pokud byl instalační CD-ROM vyjmut z mechaniky,\n"
					"zasuňte jej, prosím, zpět. Jinak se zřejmě jedná\n"
					"o poškozený instalační soubor."),

				_T(	"Error reading installation data from file ") + jmeno + _T(".\n"
					"If installation CD-ROM was removed from the drive,\n"
					"insert it back. Otherwise the installation file\n"
					"is maybe damaged."));
	}

	return (::MessageBox(MainFrame,
				txt,
				X(_T("Chyba čtení instalačních dat"), _T("Error Reading Installation Data")),
				MB_RETRYCANCEL | MB_ICONERROR) != IDRETRY);
}


/////////////////////////////////////////////////////////////////////////////
// načtení informací o souborech (vrací FALSE=přerušit)

BOOL OpenSetup0()
{
// otevření souboru SETUP.EXE
	HFile = ::CreateFile(ExeFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_READONLY, NULL);

	if (HFile == INVALID_HANDLE_VALUE) return FALSE;

// buffer k načtení záhlaví souboru
	BYTE* buf;
#define BUFSIZE 0x2000
	buf = (BYTE*)MemGet(BUFSIZE);
	CBuf bf;
	bf.Adr(buf);
	bf.Size(BUFSIZE);

// načtení začátku souboru
	DWORD read;
	if (!::ReadFile(HFile, buf, BUFSIZE, &read, NULL) || (read != BUFSIZE))
	{
		MemFree(buf);
		::CloseHandle(HFile);
		return FALSE;
	}

// nalezení NT sekce
	IMAGE_NT_HEADERS* hdr = bf.NTHeader();
	if (hdr == NULL)
	{
		MemFree(buf);
		::CloseHandle(HFile);
		return FALSE;
	}

// nalezení datové sekce
	IMAGE_SECTION_HEADER* sec = bf.NTSection(hdr, ".setupd");
	if (sec == NULL)
	{
		MemFree(buf);
		::CloseHandle(HFile);
		return FALSE;
	}
	RawData = sec->PointerToRawData;
	RawSize = sec->SizeOfRawData;
	VirtAdr = sec->VirtualAddress;

	SizeOff = (BYTE*)&sec->SizeOfRawData - buf;
	SizeImageOff = (BYTE*)&hdr->OptionalHeader.SizeOfImage - buf;
	VirtSizeOff = (BYTE*)&sec->Misc.VirtualSize - buf;
	GDIModeOff = (BYTE*)&hdr->OptionalHeader.Subsystem - buf;

// zrušení bufferu
	MemFree(buf);
	if (RawSize < 10) return FALSE;

// načtení a kontrola záhlaví dat
	::SetFilePointer(HFile, RawData, NULL, FILE_BEGIN);

	if (!::ReadFile(HFile, &Head, sizeof(Head), &read, NULL) || 
		(read != sizeof(Head)) ||
		(Head.Ident[0] != 'S') ||
		(Head.Ident[1] != 'E') ||
		(Head.Ident[2] != 'T') ||
		(Head.Ident[3] != GROUPSNUM)
	   )
	{
		::CloseHandle(HFile);
		return FALSE;
	}

// kontrolní součet záhlaví
	long check = 0;
	BYTE* data = (BYTE*)&Head + 8;
	int n = sizeof(INSTHEAD) - 8;

	for (; n > 0; n--)
	{
		check += *data;
		if (check < 0)
		{
			check *= 2;
			check++;
		}
		else
		{
			check *= 2;
		}

		data++;
	}

// načtení seznamů souborů
	for (int i = 0; i < GROUPSNUM; i++)
	{
		n = Head.Groups[i].SizeFiles;
		MemFree(Groups[i].files);
		Groups[i].files = (BYTE*)MemGet(n);

		if (!::ReadFile(HFile, Groups[i].files, n, &read, NULL) || ((int)read != n))
		{
			::CloseHandle(HFile);
			return FALSE;
		}

// kontrolní součet seznamu souborů
		data = Groups[i].files;

		for (; n > 0; n--)
		{
			check += *data;
			if (check < 0)
			{
				check *= 2;
				check++;
			}
			else
			{
				check *= 2;
			}

			data++;
		}
	}

// úschova začátku dat
	DataOff = ::SetFilePointer(HFile, 0, NULL, FILE_CURRENT);

// uzavření souboru SETUP.EXE
	::CloseHandle(HFile);

// kontrola kontrolního součtu
	return (Head.Check == check);
}

BOOL OpenSetup()
{
	BOOL res;
	do {
		res = OpenSetup0();

		if (!res)
		{
#ifdef COMPACT
#ifdef MINI
			if (ReadError("PETERMIN.EXE")) return FALSE;
#else
			if (ReadError(_T("PETER") VerzeFileTxt _T(".EXE"))) return FALSE;
#endif
			if (ReadError("SETUP.EXE")) return FALSE;
#endif
		}

	} while (!res);

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// zobrazení požadovaného místa

void DispDiskReq()
{
	DiskReq = 0;

	for (int i = 0; i < GROUPSNUM; i++)
	{
		if (Groups[i].check)
		{
			DiskReq += (Head.Groups[i].Size + 1023)/1024;
		}

		if (Groups[i].instal)
		{
			DiskReq -= (Head.Groups[i].Size + 1023)/1024;
		}
	}

	CText txt(_T('0'));
	if (DiskReq > 0)
	{
		txt.Int(DiskReq);
	}
	txt = _T("  ") + txt + _T(" MB");
	txt.SetWindowText(DiskReqNum);

// blokování tlačítka Next
	if (ButtonNext)
	{
		BOOL enable = FALSE;
		for (int i = 0; i < GROUPSNUM; i++)
		{
			if (Groups[i].check != Groups[i].instal)
			{
				enable = TRUE;
			}
		}
		::EnableWindow(ButtonNext, enable);
	}
}


/////////////////////////////////////////////////////////////////////////////
// zjištění tlačítka s fokusem (-1 = není)

int GetButton()
{
	HWND wnd = ::GetFocus();
	if (wnd == NULL) return -1;

	if (wnd == ButtonBack) return IDN_BUTTONBACK;
	if (wnd == ButtonNext) return IDN_BUTTONNEXT;
	if (wnd == ButtonExit) return IDN_BUTTONEXIT;
	if (wnd == ButtonList) return IDN_BUTTONLIST;
	if (wnd == ButtonCanc) return IDN_BUTTONCANC;
	if (wnd == CheckWnd) return IDN_CHECKWND;
	if (wnd == SmlouvaWnd) return IDN_SMLOUVAWND;
	if (wnd == PathWnd) return IDN_PATHWND;
	if (wnd == FlagCZWnd) return IDN_FLAGWNDCZ;
	if (wnd == FlagENGWnd) return IDN_FLAGWNDENG;

	return -1;
}

/////////////////////////////////////////////////////////////////////////////
// nastavení tlačítka s fokusem

void SetButton(int id)
{
	switch (id)
	{
	case IDN_BUTTONBACK:
		::SetFocus(ButtonBack);
		break;

	case IDN_BUTTONNEXT:
		::SetFocus(ButtonNext);
		break;

	case IDN_BUTTONEXIT:
		::SetFocus(ButtonExit);
		break;

	case IDN_BUTTONLIST:
		::SetFocus(ButtonList);
		break;

	case IDN_BUTTONCANC:
		::SetFocus(ButtonCanc);
		break;

	case IDN_CHECKWND:
		::SetFocus(CheckWnd);
		break;

	case IDN_SMLOUVAWND:
		::SetFocus(SmlouvaWnd);
		break;

	case IDN_PATHWND:
		::SetFocus(PathWnd);
		break;

	case IDN_FLAGWNDCZ:
		::SetFocus(FlagCZWnd);
		break;

	case IDN_FLAGWNDENG:
		::SetFocus(FlagENGWnd);
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// obsluha zprávy před rozesláním do oken (TRUE = zpráva zpracována)

BOOL PreTranslateMessage(MSG* msg)
{
	WPARAM wParam = msg->wParam;

	switch (msg->message)
	{
	case WM_KEYDOWN:
		if (wParam == VK_TAB)
		{
			if (::GetKeyState(VK_SHIFT) < 0)
			{
				if (AktPage == PAGESELECT)
				{
					switch (GetButton())
					{
					case IDN_PATHWND:
						SetButton(IDN_CHECKWND);
						return TRUE;

					case IDN_BUTTONBACK:
						SetButton(IDN_PATHWND);
						return TRUE;
					}
				}
				wParam = VK_LEFT;
			}
			else
			{
				if (AktPage == PAGESELECT)
				{
					switch (GetButton())
					{
					case IDN_PATHWND:
						SetButton(IDN_BUTTONBACK);
						return TRUE;

					case IDN_CHECKWND:
						SetButton(IDN_PATHWND);
						return TRUE;
					}
				}
				wParam = VK_RIGHT;
			}
		}

		switch (AktPage)
		{
		case PAGELANG:
			switch (wParam)
			{
			case VK_ESCAPE:
				::ShowWindow(MainFrame, SW_HIDE);
				::SendMessage(MainFrame, WM_CLOSE, 0, 0);
				return TRUE;

			case VK_UP:
			case VK_LEFT:
				switch (GetButton())
				{
				case IDN_FLAGWNDCZ:
					SetButton(IDN_BUTTONEXIT);
					return TRUE;

				case IDN_FLAGWNDENG:
					SetButton(IDN_FLAGWNDCZ);
					return TRUE;

				case IDN_BUTTONEXIT:
					SetButton(IDN_FLAGWNDENG);
					return TRUE;
				}
				break;

			case VK_DOWN:
			case VK_RIGHT:
				switch (GetButton())
				{
				case IDN_FLAGWNDCZ:
					SetButton(IDN_FLAGWNDENG);
					return TRUE;

				case IDN_FLAGWNDENG:
					SetButton(IDN_BUTTONEXIT);
					return TRUE;

				case IDN_BUTTONEXIT:
					SetButton(IDN_FLAGWNDCZ);
					return TRUE;
				}
				break;

			case VK_RETURN:
				msg->wParam = VK_SPACE;
				break;
			}
			break;

		case PAGEMAIN:
			switch (wParam)
			{
			case VK_ESCAPE:
				::ShowWindow(MainFrame, SW_HIDE);
				::SendMessage(MainFrame, WM_CLOSE, 0, 0);
				return TRUE;

			case VK_DOWN:
			case VK_RIGHT:
				KeySelect++;
				if (KeySelect > 3) KeySelect = 0;
				RePaint();
				return TRUE;

			case VK_UP:
			case VK_LEFT:
				KeySelect--;
				if (KeySelect < 0) KeySelect = 3;
				RePaint();
				return TRUE;

			case VK_HOME:
				KeySelect = 0;
				RePaint();
				return TRUE;

			case VK_END:
				KeySelect = 3;
				RePaint();
				return TRUE;

			case VK_RETURN:
				switch(KeySelect)
				{
				case 0:
					if (FirstInst)
					{
						if (OpenSetup())
						{
							SetPage(PAGESELECT);
						}
					}
					return TRUE;

				case 1:
					if (!FirstInst)
					{
						if (OpenSetup())
						{
							SetPage(PAGESELECT);
						}
					}
					return TRUE;

				case 2:
					if (!FirstInst)
					{
						SetPage(PAGEISDINST);

						if (!OpenSetup())
						{
							SetPage(PAGEMAIN);
						}
					}
					return TRUE;

				case 3:
					::ShowWindow(MainFrame, SW_HIDE);
					::SendMessage(MainFrame, WM_CLOSE, 0, 0);
					return TRUE;

				default:
					goto STARTMAIN;
				}
			}
			break;

		case PAGEISDINST:
			switch (wParam)
			{
			case VK_ESCAPE:
				::ShowWindow(MainFrame, SW_HIDE);
				::SendMessage(MainFrame, WM_CLOSE, 0, 0);
				return TRUE;

			case VK_UP:
			case VK_LEFT:
				switch (GetButton())
				{
				case IDN_BUTTONBACK:
					SetButton(IDN_BUTTONEXIT);
					return TRUE;

				case IDN_BUTTONNEXT:
					SetButton(IDN_BUTTONBACK);
					return TRUE;

				case IDN_BUTTONEXIT:
					SetButton(IDN_BUTTONNEXT);
					return TRUE;
				}
				break;

			case VK_DOWN:
			case VK_RIGHT:
				switch (GetButton())
				{
				case IDN_BUTTONBACK:
					SetButton(IDN_BUTTONNEXT);
					return TRUE;

				case IDN_BUTTONNEXT:
					SetButton(IDN_BUTTONEXIT);
					return TRUE;

				case IDN_BUTTONEXIT:
					SetButton(IDN_BUTTONBACK);
					return TRUE;
				}
				break;

			case VK_RETURN:
				msg->wParam = VK_SPACE;
				break;
			}
			break;

		case PAGESELECT:
			switch (wParam)
			{
			case VK_ESCAPE:
				::ShowWindow(MainFrame, SW_HIDE);
				::SendMessage(MainFrame, WM_CLOSE, 0, 0);
				return TRUE;

			case VK_HOME:
			case VK_END:
				if (GetButton() == IDN_PATHWND) return FALSE;

			case VK_PRIOR:
			case VK_NEXT:
			case VK_UP:
			case VK_DOWN:
				SetButton(IDN_CHECKWND);
				msg->hwnd = CheckWnd;
				return FALSE;

			case VK_LEFT:
				switch (GetButton())
				{
				case IDN_PATHWND:
					return FALSE;

				case IDN_CHECKWND:
					SetButton(IDN_BUTTONLIST);
					return TRUE;

				case IDN_BUTTONBACK:
					SetButton(IDN_CHECKWND);
					return TRUE;

				case IDN_BUTTONNEXT:
					SetButton(IDN_BUTTONBACK);
					return TRUE;

				case IDN_BUTTONEXIT:
					SetButton(IDN_BUTTONNEXT);
					return TRUE;

				case IDN_BUTTONLIST:
					SetButton(IDN_BUTTONEXIT);
					return TRUE;
				}
				break;

			case VK_RIGHT:
				switch (GetButton())
				{
				case IDN_PATHWND:
					return FALSE;

				case IDN_CHECKWND:
					SetButton(IDN_BUTTONBACK);
					return TRUE;

				case IDN_BUTTONBACK:
					SetButton(IDN_BUTTONNEXT);
					return TRUE;

				case IDN_BUTTONNEXT:
					SetButton(IDN_BUTTONEXIT);
					return TRUE;

				case IDN_BUTTONEXIT:
					SetButton(IDN_BUTTONLIST);
					return TRUE;

				case IDN_BUTTONLIST:
					SetButton(IDN_CHECKWND);
					return TRUE;
				}
				break;

			case VK_RETURN:
			case VK_SPACE:
				switch (GetButton())
				{
				case IDN_CHECKWND:
					{
						int inx = ::SendMessage(CheckWnd, LVM_GETNEXTITEM, (WPARAM)-1, LVNI_SELECTED);
						if ((DWORD)inx < (DWORD)GROUPSNUM)
						{
							Groups[inx].check = !Groups[inx].check;
							LVITEM lvi;
							lvi.mask = LVIF_IMAGE;
							lvi.iItem = inx;
							lvi.iSubItem = 0;
							lvi.iImage = Groups[inx].check ? 1 : 0;
							if (Groups[inx].instal) lvi.iImage += 2;
							::SendMessage(CheckWnd, LVM_SETITEM, 0, (LPARAM)&lvi);
							DispDiskReq();
						}
					}
					break;

				default:
					msg->wParam = VK_SPACE;
				}
				break;
			}
			break;

		case PAGEINSTAL:
			switch (wParam)
			{
			case VK_ESCAPE:
				Storno = TRUE;
				break;

			case VK_RETURN:
				msg->wParam = VK_SPACE;
				break;
			}
			break;

		case PAGEOK:
		case PAGEDINSTOK:
			switch (wParam)
			{
			case VK_ESCAPE:
				::ShowWindow(MainFrame, SW_HIDE);
				::SendMessage(MainFrame, WM_CLOSE, 0, 0);
				return TRUE;

			case VK_RETURN:
				msg->wParam = VK_SPACE;
				break;
			}
			break;
		}
		break;

	case WM_KEYUP:
		switch (AktPage)
		{
		case PAGESELECT:
			if (GetButton() == IDN_CHECKWND) break;

		case PAGEISDINST:
		case PAGEINSTAL:
		case PAGEOK:
		case PAGEDINSTOK:
		case PAGELANG:
			switch (wParam)
			{
			case VK_RETURN:
				msg->wParam = VK_SPACE;
				break;
			}
		}
		break;

// stisk levého tlačítka myši
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
		if (AktPage == PAGEMAIN)
		{
STARTMAIN:
			if ((RelX >= 0) && (RelX < ClientWidth) && (RelY >= MENUTOP) && (RelY < MENUTOP + 4*MENUHEIGHT))
			{
				if (RelY < MENUTOP + MENUHEIGHT)
				{
					if (FirstInst)
					{
						if (OpenSetup())
						{
							SetPage(PAGESELECT);
						}
					}
				}
				else
				{
					if (RelY < MENUTOP + 2*MENUHEIGHT)
					{
						if (!FirstInst)
						{
							if (OpenSetup())
							{
								SetPage(PAGESELECT);
							}
						}
					}
					else
					{
						if (RelY < MENUTOP + 3*MENUHEIGHT)
						{
							if (!FirstInst)
							{
								SetPage(PAGEISDINST);

								if (!OpenSetup())
								{
									SetPage(PAGEMAIN);
								}
							}
						}
						else
						{
							::ShowWindow(MainFrame, SW_HIDE);
							::SendMessage(MainFrame, WM_CLOSE, 0, 0);
							return TRUE;
						}
					}
				}
			}
		}
		break;

	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// zobrazení kurzoru čekání (zahájení a ukončení musí být do páru!)

void BeginWaitCursor()
{
// zvýšení čítače čekání
	Waiting++;

// zobrazení kurzoru
	if (Waiting)
	{
		::SetCursor(CurWait);
	}
}


/////////////////////////////////////////////////////////////////////////////
// vypnutí kurzoru čekání (zahájení a ukončení musí být do páru!)

void EndWaitCursor()
{
// snížení čítače čekání
	Waiting--;

// zobrazení kurzoru
	if (!Waiting)
	{
		if (AktPage == PAGEMAIN)
		{
			::SetCursor(CurPeter);
		}
		else
		{
			::SetCursor(CurArrow);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření adresáře (pokud existuje, je vše OK)

BOOL CreateDir(CText txt)
{
// ořezání mezer
	txt.TrimLeft();
	txt.TrimRight();

// ořezání koncových "\"
	while (txt.LastChar() == _T('\\'))
	{
		txt.Delete(txt.Length() - 1);
		txt.TrimRight();
	}

// ROOT je OK
	if (txt.IsEmpty()) return TRUE;

// vytvoření adresáře
	BOOL res = ::CreateDirectory(txt, NULL);
	if (res) return TRUE;

// při neúspěchu kontrola, zda adresář již existuje
	int attrib = (int)::GetFileAttributes(txt);
	return ((attrib != -1) && (attrib & FILE_ATTRIBUTE_DIRECTORY));
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření adresáře (i vícestupňově - včetně cesty)

BOOL CreatePath(CText txt)
{
// první pokus o vytvoření adresáře
	if (CreateDir(txt)) return TRUE;

// ořezání mezer
	txt.TrimLeft();
	txt.TrimRight();

// ořezání koncových "\"
	while (txt.LastChar() == _T('\\'))
	{
		txt.Delete(txt.Length() - 1);
		txt.TrimRight();
	}

// adresář se bude zkracovat
	CText txt2 = txt;
	CText txt3;

// zkracování adresáře
	int pos;
	while ((pos = txt2.RevFind(_T('\\'))) >= 0)
	{
		txt2.Delete(pos);
		if (CreateDir(txt2)) break;
	}

// prodlužování adresáře
	while (txt2.Length() < txt.Length())
	{
		txt2.Add(_T('\\'));
		txt3 = txt.Mid(txt2.Length(), txt.Length());
		pos = txt3.Find(_T('\\'));
		if (pos < 0) break;
		txt2.Add(txt3.Left(pos));
		if (!CreateDir(txt2)) return FALSE;
	}

// vytvoření posledního stupně (to je poslední pokus)
	return CreateDir(txt);
}

/////////////////////////////////////////////////////////////////////////////
// dekomprese dat

#ifdef COMPACT

#define MAXLENX 25					// min. délka dlouhého řetězce
#define MAXLEN (MAXLENX+254)		// maximální délka řetězce
#define SUBSTLEN	7				// délka nahrazená dlouhým kódem

class CDekomp
{
private:
	BYTE*	m_DstBeg;				// výstupní buffer - začátek nezpracovaných dat
	BYTE*	m_DstEnd;				// konec výstupního bufferu
	BYTE*	m_Dst;					// ukládací adresa

	BYTE*	m_SrcEnd;				// konec dat ve vstupním bufferu
	BYTE*	m_Src;					// čtecí adresa

	WORD	m_Status;				// střadač stavového slova

public:
	void Init()	{ m_Status = 0;	}	// inicializace

	inline void DstBeg(BYTE* beg) { m_DstBeg = beg; }
	inline BYTE* DstBeg() { return m_DstBeg; }

	inline void DstEnd(BYTE* end) { m_DstEnd = end; }
	inline BYTE* DstEnd() { return m_DstEnd; }

	inline void Dst(BYTE* dst) { m_Dst = dst; }
	inline BYTE* Dst() { return m_Dst; }

	inline void SrcEnd(BYTE* end) { m_SrcEnd = end; }
	inline BYTE* SrcEnd() { return m_SrcEnd; }

	inline void Src(BYTE* src) { m_Src = src; }
	inline BYTE* Src() { return m_Src; }

// načtení jednoho bitu (výstup = hodnota 0 nebo 1)
	inline int DekBit()
	{
		int result = m_Status & 1;
		m_Status >>= 1;

		if (m_Status == 0)
		{
			m_Status = *(WORD*)m_Src;
			m_Src++;
			m_Src++;
			result = m_Status & 1;
			m_Status >>= 1;
			m_Status |= 0x8000;
		}
		return result;
	}

// dekomprese bloku dat
//  Ve zdrojovém bloku je potřeba nechat za koncem dat rezervu asi tak nějak 10 bajtů (pokud možno platných)
//  V cílovém bloku je potřeba nechat za koncem dat rezervu asi tak nějak 300 bajtů,
//	před začátkem cílových dat je potřeba ponechat rezervu 32 KB starých dat.

	void DeKomp()
	{

// lokální proměnné
		BYTE offsetL, offsetH;			// offset k opakování
		int delka;						// délka k opakování

// kontrola přetečení bufferů
		while ((m_Dst < m_DstEnd) && (m_Src < m_SrcEnd))
		{

// přesun bajtu bez komprese
			if (DekBit() == 0)
			{
				*m_Dst = *m_Src;
				m_Dst++;
				m_Src++;
			}

// jinak bude opakování řetězce
			else
			{
				offsetH = 0;
				offsetL = 0;

// první bit délky
				delka = DekBit();

// zvýšení čítače délky
				for (;;)
				{
					delka++;
					delka++;

// načtení příznaku konce kódu
					if (DekBit() == 0) break;

// dosažení maximální délky
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
						delka = *m_Src;
						m_Src++;
						if (delka == 255) break;	// konec dekomprese
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
					if (DekBit() == 0)
					{
						offsetH = (BYTE)DekBit();

						offsetH <<= 1;
						offsetH |= DekBit();

						offsetH <<= 1;
						offsetH |= DekBit();

						if (offsetH > 1)
						{
							offsetH <<= 1;
							offsetH |= DekBit();

							offsetH--;
							if (offsetH > 6)
							{
								offsetH <<= 1;
								offsetH |= DekBit();

								offsetH -= 7;
								if (offsetH > 13)
								{
									offsetH <<= 1;
									offsetH |= DekBit();

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
				offsetL = *m_Src;
				m_Src++;

// přenesení řetězce (nepoužívat MemCopy - nepřenáší po bajtech!!!)
				BYTE* src2 = m_Dst - (WORD)(offsetL + offsetH*256);
				for (; delka > 0; delka--)
				{
					*m_Dst = *src2;
					m_Dst++;
					src2++;
				}
			}
		}
	}
};

#endif // COMPACT

/////////////////////////////////////////////////////////////////////////////
// načtení registru

CText GetReg(CText key, CText name)
{
// text výsledné hodnoty
	CText txt;

// otevření klíče
	HKEY hkey;
	if (::RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		key,
		0,
		KEY_QUERY_VALUE,
		&hkey) == ERROR_SUCCESS)
	{

// načtení hodnoty klíče
		DWORD type = REG_SZ;
#define REGBUFSIZE 300
		BYTE buf[REGBUFSIZE+1];
		DWORD bufsize = REGBUFSIZE;

		if ((::RegQueryValueEx(
			hkey,
			name,
			0,
			&type,
			buf,
			&bufsize) == ERROR_SUCCESS) &&
			(type == REG_SZ) &&
			(bufsize > 0) &&
			(bufsize <= REGBUFSIZE))
		{
			txt.Set((char*)buf, bufsize-1);
			txt.KorigNul();
			txt.TrimLeft();
			txt.TrimRight();
		}

// uzavření klíče
		::RegCloseKey(hkey);
	}

	return txt;
}


/////////////////////////////////////////////////////////////////////////////
// nastavení registru

void SetReg(CText key, CText name, CText data)
{
// otevření klíče
	HKEY hkey;
	DWORD pos;

	if (::RegCreateKeyEx(
		HKEY_LOCAL_MACHINE,
		key,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hkey,
		&pos) == ERROR_SUCCESS)
	{

// nastavení hodnoty registru
		::RegSetValueEx(
			hkey,
			name,
			0,
			REG_SZ,
			(const BYTE*)(const char*)data,
			data.Length()+1);

// uzavření klíče
		::RegCloseKey(hkey);
	}
}

/////////////////////////////////////////////////////////////////////////////
// zrušení registru

void DelReg(CText key, CText name)
{
// text výsledné hodnoty
	CText txt;

// otevření klíče
	HKEY hkey;
	if (::RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		key,
		0,
		KEY_ALL_ACCESS,
		&hkey) == ERROR_SUCCESS)
	{

// zrušení registru
		::RegDeleteValue(
			hkey,
			name);

// uzavření klíče
		::RegCloseKey(hkey);
	}
}

/////////////////////////////////////////////////////////////////////////////
// načtení cesty Shell

CText GetShell(LPCTSTR name)
{
// text výsledné hodnoty
	CText txt;

// otevření klíče
	HKEY hkey;
	if (::RegOpenKeyEx(
		HKEY_CURRENT_USER,
		_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"),
		0,
		KEY_QUERY_VALUE,
		&hkey) == ERROR_SUCCESS)
	{

// načtení hodnoty klíče
		DWORD type = REG_SZ;
#define REGBUFSIZE 300
		BYTE buf[REGBUFSIZE+1];
		DWORD bufsize = REGBUFSIZE;

		if ((::RegQueryValueEx(
			hkey,
			name,
			0,
			&type,
			buf,
			&bufsize) == ERROR_SUCCESS) &&
			(type == REG_SZ) &&
			(bufsize > 0) &&
			(bufsize <= REGBUFSIZE))
		{
			txt.Set((char*)buf, bufsize-1);
			txt.KorigNul();
			txt.TrimLeft();
			txt.TrimRight();
		}

// uzavření klíče
		::RegCloseKey(hkey);
	}

	return txt;
}


/////////////////////////////////////////////////////////////////////////////
// provedení instalace a přeinstalace

void Install()
{
	Storno = FALSE;
	int i,j;

// cesta k ukládání souborů
	CText path = InstPath;
	if (path.LastChar() != _T('\\')) path += _T('\\');

// odinstalování při přeinstalování
	if (!FirstInst)
	{

// zjištění počtu souborů (kvůli indikaci)
		DataSize = 0;
		DataSizeOK = 0;
		for (i = 0; i < GROUPSNUM; i++)
		{
			if (!Groups[i].check && Groups[i].instal)
			{
				DataSize += Head.Groups[i].Files;
			}
		}

// záhájení odinstalování
		if (DataSize > 0)
		{
			::SetWindowText(IndWndNadp, X(_T("Probíhá odinstalování částí aplikace Petr ..."), _T("Proceeding uninstallation of the Peter application parts ...")));

// cyklus přes odinstalovávané skupiny
			WIN32_FIND_DATA fnd;

			for (i = 0; i < GROUPSNUM; i++)
			{
				if (!Groups[i].check && Groups[i].instal)
				{

// vypnutí příznaku instalace
					CText grpnum;
					grpnum.Int(i);
					SetReg(ProductKey, _T("Group") + grpnum, _T('0'));
					Groups[i].instal = false;

// cyklus přes všechny soubory
					BYTE* data = Groups[i].files;
					for (j = Head.Groups[i].Files; j > 0; j--)
					{

// jméno souboru
						int size = ((INSTFILE*)data)->Size;
						CText name(((INSTFILE*)data)->Name, ((INSTFILE*)data)->NameN);
						data += 2*sizeof(long) + sizeof(BYTE) + ((INSTFILE*)data)->NameN;
						name = path + name;

// načtení parametrů souboru
						HANDLE file = ::FindFirstFile(name, &fnd);

// zrušení souboru
						if (file != INVALID_HANDLE_VALUE)
						{

// zobrazení jména souboru
							name.SetWindowText(IndWndFile);

							FILETIME datetime;
							::FileTimeToLocalFileTime(&fnd.ftLastWriteTime, &datetime);
							FILETIME datetime2 = datetime;
							*(__int64*)&datetime2 += (__int64)10000000*60*60;
							FILETIME datetime3 = datetime;
							*(__int64*)&datetime3 -= (__int64)10000000*60*60;

							if ((MemCompare(&datetime, &Head.DateTime, sizeof(FILETIME)) ||
								MemCompare(&datetime2, &Head.DateTime, sizeof(FILETIME)) ||
								MemCompare(&datetime3, &Head.DateTime, sizeof(FILETIME))) &&
								((int)fnd.nFileSizeLow == size))
							{
								if (fnd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
								{
									::SetFileAttributes(name, FILE_ATTRIBUTE_NORMAL);
								}
								::DeleteFile(name);

// zrušení adresáře
								while (name.Length() > InstPath.Length())
								{
									int pos = name.RevFind(_T('\\'));
									if (pos < 0) break;
									name.Delete(pos);
									if (!::RemoveDirectory(name)) break;
								}
							}

// uzavření hledání
							::FindClose(file);
						}

// zobrazení indikátoru operace
						DataSizeOK++;
						::SendMessage(IndWnd, PBM_SETPOS, ::MulDiv(DataSizeOK, PROGMAX, DataSize), 0);

// obsluha zpráv
						PumpMessage();
					}
				}
			}
		}

		::SetWindowText(IndWndNadp, X(_T("Probíhá přiinstalování částí aplikace Petr ..."), _T("Proceeding add-installation of the Peter application parts ...")));
	}

// zjištění velikosti souborů (kvůli indikaci)
	DataSize = 0;
	DataSizeOK = 0;
	for (i = 0; i < GROUPSNUM; i++)
	{
		if (Groups[i].check && !Groups[i].instal)
		{
			BYTE* data = Groups[i].files;

			for (j = Head.Groups[i].Files; j > 0; j--)
			{
				DataSize += ((INSTFILE*)data)->Size;
				data += 2*sizeof(long) + sizeof(BYTE) + ((INSTFILE*)data)->NameN;
			}
		}
	}

// není přiinstalování (bylo jen odinstalování)
	if (DataSize == 0)
	{
		SetPage(PAGEOK);
		return;
	}

// dekompresní buffer
#ifdef COMPACT
	CDekomp dek;
#endif

// otevření vstupního souboru
	do {
		HFile = ::CreateFile(ExeFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_READONLY, NULL);

		if (HFile == INVALID_HANDLE_VALUE)
		{
#ifdef COMPACT
#ifdef MINI
			if (ReadError("PETERMIN.EXE"))
#else
			if (ReadError(_T("PETER") VerzeFileTxt _T(".EXE")))
#endif
			if (ReadError("SETUP.EXE"))
#endif
			{
				SetPage(PAGESELECT);
				return;
			}
		}

	} while (HFile == INVALID_HANDLE_VALUE);

// vstupní a výstupní buffery
#define SRCBUFSIZE 0x8000						// celková velikost zdrojového bufferu
#define SRCBUFREZ  32							// rezerva za zdrojovými daty
	BYTE* srcbuf = (BYTE*)MemGet(SRCBUFSIZE + 1024);
#define DSTBUFSIZE 0x12000						// celková velikost cílového bufferu
#define DSTBUFDATA (DSTBUFSIZE/2)				// začátek dat v cílovém bufferu
#define DSTBUFREZ  1024							// rezerva za cílovými daty (a na začátku bufferu)
	BYTE* dstbuf = (BYTE*)MemGet(DSTBUFSIZE + 1024);

// příprava ukazatele dat
	DWORD readwrite;
	int off = DataOff;

// při první instalaci uložení licence a odinstalátoru
	if (FirstInst)
	{

// vytvoření cílového adresáře
		CreatePath(InstPath);

// jméno odinstalačního souboru
		CText name;
		name = path + _T("Uninstal.exe");

// vytvoření cílového souboru
		BOOL del = FALSE;
		::SetFileAttributes(name, FILE_ATTRIBUTE_NORMAL);
		HANDLE file = ::CreateFile(name, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file != INVALID_HANDLE_VALUE)
		{

// kopie prvního bloku dat souboru (s korekcí velikosti souboru)
			int size = (DataOff + 0x1ff) & ~0x1ff;
			int size0 = size;
			if (size0 > SRCBUFSIZE) size0 = SRCBUFSIZE;

			if (::ReadFile(HFile, srcbuf, size0, &readwrite, NULL) && (size0 == (int)readwrite))
			{
				*(long*)(srcbuf + SizeOff) = (size - RawData + 0x1ff) & ~0x1ff;
				*(long*)(srcbuf + SizeImageOff) = (VirtAdr + size - RawData + 0x1fff) & ~0x1fff;
				*(long*)(srcbuf + VirtSizeOff) = size - RawData;
				*(WORD*)(srcbuf + GDIModeOff) = IMAGE_SUBSYSTEM_WINDOWS_GUI;

				if (::WriteFile(file, srcbuf, size0, &readwrite, NULL) && (size0 == (int)readwrite))
				{
					size -= size0;

// kopie dat souboru
					while (size > 0)
					{
						size0 = size;
						if (size0 > SRCBUFSIZE) size0 = SRCBUFSIZE;
						size -= size0;

						if (!::ReadFile(HFile, srcbuf, size0, &readwrite, NULL) ||
							(size0 != (int)readwrite) ||
							!::WriteFile(file, srcbuf, size0, &readwrite, NULL) ||
							(size0 != (int)readwrite))
						{
							del = TRUE;
							break;
						}
					}
				}
				else
				{
					del = TRUE;
				}
			}
			else
			{
				del = TRUE;
			}

// nastavení data a času souboru
			FILETIME datetime;
			::LocalFileTimeToFileTime(&Head.DateTime, &datetime);
			::SetFileTime(file, NULL, NULL, &datetime);

// uzavření cílového souboru
			::CloseHandle(file);
		}

// při chybě zrušení souboru
		if (del) ::DeleteFile(name);

// obsluha zpráv
		PumpMessage();
	}

// cyklus přes všechny skupiny
	for (i = 0; i < GROUPSNUM; i++)
	{

// test, zda má být skupina instalována
		if (Groups[i].check && !Groups[i].instal)
		{

// načtení prvního bloku dat skupiny
			BOOL ok;

#ifdef COMPACT
			int srcsize = Head.Groups[i].SizeGroup;						// velikost dat skupiny
			int srcsize0 = srcsize;
			if (srcsize0 > SRCBUFSIZE) srcsize0 = SRCBUFSIZE;			// velikost dat k naetení do bufferu
			srcsize -= srcsize0;

			do {
				::SetFilePointer(HFile, off, NULL, FILE_BEGIN);
				ok = (
						(srcsize0 > 0) && 
						::ReadFile(HFile, srcbuf, srcsize0, &readwrite, NULL) && 
						(srcsize0 == (int)readwrite) &&
						(srcbuf[0] == 27) &&
						(srcbuf[1] == 'X') &&
						(srcbuf[2] == 'C') &&
						(srcbuf[3] == 'D') &&
						(srcbuf[4] == 0) &&
						(srcbuf[5] == 0)
					);

				if (!ok)
				{
					if (ReadError("PETERMIN.EXE"))
					{
						MemFree(srcbuf);
						MemFree(dstbuf);
						SetPage(PAGESELECT);
						return;
					}
				}

			} while (!ok);

// prubižný ukazatel etených dat
			int off0 = off + srcsize0;

// poíprava dekompresního bufferu - poeáteení dekomprese
			dek.Init();
			dek.DstBeg(dstbuf + DSTBUFDATA);
			dek.DstEnd(dstbuf + DSTBUFSIZE - DSTBUFREZ);
			dek.Dst(dstbuf + DSTBUFDATA);
			if (srcsize0 == SRCBUFSIZE)
			{
				dek.SrcEnd(srcbuf + srcsize0 - SRCBUFREZ);
			}
			else
			{
				dek.SrcEnd(srcbuf + srcsize0);
			}
			dek.Src(srcbuf + 6);
			dek.DeKomp();
#endif // COMPACT

// cyklus přes všechny soubory
			BYTE* data = Groups[i].files;
			for (j = Head.Groups[i].Files; j > 0; j--)
			{

// parametry souboru
				int check0 = 0;
				int size = ((INSTFILE*)data)->Size;
				int check = ((INSTFILE*)data)->Check;
				CText name(((INSTFILE*)data)->Name, ((INSTFILE*)data)->NameN);
#ifndef COMPACT
				CText name0(name);
				if (name0 == "PETER.EXE") name0 = "SETUP.DAT";
#endif
				data += 2*sizeof(long) + sizeof(BYTE) + ((INSTFILE*)data)->NameN;
				name = path + name;
				name.SetWindowText(IndWndFile);

// vytvoření cílového souboru
				HANDLE file = ::CreateFile(name, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

// při neúspěchu druhý pokus - s vytvořením adresáře
				if (file == INVALID_HANDLE_VALUE)
				{
					CreatePath(name.Left(name.RevFind(_T('\\'))));
					file = ::CreateFile(name, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

// další pokus - zrušení atributu R/O souboru
					if (file == INVALID_HANDLE_VALUE)
					{
						::SetFileAttributes(name, FILE_ATTRIBUTE_NORMAL);
						file = ::CreateFile(name, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					}
				}

// obsluha zpráv
				PumpMessage();

// chyba - soubor nelze vytvořit
				if (file == INVALID_HANDLE_VALUE)
				{
					::MessageBox(MainFrame,
						X(
						CText(
							_T("Chyba - nelze vytvořit cílový soubor\n\n") +
							name +
							_T("\n\nInstalace nemůže dále pokračovat a bude přerušena. Důvodem může být:"
								"\n   - přepisovaný cílový program EXE je spuštěn"
								"\n   - přepisovaný cílový soubor je otevřen některým programem"
								"\n   - chybné zadání cílové složky (např. neexistující disk)"
								"\n   - plný cílový disk nebo zákaz zápisu na disk")),
						CText(
							_T("Error - could not create destination file\n\n") +
							name +
							_T("\n\nInstallation can't continue and will be interrupted. It can be caused by:"
								"\n   - overwriten destination EXE program is running"
								"\n   - overwriten destination file is opened by some other program"
								"\n   - wrongly entered destination folder (e.g. invalid disk)"
								"\n   - destination disk is full or it is write protected"))),
						X(_T("Chyba zápisu"), _T("Write Error")),
						MB_OK | MB_ICONERROR);

					MemFree(srcbuf);
					MemFree(dstbuf);
					SetPage(PAGESELECT);
					return;
				}

// otevření vstupního souboru
#ifndef COMPACT
				HANDLE ifile;
				do {
					ifile = ::CreateFile(name0, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
						FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_READONLY, NULL);

					if (ifile == INVALID_HANDLE_VALUE)
					{
						if (ReadError(name0))
						{
							::CloseHandle(file);
							::DeleteFile(name);

							MemFree(srcbuf);
							MemFree(dstbuf);
							SetPage(PAGESELECT);
							return;
						}
					}

				} while (ifile == INVALID_HANDLE_VALUE);
#endif // COMPACT

// cyklus uložení všech dat
				while (size > 0)
				{
#ifdef COMPACT
// velikost bloku dat poipravených v bufferu
					int size0 = dek.Dst() - dek.DstBeg();
					if (size0 <= 0)
					{

// naetení dalšího bloku zdrojoých dat
						if (srcsize <= 0)		// aby bylo ješti nico naeteno za koncem dat
						{
							srcsize = 1024;
							off0 -= 1024;
						}

						int n = (srcbuf + SRCBUFSIZE) - dek.Src();
						if (n < 0) n = 0;
						MemCopy(srcbuf, dek.Src(), n);
						srcsize0 = srcsize;
						if (srcsize0 > SRCBUFSIZE - n) srcsize0 = SRCBUFSIZE - n;

						srcsize -= srcsize0;

						do {
							::SetFilePointer(HFile, off0, NULL, FILE_BEGIN);
							ok = (
									::ReadFile(HFile, srcbuf + n, srcsize0, &readwrite, NULL) && 
									(srcsize0 == (int)readwrite)
								);

							if (!ok)
							{

								if (ReadError("PETERMIN.EXE"))
								{
									::CloseHandle(file);
									::DeleteFile(name);

									MemFree(srcbuf);
									MemFree(dstbuf);
									SetPage(PAGESELECT);
									return;
								}
							}

						} while (!ok);

						off0 += srcsize0;

// posun ukazatelu a dat v bufferu
						if ((srcsize0 + n) == SRCBUFSIZE)
						{
							dek.SrcEnd(srcbuf + n + srcsize0 - SRCBUFREZ);
						}
						else
						{
							dek.SrcEnd(srcbuf + n + srcsize0);
						}
						dek.Src(srcbuf);

						n = dek.Dst() - (dstbuf + DSTBUFDATA);
						MemCopy(dstbuf + DSTBUFDATA - n, dstbuf + DSTBUFDATA, n);

						dek.DstBeg(dstbuf + DSTBUFDATA);
						dek.DstEnd(dstbuf + DSTBUFSIZE - DSTBUFREZ);
						dek.Dst(dstbuf + DSTBUFDATA);

						dek.DeKomp();

						size0 = dek.Dst() - dek.DstBeg();
					}
#else // COMPACT
					int size0 = DSTBUFSIZE;
					if (size0 > size) size0 = size;
					do {
						ok = ( ::ReadFile(ifile, dstbuf, size0, &readwrite, NULL) &&
										(size0 == (int)readwrite)
									);

						if (!ok)
						{
							if (ReadError(name0))
							{
								::CloseHandle(file);
								::DeleteFile(name);
								::CloseHandle(ifile);

								MemFree(srcbuf);
								MemFree(dstbuf);
								SetPage(PAGESELECT);
								return;
							}
						}
					} while (!ok);
#endif // COMPACT

// zápis bloku dat
					if (size0 > size) size0 = size;
					size -= size0;
#ifdef COMPACT
					if (!::WriteFile(file, dek.DstBeg(), size0, &readwrite, NULL) || (size0 != (int)readwrite))
#else
					if (!::WriteFile(file, dstbuf, size0, &readwrite, NULL) || (size0 != (int)readwrite))
#endif
					{
						::CloseHandle(file);
						::DeleteFile(name);
#ifndef COMPACT
						::CloseHandle(ifile);
#endif
						::MessageBox(MainFrame,
							X(
							CText(
								_T("Chyba zápisu do cílového souboru\n\n") +
								name +
								_T("\n\nCílový disk je zřejmě plný."
									"\nInstalace nemůže dále pokračovat a bude přerušena.")),
							CText(
								_T("Error writing destination file\n\n") +
								name +
								_T("\n\nDestination disk is maybe full."
									"\nInstallation can't continue and will be interrupted."))),
							X(_T("Chyba zápisu"), _T("Write Error")),
							MB_OK | MB_ICONERROR);

						MemFree(srcbuf);
						MemFree(dstbuf);
						SetPage(PAGESELECT);
						return;
					}
#ifdef COMPACT
					BYTE* d = dek.DstBeg();
					dek.DstBeg(dek.DstBeg() + size0);
#else
					BYTE* d = dstbuf;
#endif

// zobrazení indikátoru operace
					DataSizeOK += size0;
					::SendMessage(IndWnd, PBM_SETPOS, ::MulDiv(DataSizeOK, PROGMAX, DataSize), 0);

// obsluha zpráv
					PumpMessage();

// kontrolní součet bloku dat
					for (; size0 > 0; size0--)
					{
						check0 += *d;
						if (check0 < 0)
						{
							check0 *= 2;
							check0++;
						}
						else
						{
							check0 *= 2;
						}

						d++;
					}
				}

// nastavení data a času souboru
				FILETIME datetime;
				::LocalFileTimeToFileTime(&Head.DateTime, &datetime);
				::SetFileTime(file, NULL, NULL, &datetime);

// uzavření cílového souboru
				::CloseHandle(file);
#ifndef COMPACT
				::CloseHandle(ifile);
#endif

// kontrola kontrolního součtu souboru
				if (check != check0)
				{
					::DeleteFile(name);

					::MessageBox(MainFrame,
						X(
						CText(
							_T("Chyba instalace souboru\n\n") +
							name +
#ifdef COMPACT
#ifdef MINI
							_T("\n\nInstalační program PETERMIN.EXE je poškozen.") +
#else
							_T("\n\nInstalační program PETER") VerzeFileTxt _T(".EXE je poškozen.") +
#endif
#else
							_T("\n\nSoubor ") + name0 + _T(" je poškozen.") +
#endif
								_T("\nInstalace nemůže dále pokračovat a bude přerušena.")),

						CText(
							_T("Installation error of the file\n\n") +
							name +
#ifdef COMPACT
#ifdef MINI
							_T("\n\nInstallation program PETERMIN.EXE is damaged.") +
#else
							_T("\n\nInstallation program PETER") VerzeFileTxt _T(".EXE is damaged.") +
#endif
#else
							_T("\n\nFile ") + name0 + _T(" is damaged.") +
#endif
								_T("\nInstallation can't continue and will be interrupted."))),

						X(_T("Chyba instalace"), _T("Installation Error")),
						MB_OK | MB_ICONERROR);

					MemFree(srcbuf);
					MemFree(dstbuf);
					SetPage(PAGESELECT);
					return;
				}

// obsluha zpráv
				PumpMessage();

// přerušení instalace
				if (Storno)
				{
					if (::MessageBox(MainFrame,
							X(
							CText(
								_T("Přerušením instalace zůstanou ve složce ") +
								InstPath +
								_T("\njiž nainstalované soubory. Soubory můžete zrušit Průzkumníkem."
									"\n\nOpravdu chcete instalaci přerušit?")),
							CText(
								_T("There will remain already installed files in folder ") +
								InstPath +
								_T("\nif you break installation. You can delete those files by File Explorer."
									"\n\nDo you really want to break the installation?"))),

							X(_T("Přerušení instalace"), _T("Installation Break")),
							MB_YESNO | MB_ICONWARNING) == IDYES)
					{
						MemFree(srcbuf);
						MemFree(dstbuf);
						SetPage(PAGESELECT);
						return;
					}
					SetButton(IDN_BUTTONCANC);
				}
				Storno = FALSE;
			}
		}

// posun ukazatele dat
		off += Head.Groups[i].SizeGroup;
	}

// zrušení vypisovaného textu souboru
	EmptyText.SetWindowText(IndWndFile);

// zrušení bufferu
	MemFree(srcbuf);
	MemFree(dstbuf);

// registrace cílové cesty
	SetReg(ProductKey0, _T("LastPath"), InstPath);
	SetReg(ProductKey, _T("Path"), InstPath);

// registrace jazyku
	SetReg(ProductKey0, _T("SetupLang"), X("5", "9"));
	SetReg(ProductKey, _T("SetupLang"), X("5", "9"));

// registrace volby skupin
	CText grpnum;
	for (i = 0; i < GROUPSNUM; i++)
	{
		grpnum.Int(i);
		SetReg(ProductKey, _T("Group") + grpnum, Groups[i].check ? _T('1') : _T('0'));
	}

// inicializace při první instalaci
	if (FirstInst)
	{

// registrace odinstalování (ve Windows NT není)
		HKEY hkey;
		if (::RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,
			UninstalKey0,
			0,
			KEY_QUERY_VALUE,
			&hkey) == ERROR_SUCCESS)
		{
			::RegCloseKey(hkey);
		
			SetReg(UninstalKey, _T("DisplayName"), X(_T("Petr "), _T("Peter ")) + CText(VerzeTxt));
			SetReg(UninstalKey, _T("UninstallString"), path + _T("Uninstal.exe"));
		}

// cesta k dokumentům
		CText dokumenty;
		dokumenty = GetShell(_T("Personal"));
		if (dokumenty.IsEmpty()) dokumenty = X(_T("C:\\Dokumenty"), _T("C:\\Dokuments"));

// vytvoření pracovního adresáře
		if (dokumenty.LastChar() != _T('\\')) dokumenty += _T('\\');
		dokumenty += _T("Peter");
		CreatePath(dokumenty);
		SetReg(ProductKey, _T("User"), dokumenty);

// vytvoření konfiguračního souboru (případný existující soubor se přepíše)
		CText peterini;
		peterini = dokumenty + _T("\\Peter.ini");
		HANDLE file = ::CreateFile(peterini, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file != INVALID_HANDLE_VALUE)
		{
			DWORD writen;
			CText txt("[Global]");
			txt += 13;
			txt += 10;

			txt += "Language=";
			txt += 34;
			txt += X("Czech", "English");
			txt += 34;
			txt += 13;
			txt += 10;

			txt += "LangPrev=";
			txt += 34;
			txt += X("English", "Czech");
			txt += 34;
			txt += 13;
			txt += 10;

			::WriteFile(file, (LPCSTR)txt, txt.Length(), &writen, NULL);

			::CloseHandle(file);
		}

// cesta k menu programů
		CText programs;
		programs = GetShell(_T("Programs"));
		if (programs.LastChar() != _T('\\')) programs += _T('\\');
		programs += X(_T("Petr "), _T("Peter "));
		programs += VerzeTxt;
		CreatePath(programs);
		SetReg(ProductKey, _T("Menu"), programs);
		programs += _T('\\');

// cesta k hlavní ploše
		CText desktop;
		desktop = GetShell(_T("Desktop"));
		SetReg(ProductKey, _T("Desktop"), desktop);
		if (desktop.LastChar() != _T('\\')) desktop += _T('\\');

// přikopírování datových souborů do pracovního adresáře
		CText txt;

// inicializace COM knihovny
		if (::CoInitialize(NULL) == S_OK)
		{

// interface na IShellLink
			IShellLink* psl;
			if (::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
					IID_IShellLink, (void**)&psl) == S_OK)
			{

// interface na IPersistFile
				IPersistFile* ppf;
				if (psl->QueryInterface(IID_IPersistFile, (void **)&ppf) == S_OK)
				{
					WCHAR dest[512];

// uložení odkazu do plochy
					psl->SetPath(path + _T("Peter.exe"));
					psl->SetDescription(X(_T("Petr"), _T("Peter")));
					psl->SetWorkingDirectory(dokumenty);
					txt = desktop;
					txt += X(_T("Petr.lnk"), _T("Peter.lnk"));
					txt.WriteNull(dest);
					ppf->Save(dest, TRUE);

// uložení odkazu do menu - pracovní instalace
					psl->SetPath(path + _T("Peter.exe"));
					psl->SetDescription(X(_T("Petr"), _T("Peter")));
					psl->SetWorkingDirectory(dokumenty);
					txt = programs;
					txt += X(_T("Petr.lnk"), _T("Peter.lnk"));
					txt.WriteNull(dest);
					ppf->Save(dest, TRUE);

// uložení odkazu do menu - správce knihovny
					psl->SetPath(path + _T("Peter.exe"));
					psl->SetDescription(X(_T("Petr s modifikací vzorové knihovny"), _T("Peter with modification of example library")));
					psl->SetWorkingDirectory(InstPath);
					txt = programs;
					txt += X(_T("Petr s modifikací vzorové knihovny.lnk"), _T("Peter with modification of example library.lnk"));
					txt.WriteNull(dest);
					ppf->Save(dest, TRUE);

// uložení odkazu do menu - odinstalování
					psl->SetPath(path + _T("Uninstal.exe"));
					psl->SetDescription(X(_T("Odinstalování"), _T("Uninstall")));
					psl->SetWorkingDirectory(InstPath);
					txt = programs;
					txt += X(_T("Odinstalování.lnk"), _T("Uninstall.lnk"));
					txt.WriteNull(dest);
					ppf->Save(dest, TRUE);

// uvolnění interface IPersistFile
					ppf->Release();
				}

// uvolnění interface na IShellLink
				psl->Release();
			}

// deinicializace COM knihovny
			::CoUninitialize();
		}
	}

// instalace ukončena OK
	SetPage(PAGEOK);
}


/////////////////////////////////////////////////////////////////////////////
// zrušení cesty s podadresáři (neruší soubory)

void DeletePath(CText path)
{
	CText txt;

// doplnění "\" na konec
	if (path.LastChar() != _T('\\')) path += _T('\\');

// vyhledání podadresářů
	WIN32_FIND_DATA fnd;
	HANDLE file = ::FindFirstFile(path + _T("*.*"), &fnd);
	if (file != INVALID_HANDLE_VALUE)
	{
		do {
			if (fnd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				txt = fnd.cFileName;
				if (txt.IsNotEmpty() && (txt != _T(".")) && (txt != _T("..")))
				{
					DeletePath(path + txt);
				}
			}
		} while (::FindNextFile(file, &fnd));

		::FindClose(file);
	}
	path.Delete(path.Length()-1);
	::RemoveDirectory(path);
}

/////////////////////////////////////////////////////////////////////////////
// provedení odinstalování

void DInstall()
{
	BeginWaitCursor();

// cesta k rušení souborů
	CText path = InstPath;
	if (path.LastChar() != _T('\\')) path += _T('\\');

	Storno = FALSE;
	int i,j;
	WIN32_FIND_DATA fnd;

// cesta k dokumentům
	CText dokumenty;
	dokumenty = GetReg(ProductKey, _T("User"));

// cesta k menu
	CText programs;
	programs = GetReg(ProductKey, _T("Menu"));

// cesta k hlavní ploše
	CText desktop;
	desktop = GetReg(ProductKey, _T("Desktop"));
	if (desktop.LastChar() != _T('\\')) desktop += _T('\\');
	desktop += X(_T("Petr.lnk"), _T("Peter.lnk"));

// zrušení registrů cest
	SetReg(ProductKey, _T("Path"), EmptyText);
	DelReg(ProductKey, _T("Path"));
	SetReg(ProductKey, _T("User"), EmptyText);
	DelReg(ProductKey, _T("User"));
	SetReg(ProductKey, _T("Menu"), EmptyText);
	DelReg(ProductKey, _T("Menu"));
	SetReg(ProductKey, _T("Desktop"), EmptyText);
	DelReg(ProductKey, _T("Desktop"));

// zrušení registrů skupin
	CText grpnum;
	for (i = 0; i < GROUPSNUM; i++)
	{
		grpnum.Int(i);
		SetReg(ProductKey, _T("Group") + grpnum, EmptyText);
		DelReg(ProductKey, _T("Group") + grpnum);
	}

// zrušení klíče
	::RegDeleteKey(
		HKEY_LOCAL_MACHINE,
		ProductKey);

// zrušení poslední cesty
	SetReg(ProductKey0, _T("LastPath"), EmptyText);
	DelReg(ProductKey0, _T("LastPath"));

// zrušení registrace odinstalování (ve Windows NT není)
	HKEY hkey;
	if (::RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		UninstalKey0,
		0,
		KEY_QUERY_VALUE,
		&hkey) == ERROR_SUCCESS)
	{
		::RegCloseKey(hkey);

		SetReg(UninstalKey, _T("DisplayName"), EmptyText);
		SetReg(UninstalKey, _T("UninstallString"), EmptyText);
		DelReg(UninstalKey, _T("DisplayName"));
		DelReg(UninstalKey, _T("UninstallString"));

		::RegDeleteKey(
			HKEY_LOCAL_MACHINE,
			UninstalKey);
	}

// zrušení menu
	CText txt;
	if (programs.IsNotEmpty())
	{
		txt = programs;
		if (txt.LastChar() != _T('\\')) txt += _T('\\');
		::DeleteFile(txt + X(_T("Petr.lnk"), _T("Peter.lnk")));
		::DeleteFile(txt + X(_T("Petr s modifikací vzorové knihovny.lnk"), _T("Peter with modification of example library.lnk")));
		::DeleteFile(txt + X(_T("Odinstalování.lnk"), _T("Uninstall.lnk")));
		::RemoveDirectory(programs);
	}

// --------- zrušení odkazu z hlavní plochy (s kontrolou platnosti)

// inicializace COM knihovny
	if (::CoInitialize(NULL) == S_OK)
	{
		BOOL del = FALSE;

// interface na IShellLink
		IShellLink* psl;
		if (::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
				IID_IShellLink, (void**)&psl) == S_OK)
		{

// interface na IPersistFile
			IPersistFile* ppf;
			if (psl->QueryInterface(IID_IPersistFile, (void **)&ppf) == S_OK)
			{
				WCHAR dest[512];

				desktop.WriteNull(dest);
				if (ppf->Load(dest, STGM_SIMPLE | STGM_READ | STGM_SHARE_DENY_NONE) == S_OK)
				{
					TCHAR buf[512];
					if (psl->GetPath(buf, 512, &fnd, 0) == NOERROR)
					{
						txt = buf;
						txt.UpperCase();

						if (psl->GetWorkingDirectory(buf, 512) == NOERROR)
						{
							CText txt2 = buf;

							CText txt3 = path + _T("PETER.EXE");
							txt3.UpperCase();

							if ((txt == txt3) &&
								(txt2 == dokumenty))
							{
								del = TRUE;
							}
						}
					}

// uvolnění interface IPersistFile
					ppf->Release();
				}

// uvolnění interface na IShellLink
				psl->Release();
			}

// deinicializace COM knihovny
			::CoUninitialize();
		}

// zrušení souboru odkazu
		if (del) 
		{
			::DeleteFile(desktop);
		}
	}

// zrušení datových souborů
	FirstInst = TRUE;

// zjištění počtu souborů (kvůli indikaci)
	DataSize = 0;
	DataSizeOK = 0;
	for (i = 0; i < GROUPSNUM; i++)
	{
		DataSize += Head.Groups[i].Files;
	}

// cyklus přes všechny skupiny
	for (i = 0; i < GROUPSNUM; i++)
	{

// cyklus přes všechny soubory
		BYTE* data = Groups[i].files;
		for (j = Head.Groups[i].Files; j > 0; j--)
		{

// jméno souboru
			int size = ((INSTFILE*)data)->Size;
			CText name(((INSTFILE*)data)->Name, ((INSTFILE*)data)->NameN);
			data += 2*sizeof(long) + sizeof(BYTE) + ((INSTFILE*)data)->NameN;
			name = path + name;

// načtení parametrů souboru
			HANDLE file = ::FindFirstFile(name, &fnd);

// zrušení souboru
			if (file != INVALID_HANDLE_VALUE)
			{
				FILETIME datetime;
				::FileTimeToLocalFileTime(&fnd.ftLastWriteTime, &datetime);
				FILETIME datetime2 = datetime;
				*(__int64*)&datetime2 += (__int64)10000000*60*60;
				FILETIME datetime3 = datetime;
				*(__int64*)&datetime3 -= (__int64)10000000*60*60;

				if ((MemCompare(&datetime, &Head.DateTime, sizeof(FILETIME)) ||
					MemCompare(&datetime2, &Head.DateTime, sizeof(FILETIME)) ||
					MemCompare(&datetime3, &Head.DateTime, sizeof(FILETIME))) &&
					((int)fnd.nFileSizeLow == size))
				{
					if (fnd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
					{
						::SetFileAttributes(name, FILE_ATTRIBUTE_NORMAL);
					}
					::DeleteFile(name);
				}

// uzavření hledání
				::FindClose(file);
			}

// zobrazení indikátoru operace
			DataSizeOK++;
			if (IndWnd)
			{
				::SendMessage(IndWnd, PBM_SETPOS, ::MulDiv(DataSizeOK, PROGMAX, DataSize), 0);
			}

// obsluha zpráv
			PumpMessage();
		}
	}

// zrušení souboru INI
	::DeleteFile(path + _T("Peter.ini"));

	::SetFileAttributes(path + _T("Peter.gid"), FILE_ATTRIBUTE_NORMAL);
	::DeleteFile(path + _T("Peter.gid"));

// zrušení souboru Uninstal.exe
	txt = path + _T("Uninstal.exe");
	BOOL res = ::DeleteFile(txt);
	if (Uninst && !res)
	{
		CResource rsrc;
		rsrc.OpenCopy(IDN_DELEXE, _T("EXE"));

		char* dat = (char*)rsrc.Adr();
		int i;
		for (i = rsrc.Size(); i > 0; i--)
		{
			if ((*(long*)dat == 'ELIF') &&
				(*(long*)(dat + 4) == 'ELIF'))
			{
				txt.WriteNull(dat);
				break;
			}
			dat++;
		}

		dat = (char*)rsrc.Adr();
		for (i = rsrc.Size(); i > 0; i--)
		{
			if ((*(long*)dat == 'HTAP') &&
				(*(long*)(dat + 4) == 'HTAP'))
			{
				InstPath.WriteNull(dat);
				break;
			}
			dat++;
		}

		TCHAR tmppath[201];
		tmppath[0] = 0;
		::GetTempPath(200, tmppath);

		CText pth = tmppath;
		if (pth.LastChar() != _T('\\')) pth += _T('\\');

		for (i = 0; i < 1000; i++)
		{
			txt.Int(i);
			txt = pth + _T("~TEMP") + txt + _T(".EXE");

			if ((int)::GetFileAttributes(txt) == -1) break;
		}

		HANDLE file = ::CreateFile(txt, GENERIC_WRITE,
			0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);

		if (file != INVALID_HANDLE_VALUE)
		{
			DWORD writen;
			::WriteFile(file, rsrc.Adr(), rsrc.Size(), &writen, NULL);
			::CloseHandle(file);
			DelExe = txt;
		}
	}

// zrušení adresářů
	DeletePath(InstPath);

	EndWaitCursor();
	SetPage(PAGEDINSTOK);
}


/////////////////////////////////////////////////////////////////////////////
// zjištění informací o volném místu na cílovém disku

void GetDiskSpace()
{
// úprava na plné jméno
	InstPath.TrimLeft();
	InstPath.TrimRight();

	TCHAR buf[1024];
	TCHAR* jmeno;
	int len = ::GetFullPathName(InstPath, 1024, buf, &jmeno);
	if (len > 0)
	{
		InstPath.Term();
		InstPath.Init(buf, len);
		InstPath.SetWindowText(PathWnd);
	}

// disk
	CText txt;
	txt = InstPath.Left(3);

// načtení informací novější funkcí
	if (!pGetDiskFreeSpaceEx ||
		!pGetDiskFreeSpaceEx(txt, &DiskFreeUser, &DiskSize, &DiskFree))

// načtení informací starší metodou
	{
		DWORD sectc;
		DWORD bytes;
		DWORD freec;
		DWORD sizec;

		if (!::GetDiskFreeSpace(txt, &sectc, &bytes, &freec, &sizec))
		{
			sectc = 0;
			bytes = 0;
			freec = 0;
			sizec = 0;
		}

		DiskSize = sectc * bytes * sizec;
		DiskFree = sectc * bytes * freec;
		DiskFreeUser = DiskFree;
	}

// volné místo
	DiskFre = *(long*)((BYTE*)&DiskFree + 2) >> 4;

// zobrazení informací
	txt.Int(DiskFre);
	txt += _T(" MB");
	txt.SetWindowText(DiskFreeNum);
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření čáry nad tlačítky

void CreateLineWnd()
{
	if (LineWnd == NULL)
	{
		LineWnd = ::CreateWindowEx(
			0,												// rozšířený styl
			_T("STATIC"),									// jméno třídy okna
			NULL,											// titulek okna
			SS_ETCHEDHORZ | WS_VISIBLE | WS_CHILD,			// styl okna
			10,												// X
			ClientHeight - 58,								// Y
			ClientWidth - 20,								// šířka
			6,												// výška
			MainFrame,										// rodič
			(HMENU)IDN_LINEWND,								// menu
			hInstance,										// instance
			NULL);											// parametry
	}
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření rámu uživatele

void CreateUserRam()
{
// rám kolem okna
	if (UserRam == NULL)
	{
		UserRam = ::CreateWindowEx(
			0,												// rozšířený styl
			_T("STATIC"),									// jméno třídy okna
			NULL,											// titulek okna
			SS_ETCHEDFRAME | WS_VISIBLE | WS_CHILD,			// styl okna
			320,											// X
			40,												// Y
			ClientWidth - 320 - 10,							// šířka
			150,											// výška
			MainFrame,										// rodič
			(HMENU)IDN_USERRAM,								// menu
			hInstance,										// instance
			NULL);											// parametry
	}
}

/////////////////////////////////////////////////////////////////////////////
// vložení položky do seznamu přepínačů

void InsertCheck(int inx, BOOL check, BOOL instal, LPCTSTR text, LPCTSTR text2)
{
	LVITEM lvi;

	lvi.iSubItem = 0;
	lvi.iItem = inx;
	lvi.iImage = check ? 1 : 0;
	if (instal) lvi.iImage += 2;
	lvi.mask = LVIF_TEXT | LVIF_IMAGE;
	lvi.pszText = (LPTSTR)text;
	::SendMessage(CheckWnd, LVM_INSERTITEM, 0, (LPARAM)&lvi);

	lvi.iSubItem = 1;
	lvi.mask = LVIF_TEXT;
	lvi.pszText = (LPTSTR)text2;
	::SendMessage(CheckWnd, LVM_SETITEM, inx, (LPARAM)&lvi);
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření okna přepínačů

void CreateCheckWnd()
{
	if (CheckWnd == NULL)
	{

// vytvoření okna
		CheckWnd = ::CreateWindowEx(
			WS_EX_CLIENTEDGE,								// rozšířený styl
			WC_LISTVIEW,									// jméno třídy okna
			NULL,											// titulek okna
			LVS_NOCOLUMNHEADER | LVS_REPORT | LVS_SHAREIMAGELISTS | LVS_SINGLESEL |
			WS_VISIBLE | WS_CHILD,							// styl okna
			10,												// X
			40,												// Y
			300,											// šířka
			220,											// výška
			MainFrame,										// rodič
			(HMENU)IDN_CHECKWND,								// menu
			hInstance,										// instance
			NULL);											// parametry

// připojení senzamu ikon pro výběr
		::SendMessage(CheckWnd, LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM)CheckList);

// nastavení fontů v okně
		::SendMessage(CheckWnd, WM_SETFONT, (WPARAM)::GetStockObject(ANSI_VAR_FONT), TRUE);

// vytvoření sloupce přepínačů
		LVCOLUMN lvc;
		lvc.mask = LVCF_FMT | LVCF_WIDTH;
		lvc.fmt = LVCFMT_LEFT;
		lvc.cx = 208;
		::SendMessage(CheckWnd, LVM_INSERTCOLUMN, 0, (LPARAM)&lvc);

// vytvoření sloupce velikostí částí
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_SUBITEM;
		lvc.fmt = LVCFMT_RIGHT;
		lvc.cx = 64;
		lvc.iSubItem = 1;
		::SendMessage(CheckWnd, LVM_INSERTCOLUMN, 1, (LPARAM)&lvc);

// naplnění okna přepínači
		CText txt;
		for (int i = 0; i < GROUPSNUM; i++)
		{
			txt.Int((Head.Groups[i].Size + 1023)/1024);
			txt += _T(" MB");			
			InsertCheck(i, Groups[i].check, Groups[i].instal, Groups[i].name, txt);
		}

// přednastavení výběru na první položku
		LVITEM lvi;
		lvi.iSubItem = 0;
		lvi.iItem = 0;
		lvi.state = LVIS_SELECTED;
		lvi.mask = LVIF_STATE;
		lvi.stateMask = LVIS_SELECTED;
		::SendMessage(CheckWnd, LVM_SETITEMSTATE, 0, (LPARAM)&lvi);
	}
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření nadpisu přepínačů

void CreateCheckNadp()
{
	if (CheckNadp == NULL)
	{
		if (FirstInst)
		{
			CheckNadp = ::CreateWindowEx(
				0,												// rozšířený styl
				_T("STATIC"),									// jméno třídy okna
				X(
					_T("Vyberte instalované části a cílovou složku. Stiskem tlačítka 'Start' zahájíte instalaci."), 
					_T("Select installed parts and destination folder. By pressing 'Start' you begin installation.")),
				SS_CENTER | WS_VISIBLE | WS_CHILD,				// styl okna
				0,												// X
				10,												// Y
				ClientWidth,									// šířka
				18,												// výška
				MainFrame,										// rodič
				(HMENU)IDN_CHECKNADP,							// menu
				hInstance,										// instance
				NULL);											// parametry
		}
		else
		{
			CheckNadp = ::CreateWindowEx(
				0,												// rozšířený styl
				_T("STATIC"),									// jméno třídy okna
				X(
					_T("Změňte výběr nainstalovaných částí. Stiskem tlačítka 'Start' zahájíte změnu instalace."),
					_T("Change selection of installed parts. By pressing 'Start' you begin reinstallation.")),
				SS_CENTER | WS_VISIBLE | WS_CHILD,				// styl okna
				0,												// X
				10,												// Y
				ClientWidth,									// šířka
				18,												// výška
				MainFrame,										// rodič
				(HMENU)IDN_CHECKNADP,							// menu
				hInstance,										// instance
				NULL);											// parametry
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření instalační cesty

void CreatePathWnd()
{
	if (PathWndNadp == NULL)
	{
		PathWndNadp = ::CreateWindowEx(
			0,												// rozšířený styl
			_T("STATIC"),									// jméno třídy okna
			X(_T("Cílová složka:"), _T("Destination folder:")),	// titulek okna
			SS_LEFT | WS_VISIBLE | WS_CHILD,				// styl okna
			10,												// X
			ClientHeight - 112,								// Y
			200,											// šířka
			20,												// výška
			MainFrame,										// rodič
			(HMENU)IDN_PATHWNDNADP,							// menu
			hInstance,										// instance
			NULL);											// parametry

		::SendMessage(PathWndNadp, WM_SETFONT, (WPARAM)::GetStockObject(ANSI_VAR_FONT), TRUE);
	}

	if (PathWnd == NULL)
	{
		PathWnd = ::CreateWindowEx(
			WS_EX_CLIENTEDGE,								// rozšířený styl
			_T("EDIT"),										// jméno třídy okna
			InstPath,										// titulek okna
			ES_AUTOHSCROLL | ES_LEFT | WS_GROUP | WS_TABSTOP | WS_VISIBLE | WS_CHILD,			// styl okna
			20,												// X
			ClientHeight - 93,								// Y
			ClientWidth - 150,								// šířka
			20,												// výška
			MainFrame,										// rodič
			(HMENU)IDN_PATHWND,								// menu
			hInstance,										// instance
			NULL);											// parametry

		::SendMessage(PathWnd, WM_SETFONT, (WPARAM)::GetStockObject(ANSI_VAR_FONT), TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření tlačítek volby jazyku

HWND CreateFlag(int id, int y, LPCTSTR text)
{
	HWND wnd = ::CreateWindowEx(
			0,												// rozšířený styl
			_T("BUTTON"),									// jméno třídy okna
			text,											// titulek okna
			BS_PUSHBUTTON | BS_TEXT | BS_NOTIFY | WS_GROUP | WS_TABSTOP | WS_VISIBLE | WS_CHILD,// styl okna
			270,											// X
			y,												// Y
			150,											// šířka
			44,												// výška
			MainFrame,										// rodič
			(HMENU)id,										// menu
			hInstance,										// instance
			NULL);											// parametry
	::SendMessage(wnd, WM_SETFONT, (WPARAM)::GetStockObject(ANSI_VAR_FONT), TRUE);

	return wnd;
}


void CreateFlags()
{
	FlagCZWnd = CreateFlag(IDN_FLAGWNDCZ, 104, "Česky (Czech)");
	FlagENGWnd = CreateFlag(IDN_FLAGWNDENG, 204, "English");

	HWND foc = FlagENGWnd;

	switch (Jazyk)
	{
	case JAZYKCZ:
		foc = FlagCZWnd;
		break;
	}

	::SetFocus(foc);
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření tlačítka

HWND CreateButton(int id, int x, LPCTSTR text)
{
	HWND wnd = ::CreateWindowEx(
			0,												// rozšířený styl
			_T("BUTTON"),									// jméno třídy okna
			text,											// titulek okna
			BS_PUSHBUTTON | BS_TEXT | BS_NOTIFY | WS_GROUP | WS_TABSTOP | WS_VISIBLE | WS_CHILD,// styl okna
			x,												// X
			ClientHeight - 40,								// Y
			100,											// šířka
			26,												// výška
			MainFrame,										// rodič
			(HMENU)id,										// menu
			hInstance,										// instance
			NULL);											// parametry
	::SendMessage(wnd, WM_SETFONT, (WPARAM)::GetStockObject(ANSI_VAR_FONT), TRUE);

	return wnd;
}

HWND CreateDefButton(int id, int x, LPCTSTR text)
{
	HWND wnd = ::CreateWindowEx(
			0,												// rozšířený styl
			_T("BUTTON"),									// jméno třídy okna
			text,											// titulek okna
			BS_DEFPUSHBUTTON | BS_TEXT | BS_NOTIFY | WS_GROUP | WS_TABSTOP | WS_VISIBLE | WS_CHILD,// styl okna
			x,												// X
			ClientHeight - 40,								// Y
			100,												// šířka
			26,												// výška
			MainFrame,										// rodič
			(HMENU)id,										// menu
			hInstance,										// instance
			NULL);											// parametry
	::SendMessage(wnd, WM_SETFONT, (WPARAM)::GetStockObject(ANSI_VAR_FONT), TRUE);

	return wnd;
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření tlačítka Zpět

void CreateBack()
{
	if (ButtonBack == NULL)
	{
		ButtonBack = CreateButton(IDN_BUTTONBACK, ClientWidth - 20 - 100 - 40 - 100 - 100, X(_T("< Zpět"), _T("< Back")));
	}
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření tlačítka Další

void CreateNext()
{
	if (ButtonNext == NULL)
	{
		switch (AktPage)
		{
		case PAGEISDINST:
		case PAGESELECT:
			ButtonNext = CreateDefButton(IDN_BUTTONNEXT, ClientWidth - 20 - 100 - 40 - 100, X(_T("Start"), _T("Start")));
			break;

		default:
			ButtonNext = CreateDefButton(IDN_BUTTONNEXT, ClientWidth - 20 - 100 - 40 - 100, X(_T("Další >"), _T("Next >")));
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření tlačítka Konec

void CreateExit()
{
	if (ButtonExit == NULL)
	{
		if ((AktPage == PAGEOK) || (AktPage == PAGEDINSTOK) || (AktPage == PAGELANG))
		{
			ButtonExit = CreateButton(IDN_BUTTONEXIT, (ClientWidth - 100)/2, X(_T("Konec"), _T("Quit")));
		}
		else
		{
			ButtonExit = CreateButton(IDN_BUTTONEXIT, ClientWidth - 20 - 100, X(_T("Konec"), _T("Quit")));
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření tlačítka Storno

void CreateCanc()
{
	if (ButtonCanc == NULL)
	{
		ButtonCanc = CreateButton(IDN_BUTTONCANC, (ClientWidth - 100)/2, X(_T("Storno"), _T("Cancel")));
	}
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření tlačítka Procházet

void CreateList()
{
	if (ButtonList == NULL)
	{
		ButtonList = ::CreateWindowEx(
				0,												// rozšířený styl
				_T("BUTTON"),									// jméno třídy okna
				X(_T("Procházet..."), _T("Browse...")),	// titulek okna
				BS_PUSHBUTTON | BS_TEXT | BS_NOTIFY | WS_GROUP | WS_TABSTOP | WS_VISIBLE | WS_CHILD,// styl okna
				ClientWidth - 20 - 100,							// X
				ClientHeight - 100,								// Y
				100,											// šířka
				26,												// výška
				MainFrame,										// rodič
				(HMENU)IDN_BUTTONLIST,							// menu
				hInstance,										// instance
				NULL);											// parametry
		::SendMessage(ButtonList, WM_SETFONT, (WPARAM)::GetStockObject(ANSI_VAR_FONT), TRUE);

		if (!FirstInst)
		{
			::EnableWindow(ButtonList, FALSE);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření indikátoru instalace

void CreateIndWnd()
{
	if (IndWnd == NULL)
	{
		IndWnd = ::CreateWindowEx(
				WS_EX_CLIENTEDGE,								// rozšířený styl
				PROGRESS_CLASS,									// jméno třídy okna
				NULL,											// titulek okna
				WS_CHILD | WS_VISIBLE,							// styl okna
				20,												// X
				200,											// Y
				ClientWidth - 40,								// šířka
				20,												// výška
				MainFrame,										// rodič
				(HMENU)IDN_INDWND,								// menu
				hInstance,										// instance
				NULL);											// parametry

		if (IndWnd)
		{
			::SendMessage(IndWnd, PBM_SETRANGE, 0, MAKELPARAM(0, PROGMAX));
			::SendMessage(IndWnd, PBM_SETPOS, 0, 0);
		}
	}

	if (IndWndNadp == NULL)
	{
		if (AktPage == PAGEDINSTAL)
		{
			IndWndNadp = ::CreateWindowEx(
				0,												// rozšířený styl
				_T("STATIC"),									// jméno třídy okna
				X(_T("Probíhá odinstalování aplikace Petr ..."), _T("Proceeding uninstallation of the Peter application ...")),	// titulek okna
				SS_CENTER | WS_VISIBLE | WS_CHILD,				// styl okna
				30,												// X
				100,											// Y
				ClientWidth - 60,								// šířka
				20,												// výška
				MainFrame,										// rodič
				(HMENU)IDN_INDWNDNADP,							// menu
				hInstance,										// instance
				NULL);											// parametry
		}
		else
		{
			IndWndNadp = ::CreateWindowEx(
				0,												// rozšířený styl
				_T("STATIC"),									// jméno třídy okna
				X(_T("Probíhá instalace aplikace Petr ..."), _T("Proceeding installation of the Peter application ...")),		// titulek okna
				SS_CENTER | WS_VISIBLE | WS_CHILD,				// styl okna
				30,												// X
				100,											// Y
				ClientWidth - 60,								// šířka
				20,												// výška
				MainFrame,										// rodič
				(HMENU)IDN_INDWNDNADP,							// menu
				hInstance,										// instance
				NULL);											// parametry
		}
	}

	if (IndWndFile == NULL)
	{
		IndWndFile = ::CreateWindowEx(
			0,												// rozšířený styl
			_T("STATIC"),									// jméno třídy okna
			NULL,											// titulek okna
			SS_LEFT | WS_VISIBLE | WS_CHILD,				// styl okna
			20,												// X
			170,											// Y
			ClientWidth - 30,								// šířka
			20,												// výška
			MainFrame,										// rodič
			(HMENU)IDN_INDWNDFILE,							// menu
			hInstance,										// instance
			NULL);											// parametry
		::SendMessage(IndWndFile, WM_SETFONT, (WPARAM)::GetStockObject(ANSI_VAR_FONT), TRUE);
	}

	if (IndWnd000 == NULL)
	{
		IndWnd000 = ::CreateWindowEx(
			0,												// rozšířený styl
			_T("STATIC"),									// jméno třídy okna
			_T("0%"),										// titulek okna
			SS_CENTER | WS_VISIBLE | WS_CHILD,				// styl okna
			5,												// X
			225,											// Y
			30,												// šířka
			20,												// výška
			MainFrame,										// rodič
			(HMENU)IDN_INDWND000,							// menu
			hInstance,										// instance
			NULL);											// parametry
		::SendMessage(IndWnd000, WM_SETFONT, (WPARAM)::GetStockObject(ANSI_VAR_FONT), TRUE);
	}

	if (IndWnd025 == NULL)
	{
		IndWnd025 = ::CreateWindowEx(
			0,												// rozšířený styl
			_T("STATIC"),									// jméno třídy okna
			_T("25%"),										// titulek okna
			SS_CENTER | WS_VISIBLE | WS_CHILD,				// styl okna
			5 + 1*(ClientWidth - 40)/4,						// X
			225,											// Y
			30,												// šířka
			20,												// výška
			MainFrame,										// rodič
			(HMENU)IDN_INDWND025,							// menu
			hInstance,										// instance
			NULL);											// parametry
		::SendMessage(IndWnd025, WM_SETFONT, (WPARAM)::GetStockObject(ANSI_VAR_FONT), TRUE);
	}

	if (IndWnd050 == NULL)
	{
		IndWnd050 = ::CreateWindowEx(
			0,												// rozšířený styl
			_T("STATIC"),									// jméno třídy okna
			_T("50%"),										// titulek okna
			SS_CENTER | WS_VISIBLE | WS_CHILD,				// styl okna
			5 + 2*(ClientWidth - 40)/4,						// X
			225,											// Y
			30,												// šířka
			20,												// výška
			MainFrame,										// rodič
			(HMENU)IDN_INDWND050,							// menu
			hInstance,										// instance
			NULL);											// parametry
		::SendMessage(IndWnd050, WM_SETFONT, (WPARAM)::GetStockObject(ANSI_VAR_FONT), TRUE);
	}

	if (IndWnd075 == NULL)
	{
		IndWnd075 = ::CreateWindowEx(
			0,												// rozšířený styl
			_T("STATIC"),									// jméno třídy okna
			_T("75%"),										// titulek okna
			SS_CENTER | WS_VISIBLE | WS_CHILD,				// styl okna
			5 + 3*(ClientWidth - 40)/4,						// X
			225,											// Y
			30,												// šířka
			20,												// výška
			MainFrame,										// rodič
			(HMENU)IDN_INDWND075,							// menu
			hInstance,										// instance
			NULL);											// parametry
		::SendMessage(IndWnd075, WM_SETFONT, (WPARAM)::GetStockObject(ANSI_VAR_FONT), TRUE);
	}

	if (IndWnd100 == NULL)
	{
		IndWnd100 = ::CreateWindowEx(
			0,												// rozšířený styl
			_T("STATIC"),									// jméno třídy okna
			_T("100%"),										// titulek okna
			SS_CENTER | WS_VISIBLE | WS_CHILD,				// styl okna
			-2 + (ClientWidth - 40),						// X
			225,											// Y
			30,												// šířka
			20,												// výška
			MainFrame,										// rodič
			(HMENU)IDN_INDWND100,							// menu
			hInstance,										// instance
			NULL);											// parametry
		::SendMessage(IndWnd100, WM_SETFONT, (WPARAM)::GetStockObject(ANSI_VAR_FONT), TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření informací o disku

void CreateDiskFree()
{
	if (DiskReqNadp == NULL)
	{
		DiskReqNadp = ::CreateWindowEx(
			0,												// rozšířený styl
			_T("STATIC"),									// jméno třídy okna
			X(_T("Požadované volné místo:"), _T("Required free space:")),					// titulek okna
			SS_LEFT | WS_VISIBLE | WS_CHILD,				// styl okna
			330,											// X
			210,											// Y
			180,											// šířka
			18,												// výška
			MainFrame,										// rodič
			(HMENU)IDN_DISKREQNADP,							// menu
			hInstance,										// instance
			NULL);											// parametry
		::SendMessage(DiskReqNadp, WM_SETFONT, (WPARAM)::GetStockObject(ANSI_VAR_FONT), TRUE);
	}

	if (DiskReqNum == NULL)
	{
		DiskReq = 0;

		for (int i = 0; i < GROUPSNUM; i++)
		{
			if (Groups[i].check)
			{
				DiskReq += (Head.Groups[i].Size + 1023)/1024;
			}

			if (Groups[i].instal)
			{
				DiskReq -= (Head.Groups[i].Size + 1023)/1024;
			}
		}
		
		CText txt(_T('0'));
		if (DiskReq > 0)
		{
			txt.Int(DiskReq);
		}
		txt += _T(" MB");

		DiskReqNum = ::CreateWindowEx(
			0,												// rozšířený styl
			_T("STATIC"),									// jméno třídy okna
			txt,											// titulek okna
			SS_RIGHT | WS_VISIBLE | WS_CHILD,				// styl okna
			510,											// X
			210,											// Y
			70,												// šířka
			18,												// výška
			MainFrame,										// rodič
			(HMENU)IDN_DISKREQNADP,							// menu
			hInstance,										// instance
			NULL);											// parametry
		::SendMessage(DiskReqNum, WM_SETFONT, (WPARAM)::GetStockObject(ANSI_VAR_FONT), TRUE);
	}

	if (DiskFreeNadp == NULL)
	{
		DiskFreeNadp = ::CreateWindowEx(
			0,												// rozšířený styl
			_T("STATIC"),									// jméno třídy okna
			X(_T("Volné místo na cílovém disku:"), _T("Free space on destination disk:")),	// titulek okna
			SS_LEFT | WS_VISIBLE | WS_CHILD,				// styl okna
			330,											// X
			235,											// Y
			180,											// šířka
			18,												// výška
			MainFrame,										// rodič
			(HMENU)IDN_DISKFREENADP,							// menu
			hInstance,										// instance
			NULL);											// parametry
		::SendMessage(DiskFreeNadp, WM_SETFONT, (WPARAM)::GetStockObject(ANSI_VAR_FONT), TRUE);
	}

	if (DiskFreeNum == NULL)
	{
		CText txt;
		txt.Int(DiskFre);
		txt += _T(" MB");

		DiskFreeNum = ::CreateWindowEx(
			0,												// rozšířený styl
			_T("STATIC"),									// jméno třídy okna
			txt,											// titulek okna
			SS_RIGHT | WS_VISIBLE | WS_CHILD,				// styl okna
			510,											// X
			235,											// Y
			70,												// šířka
			18,												// výška
			MainFrame,										// rodič
			(HMENU)IDN_DISKFREENUM,							// menu
			hInstance,										// instance
			NULL);											// parametry
		::SendMessage(DiskFreeNum, WM_SETFONT, (WPARAM)::GetStockObject(ANSI_VAR_FONT), TRUE);
	}

	if (ButtonNext)
	{
// blokování tlačítka Next
		BOOL enable = FALSE;
		for (int i = 0; i < GROUPSNUM; i++)
		{
			if (Groups[i].check != Groups[i].instal)
			{
				enable = TRUE;
			}
		}
		::EnableWindow(ButtonNext, enable);
	}
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření nadpisu smlouvy

void CreateSmlouvaNadp()
{
	if (SmlouvaNadp == NULL)
	{
		SmlouvaNadp = ::CreateWindowEx(
			0,												// rozšířený styl
			_T("STATIC"),									// jméno třídy okna
			X(
				_T("Přečtěte si, prosím, Licenční smlouvu a stvrďte souhlas s ní stiskem 'Souhlasím'."),
				_T("Please read License Agreement and confirm agreement with it by pressing 'I Agree'.")),
			SS_CENTER | WS_VISIBLE | WS_CHILD,				// styl okna
			1,												// X
			10,												// Y
			ClientWidth - 2,								// šířka
			18,												// výška
			MainFrame,										// rodič
			(HMENU)IDN_SMLOUVANADP,							// menu
			hInstance,										// instance
			NULL);											// parametry
	}
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření okna s licenční smlouvou (nesmí být UNICODE !!!!)

CText smlouvatxt;
int smlouvaoff;

DWORD CALLBACK CreateSmlouvaCall(DWORD cookie, LPBYTE buf, LONG cb, LONG* pcb)
{
	if (cb < 0) cb = 0;
	if ((smlouvatxt.Length() - smlouvaoff) < cb)
	{
		cb = smlouvatxt.Length() - smlouvaoff;
	}

#ifdef _UNICODE
	ASSERTERROR;
#endif

	MemCopy(buf, smlouvatxt.DataData() + smlouvaoff, cb);
	smlouvaoff += cb;
	*pcb = cb;
	return 0;
}

void CreateSmlouva()
{
	if (SmlouvaWnd == NULL)
	{
		SmlouvaWnd = ::CreateWindowEx(
				WS_EX_CLIENTEDGE,								// rozšířený styl
				_T("RichEdit"),									// jméno třídy okna
				NULL,											// titulek okna
				ES_AUTOVSCROLL | ES_SAVESEL | ES_SUNKEN | WS_VSCROLL |
					ES_READONLY | ES_MULTILINE | WS_CHILD,// styl okna
				10,												// X
				40,												// Y
				ClientWidth - 20,								// šířka
				ClientHeight - 40 - 56,							// výška
				MainFrame,										// rodič
				(HMENU)IDN_SMLOUVAWND,							// menu
				hInstance,										// instance
				NULL);											// parametry

// načtení textu
		CResource res;
		res.Open(X(IDR_LIC_CZ, IDR_LIC_EN), _T("RTF"));
		smlouvatxt.Set((char*)res.Adr(), res.Size());
		smlouvaoff = 0;

// nastavení textu
		EDITSTREAM str;
		str.dwCookie = 0;
		str.dwError = 0;
		str.pfnCallback = CreateSmlouvaCall;	
		::SendMessage(SmlouvaWnd, EM_STREAMIN, SF_RTF, (LPARAM)&str);

// zobrazení okna
		::ShowWindow(SmlouvaWnd, SW_SHOW);
	}
}


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

/////////////////////////////////////////////////////////////////////////////
// výběr cílového adresáře

UINT CALLBACK OFNHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	HWND dlg = ::GetParent(hdlg);

	switch (uiMsg)
	{
	case WM_INITDIALOG:
		{
			CText txt;

			txt = X(_T("&Zobrazit:"), _T("&Display:"));
			txt.SetDialogText(dlg, stc2);

			txt = X(_T("&Název složky:"), _T("Folder &Name:"));
			txt.SetDialogText(dlg, stc3);

			txt = X(_T("&Cesta:"), _T("&Path:"));
			txt.SetDialogText(dlg, stc4);

			CenterWindow(dlg, MainFrame);
		}
		break;
	}

	return FALSE;
}
 

void SelectDest()
{
// příprava struktury k zadání jména souboru
	OPENFILENAME ofn;
	MemFill(&ofn, sizeof(ofn), 0);
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = MainFrame;
	ofn.lpstrFilter = X(_T("Složky souborů\0 . \0\0"), _T("File folders\0 . \0\0"));
	ofn.Flags = OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE | OFN_ENABLEHOOK | OFN_ENABLESIZING |
			OFN_NOVALIDATE | OFN_NOTESTFILECREATE | OFN_LONGNAMES | OFN_EXPLORER;
	ofn.lpstrTitle = X(_T("Cílová složka k instalaci aplikace Petr"), _T("Destination Folder to Install the Peter Application"));
	ofn.lpfnHook = OFNHookProc;

// aktuální adresář
	CText path;
	path = InstPath;
	path.Delete(path.RevFind(_T('\\')));
	ofn.lpstrInitialDir = path;

// přednastavené jméno souboru
	CText name;
	name = InstPath.Right(InstPath.Length() - (InstPath.RevFind(_T('\\')) + 1));
	int n = name.Length() + 1;
	if (n < 1024) n = 1024;

// buffer k zadání jména souboru
	TCHAR* file = (TCHAR*)MemGet(n*sizeof(TCHAR));
	if (file == NULL) return;
	MemFill(file, n*sizeof(TCHAR), 0);
	name.Write(file);
	ofn.lpstrFile = file;
	ofn.nMaxFile = n;

// zadání jména souboru
	if (!::GetSaveFileName(&ofn))
	{
		MemFree(file);
		SetButton(IDN_BUTTONLIST);
		return;
	}

// korekce jména souboru
	name = file;
	MemFree(file);
	name.PathName(0);
	name.TrimLeft();
	name.TrimRight();

	if ((name.Length() < 2) || (name[1] != _T(':')))
	{
		name = _T("c:") + name;
	}

	if ((name.Length() < 3) || (name[2] != _T('\\')))
	{
		name = name.Left(2) + _T('\\') + name.Right(name.Length() - 2);
	}

	name.PathName(0);

	if (name.LastChar() == _T('\\'))
	{
		name += _T("Peter");
	}

// uložení jména souboru
	InstPath = name;
	name.SetWindowText(PathWnd);

// aktualizace volného místa na cílovém disku
	GetDiskSpace();
//	name.Int(DiskFre);
//	name += _T(" MB");
//	name.SetWindowText(DiskFreeNum);

// aktualizace zobrazení okna
	::InvalidateRect(MainFrame, NULL, TRUE);
	::UpdateWindow(MainFrame);

	SetButton(IDN_BUTTONLIST);
}


/////////////////////////////////////////////////////////////////////////////
// vykreslení vlajek

void RePaintFlag()
{
// otevření DC okna
	HDC dc = ::GetDC(MainFrame);

// nastavení vlastních palet
	HPALETTE OldPal = ::SelectPalette(dc, StdPalette, FALSE);
	::RealizePalette(dc);

// vykreslení české vlajky
	StdBitmapInfo->bmiHeader.biWidth = FlagCZ.Width();
	StdBitmapInfo->bmiHeader.biHeight = FlagCZ.Height();

	::StretchDIBits(dc,
		190, 105, FlagCZ.Width(), FlagCZ.Height(),
		0, 0, FlagCZ.Width(), FlagCZ.Height(),
		FlagCZ.DataData(),
		StdBitmapInfo,
		DIB_RGB_COLORS, SRCCOPY);

// vykreslení anglické vlajky
	StdBitmapInfo->bmiHeader.biWidth = FlagENG.Width();
	StdBitmapInfo->bmiHeader.biHeight = FlagENG.Height();

	::StretchDIBits(dc,
		190, 210, FlagENG.Width(), FlagENG.Height(),
		0, 0, FlagENG.Width(), FlagENG.Height(),
		FlagENG.DataData(),
		StdBitmapInfo,
		DIB_RGB_COLORS, SRCCOPY);

// návrat původních palet
	::SelectPalette(dc, OldPal, FALSE);

// uzavření DC okna
	::ReleaseDC(MainFrame, dc);
}


/////////////////////////////////////////////////////////////////////////////
// nastavení stránky instalátoru

void SetPage(int page)
{

// nová aktivní stránka
	AktPage = page;

// zrušení všech prvků
	if (ButtonBack)
	{
		::DestroyWindow(ButtonBack);
		ButtonBack = NULL;
	}

	if (ButtonNext)
	{
		::DestroyWindow(ButtonNext);
		ButtonNext = NULL;
	}

	if (ButtonExit)
	{
		::DestroyWindow(ButtonExit);
		ButtonExit = NULL;
	}

	if (ButtonList)
	{
		::DestroyWindow(ButtonList);
		ButtonList = NULL;
	}

	if (ButtonCanc)
	{
		::DestroyWindow(ButtonCanc);
		ButtonCanc = NULL;
	}

	if (LineWnd)
	{
		::DestroyWindow(LineWnd);
		LineWnd = NULL;
	}

	if (CheckWnd)
	{
		::DestroyWindow(CheckWnd);
		CheckWnd = NULL;
	}

	if (CheckNadp)
	{
		::DestroyWindow(CheckNadp);
		CheckNadp = NULL;
	}

	if (SmlouvaWnd)
	{
		::DestroyWindow(SmlouvaWnd);
		SmlouvaWnd = NULL;
	}

	if (SmlouvaNadp)
	{
		::DestroyWindow(SmlouvaNadp);
		SmlouvaNadp = NULL;
	}

	if (PathWnd)
	{
		::DestroyWindow(PathWnd);
		PathWnd = NULL;
	}

	if (PathWndNadp)
	{
		::DestroyWindow(PathWndNadp);
		PathWndNadp = NULL;
	}

	if (UserRam)
	{
		::DestroyWindow(UserRam);
		UserRam = NULL;
	}

	if (LicTextNadp)
	{
		::DestroyWindow(LicTextNadp);
		LicTextNadp = NULL;
	}

	if (LicTextNum)
	{
		::DestroyWindow(LicTextNum);
		LicTextNum = NULL;
	}

	if (LicTextName)
	{
		::DestroyWindow(LicTextName);
		LicTextName = NULL;
	}

	if (LicTextAdr)
	{
		::DestroyWindow(LicTextAdr);
		LicTextAdr = NULL;
	}

	if (LicTextMest)
	{
		::DestroyWindow(LicTextMest);
		LicTextMest = NULL;
	}

	if (LicTextIco)
	{
		::DestroyWindow(LicTextIco);
		LicTextIco = NULL;
	}

	if (DiskReqNadp)
	{
		::DestroyWindow(DiskReqNadp);
		DiskReqNadp = NULL;
	}

	if (DiskReqNum)
	{
		::DestroyWindow(DiskReqNum);
		DiskReqNum = NULL;
	}

	if (DiskFreeNadp)
	{
		::DestroyWindow(DiskFreeNadp);
		DiskFreeNadp = NULL;
	}

	if (DiskFreeNum)
	{
		::DestroyWindow(DiskFreeNum);
		DiskFreeNum = NULL;
	}

	if (IndWnd)
	{
		::DestroyWindow(IndWnd);
		IndWnd = NULL;
	}

	if (IndWndNadp)
	{
		::DestroyWindow(IndWndNadp);
		IndWndNadp = NULL;
	}

	if (IndWndFile)
	{
		::DestroyWindow(IndWndFile);
		IndWndFile = NULL;
	}

	if (IndWnd000)
	{
		::DestroyWindow(IndWnd000);
		IndWnd000 = NULL;
	}

	if (IndWnd025)
	{
		::DestroyWindow(IndWnd025);
		IndWnd025 = NULL;
	}

	if (IndWnd050)
	{
		::DestroyWindow(IndWnd050);
		IndWnd050 = NULL;
	}

	if (IndWnd075)
	{
		::DestroyWindow(IndWnd075);
		IndWnd075 = NULL;
	}

	if (IndWnd100)
	{
		::DestroyWindow(IndWnd100);
		IndWnd100 = NULL;
	}

	if (FlagCZWnd)
	{
		::DestroyWindow(FlagCZWnd);
		FlagCZWnd = NULL;
	}

	if (FlagENGWnd)
	{
		::DestroyWindow(FlagENGWnd);
		FlagENGWnd = NULL;
	}

// vytvoření nového okna
	switch (page)
	{
// výběr jazyku
	case PAGELANG:
		RePaint();						// vymazání podkladu
		CreateExit();					// vytvoření tlačítka Konec
		CreateFlags();					// vytvoření tlačítek vlajek
		CreateLineWnd();				// vytvoření čáry nad tlačítky
		::SetCursor(CurArrow);			// kurzor - šipka
		RePaintFlag();					// vykreslení vlajek
		break;

// hlavní okno
	case PAGEMAIN:
		if (FirstInst)
		{
			InstPath = GetProgramFiles();
			if (InstPath.IsEmpty()) InstPath = _T("C:\\Program Files\\");
			if (InstPath.LastChar() != _T('\\')) InstPath += _T('\\');

			InstPath += _T("Peter");

			for (int i = 0; i < GROUPSNUM; i++)
			{
				Groups[i].check = Groups[i].initcheck;
			}
		}
		else
		{
			InstPath = GetReg(ProductKey, _T("Path"));
			for (int i = 0; i < GROUPSNUM; i++)
			{
				Groups[i].check = Groups[i].instal;
			}
		}
		
		::SetCursor(CurPeter);			// kurzor - Petr
		RePaint();						// vykreslení okna
		break;

// okno výběru složek	
	case PAGESELECT:
		RePaint();						// vymazání podkladu
		GetDiskSpace();					// načtení informací o cílovém disku
		CreateBack();					// vytvoření tlačítka Zpět
		CreateNext();					// vytvoření tlačítka Další
		CreateExit();					// vytvoření tlačítka Konec
		CreateList();					// vytvoření tlačítka Procházet
		CreateLineWnd();				// vytvoření čáry nad tlačítky
		CreateCheckWnd();				// vytvoření pole přepínačů
		CreateCheckNadp();				// vytvoření nadpisu přepínačů
		CreatePathWnd();				// vytvoření textu cesty
		CreateUserRam();				// vytvoření uživatelského rámu
		CreateDiskFree();				// zobrazení informací o disku

		::SetFocus(ButtonNext);			// fokus na tlačítko Next
		::SetCursor(CurArrow);			// kurzor - šipka
		break;	

// okno instalace
	case PAGEINSTAL:
		RePaint();						// vymazání podkladu
		CreateCanc();					// vytvoření tlačítka Storno
		CreateIndWnd();					// vytvoření indikátoru
		CreateLineWnd();				// vytvoření čáry nad tlačítky

		::SetFocus(ButtonCanc);			// fokus na tlačítko Storno
		::SetCursor(CurArrow);			// kurzor - šipka

		Install();						// provedení instalace (příp. přerušení)
		break;	

// okno odinstalace
	case PAGEDINSTAL:
		RePaint();						// vymazání podkladu
		CreateCanc();					// vytvoření tlačítka Storno (to jen aby tam něco bylo)
		::EnableWindow(ButtonCanc, FALSE);	// zákaz tlačítka Storno
		CreateIndWnd();					// vytvoření indikátoru
		CreateLineWnd();				// vytvoření čáry nad tlačítky

		::SetFocus(ButtonCanc);			// fokus na tlačítko Storno
		::SetCursor(CurArrow);			// kurzor - šipka

		DInstall();						// provedení odinstalace
		break;	

// okno potvrzení odinstalování
	case PAGEISDINST:
		RePaint();						// vymazání podkladu
		if (!Uninst)
		{
			CreateBack();					// vytvoření tlačítka Zpět
		}
		CreateNext();					// vytvoření tlačítka Další
		CreateExit();					// vytvoření tlačítka Konec
		CreateLineWnd();				// vytvoření čáry nad tlačítky

		::SetFocus(ButtonNext);			// fokus na tlačítko Next
		::SetCursor(CurArrow);			// kurzor - šipka
		RePaintDInst();					// zobrazení obrázku
		break;	

// okno OK
	case PAGEOK:
		RePaint();						// vymazání podkladu
		CreateExit();					// vytvoření tlačítka Storno
		CreateLineWnd();				// vytvoření čáry nad tlačítky

		::SetFocus(ButtonExit);			// fokus na tlačítko Next
		::SetCursor(CurArrow);			// kurzor - šipka
		RePaintOK();					// zobrazení obrázku OK
		break;	

// okno odinstalování OK
	case PAGEDINSTOK:
		RePaint();						// vymazání podkladu
		CreateExit();					// vytvoření tlačítka Storno
		CreateLineWnd();				// vytvoření čáry nad tlačítky

		::SetFocus(ButtonExit);			// fokus na tlačítko Next
		::SetCursor(CurArrow);			// kurzor - šipka
		RePaintDInstOK();				// zobrazení obrázku OK
		break;	
	}
}


/////////////////////////////////////////////////////////////////////////////
// překreslení okna

void RePaint()
{
// lokální proměnné
	int srcinc;
	BYTE* src;
	BYTE* dst;
	int i,j;

// inicializační vymazání bufferu
	MemFill(ClientBuf, ClientWidth * ClientHeight, (char)(BYTE)BackCol);

// vykreslení bude jen pro hlavní okno, v ostatních bude vymazání
	if (AktPage == PAGEMAIN)
	{

// přenesení stínu (jen je-li kurzor myši v okně)
		if ((OffX != MOUSEINV) && (OffY != MOUSEINV))
		{
			srcinc = Shadow.Width();
			src = Shadow.DataData();
			dst = ClientBuf;
			int width = ClientWidth;
			int height = ClientHeight;

			if (OffX < 0)
			{
				width += OffX;
				dst -= OffX;
			}
			else
			{
				src += OffX;
				if (OffX + width > srcinc)
				{
					width = srcinc - OffX;
				}
			}

			if (OffY < 0)
			{
				height += OffY;
				dst -= OffY*ClientWidth;
			}
			else
			{
				src += OffY*srcinc;
				if (OffY + height > Shadow.Height())
				{
					height = Shadow.Height() - OffY;
				}
			}

			if (width > 0)
			{
				for (i = height; i > 0; i--)
				{
					MemCopy(dst, src, width);
					dst += ClientWidth;
					src += srcinc;
				}
			}
		}

// adresa obrázku k vykreslení
		switch (KeySelect)
		{
		case 0:
			src = Picture1.DataData();
			KeyDisp = 1;
			break;

		case 1:
			src = Picture2.DataData();
			KeyDisp = 2;
			break;

		case 2:
			src = Picture3.DataData();
			KeyDisp = 3;
			break;

		case 3:
			src = Picture4.DataData();
			KeyDisp = 4;
			break;

		default:
			if ((RelX < 0) || (RelX >= ClientWidth) || (RelY < MENUTOP) || (RelY >= MENUTOP + 4*MENUHEIGHT))
			{
				src = Picture0.DataData();
				KeyDisp = 0;
			}
			else
			{
				if (RelY < MENUTOP + MENUHEIGHT)
				{
					src = Picture1.DataData();
					KeyDisp = 1;
				}
				else
				{
					if (RelY < MENUTOP + 2*MENUHEIGHT)
					{
						src = Picture2.DataData();
						KeyDisp = 2;
					}
					else
					{
						if (RelY < MENUTOP + 3*MENUHEIGHT)
						{
							src = Picture3.DataData();
							KeyDisp = 3;
						}
						else
						{
							src = Picture4.DataData();
							KeyDisp = 4;
						}
					}
				}
			}
		}

// přidání obrázku
		dst = ClientBuf;
		for (i = ClientHeight; i > 0; i--)
		{
			for (j = ClientWidth; j > 0; j--)
			{
				if (*src != BackCol) *dst = *src;
				src++;
				dst++;
			}
		}
	}

// přidání znaku
	if (AktPage == PAGEMAIN)
	{
		src = GemLogo.DataData() + GemtreeFaze*40;

		dst = ClientBuf + (ClientHeight - GemtreeY - 40)*ClientWidth + GemtreeX2;

		for (i = 40; i > 0; i--)
		{
			for (j = 40; j > 0; j--)
			{
				if (*src != BackCol) *dst = *src;
				src++;
				dst++;
			}
			src += 90*40 - 40;
			dst += ClientWidth - 40;
		}
	}

// otevření DC okna
	HDC dc = ::GetDC(MainFrame);

// nastavení vlastních palet
	HPALETTE OldPal = ::SelectPalette(dc, StdPalette, FALSE);
	::RealizePalette(dc);

// vykreslení plochy
	StdBitmapInfo->bmiHeader.biWidth = ClientWidth;
	StdBitmapInfo->bmiHeader.biHeight = ClientHeight;

	::StretchDIBits(dc,
		0, 0, ClientWidth, ClientHeight,
		0, 0, ClientWidth, ClientHeight, 
		ClientBuf,
		StdBitmapInfo,
		DIB_RGB_COLORS, SRCCOPY);

// návrat původních palet
	::SelectPalette(dc, OldPal, FALSE);

// uzavření DC okna
	::ReleaseDC(MainFrame, dc);
}


/////////////////////////////////////////////////////////////////////////////
// překreslení znaku Gemtree

void RePaintGemtree()
{

// otevření DC okna
	HDC dc = ::GetDC(MainFrame);

// nastavení vlastních palet
	HPALETTE OldPal = ::SelectPalette(dc, StdPalette, FALSE);
	::RealizePalette(dc);

// vykreslení plochy
	StdBitmapInfo->bmiHeader.biWidth = 40*90;
	StdBitmapInfo->bmiHeader.biHeight = 40;

	::StretchDIBits(dc,
		GemtreeX2, GemtreeY, 40, 40,
		GemtreeFaze*40, 0, 40, 40,
		GemLogo.DataData(),
		StdBitmapInfo,
		DIB_RGB_COLORS, SRCCOPY);

// návrat původních palet
	::SelectPalette(dc, OldPal, FALSE);

// uzavření DC okna
	::ReleaseDC(MainFrame, dc);
}


/////////////////////////////////////////////////////////////////////////////
// překreslení okna instalace OK

void RePaintOK()
{

// otevření DC okna
	HDC dc = ::GetDC(MainFrame);

// nastavení vlastních palet
	HPALETTE OldPal = ::SelectPalette(dc, StdPalette, FALSE);
	::RealizePalette(dc);

// příprava obrázku
	CPicture pic;
	pic = InstalOK;

// vykreslení plochy
	StdBitmapInfo->bmiHeader.biWidth = pic.Width();
	StdBitmapInfo->bmiHeader.biHeight = pic.Height();

	::StretchDIBits(dc,
		(ClientWidth - pic.Width())/2, 15, pic.Width(), pic.Height(),
		0, 0, pic.Width(), pic.Height(),
		pic.DataData(),
		StdBitmapInfo,
		DIB_RGB_COLORS, SRCCOPY);

// návrat původních palet
	::SelectPalette(dc, OldPal, FALSE);

// uzavření DC okna
	::ReleaseDC(MainFrame, dc);
}

/////////////////////////////////////////////////////////////////////////////
// překreslení okna odinstalace OK

void RePaintDInstOK()
{

// otevření DC okna
	HDC dc = ::GetDC(MainFrame);

// nastavení vlastních palet
	HPALETTE OldPal = ::SelectPalette(dc, StdPalette, FALSE);
	::RealizePalette(dc);

// příprava obrázku
	CPicture pic;
	pic = DInstalOK;

// vykreslení plochy
	StdBitmapInfo->bmiHeader.biWidth = pic.Width();
	StdBitmapInfo->bmiHeader.biHeight = pic.Height();

	::StretchDIBits(dc,
		(ClientWidth - pic.Width())/2, 15, pic.Width(), pic.Height(),
		0, 0, pic.Width(), pic.Height(),
		pic.DataData(),
		StdBitmapInfo,
		DIB_RGB_COLORS, SRCCOPY);

// návrat původních palet
	::SelectPalette(dc, OldPal, FALSE);

// uzavření DC okna
	::ReleaseDC(MainFrame, dc);
}

/////////////////////////////////////////////////////////////////////////////
// překreslení okna odinstalace

void RePaintDInst()
{

// otevření DC okna
	HDC dc = ::GetDC(MainFrame);

// nastavení vlastních palet
	HPALETTE OldPal = ::SelectPalette(dc, StdPalette, FALSE);
	::RealizePalette(dc);

// příprava obrázku
	CPicture pic;
	pic = DInstal;

// vykreslení plochy
	StdBitmapInfo->bmiHeader.biWidth = pic.Width();
	StdBitmapInfo->bmiHeader.biHeight = pic.Height();

	::StretchDIBits(dc,
		(ClientWidth - pic.Width())/2, 15, pic.Width(), pic.Height(),
		0, 0, pic.Width(), pic.Height(),
		pic.DataData(),
		StdBitmapInfo,
		DIB_RGB_COLORS, SRCCOPY);

// návrat původních palet
	::SelectPalette(dc, OldPal, FALSE);

// uzavření DC okna
	::ReleaseDC(MainFrame, dc);
}

/////////////////////////////////////////////////////////////////////////////
// obsluha zpráv okna

LRESULT CALLBACK WindowProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_CLOSE:
		::PostQuitMessage(0);
		return 0;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			::BeginPaint(hWnd, &ps);
			::EndPaint(hWnd, &ps);
		}

		if (AktPage == PAGELANG)
		{
			RePaintFlag();
		}

		if (AktPage == PAGEMAIN)
		{
			RePaint();
		}

		if (AktPage == PAGEOK)
		{
			RePaintOK();
		}

		if (AktPage == PAGEISDINST)
		{
			RePaintDInst();
		}

		if (AktPage == PAGEDINSTOK)
		{
			RePaintDInstOK();
		}
		return 0;

	case WM_COMMAND:
		switch (HIWORD(wParam))
		{
		case 0:
		case 1:
			switch (LOWORD(wParam))
			{
			case IDN_FLAGWNDCZ:
				Jazyk = JAZYKCZ;
				InitPic();
				SetPage(PAGEMAIN);
				return 0;

			case IDN_FLAGWNDENG:
				Jazyk = JAZYKENG;
				InitPic();
				SetPage(PAGEMAIN);
				return 0;

			case IDN_BUTTONEXIT:
				::ShowWindow(MainFrame, SW_HIDE);
				::SendMessage(MainFrame, WM_CLOSE, 0, 0);
				return 0;

			case IDN_BUTTONBACK:
				switch (AktPage)
				{
				case PAGESELECT:
					SetPage(PAGEMAIN);
					return 0;

				case PAGEISDINST:
					SetPage(PAGEMAIN);
					return 0;
				}
				return 0;

			case IDN_BUTTONNEXT:
				switch (AktPage)
				{
				case PAGESELECT:
					SetPage(PAGEINSTAL);
					return 0;

				case PAGEISDINST:
					SetPage(PAGEDINSTAL);
					return 0;
				}
				return 0;

			case IDN_BUTTONLIST:
				switch (AktPage)
				{
				case PAGESELECT:
					SelectDest();
					return 0;
				}
				return 0;

			case IDN_BUTTONCANC:
				Storno = TRUE;
				return 0;
			}
			break;

		case EN_KILLFOCUS:
			switch (LOWORD(wParam))
			{
			case IDN_PATHWND:
				if (PathWnd != NULL)
				{
					InstPath.GetWindowText(PathWnd);
					GetDiskSpace();
				}
				return 0;	
			}
			break;
		}
		break;

	case WM_NOTIFY:					// 0x04E
		{
			NMHDR* nmhdr = ((NMHDR*)lParam);
			switch (nmhdr->code)
			{
			case NM_DBLCLK:
			case NM_CLICK:
				if (nmhdr->hwndFrom == CheckWnd)
				{
					int inx = ::SendMessage(CheckWnd, LVM_GETNEXTITEM, (WPARAM)-1, LVNI_SELECTED);
					if ((DWORD)inx < (DWORD)GROUPSNUM)
					{
						Groups[inx].check = !Groups[inx].check;
						LVITEM lvi;
						lvi.mask = LVIF_IMAGE;
						lvi.iItem = inx;
						lvi.iSubItem = 0;
						lvi.iImage = Groups[inx].check ? 1 : 0;
						if (Groups[inx].instal) lvi.iImage += 2;
						::SendMessage(CheckWnd, LVM_SETITEM, 0, (LPARAM)&lvi);
						DispDiskReq();
					}
				}
				break;
			}
		}
		break;

	case WM_SETCURSOR:
		if (Waiting)
		{
			::SetCursor(CurWait);
		}
		else
		{
			if (AktPage == PAGEMAIN)
			{
				if ((KeySelect != -1) && (RelY >= MENUTOP))
				{
					KeySelect = -1;
					RePaint();
				}

				POINT cur;
				::GetCursorPos(&cur);
				::ScreenToClient(MainFrame, &cur);
				
				if ((cur.x < 0) || 
					(cur.x >= ClientWidth) ||
					(cur.y < 0) ||
					(cur.y >= ClientHeight))
				{
					break;
				}

				if ((cur.y < MENUTOP) ||
					(cur.y >= MENUTOP + 4*MENUHEIGHT))
				{
					::SetCursor(CurArrow);
				}
				else
				{
					::SetCursor(CurPeter);
				}
			}
			else
			{
				::SetCursor(CurArrow);
			}
		}
		return TRUE;

	case WM_TIMER:
		{
// souřadnice kurzoru myši
			POINT cur;
			::GetCursorPos(&cur);

// souřadnice středu okna
			POINT win;
			win.x = 0;
			win.y = 0;
			::ClientToScreen(MainFrame, &win);

// relativní pozice kurzoru myši v okně
			int oldx = RelX;
			int oldy = RelY;
			RelX = cur.x - win.x;
			RelY = cur.y - win.y;

// výpočet posunu obrázku
			int offx =  - RelX + RelX*Shadow.Width()/ClientWidth;
			int offy = Shadow.Height() - ClientHeight
						+ RelY - RelY*Shadow.Height()/ClientHeight;

// při změně posunu překreslení okna
			GemtreeFaze++;
			if (GemtreeFaze >= GemtreeN) GemtreeFaze = 0;

			if ((oldx != RelX) || (oldy != RelY))
			{
				OffX = offx;
				OffY = offy;
				if (AktPage == PAGEMAIN)
				{
					RePaint();
				}
			}
			else
			{
				if (AktPage == PAGEMAIN)
				{
					if (RelY < -150)
					{
						RePaint();			// aby byl vidět stín
					}
					else
					{
						RePaintGemtree();
					}
				}
			}

			return 0;
		}

	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORDLG:
	case WM_CTLCOLORMSGBOX:
	case WM_CTLCOLORSCROLLBAR:
		{
			HDC dc = (HDC)wParam;
			::SetTextColor(dc, 0);
			::SetBkColor(dc, BackColCol);
			return (LRESULT) BackBrush;
		}
		break;
	}

	return ::DefWindowProc(hWnd, nMsg, wParam, lParam);
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření hlavního okna aplikace


bool MainFrameCreate()
{
// registrace třídy okna
	WNDCLASS wcl;
	wcl.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcl.lpfnWndProc = WindowProc;
	wcl.cbClsExtra = 0;
	wcl.cbWndExtra = 0;
	wcl.hInstance = hInstance;
	wcl.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDN_MAINFRAME));
	wcl.hCursor = NULL;
	wcl.hbrBackground =	BackBrush;
	wcl.lpszMenuName = NULL;
	wcl.lpszClassName = MainFrameClass;
	::RegisterClass(&wcl);

// zjištění požadovaných rozměrů okna
	RECT rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = ClientWidth;
	rc.bottom = ClientHeight;
	::AdjustWindowRectEx(&rc, WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_VISIBLE, FALSE, WS_EX_APPWINDOW | WS_EX_CONTROLPARENT | WS_EX_DLGMODALFRAME);
	MainFrameWidth = rc.right - rc.left;
	MainFrameHeight = rc.bottom - rc.top;

// vytvoření hlavního okna
	MainFrame = ::CreateWindowEx(
		WS_EX_APPWINDOW | WS_EX_CONTROLPARENT | WS_EX_DLGMODALFRAME,// rozšířený styl
		MainFrameClass,									// jméno třídy okna
		"",												// titulek okna
		WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,// styl okna
		(ScreenWidth - MainFrameWidth)/2,				// X
		(ScreenHeight - MainFrameHeight)/2,				// Y
		MainFrameWidth,									// šířka
		MainFrameHeight,								// výška
		NULL,											// rodič
		NULL,											// menu
		hInstance,										// instance
		NULL);											// parametry
	if (MainFrame == NULL) return false;

	return true;
}
