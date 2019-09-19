
#include "Main.h"

/***************************************************************************\
*																			*
*									Sprajt									*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// inicializační prázdný sprajt (modifikuje se počet referencí!)

SPRITEDATA*	EmptySpriteData = NULL;
CSprite		const EmptySprite;				// prázdný sprajt


/////////////////////////////////////////////////////////////////////////////
// statická inicializace sprajtů (při chybě paměti vrací FALSE)

bool InitSprite()
{
	EmptySpriteData = NewSpriteData(1,1);
	return (EmptySpriteData != NULL);
}
	

/////////////////////////////////////////////////////////////////////////////
// vytvoření dat sprajtu (při chybě paměti vrací NULL), obrázky prázdné o velikosti ICONSIZE

SPRITEDATA* _fastcall NewSpriteData(int faze, int smer)
{
	ASSERT((faze > 0) && (smer > 0));

// vytvoření dat sprajtu
	SPRITEDATA* data = (SPRITEDATA*)MemGet(faze*smer*sizeof(CPicture) + SIZEOFSPRITEDATA);
	if (data != NULL)
	{

// nastavení parametrů sprajtu
		data->Refer = 1;				// počet referencí
		data->Faze = faze;				// počet fází
		data->Smer = smer;				// počet směrů
		data->Klid = 1;					// počet klidových fází
		data->Delay = SPRITEDEFDELAY;	// prodleva mezi fázemi (v milisekundách)
		data->Level = SPRITEDEFLEVEL;	// hladina k překreslování (0=předměty)
		data->Kroku = SPRITEDEFKROKU;	// počet fází na jednotkovou vzdálenost (0=ihned)

// inicializace obrázků
		CPicture* pic = data->Data;
		for (int i = faze*smer; i > 0; i--)
		{
			pic->Init();
			pic++;
		}
	}
	return data;
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření dat sprajtu bez inicializace obrázků a nastavení parametrů (při chybě paměti vrací NULL)

SPRITEDATA* _fastcall NewSpriteData0(int faze, int smer)
{
	ASSERT((faze > 0) && (smer > 0));

// vytvoření dat sprajtu
	SPRITEDATA* data = (SPRITEDATA*)MemGet(faze*smer*sizeof(CPicture) + SIZEOFSPRITEDATA);
	if (data != NULL)
	{

// nastavení parametrů sprajtu
		data->Refer = 1;				// počet referencí
		data->Faze = faze;				// počet fází
		data->Smer = smer;				// počet směrů
	}
	return data;
}


/////////////////////////////////////////////////////////////////////////////
// zrušení dat sprajtu (odděleno kvůli lepší optimalizaci)

void _fastcall DelSpriteData(SPRITEDATA* data)
{
	ASSERT(data != NULL);

// ukončení obrázků
	CPicture* pic = data->Data;
	for (int i = data->Faze*data->Smer; i > 0; i--)
	{
		pic->Term();
		pic++;
	}

// zrušení dat sprajtu
	MemFree(data);
}


/////////////////////////////////////////////////////////////////////////////
// konstruktor a destruktor

CSprite::CSprite() 
{ 
	attach(EmptySpriteData); 
};

CSprite::CSprite(const CSprite& src) 
{ 
	attach(src.pData); 
};

CSprite::CSprite(SPRITEDATA* data)
{ 
	attach(data); 
};

CSprite::~CSprite() 
{ 
	detach(pData); 
};


/////////////////////////////////////////////////////////////////////////////
// statický konstruktor a destruktor

void CSprite::Init()
{ 
	attach(EmptySpriteData); 
};

void _fastcall CSprite::Init(const CSprite& src) 
{ 
	attach(src.pData); 
};

void _fastcall CSprite::Init(SPRITEDATA* data)
{ 
	attach(data); 
};

bool _fastcall CSprite::Init(int faze, int smer)	// při chybě paměti vrátí FALSE, sprajt není vytvořen
{
	pData = NewSpriteData(faze, smer);
	return (pData != NULL);
};

void CSprite::Term()
{ 
	detach(pData); 
};


/////////////////////////////////////////////////////////////////////////////
// vyprázdnění sprajtu (uvolnění dat) - připojí se standardní prázdný sprajt o velikosti 1x1

void CSprite::Empty()
{ 
	Term();
	Init();
}


/////////////////////////////////////////////////////////////////////////////
// kopie do vlastního bufferu před modifikací (pozor - obrázky tímto nejsou přivlastněny!)
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool CSprite::CopyWrite()
{
// úschova ukazatelů
	SPRITEDATA* olddata = pData;		// adresa starých dat
	long* refer = &(olddata->Refer);	// počet referencí

// test, zda je nutné přivlastnění
	if (*refer > 1)					// je nějaký jiný majitel?
	{

// vytvoření nového bufferu (bez inicializace obrázků)
		SPRITEDATA* newdata = NewSpriteData0(olddata->Faze, olddata->Smer);
		if (newdata == NULL) return false;

// kopie obrázků
		CPicture* dst = newdata->Data;
		CPicture* src = olddata->Data;
		for (int i = olddata->Faze * olddata->Smer; i > 0; i--)
		{
			dst->Init(src->Data());
			dst++;
			src++;
		}

// přenesení parametrů
		newdata->Klid = olddata->Klid;
		newdata->Delay = olddata->Delay;
		newdata->Level = olddata->Level;
		newdata->Kroku = olddata->Kroku;

// odpojení starých dat
		detach(olddata);

// připojení nových dat
		pData = newdata;
	}

// operace OK
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření nového sprajtu (připraveno k zápisu, data jsou náhodná)
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool _fastcall CSprite::New(int faze, int smer)
{
// ukazatel na stará data
	SPRITEDATA* olddata = pData;

// test, zda je nutné vytvoření nového bufferu
	if ((olddata->Refer > 1) ||
		(olddata->Faze != faze) ||
		(olddata->Smer != smer))
	{

// vytvoření nového bufferu (ikony i parametry inicializovány)
		SPRITEDATA* newdata = NewSpriteData(faze, smer);
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
// nastavení nových rozměrů sprajtu (počet fází pro klid a pohyb, počet směrů)
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool CSprite::ReSize(int klid, int pohyb, int smer)
{
// korekce hodnot
	if (klid < 1) klid = 1;
	if (klid > SPRITEMAXKLID) klid = SPRITEMAXKLID;
	if (pohyb < 0) pohyb = 0;
	if (pohyb > SPRITEMAXPOHYB) pohyb = SPRITEMAXPOHYB;
	if (smer < 1) smer = 1;
	if (smer > SPRITEMAXSMER) smer = SPRITEMAXSMER;

// úschova starých parametrů
	SPRITEDATA* olddata = pData;
	int oldfaze = olddata->Faze;
	int oldsmer = olddata->Smer;
	int oldklid = olddata->Klid;
	int oldpohyb = oldfaze - oldklid;

// kontrola, zda se parametry mění
	if ((klid == oldklid) &&
		(pohyb == oldpohyb) &&
		(smer == oldsmer))
	{
		return true;
	}

// vytvoření nového bufferu (ikony neinicializovány)
	int faze = klid + pohyb;
	SPRITEDATA* newdata = NewSpriteData0(faze, smer);
	if (newdata == NULL) return false;

// nastavení parametrů
	newdata->Klid = klid;
	newdata->Delay = olddata->Delay;
	newdata->Level = olddata->Level;
	newdata->Kroku = olddata->Kroku;

// ukazatele dat
	CPicture* src = olddata->Data;
	CPicture* dst = newdata->Data;

// přenesení dat (cyklus po nových řádcích)
	for (int i = 0; i < smer; i++)
	{

// index starého řádku (směru)
		int oldi = (i*oldsmer + (smer-1)/2) / smer;
		if (oldi >= oldsmer) oldi = oldsmer-1;

// offset začátku starého a nového řádku
		int off = i*faze;
		int oldoff = oldi*oldfaze;

// přenesení dat v klidu (cyklus po nových klidových pozicích)
		for (int j = 0; j < klid; j++)
		{
//			int oldj = (j*oldklid + (klid-1)/2) / klid;
			int oldj = j - (j/oldklid)*oldklid;
//			if (oldj >= oldklid) oldj = oldklid-1;
			dst[off].Init(src[oldoff + oldj].Data());
			off++;
		}

// přenesení dat v pohybu (cyklus po nových pozicích v pohybu)
		for (j = 0; j < pohyb; j++)
		{
			int oldj;
			if (oldpohyb > 0)
			{
//				oldj = (j*oldpohyb + (pohyb-1)/2) / pohyb + oldklid;
				oldj = j - (j/oldpohyb)*oldpohyb + oldklid;
			}
			else
			{
//				oldj = (j*oldklid + (pohyb-1)/2) / pohyb;
				oldj = j - (j/oldklid)*oldklid;
			}
//			if (oldj >= oldfaze) oldj = oldfaze-1;

			dst[off].Init(src[oldoff + oldj].Data());
			off++;
		}
	}

// odpojení starých dat
	detach(olddata);

// připojení nových dat
	pData = newdata;

// operace OK
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// poskytnutí prvku (s kontrolou platnosti offsetu/indexu)
// pro neplatný prvek vrací přístup na prvek prázdného sprajtu

const CPicture& _fastcall CSprite::Get(const int off) const
{
	if (IsValid(off))				// je offset platný?
	{
		return pData->Data[off];	// prvek na daném offsetu
	}
	return EmptySpriteData->Data[0]; // neplatný offset
}

const CPicture& _fastcall CSprite::Get(const int faze, const int smer) const
{
	if (IsValid(faze, smer))				// je index platný?
	{
		return pData->Data[faze + smer*pData->Faze];	// prvek
	}
	return EmptySpriteData->Data[0]; // neplatný offset
}


////////////////////////////////////////////////////////////////////
// nastavení prvku (s kontrolou platnosti offsetu/indexu)

void _fastcall CSprite::Set(const int off, const CPicture& data)
{
	if (IsValid(off))				// je offset platný?
	{
		pData->Data[off] = data;
	}
}

void _fastcall CSprite::Set(const int faze, const int smer, const CPicture& data)
{
	if (IsValid(faze, smer))				// je index platný?
	{
		pData->Data[faze + smer*pData->Faze] = data;
	}
}


/////////////////////////////////////////////////////////////////////////////
// načtení sprajtu ze souboru
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool CSprite::LoadFile(CText jmeno)
{
// otevření souboru mapovaného do paměti (uzavřen při destrukci!)
	CFileMap file;
	if (!file.Open(jmeno)) return false;

// kontrola velikosti souboru
	int size = file.Size();
	if (size < 100) return false;

// načtení základních parametrů
	SPRITEFILE* spr = (SPRITEFILE*)file.Adr();
	int faze = spr->Faze;			// počet fází celkem
	int smer = spr->Smer;			// počet směrů celkem
	int klid = spr->Klid;			// počet klidových fází
	int width = spr->Width;			// šířka obrázku
	int height = spr->Height;		// výška obrázku
	int delay = spr->Delay;			// prodleva mezi fázemi
	int level = spr->Level;			// hladina k překreslování
	double kroku = spr->Kroku;		// počet fází na krok
	int pals = spr->Colors;			// počet palet
	int format = spr->Format;		// formát dat

// kontrola platnosti souboru
	if ((spr->Ident[0] != 'P') ||
		(spr->Ident[1] != 'S') ||
		(spr->Ident[2] != 'P') ||
		(spr->Ident[3] != 'R') ||
		(faze < 1) ||
		(faze > SPRITEMAXFAZE) ||
		(smer < 1) ||
		(smer > SPRITEMAXSMER) ||
		(klid < 1) ||
		(klid > SPRITEMAXKLID) ||
		(klid > faze) ||
		(faze-klid > SPRITEMAXPOHYB) ||
		(width < 1) ||
		(height < 1) ||
		(delay < 0) ||
		(delay > SPRITEMAXDELAY) ||
		(kroku < 0) ||
		(kroku > SPRITEMAXKROKU) ||
		(pals < 10) ||
		(pals > 256) ||
		(format > 1))
	{
		return false;
	}

// příprava konverzní tabulky palet
	BITMAPINFO* bmp = (BITMAPINFO*)MemGet(sizeof(BITMAPINFO) + pals*4 + 200);
	if (bmp == NULL) return false;
	bmp->bmiHeader.biClrUsed = pals;
	bmp->bmiHeader.biBitCount = 8;
	MemCopy(bmp->bmiColors, spr->ColorTab, pals*4);
	bmp->bmiColors[0] = StdBitmapInfo->bmiColors[0];

// barva 1 bude jako stín (kromě importu ze starší verze)
	if (pals >= StdColors)
	{
		bmp->bmiColors[1] = StdBitmapInfo->bmiColors[1];
	}

	GenKonvPal(bmp);
	MemFree(bmp);

// vytvoření sprajtu a nastavení parametrů
	SPRITEDATA* newdata = NewSpriteData(faze, smer);
	if (newdata == NULL) return false;
	newdata->Klid = klid;			// počet klidových fází
	newdata->Delay = delay;		// prodleva mezi fázemi
	newdata->Level = level;		// hladina k překreslování
	newdata->Kroku = kroku;		// počet fází na jednotkovou vzdálenost

// dekomprimace bufferu
	BYTE* src = file.Adr() + SIZEOFSPRITEFILE + pals*4;
	BYTE* buf = NULL;
	if (format == 1)
	{
		buf = (BYTE*)MemGet(faze*smer*width*height + 1000);
		if (buf == NULL)
		{
			DelSpriteData(newdata);
			return false;
		}
		DeCompress(buf, src + 4, faze*smer*width*height, width);
		src = buf;
	}

// načtení obrázků
	CPicture* dst = newdata->Data;
	for (int i = faze*smer; i > 0; i--)
	{
		if (!dst->New(width, height) ||	!dst->CopyKonvData(src) || !dst->Comp())
		{
			DelSpriteData(newdata);
			MemFree(buf);
			return false;
		}
		dst++;
		src += width*height;
	}

// zrušení případného bufferu
	MemFree(buf);

// odpojení starých dat
	detach(pData);

// připojení nových dat
	pData = newdata;

// příznak - načteno OK
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// uložení do souboru (FALSE=chyba)

bool CSprite::SaveFile(CText jmeno) const
{
// vytvoření souboru
	CFile file;
	file.Name(jmeno);
	if (!file.Create()) return false;

// příprava velikosti souboru
	int width = pData->Data[0].Width();
	int height = pData->Data[0].Height();
	int icons = pData->Faze * pData->Smer;

// buffer k uložení souboru
	BYTE* buf = (BYTE*)MemGet(SIZEOFSPRITEFILE + StdColors*4 + 4 + icons*width*height*3/2);
	if (buf == NULL)
	{
		file.Close();
		file.Delete();
		return false;
	}

// naplnění záhlaví souboru
	SPRITEFILE* head = (SPRITEFILE*)buf;
	head->Ident[0] = 'P';					// identifikace
	head->Ident[1] = 'S';
	head->Ident[2] = 'P';
	head->Ident[3] = 'R';
	head->Faze = pData->Faze;				// počet fází celkem
	head->Smer = pData->Smer;				// počet směrů
	head->Klid = pData->Klid;				// z toho počet klidových fází
	head->Width = (WORD)width;				// šířka obrázku
	head->Height = (WORD)height;			// výška obrázku
	head->Delay = pData->Delay;				// prodleva mezi fázemi v milisekundách
	head->Level = pData->Level;				// hladina k překreslování
	head->Kroku = pData->Kroku;				// počet fází na jednotkovou vzdálenost
	head->Colors = (short)StdColors;		// počet palet
	head->Format = 1;						// formát - je komprese dat

// přenesení palet
	MemCopy(head->ColorTab, StdBitmapInfo->bmiColors, StdColors*4);

// příprava a komprimace dat obrázku
	BYTE* buf0 = (BYTE*)MemGet(width*height*icons);
	if (buf0 == NULL)
	{
		MemFree(buf);
		file.Close();
		file.Delete();
		return false;
	}

	BYTE* dst = buf0;
	for (int i = 0; i < icons; i++)
	{
		if (!pData->Data[i].DeComp())
		{
			MemFree(buf0);
			MemFree(buf);
			file.Close();
			file.Delete();
			return false;
		}
		MemCopy(dst, pData->Data[i].DataData(), width*height);
		dst += width*height;
	}

	int n = Compress(buf + SIZEOFSPRITEFILE + StdColors*4 + 4, buf0, width*height*icons, width);
	*(int*)(buf + SIZEOFSPRITEFILE + StdColors*4) = n;
	MemFree(buf0);

// uložení souboru
	BOOL result = file.Write(buf, SIZEOFSPRITEFILE + StdColors*4 + 4 + n);

// uzavření souboru
	file.Close();

// zrušení datového bufferu
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

const CSprite& CSprite::operator= (const CSprite& src)
{
	detach(pData);				// zrušení starých dat
	attach(src.pData);		// přiřazení nových dat
	return *this;
}

const CSprite& CSprite::operator= (SPRITEDATA* src)
{
	detach(pData);				// zrušení starých dat
	attach(src);				// přiřazení nových dat
	return *this;
}

/***************************************************************************\
*																			*
*								Buffer sprajtů								*
*																			*
\***************************************************************************/

////////////////////////////////////////////////////////////////////
// přidání záznamu o vložení položky do UNDO bufferu

template <> bool CBuffer<CSprite>::UndoAddIns(int index)
{
	return Undo.AddSprIns(index);
}

////////////////////////////////////////////////////////////////////
// vytvoření nového sprajtu (vrací index položky, <0 = chyba), data jsou náhodná
// provádí záznam do UNDO bufferu

int _fastcall CBufSprite::New(int faze, int smer)
{
// vytvoření nové položky
	int result = New();
	if (result >= 0)
	{

// inicializace objektu
		if (!m_Data[result].New(faze, smer))
		{
			Del(result);
			if (m_Undo) Undo.DelAll();
			result = -1;
		}
	}

// index nové položky (nebo <0 = chyba)
	return result;
}
