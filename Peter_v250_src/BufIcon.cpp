
#include "Main.h"

/***************************************************************************\
*																			*
*								Ikonové proměnné							*
*																			*
\***************************************************************************/

////////////////////////////////////////////////////////////////////
// Pozn.: linky souboru BMP pro 256 barev jsou zarovnávány na 4 bajty
//        linky souboru BMP pro 2 barvy (maska) jsou zarovnány na 2 bajty
////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// inicializační prázdná ikona (modifikuje se počet referencí!) (viz MAIN.CPP)

ICONDATA	EmptyIconData		= { 1, PicParamBack, ICONSIZE, NULL };
const CIcon		EmptyIcon;			// prázdná ikona


/////////////////////////////////////////////////////////////////////////////
// statická inicializace obsluhy ikon (při chybě paměti vrací FALSE)

bool InitIcon()
{
	EmptyIconData.Data = (BYTE*)MemGet(ICONSIZE);
	if (EmptyIconData.Data == NULL) return false;
	MemFill(EmptyIconData.Data, ICONSIZE, BackCol);
	return true;
}


////////////////////////////////////////////////////////////////////
// vytvoření dat ikony (při chybě paměti vrací NULL)

ICONDATA* _fastcall NewIconData(int size)
{
	ASSERT(size > 0);

// vytvoření záhlaví ikony
	ICONDATA* data = (ICONDATA*)MemGet(SIZEOFICONDATA); // vytvoření záhlaví
	if (data != NULL)
	{

// nastavení dat ikony
		data->Refer = 1;					// počet referencí
		data->Param = PicParamNone;			// parametry (obsah neznámý)
		data->Size = size;					// velikost dat ikony

// vytvoření bufferu dat ikony
		BYTE* datadata = (BYTE*)MemGet(size);	// vytvoření bufferu dat ikony
		data->Data = datadata;				// adresa dat

// při chybě paměti zrušení záhlaví ikony
		if (datadata == NULL)
		{
			MemFree(data);
			data = NULL;
		}
	}
	return data;
}

/////////////////////////////////////////////////////////////////////////////
// zrušení dat ikony (odděleno kvůli lepší optimalizaci)

void _fastcall DelIconData(ICONDATA* data)
{
	ASSERT(data != NULL);
	ASSERT(data->Data != NULL);

	MemFree(data->Data);		// zrušení dat ikony
	MemFree(data);				// zrušení záhlaví ikony
}


/////////////////////////////////////////////////////////////////////////////
// konstruktor a destruktor

CIcon::CIcon() 
{ 
	attach(&EmptyIconData); 
};

CIcon::CIcon(const CIcon& src) 
{ 
	attach(src.pData); 
};

CIcon::CIcon(ICONDATA* data)
{ 
	attach(data); 
};

CIcon::~CIcon() 
{ 
	detach(pData); 
};

/////////////////////////////////////////////////////////////////////////////
// statický konstruktor a destruktor

void CIcon::Init()
{ 
	attach(&EmptyIconData); 
};

void _fastcall CIcon::Init(const CIcon& src) 
{ 
	attach(src.pData); 
};

void _fastcall CIcon::Init(ICONDATA* data)
{ 
	attach(data); 
};

bool _fastcall CIcon::Init(int size)	// při chybě paměti vrací FALSE, ikona není vytvořena
{ 
	pData = NewIconData(size);
	return (pData != NULL);
};

void CIcon::Term()
{ 
	detach(pData); 
};

/////////////////////////////////////////////////////////////////////////////
// vyprázdnění ikony (uvolnění dat) - připojí se standardní prázdná ikona ICONSIZE

void CIcon::Empty()
{ 
	Term();
	Init();
}


////////////////////////////////////////////////////////////////////
// vymazání obsahu ikony (naplnění průhlednou barvou), zajistí přivlastnění (a dekomprimaci) bufferu,
// při chybě paměti vrací FALSE, původní obsah nezměněn

bool CIcon::Clear()
{
// vytvoření nového bufferu, je-li potřeba
	if (!New()) return false;

// vymazání bufferu
	MemFill(pData->Data, pData->Size, BackCol);
	
// nastavení parametrů na pozadí	
	pData->Param = PicParamBack;
	return true;
}


////////////////////////////////////////////////////////////////////
// vymazání obsahu ikony s nastavením velikosti (naplnění průhlednou barvou),
// zajistí přivlastnění (a dekomprimaci) bufferu, při chybě paměti vrací FALSE, původní obsah nezměněn

bool _fastcall CIcon::Clear(int size)
{
// vytvoření nového bufferu, je-li potřeba
	if (!New(size)) return false;

// vymazání bufferu
	MemFill(pData->Data, size, BackCol);
	
// nastavení parametrů na pozadí	
	pData->Param = PicParamBack;
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// kopie ikony do vlastního bufferu před modifikací (komprimovaná data zůstanou komprimovaná)
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool CIcon::CopyWrite()
{
// úschova ukazatelů
	ICONDATA* olddata = pData;		// adresa starých dat
	long* refer = &(olddata->Refer);// počet referencí

// test, zda je nutné přivlastnění
	if (*refer > 1)					// je nějaký jiný majitel?
	{

// příprava velikosti bufferu s daty
		int size = olddata->Size;
		if (olddata->Param == PicParamComp)
		{
			size = *(long*)(olddata->Data) + 4;
		}

// vytvoření nového bufferu
		ICONDATA* newdata = NewIconData(size);
		if (newdata == NULL) return false;

// přenesení dat
		MemCopy(newdata->Data, olddata->Data, size);
		newdata->Param = olddata->Param;	// přenesení parametrů
		newdata->Size = olddata->Size; // velikost ikony

// odpojení starých dat
		detach(olddata);

// připojení nových dat
		pData = newdata;
	}

// operace OK
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření nové ikony se stejnou velikostí (připraveno k zápisu, data jsou náhodná)
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool CIcon::New()
{
// ukazatel na stará data
	ICONDATA* olddata = pData;			// adresa starých dat

// test, zda je nutné vytvoření nového bufferu
	if ((olddata->Refer > 1) ||				// na buffer je více referencí
		(olddata->Param == PicParamComp))	// data jsou komprimovaná
	{

// vytvoření nového bufferu
		ICONDATA* newdata = NewIconData(pData->Size);
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
// vytvoření nové ikony (připraveno k zápisu, data jsou náhodná)
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool _fastcall CIcon::New(int size)
{
// ukazatel na stará data
	ICONDATA* olddata = pData;			// adresa starých dat

// test, zda je nutné vytvoření nového bufferu
	if ((olddata->Refer > 1) ||				// na buffer je více referencí
		(olddata->Size != size) ||			// nesouhlasí velikost
		(olddata->Param == PicParamComp))	// data jsou komprimovaná
	{

// vytvoření nového bufferu
		ICONDATA* newdata = NewIconData(size);
		if (newdata == NULL) return false;

// odpojení starých dat
		detach(olddata);

// připojení nových dat
		pData = newdata;
	}

// operace OK
	return true;
}


////////////////////////////////////////////////////////////////////
// poskytnutí bodu (s kontrolou platnosti offsetu) - ikona nesmí být komprimovaná!
// pro neplatný offset vrací barvu pozadí

BYTE _fastcall CIcon::Get(const int off) const
{
	ASSERT(pData->Param != PicParamComp);

	if (IsValid(off))				// je offset platný?
	{
		return pData->Data[off];	// bod na daném offsetu
	}
	return BackCol;					// pro neplatný offset barva pozadí
}


////////////////////////////////////////////////////////////////////
// nastavení bodu (s kontrolou platnosti offsetu) - ikona nesmí být komprimovaná!
// před zápisem je nutno zajistit přivlastnění bufferu!

void _fastcall CIcon::Set(const int off, const BYTE data)
{
	ASSERT(pData->Param != PicParamComp);
	ASSERT(pData->Refer == 1);

	if (IsValid(off))				// je offset platný?
	{
		pData->Data[off] = data;
	}
}


/////////////////////////////////////////////////////////////////////////////
// komprese dat ikony 32*32, vrací velikost dat (při chybě paměti vrací <0, data jsou nezměněna)
// komprese se provádí ve společném bufferu pro všechny proměnné!
// (v případě potřeby je nutno zajistit přivlastnění bufferu)
// původní buffer s daty je zrušen
// (velikost komprimovaných dat je udávána bez dvojslova s velikostí, buffer s daty
// je tedy o 4 větší, data začínají na offsetu 4 v bufferu)

int CIcon::Comp() const
{
// test, zda je ikona již komprimována
	if (pData->Param == PicParamComp)
	{
		return *(long*)pData->Data;
	}

// kontrola velikosti ikony
	ASSERT(pData->Size == ICONSIZE);
	if (pData->Size != ICONSIZE) return -1;

// vytvoření nového bufferu pro data
	BYTE* newdata = (BYTE*)MemGet(ICONSIZE*2);
	if (newdata == NULL) return -1;

// komprese dat
	BYTE* olddata = pData->Data;
	int newsize = Compress(newdata + 4, olddata, ICONSIZE, ICONWIDTH);
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
// dekomprimace dat ikony 32*32 (jsou-li komprimována), při chybě vrací FALSE, data jsou nezměněna
// dekomprese se provádí ve společném bufferu pro všechny proměnné
// (v případě potřeby je nutno zajistit přivlastnění bufferu)
// původní buffer s komprimovanými daty je zrušen

bool CIcon::DeComp() const
{
	// havarie
// test, zda je ikona komprimována
	if (pData->Param == PicParamComp)
	{

// kontrola velikosti ikony
		ASSERT(pData->Size == ICONSIZE);
		if (pData->Size != ICONSIZE) return false;

// vytvoření nového bufferu pro data
		BYTE* newdata = (BYTE*)MemGet(ICONSIZE);
		if (newdata == NULL) return false;

// dekomprese dat
		BYTE* olddata = pData->Data;
		DeCompress(newdata, olddata + 4, ICONSIZE, ICONWIDTH);

// zrušení starého bufferu
		MemFree(olddata);

// nastavení parametrů
		pData->Data = newdata;
		pData->Param = PicParamNone;
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// kopie nových dat ikony (rozměry zůstanou nezměněny) - zajistí přivlastnění (a dekomprimaci) bufferu,
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool CIcon::CopyData(BYTE* src)
{
// vytvoření nového bufferu, je-li potřeba
	if (!New()) return false;

// kopie dat do bufferu
	MemCopy(pData->Data, src, pData->Size);
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// kopie komprimovaných nových dat ikony (rozměry zůstanou nezměněny) - zajistí odpojení dat
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool CIcon::CompCopy(BYTE* src)
{
// úschova parametrů
	int datasize = pData->Size;
	int size = *(long*)src + 4;

// vytvoření nového bufferu
	if (!New(size)) return false;

// nastavení parametrů
	pData->Size = datasize;
	pData->Param = PicParamComp;

// kopie dat do bufferu
	MemCopy(pData->Data, src, size);
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// kopie nových dat ikony s konverzí (rozměry zůstanou nezměněny) - zajistí odpojení dat
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool CIcon::CopyKonvData(BYTE* src)
{
// vytvoření nového bufferu, je-li potřeba
	if (!New()) return false;

// kopie dat do bufferu
	KonvCopy(pData->Data, src, pData->Size);
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// extrakt ikony ze souboru formátu PET (při chybě vrátí FALSE, obsah nezměněn)

bool CIcon::Extract(CText jmeno)
{
// otevření souboru mapovaného do paměti (uzavřen při destrukci!)
	CFileMap file;
	if (!file.Open(jmeno)) return false;

// nalezení implicitní ikony
	BITMAPINFO* bmp = file.NTIcon();
	if (bmp == NULL) return false;

// vygenerování palet pro konverzi
	GenKonvPal(bmp);

// vytvoření nového bufferu
	if (!New(ICONSIZE)) return false;

// kopie dat ikony
	KonvCopy(pData->Data, (BYTE*)(bmp->bmiColors + 256), ICONSIZE);

// korekce pozadí ikony
	BYTE* src = (BYTE*)(bmp->bmiColors + 256) + ICONSIZE;
	BYTE* dst = pData->Data;

	for (int i = ICONSIZE/8; i > 0; i--)
	{
		BYTE maska = *src;

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

// příznak operace OK
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// načtení ikony ze souboru formátu ICO (FALSE=chyba, obsah nezměněn)

bool CIcon::LoadFile(CText jmeno)
{
// otevření souboru mapovaného do paměti (uzavřen při destrukci!)
	CFileMap file;
	if (!file.Open(jmeno)) return false;

// kontrola velikosti souboru
	if (file.Size() < 200) return false;

// příprava ukazatelů na ikonu
	ICONDIR* dir = (ICONDIR*)file.Adr();

// počet ikon v souboru
	int num = dir->Count;

// kontrola platnosti ikony
	if ((dir->Type != 1) ||
		(num < 1) ||
		(num > 256) ||
		((int)(num*SIZEOFICONDIRENTRY + SIZEOFICONDIR + 100) > file.Size()))
	{
		return false;
	}

// přednastavení neplatné ikony
	int index = -1;
	int i, j;

// nalezení ikony s rozměry 32x32
	ICONDIRENTRY* entry = dir->Data;
	for (i = 0; i < num; i++)
	{
		if	(	
				(entry->Width == 32) && 
				(entry->Height == 32) &&
				(
					(entry->Colors == 0) ||
					(
						(entry->Colors == 16) && 
						(index < 0)
					)
				)
			)
		{
			index = i;
		}
		entry++;
	}

// nalezení ikony s rozměry 16*16
	entry = dir->Data;
	if (index < 0)
	{
		for (i = 0; i < num; i++)
		{
			if	(	
					(entry->Width == 16) && 
					(entry->Height == 16) &&
					(
						(entry->Colors == 0) ||
						(
							(entry->Colors == 16) && 
							(index < 0)
						)
					)
				)
			{
				index = i;
			}
			entry++;
		}
	}

// kontrola, zda byla ikona nalezena
	if (index < 0) return false;

// příprava parametrů ikony
	entry = dir->Data + index;
	int width = entry->Width;
	int height = entry->Height;
	int colors = entry->Colors;
	if (colors == 0) colors = 256;
	
// příprava velikosti záhlaví a dat
	int sizehead = sizeof(BITMAPINFOHEADER) + 256*sizeof(RGBQUAD);
	int sizedata = width*height + width*height/8;
	if (colors == 16)
	{
		sizehead = sizeof(BITMAPINFOHEADER) + 16*sizeof(RGBQUAD);
		sizedata = width*height/2 + width*height/8;
	}

// kontrola adresy ikony
	if (file.IsNotValid(file.Adr() + entry->Offset, sizehead + sizedata))
	{
		return false;
	}

// vygenerování konverzní tabulky palet
	BITMAPINFO* bmp = (BITMAPINFO*)MemGet(sizehead);
	if (bmp == NULL) return false;

	MemCopy(bmp, file.Adr() + entry->Offset, sizehead);

	bmp->bmiHeader.biClrUsed = 0;

	if (colors == 16)
	{
		bmp->bmiHeader.biBitCount = 4;
	}
	else
	{
		bmp->bmiHeader.biBitCount = 8;
	}

	GenKonvPal(bmp);
	MemFree(bmp);

// vytvoření nového bufferu pro data
	if (!New(ICONSIZE)) return false;

// příprava zdrojové a cílové adresy
	BYTE* dst = pData->Data;
	BYTE* src = file.Adr() + entry->Offset + sizehead;

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
				for (j = 8; j > 0; j--)
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
				for (j = 16; j > 0; j--)
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
	src = file.Adr() + entry->Offset + sizehead + sizedata - width*height/8;

// korekce pozadí ikony pro 32x32 (pro ikony 16x16 to můžeme ignorovat)
	if (width == 32)
	{
		for (i = ICONSIZE/8; i > 0; i--)
		{
			BYTE maska = *src;

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

// komprese (chyba se ignoruje)
	Comp();

// příznak - ikona načtena OK
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// uložení ikony do souboru formátu ICO (FALSE=chyba)

bool CIcon::SaveFile(CText jmeno) const
{
// kontrola velikosti ikony (musí být 32x32)
	ASSERT(pData->Size == ICONSIZE);
	if (pData->Size != ICONSIZE) return false;

// dekomprimace dat ikony
	if (!DeComp()) return false;

// vytvoření souboru
	CFile file;
	file.Name(jmeno);
	if (!file.Create()) return false;

// příprava velikosti ikony
	int size =	SIZEOFICONDIR +					// záhlaví adresáře
				SIZEOFICONDIRENTRY +			// první adresářová položka
				sizeof(BITMAPINFOHEADER) +		// záhlaví BMP
				256 * sizeof(RGBQUAD) +			// tabulka palet
				ICONSIZE +						// velikost dat ikony
				ICONSIZE/8;						// velikost masky

// buffer pro ikonu
	BYTE* buf = (BYTE*)MemGet(size);
	if (buf == NULL)
	{
		file.Close();
		file.Delete();
		return false;
	}

// vymazání bufferu
	MemFill(buf, size, 0);

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

	for (int i = ICONSIZE/8; i > 0; i--)
	{
		BYTE stradac = 0;

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
	BOOL result = file.Write(buf, size);

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

// příznak operace OK
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// operátor přiřazení

const CIcon& CIcon::operator= (const CIcon& src)
{
	detach(pData);			// zrušení starých dat
	attach(src.pData);		// přiřazení nových dat
	return *this;
}

const CIcon& CIcon::operator= (ICONDATA* src)
{
	detach(pData);			// zrušení starých dat
	attach(src);			// přiřazení nových dat
	return *this;
}


/***************************************************************************\
*																			*
*								Buffer ikon									*
*																			*
\***************************************************************************/

////////////////////////////////////////////////////////////////////
// přidání záznamu o vložení položky do UNDO bufferu

template <> bool CBuffer<CIcon>::UndoAddIns(int index)
{
	return Undo.AddIconIns(index);
}

////////////////////////////////////////////////////////////////////
// konstruktor a destruktor

CBufIcon::CBufIcon()
{
	m_Width = ICONWIDTH;				// šířka jedné ikony
	m_Height = ICONHEIGHT;				// výška jedné ikony
	m_WidthByte = ICONWIDTH;			// délka jedné linky v bajtech
	m_IconSize = ICONSIZE;				// velikost ikony v bajtech
}

/////////////////////////////////////////////////////////////////////////////
// statický konstruktor a destruktor

void CBufIcon::Init()
{
	CBuffer<CIcon>::Init();
	m_Width = ICONWIDTH;				// šířka jedné ikony
	m_Height = ICONHEIGHT;				// výška jedné ikony
	m_WidthByte = ICONWIDTH;			// délka jedné linky v bajtech
	m_IconSize = ICONSIZE;				// velikost ikony v bajtech
}

////////////////////////////////////////////////////////////////////
// nastavení velikosti ikon (případné existující ikony zruší!)

void _fastcall CBufIcon::IconSize(const int width, const int height)
{
	DelAll();							// zrušení existujících ikon

	m_Width = width;					// šířka jedné ikony
	m_Height = height;					// výška jedné ikony
	m_WidthByte = (width + 3) & ~3;		// délka linky jedné ikony
	m_IconSize = m_WidthByte * height;	// velikost ikony v bajtech

	m_EmptyItem.Clear(m_IconSize);		// nová data prázdné ikony
}

////////////////////////////////////////////////////////////////////
// přidání položky (vrací index položky, <0 = chyba paměti)
// provádí záznam do UNDO bufferu

int _fastcall CBufIcon::Add(ICONDATA* data)
{
	CIcon icon(data);
	int result = Add(icon);		// result musí být oddělen, pro inline by se špatně volal destruktor
	return result;
}

////////////////////////////////////////////////////////////////////
// načtení ikon z resource (vynulovat buffer, aby bylo ukládáno po řadě!), vrací TRUE=operace OK

bool CBufIcon::Load(const int nID, int num)
{
// nalezení resource bitmapy
	CResource res;
	if (!res.Open(nID, RT_BITMAP)) { ASSERTERROR; return false; }

// adresa bitmapy
	BITMAPINFO* bmp = (BITMAPINFO*)res.Adr();
	ASSERT(bmp->bmiHeader.biBitCount == 8);
	int colors = bmp->bmiHeader.biClrUsed;
	if (colors == 0) colors = 256;
	ASSERT((DWORD)colors <= 256);

// příprava konverzní tabulky palet
	GenKonvPal(bmp);

// příprava počtu ikon horizontálně
	int numX = bmp->bmiHeader.biWidth/m_Width;
	ASSERT((bmp->bmiHeader.biWidth % m_Width) == 0);

// příprava přírustku zdrojové adresy mezi linkami
	int srcinc = (bmp->bmiHeader.biWidth + 3) & ~3;

// dekomprese bitmapy
	BITMAPINFO*	bmp2 = NULL; // pomocný buffer pro dekompresi bitmapy
	if (bmp->bmiHeader.biCompression == BI_RLE8)
	{
		int sizehead = sizeof(BITMAPINFO) + (colors-1)*sizeof(RGBQUAD);
		bmp2 = (BITMAPINFO*)MemGet(sizehead + srcinc*bmp->bmiHeader.biHeight);
		if (bmp2 == NULL) return false;
		MemCopy(bmp2, bmp, sizehead);
		DekompRLE8((BYTE*)(bmp2->bmiColors + colors), srcinc*bmp->bmiHeader.biHeight, 
					(BYTE*)(bmp->bmiColors + colors), res.Size() - sizehead); 
		bmp = bmp2;
	}

// příprava zdrojové adresy dat (za pravým horním rohem)
	BYTE* src = (BYTE*)(bmp->bmiColors + colors) + srcinc*bmp->bmiHeader.biHeight + m_Width*numX;

// cyklus načtení ikon
	int citX = 1;
	for (; num > 0; num--)
	{

// posun adresy na další řádek ikon
		if (--citX == 0)
		{
			src -=	srcinc*m_Height + m_Width*numX;
			citX = numX;
		}

// vytvoření nové položky ikony
		int item = New();
		if (item < 0)
		{
			MemFree(bmp2);
			return false;
		}

// zajištění přivlastnění bufferu (bez uchování původních dat)
		if (!m_Data[item].New())
		{
			Del(item);
			MemFree(bmp2);
			return false;
		}

// přenos dat ikony
		BYTE* dst = m_Data[item].DataData();
		for (int height = m_Height; height > 0; height--)
		{
			KonvCopy(dst, src, m_Width);
			dst += m_WidthByte;
			src += srcinc;
		}

		if ((m_Height == ICONHEIGHT) && (m_Width == ICONWIDTH))
		{
			m_Data[item].Comp();
		}

// zvýšení adresy ikony
		src += m_Width - m_Height*srcinc;
	}

// případné zrušení bufferu
	MemFree(bmp2);

	return true;
}


////////////////////////////////////////////////////////////////////
// vygenerování dat bitmapy (vrací ukazatel na buffer s rezervou 8 bajtů na konci, při chybě vrací NULL)

BYTE* CBufIcon::GenerBitmap(SMALLICON smallicon)
{
// příprava ukazatelů pro normální ikonu
	int	widthbyte = m_WidthByte; // délka linky ikony v bajtech
	int	srcinc = widthbyte;		// přírustek zdrojové adresy
	int	width2 = m_Width;		// cílová šířka ikony
	int	height2 = m_Height;		// cílová výška ikony

// příprava ukazatelů pro malou ikonu
	switch (smallicon)
	{
	case SI_SMALL:
		width2 = m_Width/2;
		height2 = m_Height/2;
		srcinc = 2*widthbyte - m_Width;
		break;

	case SI_MEDIUM:
		width2 = m_Width*3/4;
		height2 = m_Height*3/4;
		srcinc = 4*widthbyte - m_Width;
		break;
	}
	int dstinc = (m_Num*width2 + 3) & ~3; // přírustek cílové adresy (délka cílové linky)

// vytvoření bufferu (s rezervou 8 bajtů na konci)
	BYTE* buffer = (BYTE*)MemGet(dstinc*height2 + 8);
	if (buffer == NULL) return NULL;

// ukazatel cílové adresy
	BYTE* dst = buffer;					// ukazatel cílové adresy

// cyklus přes všechny ikony
	int i, j, k;
	for (i = 0; i < m_Max; i++)
	{

// test, zda je ikona platná
		if (m_Valid[i])
		{

// příprava zdrojové adresy dat ikony
			m_Data[i].DeComp();
			BYTE* src = m_Data[i].DataData();

			switch (smallicon)
			{
// malá ikona
			case SI_SMALL:
// cyklus přes všechny linky jedné ikony
				for (j = height2; j > 0; j--)
				{
// kopie jedné linky ikony
					for (k = width2; k > 0; k--)
					{
						*dst = ColAvrg(src[0], src[1], src[widthbyte], src[widthbyte+1]);
						src++;
						src++;
						dst++;
					}

// zvýšení adres na další linku
					src += srcinc;
					dst += dstinc - width2;
				}
				break;

			case SI_MEDIUM:
				for (j = height2/3; j > 0; j--)
				{

// kopie jedné linky ikony
					for (k = width2/3; k > 0; k--)
					{
						dst[0] = src[0];
						dst[1] = ColAvrg(src[1], src[2], src[1], src[2]);
						dst[2] = ColAvrg(src[2], src[3], src[2], src[3]);
						
						dst[0+dstinc] = ColAvrg(src[widthbyte], src[2*widthbyte], src[widthbyte], src[2*widthbyte]);
						dst[1+dstinc] = ColAvrg(src[1+widthbyte], src[1+2*widthbyte], src[2+widthbyte], src[2+2*widthbyte]);
						dst[2+dstinc] = ColAvrg(src[2+widthbyte], src[2+2*widthbyte], src[3+widthbyte], src[3+2*widthbyte]);

						dst[0+2*dstinc] = ColAvrg(src[2*widthbyte], src[3*widthbyte], src[2*widthbyte], src[3*widthbyte]);
						dst[1+2*dstinc] = ColAvrg(src[1+2*widthbyte], src[1+3*widthbyte], src[2+2*widthbyte], src[2+3*widthbyte]);
						dst[2+2*dstinc] = ColAvrg(src[2+2*widthbyte], src[2+3*widthbyte], src[3+2*widthbyte], src[3+3*widthbyte]);

						src += 4;
						dst += 3;
					}

// zvýšení adres na další linku
					src += srcinc;
					dst += dstinc*3 - width2;
				}
				break;

// velká ikona
			default:

// cyklus přes všechny linky jedné ikony
				for (j = height2; j > 0; j--)
				{
					MemCopy(dst, src, width2);

// zvýšení adres na další linku
					src += srcinc;
					dst += dstinc;
				}
			}

// posun ukládací adresy na další ikonu
			dst += width2 - height2*dstinc;
		}
	}

// adresa bufferu s daty
	return buffer;
}


/////////////////////////////////////////////////////////////////////////////
// vygenerování masky ikon z dat barevné bitmapy (smallicon = poloviční velikost)
// barevná bitmapa musí mít na konci rezervu pro zarovnání linky na 8 bajtů !
// vrací adresu na buffer, při chybě vrací NULL

BYTE* CBufIcon::GenerMask(BYTE* bitmapa, SMALLICON smallicon)
{
// příprava ukazatelů pro normální ikonu
	int width2 = m_Width;				// cílová šířka ikony
	int height2 = m_Height;				// cílová výška ikony

// příprava ukazatelů pro malou ikonu
	switch (smallicon)
	{
	case SI_SMALL:
		width2 /= 2;
		height2 /= 2;
		break;

	case SI_MEDIUM:
		width2 = m_Width*3/4;
		height2 = m_Height*3/4;
		break;
	}

// délka linky v bodech
	int width = m_Num*width2;

// přírustek zdrojové adresy
	int srcinc = (width + 3) & ~3;

// přírustek cílové adresy
	int dstinc = ((width + 15) & ~0xf) / 8;

// vytvoření bufferu pro masku
	BYTE* data = (BYTE*)MemGet(dstinc * height2);
	if (data == NULL) return NULL;

// ukazatel cílové adresy
	BYTE* dst = data;

// cyklus přes všechny linky
	for (int i = height2-1; i >= 0; i--)
	{

// ukazatel zdrojové adresy
		BYTE* src = bitmapa + srcinc*i;

// cyklus přes všechny bajty cílové linky
		for (int j = dstinc; j > 0; j--)
		{

// vygenerování bajtu pro 8 bodů (může přesáhnout za konec linky!)
			BYTE stradac = 0;
			if (src[0] == BackCol) stradac  = 0x80;
			if (src[1] == BackCol) stradac |= 0x40;
			if (src[2] == BackCol) stradac |= 0x20;
			if (src[3] == BackCol) stradac |= 0x10;
			if (src[4] == BackCol) stradac |= 0x08;
			if (src[5] == BackCol) stradac |= 0x04;
			if (src[6] == BackCol) stradac |= 0x02;
			if (src[7] == BackCol) stradac |= 0x01;
			src += 8;

// uložení cílového bajtu
			dst[0] = stradac;			// uložení střadače bitů masky
			dst++;						// zvýšení ukládací adresy masky
		}
	}

// adresa bufferu s daty
	return data;
}

/////////////////////////////////////////////////////////////////////////////
// vygenerování seznamu ikon (drag = tažení, smallicon = poloviční velikost,
//		himg = použitý seznam, pro himg=NULL vytvoří nový seznam)
// při chybě vrací NULL, původní seznam ikon přitom zůstává nezměněn

HIMAGELIST CBufIcon::GenerList(bool drag, SMALLICON smallicon, HIMAGELIST himg)
{
// příprava cílové šířky a výšky ikony
	int width2 = m_Width;				// cílová šířka ikony
	int height2 = m_Height;				// cílová výška ikony
	switch (smallicon)
	{
	case SI_SMALL:
		width2 /= 2;
		height2 /= 2;
		break;

	case SI_MEDIUM:
		width2 = m_Width*3/4;
		height2 = m_Height*3/4;
		break;
	}

// vytvoření dat barevné a mono bitmapy
	BYTE* data = GenerBitmap(smallicon);	// vygenerování barevné bitmapy
	if (data == NULL) return NULL;			// chyba paměti
	BYTE* mono = GenerMask(data, smallicon); // vygenerování masky bitmapy
	if (mono == NULL)					// chyba paměti
	{
		MemFree(data);					// zrušení bufferu barevné bitmapy
		return NULL;
	}

// vytvoření nového seznamu ikon
	bool newimg = false;				// příznak vytvoření nového seznamu
	if (himg == NULL)
	{
		himg = ::ImageList_Create(width2, height2, ILC_COLORDDB | ILC_MASK,m_Num,50);
		newimg = true;
	}
	ASSERT(himg);	

	if (himg == NULL)
	{
		MemFree(data);
		MemFree(mono);
		return NULL;
	}

// korekce na obrázky taźení
	if (drag)
	{
		BYTE* dst = mono;
		BYTE maska = 0xAA;

		for (int j = height2; j > 0; j--)
		{
			for (int i = (((m_Num*width2) + 15) & ~0xf) /8; i > 0; i--)
			{
				dst[0] |= maska;
				dst++;
			}
			maska ^= 0xff;
		}
	}

// vytvoření mono bitmapy (maska)
	HBITMAP bmpMono = ::CreateBitmap(width2*m_Num, height2, 1, 1, mono);
	ASSERT(bmpMono != NULL);
	if (bmpMono == NULL)
	{
		if (newimg) ::ImageList_Destroy(himg);
		MemFree(data);
		MemFree(mono);
		return NULL;
	}

// otevření DC displeje
	HDC dc = ::GetDC(0);
	ASSERT(dc != NULL);

// výběr a realizace vlastních palet 
	HPALETTE OldPal = ::SelectPalette(dc, StdPalette, FALSE);
	::RealizePalette(dc);

// příprava záhlaví BMP
	StdBitmapInfo->bmiHeader.biWidth = width2*m_Num;
	StdBitmapInfo->bmiHeader.biHeight = height2;

// vytvoření barevné bitmapy
	HBITMAP bmp = ::CreateDIBitmap(dc,&(StdBitmapInfo->bmiHeader),
		CBM_INIT, data, StdBitmapInfo, DIB_RGB_COLORS);
	ASSERT(bmp != NULL);

// přidání bitmapy k seznamu
	int result = ::ImageList_Add(himg, bmp, bmpMono);
	ASSERT(result != -1);

// zrušení bitmap
	if (bmp != NULL) ::DeleteObject(bmp);
	::DeleteObject(bmpMono);

// uvolnění palet a DC displeje
	if (OldPal != NULL) ::SelectPalette(dc,OldPal,FALSE);
	::ReleaseDC(0,dc);

// zrušení pracovních bufferů s daty
	MemFree(data);
	MemFree(mono);

// chyba
	if (result == -1)
	{
		if (newimg) ::ImageList_Destroy(himg);
		return NULL;
	}

// navrácení seznamu ikon
	return himg;
}
