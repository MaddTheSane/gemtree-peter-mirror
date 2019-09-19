
#include "Main.h"

/***************************************************************************\
*																			*
*								Obrázkové proměnné							*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// inicializační prázdný obrázek (modifikuje se počet referencí!)

// Prázdný obrázek musí mít rozměry ICONSIZE kvůli vytvoření nového sprajtu!

#ifndef _MINI
PICTUREDATA	EmptyPictureData		= {	1,					// čítač referencí
										PicParamNone,		// parametry
										ICONWIDTH,			// šířka
										ICONHEIGHT,			// výška
										NULL,				// adresa dat
										-1,					// index textury
										0,					// šířka textury
										0,					// výška textury
										0,					// je mipmap
										0,					// počet mipmap
										0,					// typ zařízení
										TRUE,				// textura obsahuje průhlednou barvu
										NULL,				// data textury
										NULL,				// data textury RGBA
										NULL,				// data textury R8G8B8
										NULL,				// data textury R5G6B5
										NULL,				// data textury A1R5G5B5
										NULL,				// data textury A4R4G4B4
										0,					// zjemnění
									};
#else // _MINI
PICTUREDATA	EmptyPictureData		= { 1, PicParamNone, ICONWIDTH, ICONHEIGHT, NULL, 0 };
#endif // _MINI

int		AviWidth = 384;				// šířka AVI
int		AviHeight = 288;			// výška AVI

/////////////////////////////////////////////////////////////////////////////
// odpojení (a zrušení) dat

void _fastcall CPicture::Detach()
{
	ASSERT(pData);
	ASSERT(pData->Refer > 0);

#ifndef _MINI
	if ((pData->Refer <= 2) && (pData->TextInx >= 0))
	{
		D3DTITEM* titem = D3DT_Get(pData->TextInx);

		if (titem->Ref == 1)
		{
			titem->Ref = 0;

			if (D3D)
			{
				for (int i = 0; i < MAX_STAGES; i++)
				{
					if (D3DT_Active[i] == pData->TextInx)
					{
						D3DT_Active[i] = -1;
						D3DT_On[i] = false;
						pD3TextureOff(i);
					}
				}
				pData->TextInx = -1;
				pD3TextureRes(titem);
			}

			titem->Mips = FALSE;
			pData->Refer = 1;
		}
	}

#endif // _MINI

	if (LongDecrement(&(pData->Refer)))
	{
#ifndef _MINI
		MemFree(pData->TextData);
		MemFree(pData->TextDataRGBA);
		MemFree(pData->TextDataR8G8B8);
		MemFree(pData->TextDataR5G6B5);
		MemFree(pData->TextDataA1R5G5B5);
		MemFree(pData->TextDataA4R4G4B4);
#endif // _MINI
		ASSERT(pData->Data);
		MemFree(pData->Data);
		MemFree(pData);
#ifdef _DEBUG
		pData = NULL;
#endif
	}
}

/////////////////////////////////////////////////////////////////////////////
// konstruktor a destruktor

CPicture::CPicture() 
{ 
	Attach(&EmptyPictureData); 
};

CPicture::CPicture(const CPicture& src) 
{ 
	Attach(src.pData); 
};

CPicture::CPicture(int width, int height)
{
	NewBuffer(width, height);
};

CPicture::~CPicture() 
{ 
	Detach(); 
};


/////////////////////////////////////////////////////////////////////////////
// statický konstruktor a destruktor

void CPicture::Init()
{ 
	Attach(&EmptyPictureData); 
};

void CPicture::Init(PICTUREDATA* data)
{ 
	Attach(data); 
};

void CPicture::Init(int width, int height)
{
	NewBuffer(width, height);
};

void CPicture::Term()
{ 
	Detach(); 
};


// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI
		 
/////////////////////////////////////////////////////////////////////////////
// importování textury z True Color bufferu
// ... Funkce vstupní buffer zruší!
// ... Buffery pro texturu musí být zrušeny!

void CPicture::ImportTexture(BYTE* buf, int newwidth, int newheight)
{

// současné rozměry obrázku
	int oldwidth = pData->Width;
	int oldheight = pData->Height;

// výchozí rozměry textury
	pData->TextWidth = newwidth;
	pData->TextHeight = newheight;

// příprava počtu MipMap (až bude rozměr 1x1)
	int w = newwidth;
	int h = newheight;
	int sizesum = w*h*4;
	int mips = 1;

	if (pData->TextIsMip)
	{
		while ((w > 1) || (h > 1))
		{
			w = (w >> 1);
			if (w == 0) w++;
			h = (h >> 1);
			if (h == 0) h++;
			sizesum += (w*h*4);
			mips++;
		}
	}
	pData->TextMipMaps = mips;

// buffer textury
	DWORD* dst = (DWORD*)MemGet(sizesum);
	pData->TextData = (BYTE*)dst; 

	DWORD* dstRGBA = (DWORD*)MemGet(sizesum);
	pData->TextDataRGBA = (BYTE*)dstRGBA;

	BYTE* dstR8G8B8 = (BYTE*)MemGet((sizesum/4)*3);
	pData->TextDataR8G8B8 = dstR8G8B8;

	sizesum /= 2;

	WORD* dstR5G6B5 = (WORD*)MemGet(sizesum);
	pData->TextDataR5G6B5 = (BYTE*)dstR5G6B5;

	WORD* dstA1R5G5B5 = (WORD*)MemGet(sizesum);
	pData->TextDataA1R5G5B5 = (BYTE*)dstA1R5G5B5;

	WORD* dstA4R4G4B4 = (WORD*)MemGet(sizesum);
	pData->TextDataA4R4G4B4 = (BYTE*)dstA4R4G4B4;

// cyklus přes všechny MipMaps
	for (; mips > 0; mips--)
	{
		int newsize = newwidth*newheight;
		dst = dst + (newsize - newwidth);
		dstRGBA = dstRGBA + (newsize - newwidth);
		dstR8G8B8 = dstR8G8B8 + 3*(newsize - newwidth);
		dstR5G6B5 = dstR5G6B5 + (newsize - newwidth);
		dstA1R5G5B5 = dstA1R5G5B5 + (newsize - newwidth);
		dstA4R4G4B4 = dstA4R4G4B4 + (newsize - newwidth);

// konverze obrázku na správnou velikost (nevadí, když se rozměry nemění)
		buf = ZoomTrueColor(buf, oldwidth, oldheight, newwidth, newheight);

// konverze dat do výstupního bufferu
		BYTE* src = buf;

		for (int i = newheight; i > 0; i--)
		{
			for (int j = newwidth; j > 0; j--)
			{
				*dst = (DWORD)(*(DWORD*)src ^ 0xff000000);
				*dstRGBA = XRGB(*(DWORD*)src);

				*(WORD*)dstR8G8B8 = *(WORD*)src;
				dstR8G8B8[2] = src[2];

				int b = (src[0] >> 3);
				int g = (src[1] >> 2);
				int r = (src[2] >> 3);

				*dstR5G6B5 = (WORD)(b | (g << 5) | (r << 11));

				g = (g >> 1);
				int a = (((BYTE*)dst)[3] >> 4); 

				*dstA1R5G5B5 = (WORD)(b | (g << 5) | (r << 10) | ((a & 0x8) << 12));

				*dstA4R4G4B4 = (WORD)((b >> 1) | ((g >> 1) << 4) | ((r >> 1) << 8) | (a << 12));

				src += 5;

				dst++;
				dstRGBA++;
				dstR8G8B8 += 3;
				dstR5G6B5++;
				dstA1R5G5B5++;
				dstA4R4G4B4++;
			}
			dst -= (2*newwidth);
			dstRGBA -= (2*newwidth);
			dstR8G8B8 -= 3*(2*newwidth);
			dstR5G6B5 -= (2*newwidth);
			dstA1R5G5B5 -= (2*newwidth);
			dstA4R4G4B4 -= (2*newwidth);
		}

// příprava pro další úroveň MipMaps
		dst = dst + (newwidth + newsize);
		dstRGBA = dstRGBA + (newwidth + newsize);
		dstR8G8B8 = dstR8G8B8 + 3*(newwidth + newsize);
		dstR5G6B5 = dstR5G6B5 + (newwidth + newsize);
		dstA1R5G5B5 = dstA1R5G5B5 + (newwidth + newsize);
		dstA4R4G4B4 = dstA4R4G4B4 + (newwidth + newsize);

		oldwidth = newwidth;
		newwidth = (newwidth >> 1);
		if (newwidth == 0) newwidth++;
		oldheight = newheight;
		newheight = (newheight >> 1);
		if (newheight == 0) newheight++;
	}

// zrušení datového bufferu
	MemFree(buf);
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření dat textury, není-li vytvořena
// Musí být nastavena položka TextSmooth !

void CPicture::InitTexture(int stage)
{
	int devakt = D3DIntAkt*100 + D3DDevAkt;

// jsou-li data a je to buď textura ze souboru nebo nebylo změněné zařízení, není potřeba změny
	if ((pData->TextData != NULL) &&
		((pData->TextSmooth == 0) ||
		(pData->TextDevType == devakt))) return;

// dekomprese obrázku
	DeComp();

// současné rozměry obrázku
	int oldwidth = pData->Width;
	int oldheight = pData->Height;

// nové rozměry obrázku
	int newwidth = oldwidth;
	int newheight = oldheight;

// aplikace zjemnění
	newwidth = Round((double)newwidth/pData->TextSmooth);
	if (newwidth <= 0) newwidth = 1;

	newheight = Round((double)newheight/pData->TextSmooth);
	if (newheight <= 0) newheight = 1;

// korekce na čtvercovou texturu (použije se průměr)
	if (SquareTexture)
	{
		newwidth = (newwidth + newheight + 1)/2;
		newheight = newwidth;
	}

// korekce rozměrů na mocninu 2 (použije se nejbližší rozměr)
	if (TexturePow2)
	{
		int k = (newwidth*3 + 2) / 4;
		newwidth = 1;
		while (newwidth < k) newwidth = (newwidth << 1);

		k = (newheight*3 + 2) / 4;
		newheight = 1;
		while (newheight < k) newheight = (newheight << 1);
	}

// omezení rozměrů textury
	if (newwidth > MaxTextureWidth) newwidth = MaxTextureWidth;
	if (newheight > MaxTextureHeight) newheight = MaxTextureHeight;

// zrušení textury, pokud byla vytvořena pro jiné zařízení a jsou nyní nutné menší rozměry
	if ((pData->TextData != NULL) &&
		(pData->TextDevType != devakt) &&
		((pData->TextWidth > newwidth) ||
		(pData->TextHeight > newheight)))
	{
		MemFree(pData->TextData);
		pData->TextData = NULL;
		MemFree(pData->TextDataRGBA);
		pData->TextDataRGBA = NULL;
		MemFree(pData->TextDataR8G8B8);
		pData->TextDataR8G8B8 = NULL;
		MemFree(pData->TextDataR5G6B5);
		pData->TextDataR5G6B5 = NULL;
		MemFree(pData->TextDataA1R5G5B5);
		pData->TextDataA1R5G5B5 = NULL;
		MemFree(pData->TextDataA4R4G4B4);
		pData->TextDataA4R4G4B4 = NULL;
	}
	pData->TextDevType = devakt;

// test, zda je textura již vytvořena
	if (pData->TextData != NULL) return;

// příznak mipmap
	pData->TextIsMip = D3DMipFilterAkt[stage];

// detekce průhlednosti
	pData->TextTrans = FALSE;
	BYTE* ss = pData->Data;
	for (int kk = oldwidth*oldheight; kk > 0; kk--)
	{
		if (*ss == BackCol)
		{
			pData->TextTrans = TRUE;
			break;
		}

		ss++;
	}

// importování textury
	BYTE* buf = ExportTrueAlphaBeta();
	ImportTexture(buf, newwidth, newheight);
}

#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------


/////////////////////////////////////////////////////////////////////////////
// kopie do vlastního bufferu před modifikací (nekopírují se data textury)

void CPicture::CopyWrite()
{
	DeComp();

	PICTUREDATA* data = pData;		// adresa starých dat
	long* refer = &(data->Refer);	// počet referencí

	if (*refer > 1)					// je nějaký jiný majitel?
	{
		NewBuffer(data->Width, data->Height);	// vytvoření nového bufferu
		MemCopy(pData->Data, data->Data, data->Width * data->Height);
		pData->Param = data->Param;	// přenesení parametrů

// odpojení starých dat - v multithread může nastat i zrušení
		if (LongDecrement(refer))
		{
#ifndef _MINI
			MemFree(data->TextData);
			MemFree(data->TextDataRGBA);
			MemFree(data->TextDataR8G8B8);
			MemFree(data->TextDataR5G6B5);
			MemFree(data->TextDataA1R5G5B5);
			MemFree(data->TextDataA4R4G4B4);
#endif // _MINI

#ifdef _MT
			MemFree(data->Data);
			MemFree(data);			// případné zrušení dat
#endif	// _MT
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// vyprázdnění obrázku (uvolnění dat)

void CPicture::Empty()
{ 
	Detach(); 
	Attach(&EmptyPictureData); 
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření nového obrázku (připraveno k zápisu, data jsou náhodná)

void CPicture::New(int width, int height)
{
	Detach();						// odpojení starého obrázku
	NewBuffer(width, height);		// vytvoření nového bufferu
}


/////////////////////////////////////////////////////////////////////////////
// dekomprimace dat obrázku (jsou-li komprimována)

void CPicture::DeComp()
{
	if (pData->Param == PicParamComp)
	{
		int width = pData->Width;
		int height = pData->Height;
		int size = width*height;
		BYTE* data = (BYTE*)MemGet(size);
		DeCompress(data, pData->Data + 4, size, width);
		pData->Data = data;
		pData->Refer -= BigInt;
		pData->Param = PicParamNone;
	}
}

/////////////////////////////////////////////////////////////////////////////
// maskování obrázku

void CPicture::Mask(CPicture& mask)
{
// dekomprese obrázku
	CopyWrite();
	mask.DeComp();

// příprava šířky
	int dstwidth = Width();
	int srcwidth = mask.Width();
	int width;

	if (srcwidth < dstwidth)
	{
		width = srcwidth;
		dstwidth -= width;
		srcwidth = 0;
	}
	else
	{
		width = dstwidth;
		srcwidth -= width;
		dstwidth = 0;
	}

// příprava výšky
	int dstheight = Height();
	int srcheight = mask.Height();
	int height;

	if (srcheight < dstheight)
	{
		height = srcheight;
		dstheight -= height;
		srcheight = 0;
	}
	else
	{
		height = dstheight;
		srcheight -= height;
		dstheight = 0;
	}

// příprava ukazatelů dat
	BYTE* dstdata = DataData();
	BYTE* srcdata = mask.DataData();

// provedení konverze
	for (int i = height; i > 0; i--)
	{
		for (int j = width; j > 0; j--)
		{
			if (*srcdata == BackCol) *dstdata = BackCol;
			srcdata++;
			dstdata++;
		}

		MemFill(dstdata, dstwidth, BackCol);
		dstdata += dstwidth;
		srcdata += srcwidth;
	}

// vymazání zbytku linek
	MemFill(dstdata, dstheight * (width + dstwidth), BackCol);
}

/////////////////////////////////////////////////////////////////////////////
// záměna barev v obrázku

void CPicture::XCol(BYTE col1, BYTE col2)
{
// dekomprese obrázku
	CopyWrite();

// příprava ukazatelů
	BYTE* data = DataData();

// záměna barev
	for (int i = Width()*Height(); i > 0; i--)
	{
		BYTE col = *data;

		if (col == col1)
		{
			*data = col2;
		}
		else
		{
			if (col == col2)
			{
				*data = col1;
			}
		}

		data++;
	}
}

/////////////////////////////////////////////////////////////////////////////
// náhrada barvy v obrázku

void CPicture::SCol(BYTE oldcol, BYTE newcol)
{
// dekomprese obrázku
	CopyWrite();

// příprava ukazatelů
	BYTE* data = DataData();

// záměna barev
	for (int i = Width()*Height(); i > 0; i--)
	{
		if (*data == oldcol)
		{
			*data = newcol;
		}

		data++;
	}
}

/////////////////////////////////////////////////////////////////////////////
// nastavení nových rozměrů obrázku

void CPicture::Resize(int width, int height)
{
	DeComp();

	if ((width != pData->Width) || (height != pData->Height))
	{
// vytvoření nového bufferu (starý se zatím jen uschová)
		PICTUREDATA* olddata = pData;
		NewBuffer(width, height);
		PICTUREDATA* newdata = pData;

// příprava počtu linek ke kopii
		int i = newdata->Height;
		if (olddata->Height < i) i = olddata->Height;

// příprava délky linky ke kopii
		int j = newdata->Width;
		if (olddata->Width < j) j = olddata->Width;

// příprava zbytku linky k vymazání
		int k = newdata->Width - j;

// přírustek zdrojové a cílové adresy
		int srcinc = olddata->Width;
		int dstinc = newdata->Width;

// příprava zdrojové a cílové adresy
		BYTE* src = olddata->Data;
		BYTE* dst = newdata->Data;

// kopie platných linek
		if (srcinc == dstinc)
		{
			i *= dstinc;
			MemCopy(dst, src, i);
			dst += i;
			src += i;
		}
		else
		{
			if (k == 0)
			{
				for (; i > 0; i--)
				{
					MemCopy(dst, src, j);
					dst += dstinc;
					src += srcinc;
				}
			}
			else
			{
				for (; i > 0; i--)
				{
					MemCopy(dst, src, j);
					MemFill(dst + j, k, BackCol);
					dst += dstinc;
					src += srcinc;
				}
			}
		}

// vymazání zbylých linek
		i = newdata->Height - olddata->Height;
		if (i > 0)
		{
			MemFill(dst, i*dstinc, BackCol);
		}

// odpojení starého bufferu
		pData = olddata;
		Detach();
		pData = newdata;
	}
}


/////////////////////////////////////////////////////////////////////////////
// exportování obrázku do bufferu TRUECOLORALPHABETA (vrací buffer)

BYTE* CPicture::ExportTrueAlphaBeta()
{
// zajištění dekomprimace
	DeComp();

// vytvoření bufferu
	int size = pData->Width * pData->Height;
	BYTE* buf = (BYTE*)MemGet(size * 5);

// převod na TRUE COLOR ALPHA
	BYTE* src = pData->Data;
	BYTE* dst = buf;
	RGBQUAD* rgb = StdBitmapInfo->bmiColors;

	for (; size > 0; size--)
	{
		BYTE col = *src;
		src++;

		switch (col)
		{
		case BackCol:
			*dst = 0;		// B: modrá
			dst++;
			*dst = 0;		// G: zelená
			dst++;
			*dst = 0;		// R: červená
			dst++;
			*dst = 255;		// A: průhlednost
			dst++;
			*dst = 0;		// S: stín
			dst++;
			break;

		case ShadCol:
			*dst = 0;		// B: modrá
			dst++;
			*dst = 0;		// G: zelená
			dst++;
			*dst = 0;		// R: červená
			dst++;
			*dst = 0;		// A: průhlednost
			dst++;
			*dst = 255;		// S: stín
			dst++;
			break;

		default:
			*dst = rgb[col].rgbBlue;		// B: modrá
			dst++; 
			*dst = rgb[col].rgbGreen;		// G: zelená
			dst++;
			*dst = rgb[col].rgbRed;			// R: červená
			dst++;
			*dst = 0;						// A: průhlednost
			dst++;
			*dst = 0;						// S: stín
			dst++;
		}
	}

	return buf;
}


/////////////////////////////////////////////////////////////////////////////
// importování obrázku z bufferu TRUECOLORALPHABETA

void CPicture::ImportTrueAlphaBeta(BYTE* buf, BOOL dith)
{
// rozměry obrázku
	int width = pData->Width;
	int height = pData->Height;

// zajištění prázdného obrázku
	if (pData->Refer > 1) New(width, height);
	DeComp();

// ukazatele dat
	BYTE* src = buf;
	BYTE* dst = pData->Data;

// konverze s ditheringem
	if (dith)
	{

// příprava bufferu odchylky pro dithering
		int* odch = (int*)MemGet((3*width + 6 + 10) * sizeof(int));
		MemFill(odch, (3*width + 6 + 10) * sizeof(int), 0);
		
// cyklus přes všechny linky		
		for (int y = height; y > 0; y--)
		{
			int* odch0 = odch + 3;			// ukazatel v bufferu odchylky

// cyklus přes všechny body na lince
			for (int x = width; x > 0; x--)
			{

// pozadí
				if (src[3] > 128)
				{
					*dst = BackCol;

					*odch0 = 0;
					odch0++;
					*odch0 = 0;
					odch0++;
					*odch0 = 0;
					odch0++;
				}
				else
				{

// stín
					if ((src[3] + src[4]) > 128)
					{
						*dst = ShadCol;

						*odch0 = 0;
						odch0++;
						*odch0 = 0;
						odch0++;
						*odch0 = 0;
						odch0++;
					}
					else
					{

// požadovaná barva
						BYTE b = src[0];			// modrá složka
						BYTE g = src[1];			// zelená složka
						BYTE r = src[2];			// červená složka

// zkorigovaná barva
						int b2 = b - (odch0[-3] + odch0[0] + odch0[3])*5/8;		// modrá složka
						int g2 = g - (odch0[-2] + odch0[1] + odch0[4])*5/8;		// zelená složka
						int r2 = r - (odch0[-1] + odch0[2] + odch0[5])*5/8;		// červená složka

// omezení přetečení barvy
						if ((DWORD)b2 > 255) { if (b2 < 0) b2 = 0; else b2 = 255; }
						if ((DWORD)g2 > 255) { if (g2 < 0) g2 = 0; else g2 = 255; }
						if ((DWORD)r2 > 255) { if (r2 < 0) r2 = 0; else r2 = 255; }

// import barvy
						BYTE col = PalImport((BYTE)r2, (BYTE)g2, (BYTE)b2);

// odchylka aktuálního bodu od požadované barvy
						RGBQUAD* rgb = StdBitmapInfo->bmiColors + col;
						*odch0 = rgb->rgbBlue - b;
						odch0++;

						*odch0 = rgb->rgbGreen - g;
						odch0++;

						*odch0 = rgb->rgbRed - r;
						odch0++;

// uložení bodu
						*dst = col;
					}
				}
				dst++;
				src += 5;
			}
		}

// zrušení bufferu odchylky pro dithering
		MemFree(odch);
	}

// konverze bez ditheringu
	else
	{
		for (int y = height; y > 0; y--)
		{
			for (int x = width; x > 0; x--)
			{
				if (src[3] > 128)
				{
					*dst = BackCol;
				}
				else
				{
					if ((src[3] + src[4]) > 128)
					{
						*dst = ShadCol;
					}
					else
					{
						*dst = PalImport(src[2], src[1], src[0]);
					}
				}
				dst++;
				src += 5;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////
// pomocné funkce pro filtry

#define		Max(x,y) (((x) > (y)) ? (x) : (y))	// maximální hodnota
#define		Min(x,y) (((x) < (y)) ? (x) : (y))	// minimální hodnota

//static double Box(double x)
//{
//  if ((x >= -0.5) && (x < 0.5))
//    return(1.0);
//  return(0.0);
//}

inline double Filter(double x)
{

/*
// Triangle

#define FSUPPORT 1

  if (x < 0.0)
    x=(-x);
  if (x < 1.0)
    return(1.0-x);
  return(0.0);
*/

/*
// Catrom

#define FSUPPORT 2

  if (x < 0)
    x=(-x);
  if (x < 1.0)
    return(0.5*(2.0+x*x*(-5.0+x*3.0)));
  if (x < 2.0)
    return(0.5*(4.0+x*(-8.0+x*(5.0-x))));
  return(0.0);
*/

// Mitchell

#define FSUPPORT 2

  double
    b,
    c;

  b=1.0/3.0;
  c=1.0/3.0;
  if (x < 0)
    x=(-x);
  if (x < 1.0)
    {
      x=((12.0-9.0*b-6.0*c)*(x*x*x))+((-18.0+12.0*b+6.0*c)*x*x)+(6.0-2.0*b);
      return(x/6.0);
    }
 if (x < 2.0)
   {
     x=((-1.0*b-6.0*c)*(x*x*x))+((6.0*b+30.0*c)*x*x)+((-12.0*b-48.0*c)*x)+
       (8.0*b+24.0*c);
     return(x/6.0);
   }
  return(0.0);

}

typedef struct ContribInfo_
{
  int		pixel;
  double    weight;
} ContribInfo;


/////////////////////////////////////////////////////////////////////////
// horizontální filtr

void HorizontalFilter(BYTE* oldbuf, int oldwidth, int oldheight, BYTE* newbuf, int newwidth)
{
// příprava ukazatelů podpory
	double xfactor = (double)newwidth/oldwidth;
	double support = FSUPPORT/xfactor;
	if (support < FSUPPORT) support = FSUPPORT;

// buffer konverze bodů
	ContribInfo* cinfo = (ContribInfo*)MemGet(Round(support*2+3)*sizeof(ContribInfo) + 128);

// konverzní faktory
	double scale = Max(1.0/xfactor, 1.0);
	support = Max(scale * FSUPPORT, 0.5);
	if (support <= 0.5)
	{
		support = 0.5;
		scale = 1.0;
	}
	support += 1.0e-7;							// korekce pro zaokrouhlování

// cyklus přes všechny cílové body
	for (int x = 0; x < newwidth; x++)
	{

// příprava pro převodní tabulku bodu
		double density = 0.0;					// hustota bodu
		int n = 0;								// čítač bodů
		double center = (double)x/xfactor;		// zdrojová souřadnice bodu
		int start = Round(center - support);	// počáteční bod
		int end = Round(center + support);		// koncový bod

// konverzní tabulka vah bodů
		for (int i = Max(start, 0); i < Min(end + 1, oldwidth); i++)
		{
			cinfo[n].pixel = i;					// číslo zdrojového bodu
			double weight = Filter(((double)i - center)/scale)/scale;
			density += weight;
			cinfo[n].weight = weight;
			n++;
		}

// normalizace vah
		if ((density != 0.0) && (density != 1.0))
		{
			for (int i = 0; i < n; i++)
			{
				cinfo[i].weight /= density;
			}
		}

// adresa cílového bodu ke konverzi
		BYTE* dst = newbuf + x*5;

// cyklus přes všechny linky
		for (int y = 0; y < oldheight; y++)
		{

// příprava střadačů vah barev
			double blue = 0;
			double green = 0;
			double red = 0;
			double alpha = 0;
			double shadow = 0;

// výpočet barevných složek
			for (int i = 0; i < n; i++)
			{
				BYTE* src = oldbuf + (y*oldwidth + cinfo[i].pixel)*5;

				double weight = cinfo[i].weight;

				blue	+= weight * src[0];
				green	+= weight * src[1];
				red		+= weight * src[2];
				alpha	+= weight * src[3];
				shadow	+= weight * src[4];
			}

// korekce barev při průhlednosti a stínu
			double as = alpha + shadow;

			if ((as > 0.0001) && (as < 254))
			{
//				double kor = sqrt(1/(1 - as/255));
				double kor = 1/(1 - as/1024);

				blue *= kor;
				green *= kor;
				red *= kor;
			}

// normalizace barevných složek
			int newblue = Round(blue);
			if ((DWORD)newblue >= 256)
			{
				if (newblue < 0) newblue = 0; else newblue = 255;
			}
			dst[0] = (BYTE)newblue;

			int newgreen = Round(green);
			if ((DWORD)newgreen >= 256)
			{
				if (newgreen < 0) newgreen = 0; else newgreen = 255;
			}
			dst[1] = (BYTE)newgreen;

			int newred = Round(red);
			if ((DWORD)newred >= 256)
			{
				if (newred < 0) newred = 0; else newred = 255;
			}
			dst[2] = (BYTE)newred;

			int newalpha = Round(alpha);
			if ((DWORD)newalpha >= 256)
			{
				if (newalpha < 0) newalpha = 0; else newalpha = 255;
			}
			dst[3] = (BYTE)newalpha;

			int newshadow = Round(shadow);
			if ((DWORD)newshadow >= 256)
			{
				if (newshadow < 0) newshadow = 0; else newshadow = 255;
			}
			dst[4] = (BYTE)newshadow;

			dst += newwidth*5;
		}
	}

// zrušení bufferu konverze bodů
	MemFree(cinfo);
}

/////////////////////////////////////////////////////////////////////////
// vertikální filtr

void VerticalFilter(BYTE* oldbuf, int oldwidth, int oldheight, BYTE* newbuf, int newheight)
{
// příprava ukazatelů podpory
	double yfactor = (double)newheight/oldheight;
	double support = FSUPPORT/yfactor;
	if (support < FSUPPORT) support = FSUPPORT;

// buffer konverze bodů
	ContribInfo* cinfo = (ContribInfo*)MemGet(Round(support*2+3)*sizeof(ContribInfo) + 128);

// konverzní faktory
	double scale = Max(1.0/yfactor, 1.0);
	support = Max(scale * FSUPPORT, 0.5);
	if (support <= 0.5)
	{
		support = 0.5;
		scale = 1.0;
	}
	support += 1.0e-7;							// korekce pro zaokrouhlování

// cyklus přes všechny cílové linky
	for (int y = 0; y < newheight; y++)
	{

// příprava pro převodní tabulku bodu
		double density = 0.0;					// hustota bodu
		int n = 0;								// čítač bodů
		double center = (double)y/yfactor;		// zdrojová souřadnice bodu
		int start = Round(center - support);	// počáteční bod
		int end = Round(center + support);		// koncový bod

// konverzní tabulka vah bodů
		for (int i = Max(start, 0); i < Min(end + 1, oldheight); i++)
		{
			cinfo[n].pixel = i;					// číslo zdrojového bodu
			double weight = Filter(((double)i - center)/scale)/scale;
			density += weight;
			cinfo[n].weight = weight;
			n++;
		}

// normalizace vah
		if ((density != 0.0) && (density != 1.0))
		{
			for (int i = 0; i < n; i++)
			{
				cinfo[i].weight /= density;
			}
		}

// adresa cílového bodu ke konverzi
		BYTE* dst = newbuf + y*oldwidth*5;

// cyklus přes všechny bodu na lince
		for (int x = 0; x < oldwidth; x++)
		{

// příprava střadačů vah barev
			double blue = 0;
			double green = 0;
			double red = 0;
			double alpha = 0;
			double shadow = 0;

// výpočet barevných složek
			for (int i = 0; i < n; i++)
			{
				BYTE* src = oldbuf + (x + cinfo[i].pixel*oldwidth)*5;

				double weight = cinfo[i].weight;

				blue	+= weight * src[0];
				green	+= weight * src[1];
				red		+= weight * src[2];
				alpha	+= weight * src[3];
				shadow	+= weight * src[4];
			}

// korekce barev při průhlednosti a stínu
			double as = alpha + shadow;

			if ((as > 0.0001) && (as < 254))
			{
//				double kor = sqrt(1/(1 - as/255));
				double kor = 1/(1 - as/1024);

				blue *= kor;
				green *= kor;
				red *= kor;
			}

// normalizace barevných složek
			int newblue = Round(blue);
			if ((DWORD)newblue >= 256)
			{
				if (newblue < 0) newblue = 0; else newblue = 255;
			}
			dst[0] = (BYTE)newblue;

			int newgreen = Round(green);
			if ((DWORD)newgreen >= 256)
			{
				if (newgreen < 0) newgreen = 0; else newgreen = 255;
			}
			dst[1] = (BYTE)newgreen;

			int newred = Round(red);
			if ((DWORD)newred >= 256)
			{
				if (newred < 0) newred = 0; else newred = 255;
			}
			dst[2] = (BYTE)newred;

			int newalpha = Round(alpha);
			if ((DWORD)newalpha >= 256)
			{
				if (newalpha < 0) newalpha = 0; else newalpha = 255;
			}
			dst[3] = (BYTE)newalpha;

			int newshadow = Round(shadow);
			if ((DWORD)newshadow >= 256)
			{
				if (newshadow < 0) newshadow = 0; else newshadow = 255;
			}
			dst[4] = (BYTE)newshadow;

			dst += 5;
		}
	}

// zrušení bufferu konverze bodů
	MemFree(cinfo);
}
		

/////////////////////////////////////////////////////////////////////////////
// konverze obrázku v bufferu TrueColorAlphaBeta (vrací nový buffer)

BYTE* ZoomTrueColor(BYTE* buf, int oldwidth, int oldheight, int newwidth, int newheight)
{

// při zvýšení výšky obrázku se konvertuje nejdříve horizontálně
	if (newheight >= oldheight)
	{

// test, zda je potřeba provádět horizontální konverzi
		if (newwidth != oldwidth)
		{

// vytvoření výstupního bufferu pro horizontální konverzi
			BYTE* buf2 = (BYTE*)MemGet(newwidth*oldheight*5);

// provedení horizontální konverze
			HorizontalFilter(buf, oldwidth, oldheight, buf2, newwidth);

// přenesení na vstupní buffer
			MemFree(buf);
			buf = buf2;
		}

// test, zda je potřeba provádět vertikální konverzi
		if (newheight != oldheight)
		{

// vytvoření výstupního bufferu pro vertikální konverzi
			BYTE* buf2 = (BYTE*)MemGet(newheight*newwidth*5);

// provedení vertikální konverze
			VerticalFilter(buf, newwidth, oldheight, buf2, newheight);

// přenesení na vstupní buffer
			MemFree(buf);
			buf = buf2;
		}
	}

// při snížení výšky obrázku se konvertuje nejdříve vertikálně
	else
	{

// test, zda je potřeba provádět vertikální konverzi
		if (newheight != oldheight)
		{

// vytvoření výstupního bufferu pro vertikální konverzi
			BYTE* buf2 = (BYTE*)MemGet(newheight*oldwidth*5);

// provedení vertikální konverze
			VerticalFilter(buf, oldwidth, oldheight, buf2, newheight);

// přenesení na vstupní buffer
			MemFree(buf);
			buf = buf2;
		}

// test, zda je potřeba provádět horizontální konverzi
		if (newwidth != oldwidth)
		{

// vytvoření výstupního bufferu pro horizontální konverzi
			BYTE* buf2 = (BYTE*)MemGet(newwidth*newheight*5);

// provedení horizontální konverze
			HorizontalFilter(buf, oldwidth, newheight, buf2, newwidth);

// přenesení na vstupní buffer
			MemFree(buf);
			buf = buf2;
		}
	}

	return buf;
}

/////////////////////////////////////////////////////////////////////////////
// změna velikosti obrázku (int = interpolovat, dith = ditherovat)

void CPicture::Zoom(int newwidth, int newheight, BOOL inter, BOOL dith)
{
// úschova starých rozměrů
	int oldwidth = pData->Width;
	int oldheight = pData->Height;

// test, zda bude změna velikosti obrázku
	if ((newwidth == oldwidth) && (newheight == oldheight)) return;

// zajištění dekomprimace
	DeComp();

// bude konverze bez interpolace
	if (!inter)
	{

// vytvoření bufferu horizontální konverzní tabulky
		int* konv = (int*)MemGet(newwidth * sizeof(int));

// vytvoření nového (prázdného) obrázku
		CPicture pic;
		pic.New(newwidth, newheight);

// inicializace horizontální konverzní tabulky
		int i, j;
		double k = (double)oldwidth/newwidth;

		for (i = 0; i < newwidth; i++)
		{
			j = RoundM(i * k);
			if (j >= oldwidth) j = oldwidth - 1;
			konv[i] = j;
		}

// provedení konverze
		BYTE* dst = pic.DataData();
		k = (double)oldheight/newheight;

		for (i = 0; i < newheight; i++)
		{
			j = RoundM(i * k);
			if (j >= oldheight) j = oldheight - 1;

			BYTE* src = pData->Data + oldwidth*j;

			for (j = 0; j < newwidth; j++)
			{
				*dst = src[konv[j]];
				dst++;
			}
		}

// zrušení konverzní tabulky
		MemFree(konv);

// nastavení nového obrázku
		*this = pic;

		return;
	}

// vytvoření bufferu s daty TRUECOLOR
	BYTE* buf = ExportTrueAlphaBeta();

// konverze obrázku
	buf = ZoomTrueColor(buf, oldwidth, oldheight, newwidth, newheight);

// vytvoření nového (prázdného) obrázku
	New(newwidth, newheight);

// importování obrázku z datového bufferu
	ImportTrueAlphaBeta(buf, dith);

// zrušení datového bufferu
	MemFree(buf);
}


/////////////////////////////////////////////////////////////////////////////
// změna jasu obrázku

void CPicture::Level(double koef, bool dith)
{
// kontrola, zda je obrázek platný
	if ((pData->Width <= 0) || (pData->Height <= 0)) return;

// omezení rozsahu koeficientů
	koef = fabs(koef);
	if (koef > 1000) koef = 1000;

// není změna jasu barvy
	if (koef == 1) return;

// zajištění dekomprimace
	DeComp();

// konverzní tabulka
	BYTE* tab = (BYTE*)MemGet(256);

// konverze bez ditheringu
	if (!dith)
	{

// přivlastnění bufferu
		CopyWrite();

// průhledná barva a barva pozadí
		tab[BackCol] = BackCol;
		tab[ShadCol] = ShadCol;

// příprava konverzní tabulky
		for (int i = ResCols; i < StdColors; i++)
		{
			RGBQUAD* rgb = StdBitmapInfo->bmiColors + i;		// definice palet barvy
			BYTE r = rgb->rgbRed;								// červená složka
			BYTE g = rgb->rgbGreen;								// zelená složka
			BYTE b = rgb->rgbBlue;								// modrá složka

			int r2 = Round(r * koef);
			if (r2 > 255) r2 = 255;
			int g2 = Round(g * koef);
			if (g2 > 255) g2 = 255;
			int b2 = Round(b * koef);
			if (b2 > 255) b2 = 255;

			tab[i] = PalImport((BYTE)r2, (BYTE)g2, (BYTE)b2);
		}

// neplatné barvy budou vždy černé
		MemFill(tab + StdColors, 256 - StdColors, BlackCol);

// provedení konverze obrázku
		BYTE* data = pData->Data;
		for (i = pData->Width*pData->Height; i > 0; i--)
		{
			*data = tab[*data];
			data++;
		}
	}

// konverze s ditheringem
	else
	{

// příprava konverzní tabulky
		for (int i = 0; i < 256; i++)
		{
			int c = Round(i * koef);
			if (c > 255) c = 255;
			tab[i] = (BYTE)c;
		}

// export do bufferu TRUECOLORALPHA
		BYTE* buf = ExportTrueAlphaBeta();

// provedení konverze
		BYTE* data = buf;
		for (i = pData->Width*pData->Height; i > 0; i--)
		{
			*data = tab[*data];		// modrá
			data++;
			*data = tab[*data];		// zelená
			data++;
			*data = tab[*data];		// červená
			data++;
			data++;					// průhlednost
			data++;					// stín
		}

// importování obrázku z datového bufferu
		ImportTrueAlphaBeta(buf, true);

// zrušení datového bufferu
		MemFree(buf);
	}

// zrušení konverzní tabulky
	MemFree(tab);
}


/////////////////////////////////////////////////////////////////////////////
// otočení obrázku

void CPicture::Rotate(double angle, bool inter, bool dith)
{
// šířka a výška obrázku
	int width = pData->Width;
	int height = pData->Height;

// kontrola, zda je obrázek platný
	if ((width <= 0) || (height <= 0)) return;

// korekce úhlu
	while (angle < 0) angle += 128*pi2;
	angle = fmod(angle, pi2);

// není rotace
	if ((angle > -0.0001) && (angle < 0.0001)) return;

// zajištění dekomprimace
	DeComp();

// otočení o 90 stupňů
	if ((angle > (pi/2 - 0.0001)) && (angle < (pi/2 + 0.0001)))
	{
		CPicture pic(height, width);
		pic.DeComp();
		pic.CopyWrite();

		BYTE* dst = pic.pData->Data;
		BYTE* src = pData->Data + width*(height - 1);

		for (int j = width; j > 0; j--)
		{
			for (int i = height; i > 0; i--)
			{
				*dst = *src;
				dst++;
				src -= width;
			}
			src += height*width + 1;
		}

		*this = pic;
		return;		
	}

// otočení o 180 stupňů
	if ((angle > (pi - 0.0001)) && (angle < (pi + 0.0001)))
	{
		CopyWrite();

		int i = width * height;
		BYTE* src = pData->Data;
		BYTE* dst = src + i - 1;
		i = i/2;

		for (; i > 0; i--)
		{
			BYTE c = *src;
			*src = *dst;
			*dst = c;
			src++;
			dst--;
		}
		return;
	}

// otočení o 270 stupňů
	if ((angle > (pi*3/2 - 0.0001)) && (angle < (pi*3/2 + 0.0001)))
	{
		CPicture pic(height, width);
		pic.DeComp();
		pic.CopyWrite();

		BYTE* dst = pic.pData->Data;
		BYTE* src = pData->Data + width - 1;

		for (int j = width; j > 0; j--)
		{
			for (int i = height; i > 0; i--)
			{
				*dst = *src;
				dst++;
				src += width;
			}
			src -= height*width + 1;
		}

		*this = pic;
		return;		
	}

// při interpolaci zvětšení obrázku
	if (inter) Zoom(width*2, height*2, false, false);

// původní velikost obrázku
	int oldwidth = pData->Width;
	int oldheight = pData->Height;

// délka a úhel úhlopříčky
	double angle2 = atan2((double)oldheight, (double)oldwidth);
	double delka2 = sqrt((double)oldwidth*oldwidth + (double)oldheight*oldheight);

// kvadrant úhlu otočení
	int kvadrant = RoundM(angle / (pi/2));

// nová šířka a výška
	int newwidth;
	int newheight;

	switch (kvadrant)
	{
	case 0:
		newwidth = Round(delka2*cos(angle-angle2)+0.4);
		newheight = Round(delka2*sin(angle+angle2)+0.4);
		break;

	case 1:
		newwidth = Round(-delka2*cos(angle+angle2)+0.4);
		newheight = Round(delka2*sin(angle-angle2)+0.4);
		break;

	case 2:
		newwidth = Round(-delka2*cos(angle-angle2)+0.4);
		newheight = Round(-delka2*sin(angle+angle2)+0.4);
		break;

	default:
		newwidth = Round(delka2*cos(angle+angle2)+0.4);
		newheight = Round(-delka2*sin(angle-angle2)+0.4);
		break;
	}
	if (newwidth > 0x8000) newwidth = 0x8000;
	if (newwidth <= 0) newwidth = 1;
	if (newheight > 0x8000) newheight = 0x8000;
	if (newheight <= 0) newheight = 1;

// střed nového obrázku
	int xs = newwidth/2;
	int ys = newheight/2;

	int xs2 = (oldwidth+1)/2;
	int ys2 = (oldheight+1)/2;

// vytvoření nového (prázdného) obrázku
	CPicture pic(newwidth, newheight);
	pic.DeComp();
	pic.CopyWrite();

// provedení konverze obrázku
	BYTE* src = pData->Data;
	BYTE* dst = pic.pData->Data;

	for (int y = 0; y < newheight; y++)
	{
		int dy = y - ys;
		double dy2 = (double)dy*dy;

		for (int x = 0; x < newwidth; x++)
		{
			int dx = x - xs;

			angle2 = atan2((double)dy, (double)dx) - angle;
			delka2 = sqrt((double)dx*dx + dy2);

			int xr = Round(delka2*cos(angle2)) + oldwidth - xs2;
			int yr = Round(delka2*sin(angle2)) + oldheight - ys2;

			if (((DWORD)xr < (DWORD)oldwidth) && ((DWORD)yr < (DWORD)oldheight))
			{
				*dst = src[xr + oldwidth*yr];
			}
			else
			{
				*dst = BackCol;
			}

			dst++;
		}
	}

// náhrada novým obrázkem
	*this = pic;

// při interpolaci zmenšení obrázku
	if (inter) Zoom((newwidth+1)/2, (newheight+1)/2, true, dith);
}


/////////////////////////////////////////////////////////////////////////////
// poskytnutí bodu (s kontrolou platnosti offsetu/indexu)

BYTE _fastcall CPicture::Get(const int off) const
{
	if (IsValid(off))				// je offset platný?
	{
		return pData->Data[off];	// bod na daném offsetu
	}
	return BackCol;					// neplatný offset
}

BYTE _fastcall CPicture::Get(const int x, const int y) const
{
	if (IsValid(x, y))				// je index platný?
	{
		return pData->Data[x + y*Width()];	// prvek
	}
	return BackCol;					// neplatný offset
}


////////////////////////////////////////////////////////////////////
// nastavení bodu (s kontrolou platnosti offsetu/indexu)

void _fastcall CPicture::Set(const int off, const BYTE data)
{
	if (IsValid(off))				// je offset platný?
	{
		pData->Data[off] = data;
	}
}

void _fastcall CPicture::Set(const int x, const int y, const BYTE data)
{
	if (IsValid(x, y))				// je index platný?
	{
		pData->Data[x + y*Width()] = data;
	}
}


/////////////////////////////////////////////////////////////////////////////
// kopie nových dat obrázku (rozměry zůstanou nezměněny) - zajistí odpojení dat

void CPicture::CopyData(BYTE* src)
{
	ASSERT(src);
	PICTUREDATA* data = pData;		// adresa starých dat (záhlaví)
	long* refer = &(data->Refer);	// počet referencí
	int width = data->Width;		// šířka obrázku
	int height = data->Height;		// výška obrázku;

	if (*refer > 1)					// je nějaký jiný majitel?
	{
		NewBuffer(width, height);	// vytvoření nového bufferu

// odpojení starých dat - v multithread může nastat i zrušení
		if (LongDecrement(refer))
		{
#ifndef _MINI
			MemFree(data->TextData);
			MemFree(data->TextDataRGBA);
			MemFree(data->TextDataR8G8B8);
			MemFree(data->TextDataR5G6B5);
			MemFree(data->TextDataA1R5G5B5);
			MemFree(data->TextDataA4R4G4B4);
#endif // _MINI

#ifdef _MT
			MemFree(data->Data);	// případné zrušení dat
			MemFree(data);			// případné zrušení záhlaví
#endif	// _MT
		}
	}

	data->Param = PicParamNone;		// parametry
	MemCopy(pData->Data, src, width*height); // kopie obsahu obrázku
}


/////////////////////////////////////////////////////////////////////////////
// kopie nových dat obrázku s konverzí (rozměry zůstanou nezměněny) - zajistí odpojení dat

void CPicture::CopyKonvData(BYTE* src)
{
	ASSERT(src);
	PICTUREDATA* data = pData;		// adresa starých dat (záhlaví)
	long* refer = &(data->Refer);	// počet referencí
	int width = data->Width;		// šířka obrázku
	int height = data->Height;		// výška obrázku;

	if (*refer > 1)					// je nějaký jiný majitel?
	{
		NewBuffer(width, height);	// vytvoření nového bufferu

// odpojení starých dat - v multithread může nastat i zrušení
		if (LongDecrement(refer))
		{
#ifndef _MINI
			MemFree(data->TextData);
			MemFree(data->TextDataRGBA);
			MemFree(data->TextDataR8G8B8);
			MemFree(data->TextDataR5G6B5);
			MemFree(data->TextDataA1R5G5B5);
			MemFree(data->TextDataA4R4G4B4);
#endif // _MINI

#ifdef _MT
			MemFree(data->Data);	// případné zrušení dat
			MemFree(data);			// případné zrušení záhlaví
#endif	// _MT
		}
	}

	data->Param = PicParamNone;		// parametry
	KonvCopy(pData->Data, src, width*height); // kopie obsahu obrázku
}

/////////////////////////////////////////////////////////////////////////////
// horizontální převrácení obrázku

void CPicture::XFlip()
{
// dekomprese obrázku
	DeComp();

// přivlastnění bufferu
	CopyWrite();

// lokální proměnné
	int width = pData->Width;
	BYTE* adr = pData->Data;

// převrácení obrázku
	for (int i = pData->Height; i > 0; i--)
	{
		BYTE* src = adr;
		adr += width;
		BYTE* dst = adr - 1;

		for (int j = width / 2; j > 0; j--)
		{
			BYTE c = *src;
			*src = *dst;
			*dst = c;
			src++;
			dst--;
		}

	}
}

/////////////////////////////////////////////////////////////////////////////
// vertikální převrácení obrázku

void CPicture::YFlip()
{
// dekomprese obrázku
	DeComp();

// přivlastnění bufferu
	CopyWrite();

// lokální proměnné
	int width = pData->Width;
	BYTE* src = pData->Data;
	BYTE* dst = src + width*(pData->Height-1);

// převrácení obrázku
	for (int i = pData->Height / 2; i > 0; i--)
	{
		for (int j = width; j > 0; j--)
		{
			BYTE c = *src;
			*src = *dst;
			*dst = c;
			src++;
			dst++;
		}

		dst -= 2*width;
	}
}

/////////////////////////////////////////////////////////////////////////////
// načtení obrázku ze souboru (tex = importovat texturu)

void CPicture::LoadFile(bool tex)
{
#ifndef _MINI

// úschova offsetu souboru
	int oldoff = FileReadOff;

	CBuf jbuf;			// buffer pro načtení z JPEG
	bool jpeg = false;	// je formát JPEG

// načtení záhlaví souboru
	char hd[88];
	int i;
	for (i = 0; i < 14; i++)
	{
		hd[i] = FileReadByte();
	}

// kontrola záhlaví souboru
	int size = *(int*)(hd + 2) - 14;
	if ((hd[0] != 'B') ||
		(hd[1] != 'M') ||
		(size < 30))
	{

// případný offset začátku dat AVI
		int avidata = oldoff;

// načtení dalších dat pro interpretaci jako AVI
		for (; i < 88; i++)
		{
			hd[i] = FileReadByte();
		}
		AVIMAINHEADER* avi = (AVIMAINHEADER*)(hd + 24);
		size = *(int*)(hd + 16);
		int w = avi->dwWidth;
		int h = avi->dwHeight;

// pokus o interpretaci jako záhlaví AVI souboru
		if ((size > 40) &&
			(hd[0] == 'R') &&
			(hd[1] == 'I') &&
			(hd[2] == 'F') &&
			(hd[3] == 'F') &&

			(hd[8] == 'A') &&
			(hd[9] == 'V') &&
			(hd[10] == 'I') &&
			(hd[11] == ' ') &&
			(hd[12] == 'L') &&
			(hd[13] == 'I') &&
			(hd[14] == 'S') &&
			(hd[15] == 'T') &&

			(hd[20] == 'h') &&
			(hd[21] == 'd') &&
			(hd[22] == 'r') &&
			(hd[23] == 'l') &&
			(hd[24] == 'a') &&
			(hd[25] == 'v') &&
			(hd[26] == 'i') &&
			(hd[27] == 'h') &&

			(w > 4) &&
			(w <= 32768) &&
			(h > 4) &&
			(h <= 32768))
		{
			AviWidth = w;
			AviHeight = h;

// načtení dalšího záhlaví
			FileReadOff += size + 20 - 88;

			for (i = 0; i < 12; i++)
			{
				hd[i] = FileReadByte();
			}
			size = *(int*)(hd + 4);

// přeskočení výplně JUNK
			if ((hd[0] == 'J') &&
				(hd[1] == 'U') &&
				(hd[2] == 'N') &&
				(hd[3] == 'K') &&
				(size > 0))
			{
				FileReadOff += size - 4 + 12;
			}

			avidata = FileReadOff;
		}

// přeskočení nevyužitých dat AVI
		do {
			FileReadOff = avidata;
			for (i = 0; i < 8; i++)
			{
				hd[i] = FileReadByte();
			}
			size = *(int*)(hd + 4);

			avidata = FileReadOff + size;
		} while ((size >= 0) &&
				(
				  (
					(hd[0] == '0') &&
					(((hd[2] == 'd') && (hd[3] == 'c')) ||
					 ((hd[2] == 'p') && (hd[3] == 'c')) ||
					 ((hd[2] == 'w') && (hd[3] == 'b')))
				  )

				  ||

				  (
				    (hd[0] == 'J') &&
					(hd[1] == 'U') &&
					(hd[2] == 'N') &&
					(hd[3] == 'K')
				  )
				 )
				 );

// načtení obrázku z AVI
		if ((size >= 0) &&
			(hd[0] == '0') &&
			(hd[2] == 'd') &&
			(hd[3] == 'b'))
		{
			int size0 = AviWidth * AviHeight * 2;

			if (size > size0) size = size0;

			BYTE* buf = (BYTE*)MemGet(size0);
			FileReadBlok(buf, size);
			if (size < size0) MemFill(buf + size, size0 - size, 0);

			New(AviWidth, AviHeight);

			int* odch = NULL;
			if (Dither)
			{
				odch = (int*)MemGet((3*AviWidth + 6) * sizeof(int));
				MemFill(odch, (3*AviWidth + 6) * sizeof(int), 0);
			}

			BYTE* dst = pData->Data;				// ukládací adresa
			BYTE* src = buf;						// čtecí adresa
			BYTE r, g, b;							// barevné složky
			int j;									// pracovní čítače
			WORD srcdat;							// zdrojová data 16 bitů

			int srcinc = ((2*AviWidth + 3) & ~3) - 2*AviWidth;

			for (i = AviHeight; i > 0; i--)		// cyklus přes všechny linky
			{
				if (Dither)
				{
					int* odch0 = odch + 3;			// ukazatel v bufferu odchylek

					for (j = AviWidth; j > 0; j--)		// cyklus přes všechny body na lince
					{

					// požadovaná barva
						srcdat = *(WORD*)src;		// data jednoho bodu
						b = (BYTE)(srcdat & 0x1F);	// modrá složka
						b = (BYTE)(b*8 + b/4);
						srcdat >>= 5;				// zrušení bitů modré složky
						g = (BYTE)(srcdat & 0x1F);	// zelená složka
						g = (BYTE)(g*8 + g/4);
						srcdat >>= 5;				// zrušení bitů zelené složky
						r = (BYTE)(srcdat & 0x1F);	// červená složka
						r = (BYTE)(r*8 + r/4);
						src++;						// zvýšení zdrojové adresy
						src++;						// zvýšení zdrojové adresy

					// zkorigovaná barva
						int b2 = b - (odch0[-3] + odch0[0] + odch0[3])*5/8;		// modrá složka
						int g2 = g - (odch0[-2] + odch0[1] + odch0[4])*5/8;		// zelená složka
						int r2 = r - (odch0[-1] + odch0[2] + odch0[5])*5/8;		// červená složka

					// omezení přetečení barvy
						if (b2 < 0) b2 = 0;
						if (b2 > 255) b2 = 255;
						if (g2 < 0) g2 = 0;
						if (g2 > 255) g2 = 255;
						if (r2 < 0) r2 = 0;
						if (r2 > 255) r2 = 255;

					// import barvy
						BYTE col = PalImport((BYTE)r2, (BYTE)g2, (BYTE)b2);

					// odchylka aktuálního bodu od požadované barvy
						RGBQUAD* rgb = StdBitmapInfo->bmiColors + col;
						*odch0 = rgb->rgbBlue - b;
						odch0++;

						*odch0 = rgb->rgbGreen - g;
						odch0++;

						*odch0 = rgb->rgbRed - r;
						odch0++;

					// uložení bodu
						*dst = col;	// import barvy do vlastních palet
						dst++;						// zvýšení cílové adresy
					}
				}
				else
				{
					for (j = AviWidth; j > 0; j--)		// cyklus přes všechny body
					{
						srcdat = *(WORD*)src;		// data jednoho bodu
						b = (BYTE)(srcdat & 0x1F);	// modrá složka
						b = (BYTE)(b*8 + b/4);
						srcdat >>= 5;				// zrušení bitů modré složky
						g = (BYTE)(srcdat & 0x1F);	// zelená složka
						g = (BYTE)(g*8 + g/4);
						srcdat >>= 5;				// zrušení bitů zelené složky
						r = (BYTE)(srcdat & 0x1F);	// červená složka
						r = (BYTE)(r*8 + r/4);
						*dst = PalImport(r, g, b);	// import barvy do vlastních palet
						src++;						// zvýšení zdrojové adresy
						src++;
						dst++;						// zvýšení cílové adresy
					}
				}
				src += srcinc;
			}

			MemFree(odch);
			MemFree(buf);
			FileReadOff = avidata;
			return;
		}

// pokus o načtení jako JPEG
		FileReadOff = oldoff;

		if (((BYTE)hd[0] != 0xff) ||
			((BYTE)hd[1] != 0xd8) ||
			((BYTE)hd[2] != 0xff) ||
			!JPEG::Decomp(&jbuf) ||
			(jbuf.Size() < 40) ||
			((jbuf.Adr())[0] != 'B') ||
			((jbuf.Adr())[1] != 'M') ||
			(*(int*)(jbuf.Adr() + 2) > jbuf.Size()) ||
			(*(int*)(jbuf.Adr() + 2) < 44))
		{

// chyba - neplatný formát
			FileReadOff = oldoff;
			FileError = true;
			return;
		}

		size = *(int*)(jbuf.Adr() + 2) - 14;
		jpeg = true;
	}

// příprava bufferu k načtení souboru
	BITMAPINFO* bmp;
	
	if (jpeg)
	{
		bmp = (BITMAPINFO*)(jbuf.Adr() + 14);
	}
	else
	{
		bmp = (BITMAPINFO*)MemGet(size);

// načtení souboru do bufferu
		MemFill(bmp, 30);
		FileReadBlok((BYTE*)bmp, size);
	}

// kontrola platnosti souboru
	if (!TestBMP(bmp))
	{
		if (jpeg)
		{
			MemFree(jbuf.Adr());
		}
		else
		{
			MemFree(bmp);				// uvolnění bufferu
		}
		FileReadOff = oldoff;
		FileError = true;
		return;
	}

// příprava parametrů bitmapy
	int width = bmp->bmiHeader.biWidth;			// šířka
	int height = bmp->bmiHeader.biHeight;		// výška
	int bits = bmp->bmiHeader.biBitCount;		// počet bitů na bod
	int colors = bmp->bmiHeader.biClrUsed;		// počet použitých barev
	if (colors <= 0) colors = (1 << bits);		// implicitní počet barev
	if (bits > 8) colors = 0;					// pro TRUE COLOR nejsou palety
	int sizehead = sizeof(BITMAPINFOHEADER) + colors*sizeof(RGBQUAD); // velikost záhlaví
	size -= sizehead;							// oprava velikosti dat
	if (size < 0) size = 0;

// dekomprese bitmapy RLE8
	if (bmp->bmiHeader.biCompression == BI_RLE8)
	{
		int newsize = ((width + 3) & ~3) * height;
		BITMAPINFO* bmp2;
		bmp2 = (BITMAPINFO*)MemGet(sizehead + newsize + 10000);
		MemCopy(bmp2, bmp, sizehead);
		bmp2->bmiHeader.biCompression = BI_RGB;
		DekompRLE8((BYTE*)bmp2+sizehead, newsize, (BYTE*)bmp+sizehead, size);
		if (jpeg)
		{
			MemFree(jbuf.Adr());
			jpeg = false;
		}
		else
		{
			MemFree(bmp);				// uvolnění bufferu
		}
		bmp = bmp2;
		size = newsize;
	}

// dekomprese bitmapy RLE4
	if (bmp->bmiHeader.biCompression == BI_RLE4)
	{
		int newsize = (((width+1)/2 + 3) & ~3) * height;
		BITMAPINFO* bmp2;
		bmp2 = (BITMAPINFO*)MemGet(sizehead + newsize + 10000);
		MemCopy(bmp2, bmp, sizehead);
		bmp2->bmiHeader.biCompression = BI_RGB;
		DekompRLE4((BYTE*)bmp2+sizehead, newsize, (BYTE*)bmp+sizehead, size);
		if (jpeg)
		{
			MemFree(jbuf.Adr());
			jpeg = false;
		}
		else
		{
			MemFree(bmp);				// uvolnění bufferu
		}
		bmp = bmp2;
		size = newsize;
	}

// úschova původního obrázku
	CPicture pic(*this);

// vytvoření nového obrázku
	New(width, height);

// vygenerování konverzní tabulky palet
	if (bits <= 8)
	{
		GenKonvPal(bmp);
	}

// příprava bufferu odchylky pro dithering
	int* odch = NULL;
	if (Dither)
	{
		odch = (int*)MemGet((3*width + 6) * sizeof(int));
		MemFill(odch, (3*width + 6) * sizeof(int), 0);
	}

// příprava parametrů ke konverzi
	BYTE* dst = pData->Data;				// ukládací adresa
	BYTE* src = (BYTE*)bmp + sizehead;		// čtecí adresa
	int srcinc;								// přírustek zdrojové adresy
	int j;									// pracovní čítače
	BYTE r, g, b;							// barevné složky
	WORD srcdat;							// zdrojová data 16 bitů

// rozlišení podle počtu bodů
	switch (bits)
	{

// 1 bit
	case 1:
		srcinc = ((width+7)/8 + 3) & ~3;	// přírustek zdrojové adresy

		for (i = height; i > 0; i--)		// cyklus přes všechny linky
		{
			for (j = 0; j < width; j++)
			{
				*dst = KonvPal[(src[j/8] >> (7 - (j & 7))) & 1];
				dst++;
			}
			src += srcinc;					// zvýšení zdrojové adresy
		}
		break;

// 4 bity
	case 4:
		srcinc = ((width+1)/2 + 3) & ~3;	// přírustek zdrojové adresy

		for (i = height; i > 0; i--)		// cyklus přes všechny linky
		{
			for (j = 0; j < width; j++)
			{
				if (j & 1)
				{
					*dst = KonvPal[src[j/2] & 0xf];
				}
				else
				{
					*dst = KonvPal[src[j/2] >> 4];
				}
				dst++;
			}
			src += srcinc;					// zvýšení zdrojové adresy
		}
		break;

// 8 bitů
	case 8:
		srcinc = ((width + 3) & ~3) - width; // přírustek zdrojové adresy

		for (i = height; i > 0; i--)		// cyklus přes všechny linky
		{
			if (Dither)
			{
				int* odch0 = odch + 3;			// ukazatel v bufferu odchylek

				for (j = width; j > 0; j--)		// cyklus přes všechny body
				{
				// bod k zápisu
					BYTE col = *src;
					src++;
					if (col >= colors) col = 0;
					RGBQUAD* rgb = bmp->bmiColors + col;

				// pozadí
					if ((*(int*)rgb & 0xffffff) == (BACKCOLOR_BLUE | (BACKCOLOR_GREEN*256) | (BACKCOLOR_RED*256*256)))
					{
						col = BackCol;

						*odch0 = 0;
						odch0++;
						*odch0 = 0;
						odch0++;
						*odch0 = 0;
						odch0++;
					}
					else
					{	

				// stín
					  if ((*(int*)rgb & 0xffffff) == (SHADCOLOR_BLUE | (SHADCOLOR_GREEN*256) | (SHADCOLOR_RED*256*256)))
					  {
						col = ShadCol;

						*odch0 = 0;
						odch0++;
						*odch0 = 0;
						odch0++;
						*odch0 = 0;
						odch0++;
					  }
					  else
					  {	

				// požadovaná barva
						b = rgb->rgbBlue;			// modrá složka
						g = rgb->rgbGreen;			// zelená složka
						r = rgb->rgbRed;			// červená složka

				// zkorigovaná barva
						int b2 = b - (odch0[-3] + odch0[0] + odch0[3])*5/8;		// modrá složka
						int g2 = g - (odch0[-2] + odch0[1] + odch0[4])*5/8;		// zelená složka
						int r2 = r - (odch0[-1] + odch0[2] + odch0[5])*5/8;		// červená složka

				// omezení přetečení barvy
						if (b2 < 0) b2 = 0;
						if (b2 > 255) b2 = 255;
						if (g2 < 0) g2 = 0;
						if (g2 > 255) g2 = 255;
						if (r2 < 0) r2 = 0;
						if (r2 > 255) r2 = 255;

				// import barvy
						col = PalImport((BYTE)r2, (BYTE)g2, (BYTE)b2);

				// odchylka aktuálního bodu od požadované barvy
						rgb = StdBitmapInfo->bmiColors + col;
						*odch0 = rgb->rgbBlue - b;
						odch0++;

						*odch0 = rgb->rgbGreen - g;
						odch0++;

						*odch0 = rgb->rgbRed - r;
						odch0++;
					  }
					}

				// uložení bodu
					*dst = col;
					dst++;						// zvýšení cílové adresy
				}
			}
			else
			{
				KonvCopy(dst, src, width);
				src += width;
				dst += width;
			}
			src += srcinc;					// zvýšení zdrojové adresy
		}
		break;

// 16 bitů
	case 16:
		srcinc = ((2*width + 3) & ~3) - 2*width;

		for (i = height; i > 0; i--)		// cyklus přes všechny linky
		{
			if (Dither)
			{
				int* odch0 = odch + 3;			// ukazatel v bufferu odchylek

				for (j = width; j > 0; j--)		// cyklus přes všechny body na lince
				{

				// požadovaná barva
					srcdat = *(WORD*)src;		// data jednoho bodu
					b = (BYTE)(srcdat & 0x1F);	// modrá složka
					b = (BYTE)(b*8 + b/4);
					srcdat >>= 5;				// zrušení bitů modré složky
					g = (BYTE)(srcdat & 0x1F);	// zelená složka
					g = (BYTE)(g*8 + g/4);
					srcdat >>= 5;				// zrušení bitů zelené složky
					r = (BYTE)(srcdat & 0x1F);	// červená složka
					r = (BYTE)(r*8 + r/4);
					src++;						// zvýšení zdrojové adresy
					src++;						// zvýšení zdrojové adresy

				// zkorigovaná barva
					int b2 = b - (odch0[-3] + odch0[0] + odch0[3])*5/8;		// modrá složka
					int g2 = g - (odch0[-2] + odch0[1] + odch0[4])*5/8;		// zelená složka
					int r2 = r - (odch0[-1] + odch0[2] + odch0[5])*5/8;		// červená složka

				// omezení přetečení barvy
					if (b2 < 0) b2 = 0;
					if (b2 > 255) b2 = 255;
					if (g2 < 0) g2 = 0;
					if (g2 > 255) g2 = 255;
					if (r2 < 0) r2 = 0;
					if (r2 > 255) r2 = 255;

				// import barvy
					BYTE col = PalImport((BYTE)r2, (BYTE)g2, (BYTE)b2);

				// odchylka aktuálního bodu od požadované barvy
					RGBQUAD* rgb = StdBitmapInfo->bmiColors + col;
					*odch0 = rgb->rgbBlue - b;
					odch0++;

					*odch0 = rgb->rgbGreen - g;
					odch0++;

					*odch0 = rgb->rgbRed - r;
					odch0++;

				// uložení bodu
					*dst = col;	// import barvy do vlastních palet
					dst++;						// zvýšení cílové adresy
				}
			}
			else
			{
				for (j = width; j > 0; j--)		// cyklus přes všechny body
				{
					srcdat = *(WORD*)src;		// data jednoho bodu
					b = (BYTE)(srcdat & 0x1F);	// modrá složka
					b = (BYTE)(b*8 + b/4);
					srcdat >>= 5;				// zrušení bitů modré složky
					g = (BYTE)(srcdat & 0x1F);	// zelená složka
					g = (BYTE)(g*8 + g/4);
					srcdat >>= 5;				// zrušení bitů zelené složky
					r = (BYTE)(srcdat & 0x1F);	// červená složka
					r = (BYTE)(r*8 + r/4);
					*dst = PalImport(r, g, b);	// import barvy do vlastních palet
					src++;						// zvýšení zdrojové adresy
					src++;
					dst++;						// zvýšení cílové adresy
				}
			}
			src += srcinc;
		}
		break;

// 24 bitů
	case 24:
		srcinc = ((3*width + 3) & ~3) - 3*width;

		for (i = height; i > 0; i--)			// cyklus přes všechny linky
		{
			if (Dither)
			{
				int* odch0 = odch + 3;			// ukazatel v bufferu odchylek

				for (j = width; j > 0; j--)		// cyklus přes všechny body na lince
				{

				// pozadí
					if ((*(int*)src & 0xffffff) == (BACKCOLOR_BLUE | (BACKCOLOR_GREEN*256) | (BACKCOLOR_RED*256*256)))
					{
						*dst = BackCol;
						src += 3;

						*odch0 = 0;
						odch0++;
						*odch0 = 0;
						odch0++;
						*odch0 = 0;
						odch0++;
					}
					else
					{	

				// stín
					  if ((*(int*)src & 0xffffff) == (SHADCOLOR_BLUE | (SHADCOLOR_GREEN*256) | (SHADCOLOR_RED*256*256)))
					  {
						*dst = ShadCol;
						src += 3;

						*odch0 = 0;
						odch0++;
						*odch0 = 0;
						odch0++;
						*odch0 = 0;
						odch0++;
					  }
					  else
					  {	

				// požadovaná barva
						b = *src;					// modrá složka
						src++;						// zvýšení zdrojové adresy
						g = *src;					// zelená složka
						src++;						// zvýšení zdrojové adresy
						r = *src;					// červená složka
						src++;						// zvýšení zdrojové adresy

				// zkorigovaná barva
						int b2 = b - (odch0[-3] + odch0[0] + odch0[3])*5/8;		// modrá složka
						int g2 = g - (odch0[-2] + odch0[1] + odch0[4])*5/8;		// zelená složka
						int r2 = r - (odch0[-1] + odch0[2] + odch0[5])*5/8;		// červená složka

				// omezení přetečení barvy
						if (b2 < 0) b2 = 0;
						if (b2 > 255) b2 = 255;
						if (g2 < 0) g2 = 0;
						if (g2 > 255) g2 = 255;
						if (r2 < 0) r2 = 0;
						if (r2 > 255) r2 = 255;

				// import barvy
						BYTE col = PalImport((BYTE)r2, (BYTE)g2, (BYTE)b2);

				// odchylka aktuálního bodu od požadované barvy
						RGBQUAD* rgb = StdBitmapInfo->bmiColors + col;
						*odch0 = rgb->rgbBlue - b;
						odch0++;

						*odch0 = rgb->rgbGreen - g;
						odch0++;

						*odch0 = rgb->rgbRed - r;
						odch0++;

				// uložení bodu
						*dst = col;	// import barvy do vlastních palet
					  }
					}
					dst++;						// zvýšení cílové adresy
				}
			}
			else
			{
				for (j = width; j > 0; j--)		// cyklus přes všechny body
				{
					if ((*(int*)src & 0xffffff) == (BACKCOLOR_BLUE | (BACKCOLOR_GREEN*256) | (BACKCOLOR_RED*256*256)))
					{
						*dst = BackCol;
						src += 3;
					}
					else
					{
				// stín
					  if ((*(int*)src & 0xffffff) == (SHADCOLOR_BLUE | (SHADCOLOR_GREEN*256) | (SHADCOLOR_RED*256*256)))
					  {
						*dst = ShadCol;
						src += 3;
					  }
					  else
					  {	
						b = *src;					// modrá složka
						src++;						// zvýšení zdrojové adresy
						g = *src;					// zelená složka
						src++;						// zvýšení zdrojové adresy
						r = *src;					// červená složka
						src++;						// zvýšení zdrojové adresy
						*dst = PalImport(r, g, b);	// import barvy do vlastních palet
					  }
					}
					dst++;						// zvýšení cílové adresy
				}
			}
			src += srcinc;
		}
		break;

// 32 bitů
	case 32:
		for (i = height; i > 0; i--)		// cyklus přes všechny linky
		{
			if (Dither)
			{
				int* odch0 = odch + 3;			// ukazatel v bufferu odchylek

				for (j = width; j > 0; j--)		// cyklus přes všechny body na lince
				{

				// pozadí
					if ((*(int*)src & 0xffffff) == (BACKCOLOR_BLUE | (BACKCOLOR_GREEN*256) | (BACKCOLOR_RED*256*256)))
					{
						*dst = BackCol;
						src += 4;

						*odch0 = 0;
						odch0++;
						*odch0 = 0;
						odch0++;
						*odch0 = 0;
						odch0++;
					}
					else
					{	

				// stín
					  if ((*(int*)src & 0xffffff) == (SHADCOLOR_BLUE | (SHADCOLOR_GREEN*256) | (SHADCOLOR_RED*256*256)))
					  {
						*dst = ShadCol;
						src += 4;

						*odch0 = 0;
						odch0++;
						*odch0 = 0;
						odch0++;
						*odch0 = 0;
						odch0++;
					  }
					  else
					  {	

				// požadovaná barva
						b = *src;					// modrá složka
						src++;						// zvýšení zdrojové adresy
						g = *src;					// zelená složka
						src++;						// zvýšení zdrojové adresy
						r = *src;					// červená složka
						src++;						// zvýšení zdrojové adresy
						src++;						// zvýšení zdrojové adresy

				// zkorigovaná barva
						int b2 = b - (odch0[-3] + odch0[0] + odch0[3])*5/8;		// modrá složka
						int g2 = g - (odch0[-2] + odch0[1] + odch0[4])*5/8;		// zelená složka
						int r2 = r - (odch0[-1] + odch0[2] + odch0[5])*5/8;		// červená složka

				// omezení přetečení barvy
						if (b2 < 0) b2 = 0;
						if (b2 > 255) b2 = 255;
						if (g2 < 0) g2 = 0;
						if (g2 > 255) g2 = 255;
						if (r2 < 0) r2 = 0;
						if (r2 > 255) r2 = 255;

				// import barvy
						BYTE col = PalImport((BYTE)r2, (BYTE)g2, (BYTE)b2);

				// odchylka aktuálního bodu od požadované barvy
						RGBQUAD* rgb = StdBitmapInfo->bmiColors + col;
						*odch0 = rgb->rgbBlue - b;
						odch0++;

						*odch0 = rgb->rgbGreen - g;
						odch0++;

						*odch0 = rgb->rgbRed - r;
						odch0++;

				// uložení bodu
						*dst = col;	// import barvy do vlastních palet
					  }
					}
					dst++;						// zvýšení cílové adresy
				}
			}
			else
			{
				for (j = width; j > 0; j--)		// cyklus přes všechny body
				{
					if ((*(int*)src & 0xffffff) == (BACKCOLOR_BLUE | (BACKCOLOR_GREEN*256) | (BACKCOLOR_RED*256*256)))
					{
						*dst = BackCol;
						src += 4;
					}
					else
					{
				// stín
					  if ((*(int*)src & 0xffffff) == (SHADCOLOR_BLUE | (SHADCOLOR_GREEN*256) | (SHADCOLOR_RED*256*256)))
					  {
						*dst = ShadCol;
						src += 4;
					  }
					  else
					  {	
						b = *src;					// modrá složka
						src++;						// zvýšení zdrojové adresy
						g = *src;					// zelená složka
						src++;						// zvýšení zdrojové adresy
						r = *src;					// červená složka
						src++;						// zvýšení zdrojové adresy
						src++;						// zvýšení zdrojové adresy
						*dst = PalImport(r, g, b);	// import barvy do vlastních palet
					  }
					}
					dst++;						// zvýšení cílové adresy
				}
			}
		}
		break;
	}

// uvolnění bufferu odchylky pro dithering
	MemFree(odch);

// vygenerování textur
	if (tex & ((bits == 24) || (bits == 32)))
	{
		int newwidth = width;
		int newheight = height;

		int k = (newwidth*3 + 2) / 4;
		newwidth = 1;
		while (newwidth < k) newwidth = (newwidth << 1);

		k = (newheight*3 + 2) / 4;
		newheight = 1;
		while (newheight < k) newheight = (newheight << 1);

		BYTE* buf = (BYTE*)MemGet(newwidth * newheight * 5);

		pData->TextIsMip = true;
		pData->TextDevType = D3DIntAkt*100 + D3DDevAkt;
		pData->TextTrans = (bits == 32);
		pData->TextSmooth = 0;

		src = (BYTE*)bmp + sizehead;		// čtecí adresa
		BYTE* dst = buf;

		if (bits == 24)
		{
			srcinc = ((3*width + 3) & ~3) - 3*width;

			for (i = height; i > 0; i--)			// cyklus přes všechny linky
			{
				for (j = width; j > 0; j--)		// cyklus přes všechny body na lince
				{
					*((DWORD*)dst) = *((DWORD*)src) & 0xFFFFFF;
					src += 3;
					dst += 4;
					*dst = 0;					// stín
					dst++;
				}
				src += srcinc;
			}
		}
		else
		{
			for (i = height; i > 0; i--)		// cyklus přes všechny linky
			{
				for (j = width; j > 0; j--)		// cyklus přes všechny body na lince
				{
					*((DWORD*)dst) = *((DWORD*)src) ^ 0xFF000000;
					src += 4;
					dst += 4;
					*dst = 0;					// stín
					dst++;
				}
			}
		}

		ImportTexture(buf, newwidth, newheight);
	}

// uvolnění bufferu
	if (jpeg)
	{
		MemFree(jbuf.Adr());
	}
	else
	{
		MemFree(bmp);				// uvolnění bufferu
	}

// příznak - obrázek načten OK
	return;

#endif // _MINI
}


/////////////////////////////////////////////////////////////////////////////
// uložení do souboru

void CPicture::SaveFile()
{
#ifndef _MINI

// dekomprese obrázku
	DeComp();

// příprava velikosti záhlaví souboru
	int headsize = sizeof(BITMAPFILEHEADER) + 
					sizeof(BITMAPINFOHEADER) +
					StdColors*sizeof(RGBQUAD);

// příprava bufferu pro obrázek
	BYTE* buf = (BYTE*)MemGet(headsize + (pData->Width+6)*pData->Height*2 + 1000);

// komprese dat
	int size = KompRLE8(buf + headsize, pData->Data, pData->Width, pData->Height);

// naplnění záhlaví popisovače souboru
	BITMAPFILEHEADER* head = (BITMAPFILEHEADER*) buf;
	buf[0] = 'B';								// identifikace souboru
	buf[1] = 'M';
	head->bfSize = headsize + size;				// velikost souboru
	head->bfReserved1 = 0;
	head->bfReserved2 = 0;
	head->bfOffBits = headsize;					// offset dat

// naplnění záhlaví popisovače dat obrázku
	BITMAPINFOHEADER* bmp = (BITMAPINFOHEADER*)(buf + sizeof(BITMAPFILEHEADER));
	bmp->biSize = sizeof(BITMAPINFOHEADER);		// velikost struktury
	bmp->biWidth = pData->Width;				// šířka
	bmp->biHeight = pData->Height;				// výška
	bmp->biPlanes = 1;							// počet barevných rovin
	bmp->biBitCount = 8;						// počet bitů na bod
	bmp->biCompression = BI_RLE8;				// komprese
	bmp->biSizeImage = size;					// velikost dat obrázku
	bmp->biXPelsPerMeter = 5906;				// horizontálně 150 bodů na palec
	bmp->biYPelsPerMeter = 5906;				// vertikálně 150 bodů na palec
	bmp->biClrUsed = StdColors;					// počet použitých palet
	bmp->biClrImportant = StdColors;			// počet důležitých palet

// přenesení palet
	MemCopy(buf + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER),
				StdBitmapInfo->bmiColors, StdColors*sizeof(RGBQUAD));

// uložení souboru
	size += headsize;
	FileWriteBlok((BYTE*)buf, size);

// zrušení bufferu
	MemFree(buf);

#endif // _MINI
}


/////////////////////////////////////////////////////////////////////////////
// operátor přiřazení

const CPicture& CPicture::operator= (const CPicture& src)
{
	Detach();				// zrušení starých dat
	Attach(src.pData);		// přiřazení nových dat
	return *this;
}

const CPicture& CPicture::operator= (PICTUREDATA* src)
{
	Detach();				// zrušení starých dat
	Attach(src);			// přiřazení nových dat
	return *this;
}

const CPicture& CPicture::operator= (CIcon& icon)
{
	icon.DeComp();									// dekomprese dat ikony
	Detach();										// odpojení starého obrázku
	NewBuffer(ICONWIDTH, ICONHEIGHT);				// vytvoření nového bufferu
	MemCopy(pData->Data, icon.DataData(), ICONSIZE); // kopie obsahu obrázku
	return *this;
}

const CPicture& CPicture::operator= (CMap& map)
{
// parametry plochy
	int mapwidth = map.Width();
	int mapheight = map.Height();

// parametry obrázku
	int picwidth = mapwidth * ICONWIDTH;
	int picheight = mapheight * ICONHEIGHT;

// vytvoření nového obrázku
	Detach();
	NewBuffer(picwidth, picheight);

// příprava ukazatelů
	BYTE* dst = pData->Data;
	MAPITEM* item = map.DataData();
	CIcon* icon;
	BYTE* src;

// přenesení dat ikon (po řádcích ikon)
	for (int i = mapheight; i > 0; i--)
	{
		for (int j = mapwidth; j > 0; j--)
		{
			icon = &(item->Icon);
			icon->DeComp();
			src = icon->DataData();

			for (int k = ICONHEIGHT; k > 0; k--)
			{
				MemCopy(dst, src, ICONWIDTH);
				dst += picwidth;
				src += ICONWIDTH;
			}

			dst -= ICONHEIGHT*picwidth;
			dst += ICONWIDTH;
			item++;
		}

		dst += picwidth*(ICONHEIGHT-1);
	}
	return *this;
}


/***************************************************************************\
*																			*
*								Buffer obrázků								*
*																			*
\***************************************************************************/


////////////////////////////////////////////////////////////////////
// konstruktor a destruktor

CBufPic::CBufPic()
{
	m_Data = NULL;			// není buffer dat
	m_Num = 0;				// není žádná platná položka
	m_Max = 0;				// není buffer položek
}

CBufPic::~CBufPic()
{
	DelAll();				// zrušení všech položek
}


////////////////////////////////////////////////////////////////////
// statický konstruktor a destruktor

void CBufPic::Init()
{
	m_Data = NULL;			// není buffer dat
	m_Num = 0;				// není žádná platná položka
	m_Max = 0;				// není buffer položek
}

void CBufPic::Term()
{
	DelAll();				// zrušení všech položek
}


////////////////////////////////////////////////////////////////////
// vytvoření nových dat (odděleno kvůli lepší optimalizaci)

void CBufPic::NewData()
{
	m_Max *= 2;
	if (m_Max == 0) m_Max = 0x400;
//	m_Max += 0x400;				// 4 KB
	MemBuf(m_Data, m_Max);
}


////////////////////////////////////////////////////////////////////
// zrušení všech položek v bufferu (ukládání začne opět po řadě)

void CBufPic::DelAll()
{
	Del(m_Num);					// zrušení položek
	MemBuf(m_Data, 0);			// zrušení bufferu dat
	m_Max = 0;					// není žádná položka v bufferu
}

////////////////////////////////////////////////////////////////////
// poskytnutí položky (s kontrolou platnosti indexu)

const CPicture& _fastcall CBufPic::Get(const int index) const
{
	if (IsValid(index))			// je index platný?
	{
		return m_Data[index];	// položka na daném indexu
	}
	return EmptyPicture;		// pro neplatný index vrátí prázdný obrázek
}

////////////////////////////////////////////////////////////////////
// nastavení položky (s kontrolou platnosti indexu)

void _fastcall CBufPic::Set(const int index, const CPicture& data)
{
	if (IsValid(index))			// je index platný?
	{
		m_Data[index] = data;
	}
}


/////////////////////////////////////////////////////////////////////////////
// vyprázdnění položky (bez jejího zrušení - jen pro uvolnění dat)

void _fastcall CBufPic::Empty(const int index)
{
	if (IsValid(index))					// je index platný?
	{
		m_Data[index].Empty();
	}
}


////////////////////////////////////////////////////////////////////
// zrušení položek z konce bufferu

void _fastcall CBufPic::Del(int num)
{
	if (num > m_Num) num = m_Num;

	CPicture* pic = m_Data + m_Num - 1;
	m_Num -= num;

	for (; num > 0; num--)
	{
		pic->Term();
		pic--;
	}
}


////////////////////////////////////////////////////////////////////
// vytvoření položky (vrací index položky)

int CBufPic::New()
{
	int result = NewItem();		// vytvoření nové položky
	m_Data[result].Init();		// inicializace položky
	return result;
}

int CBufPic::New(int width, int height)
{
	int result = NewItem();		// vytvoření nové položky
	m_Data[result].Init(width, height);	// inicializace položky
	m_Data[result].Clear();		// vymazání obrázku
	return result;
}

////////////////////////////////////////////////////////////////////
// přidání položky (vrací index položky)

int _fastcall CBufPic::Add(const CPicture& data)
{
	int result = NewItem();		// vytvoření nové položky
	m_Data[result].Init(data.Data());	// inicializace položky
	return result;
}

int _fastcall CBufPic::Add(PICTUREDATA* data)
{
	int result = NewItem();		// vytvoření nové položky
	m_Data[result].Init(data);	// inicializace položky
	return result;
}


////////////////////////////////////////////////////////////////////
// duplikace položky (s kontrolou platnosti indexu, vrací index první položky)

int _fastcall CBufPic::Dup(const int index)
{
	int result = NewItem();		// vytvoření nové položky

	if (IsValid(index))			// je index platný?
	{
		m_Data[result].Init(m_Data[index].Data());	// kopie položky
	}
	else
	{
		m_Data[result].Init();		// inicializace neplatné položky
	}
	return result;
}

int _fastcall CBufPic::Dup(const int index, int num)
{
	int result = NewItem();		// vytvoření nové položky

	if (IsValid(index))					// je index platný?
	{
		PICTUREDATA* data = m_Data[index].Data();

		m_Data[result].Init(data);		// kopie položky

		for (num--; num > 0; num--)
		{
			m_Data[NewItem()].Init(data);
		}
	}
	else
	{
		m_Data[result].Init();		// inicializace neplatné položky

		for (num--; num > 0; num--)
		{
			m_Data[NewItem()].Init();
		}
	}
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// operátor přiřazení

const CBufPic& _fastcall CBufPic::operator= (const CBufPic& src)
{
	Del(m_Num);					// zrušení starých dat

	int index = 0;				// index načítané položky
	int i = src.m_Num;			// velikost zdrojového bufferu

	for (; i > 0; i--)			// pro všechny položky v bufferu
	{
		Add(src[index]);	// kopie položky
		index++;				// inkrementace čtecího indexu
	}
	ASSERT(m_Num == src.m_Num);
	return *this;
}

