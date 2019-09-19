
#include "Main.h"

//////////////////////////////////////////////////////////////////////////////
// inicializaèní segmenty CRT (konstruktory a destruktory)

typedef void (__cdecl *_PVFV)(void);		// ukazatel na funkci void fnc(void)
typedef void (*PFV)(void);					// funkce void fnc(void)

#pragma data_seg(".CRT$XCA")
_PVFV	__xc_a[] = { NULL };				// konstruktory C++
#pragma data_seg(".CRT$XCZ")
_PVFV	__xc_z[] = { NULL };

#pragma data_seg()							// resetování na bìžnou datovou sekci

#pragma comment(linker, "/MERGE:.CRT=.data")	// pøipojení CRT sekcí do datové sekce


//////////////////////////////////////////////////////////////////////////////
// globální promìnné

CString		CommandLine;				// pøíkazový øádek
int			VerzeOS;					// verze systému
HINSTANCE	hInstance = NULL;			// instance programu

BYTE*		StdPalImport;				// tabulka importu palet
BYTE*		StdPalImportDither;			// tabulka importu palet s ditheringem
BITMAPINFO* StdBitmapInfo;				// standardní záhlaví BMP
BYTE*		KonvPal;					// konverzní tabulka palet


bool	ConsoleOn = false;		// pøíznak režimu konzoly
HANDLE	ConIn = INVALID_HANDLE_VALUE;	// handle pro vstup z konzoly
HANDLE	ConOut = INVALID_HANDLE_VALUE;	// handle pro výstup na konzolu
HANDLE	ConErr = INVALID_HANDLE_VALUE;	// handle pro chybový výstup na konzolu

bool	Dither = true;					// použít dithering

//////////////////////////////////////////////////////////////////////////////
// prázdné objekty (napø. pro návrat neplatné položky z funkce)

CString		EmptyString;				// prázdný øetìzec
CPicture	EmptyPicture;				// prázdný obrázek
CSprite		EmptySprite;				// prázdný sprajt

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

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// po zmìnì palet zruš soubor PALIMP.DAT a spus DEBUG verzi pro nové vygenerování
// (pozor - generování mùže trvat desítky sekund). Potom znovu pøeklad.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// tabulka odstínù standardních barev (0 až 255) - poøadí B, G, R, F,
const RGBQUAD ColColTab[] = 
{ 
	0,		0,		255,		0,			//  0: èervená
	72,		72,		255,		0,			//  1:
	116,	116,	255,		0,			//  2:
	158,	166,	255,		0,			//  3:
	210,	220,	255,		0,			//  4:
	0,		114,	255,		0,			//  5: hnìdá
	150,	198,	255,		0,			//  6:
	0,		182,	255,		0,			//  7: oranžová
	96,		192,	255,		0,			//  8:
	0,		255,	255,		0,			//  9: žlutá
	128,	255,	255,		0,			// 10:
	192,	255,	255,		0,			// 11: svìtle žlutá
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
	255,	128,	255,		0,			// 31: svìtle fialová
	138,	0,		255,		0,			// 32: fialovì èervená
	192,	128,	255,		0,			// 33:
	210,	186,	255,		0,			// 34:

	255,	255,	255,		0,			// 35: bílá (nepoužije se, vygeneruje se)
	96,		96,		96,			0,			// 36: šedá (nepoužije se, vygeneruje se)
};

const int ColLev = sizeof(ColLevTab)/sizeof(ColLevTab[0]);	// poèet hladin barev
const int ColCol = sizeof(ColColTab)/sizeof(ColColTab[0]);	// poèet odstínù barev
const int StdColors = ResCols + ColCol*ColLev;			// poèet vlastních palet (zaèínají od 0)
const BYTE WhiteCol = StdColors - 2*ColLev;			// bílá barva
const BYTE BlackCol = StdColors - 1;				// èerná barva

//////////////////////////////////////////////////////////////////////////////
// lokální promìnné

#ifdef _MT
static	CRITICAL_SECTION	ExitCriticalSection;	// kritická sekce pro konec programu
#endif

//CString UvText(  _T("Pov2Spr v1.0 - konverze obrázkù na sprajt; (c) Miroslav Nìmeèek\n"));
//CString HelpText(_T("   POV2SPR vstup výstup smìrù klid pohyb prodleva hladina krok dither\n")
//				 _T("               vstup .... vstupní soubor BMP prvního obrázku\n")
//				 _T("               výstup ... výstupní soubor SPR sprajtu\n")
//				 _T("               smìrù .... poèet smìrù 0 až 1000\n")
//				 _T("               klid ..... poèet klidových fází 1 až 1000\n")
//				 _T("               pohyb .... poèet fází pohybu 0 až 1000\n")
//				 _T("               prodleva . prodleva mezi fázemi v milisekundách\n")
//				 _T("               hladina .. hladina k zobrazení, 0=pøedmìty\n")
//				 _T("               krok ..... poèet fází na jednotkovou vzdálenost\n")
//				 _T("               dither ... použít dithering 1=ano, 0=ne\n")
//				 _T("stisknìte <Enter>... "));
//
//CString WriteErr1(_T("Chyba zápisu do výstupního souboru "));
//CString WriteErr2(_T("!\n"));
//
//CString ReadErr1(_T("Chyba ètení ze vstupního souboru "));
//CString ReadErr2(_T("!\n"));

CString UvText(  _T("Pov2Spr v1.1 - pictures to sprite conversion; (c) Ing. Miroslav Nemecek\n"));
CString HelpText(_T("   POV2SPR in out dir still move delay level step dither\n")
				 _T("               in ...... input file BMP of the first picture\n")
				 _T("               out ..... output file SPR of the sprite\n")
				 _T("               dir ..... number of directions 0 to 1000\n")
				 _T("               still ... number of standstill phases 1 to 1000\n")
				 _T("               move .... number of moving phases 0 to 1000\n")
				 _T("               delay ... delay between phases in milliseconds\n")
				 _T("               level ... level to be displayed, 0=items\n")
				 _T("               step .... number of phases per one-step distance\n")
				 _T("               dither .. use dithering 1=yes, 0=no\n")
				 _T("press <Enter>... "));

CString WriteErr1(_T("Error writing into output file "));
CString WriteErr2(_T("!\n"));

CString ReadErr1(_T("Error reading from input file "));
CString ReadErr2(_T("!\n"));

CString InName;				// vstupní soubor
CString OutName;			// výstupní soubor
CPicture Picture;			// vstupní obrázek
CSprite Sprite;				// výstupní sprajt

/***************************************************************************\
*																			*
*									konzola									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// zapnutí/vypnutí konzoly

void ConsoleOnSet(bool on)
{
	if (ConsoleOn == on) return;
	ConsoleOn = on;

// zapnutí konzoly
	if (on)
	{
		if ((ConIn == INVALID_HANDLE_VALUE) || (ConIn == NULL))
		{
			ConIn = ::GetStdHandle(STD_INPUT_HANDLE);
		}

		if ((ConOut == INVALID_HANDLE_VALUE) || (ConOut == NULL))
		{
			ConOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
		}

		if ((ConErr == INVALID_HANDLE_VALUE) || (ConErr == NULL))
		{
			ConErr = ::GetStdHandle(STD_ERROR_HANDLE);
		}

	}
}


/////////////////////////////////////////////////////////////////////////////
// výstup textu na konzolu

void ConsoleOut(const CString& text)
{
	ConsoleOnSet(true);
		DWORD writen;
		int len = text.Length();
		char* buf = (char*)MemGet(len);
		::CharToOemBuff(text.DataData(), buf, len);
		::WriteFile(ConOut, buf, len, &writen, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// výstup chybového textu na konzolu

void ConsoleErr(const CString& text)
{
	ConsoleOnSet(true);
		DWORD writen;
		int len = text.Length();
		char* buf = (char*)MemGet(len);
		::CharToOemBuff(text.DataData(), buf, len);
		::WriteFile(ConErr, buf, len, &writen, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// vstup textu z konzoly

void ConsoleIn(CString& text)
{
	ConsoleOnSet(true);
		DWORD readen = 0;
		char* bufoem = (char*)MemGet(0x2000);
		::ReadFile(ConIn, bufoem, 0x2000, &readen, NULL);
		TCHAR* buf = (TCHAR*)MemGet(readen * sizeof(TCHAR));
		::OemToCharBuff(bufoem, buf, readen);
		text.Term();
		text.Init(buf, readen);

		while (text.IsNotEmpty() && ((text[text.Length()-1] == 13) ||
				(text[text.Length()-1] == 10)))
		{
			text.Delete(text.Length()-1);
		}
}

CString CommandPar(int i)
{
	CString text;

	int pos = 0;
	int beg = 0;
	int len = CommandLine.Length();

// nalezení parametru
	for (; i >= 0; i--)
	{

// nalezení zaèátku parametru
		while (	(pos < len) && 
				(CommandLine[pos] <= _T(' ')) && 
				(CommandLine[pos] > 0)) 
		{
			pos++;
		}
		beg = pos;

// nalezení konce parametru
		bool uvoz = false;
		while (	(pos < len) &&
				(	(CommandLine[pos] < 0) ||
					(CommandLine[pos] > _T(' ')) ||
					uvoz
				) )
		{
			if (CommandLine[pos] == _T('"')) uvoz = !uvoz;
			pos++;
		}
	}

// pøenesení parametru
	text = CommandLine.Mid(beg, pos-beg);

// zrušení uvozovek
	pos = 0;
	len = text.Length();
	while (pos < len)
	{
		if (text[pos] == _T('"'))
		{
			text.Delete(pos, 1);
			len--;
			if ((pos < len) && (text[pos] != _T('"'))) pos--;
		}
		pos++;
	}

	return text;
}

//////////////////////////////////////////////////////////////////////////////
// inicializace úseku inicializaèních/terminaèních funkcí

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
// hlavní start programu

void mainCRTStartup()
{
// úschova verze systému
	VerzeOS = (int)::GetVersion();

// handle instance programu
	hInstance = ::GetModuleHandle(NULL);

// inicializace uzamykání ukonèení programu
#ifdef _MT
	::InitializeCriticalSection(&ExitCriticalSection);
#endif

// inicializace správce pamìti
	if (!MemInit()) 
	{
		Exit(EXITCODE_MEMERR);
	}

// inicializace koprocesoru
#ifdef _M_IX86
	WORD stat;
	_asm {
		wait						// synchronizace
		fnstcw		stat			// uložení øídicího slova
		wait						// synchronizace
		mov			ax,stat			// stav øídicího slova
		and			ah,not 0xc		// implicitní zaokrouhlování
		or			ah,3			// pøesnost 64 bitù
		mov			stat,ax			// nový stav øídicího slova
		fldcw		stat			// nastavení nového øídicího slova
	}
#endif

// inicializace dat prázdné ikony a prázdného obrázku
	EmptyPictureData.Data = (BYTE*)MemGet(ICONSIZE);
	MemFill(EmptyPictureData.Data, ICONSIZE, BackCol);

// inicializace obsluhy sprajtù
	InitSprite();						// statická inicializace sprajtù

// inicializace globálních objektù
	InitTerm(__xc_a, __xc_z);

// inicializace standardního záhlaví BMP
	StdBitmapInfo = (BITMAPINFO*) MemGet(sizeof(BITMAPINFO) + sizeof(RGBQUAD)*255);
	MemFill(StdBitmapInfo, sizeof(BITMAPINFO) + sizeof(RGBQUAD)*255);	// vynulování
	StdBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);	// velikost záhlaví
	StdBitmapInfo->bmiHeader.biWidth = ICONWIDTH;	// šíøka bitmapy
	StdBitmapInfo->bmiHeader.biHeight = ICONHEIGHT;	// výška bitmapy
	StdBitmapInfo->bmiHeader.biPlanes = 1;			// poèet barevných rovin
	StdBitmapInfo->bmiHeader.biBitCount = 8;		// poèet bitù na bod
	StdBitmapInfo->bmiHeader.biCompression = BI_RGB; // není komprese
	StdBitmapInfo->bmiHeader.biClrImportant = StdColors; // poèet dùležitých palet

// vytvoøení standardních palet
	RGBQUAD* rgb = StdBitmapInfo->bmiColors;		// zaèátek tabulky palet

// nemìnit paletu barvu pozadí - používá se pøi importu obrázkù
	rgb->rgbRed =	BACKCOLOR_RED;					// prùhledná barva
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
		*rgb = ColColTab[i];						// pøenesení základní barvy

		for (j = 1; j < ColLev; j++)				// pro všechny odstíny
		{
			k = ColLevTab[j];						// násobící koeficient
			rgb[j].rgbRed = (BYTE)(rgb->rgbRed*k/256);		// èervená
			rgb[j].rgbGreen = (BYTE)(rgb->rgbGreen*k/256);	// zelená
			rgb[j].rgbBlue = (BYTE)(rgb->rgbBlue*k/256);	// modrá
		}
		rgb += ColLev;								// adresa další barvy
	}

	i = WhiteCol;									// index bílé barvy
	for (; i <= BlackCol; i++)						// od bílé barvy až po èernou
	{
		k = (BlackCol-i)*255/(2*ColLev-1);			// odstín bílé barvy
		rgb->rgbRed = (BYTE)k;						// èervená
		rgb->rgbGreen = (BYTE)k;					// zelená
		rgb->rgbBlue = (BYTE)k;						// modrá
		rgb++;										// zvýšení adresy barvy
	}

// vygenerování tabulky pro import palet a tabulky palet (pro konverzi BMP)
	StdPalImport = (BYTE*)MemGet(64 * 64 * 64);		// tabulka pro import palet
	StdPalImportDither = (BYTE*)MemGet(64 * 64 * 64 * 4);	// tabulka pro import palet s dithering

// naètení tabulky importu palet
	HRSRC hRes = ::FindResource(hInstance, MAKEINTRESOURCE(IDN_PALIMP), _T("LOADER"));
	HGLOBAL hData = ::LoadResource(hInstance, hRes);
	ASSERT((hRes != NULL) && (hData != NULL));
	if ((hRes == NULL) || (hData == NULL)) Exit(EXITCODE_LOADRES);
	DeKomp(StdPalImport, 64*64*64, (BYTE*)::LockResource(hData)+6, ::SizeofResource(hInstance, hRes)-6);

	hRes = ::FindResource(hInstance, MAKEINTRESOURCE(IDN_PALIMPD), _T("LOADER"));
	hData = ::LoadResource(hInstance, hRes);
	ASSERT((hRes != NULL) && (hData != NULL));
	if ((hRes == NULL) || (hData == NULL)) Exit(EXITCODE_LOADRES);
	DeKomp(StdPalImportDither, 64*64*64*4, (BYTE*)::LockResource(hData)+6, ::SizeofResource(hInstance, hRes)-6);

// vytvoøení konverzní tabulky palet pro ímport souborù BMP
	KonvPal = (BYTE*)MemGet(256);			// konverzní tabulka palet

// úschova pøíkazového øádku
	ConsoleOut(UvText);
	CommandLine = ::GetCommandLine();

#ifdef _DEBUG
	if (CommandPar(1).IsEmpty())
	{
		CommandLine = _T("Pov2Spr AutoB01.BMP AutoB.SPR 64 1 0 55 1000 8");
	}
#endif

	if (CommandPar(9).IsEmpty() || CommandPar(10).IsNotEmpty())
	{
		ConsoleOut(HelpText);
		CString cek;
		ConsoleIn(cek);
		Exit(1);
	}
// parametry
	InName = CommandPar(1);

	OutName = CommandPar(2);

	int smeru = Int(CommandPar(3));
	if (smeru < 1) smeru = 1;
	if (smeru > SPRITEMAXSMER) smeru = SPRITEMAXSMER;

	int klid = Int(CommandPar(4));
	if (klid < 1) klid = 1;
	if (klid > SPRITEMAXKLID) klid = SPRITEMAXKLID;

	int pohyb = Int(CommandPar(5));
	if (pohyb < 0) pohyb = 0;
	if (pohyb > SPRITEMAXPOHYB) pohyb = SPRITEMAXPOHYB;

	Sprite.ReSize(klid, pohyb, smeru);

	Sprite.Delay(Int(CommandPar(6)));
	if (Sprite.Delay() < 55) Sprite.Delay(55);
	if (Sprite.Delay() > 4000000) Sprite.Delay(4000000);
	Sprite.Delay((Sprite.Delay() + 22) / 55 * 55);
	if (Sprite.Delay() > SPRITEMAXDELAY) Sprite.Delay(SPRITEMAXDELAY);

	Sprite.Level(Int(CommandPar(7)));

	Sprite.Kroku(Double(CommandPar(8)));
	if (Sprite.Kroku() < 0) Sprite.Kroku(0);
	if (Sprite.Kroku() > SPRITEMAXKROKU) Sprite.Kroku(SPRITEMAXKROKU);

	Dither = (Int(CommandPar(9)) != 0);

// cyklus pøes všechny obrázky
	for (int faze = 0; faze < klid+pohyb; faze++)
	{
		for (int smer = 0; smer < smeru; smer++)
		{

// naètení obrázku
			if (!Sprite.At(faze, smer).LoadFile2(InName))
			{
				ConsoleOut(ReadErr1);
				ConsoleOut(InName);
				ConsoleOut(ReadErr2);
				Exit(1);
			}

// zmenšení obrázku
//			Sprite.At(faze, smer).HalfSize();
			Sprite.At(faze, smer).Resize(Sprite.Width(), Sprite.Height());

// zrušení vstupního souboru
//			::DeleteFile(InName);

// zvýšení jména souboru
			int pos = InName.RevFind('.');
			if (pos < 0) 
			{
				pos = InName.Length();
				InName += _T(".bmp");
			}

			while (--pos >= 0)
			{
				if ((InName[pos] < '0') || (InName[pos] > '9')) break;
				InName[pos]++;
				if (InName[pos] <= '9') break;
				InName[pos] = '0';
			}
		}
	}

// zápis spraju
	if (!Sprite.SaveFile(OutName))
	{
		ConsoleOut(WriteErr1);
		ConsoleOut(OutName);
		ConsoleOut(WriteErr2);
		Exit(1);
	}

// konec programu
	Exit(EXITCODE_OK);
};

//////////////////////////////////////////////////////////////////////////////
// ukonèení programu

void Exit(int code)
{
// uzamknutí ukonèení programu
#ifdef _MT
	::EnterCriticalSection(&ExitCriticalSection);
#endif

// ukonèení správce pamìti
	MemTerm();

// uvolnìní uzamykání ukonèení programu
#ifdef _MT
	::DeleteCriticalSection(&ExitCriticalSection);
#endif

// ukonèení programu
	ExitProcess(code);
}

/////////////////////////////////////////////////////////////////////////////
// generování konverzní tabulky barev obrázku

void GenKonvPal(BITMAPINFO* bmp)
{
// lokální promìnné
	int			i;									// èítaè barev
	int			palet = bmp->bmiHeader.biClrUsed;	// poèet palet v bitmapì
	RGBQUAD*	col = bmp->bmiColors;				// ukazatel barevných složek
	BYTE*		pal = KonvPal;						// ukazatel konverzních palet
	DWORD		BackColData = *(DWORD*)(StdBitmapInfo->bmiColors + BackCol); // paleta pozadí
	DWORD		ShadColData = *(DWORD*)(StdBitmapInfo->bmiColors + ShadCol); // paleta stínu

// pøíprava poètu palet
	if (palet == 0) palet = (1 << bmp->bmiHeader.biBitCount);
	i = palet;
	if ((palet < 1) || (palet > 256)) return;

// cyklus pøes platné barvy
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

// vymazání zbylých neplatných barev (import na èernou barvu)
	MemFill(pal, 256-palet, BlackCol);
}


/////////////////////////////////////////////////////////////////////////////
// vygenerování støední barvy (ze 4 bodù)

BYTE _fastcall ColAvrg(BYTE col1, BYTE col2, BYTE col3, BYTE col4)
{
// lokální promìnné
	int			r = 0;									// èervená složka
	int			g = 0;									// zelená složka
	int			b = 0;									// modrá složka
	BYTE		n = 0;									// poèet platných bodù
	RGBQUAD*	rgb;									// ukazatel palet
	int			shad = 0;								// èítaè stínù

// první bod
	if (col1 != BackCol)
	{
		if (col1 == ShadCol)
		{
			shad++;
		}
		else
		{
			n++;											// zvýšení èítaèe bodù
			rgb = StdBitmapInfo->bmiColors + col1;			// definice palet bodu
			r = rgb->rgbRed;								// èervená složka
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
			n++;											// zvýšení èítaèe bodù
			rgb = StdBitmapInfo->bmiColors + col2;			// definice palet bodu
			r += rgb->rgbRed;								// èervená složka
			g += rgb->rgbGreen;								// zelená složka
			b += rgb->rgbBlue;								// modrá složka
		}
	}

// tøetí bod
	if (col3 != BackCol)
	{
		if (col3 == ShadCol)
		{
			shad++;
		}
		else
		{
			n++;											// zvýšení èítaèe bodù
			rgb = StdBitmapInfo->bmiColors + col3;			// definice palet bodu
			r += rgb->rgbRed;								// èervená složka
			g += rgb->rgbGreen;								// zelená složka
			b += rgb->rgbBlue;								// modrá složka
		}
	}

// ètvrtý bod
	if (col4 != BackCol)
	{
		if (col4 == ShadCol)
		{
			shad++;
		}
		else
		{
			n++;											// zvýšení èítaèe bodù
			rgb = StdBitmapInfo->bmiColors + col4;			// definice palet bodu
			r += rgb->rgbRed;								// èervená složka
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
// vygenerování støední barvy (ze 4 bodù) s ditheringem

BYTE ColAvrgDither(BYTE col1, BYTE col2, BYTE col3, BYTE col4, int x, int y)
{
// lokální promìnné
	int			r = 0;									// èervená složka
	int			g = 0;									// zelená složka
	int			b = 0;									// modrá složka
	BYTE		n = 0;									// poèet platných bodù
	RGBQUAD*	rgb;									// ukazatel palet
	int			shad = 0;								// èítaè stínù

// první bod
	if (col1 != BackCol)
	{
		if (col1 == ShadCol)
		{
			shad++;
		}
		else
		{
			n++;											// zvýšení èítaèe bodù
			rgb = StdBitmapInfo->bmiColors + col1;			// definice palet bodu
			r = rgb->rgbRed;								// èervená složka
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
			n++;											// zvýšení èítaèe bodù
			rgb = StdBitmapInfo->bmiColors + col2;			// definice palet bodu
			r += rgb->rgbRed;								// èervená složka
			g += rgb->rgbGreen;								// zelená složka
			b += rgb->rgbBlue;								// modrá složka
		}
	}

// tøetí bod
	if (col3 != BackCol)
	{
		if (col3 == ShadCol)
		{
			shad++;
		}
		else
		{
			n++;											// zvýšení èítaèe bodù
			rgb = StdBitmapInfo->bmiColors + col3;			// definice palet bodu
			r += rgb->rgbRed;								// èervená složka
			g += rgb->rgbGreen;								// zelená složka
			b += rgb->rgbBlue;								// modrá složka
		}
	}

// ètvrtý bod
	if (col4 != BackCol)
	{
		if (col4 == ShadCol)
		{
			shad++;
		}
		else
		{
			n++;											// zvýšení èítaèe bodù
			rgb = StdBitmapInfo->bmiColors + col4;			// definice palet bodu
			r += rgb->rgbRed;								// èervená složka
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
		return PalImportDither((BYTE)(r/n), (BYTE)(g/n), (BYTE)(b/n), x, y);
	}
}

/////////////////////////////////////////////////////////////////////////////
// dekomprese dat

#define MAXLENX 25					// min. délka dlouhého øetìzce
#define MAXLEN (MAXLENX+254)		// maximální délka øetìzce
#define SUBSTLEN	7				// délka nahrazená dlouhým kódem

void DeKomp(BYTE* dstBuf, int dstNum, BYTE* srcBuf, int srcNum)
{
// naètení jednoho bitu ze stavového slova
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
	int dsti = 0;					// èítaè cílových dat
	BYTE* src = srcBuf;				// ukazatel zdrojové adresy
	int srci = 0;					// èítaè zdrojových dat
	BYTE* src2;						// pomocný ukazatel
	int srci2;

	WORD status = 0;				// støadaè stavového slova
	BYTE offsetL, offsetH;			// offset k opakování
	int delka;						// délka k opakování
	int bit;						// 1 = naètený bit

	for (;;)
	{
// naètení prvního bitu pøíznaku
		DekBit

// pøesun bajtu bez komprese
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

// jinak bude opakování øetìzce
		else
		{
			offsetH = 0;
			offsetL = 0;

// první bit délky
			DekBit
			delka = bit;

// zvýšení èítaèe délky
			for (;;)
			{
				delka++;
				delka++;

// naètení pøíznaku konce kódu
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

// naètení offsetu - nižší bajt
			if (srci >= srcNum) break;
			offsetL = *src;
			src++;
			srci++;

// pøenesení øetìzce
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
// pøevod reálného èísla na celé èíslo se zaokrouhlením k nejbližší hodnotì

int Round(double num)
{
#ifdef _M_IX86

	DWORD		result;				// výsledek operace

	_asm {
		fld			num				// naètení èísla k provedení operace
		fistp		result			// pøevod èísla na celé èíslo
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
// pøevod reálného èísla na celé èíslo s oøíznutím k nule

int Round0(double num)
{
#ifdef _M_IX86

	WORD		oldstat;			// starý stav øídicího slova
	WORD		newstat;			// nový stav øídicího slova
	int			result;				// výsledek operace

	_asm {
		fld			num				// naètení èísla k provedení operace
		wait						// synchronizace
		fnstcw		oldstat			// uloženi øídicího slova
		wait						// synchronizace
		mov			ax,oldstat		// starý stav øídicího slova
		or			ah,0xc			// mód zaokrouhlení smìrem k nule
		mov			newstat,ax		// nový stav øídicího slova
		fldcw		newstat			// nastavení nového øídicího slova
		fistp		result			// pøevod èísla na celé èíslo
		fldcw		oldstat			// navrácení pùvodního øídicího slova
	}
	return result;

#else

	return (int)num;

#endif
}

/////////////////////////////////////////////////////////////////////////////
// pøevod reálného èísla na celé èíslo se zaokrouhlením nahoru (ke kladné hodnotì)

int RoundP(double num)
{
#ifdef _M_IX86

	WORD		oldstat;			// starý stav øídicího slova
	WORD		newstat;			// nový stav øídicího slova
	int			result;				// výsledek operace

	_asm {
		fld			num				// naètení èísla k provedení operace
		wait						// synchronizace
		fnstcw		oldstat			// uloženi øídicího slova
		wait						// synchronizace
		mov			ax,oldstat		// starý stav øídicího slova
		and			ah,not 0xc;		// vynulování øídicích bitù pro zaokrouhlení
		or			ah,8			// mód zaokrouhlení smìrem nahoru
		mov			newstat,ax		// nový stav øídicího slova
		fldcw		newstat			// nastavení nového øídicího slova
		fistp		result			// pøevod èísla na celé èíslo
		fldcw		oldstat			// navrácení pùvodního øídicího slova
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
// pøevod reálného èísla na celé èíslo se zaokrouhlením dolù (k záporné hodnotì)

int RoundM(double num)
{
#ifdef _M_IX86

	WORD		oldstat;			// starý stav øídicího slova
	WORD		newstat;			// nový stav øídicího slova
	int			result;				// výsledek operace

	_asm {
		fld			num				// naètení èísla k provedení operace
		wait						// synchronizace
		fnstcw		oldstat			// uloženi øídicího slova
		wait						// synchronizace
		mov			ax,oldstat		// starý stav øídicího slova
		and			ah,not 0xc;		// vynulování øídicích bitù pro zaokrouhlení
		or			ah,4			// mód zaokrouhlení smìrem dolù
		mov			newstat,ax		// nový stav øídicího slova
		fldcw		newstat			// nastavení nového øídicího slova
		fistp		result			// pøevod èísla na celé èíslo
		fldcw		oldstat			// navrácení pùvodního øídicího slova
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
