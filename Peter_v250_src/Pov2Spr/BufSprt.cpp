
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
// statická inicializace sprajtů

void InitSprite()
{
	EmptySpriteData = (SPRITEDATA*)MemGet(sizeof(CPicture) + SIZEOFSPRITEDATA);
	EmptySpriteData->Refer = 1;		// počet referencí
	EmptySpriteData->Faze = 1;		// počet fází
	EmptySpriteData->Smer = 1;		// počet směrů
	EmptySpriteData->Klid = 1;		// počet klidových fází
	EmptySpriteData->Delay = SPRITEDEFDELAY; // prodleva mezi fázemi
	EmptySpriteData->Level = SPRITEDEFLEVEL; // hladina k překreslování
	EmptySpriteData->Kroku = SPRITEDEFKROKU; // počet fází na jednotku
	EmptySpriteData->Data[0].Init(); // inicializace obrázku (velikost ICONSIZE)
}


/////////////////////////////////////////////////////////////////////////////
// konstruktor a destruktor

CSprite::CSprite() 
{ 
	Attach(EmptySpriteData); 
};

CSprite::CSprite(const CSprite& src) 
{ 
	Attach(src.pData); 
};

CSprite::CSprite(int faze, int smer)
{
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
	Attach(EmptySpriteData); 
};

void CSprite::Init(SPRITEDATA* data)
{ 
	Attach(data); 
};

void CSprite::Init(int faze, int smer)
{
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
// vytvoření nového sprajtu (připraveno k zápisu)

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
// načtení sprajtu ze souboru (TRUE=operace OK)

BOOL CSprite::LoadFile(CString jmeno)
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

// zjištění velikosti souboru
	int size = (int)::SetFilePointer(hFile, 0, NULL, FILE_END);
	::SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

// kontrola velikosti souboru
	if ((size < 100) || (size > 200000000))
	{
		::CloseHandle(hFile);		// uzavření souboru
		return FALSE;
	}

// příprava bufferu k načtení souboru
	BYTE* buf = (BYTE*)MemGet(size + 1000);

// načtení souboru
	DWORD read;
	::ReadFile(hFile, buf, size, &read, NULL);

// uzavření souboru
	::CloseHandle(hFile);		// uzavření souboru

// načtení základních parametrů
	SPRITEFILE* spr = (SPRITEFILE*)buf;
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
	if ((read != (DWORD)size) ||
		(spr->Ident[0] != 'P') ||
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
		MemFree(buf);
		return FALSE;
	}

// příprava konverzní tabulky palet
	BITMAPINFO* bmp = (BITMAPINFO*)MemGet(sizeof(BITMAPINFO) + pals*4 + 200);
	bmp->bmiHeader.biClrUsed = pals;
	bmp->bmiHeader.biBitCount = 8;
	MemCopy(bmp->bmiColors, spr->ColorTab, pals*4);
	bmp->bmiColors[0] = StdBitmapInfo->bmiColors[0];
	GenKonvPal(bmp);
	MemFree(bmp);

// vytvoření sprajtu a nastavení parametrů
	New(faze, smer);
	pData->Klid = klid;			// počet klidových fází
	pData->Delay = delay;		// prodleva mezi fázemi
	pData->Level = level;		// hladina k překreslování
	pData->Kroku = kroku;		// počet fází na jednotkovou vzdálenost

// dekomprimace dat
	if (format == 1)
	{
		BYTE* buf0 = (BYTE*)MemGet(faze*smer*width*height + 1000);
		DeCompress(buf0, buf + SIZEOFSPRITEFILE + pals*4 + 4, faze*smer*width*height, width);

		for (int i = 0; i < faze*smer; i++)
		{
			pData->Data[i].New(width, height);
			pData->Data[i].CopyKonvData(buf0 + i*width*height);
		}
		MemFree(buf0);
	}

// načtení obrázků
	else
	{
		for (int i = 0; i < faze*smer; i++)
		{
			pData->Data[i].New(width, height);
			pData->Data[i].CopyKonvData(buf + SIZEOFSPRITEFILE + pals*4 + i*width*height);
		}
	}

// zrušení bufferu
	MemFree(buf);

// příznak - načteno OK
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// uložení do souboru (FALSE=chyba)

BOOL CSprite::SaveFile(CString jmeno) const
{

// otevření souboru
	HANDLE hFile = ::CreateFile(jmeno, GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

// test, zda byl soubor vytvořen
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

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
		MemCopy(dst, pData->Data[i].DataData(), width*height);
		dst += width*height;
	}
	int n = Compress(buf + SIZEOFSPRITEFILE + StdColors*4 + 4, buf0, width*height*icons, width);
	*(int*)(buf + SIZEOFSPRITEFILE + StdColors*4) = n;
	MemFree(buf0);

// uložení souboru
	DWORD write;
	int size = SIZEOFSPRITEFILE + StdColors*4 + 4 + n;
	BOOL result = ::WriteFile(hFile, buf, size, &write, NULL);

// uzavření souboru
	::CloseHandle(hFile);

// zrušení datového bufferu
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

/*
BOOL CSprite::LoadFile(CString jmeno)
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

// zjištění velikosti souboru
	int size = (int)::SetFilePointer(hFile, 0, NULL, FILE_END);
	::SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

// kontrola velikosti souboru
	if ((size < 100) || (size > 200000000))
	{
		::CloseHandle(hFile);		// uzavření souboru
		return FALSE;
	}

// příprava bufferu k načtení souboru
	BYTE* buf = (BYTE*)MemGet(size + 100);

// načtení souboru
	DWORD read;
	::ReadFile(hFile, buf, size, &read, NULL);

// uzavření souboru
	::CloseHandle(hFile);		// uzavření souboru

// načtení základních parametrů
	SPRITEFILE* spr = (SPRITEFILE*)buf;
	int faze = spr->Faze;			// počet fází celkem
	int smer = spr->Smer;			// počet směrů celkem
	int klid = spr->Klid;			// počet klidových fází
	int width = spr->Width;			// šířka obrázku
	int height = spr->Height;		// výška obrázku
	int delay = spr->Delay;			// prodleva mezi fázemi
	int level = spr->Level;			// hladina k překreslování
	double kroku = spr->Kroku;		// počet fází na krok
	int pals = spr->Colors;			// počet palet

// kontrola platnosti souboru
	if ((read != (DWORD)size) ||
		(spr->Ident[0] != 'P') ||
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
		((int)(SIZEOFSPRITEFILE + pals*4 + faze*smer*width*height) > size))
	{
		MemFree(buf);
		return FALSE;
	}

// příprava konverzní tabulky palet
	BITMAPINFO* bmp = (BITMAPINFO*)MemGet(sizeof(BITMAPINFO) + pals*4 + 200);
	bmp->bmiHeader.biClrUsed = pals;
	bmp->bmiHeader.biBitCount = 8;
	MemCopy(bmp->bmiColors, spr->ColorTab, pals*4);
	bmp->bmiColors[0] = StdBitmapInfo->bmiColors[0];
	GenKonvPal(bmp);
	MemFree(bmp);

// vytvoření sprajtu a nastavení parametrů
	New(faze, smer);
	pData->Klid = klid;			// počet klidových fází
	pData->Delay = delay;		// prodleva mezi fázemi
	pData->Level = level;		// hladina k překreslování
	pData->Kroku = kroku;		// počet fází na jednotkovou vzdálenost

// načtení obrázků
	for (int i = 0; i < faze*smer; i++)
	{
		pData->Data[i].New(width, height);
		pData->Data[i].CopyKonvData(buf + SIZEOFSPRITEFILE + pals*4 + i*width*height);
	}

// zrušení bufferu
	MemFree(buf);

// příznak - načteno OK
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// uložení do souboru (FALSE=chyba)

BOOL CSprite::SaveFile(CString jmeno) const
{

// otevření souboru
	HANDLE hFile = ::CreateFile(jmeno, GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

// test, zda byl soubor vytvořen
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

// příprava velikosti souboru
	int width = pData->Data[0].Width();
	int height = pData->Data[0].Height();
	int icons = pData->Faze * pData->Smer;
	int size = SIZEOFSPRITEFILE + StdColors*4 + icons*width*height;

// buffer k uložení souboru
	BYTE* buf = (BYTE*)MemGet(size);

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
	head->Colors = StdColors;				// počet palet

// přenesení palet
	MemCopy(head->ColorTab, StdBitmapInfo->bmiColors, StdColors*4);

// přenesení dat obrázku
	BYTE* dst = buf + SIZEOFSPRITEFILE + StdColors*4;
	for (int i = 0; i < icons; i++)
	{
		MemCopy(dst, pData->Data[i].DataData(), width*height);
		dst += width*height;
	}


// uložení souboru
	DWORD write;
	BOOL result = ::WriteFile(hFile, buf, size, &write, NULL);

// uzavření souboru
	::CloseHandle(hFile);

// zrušení datového bufferu
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
*/

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
// vytvoření nových dat

#define NEWDATANUM 256				// počet nově vytvořených položek (1 KB)

void CBufSprite::NewData()
{
	int i = NEWDATANUM;			// počet nových položek
	int next = m_Max;			// příští položka - 1
	m_Max = next + i;			// zvýšení počtu položek (o 1 KB)

	MemBuf(m_Data, m_Max);		// zvýšení velikosti bufferu

	MemBuf(m_Valid, m_Max);		// zvýšení velikosti bufferu platnosti
	MemFill(m_Valid + next, NEWDATANUM, FALSE); // nastavení příznaků na neplatné položky

	CSprite* data = m_Data + next - 1; // ukazatel dat - 1
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

CBufSprite::CBufSprite()
{
	m_Data = NULL;			// není buffer dat
	m_Valid = NULL;			// není buffer platnosti
	m_Num = 0;				// není žádná platná položka
	m_Max = 0;				// není buffer položek
	m_Next = -1;			// přiští volná položka (-1=není)
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
	m_Valid = NULL;			// není buffer platnosti
	m_Num = 0;				// není žádná platná položka
	m_Max = 0;				// není buffer položek
	m_Next = -1;			// přiští volná položka (-1=není)
}

void CBufSprite::Term()
{
	DelAll();				// zrušení všech položek
}

////////////////////////////////////////////////////////////////////
// zrušení všech položek v bufferu (ukládání začne opět po řadě)

void CBufSprite::DelAll()
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

////////////////////////////////////////////////////////////////////
// zrušení položky (s kontrolou platnosti indexu)

void _fastcall CBufSprite::Del(const int index)
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
// duplikace položky (s kontrolou platnosti indexu, vrací index položky)

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


/////////////////////////////////////////////////////////////////////////////
// operátor přiřazení

const CBufSprite& CBufSprite::operator= (const CBufSprite& src)
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
