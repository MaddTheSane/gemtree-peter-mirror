
#include "Main.h"

/***************************************************************************\
*																			*
*								Ikonové proměnné							*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// inicializační prázdná ikona (modifikuje se počet referencí!) (viz MAIN.CPP)

ICONDATA	EmptyIconData		= { 1, PicParamNone, NULL, NULL, 0, NULL};


/////////////////////////////////////////////////////////////////////////////
// konstruktor a destruktor

CIcon::CIcon() 
{ 
	Attach(&EmptyIconData); 
};

CIcon::CIcon(const CIcon& src) 
{ 
	Attach(src.pData); 
};

CIcon::~CIcon() 
{ 
	Detach(); 
};

/////////////////////////////////////////////////////////////////////////////
// statický konstruktor a destruktor

void CIcon::Init()
{ 
	Attach(&EmptyIconData); 
};

void CIcon::Init(ICONDATA* data)
{ 
	Attach(data); 
};

void CIcon::InitNew()
{ 
	NewBuffer(); 
};

void CIcon::Term()
{ 
	Detach(); 
};


/////////////////////////////////////////////////////////////////////////////
// kopie nových dat ikony - zajistí odpojení dat

void CIcon::CopyData(BYTE* src)
{
	ASSERT(src);
	ICONDATA* data = pData;			// adresa starých dat (záhlaví)
	long* refer = &(data->Refer);	// počet referencí

	if (*refer > 1)					// je nějaký jiný majitel?
	{
		NewBuffer();				// vytvoření nového bufferu

// odpojení starých dat - v multithread může nastat i zrušení
		if (LongDecrement(refer))
		{
#ifdef _MT
			if (data->HIcon != NULL)
			{
				::DestroyIcon(data->HIcon);
			}

			if (data->Cursor != NULL)
			{
				::DestroyCursor(data->Cursor);
			}

			MemFree(data->Data);	// případné zrušení dat
			MemFree(data);			// případné zrušení záhlaví
#endif	// _MT
		}
	}
	else
	{
		if (data->HIcon != NULL)
		{
			::DestroyIcon(data->HIcon);
			data->HIcon = NULL;
		}

		if (data->Cursor != NULL)
		{
			::DestroyCursor(data->Cursor);
			data->Cursor = NULL;
		}
	}

	MemCopy(pData->Data, src, ICONSIZE); // kopie obsahu ikony
}


/////////////////////////////////////////////////////////////////////////////
// dekomprimace dat ikony (jsou-li komprimována)

void CIcon::DeComp()
{
	if (pData->Param == PicParamComp)
	{
		BYTE* data = (BYTE*)MemGet(ICONSIZE);
		DeCompress(data, pData->Data + 4, ICONSIZE, ICONWIDTH);
		pData->Data = data;
		pData->Refer -= BigInt;
		pData->Param = PicParamNone;
	}
}



/////////////////////////////////////////////////////////////////////////////
// kopie nových dat ikony s konverzí - zajistí odpojení dat

void CIcon::CopyKonvData(BYTE* src)
{
	ASSERT(src);
	ICONDATA* data = pData;			// adresa starých dat (záhlaví)
	long* refer = &(data->Refer);	// počet referencí

	if (*refer > 1)					// je nějaký jiný majitel?
	{
		NewBuffer();				// vytvoření nového bufferu

// odpojení starých dat - v multithread může nastat i zrušení
		if (LongDecrement(refer))
		{
#ifdef _MT
			if (data->HIcon != NULL)
			{
				::DestroyIcon(data->HIcon);
			}

			if (data->Cursor != NULL)
			{
				::DestroyCursor(data->Cursor);
			}
			MemFree(data->Data);	// případné zrušení dat
			MemFree(data);			// případné zrušení záhlaví
#endif	// _MT
		}
	}
	else
	{
		if (data->HIcon != NULL)
		{
			::DestroyIcon(data->HIcon);
			data->HIcon = NULL;
		}

		if (data->Cursor != NULL)
		{
			::DestroyCursor(data->Cursor);
			data->Cursor = NULL;
		}
	}

	KonvCopy(pData->Data, src, ICONSIZE); // kopie obsahu ikony
}


/////////////////////////////////////////////////////////////////////////////
// kopie ikony do vlastního bufferu před modifikací

void CIcon::CopyWrite()
{
	DeComp();
	ICONDATA* data = pData;			// adresa starých dat (záhlaví)
	long* refer = &(data->Refer);	// počet referencí

	if (*refer > 1)					// je nějaký jiný majitel?
	{
		NewBuffer();				// vytvoření nového bufferu
		MemCopy(pData->Data, data->Data, ICONSIZE);
		pData->Param = data->Param;	// přenesení parametrů

// odpojení starých dat - v multithread může nastat i zrušení
		if (LongDecrement(refer))
		{
#ifdef _MT
			if (data->HIcon != NULL)
			{
				::DestroyIcon(data->HIcon);
			}

			if (data->Cursor != NULL)
			{
				::DestroyCursor(data->Cursor);
			}
			MemFree(data->Data);	// případné zrušení dat
			MemFree(data);			// případné zrušení záhlaví
#endif	// _MT
		}
	}
	else
	{
		if (data->HIcon != NULL)
		{
			::DestroyIcon(data->HIcon);
			data->HIcon = NULL;
		}

		if (data->Cursor != NULL)
		{
			::DestroyCursor(data->Cursor);
			data->Cursor = NULL;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// vyprázdnění ikony (uvolnění dat)

void CIcon::Empty()
{ 
	Detach(); 
	Attach(&EmptyIconData); 
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření nové ikony (připraveno k zápisu, data jsou náhodná)

void CIcon::New()
{
	if (pData->Refer != 1)
	{
		Detach();						// odpojení staré ikony
		NewBuffer();					// vytvoření nového bufferu
	}
}


/////////////////////////////////////////////////////////////////////////////
// načtení ikony ze souboru

#pragma warning ( disable: 4701)		// hlášení - neinicializovaná proměnná
void CIcon::LoadFile()
{
#ifndef _MINI

// úschova offsetu souboru
	int oldoff = FileReadOff;

// načtení záhlaví souboru
	ICONDIR dir;
	dir.Type = 0;
	FileReadBlok((BYTE*)&dir, SIZEOFICONDIR);

// kontrola záhlaví souboru
	int count = dir.Count;
	if ((dir.Reserved != 0) ||
		(dir.Type != 1) ||
		(count <= 0) ||
		(count > 255))
	{
		FileReadOff = oldoff;
		FileError = true;
		return;
	}

// přednastavení neplatné ikony
	int off = -1;
	int width, colors;
	int i;
	int newoff = FileReadOff;

// nalezení ikony s rozměry 32x32
	for (i = 0; i < count; i++)
	{

// načtení adresářové položky
		FileReadBlok((BYTE*)dir.Data, SIZEOFICONDIRENTRY);

// test, zda to je hledaná ikona
		if	(	
				(dir.Data[0].Width == 32) && 
				(dir.Data[0].Height == 32) &&
				(
					(dir.Data[0].Colors == 0) ||
					(
						(dir.Data[0].Colors == 16) && 
						(off < 0)
					)
				)
			)
		{
			colors = dir.Data[0].Colors;
			off = oldoff + dir.Data[0].Offset;
			width = 32;
		}

// posun offsetu konce
		int j = oldoff + dir.Data[0].Offset + dir.Data[0].Size;
		if (j > newoff) newoff = j;
	}

// nalezení ikony s rozměry 16*16
	if (off < 0)
	{
		FileReadOff = oldoff + SIZEOFICONDIR;
		for (i = 0; i < count; i++)
		{
// načtení adresářové položky
			FileReadBlok((BYTE*)dir.Data, SIZEOFICONDIRENTRY);

// test, zda to je hledaná ikona
			if	(	
					(dir.Data[0].Width == 16) && 
					(dir.Data[0].Height == 16) &&
					(
						(dir.Data[0].Colors == 0) ||
						(
							(dir.Data[0].Colors == 16) && 
							(off < 0)
						)
					)
				)
			{
				colors = dir.Data[0].Colors;
				off = oldoff + dir.Data[0].Offset;
				width = 16;
			}
		}
	}

// kontrola, zda byla ikona nalezena
	if (off < 0)
	{
		FileReadOff = oldoff;
		FileError = true;
		return;
	}
	if (colors == 0) colors = 256;

// příprava velikosti záhlaví a dat
	int sizehead = sizeof(BITMAPINFOHEADER) + 256*sizeof(RGBQUAD);
	int sizedata = width*width + width*width/8;
	if (colors == 16)
	{
		sizehead = sizeof(BITMAPINFOHEADER) + 16*sizeof(RGBQUAD);
		sizedata = width*width/2 + width*width/8;
	}

// vytvoření nového bufferu pro data
	Detach();
	NewBuffer();

// načtení dat do pomocného buferu
	FileReadOff = off;
	int size = sizehead + sizedata;
	BYTE* buf = (BYTE*)MemGet(size);
	FileReadBlok(buf, size);
	FileReadOff = newoff;

// vygenerování konverzní tabulky palet
	((BITMAPINFO*)buf)->bmiHeader.biClrUsed = 0;
	if (colors == 16)
	{
		((BITMAPINFO*)buf)->bmiHeader.biBitCount = 4;
	}
	else
	{
		((BITMAPINFO*)buf)->bmiHeader.biBitCount = 8;
	}
	GenKonvPal((BITMAPINFO*)buf);

// příprava zdrojové a cílové adresy
	BYTE* dst = pData->Data;
	BYTE* src = buf + sizehead;

// konverze pro 16 barev
	BYTE col;
	if (colors == 16)
	{

// ikona 32x32
		if (width == 32)
		{
			for (i = 32*32/2; i > 0; i--)
			{
				*dst = KonvPal[*src >> 4];
				dst++;
				*dst = KonvPal[*src & 0xf];
				dst++;
				src++;
			}
		}
		else

// ikona 16x16
		{
			for (i = 16; i > 0; i--)
			{
				for (int j = 8; j > 0; j--)
				{
					col = KonvPal[*src >> 4];
					dst[0] = col;
					dst[32] = col;
					dst++;
					dst[0] = col;
					dst[32] = col;
					dst++;

					col = KonvPal[*src & 0xf];
					dst[0] = col;
					dst[32] = col;
					dst++;
					dst[0] = col;
					dst[32] = col;
					dst++;

					src++;
				}
				dst += 32;
			}
		}
	}
	else

// konverze pro 256 barev
	{

// ikona 32x32
		if (width == 32)
		{
			KonvCopy(dst, src, ICONSIZE);
		}
		else

// ikona 16x16
		{
			for (i = 16; i > 0; i--)
			{
				for (int j = 16; j > 0; j--)
				{
					col = KonvPal[*src];
					dst[0] = col;
					dst[32] = col;
					dst++;
					dst[0] = col;
					dst[32] = col;
					dst++;
					src++;
				}
				dst += 32;
			}
		}
	}

// příprava zdrojové a cílové adresy
	dst = pData->Data;
	src = buf + sizehead + sizedata - width*width/8;

// korekce pozadí ikony pro 32x32
	BYTE maska;

	if (width == 32)
	{
		for (i = ICONSIZE/8; i > 0; i--)
		{
			maska = *src;

			if (maska & 0x80) dst[0] = BackCol;
			if (maska & 0x40) dst[1] = BackCol;
			if (maska & 0x20) dst[2] = BackCol;
			if (maska & 0x10) dst[3] = BackCol;
			if (maska & 0x08) dst[4] = BackCol;
			if (maska & 0x04) dst[5] = BackCol;
			if (maska & 0x02) dst[6] = BackCol;
			if (maska & 0x01) dst[7] = BackCol;

			src++;
			dst += 8;
		}
	}

// uvolnění bufferu
	MemFree(buf);				// uvolnění bufferu

#endif // _MINI
}
#pragma warning ( default: 4701)		// hlášení - neinicializovaná proměnná


/////////////////////////////////////////////////////////////////////////////
// uložení ikony do souboru

void CIcon::SaveFile()
{
#ifndef _MINI

	DeComp();

// příprava velikosti ikony
	int size =	SIZEOFICONDIR +					// záhlaví adresáře
				SIZEOFICONDIRENTRY +			// první adresářová položka
				sizeof(BITMAPINFOHEADER) +		// záhlaví BMP
				256 * sizeof(RGBQUAD) +			// tabulka palet
				ICONSIZE +						// velikost dat ikony
				ICONSIZE/8;						// velikost masky

// buffer pro ikonu
	BYTE* buf = (BYTE*)MemGet(size);

// vymazání bufferu
	MemFill(buf, size);

// vyplnění záhlaví adresáře
	ICONDIR* dir = (ICONDIR*)buf;
	dir->Type = 1;								// typ souboru = ikona
	dir->Count = 1;								// počet položek v adresáři

// vyplnění adresářové položky
	ICONDIRENTRY* entry = dir->Data;
	entry->Width = ICONWIDTH;					// šířka ikony
	entry->Height = ICONHEIGHT;					// výška ikony
	entry->Size = size - SIZEOFICONDIR - SIZEOFICONDIRENTRY;
	entry->Offset = SIZEOFICONDIR + SIZEOFICONDIRENTRY;

// vyplnění záhlaví bitmapy
	BITMAPINFOHEADER* bmp = (BITMAPINFOHEADER*)(buf + SIZEOFICONDIR + SIZEOFICONDIRENTRY);
	bmp->biSize = sizeof(BITMAPINFOHEADER);		// velikost záhlaví
	bmp->biWidth = ICONWIDTH;					// šířka obrázku
	bmp->biHeight = ICONHEIGHT*2;				// výška obrázku a masky
	bmp->biPlanes = 1;							// počet barevných rovin
	bmp->biBitCount = 8;						// počet bitů na bod
	bmp->biSizeImage = ICONSIZE;				// velikost obrázku

// uložení palet (bez barvy pozadí 0 - ta zůstane nastavena na 0, tj černá)
	MemCopy(buf + SIZEOFICONDIR + SIZEOFICONDIRENTRY + sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD), StdBitmapInfo->bmiColors + 1, 255*sizeof(RGBQUAD));

// uložení dat ikony
	BYTE* src = buf + SIZEOFICONDIR + SIZEOFICONDIRENTRY + 
						sizeof(BITMAPINFOHEADER) + 256*sizeof(RGBQUAD);
	MemCopy(src, pData->Data, ICONSIZE);

// vygenerování masky
	BYTE* dst = src + ICONSIZE;
	int i;
	BYTE stradac;

	for (i = ICONSIZE/8; i > 0; i--)
	{
		stradac = 0;

		if (src[0] == BackCol) stradac  = 0x80;
		if (src[1] == BackCol) stradac |= 0x40;
		if (src[2] == BackCol) stradac |= 0x20;
		if (src[3] == BackCol) stradac |= 0x10;
		if (src[4] == BackCol) stradac |= 0x08;
		if (src[5] == BackCol) stradac |= 0x04;
		if (src[6] == BackCol) stradac |= 0x02;
		if (src[7] == BackCol) stradac |= 0x01;

		src += 8;
		*dst = stradac;
		dst++;
	}

// uložení souboru
	FileWriteBlok(buf, size);

// zrušení bufferu
	MemFree(buf);

#endif // _MINI
}


/////////////////////////////////////////////////////////////////////////////
// uložení ikony do souboru formátu ICO (false=chyba)

bool CIcon::SaveFile(CString jmeno)
{
	DeComp();

// otevření souboru
	HANDLE hFile = ::CreateFile(jmeno, GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

// test, zda byl soubor vytvořen
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

// příprava velikosti ikony
	int size =	SIZEOFICONDIR +					// záhlaví adresáře
				SIZEOFICONDIRENTRY +			// první adresářová položka
				sizeof(BITMAPINFOHEADER) +		// záhlaví BMP
				256 * sizeof(RGBQUAD) +			// tabulka palet
				ICONSIZE +						// velikost dat ikony
				ICONSIZE/8;						// velikost masky

// buffer pro ikonu
	BYTE* buf = (BYTE*)MemGet(size);

// vymazání bufferu
	MemFill(buf, size);

// vyplnění záhlaví adresáře
	ICONDIR* dir = (ICONDIR*)buf;
	dir->Type = 1;								// typ souboru = ikona
	dir->Count = 1;								// počet položek v adresáři

// vyplnění adresářové položky
	ICONDIRENTRY* entry = dir->Data;
	entry->Width = ICONWIDTH;					// šířka ikony
	entry->Height = ICONHEIGHT;					// výška ikony
	entry->Size = size - SIZEOFICONDIR - SIZEOFICONDIRENTRY;
	entry->Offset = SIZEOFICONDIR + SIZEOFICONDIRENTRY;

// vyplnění záhlaví bitmapy
	BITMAPINFOHEADER* bmp = (BITMAPINFOHEADER*)(buf + SIZEOFICONDIR + SIZEOFICONDIRENTRY);
	bmp->biSize = sizeof(BITMAPINFOHEADER);		// velikost záhlaví
	bmp->biWidth = ICONWIDTH;					// šířka obrázku
	bmp->biHeight = ICONHEIGHT*2;				// výška obrázku a masky
	bmp->biPlanes = 1;							// počet barevných rovin
	bmp->biBitCount = 8;						// počet bitů na bod
	bmp->biSizeImage = ICONSIZE;				// velikost obrázku

// uložení palet (bez barvy pozadí 0 - ta zůstane nastavena na 0, tj černá)
	MemCopy(buf + SIZEOFICONDIR + SIZEOFICONDIRENTRY + sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD), StdBitmapInfo->bmiColors + 1, 255*sizeof(RGBQUAD));

// uložení dat ikony
	BYTE* src = buf + SIZEOFICONDIR + SIZEOFICONDIRENTRY + 
						sizeof(BITMAPINFOHEADER) + 256*sizeof(RGBQUAD);
	MemCopy(src, pData->Data, ICONSIZE);

// vygenerování masky
	BYTE* dst = src + ICONSIZE;
	int i;
	BYTE stradac;

	for (i = ICONSIZE/8; i > 0; i--)
	{
		stradac = 0;

		if (src[0] == BackCol) stradac  = 0x80;
		if (src[1] == BackCol) stradac |= 0x40;
		if (src[2] == BackCol) stradac |= 0x20;
		if (src[3] == BackCol) stradac |= 0x10;
		if (src[4] == BackCol) stradac |= 0x08;
		if (src[5] == BackCol) stradac |= 0x04;
		if (src[6] == BackCol) stradac |= 0x02;
		if (src[7] == BackCol) stradac |= 0x01;

		src += 8;
		*dst = stradac;
		dst++;
	}

// uložení souboru
	DWORD write;
	BOOL result = ::WriteFile(hFile, buf, size, &write, NULL);

// uzavření souboru
	::CloseHandle(hFile);

// zrušení bufferu
	MemFree(buf);

// při chybě zrušení ikony
	if (!result || (write != (DWORD)size))
	{
		::DeleteFile(jmeno);
		return false;
	}

// příznak - ikona uložena OK
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření ikony WINDOWS

HICON CIcon::HIcon()
{
// není ikona vytvořena?
	if (pData->HIcon == NULL)
	{

// příprava jména přechodného souboru
		CString name;
		name.TempName();

// uložení ikony do souboru
		if (SaveFile(name))
		{

// vytvoření ikony
			pData->HIcon = ::ExtractIcon(hInstance, name, 0);

// chyba
			if ((int)pData->HIcon == 1) pData->HIcon = NULL;

// zrušení přechodného souboru
			::DeleteFile(name);
		}
	}
	return pData->HIcon;
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření kurzoru myši

HCURSOR CIcon::Cursor()
{
// není kurzor vytvořen?
	if (pData->Cursor == NULL)
	{

// dekomprimace ikony
		DeComp();

// vytvoření pracovních bufferů
		BYTE* src = pData->Data;					// zdrojová adresa dat
		BYTE* dstAND = (BYTE*)MemGet(ICONSIZE/8);	// maska AND
		BYTE* dstand = dstAND + (ICONHEIGHT-1)*ICONWIDTH/8;	// adresa poslední linky AND
		BYTE* dstXOR = (BYTE*)MemGet(ICONSIZE/8);	// maska XOR
		BYTE* dstxor = dstXOR + (ICONHEIGHT-1)*ICONWIDTH/8; // adresa poslední linky XOR
		int hotX = ICONWIDTH/2;						// horký bod X
		int hotY = ICONHEIGHT/2;					// horký bod Y

// cyklus přes všechny linky
		for (int i = ICONHEIGHT - 1; i >= 0; i--)
		{

// cyklus přes všechny bajty na lince
			for (int j = ICONWIDTH/8 - 1; j >= 0; j--)
			{

// příprava střadačů a masky
				BYTE stradacAND = 0xff;					// střadač bitů AND
				BYTE stradacXOR = 0;					// střadač bitů XOR
				BYTE maska = 0x80;						// bitová maska bodu

// cyklus přes bity bajtu
				for (int k = 7; k >= 0; k--)
				{

// načtení barvy
					BYTE barva = *src;
					src++;

// není průhledná barva ?
					if (barva != BackCol)
					{

// bílá a černá barva - běžná barva
						if (barva >= WhiteCol)
						{
							stradacAND ^= maska;		// bude AND
							if (barva < (StdColors-ColLev))
							{
								stradacXOR |= maska;	// bude XOR (bílá barva)
							}
						}
						else
						{

// žlutá, zelená a modrá barva - horký bod
							if (barva >= YellowCol - ColLev)
							{
								hotX = ICONWIDTH - j*8 - k;
								hotY = i;
							}

// červená a žlutá barva - inverze
							if (barva <= YellowCol + ColLev)
							{
								stradacXOR |= maska;	// bude XOR
							}
							
							else
							{

// zelená barva - platná data (světlá/tmavá), modrá barva - průhledná
								if (barva < BlueCol - ColLev)
								{
									stradacAND ^= maska;	// bude AND
									if (((barva - ResCols) % ColLev) < (ColLev/2))
									{
										stradacXOR |= maska;	// bude XOR (bílá barva)
									}
								}
							}
						}
					}

// posun masky na další bit
					maska >>= 1;
				}

// uložení bajtu (osmice bodů)
				*dstand = stradacAND;
				dstand++;
				*dstxor = stradacXOR;
				dstxor++;

			}

// posun adresy na další linku
			dstand -= 2*ICONWIDTH/8;
			dstxor -= 2*ICONWIDTH/8;
		}

// vytvoření kurzoru
		pData->Cursor = ::CreateCursor(hInstance, hotX, hotY, ICONWIDTH, ICONHEIGHT, dstAND, dstXOR);
		ASSERT(pData->Cursor != NULL);

// zrušení bufferů
		MemFree(dstAND);
		MemFree(dstXOR);
	}
	return pData->Cursor;
}


/////////////////////////////////////////////////////////////////////////////
// operátor přiřazení

const CIcon& CIcon::operator= (const CIcon& src)
{
	Detach();				// zrušení starých dat
	Attach(src.pData);		// přiřazení nových dat
	return *this;
}

const CIcon& CIcon::operator= (ICONDATA* src)
{
	Detach();				// zrušení starých dat
	Attach(src);			// přiřazení nových dat
	return *this;
}


/***************************************************************************\
*																			*
*								Buffer ikon									*
*																			*
\***************************************************************************/


////////////////////////////////////////////////////////////////////
// konstruktor a destruktor

CBufIcon::CBufIcon()
{
	m_Data = NULL;						// není buffer dat
	m_Num = 0;							// není žádná platná položka
	m_Max = 0;							// není buffer položek
}

CBufIcon::~CBufIcon()
{
	DelAll();							// zrušení všech položek
}


/////////////////////////////////////////////////////////////////////////////
// statický konstruktor a destruktor

void CBufIcon::Init()
{
	m_Data = NULL;						// není buffer dat
	m_Num = 0;							// není žádná platná položka
	m_Max = 0;							// není buffer položek
}

void CBufIcon::Term()
{
	DelAll();							// zrušení všech položek
}


////////////////////////////////////////////////////////////////////
// vytvoření nových dat (odděleno kvůli lepší optimalizaci)

void CBufIcon::NewData()
{
	m_Max *= 2;
	if (m_Max == 0) m_Max = 0x400;
//	m_Max += 0x400;				// 4 KB
	MemBuf(m_Data, m_Max);
}


////////////////////////////////////////////////////////////////////
// zrušení všech položek v bufferu

void CBufIcon::DelAll()
{
	Del(m_Num);					// zrušení položek
	MemBuf(m_Data, 0);			// zrušení bufferu dat
	m_Max = 0;					// není žádná položka v bufferu
}


////////////////////////////////////////////////////////////////////
// poskytnutí položky (s kontrolou platnosti indexu)

const CIcon& _fastcall CBufIcon::Get(const int index) const
{
	if (IsValid(index))			// je index platný?
	{
		return m_Data[index];	// položka na daném indexu
	}
	return EmptyIcon;			// pro neplatný index vrátí prázdnou ikonu
}

////////////////////////////////////////////////////////////////////
// nastavení položky (s kontrolou platnosti indexu)

void _fastcall CBufIcon::Set(const int index, const CIcon& data)
{
	if (IsValid(index))					// je index platný?
	{
		m_Data[index] = data;
	}
}


/////////////////////////////////////////////////////////////////////////////
// vyprázdnění položky (bez jejího zrušení - jen pro uvolnění dat)

void _fastcall CBufIcon::Empty(const int index)
{
	if (IsValid(index))					// je index platný?
	{
		m_Data[index].Empty();
	}
}


////////////////////////////////////////////////////////////////////
// zrušení položek z konce bufferu

void _fastcall CBufIcon::Del(int num)
{
	if (num > m_Num) num = m_Num;

	CIcon* icon = m_Data + m_Num - 1;
	m_Num -= num;

	for (; num > 0; num--)
	{
		icon->Term();
		icon--;
	}
}


/////////////////////////////////////////////////////////////////////////////
// operátor přiřazení

const CBufIcon& _fastcall CBufIcon::operator= (const CBufIcon& src)
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


////////////////////////////////////////////////////////////////////
// vytvoření položky (vrací index položky)

int CBufIcon::New()
{
	int result = NewItem();				// vytvoření nové položky
	m_Data[result].InitNew();			// inicializace položky
	return result;
}


////////////////////////////////////////////////////////////////////
// přidání položky (vrací index položky)

int _fastcall CBufIcon::Add(const CIcon& data)
{
	int result = NewItem();		// vytvoření nové položky
	m_Data[result].Init(data.Data());	// inicializace položky
	return result;
}

int _fastcall CBufIcon::Add(ICONDATA* data)
{
	int result = NewItem();		// vytvoření nové položky
	m_Data[result].Init(data);	// inicializace položky
	return result;
}


////////////////////////////////////////////////////////////////////
// duplikace položky (s kontrolou platnosti indexu, vrací index první položky)

int _fastcall CBufIcon::Dup(const int index)
{
	int result = NewItem();		// vytvoření nové položky

	if (IsValid(index))					// je index platný?
	{
		m_Data[result].Init(m_Data[index].Data());	// kopie položky
	}
	else
	{
		m_Data[result].Init();		// inicializace neplatné položky
	}
	return result;
}

int _fastcall CBufIcon::Dup(const int index, int num)
{
	int result = NewItem();		// vytvoření nové položky

	if (IsValid(index))					// je index platný?
	{
		ICONDATA* data = m_Data[index].Data();

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
