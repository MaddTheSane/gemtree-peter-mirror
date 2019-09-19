
#include "Main.h"

/***************************************************************************\
*																			*
*						Provádění programu - ikona							*
*																			*
\***************************************************************************/

#pragma optimize("t", on)			// optimalizace na maximální rychlost


/////////////////////////////////////////////////////////////////////////////
// načtení ikony z plochy

void _fastcall FIconGet(CIcon& icon)
{
// vyprázdnění ikony
	icon.Empty();

// načtení parametrů výřezu
	int x = FIntX();				// počátek výřezu X
	int y = FIntY();				// počátek výřezu Y
	int width = ICONWIDTH;			// šířka výřezu
	int height = ICONHEIGHT;		// výška výřezu

// omezení parametrů
	if (x < 0) { width += x; x = 0; }
	if (x > Width) x = Width;
	if (width < 0) width = 0;
	if (width > Width) width = Width;
	if (x + width > Width) width = Width - x;

	if (y < 0) { height += y; y = 0; }
	if (y > Height) y = Height;
	if (height < 0) height = 0;
	if (height > Height) height = Height;
	if (y + height > Height) height = Height - y;

// kontrola platnosti parametrů
	if ((width <= 0) || (height <= 0)) return;

// vytvoření ikony
	icon.New();
	if ((width < ICONWIDTH) || (height < ICONHEIGHT))
	{
		MemFill(icon.DataData(), ICONSIZE, BackCol);
	}

// přenesení dat obrázku
	BYTE* dst = icon.DataData() + (ICONWIDTH - width)/2 + (ICONHEIGHT - height)/2*ICONWIDTH;
	BYTE* src = BackBuf + x + y*WidthByte;
	for (; height > 0; height--)
	{
		MemCopy(dst, src, width);
		dst += ICONWIDTH;
		src += WidthByte;
	}
}


/***************************************************************************\
*																			*
*									funkce									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// funkce s návratem ikony

void _fastcall FIconFunc(CIcon& icon)
{
// úschova indexu volané funkce
	int data = ExecItem[-1].Data;

// úschova indexu proměnné s návratovou hodnotou
	int res = ExecItem[-1].List;

// inicializace lokálních proměnných
	FCommand();

// úschova ukazatele programu
	EXECITEM* oldexe = ExecItem;

// nová adresa programu
	ExecItem = ProgBuf + data;

// kontrola hloubky vnoření
	Hloubka--;
	if (Hloubka >= 0)
	{

// vyvolání funkce
		FCommand();
	}
	Hloubka++;

// návrat adresy programu
	ExecItem = oldexe;

// zrušení požadavku o přerušení
	Break &= ~(BREAKFUNC | BREAKWHILE);

// návrat výsledku operace
	icon = Icon[Icon.Num() - res];

// zrušení lokálních proměnných
	FCommand();
}


/***************************************************************************\
*																			*
*								proměnné									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// prázdná ikona

void _fastcall FIconEmpty(CIcon& icon) { icon.Empty(); };


/////////////////////////////////////////////////////////////////////////////
// globální ikona (Data = index)

void _fastcall FIconObj(CIcon& icon)
{
// index ikony
	int inx = ExecItem[-1].Data;

// nastavení ikony
	icon = Icon[inx];
}


/////////////////////////////////////////////////////////////////////////////
// globalní ikona v seznamu (Data = index, List = seznam)

void _fastcall FIconObjList(CIcon& icon)
{
// index seznamu
	int list = ExecItem[-1].List;

// index ikony, inkrementace indexu seznamu
	int inx = ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// nastavení ikony
	icon = Icon[inx];
}


/////////////////////////////////////////////////////////////////////////////
// lokální ikona (Data = index)

void _fastcall FIconLoc(CIcon& icon)
{
// index ikony
	int inx = Icon.Num() - ExecItem[-1].Data;

// nastavení ikony
	icon = Icon[inx];
}


/////////////////////////////////////////////////////////////////////////////
// lokální ikona v seznamu (Data = index, List = seznam)

void _fastcall FIconLocList(CIcon& icon)
{
// index seznamu
	int list = List.Num() - ExecItem[-1].List;

// index ikony, inkrementace indexu seznamu
	int inx = Icon.Num() - ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// nastavení ikony
	icon = Icon[inx];
}


/***************************************************************************\
*																			*
*							obsluha Petra a Petry							*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// načtení předmětu před Petrem

void _fastcall FPredmet(CIcon& icon)
{
	int x;
	int y;
	if (PetrXYNext(x, y))
	{
		icon = Map[0].At(x, y).Icon;
	}
	else
	{
		icon.Empty();
	}
}


/////////////////////////////////////////////////////////////////////////////
// načtení předmětu před Petrou

void _fastcall FPredmet2(CIcon& icon)
{
	int x;
	int y;
	if (Petr2XYNext(x, y))
	{
		icon = Map[0].At(x, y).Icon;
	}
	else
	{
		icon.Empty();
	}
}


/////////////////////////////////////////////////////////////////////////////
// načtení předmětu pod Petrem

void _fastcall FPredmetPoz(CIcon& icon)
{
	int x;
	int y;
	if (PetrXY(x, y))
	{
		icon = Map[0].At(x, y).Icon;
	}
	else
	{
		icon.Empty();
	}
}


/////////////////////////////////////////////////////////////////////////////
// načtení předmětu pod Petrou

void _fastcall FPredmet2Poz(CIcon& icon)
{
	int x;
	int y;
	if (Petr2XY(x, y))
	{
		icon = Map[0].At(x, y).Icon;
	}
	else
	{
		icon.Empty();
	}
}


/////////////////////////////////////////////////////////////////////////////
// načtení předmětu z plochy

void _fastcall FGetMapIcon(CIcon& icon)
{
// načtení plochy
	CMap map;
	FMap(map);

// načtení souřadnic
	int x = FIntR();
	int y = FIntR();

// ikona na pozici
	if (((DWORD)x < (DWORD)map.Width()) && ((DWORD)y < (DWORD)map.Height()))
	{
		icon = map.At(x, y).Icon;
	}
	else
	{
		icon.Empty();
	}
}


/////////////////////////////////////////////////////////////////////////////
// načtení ikony okna

void _fastcall FGetWindowIcon(CIcon& icon)
{
	if (Win.IsValid(WindowID))
	{
		icon = Win[WindowID].Icon;
	}
	else
	{
		icon.Empty();
	}
}

/////////////////////////////////////////////////////////////////////////////
// načtení ikony ze souboru

void _fastcall FGetFileIcon(CIcon& icon)
{
	icon.Empty();
	icon.LoadFile();
}
