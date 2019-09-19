
#include "Main.h"

/***************************************************************************\
*																			*
*									Sprajt									*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// inicializační prázdný sprajt (modifikuje se počet referencí!)

SPRITEDATA*	EmptySpriteData = NULL;

/////////////////////////////////////////////////////////////////////////////
// statická inicializace sprajtů (při chybě paměti vrací FALSE)

BOOL InitSprite()
{
	EmptySpriteData = (SPRITEDATA*)MemGet(sizeof(CPicture) + SIZEOFSPRITEDATA);
	if (EmptySpriteData == NULL) return FALSE;
	EmptySpriteData->Refer = 1;		// počet referencí
	EmptySpriteData->Faze = 1;		// počet fází
	EmptySpriteData->Smer = 1;		// počet směrů
	EmptySpriteData->Klid = 1;		// počet klidových fází
	EmptySpriteData->Delay = SPRITEDEFDELAY; // prodleva mezi fázemi
	EmptySpriteData->Level = SPRITEDEFLEVEL; // hladina k překreslování
	EmptySpriteData->Kroku = SPRITEDEFKROKU; // počet fází na jednotku
	EmptySpriteData->Data[0].Init(); // inicializace obrázku (velikost ICONSIZE)
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// konstruktor a destruktor

CSprite::CSprite() 
{ 
	m_Visible = false;
	m_Moving = false;
	m_DispReq = false;

	m_X = 0;
	m_Y = 0;

	m_Timer = 0;
	m_KrokCit = 0;
	m_FazeN = 0;
	m_SmerN = 0;

	m_CilX = 0;
	m_CilY = 0;

	m_AktX = 0;
	m_AktY = 0;

	m_dX = 0;
	m_dY = 0;

	m_Smer = 0;
	m_FazeNext = -1;
	m_Status = -1;

	Attach(EmptySpriteData); 
};

CSprite::CSprite(const CSprite& src) 
{ 
	m_Visible = false;
	m_Moving = false;
	m_DispReq = false;

	m_X = 0;
	m_Y = 0;

	m_Timer = 0;
	m_KrokCit = 0;
	m_FazeN = 0;
	m_SmerN = 0;

	m_CilX = 0;
	m_CilY = 0;

	m_AktX = 0;
	m_AktY = 0;

	m_dX = 0;
	m_dY = 0;

	m_Smer = 0;
	m_FazeNext = -1;
	m_Status = -1;

	Attach(src.pData); 
};

CSprite::CSprite(int faze, int smer)
{
	m_Visible = false;
	m_Moving = false;
	m_DispReq = false;

	m_X = 0;
	m_Y = 0;

	m_Timer = 0;
	m_KrokCit = 0;
	m_FazeN = 0;
	m_SmerN = 0;

	m_CilX = 0;
	m_CilY = 0;

	m_AktX = 0;
	m_AktY = 0;

	m_dX = 0;
	m_dY = 0;

	m_Smer = 0;
	m_FazeNext = -1;
	m_Status = -1;

	NewBuffer(faze, smer);
};

CSprite::~CSprite() 
{ 
	Detach(); 
};


/////////////////////////////////////////////////////////////////////////////
// statický konstruktor a destruktor

void CSprite::Init()
{ 
	m_Visible = false;
	m_Moving = false;
	m_DispReq = false;

	m_X = 0;
	m_Y = 0;

	m_Timer = 0;
	m_KrokCit = 0;
	m_FazeN = 0;
	m_SmerN = 0;

	m_CilX = 0;
	m_CilY = 0;

	m_AktX = 0;
	m_AktY = 0;

	m_dX = 0;
	m_dY = 0;

	m_Smer = 0;
	m_FazeNext = -1;
	m_Status = -1;

	Attach(EmptySpriteData); 
};

void CSprite::Init(SPRITEDATA* data)
{ 
	m_Visible = false;
	m_Moving = false;
	m_DispReq = false;

	m_X = 0;
	m_Y = 0;

	m_Timer = 0;
	m_KrokCit = 0;
	m_FazeN = 0;
	m_SmerN = 0;

	m_CilX = 0;
	m_CilY = 0;

	m_AktX = 0;
	m_AktY = 0;

	m_dX = 0;
	m_dY = 0;

	m_Smer = 0;
	m_FazeNext = -1;
	m_Status = -1;

	Attach(data); 
};

void CSprite::Init(int faze, int smer)
{
	m_Visible = false;
	m_Moving = false;
	m_DispReq = false;

	m_X = 0;
	m_Y = 0;

	m_Timer = 0;
	m_KrokCit = 0;
	m_FazeN = 0;
	m_SmerN = 0;

	m_CilX = 0;
	m_CilY = 0;

	m_AktX = 0;
	m_AktY = 0;

	m_dX = 0;
	m_dY = 0;

	m_Smer = 0;
	m_FazeNext = -1;
	m_Status = -1;

	NewBuffer(faze, smer);
};

void CSprite::Term()
{ 
	Detach(); 
};


/////////////////////////////////////////////////////////////////////////////
// kopie do vlastního bufferu před modifikací

void CSprite::CopyWrite()
{
	SPRITEDATA* data = pData;			// adresa starých dat
	long* refer = &(data->Refer);	// počet referencí

	if (*refer > 1)					// je nějaký jiný majitel?
	{
		NewBuffer(data->Faze, data->Smer);	// vytvoření nového bufferu

		for (int i = data->Faze * data->Smer - 1; i >= 0; i--)
		{
			pData->Data[i] = data->Data[i];
		}

		pData->Klid = data->Klid;	// přenesení počtu klidových fází
		pData->Delay = data->Delay;	// přenesení prodlevy mezi fázemi
		pData->Level = data->Level;	// přenesení hladiny k překreslování
		pData->Kroku = data->Kroku;	// přenesení počtu fází na jednotku

// odpojení starých dat - v multithread může nastat i zrušení
		if (LongDecrement(refer))
		{
#ifdef _MT
			MemFree(data);			// případné zrušení dat
#endif	// _MT
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření nového sprajtu (připraveno k zápisu, data jsou náhodná)

void CSprite::New(int faze, int smer)
{
	Detach();						// odpojení starého sprajtu
	NewBuffer(faze, smer);			// vytvoření nového bufferu
}


/////////////////////////////////////////////////////////////////////////////
// nastavení nových rozměrů sprajtu (počet fází pro klid a pohyb, počet směrů)

void CSprite::ReSize(int klid, int pohyb, int smer)
{
// kontrola, zda se parametry mění
	if ((klid == pData->Klid) &&
		(pohyb == pData->Faze - pData->Klid) &&
		(smer == pData->Smer))
	{
		return;
	}

// korekce hodnot
	if (klid < 1) klid = 1;
	if (klid > SPRITEMAXKLID) klid = SPRITEMAXKLID;
	if (pohyb < 0) pohyb = 0;
	if (pohyb > SPRITEMAXPOHYB) pohyb = SPRITEMAXPOHYB;
	if (smer < 1) smer = 1;
	if (smer > SPRITEMAXSMER) smer = SPRITEMAXSMER;

// vytvoření nového bufferu
	SPRITEDATA* olddata = pData;
	NewBuffer(klid + pohyb, smer);
	SPRITEDATA* newdata = pData;
	newdata->Klid = klid;

// ukazatele dat
	CPicture* src = olddata->Data;
	CPicture* dst = newdata->Data;

// příprava parametrů
	int oldklid = olddata->Klid;
	int newklid = newdata->Klid;
	int oldfaze = olddata->Faze;
	int newfaze = newdata->Faze;
	int oldpohyb = oldfaze - oldklid;
	int newpohyb = newfaze - newklid;
	int oldsmer = olddata->Smer;
	int newsmer = newdata->Smer;

// přenesení dat
	int snew, sold, newoff, oldoff, f;

	for (snew = 0; snew < newsmer; snew++)
	{
		sold = ((snew * oldsmer + (newsmer-1)/2) / newsmer) % oldsmer;

		newoff = snew * newfaze;
		oldoff = sold * oldfaze;

		for (f = 0; f < newklid; f++)
		{
			dst[newoff + f] = src[oldoff + (f % oldklid)];
		}

		newoff += newklid;
		oldoff += oldklid;

		if (oldpohyb)
		{
			for (f = 0; f < newpohyb; f++)
			{
				dst[newoff + f] = src[oldoff + (f % oldpohyb)];
			}
		}
	}

// odpojení starého bufferu
	pData = olddata;
	Detach();
	pData = newdata;
}


/////////////////////////////////////////////////////////////////////////////
// poskytnutí prvku (s kontrolou platnosti offsetu/indexu)

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


/////////////////////////////////////////////////////////////////////////////
// poskytnutí dat aktuálního obrázku (podle aktuálního směru a fáze, bez kontroly)

PICTUREDATA* CSprite::AktPicData()
{
	ASSERT((DWORD)m_FazeN < (DWORD)pData->Faze);
	ASSERT((DWORD)m_SmerN < (DWORD)pData->Smer);
	CPicture* pic = &pData->Data[m_FazeN + m_SmerN*pData->Faze];
	pic->DeComp();
	return pic->Data();
}


/////////////////////////////////////////////////////////////////////////////
// vyprázdnění sprajtu (uvolnění dat)

void CSprite::Empty()
{ 
	Detach(); 
	Attach(EmptySpriteData); 
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

void CSprite::LoadFile()
{
// úschova offsetu souboru
	int oldoff = FileReadOff;

// načtení záhlaví souboru
	SPRITEFILE spr;
	spr.Colors = 0;
	FileReadBlok((BYTE*)&spr, SIZEOFSPRITEFILE);

// načtení základních parametrů
	int faze = spr.Faze;			// počet fází celkem
	int smer = spr.Smer;			// počet směrů celkem
	int klid = spr.Klid;			// počet klidových fází
	int width = spr.Width;			// šířka obrázku
	int height = spr.Height;		// výška obrázku
	int delay = spr.Delay;			// prodleva mezi fázemi
	int level = spr.Level;			// hladina k překreslování
	double kroku = spr.Kroku;		// počet fází na krok
	int pals = spr.Colors;			// počet palet
	int format = spr.Format;		// formát dat

// kontrola platnosti souboru
	if ((spr.Ident[0] != 'P') ||
		(spr.Ident[1] != 'S') ||
		(spr.Ident[2] != 'P') ||
		(spr.Ident[3] != 'R') ||
		(faze < 1) ||
		(faze > SPRITEMAXFAZE) ||
		(smer < 1) ||
		(smer > SPRITEMAXSMER) ||
		(klid < 1) ||
		(klid > SPRITEMAXKLID) ||
		(klid > faze) ||
		(faze - klid > SPRITEMAXPOHYB) ||
		(width < 1) ||
		(width > 32768) ||
		(height < 1) ||
		(height > 32768) ||
		(delay < 0) ||
		(delay > SPRITEMAXDELAY) ||
		(kroku < 0) ||
		(kroku > SPRITEMAXKROKU) ||
		(pals < 10) ||
		(pals > 256) ||
		(format > 1))
	{
		FileReadOff = oldoff;
		FileError = true;
		return;
	}

// příprava konverzní tabulky palet
	BITMAPINFO* bmp = (BITMAPINFO*)MemGet(sizeof(BITMAPINFO) + pals*4 + 200);
	bmp->bmiHeader.biClrUsed = pals;
	bmp->bmiHeader.biBitCount = 8;
	FileReadBlok((BYTE*)bmp->bmiColors, pals*4);
	bmp->bmiColors[0] = StdBitmapInfo->bmiColors[0];

// barva 1 bude jako stín (kromě importu ze starší verze)
	if (pals >= StdColors)
	{
		bmp->bmiColors[1] = StdBitmapInfo->bmiColors[1];
	}

	GenKonvPal(bmp);
	MemFree(bmp);

// vytvoření sprajtu a nastavení parametrů
	New(faze, smer);
	pData->Klid = klid;			// počet klidových fází
	pData->Delay = delay;		// prodleva mezi fázemi
	pData->Level = level;		// hladina k překreslování
	pData->Kroku = kroku;		// počet fází na jednotkovou vzdálenost

// načtení obrázků
	int i = width*height*faze*smer;
	BYTE* data = (BYTE*)MemGet(i);

	if (format == 1)
	{
		long n;
		FileReadBlok((BYTE*)&n, 4);
		BYTE* data0 = (BYTE*)MemGet(n);
		FileReadBlok(data0, n);
		DeCompress(data, data0, i, width);
		MemFree(data0);
	}
	else
	{
		FileReadBlok(data, i);
	}

	for (i = 0; i < faze*smer; i++)
	{
		pData->Data[i].New(width, height);
		pData->Data[i].CopyKonvData(data + i*width*height);
	}

	MemFree(data);
}


/////////////////////////////////////////////////////////////////////////////
// uložení do souboru

void CSprite::SaveFile() const
{
// příprava velikosti souboru
	int width = pData->Data[0].Width();
	int height = pData->Data[0].Height();
	int icons = pData->Faze * pData->Smer;

// buffer k uložení souboru
	BYTE* buf = (BYTE*)MemGet(SIZEOFSPRITEFILE + StdColors*4 + 4 + icons*width*height*3/2);

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
	BYTE* dst = buf0;
	for (int i = 0; i < icons; i++)
	{
		pData->Data[i].DeComp();
		MemCopy(dst, pData->Data[i].DataData(), width*height);
		dst += width*height;
	}
	int n = Compress(buf + SIZEOFSPRITEFILE + StdColors*4 + 4, buf0, width*height*icons, width);
	*(long*)(buf + SIZEOFSPRITEFILE + StdColors*4) = n;
	MemFree(buf0);

// uložení souboru
	FileWriteBlok(buf, SIZEOFSPRITEFILE + StdColors*4 + 4 + n);

// zrušení datového bufferu
	MemFree(buf);
}


/////////////////////////////////////////////////////////////////////////////
// operátor přiřazení

const CSprite& CSprite::operator= (const CSprite& src)
{
	Detach();				// zrušení starých dat
	Attach(src.pData);		// přiřazení nových dat
	return *this;
}


/***************************************************************************\
*																			*
*								Buffer sprajtů								*
*																			*
\***************************************************************************/


////////////////////////////////////////////////////////////////////
// konstruktor a destruktor

CBufSprite::CBufSprite()
{
	m_Data = NULL;			// není buffer dat
	m_Num = 0;				// není žádná platná položka
	m_Max = 0;				// není buffer položek
}

CBufSprite::~CBufSprite()
{
	DelAll();				// zrušení všech položek
}


////////////////////////////////////////////////////////////////////
// konstruktor a destruktor

void CBufSprite::Init()
{
	m_Data = NULL;			// není buffer dat
	m_Num = 0;				// není žádná platná položka
	m_Max = 0;				// není buffer položek
}

void CBufSprite::Term()
{
	DelAll();				// zrušení všech položek
}


////////////////////////////////////////////////////////////////////
// vytvoření nových dat

void CBufSprite::NewData()
{
	m_Max *= 2;
	if (m_Max == 0) m_Max = 0x400;
//	m_Max += 0x400;			// zvýšení počtu položek (o 4 KB)
	MemBuf(m_Data, m_Max);	// zvýšení velikosti bufferu
};


////////////////////////////////////////////////////////////////////
// zrušení všech položek v bufferu

void CBufSprite::DelAll()
{
	Del(m_Num);					// zrušení položek
	MemBuf(m_Data, 0);			// zrušení bufferu dat
	m_Max = 0;					// není žádná položka v bufferu
}

////////////////////////////////////////////////////////////////////
// poskytnutí položky (s kontrolou platnosti indexu)

const CSprite& _fastcall CBufSprite::Get(const int index) const
{
	if (IsValid(index))			// je index platný?
	{
		return m_Data[index];	// položka na daném indexu
	}
	return EmptySprite;			// pro neplatný index vrátí prázdný sprajt
}

////////////////////////////////////////////////////////////////////
// nastavení položky (s kontrolou platnosti indexu)

void _fastcall CBufSprite::Set(const int index, const CSprite& data)
{
	if (IsValid(index))			// je index platný?
	{
		m_Data[index] = data;
	}
}


/////////////////////////////////////////////////////////////////////////////
// vyprázdnění položky (bez jejího zrušení - jen pro uvolnění dat)

void _fastcall CBufSprite::Empty(const int index)
{
	if (IsValid(index))					// je index platný?
	{
		m_Data[index].Empty();
	}
}


////////////////////////////////////////////////////////////////////
// zrušení položek z konce bufferu

void _fastcall CBufSprite::Del(int num)
{
	if (num > m_Num) num = m_Num;

	CSprite* spr = m_Data + m_Num - 1;
	m_Num -= num;

	for (; num > 0; num--)
	{
		spr->Term();
		spr--;
	}
}


////////////////////////////////////////////////////////////////////
// vytvoření položky (vrací index položky)

int CBufSprite::New()
{
	int result = NewItem();		// vytvoření nové položky
	m_Data[result].Init();		// inicializace položky
	return result;
}

int CBufSprite::New(int faze, int smer)
{
	int result = NewItem();		// vytvoření nové položky
	m_Data[result].Init(faze, smer);	// inicializace položky
	return result;
}

////////////////////////////////////////////////////////////////////
// přidání položky (vrací index položky)

int _fastcall CBufSprite::Add(const CSprite& data)
{
	int result = NewItem();		// vytvoření nové položky
	m_Data[result].Init(data.Data());	// inicializace položky
	return result;
}


////////////////////////////////////////////////////////////////////
// duplikace položky (s kontrolou platnosti indexu, vrací index první položky)

int _fastcall CBufSprite::Dup(const int index)
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

int _fastcall CBufSprite::Dup(const int index, int num)
{
	int result = NewItem();		// vytvoření nové položky

	if (IsValid(index))					// je index platný?
	{
		SPRITEDATA* data = m_Data[index].Data();

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

const CBufSprite& CBufSprite::operator= (const CBufSprite& src)
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
