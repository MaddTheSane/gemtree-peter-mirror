
#include "Main.h"

/***************************************************************************\
*																			*
*								Obrázkové proměnné							*
*																			*
\***************************************************************************/

////////////////////////////////////////////////////////////////////
// Pozn.: linky souboru BMP pro 256 barev jsou zarovnávány na 4 bajty
//        linky souboru BMP pro 2 barvy (maska) jsou zarovnány na 2 bajty
////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// inicializační prázdný obrázek (modifikuje se počet referencí!)

// Prázdný obrázek musí mít rozměry ICONSIZE kvůli vytvoření nového sprajtu!
PICTUREDATA	EmptyPictureData		= { 1, ICONWIDTH, ICONHEIGHT, PicParamBack, 0, 0, 0, NULL };
const CPicture	EmptyPicture;				// prázdný obrázek

/////////////////////////////////////////////////////////////////////////////
// statická inicializace obrázků (při chybě paměti vrací FALSE)

bool InitPicture()
{
// prázdný obrázek
	EmptyPictureData.Data = (BYTE*)MemGet(ICONSIZE);
	if (EmptyPictureData.Data == NULL) return false;
	MemFill(EmptyPictureData.Data, ICONSIZE, BackCol);
	return true;
}

////////////////////////////////////////////////////////////////////
// vytvoření dat obrázku (při chybě paměti vrací NULL)

PICTUREDATA* _fastcall NewPictureData(int width, int height)
{
	ASSERT((width > 0) && (height > 0));

// vytvoření záhlaví obrázku
	PICTUREDATA* data = (PICTUREDATA*)MemGet(SIZEOFPICTUREDATA); // vytvoření záhlaví
	if (data != NULL)
	{

// nastavení dat obrázku
		data->Refer = 1;					// počet referencí
		data->Width = width;				// šířka
		data->Height = height;				// výška
		data->Param = PicParamNone;			// parametry (obsah neznámý)

// vytvoření bufferu dat obrázku
		BYTE* datadata = (BYTE*)MemGet(width*height);
		data->Data = datadata;				// adresa dat

// při chybě paměti zrušení záhlaví obrázku
		if (datadata == NULL)
		{
			MemFree(data);
			data = NULL;
		}
	}
	return data;
}

/////////////////////////////////////////////////////////////////////////////
// zrušení dat obrázku (odděleno kvůli lepší optimalizaci)

void _fastcall DelPictureData(PICTUREDATA* data)
{
	ASSERT(data != NULL);
	ASSERT(data->Data != NULL);

	MemFree(data->Data);		// zrušení dat obrázku
	MemFree(data);				// zrušení záhlaví obrázku
}


/////////////////////////////////////////////////////////////////////////////
// konstruktor a destruktor

CPicture::CPicture() 
{ 
	attach(&EmptyPictureData); 
};

CPicture::CPicture(const CPicture& src) 
{ 
	attach(src.pData); 
};

CPicture::CPicture(PICTUREDATA* data) 
{ 
	attach(data); 
};

CPicture::~CPicture() 
{ 
	detach(pData); 
};


/////////////////////////////////////////////////////////////////////////////
// statický konstruktor a destruktor

void CPicture::Init()
{ 
	attach(&EmptyPictureData); 
};

void _fastcall CPicture::Init(const CPicture& src) 
{ 
	attach(src.pData); 
};

void _fastcall CPicture::Init(PICTUREDATA* data)
{ 
	attach(data); 
};

bool _fastcall CPicture::Init(int width, int height) // při chybě paměti vrací FALSE, obrázek není vytvořen
{
	pData = NewPictureData(width, height);
	return (pData != NULL);
};

void CPicture::Term()
{ 
	detach(pData); 
};


/////////////////////////////////////////////////////////////////////////////
// vyprázdnění obrázku (uvolnění dat)

void CPicture::Empty()
{ 
	Term();
	Init();
}


////////////////////////////////////////////////////////////////////
// vymazání obsahu obrázku (naplnění průhlednou barvou), zajistí přivlastnění (a dekomprimaci) bufferu,
// při chybě paměti vrací FALSE, původní obsah nezměněn

bool CPicture::Clear()
{
// vytvoření nového bufferu, je-li potřeba
	if (!New()) return false;

// vymazání bufferu
	MemFill(pData->Data, pData->Width * pData->Height, BackCol);
	
// nastavení parametrů na pozadí	
	pData->Param = PicParamBack;
	return true;
}


////////////////////////////////////////////////////////////////////
// vymazání obsahu obrázku s nastavením velikosti (naplnění průhlednou barvou), zajistí přivlastnění (a dekomprimaci) bufferu,
// při chybě paměti vrací FALSE, původní obsah nezměněn

bool _fastcall CPicture::Clear(int width, int height)
{
// vytvoření nového bufferu, je-li potřeba
	if (!New(width, height)) return false;

// vymazání bufferu
	MemFill(pData->Data, pData->Width * pData->Height, BackCol);
	
// nastavení parametrů na pozadí	
	pData->Param = PicParamBack;
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// kopie obrázku do vlastního bufferu před modifikací (komprimovaná data zůstanou komprimovaná)
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool CPicture::CopyWrite()
{
// úschova ukazatelů
	PICTUREDATA* olddata = pData;	// adresa starých dat
	long* refer = &(olddata->Refer);// počet referencí

// test, zda je nutné přivlastnění
	if (*refer > 1)					// je nějaký jiný majitel?
	{

// vytvoření nového bufferu
		int size;
		PICTUREDATA* newdata;
		int width = olddata->Width;
		int height = olddata->Height;

		if (olddata->Param == PicParamComp)
		{
			size = *(long*)(olddata->Data) + 4;
			newdata = NewPictureData(size, 1);
			if (newdata == NULL) return false;
			newdata->Width = width;
			newdata->Height = height;
		}
		else
		{
			size = width * height;
			newdata = NewPictureData(width, height);
			if (newdata == NULL) return false;
		}

// přenesení dat
		MemCopy(newdata->Data, olddata->Data, size);
		newdata->Param = olddata->Param;

// odpojení starých dat
		detach(olddata);

// připojení nových dat
		pData = newdata;
	}

// operace OK
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření nového obrázku se stejnou velikostí (připraveno k zápisu, data jsou náhodná)
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool CPicture::New()
{		 
// ukazatel na stará data
	PICTUREDATA* olddata = pData;			// adresa starých dat

// test, zda je nutné vytvoření nového bufferu
	if ((olddata->Refer > 1) ||				// na buffer je více referencí
		(olddata->Param == PicParamComp))	// data jsou komprimovaná
	{

// vytvoření nového bufferu
		PICTUREDATA* newdata = NewPictureData(pData->Width, pData->Height);
		if (newdata == NULL) return false;

// odpojení starých dat
		detach(olddata);

// připojení nových dat
		pData = newdata;
	}

// operace OK
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření nového obrázku (připraveno k zápisu, data jsou náhodná)
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool _fastcall CPicture::New(int width, int height)
{		 
// ukazatel na stará data
	PICTUREDATA* olddata = pData;			// adresa starých dat

// test, zda je nutné vytvoření nového bufferu
	if ((olddata->Refer > 1) ||				// na buffer je více referencí
		(olddata->Width != width) ||		// nesouhlasí šířka
		(olddata->Height != height) ||		// nesouhlasí výška
		(olddata->Param == PicParamComp))	// data jsou komprimovaná
	{

// vytvoření nového bufferu
		PICTUREDATA* newdata = NewPictureData(width, height);
		if (newdata == NULL) return false;

// odpojení starých dat
		detach(olddata);

// připojení nových dat
		pData = newdata;
	}

// operace OK
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// nastavení nových rozměrů obrázku (nová data jsou vymazána)
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool CPicture::Resize(int width, int height)
{
// zajištění dekomprimace
	if (!DeComp()) return false;

// přivlastnění bufferu
	if (!CopyWrite()) return false;

// úschova starých rozměrů
	int oldwidth = pData->Width;
	int oldheight = pData->Height;

// test, zda je potřeba velikost obrázku měnit
	if ((width != oldwidth) || (height != oldheight))
	{

// vytvoření nového bufferu
		PICTUREDATA* olddata = pData;
		PICTUREDATA* newdata = NewPictureData(width, height);
		if (newdata == NULL) return false;

// příprava počtu linek ke kopii
		int i = height;
		if (oldheight < i) i = oldheight;

// příprava délky linky ke kopii
		int j = width;
		if (oldwidth < j) j = oldwidth;

// příprava zbytku linky k vymazání
		int k = width - j;

// příprava zdrojové a cílové adresy
		BYTE* src = olddata->Data;
		BYTE* dst = newdata->Data;

// kopie platných linek při shodě délek linek
		if (width == oldwidth)
		{
			i *= width;
			MemCopy(dst, src, i);
			dst += i;
			src += i;
		}

// kopie platných linek při rozdílné délce linek
		else
		{
			if (k == 0)
			{
				for (; i > 0; i--)
				{
					MemCopy(dst, src, j);
					dst += width;
					src += oldwidth;
				}
			}
			else
			{
				for (; i > 0; i--)
				{
					MemCopy(dst, src, j);
					dst += j;
					MemFill(dst, k, BackCol);
					dst += k;
					src += oldwidth;
				}
			}
		}

// vymazání zbylých linek
		i = height - oldheight;
		if (i > 0)
		{
			MemFill(dst, i*width, BackCol);
		}

// odpojení starých dat
		detach(olddata);

// připojení nových dat
		pData = newdata;
	}

// operace OK
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// exportování obrázku do bufferu TRUECOLORALPHABETA (vrací buffer, NULL=chyba paměti)

BYTE* CPicture::ExportTrueAlphaBeta()
{
// zajištění dekomprimace
	if (!DeComp()) return NULL;

// vytvoření bufferu
	int size = pData->Width * pData->Height;
	BYTE* buf = (BYTE*)MemGet(size * 5);
	if (buf == NULL) return NULL;

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
// importování obrázku z bufferu TRUECOLORALPHABETA (vrací FALSE=chyba paměti)

bool CPicture::ImportTrueAlphaBeta(BYTE* buf, BOOL dith)
{
// zajištění prázdného obrázku
	if (!New()) return false;

// rozměry obrázku
	int width = pData->Width;
	int height = pData->Height;

// ukazatele dat
	BYTE* src = buf;
	BYTE* dst = pData->Data;

// konverze s ditheringem
	if (dith)
	{

// příprava bufferu odchylky pro dithering
		int* odch = (int*)MemGet((3*width + 6 + 10) * sizeof(int));
		if (odch == NULL) return false;
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

// příznak operace OK
	return true;
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
// horizontální filtr (vrací FALSE=chyba paměti)

bool HorizontalFilter(BYTE* oldbuf, int oldwidth, int oldheight, BYTE* newbuf, int newwidth)
{
// příprava ukazatelů podpory
	double xfactor = (double)newwidth/oldwidth;
	double support = FSUPPORT/xfactor;
	if (support < FSUPPORT) support = FSUPPORT;

// buffer konverze bodů
	ContribInfo* cinfo = (ContribInfo*)MemGet(Round(support*2+3)*sizeof(ContribInfo) + 128);
	if (cinfo == NULL) return false;

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

// příznak operace OK
	return true;
}

/////////////////////////////////////////////////////////////////////////
// vertikální filtr (vrací FALSE=chyba paměti)

bool VerticalFilter(BYTE* oldbuf, int oldwidth, int oldheight, BYTE* newbuf, int newheight)
{
// příprava ukazatelů podpory
	double yfactor = (double)newheight/oldheight;
	double support = FSUPPORT/yfactor;
	if (support < FSUPPORT) support = FSUPPORT;

// buffer konverze bodů
	ContribInfo* cinfo = (ContribInfo*)MemGet(Round(support*2+3)*sizeof(ContribInfo) + 128);
	if (cinfo == NULL) return false;

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

// příznak operace OK
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// změna velikosti obrázku (int = interpolovat, dith = ditherovat)
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool CPicture::Zoom(int newwidth, int newheight, BOOL inter, BOOL dith)
{
// úschova starých rozměrů
	int oldwidth = pData->Width;
	int oldheight = pData->Height;

// test, zda bude změna velikosti obrázku
	if ((newwidth == oldwidth) && (newheight == oldheight)) return true;

// zajištění dekomprimace
	if (!DeComp()) return false;

// bude konverze bez interpolace
	if (!inter)
	{

// vytvoření bufferu horizontální konverzní tabulky
		int* konv = (int*)MemGet(newwidth * sizeof(int));
		if (konv == NULL) return false;

// vytvoření nového (prázdného) obrázku
		CPicture pic;
		if (!pic.New(newwidth, newheight))
		{
			MemFree(konv);
			return false;
		}

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

		return true;
	}

// vytvoření bufferu s daty TRUECOLOR
	BYTE* buf = ExportTrueAlphaBeta();
	if (buf == NULL) return false;

// při zvýšení výšky obrázku se konvertuje nejdříve horizontálně
	if (newheight >= oldheight)
	{

// test, zda je potřeba provádět horizontální konverzi
		if (newwidth != oldwidth)
		{

// vytvoření výstupního bufferu pro horizontální konverzi
			BYTE* buf2 = (BYTE*)MemGet(newwidth*oldheight*5);
			if (buf2 == NULL)
			{
				MemFree(buf);
				return false;
			}

// provedení horizontální konverze
			if (!HorizontalFilter(buf, oldwidth, oldheight, buf2, newwidth))
			{
				MemFree(buf);
				MemFree(buf2);
				return false;
			}

// přenesení na vstupní buffer
			MemFree(buf);
			buf = buf2;
		}

// test, zda je potřeba provádět vertikální konverzi
		if (newheight != oldheight)
		{

// vytvoření výstupního bufferu pro vertikální konverzi
			BYTE* buf2 = (BYTE*)MemGet(newheight*newwidth*5);
			if (buf2 == NULL)
			{
				MemFree(buf);
				return false;
			}

// provedení vertikální konverze
			if (!VerticalFilter(buf, newwidth, oldheight, buf2, newheight))
			{
				MemFree(buf);
				MemFree(buf2);
				return false;
			}

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
			if (buf2 == NULL)
			{
				MemFree(buf);
				return false;
			}

// provedení vertikální konverze
			if (!VerticalFilter(buf, oldwidth, oldheight, buf2, newheight))
			{
				MemFree(buf);
				MemFree(buf2);
				return false;
			}

// přenesení na vstupní buffer
			MemFree(buf);
			buf = buf2;
		}

// test, zda je potřeba provádět horizontální konverzi
		if (newwidth != oldwidth)
		{

// vytvoření výstupního bufferu pro horizontální konverzi
			BYTE* buf2 = (BYTE*)MemGet(newwidth*newheight*5);
			if (buf2 == NULL)
			{
				MemFree(buf);
				return false;
			}

// provedení horizontální konverze
			if (!HorizontalFilter(buf, oldwidth, newheight, buf2, newwidth))
			{
				MemFree(buf);
				MemFree(buf2);
				return false;
			}

// přenesení na vstupní buffer
			MemFree(buf);
			buf = buf2;
		}
	}

// vytvoření nového (prázdného) obrázku
	CPicture oldpic(*this);

	if (!New(newwidth, newheight))
	{
		MemFree(buf);
		return false;
	}

// importování obrázku z datového bufferu
	if (!ImportTrueAlphaBeta(buf, dith))
	{
		*this = oldpic;
		MemFree(buf);
		return false;
	}

// zrušení datového bufferu
	MemFree(buf);

// operace OK
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// poskytnutí bodu (s kontrolou platnosti offsetu/indexu), obrázek nesmí být komprimovaný!
// pro neplatný bod vrátí barvu pozadí)

BYTE _fastcall CPicture::Get(const int off) const
{
	ASSERT(pData->Param != PicParamComp);

	if (IsValid(off))				// je offset platný?
	{
		return pData->Data[off];	// bod na daném offsetu
	}
	return BackCol;					// neplatný offset
}

BYTE _fastcall CPicture::Get(const int x, const int y) const
{
	ASSERT(pData->Param != PicParamComp);

	if (IsValid(x, y))				// je index platný?
	{
		return pData->Data[x + y*pData->Width];	// prvek
	}
	return BackCol;					// neplatný offset
}


////////////////////////////////////////////////////////////////////
// nastavení bodu (s kontrolou platnosti offsetu/indexu), obrázek nesmí být komprimovaný!
// před zápisem je nutno zajistit přivlastnění bufferu!

void _fastcall CPicture::Set(const int off, const BYTE data)
{
	ASSERT(pData->Param != PicParamComp);
	ASSERT(pData->Refer == 1);

	if (IsValid(off))				// je offset platný?
	{
		pData->Data[off] = data;
	}
}

void _fastcall CPicture::Set(const int x, const int y, const BYTE data)
{
	ASSERT(pData->Param != PicParamComp);
	ASSERT(pData->Refer == 1);

	if (IsValid(x, y))				// je index platný?
	{
		pData->Data[x + y*pData->Width] = data;
	}
}


/////////////////////////////////////////////////////////////////////////////
// komprese dat obrázku, vrací velikost dat (při chybě paměti vrací <0, data jsou nezměněna)
// komprese se provádí ve společném bufferu pro všechny proměnné!
// (v případě potřeby je nutno zajistit přivlastnění bufferu)
// původní buffer s daty je zrušen
// (velikost komprimovaných dat je udávána bez dvojslova s velikostí, buffer s daty
// je tedy o 4 větší, data začínají na offsetu 4 v bufferu)

int CPicture::Comp() const
{
// test, zda je obrázek již komprimován
	if (pData->Param == PicParamComp)
	{
		return *(long*)pData->Data;
	}

// velikost dat obrázku
	int size = pData->Width * pData->Height;

// vytvoření nového bufferu pro data
	BYTE* newdata = (BYTE*)MemGet(2*size + 200);
	if (newdata == NULL) return -1;

// komprese dat
	BYTE* olddata = pData->Data;
	int newsize = Compress(newdata + 4, olddata, size, pData->Width);
	*(long*)newdata = newsize;

// vytvoření bufferu se správnou velikostí (MemSize() by blok nezmenšil)
	BYTE* newdata2 = (BYTE*)MemGet(newsize + 4);
	if (newdata2 == NULL)
	{
		MemFree(newdata);
		return -1;
	}

// kopie dat do nového bufferu
	MemCopy(newdata2, newdata, newsize+4);
	MemFree(newdata);

// zrušení starého bufferu
	MemFree(olddata);

// nastavení parametrů
	pData->Data = newdata2;
	pData->Param = PicParamComp;

// nová velikost dat
	return newsize;
}


/////////////////////////////////////////////////////////////////////////////
// dekomprimace dat obrázku (jsou-li komprimována), při chybě vrací FALSE
// dekomprese se provádí ve společném bufferu pro všechny proměnné
// (v případě potřeby je nutno zajistit přivlastnění bufferu)
// původní buffer s komprimovanými daty je zrušen

bool CPicture::DeComp() const
{
// test, zda je obrázek komprimován
	if (pData->Param == PicParamComp)
	{

// vytvoření nového bufferu pro data
		int size = pData->Width * pData->Height;
		BYTE* newdata = (BYTE*)MemGet(size);
		if (newdata == NULL) return false;

// dekomprese dat
		BYTE* olddata = pData->Data;
		DeCompress(newdata, olddata + 4, size, pData->Width);

// zrušení starého bufferu
		MemFree(olddata);

// nastavení parametrů
		pData->Data = newdata;
		pData->Param = PicParamNone;
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// kopie nových dat obrázku (rozměry zůstanou nezměněny) - zajistí odpojení dat
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool CPicture::CopyData(BYTE* src)
{
// vytvoření nového bufferu, je-li potřeba
	if (!New()) return false;

// kopie dat do bufferu
	MemCopy(pData->Data, src, pData->Width * pData->Height);
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// kopie komprimovaných nových dat obrázku (rozměry zůstanou nezměněny) - zajistí odpojení dat
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool CPicture::CompCopy(BYTE* src)
{
// úschova parametrů
	int width = pData->Width;
	int height = pData->Height;
	int size = *(long*)src + 4;

// vytvoření nového bufferu
	if (!New(size, 1)) return false;

// nastavení parametrů
	pData->Width = width;
	pData->Height = height;
	pData->Param = PicParamComp;

// kopie dat do bufferu
	MemCopy(pData->Data, src, size);
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// kopie nových dat obrázku s konverzí (rozměry zůstanou nezměněny) - zajistí odpojení dat
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool CPicture::CopyKonvData(BYTE* src)
{
// vytvoření nového bufferu, je-li potřeba
	if (!New()) return false;

// kopie dat do bufferu
	KonvCopy(pData->Data, src, pData->Width * pData->Height);
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// načtení obrázku ze souboru, comp=komprimovat (FALSE=chyba, obsah nezměněn)

bool CPicture::LoadFile(CText jmeno, BOOL comp)
{
// otevření souboru mapovaného do paměti (uzavřen při destrukci!)
	CFileMap file;			// vstupní soubor (BMP nebo JPG)
	CBuf buf;				// buffer pro dekomprimaci JPEG
	BOOL jpeg = FALSE;		// příznak obrázku JPEG

	if (!file.Open(jmeno)) return false;

// velikost souboru bez záhlaví 14 bajtů
	int size = file.Size() - 14;

// identifikační záhlaví
	char* hd = (char*)file.Adr();
	BITMAPINFO* bmp = (BITMAPINFO*)(file.Adr() + 14);
	
// kontrola velikosti souboru a záhlaví
	if ((size < 50) || 
		(hd[0] != 'B') || 
		(hd[1] != 'M') ||
		!TestBMP(bmp))
	{

// načtení souboru JPG
		if (!JPEG::Decomp(&file, &buf)) return false;

// velikost souboru bez záhlaví 14 bajtů
		size = buf.Size() - 14;

// identifikační záhlaví
		hd = (char*)buf.Adr();
		bmp = (BITMAPINFO*)(buf.Adr() + 14);
	
// kontrola velikosti souboru a záhlaví
		if ((size < 50) || 
			(hd[0] != 'B') || 
			(hd[1] != 'M') ||
			!TestBMP(bmp))
		{
			MemFree(buf.Adr());
			return false;
		}

// příznak, že je obrázek JPEG
		jpeg = TRUE;
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
	BITMAPINFO* bmp2 = NULL;

	if (bmp->bmiHeader.biCompression == BI_RLE8)
	{
		int newsize = ((width + 3) & ~3) * height;
		bmp2 = (BITMAPINFO*)MemGet(sizehead + newsize + 10000);
		if (bmp2 == NULL)
		{
			if (jpeg) MemFree(buf.Adr());
			return false;
		}
		MemCopy(bmp2, bmp, sizehead);
		bmp2->bmiHeader.biCompression = BI_RGB;
		DekompRLE8((BYTE*)bmp2+sizehead, newsize, (BYTE*)bmp+sizehead, size);
		bmp = bmp2;
		size = newsize;
	}

// dekomprese bitmapy RLE4
	if (bmp->bmiHeader.biCompression == BI_RLE4)
	{
		int newsize = (((width+1)/2 + 3) & ~3) * height;
		bmp2 = (BITMAPINFO*)MemGet(sizehead + newsize + 10000);
		if (bmp2 == NULL)
		{
			if (jpeg) MemFree(buf.Adr());
			return false;
		}
		MemCopy(bmp2, bmp, sizehead);
		bmp2->bmiHeader.biCompression = BI_RGB;
		DekompRLE4((BYTE*)bmp2+sizehead, newsize, (BYTE*)bmp+sizehead, size);
		bmp = bmp2;
		size = newsize;
	}

// vytvoření nového bufferu
	PICTUREDATA* newdata = NewPictureData(width, height);
	if (newdata == NULL)
	{
		MemFree(bmp2);
		if (jpeg) MemFree(buf.Adr());
		return false;
	}

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
	BYTE* dst = newdata->Data;				// ukládací adresa
	BYTE* src = (BYTE*)bmp + sizehead;		// čtecí adresa
	int srcinc;								// přírustek zdrojové adresy
	int i, j;								// pracovní čítače
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

// uvolnění případného bufferu
	MemFree(bmp2);

// odpojení starých dat
	detach(pData);

// připojení nových dat
	pData = newdata;

// komprimace dat (chyba nevadí)
	if (comp) Comp();

// zrušení bufferu JPEG
	if (jpeg) MemFree(buf.Adr());

// příznak - obrázek načten OK
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// uložení do souboru formátu BMP (komprimovaný obrázek dekomprimuje) (FALSE=chyba)

bool CPicture::SaveFile(CText jmeno) const
{
// dekomprimace dat obrázku
	if (!DeComp()) return false;

// vytvoření souboru
	CFile file;
	file.Name(jmeno);
	if (!file.Create()) return false;

// příprava velikosti záhlaví souboru
	int headsize = sizeof(BITMAPFILEHEADER) + 
					sizeof(BITMAPINFOHEADER) +
					StdColors*sizeof(RGBQUAD);

// příprava bufferu pro obrázek
	BYTE* buf = (BYTE*)MemGet(headsize + (pData->Width+6)*pData->Height*2 + 1000);
	if (buf == NULL)
	{
		file.Close();
		file.Delete();
		return false;
	}
	MemFill(buf, sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER), 0);

// komprese dat
	int size = KompRLE8(buf + headsize, pData->Data, pData->Width, pData->Height);

// naplnění záhlaví popisovače souboru
	BITMAPFILEHEADER* head = (BITMAPFILEHEADER*) buf;
	buf[0] = 'B';								// identifikace souboru
	buf[1] = 'M';
	head->bfSize = headsize + size;				// velikost souboru
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
	BOOL result = file.Write(buf, size + headsize);

// uzavření souboru
	file.Close();

// zrušení bufferu
	MemFree(buf);

// při chybě zrušení souboru
	if (!result)
	{
		file.Delete();
		return false;
	}

// příznak - uloženo OK
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// operátor přiřazení

const CPicture& CPicture::operator= (const CPicture& src)
{
	detach(pData);				// zrušení starých dat
	attach(src.pData);		// přiřazení nových dat
	return *this;
}

const CPicture& CPicture::operator= (PICTUREDATA* src)
{
	detach(pData);				// zrušení starých dat
	attach(src);				// přiřazení nových dat
	return *this;
}


/////////////////////////////////////////////////////////////////////////////
// vygenerování obrázku pro tažení (při chybě vrací NULL)

HIMAGELIST CPicture::GenerDrag(const CText& text)
{
// dekomprimace dat obrázku
	if (!DeComp()) return NULL;

// příprava bufferu textu
	char* textbuf = NULL;				// buffer s textem
	if (text.IsNotEmpty())
	{
#ifdef _UNICODE
		textbuf = (char*)MemGet(text.Length()+1);
		if (textbuf == NULL) return NULL;
		text.WriteNull(textbuf);
#else
		textbuf = text.DataData();
#endif
	}

// příprava rozměrů
	int width = pData->Width;			// šířka obrázku
	int height = pData->Height;			// výška obrázku
	int widtht = 0;						// šířka textové části
	int len = 0;						// délka textu

// zjištění šířky textové části a délky textu (s omezením - při TRUECOLOR nefunguje při velikosti 512*32 bodů)
	if (text.IsNotEmpty())
	{
		widtht = 6 + 16;				// šířka pro úvodní a koncovou mezeru

		for (; ((len < text.Length()) && (widtht < (512-width-16-32))); len++)
		{
			widtht += StdFontsWidth[(BYTE)textbuf[len]] + 1;
		}

		if (height < 16) height = 16;
	}

// celková šířka obrázku s textem
	int widthc = width + widtht;		// celková šířka obrázku s textem
	int widthbyte = (widthc + 3) & ~3;	// délka linky barevné bitmapy

// buffer pro barevnou bitmapu
	int datasize = (widthbyte+8) * height;
	BYTE* data = (BYTE*)MemGet(datasize);
	if (data == NULL)
	{
#ifdef _UNICODE
		MemFree(textbuf);
#endif
		return NULL;
	}

// vymazání bufferu (pokud bude text)
	if (widtht > 0) MemFill(data, datasize, BackCol);

// vytvoření dat barevné bitmapy
	BYTE* dst = data;
	BYTE* src = pData->Data;
	for (int i = pData->Height; i > 0; i--)
	{
		MemCopy(dst, src, width);
		dst += widthbyte;
		src += width;
	}

// dekódování textu
	if (widtht > 0)
	{
		BYTE* dst2 = data + width + 6 + (height - 16)/2*widthbyte;

		for (i = 0; i < len; i++)
		{
			BYTE c = (BYTE)textbuf[i];

			BYTE* dst3 = dst2;
			BYTE* src2 = StdFonts[c].DataData();
			for (int k = 16; k > 0; k--)
			{
				MemCopy(dst3, src2, 16);
				dst3 += widthbyte;
				src2 += 16;
			}
			dst2 += StdFontsWidth[c] + 1;
		}

	}

#ifdef _UNICODE
	MemFree(textbuf);
#endif

// buffer pro mono bitmapu
	int dstinc = ((widthc + 15) & ~0xf) / 8;	// délka linky mono bitmapy
	BYTE* mono = (BYTE*)MemGet(dstinc * height);
	if (mono == NULL)
	{
		MemFree(data);
		return NULL;
	}

// vygenerování mono bitmapy
	dst = mono;
	BYTE maska2 = 0xAA;

	for (i = height-1; i >= 0; i--)
	{
		src = data + widthbyte*i;
		int n = ((width + 15) & ~0xf) / 8;

		for (int j = dstinc - 1; j >= 0; j--)
		{
			BYTE stradac = 0;
			BYTE maska = 0x80;
			for (int k = 8; k > 0; k--)
			{
				if (*src == BackCol) stradac |= maska;
				src++;
				maska >>= 1;
			}

			if (n > 0)
			{
				*dst = (BYTE)(stradac | maska2);	
			}
			else
			{
				*dst = (BYTE)(stradac);
			}
			dst++;
			n--;
		}
		maska2 ^= 0xff;
	}

// vytvoření mono bitmapy (maska)
	HBITMAP bmpMono = ::CreateBitmap(widthc, height, 1, 1, mono);
	ASSERT (bmpMono != NULL);
	if (bmpMono == NULL)
	{
		MemFree(mono);
		MemFree(data);
		return NULL;
	}

// otevření DC displeje
	HDC dc = ::GetDC(0);
	ASSERT(dc != NULL);

// výběr a realizace vlastních palet 
	HPALETTE OldPal = ::SelectPalette(dc, StdPalette, FALSE);
	::RealizePalette(dc);

// příprava záhlaví BMP
	StdBitmapInfo->bmiHeader.biWidth = widthc;
	StdBitmapInfo->bmiHeader.biHeight = height;

// vytvoření barevné bitmapy
	HBITMAP bmp = ::CreateDIBitmap(dc, &(StdBitmapInfo->bmiHeader),
		CBM_INIT, data, StdBitmapInfo, DIB_RGB_COLORS);
	ASSERT(bmp != NULL);

// vytvoření seznamu
	HIMAGELIST hImg = ::ImageList_Create(widthc, height, ILC_COLORDDB | ILC_MASK, 1, 1);
	ASSERT(hImg != NULL);

// přidání bitmapy k seznamu
	int result = ::ImageList_Add(hImg, bmp, bmpMono);
	ASSERT(result != -1);

// zrušení bitmap
	if (bmp != NULL) ::DeleteObject(bmp);
	::DeleteObject(bmpMono);

// uvolnění palet a DC displeje
	if (OldPal != NULL) ::SelectPalette(dc,OldPal,FALSE);
	::ReleaseDC(0,dc);

// zrušení pracovních bufferů s daty
	MemFree(mono);
	MemFree(data);

// kontrola operace
	if (result == -1)
	{
		::ImageList_Destroy(hImg);
		hImg = NULL;
	}

// navrácení seznamu
	return hImg;
}


/***************************************************************************\
*																			*
*								Buffer obrázků								*
*																			*
\***************************************************************************/

////////////////////////////////////////////////////////////////////
// přidání záznamu o vložení položky do UNDO bufferu

template <> bool CBuffer<CPicture>::UndoAddIns(int index)
{
	return Undo.AddPicIns(index);
}



////////////////////////////////////////////////////////////////////
// vytvoření prázdné položky (vrací index položky, <0 = chyba)
// provádí záznam do UNDO bufferu

int _fastcall CBufPic::New(int width, int height)
{
// vytvoření nové položky
	int result = New();
	if (result >= 0)
	{

// inicializace objektu
		if (!m_Data[result].Clear(width, height))
		{
			Del(result);
			if (m_Undo) Undo.DelAll();
			result = -1;
		}
	}

// index nové položky (nebo <0 = chyba)
	return result;
}

////////////////////////////////////////////////////////////////////
// přidání položky (vrací index položky, <0 = chyba paměti)
// provádí záznam do UNDO bufferu

int _fastcall CBufPic::Add(PICTUREDATA* data)
{
	CPicture pic(data);
	int result = Add(pic);		// result musí být oddělen, pro inline by se špatně volal destruktor
	return result;
}
