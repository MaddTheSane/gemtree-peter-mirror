
#include "Main.h"

/***************************************************************************\
*																			*
*								Obrázkové proměnné							*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// inicializační prázdný obrázek (modifikuje se počet referencí!)

// Prázdný obrázek musí mít rozměry ICONSIZE kvůli vytvoření nového sprajtu!

PICTUREDATA	EmptyPictureData		= { 1, ICONWIDTH, ICONHEIGHT, NULL };


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


/////////////////////////////////////////////////////////////////////////////
// kopie do vlastního bufferu před modifikací

void CPicture::CopyWrite()
{
	PICTUREDATA* data = pData;		// adresa starých dat
	long* refer = &(data->Refer);	// počet referencí

	if (*refer > 1)					// je nějaký jiný majitel?
	{
		NewBuffer(data->Width, data->Height);	// vytvoření nového bufferu
		MemCopy(pData->Data, data->Data, data->Width * data->Height);

// odpojení starých dat - v multithread může nastat i zrušení
		if (LongDecrement(refer))
		{
#ifdef _MT
			MemFree(data->Data);
			MemFree(data);			// případné zrušení dat
#endif	// _MT
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření nového obrázku (připraveno k zápisu, data jsou náhodná)

void CPicture::New(int width, int height)
{
	Detach();						// odpojení starého obrázku
	NewBuffer(width, height);		// vytvoření nového bufferu
}


/////////////////////////////////////////////////////////////////////////////
// nastavení nových rozměrů obrázku

void CPicture::Resize(int width, int height)
{
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

// barva k vymazání pozadí
		BYTE col = BackCol;

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
					MemFill(dst + j, k, col);
					dst += dstinc;
					src += srcinc;
				}
			}
		}

// vymazání zbylých linek
		i = newdata->Height - olddata->Height;
		if (i > 0)
		{
			MemFill(dst, i*dstinc, col);
		}

// odpojení starého bufferu
		pData = olddata;
		Detach();
		pData = newdata;
	}
}


void CPicture::HalfSize()
{
// staré a nové rozměry
	int oldwidth = Width();
	int oldheight = Height();
	int newwidth = oldwidth/2;
	int newheight = oldheight/2;
	if ((newwidth <= 0) || (newheight <= 0)) return;

// vytvoření nového bufferu
	PICTUREDATA* olddata = pData;
	NewBuffer(newwidth, newheight);
	PICTUREDATA* newdata = pData;

// konverze obrázku
	BYTE* src = olddata->Data;
	BYTE* dst = newdata->Data;

	for (int i = newheight; i > 0; i--)
	{
		if (Dither)
		{
			for (int j = newwidth; j > 0; j--)
			{
				dst[0] = ColAvrgDither(
					src[0],
					src[1],
					src[oldwidth],
					src[oldwidth+1],
					j, i);
				src++;
				src++;
				dst++;
			}
		}
		else
		{
			for (int j = newwidth; j > 0; j--)
			{
				dst[0] = ColAvrg(
					src[0],
					src[1],
					src[oldwidth],
					src[oldwidth+1]);
				src++;
				src++;
				dst++;
			}
		}
		src += 2*(oldwidth - newwidth);
	}

// odpojení starého bufferu
	pData = olddata;
	Detach();
	pData = newdata;
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
#ifdef _MT
			MemFree(data->Data);	// případné zrušení dat
			MemFree(data);			// případné zrušení záhlaví
#endif	// _MT
		}
	}

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
#ifdef _MT
			MemFree(data->Data);	// případné zrušení dat
			MemFree(data);			// případné zrušení záhlaví
#endif	// _MT
		}
	}

	KonvCopy(pData->Data, src, width*height); // kopie obsahu obrázku
}


/////////////////////////////////////////////////////////////////////////////
// načtení obrázku ze souboru s poloviční velikostí (TRUE=operace OK)

BOOL CPicture::LoadFile2(CString jmeno)
{

// otevření souboru
	HANDLE hFile = ::CreateFile(jmeno, GENERIC_READ, 
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, NULL);

// test, zda byl soubor nalezen
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

// načtení záhlaví souboru
	char hd[14];
	DWORD read;
	::ReadFile(hFile, hd, 14, &read, NULL);

// zjištění velikosti souboru (bez záhlaví 14 bajtů)
	int size = (int)::SetFilePointer(hFile, 0, NULL, FILE_END) - 14;
	::SetFilePointer(hFile, 14, NULL, FILE_BEGIN);

// kontrola velikosti souboru a záhlaví
	if ((size < 50) || (size > 20000000) ||	(hd[0] != 'B') || (hd[1] != 'M'))
	{
		::CloseHandle(hFile);		// uzavření souboru
		return FALSE;
	}

// příprava bufferu k načtení souboru
	BITMAPINFO* bmp = (BITMAPINFO*)MemGet(size);

// načtení souboru do bufferu
	::ReadFile(hFile, bmp, size, &read, NULL);

// uzavření souboru
	::CloseHandle(hFile);		// uzavření souboru

// kontrola správnosti operace čtení, kontrola platnosti souboru
	if ((read != (DWORD)size) || !TestBMP(bmp))
	{
		MemFree(bmp);				// uvolnění bufferu
		return FALSE;
	}

// příprava parametrů bitmapy
	int width = bmp->bmiHeader.biWidth;			// šířka
	int width2 = width/2;
	int height = bmp->bmiHeader.biHeight;		// výška
	int height2 = height/2;
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
		MemFree(bmp);
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
		MemFree(bmp);
		bmp = bmp2;
		size = newsize;
	}

// úschova původního obrázku
	CPicture pic(*this);

// vytvoření nového obrázku
	if (bits == 24)
	{
		New(width2, height2);
	}
	else
	{
		New(width, height);
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
	BYTE* dst = pData->Data;				// ukládací adresa
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
		srcinc = ((width+7)/8 + 1) & ~1;	// přírustek zdrojové adresy

		for (i = height; i > 0; i--)		// cyklus přes všechny linky
		{
			for (j = 0; j < width; j++)
			{
				*dst = KonvPal[(src[j/8] >> (7 - (j & 7))) & 1];
				dst++;
			}
			src += srcinc;					// zvýšení zdrojové adresy
		}
		HalfSize();
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
		HalfSize();
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
		HalfSize();
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
		HalfSize();
		break;

// 24 bitů
	case 24:
		srcinc = (3*width + 3) & ~3;

		for (i = height2; i > 0; i--)		// cyklus přes všechny linky
		{
			int* odch0 = odch + 3;			// ukazatel v bufferu odchylek

			for (j = width2; j > 0; j--)		// cyklus přes všechny body
			{
				int			r = 0;									// červená složka
				int			g = 0;									// zelená složka
				int			b = 0;									// modrá složka
				BYTE		n = 0;									// počet platných bodů
				int			s = 0;									// počet stinů

				if ((*(int*)src & 0xffffff) != (BACKCOLOR_BLUE | (BACKCOLOR_GREEN*256) | (BACKCOLOR_RED*256*256)))
				{
				  if ((*(int*)src & 0xffffff) == (SHADCOLOR_BLUE | (SHADCOLOR_GREEN*256) | (SHADCOLOR_RED*256*256)))
				  {
					  s++;
				  }
				  else
				  {
					n++;											// zvýšení čítače bodů
					r = src[2];										// červená složka
					g = src[1];										// zelená složka
					b = src[0];										// modrá složka
				  }
				}

				if ((*(int*)(src + 3) & 0xffffff) != (BACKCOLOR_BLUE | (BACKCOLOR_GREEN*256) | (BACKCOLOR_RED*256*256)))
				{
				  if ((*(int*)(src + 3) & 0xffffff) == (SHADCOLOR_BLUE | (SHADCOLOR_GREEN*256) | (SHADCOLOR_RED*256*256)))
				  {
					  s++;
				  }
				  else
				  {
					n++;											// zvýšení čítače bodů
					r += src[5];									// červená složka
					g += src[4];									// zelená složka
					b += src[3];									// modrá složka
				  }
				}

				if ((*(int*)(src + srcinc) & 0xffffff) != (BACKCOLOR_BLUE | (BACKCOLOR_GREEN*256) | (BACKCOLOR_RED*256*256)))
				{
				  if ((*(int*)(src + srcinc) & 0xffffff) == (SHADCOLOR_BLUE | (SHADCOLOR_GREEN*256) | (SHADCOLOR_RED*256*256)))
				  {
					  s++;
				  }
				  else
				  {
					n++;											// zvýšení čítače bodů
					r += src[srcinc+2];								// červená složka
					g += src[srcinc+1];								// zelená složka
					b += src[srcinc+0];								// modrá složka
				  }
				}

				if ((*(int*)(src + srcinc + 3) & 0xffffff) != (BACKCOLOR_BLUE | (BACKCOLOR_GREEN*256) | (BACKCOLOR_RED*256*256)))
				{
				  if ((*(int*)(src + srcinc + 3) & 0xffffff) == (SHADCOLOR_BLUE | (SHADCOLOR_GREEN*256) | (SHADCOLOR_RED*256*256)))
				  {
					  s++;
				  }
				  else
				  {
					n++;											// zvýšení čítače bodů
					r += src[srcinc+5];								// červená složka
					g += src[srcinc+4];								// zelená složka
					b += src[srcinc+3];								// modrá složka
				  }
				}

				if (s > 2)
				{
					*dst = ShadCol;

					if (Dither)
					{
						*odch0 = 0;
						odch0++;
						*odch0 = 0;
						odch0++;
						*odch0 = 0;
						odch0++;
					}
				}
				else

				if (n < 2)
				{
					*dst = BackCol;

					if (Dither)
					{
						*odch0 = 0;
						odch0++;
						*odch0 = 0;
						odch0++;
						*odch0 = 0;
						odch0++;
					}
				}
				else
				{
					if (Dither)
					{
						r /= n;
						g /= n;
						b /= n;

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
					else
					{
						*dst = PalImport((BYTE)(r/n), (BYTE)(g/n), (BYTE)(b/n));
					}
				}

				dst++;						// zvýšení cílové adresy
				src += 6;
			}
			src += 2*srcinc - 6*width2;
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
		HalfSize();
		break;
	}

// uvolnění bufferu odchylky pro dithering
	MemFree(odch);

// uvolnění bufferu
	MemFree(bmp);

// příznak - obrázek načten OK
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// načtení obrázku ze souboru (TRUE=operace OK)
/*
BOOL CPicture::LoadFile(CString jmeno)
{

// otevření souboru
	HANDLE hFile = ::CreateFile(jmeno, GENERIC_READ, 
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, NULL);

// test, zda byl soubor nalezen
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

// načtení záhlaví souboru
	char hd[14];
	DWORD read;
	::ReadFile(hFile, hd, 14, &read, NULL);

// zjištění velikosti souboru (bez záhlaví 14 bajtů)
	int size = (int)::SetFilePointer(hFile, 0, NULL, FILE_END) - 14;
	::SetFilePointer(hFile, 14, NULL, FILE_BEGIN);

// kontrola velikosti souboru a záhlaví
	if ((size < 50) || (size > 20000000) ||	(hd[0] != 'B') || (hd[1] != 'M'))
	{
		::CloseHandle(hFile);		// uzavření souboru
		return FALSE;
	}

// příprava bufferu k načtení souboru
	BITMAPINFO* bmp = (BITMAPINFO*)MemGet(size);

// načtení souboru do bufferu
	::ReadFile(hFile, bmp, size, &read, NULL);

// uzavření souboru
	::CloseHandle(hFile);		// uzavření souboru

// kontrola správnosti operace čtení, kontrola platnosti souboru
	if ((read != (DWORD)size) || !TestBMP(bmp))
	{
		MemFree(bmp);				// uvolnění bufferu
		return FALSE;
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
		MemFree(bmp);
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
		MemFree(bmp);
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

// příprava parametrů ke konverzi
	BYTE* dst = pData->Data;				// ukládací adresa
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
		srcinc = ((width+7)/8 + 1) & ~1;	// přírustek zdrojové adresy

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
			for (j = width; j > 0; j--)		// cyklus přes všechny body
			{
				if (Dither)
				{
					*dst = PalImportDither(bmp->bmiColors[*src].rgbRed,
							bmp->bmiColors[*src].rgbGreen, bmp->bmiColors[*src].rgbBlue, j, i);
				}
				else
				{
					*dst = PalImport(bmp->bmiColors[*src].rgbRed,
							bmp->bmiColors[*src].rgbGreen, bmp->bmiColors[*src].rgbBlue);
				}
				src++;						// zvýšení zdrojové adresy
				dst++;						// zvýšení cílové adresy
			}
			src += srcinc;					// zvýšení zdrojové adresy
		}
		break;

// 16 bitů
	case 16:
		srcinc = ((2*width + 3) & ~3) - 2*width;

		for (i = height; i > 0; i--)		// cyklus přes všechny linky
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
				if (Dither)
				{
					*dst = PalImportDither(r, g, b, j, i);	// import barvy do vlastních palet
				}
				else
				{
					*dst = PalImport(r, g, b);	// import barvy do vlastních palet
				}
				src++;						// zvýšení zdrojové adresy
				src++;
				dst++;						// zvýšení cílové adresy
			}
			src += srcinc;
		}
		break;

// 24 bitů
	case 24:
		srcinc = ((3*width + 3) & ~3) - 3*width;

		for (i = height; i > 0; i--)		// cyklus přes všechny linky
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
					if (Dither)
					{
						*dst = PalImportDither(r, g, b, j, i);	// import barvy do vlastních palet
					}
					else
					{
						*dst = PalImport(r, g, b);	// import barvy do vlastních palet
					}
				  }
				}
				dst++;						// zvýšení cílové adresy
			}
			src += srcinc;
		}
		break;

// 32 bitů
	case 32:
		for (i = height; i > 0; i--)		// cyklus přes všechny linky
		{
			for (j = width; j > 0; j--)		// cyklus přes všechny body
			{
				if (*(int*)src == (BACKCOLOR_BLUE | (BACKCOLOR_GREEN*256) | (BACKCOLOR_RED*256*256)))
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
					if (Dither)
					{
						*dst = PalImportDither(r, g, b, j, i);	// import barvy do vlastních palet
					}
					else
					{
						*dst = PalImport(r, g, b);	// import barvy do vlastních palet
					}
				  }
				}
				dst++;						// zvýšení cílové adresy
			}
		}
		break;
	}

// uvolnění bufferu
	MemFree(bmp);

// příznak - obrázek načten OK
	return TRUE;
}
*/

/////////////////////////////////////////////////////////////////////////////
// uložení do souboru formátu BMP (FALSE=chyba)

BOOL CPicture::SaveFile(CString jmeno) const
{
// otevření souboru
	HANDLE hFile = ::CreateFile(jmeno, GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

// test, zda byl soubor vytvořen
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

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
	bmp->biXPelsPerMeter = 0;					// horizontální rozlišení
	bmp->biYPelsPerMeter = 0;					// vertikální rozlišení
	bmp->biClrUsed = StdColors;					// počet použitých palet
	bmp->biClrImportant = StdColors;			// počet důležitých palet

// přenesení palet
	MemCopy(buf + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER),
				StdBitmapInfo->bmiColors, StdColors*sizeof(RGBQUAD));

// uložení souboru
	size += headsize;
	DWORD write;
	BOOL result = ::WriteFile(hFile, buf, size, &write, NULL);

// uzavření souboru
	::CloseHandle(hFile);

// zrušení bufferu
	MemFree(buf);

// při chybě zrušení souboru
	if (!result || (write != (DWORD)size))
	{
		::DeleteFile(jmeno);
		return FALSE;
	}

// příznak - uloženo OK
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// operátor přiřazení

const CPicture& CPicture::operator= (const CPicture& src)
{
	Detach();				// zrušení starých dat
	Attach(src.pData);		// přiřazení nových dat
	return *this;
}


/***************************************************************************\
*																			*
*								Buffer obrázků								*
*																			*
\***************************************************************************/


////////////////////////////////////////////////////////////////////
// vytvoření nových dat

#define NEWDATANUM 256				// počet nově vytvořených položek (1 KB)

void CBufPic::NewData()
{
	int i = NEWDATANUM;			// počet nových položek
	int next = m_Max;			// příští položka - 1
	m_Max = next + i;			// zvýšení počtu položek (o 1 KB)

	MemBuf(m_Data, m_Max);		// zvýšení velikosti bufferu

	MemBuf(m_Valid, m_Max);		// zvýšení velikosti bufferu platnosti
	MemFill(m_Valid + next, NEWDATANUM, FALSE); // nastavení příznaků na neplatné položky

	CPicture* data = m_Data + next - 1; // ukazatel dat - 1
	for (; i > 0; i--)
	{
		data++;					// zvýšení ukazatele položek
		next++;					// zvýšení indexu příští položky
		*(int*)data = next;		// odkaz na příští položku
	}
	*(int*)data = m_Next;		// navázání na další položku
	m_Next = m_Max-NEWDATANUM;	// odkaz na první novou položku
};


////////////////////////////////////////////////////////////////////
// konstruktor a destruktor

CBufPic::CBufPic()
{
	m_Data = NULL;			// není buffer dat
	m_Valid = NULL;			// není buffer platnosti
	m_Num = 0;				// není žádná platná položka
	m_Max = 0;				// není buffer položek
	m_Next = -1;			// přiští volná položka (-1=není)
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
	m_Valid = NULL;			// není buffer platnosti
	m_Num = 0;				// není žádná platná položka
	m_Max = 0;				// není buffer položek
	m_Next = -1;			// přiští volná položka (-1=není)
}

void CBufPic::Term()
{
	DelAll();				// zrušení všech položek
}

////////////////////////////////////////////////////////////////////
// zrušení všech položek v bufferu (ukládání začne opět po řadě)

void CBufPic::DelAll()
{
// zrušení položek
	for (int i = m_Max-1; i >= 0; i--)
	{
		Del(i);
	}

// zrušení bufferu
	MemBuf(m_Data, 0);			// zrušení bufferu dat
	MemBuf(m_Valid, 0);			// zrušení bufferu platnosti
	m_Num = 0;					// není žádná platná položka
	m_Max = 0;					// není žádná položka v bufferu
	m_Next = -1;				// není příští položka
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

////////////////////////////////////////////////////////////////////
// zrušení položky (s kontrolou platnosti indexu)

void _fastcall CBufPic::Del(const int index)
{
	if (IsValid(index))						// je index platný?
	{
		m_Data[index].Term();				// ukončení objektu
		*(int*)(m_Data + index) = m_Next;	// příští volná položka
		m_Valid[index] = FALSE;				// zrušení příznaku platnosti
		m_Num--;							// snížení čítače platných položek
		m_Next = index;						// odkaz na tuto položku
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
// duplikace položky (s kontrolou platnosti indexu, vrací index položky)

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


/////////////////////////////////////////////////////////////////////////////
// operátor přiřazení

const CBufPic& CBufPic::operator= (const CBufPic& src)
{
	DelAll();					// zrušení starých dat
	int index = 0;				// index načítané položky
	int i = src.Max();			// velikost zdrojového bufferu

	for (; i > 0; i--)			// pro všechny položky v bufferu
	{
		if (src.m_Valid[index])	// je to platná položka?
		{
			Add(src[index]);	// kopie položky
		}
		index++;				// inkrementace čtecího indexu
	}
	ASSERT(m_Num == src.Num());
	return *this;
}

