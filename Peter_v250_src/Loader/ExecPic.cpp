
#include "Main.h"

/***************************************************************************\
*																			*
*						Provádění programu - obrázek						*
*																			*
\***************************************************************************/

#pragma optimize("t", on)			// optimalizace na maximální rychlost


/////////////////////////////////////////////////////////////////////////////
// načtení výřezu obrázku z plochy

void _fastcall FPicGet(CPicture& pic)
{
// vyprázdnění obrázku
	pic.Empty();

// načtení parametrů výřezu
	int x = FIntX();				// počátek výřezu X
	int y = FIntY();				// počátek výřezu Y
	int width = FIntX();			// šířka výřezu
	int height = FIntY();			// výška výřezu

// korekce pro údaje mimo rozsah
	if (width >= 100000) width = Width;
	if (height >= 100000) height = Height;

// kontrola platnosti parametrů
	if ((width <= 0) || (height <= 0)) return;

// okraje obrázku vlevo a vpravo, šířka střední části
	int width0 = width;

	int width1 = 0;
	if (x < 0)
	{
		width1 = -x;
		if (width1 > width0) width1 = width0;
		width0 -= width1;
		x = 0;
	}

	int width2 = 0;
	if ((x + width0) > Width)
	{
		width2 = x + width0 - Width;
		if (width2 > width0) width2 = width0;
		width0 -= width2;
	}
	
// okraje obrázku dole a nahoře, výška střední části
	int height0 = height;

	int height1 = 0;
	if (y < 0)
	{
		height1 = -y;
		if (height1 > height0) height1 = height0;
		height0 -= height1;
		y = 0;
	}

	int height2 = 0;
	if ((y + height0) > Height)
	{
		height2 = y + height0 - Height;
		if (height2 > height0) height2 = height0;
		height0 -= height2;
	}

// vytvoření obrázku
	pic.New(width, height);

// vymazání dolních linek
	BYTE* dst = pic.DataData();
	MemFill(dst, height1*width, BlackCol);
	dst += height1*width;

// přenesení dat obrázku
	BYTE* src = BackBuf + x + y*WidthByte;
	for (; height0 > 0; height0--)
	{
		MemFill(dst, width1, BlackCol);
		dst += width1;

		MemCopy(dst, src, width0);
		dst += width0;
		src += WidthByte;

		MemFill(dst, width2, BlackCol);
		dst += width2;
	}

// vymazání dolních linek
	MemFill(dst, height2*width, BlackCol);
}


/***************************************************************************\
*																			*
*									funkce									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// funkce s návratem obrázku

void _fastcall FPicFunc(CPicture& pic)
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
	pic = Picture[Picture.Num() - res];

// zrušení lokálních proměnných
	FCommand();
}


/***************************************************************************\
*																			*
*								proměnné									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// prázdný obrázek

void _fastcall FPicEmpty(CPicture& pic) { pic.Empty(); };


/////////////////////////////////////////////////////////////////////////////
// globální obrázek (Data = index)

void _fastcall FPicObj(CPicture& pic)
{
// index obrázku
	int inx = ExecItem[-1].Data;

// nastavení obrázku
	pic = Picture[inx];
}


/////////////////////////////////////////////////////////////////////////////
// globalní obrázek v seznamu (Data = index, List = seznam)

void _fastcall FPicObjList(CPicture& pic)
{
// index seznamu
	int list = ExecItem[-1].List;

// index obrázku, inkrementace indexu seznamu
	int inx = ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// nastavení obrázku
	pic = Picture[inx];
}


/////////////////////////////////////////////////////////////////////////////
// lokální obrázek (Data = index)

void _fastcall FPicLoc(CPicture& pic)
{
// index obrázku
	int inx = Picture.Num() - ExecItem[-1].Data;

// nastavení obrázku
	pic = Picture[inx];
}


/////////////////////////////////////////////////////////////////////////////
// lokální obrázek v seznamu (Data = index, List = seznam)

void _fastcall FPicLocList(CPicture& pic)
{
// index seznamu
	int list = List.Num() - ExecItem[-1].List;

// index obrázku, inkrementace indexu seznamu
	int inx = Picture.Num() - ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// nastavení obrázku
	pic = Picture[inx];
}

/////////////////////////////////////////////////////////////////////////////
// načtení obrázku ze souboru

void _fastcall FGetFilePicture(CPicture& pic)
{
	pic.Empty();
	pic.LoadFile(false);
}

/////////////////////////////////////////////////////////////////////////////
// horizontální převrácení obrázku

void _fastcall FPicXFlip(CPicture& pic)
{
	FPicture(pic);
	pic.XFlip();
}

/////////////////////////////////////////////////////////////////////////////
// vertikální převrácení obrázku

void _fastcall FPicYFlip(CPicture& pic)
{
	FPicture(pic);
	pic.YFlip();
}


/////////////////////////////////////////////////////////////////////////////
// změna velikosti obrázku

void _fastcall FPicZoom(CPicture& pic)
{
// načtení obrázku ke konverzi
	FPicture(pic);

// načtení požadovaných rozměrů a přepínačů obrázku
	int newwidth = FIntX();			// nová šířka obrázku
	int newheight = FIntY();		// nová výška obrázku
	bool inter = FBool();			// interpolace
	bool dith = FBool();			// dithering

// test, zda má být šířka a výška implicitní
	bool impwidth = ((newwidth > 0x8000) || (newwidth < -0x8000));
	bool impheight = ((newheight > 0x8000) || (newheight < -0x8000));

// obrázek se nemění
	if (impwidth && impheight) return;

// aktuální šířka a výška obrázku
	int oldwidth = pic.Width();
	int oldheight = pic.Height();

// výsledkem bude prázdný obrázek
	if ((oldwidth <= 0) || (oldheight <= 0) || (newwidth == 0) || (newheight == 0))
	{
		pic.Empty();
		return;
	}

// implicitní šířka
	if (impwidth)
	{
		newwidth = Round(((double)newheight/oldheight) * oldwidth);
		if (newwidth < -0x8000) newwidth = -0x8000;
		if (newwidth > 0x8000) newwidth = 0x8000;
	}

// implicitní výška
	if (impheight)
	{
		newheight = Round(((double)newwidth/oldwidth) * oldheight);
		if (newheight < -0x8000) newheight = -0x8000;
		if (newheight > 0x8000) newheight = 0x8000;
	}

// kontrola po přepočtech, zda nebude prázdný obrázek
	if ((newwidth == 0) || (newheight == 0))
	{
		pic.Empty();
		return;
	}

// test, zda bude horizontální převrácení
	bool xflip1 = false;
	bool xflip2 = false;
	if (newwidth < 0)
	{
		newwidth = -newwidth;

		if (newwidth > oldwidth)
		{
			xflip1 = true;
		}
		else
		{
			xflip2 = true;
		}
	}

// test, zda bude vertikální převrácení
	bool yflip1 = false;
	bool yflip2 = false;
	if (newheight < 0)
	{
		newheight = -newheight;

		if (newheight > oldheight)
		{
			yflip1 = true;
		}
		else
		{
			yflip2 = true;
		}
	}

// provedení konverze s případným převrácením
	if (xflip1) pic.XFlip();
	if (yflip1) pic.YFlip();
	pic.Zoom(newwidth, newheight, inter, dith);
	if (xflip2) pic.XFlip();
	if (yflip2) pic.YFlip();
}

/////////////////////////////////////////////////////////////////////////////
// změna jasu obrázku

void _fastcall FPicLevel(CPicture& pic)
{
// načtení obrázku ke konverzi
	FPicture(pic);

// načtení požadovaného jasu obrázku
	double koef = FNum();			// koeficient jasu obrázku

// načtení příznaku ditheringu
	bool dith = FBool();			// dithering

// konverze jasu obrázku
	pic.Level(koef, dith);
}


/////////////////////////////////////////////////////////////////////////////
// otočení obrázku

void _fastcall FPicRotate(CPicture& pic)
{
// načtení obrázku ke konverzi
	FPicture(pic);

// načtení požadovaného úhlu
	double angle = FNum();

// načtení příznaku interpolace
	bool inter = FBool();			// interpolace

// načtení příznaku ditheringu
	bool dith = FBool();			// dithering

// otočení obrázku
	pic.Rotate(angle, inter, dith);
}


/////////////////////////////////////////////////////////////////////////////
// načtení obrázku okna

void _fastcall FGetWindowPic(CPicture& pic)
{
	if (Win.IsValid(WindowID))
	{
		pic = Win[WindowID].Picture;
	}
	else
	{
		pic.Empty();
	}
}

/////////////////////////////////////////////////////////////////////////////
// konverze plochy na obrázek

void _fastcall FMapPicture(CPicture& pic)
{
	CMap map;
	FMap(map);
	pic = map;
}

/////////////////////////////////////////////////////////////////////////////
// konverze ikony na obrázek

void _fastcall FIconPicture(CPicture& pic)
{
	CIcon icon;
	FIcon(icon);
	pic = icon;
}

/////////////////////////////////////////////////////////////////////////////
// maskování obrázku

void _fastcall FPicMask(CPicture& pic)
{
// načtení obrázku ke konverzi
	FPicture(pic);

// načtení masky
	CPicture mask;
	FPicture(mask);

// maskování obrázku
	pic.Mask(mask);
}

/////////////////////////////////////////////////////////////////////////////
// záměna barev obrázku

void _fastcall FPicXCol(CPicture& pic)
{
// načtení obrázku ke konverzi
	FPicture(pic);

// načtení barev
	BYTE col1 = (BYTE)FNumCol();
	BYTE col2 = (BYTE)FNumCol();

// záměna barev v obrázku
	pic.XCol(col1, col2);
}

/////////////////////////////////////////////////////////////////////////////
// náhrada barvy obrázku

void _fastcall FPicSCol(CPicture& pic)
{
// načtení obrázku ke konverzi
	FPicture(pic);

// načtení barev
	BYTE oldcol = (BYTE)FNumCol();
	BYTE newcol = (BYTE)FNumCol();

// náhrada barvy v obrázku
	pic.SCol(oldcol, newcol);
}


// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI

/////////////////////////////////////////////////////////////////////////////
// zjištění obrázku textury Direct3D objektu

void _fastcall FGetD3DTexture(CPicture& pic)
{
	pic.Empty();

	if (IsValidID())
	{
		D3DFITEM* item = GetID();
		int texture = item->Texture[0];
		if (texture >= 0)
		{
			pic = D3DT_Get(texture)->Pic;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// zjištění obrázku textury Direct3D pozadí scény

void _fastcall FGetD3DSceneTexture(CPicture& pic)
{
	if (D3DBackText >= 0)
	{
		pic = D3DT_Get(D3DBackText)->Pic;
	}
	else
	{
		pic.Empty();
	}
}

#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------
