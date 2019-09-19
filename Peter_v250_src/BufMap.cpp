
#include "Main.h"

/***************************************************************************\
*																			*
*									Plocha									*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// inicializační prázdná plocha (modifikuje se počet referencí!)
// Prázdná plocha ukazuje na prázdnou ikonu 0!

MAPDATA	EmptyMapData		= { 1, 1, 1, 0, 0 };
const CMap		EmptyMap;			// prázdná plocha


////////////////////////////////////////////////////////////////////
// vytvoření dat plochy (při chybě paměti vrací NULL)

MAPDATA* _fastcall NewMapData(int width, int height)
{
	ASSERT((width > 0) && (height > 0));

// vytvoření dat plochy
	MAPDATA* data = (MAPDATA*)MemGet(SIZEOFMAPDATA + width*height*SIZEOFMAPITEM);
	if (data != NULL)
	{

// nastavení parametrů plochy
		data->Refer = 1;				// počet referencí
		data->Width = width;			// šířka
		data->Height = height;			// výška
	}

	return data;
}


/////////////////////////////////////////////////////////////////////////////
// zrušení dat plochy (odděleno kvůli lepší optimalizaci)

void _fastcall DelMapData(MAPDATA* data)
{
	ASSERT(data != NULL);

	MemFree(data);
}


/////////////////////////////////////////////////////////////////////////////
// konstruktor a destruktor

CMap::CMap() 
{ 
	attach(&EmptyMapData); 
};

CMap::CMap(const CMap& src) 
{ 
	attach(src.pData); 
};

CMap::CMap(MAPDATA* data) 
{ 
	attach(data); 
};

CMap::~CMap() 
{ 
	detach(pData); 
};


/////////////////////////////////////////////////////////////////////////////
// statický konstruktor a destruktor

void CMap::Init()
{ 
	attach(&EmptyMapData);
};

void _fastcall CMap::Init(const CMap& src) 
{ 
	attach(src.pData); 
};

void _fastcall CMap::Init(MAPDATA* data)
{ 
	attach(data); 
};

bool _fastcall CMap::Init(int width, int height)	// při chybě paměti vrátí FALSE, plocha není vytvořena
{
	pData = NewMapData(width, height);
	return (pData != NULL);
};

void CMap::Term()
{ 
	detach(pData); 
};


/////////////////////////////////////////////////////////////////////////////
// vyprázdnění plochy (uvolnění dat)

void CMap::Empty()
{ 
	Term();
	Init();
}


/////////////////////////////////////////////////////////////////////////////
// vymazání obsahu plochy (naplnění ikonami 0, vynulované parametry), zajistí přivlastnění bufferu,
// při chybě paměti vrací FALSE, původní obsah nezměněn

bool CMap::Clear()
{
// vytvoření nového bufferu, je-li potřeba
	if (!New()) return false;

// vymazání bufferu
	MemFill(pData->Data, pData->Width * pData->Height * SIZEOFMAPITEM, 0);
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// vymazání obsahu plochy s nastavením velikosti (naplnění ikonami 0, vynulované parametry),
// zajistí přivlastnění bufferu, při chybě paměti vrací FALSE, původní obsah nezměněn

bool _fastcall CMap::Clear(int width, int height)
{
// vytvoření nového bufferu, je-li potřeba
	if (!New(width, height)) return false;

// vymazání bufferu
	MemFill(pData->Data, pData->Width * pData->Height * SIZEOFMAPITEM, 0);
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// kopie do vlastního bufferu před modifikací
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool CMap::CopyWrite()
{
// úschova ukazatelů
	MAPDATA* olddata = pData;		// adresa starých dat
	long* refer = &(olddata->Refer);// počet referencí

// test, zda je nutné přivlastnění
	if (*refer > 1)					// je nějaký jiný majitel?
	{

// vytvoření nového bufferu
		MAPDATA* newdata = NewMapData(olddata->Width, olddata->Height);
		if (newdata == NULL) return false;

// přenesení dat
		MemCopy(newdata->Data, olddata->Data, olddata->Width * olddata->Height * SIZEOFMAPITEM);

// odpojení starých dat
		detach(olddata);

// připojení nových dat
		pData = newdata;
	}

// operace OK
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření nové plochy se stejnou velikostí (připraveno k zápisu, data jsou náhodná)
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool CMap::New()
{
// ukazatel na stará data
	MAPDATA* olddata = pData;

// test, zda je nutné vytvoření nového bufferu
	if (olddata->Refer > 1)			// na buffer je více referencí
	{

// vytvoření nového bufferu
		MAPDATA* newdata = NewMapData(pData->Width, pData->Height);
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
// vytvoření nové plochy (připraveno k zápisu, data jsou náhodná)
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool _fastcall CMap::New(int width, int height)
{
// ukazatel na stará data
	MAPDATA* olddata = pData;

// test, zda je nutné vytvoření nového bufferu
	if ((olddata->Refer > 1) ||			// na buffer je více referencí
		(olddata->Width != width) ||	// nesouhlasí šířka
		(olddata->Height != height))	// nesouhlasí výška
	{

// vytvoření nového bufferu
		MAPDATA* newdata = NewMapData(width, height);
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
// změna velikosti plochy
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool CMap::Resize(int width, int height)
{
// úschova starých rozměrů
	int oldwidth = pData->Width;
	int oldheight = pData->Height;

// test, zda je potřeba velikost plochy měnit
	if ((width != oldwidth) || (height != oldheight))
	{

// vytvoření nového bufferu
		MAPDATA* olddata = pData;
		MAPDATA* newdata = NewMapData(width, height);
		if (newdata == NULL) return false;

// příprava počtu řádků ke kopii
		int i = height;
		if (oldheight < i) i = oldheight;

// příprava délky řádku ke kopii
		int j = width;
		if (oldwidth < j) j = oldwidth;

// příprava zbytku řádku k vymazání
		int k = width - j;

// přírustek zdrojové a cílové adresy
		int srcinc = oldwidth * SIZEOFMAPITEM;
		int dstinc = width * SIZEOFMAPITEM;

// příprava zdrojové a cílové adresy
		BYTE* src = (BYTE*)(olddata->Data);
		BYTE* dst = (BYTE*)(newdata->Data);

// kopie platných řádků při shodné délce řádků
		if (width == oldwidth)
		{
			i *= dstinc;
			MemCopy(dst, src, i);
			dst += i;
			src += i;
		}

// kopie platných řádků při rozdílné délce řádků
		else
		{
			j *= SIZEOFMAPITEM;

			if (k == 0)
			{
				for (; i > 0; i--)
				{
					MemCopy(dst, src, j);
					dst += j;
					src += srcinc;
				}
			}
			else
			{
				k *= SIZEOFMAPITEM;

				for (; i > 0; i--)
				{
					MemCopy(dst, src, j);
					dst += j;
					MemFill(dst, k, 0);
					dst += k;
					src += srcinc;
				}
			}
		}

// vymazání zbylých řádků
		i = height - oldheight;
		if (i > 0)
		{
			MemFill(dst, i*dstinc, 0);
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
// poskytnutí prvku (s kontrolou platnosti offsetu/indexu)
// pro neplatný prvek vrací přístup na prvek prázdné plochy

const MAPITEM& _fastcall CMap::Get(const int off) const
{
	if (IsValid(off))				// je offset platný?
	{
		return pData->Data[off];	// prvek na daném offsetu
	}
	return EmptyMapData.Data[0];	// neplatný offset
}

const MAPITEM& _fastcall CMap::Get(const int x, const int y) const
{
	if (IsValid(x, y))				// je index platný?
	{
		return pData->Data[x + y*pData->Width];	// prvek
	}
	return EmptyMapData.Data[0];	// neplatný offset
}


////////////////////////////////////////////////////////////////////
// nastavení prvku (s kontrolou platnosti offsetu/indexu)
// před zápisem je nutno zajistit přivlastnění bufferu!

void _fastcall CMap::Set(const int off, const MAPITEM& data)
{
	if (IsValid(off))				// je offset platný?
	{
		pData->Data[off] = data;
	}
}

void _fastcall CMap::Set(const int x, const int y, const MAPITEM& data)
{
	if (IsValid(x, y))				// je index platný?
	{
		pData->Data[x + y*pData->Width] = data;
	}
}


/////////////////////////////////////////////////////////////////////////////
// zvýšení reference na ikonu (v bufferu globálních a lokálních objektů)

void _fastcall RefIconInc(const int icon)
{
// nalezení ikony v bufferu globálních objektů
	int index = BufObj.SrcDat(BufIcoID, icon);
	if (index >= 0)
	{
		BufObj[index].Refer++;
		return;
	}

// nalezení ikony v bufferu lokálních objektů
	index = BufLoc.SrcDat(BufIcoID, icon);
	if (index >= 0)
	{
		BufLoc[index].Refer++;
		return;
	}

// ikona nenalezena - to je chyba!!!!
	ASSERTERROR;
}


/////////////////////////////////////////////////////////////////////////////
// zvýšení reference na všechny ikony v ploše

void CMap::RefInc()
{
	MAPITEM* item = DataData();

	for (int i = pData->Height*pData->Width; i > 0; i--)
	{
		RefIconInc(item->Icon);
		item++;
	}
}


/////////////////////////////////////////////////////////////////////////////
// snížení reference na ikonu (v bufferu globálních a lokálních objektů)

void _fastcall RefIconDec(const int icon)
{
// nalezení ikony v bufferu globálních objektů
	int index = BufObj.SrcDat(BufIcoID, icon);
	if (index >= 0)
	{
		BufObj[index].Refer--;
		return;
	}

// nalezení ikony v bufferu lokálních objektů
	index = BufLoc.SrcDat(BufIcoID, icon);
	if (index >= 0)
	{
		BufLoc[index].Refer--;
		return;
	}

// ikona nenalezena - to je chyba!!!!
	ASSERTERROR;
}


/////////////////////////////////////////////////////////////////////////////
// snížení reference na všechny ikony v ploše

void CMap::RefDec()
{
	MAPITEM* item = DataData();
	for (int i = pData->Height*pData->Width; i > 0; i--)
	{
		RefIconDec(item->Icon);
		item++;
	}
}


/////////////////////////////////////////////////////////////////////////////
// zrušení použití ikony v ploše (index = index plochy v seznamu) - nahradí ikonou 0
// ignoruje případnou chybu paměti!

void _fastcall CMap::DelIcon(int icon, int index, bool undo)
{
// nesmí to být ikona 0 (=základní ikona)
	ASSERT(icon > 0);
	if (icon <= 0) return;

// ukazatel položek
	MAPITEM* item = DataData();

// ukazatel indexu položky v ploše
	int off = 0;

// příznak provedení kopie bufferu
	bool copy = false;

// cyklus přes všechny položky plochy
	for (int i = pData->Height*pData->Width; i > 0; i--)
	{

// je to hledaná ikona?
		if (item->Icon == icon)
		{

// zajištění přivlastnění bufferu
			if (!copy)
			{
				if (!CopyWrite()) return;
				item = DataData() + off;
				copy = true;
			}

// snížení reference na ikonu z bufferů
			RefIconDec(icon);

// záznam o zrušení ikony do UNDO bufferu
			if (undo)
			{
				if (!Undo.AddMapIcoSet(index, off, icon))
				{
					Undo.DelAll();
				}
			}

// nastavení nové ikony
			item->Icon = 0;

// zvýšení reference na novou ikonu
			RefIconInc(0);
		}

// zvýšení ukazatele položek
		item++;
		off++;
	}
}


/////////////////////////////////////////////////////////////////////////////
// načtení plochy ze souboru (TRUE=operace OK, při chybě obsah nezměněn)

bool CMap::LoadFile(CText jmeno)
{
// otevření souboru mapovaného do paměti (uzavřen při destrukci!)
	CFileMap file;
	if (!file.Open(jmeno)) return false;

// kontrola velikosti souboru
	int size = file.Size();
	if (size < 100) return false;

// načtení základních parametrů
	MAPFILE* map = (MAPFILE*)file.Adr();
	int verze = map->Verze;			// verze souboru
	int jazyku = map->Jazyku;		// počet jazyků
	if (verze < 1) jazyku = 1;		// pro verzi 0 bude 1 jazyk
	int width = map->Width;			// šířka plochy
	int height = map->Height;		// výška plochy
	int pals = map->Colors;			// počet palet

// kontrola platnosti souboru
	if ((map->Ident[0] != 'P') ||
		(map->Ident[1] != 'M') ||
		(map->Ident[2] != 'A') ||
		(map->Ident[3] != 'P') ||
		(width < 1) ||
		(width > MAPMAXWIDTH) ||
		(height < 1) ||
		(height > MAPMAXHEIGHT) ||
		(pals < 2) ||
		(pals > 256) ||
		(size < width*height*8 + pals*4 + 30))
	{
		return false;
	}

// příprava konverzní tabulky palet
	BITMAPINFO* bmp = (BITMAPINFO*)MemGet(sizeof(BITMAPINFO) + pals*4 + 200);
	if (bmp == NULL) return false;
	bmp->bmiHeader.biClrUsed = pals;
	bmp->bmiHeader.biBitCount = 8;
	MemCopy(bmp->bmiColors, map->ColorTab, pals*4);

// barva 0 bude jako pozadí
	bmp->bmiColors[0] = StdBitmapInfo->bmiColors[0];

// barva 1 bude jako stín
	bmp->bmiColors[1] = StdBitmapInfo->bmiColors[1];

	GenKonvPal(bmp);
	MemFree(bmp);

// příprava počtu referencí
	BYTE* src = file.Adr() + SIZEOFMAPFILE + pals*4;
	int refs = *(int*)src;
	src += 4;
	size -= SIZEOFMAPFILE + pals*4 + 4;

// kontrola počtu referencí
	if ((refs < 1) || (refs > width*height)) return false;

// vytvoření bufferu referencí
	CBufIndex inx;
	if (!inx.NumClear(refs)) return false;

// záloha aktuální plochy (zrušeno destruktorem při ukončení funkce)
	CMap backup(pData);

// vytvoření nové plochy (vymazání na ikony 0)
	if (!Clear(width, height)) return false;

// načtení referencí
	CMultiText txt;					// text reference
	CText txt0;
	CIcon icon;						// ikona, rozměry ICONSIZE

// cyklus přes všechny ikony
	for (int i = 0; i < refs; i++)
	{
		txt.Empty();

// cyklus přes všechny jazyky
		for (int j = 0; j < jazyku; j++)
		{

// kontrola dat
			if (size < 3*4 + ICONSIZE)
			{
				*this = backup;
				return false;
			}

// načtení parametrů textu
			int id = JazykInfo[JazykDef].LangID & 0xff;
			int cp = 0;

			if (verze >= 1)
			{
				id = *(long*)src & 0xff;	// identifikátor jazyku
				src += 4;
				size -= 4;
				if (id == 0) id = (JazykInfo[JazykDef].LangID & 0xff);

				cp = *(long*)src;			// kódová stránka
				src += 4;
				size -= 4;
			}

// načtení délky textu
			int len = *(long*)src;			// délka textu ve znacích
			if (len < 0) len = 0;
			src += 4;						// posun zdrojové adresy
			size -= 4;						// snížení zbývajících dat
			int len0 = len;					// délka textu v bajtech
			if (cp == 0) len0 = 2*len;		// délka textu v bajtech pro UNICODE

// kontrola délky textu a ikony
			if (((DWORD)len > 1000000) ||
				(len0 + ICONSIZE > size) ||
				(size <= 0))
			{
				*this = backup;
				return false;
			}

// načtení textu
			if (cp == 0)
			{
				if (!txt0.Set((WCHAR*)src, len))
				{
					*this = backup;
					return false;
				}
			}
			else
			{
				if (!txt0.Set((char*)src, len))
				{
					*this = backup;
					return false;
				}
			}
			txt0.KorigNul();
			src += len0;
			size -= len0;

// vyhledání ID jazyku
			for (int lan = 0; lan < JAZYKNUM; lan++)
			{
				if (lan != JAZYK000)
				{
					if ((JazykInfo[lan].LangID & 0xff) == id)
					{
						txt.MultiText(lan, txt0);
						break;
					}
				}
			}
		}

// načtení ikony
		if (!icon.CopyKonvData(src))
		{
			*this = backup;
			return false;
		}
		src += ICONSIZE;
		size -= ICONSIZE;

// nalezení objektu v některém z bufferů
		int reffrst = -1;
		int refnext = -1;
		CBufProg* bf = &BufObj;				// bude se hledat v globálním bufferu
		int index = bf->FindObj(txt, -1, -1, -1); // nalezení reference

		if (index < 0)					// nebyla položka nalezena?
		{
			bf = &BufLoc;				// bude se hledat v lokálním bufferu
			reffrst = bf->Disp();		// hledaná první lokální položka
			if (bf->IsValid(reffrst))	// je lokální položka platná?
			{
				refnext = bf->At(reffrst).Next;	// poslední hledaná položka
			}
			index = bf->FindObj(txt, -1, reffrst, refnext);
		}

// pokus o nalezení objektu správného typu
		int nn = 1;
		while (bf->IsValid(index) && (bf->At(index).Func != IDF_ICON)
				&& (bf->At(index).Func != IDF_NONE))
		{
			nn++;
			txt.SetNumObj(nn);
			index = bf->FindObj(txt, -1, reffrst, refnext);
		}
		
// objekt nenalezen - bude se vytvářet (v globálním bufferu)
		if (bf->IsNotValid(index))
		{
			bf = &BufObj;

			int ic = Icon.Add(icon);
			if (ic < 0)
			{
				*this = backup;
				return false;
			}

			int tx = Text.Add(txt);
			if (tx < 0)
			{
				Icon.Del(ic);
				*this = backup;
				return false;
			}

			index = bf->Copy(-1, -2, BufClsID, CLASSICONINDEX);

			if (bf->IsValid(index))
			{

// nastavení ikony
				ASSERT(bf->At(index).Func == IDF_ICON);
				if (bf->At(index).Icon >= 0)
				{
					Icon.Del(bf->At(index).Icon);
				}
				bf->At(index).Icon = ic;

				bf->At(index).DatIndex = bf->At(index).Icon;
				bf->At(index).DatBlok = BufIcoID;

// zajištění jedinečnosti jména nové ikony
				reffrst = BufLoc.Disp();		// hledaná první lokální položka
				refnext = -1;
				if (BufLoc.IsValid(reffrst))	// je lokální položka platná?
				{
					refnext = BufLoc.At(reffrst).Next;	// poslední hledaná položka
				}

				nn = 0;
				while ((bf->FindObj(txt, index, -1, -1) >= 0) ||
					(BufLoc.FindObj(txt, -1, reffrst, refnext) >= 0))
				{
					nn++;
					txt.SetNumObj(nn);
				}

// nastavení jména nového objektu
				bf->At(index).Name = tx;

				if (bf->At(index).HTree)
				{
					TV_ITEM tvi;
					tvi.mask = TVIF_TEXT;
					tvi.hItem = bf->At(index).HTree;
					tvi.pszText = (LPTSTR)(LPCTSTR)txt.MultiText();
					::SendMessage(bf->Tree(), TVM_SETITEM, 0, (LPARAM)&tvi);
				}
			}
			else
			{
				Text.Del(tx);
				Icon.Del(ic);
				*this = backup;
				return false;
			}
		}

// úschova indexu ikony
		inx[i] = bf->GetIcon(index);
		if (inx[i] < 0) inx[i] = 0;
	}

// přenesení ikon
	for (i = 0; i < width*height; i++)
	{
		if (size < 8) break;

		int n = *(int*)src;
		if ((DWORD)n < (DWORD)refs)
		{
			pData->Data[i].Icon = inx[n];
		}
		else
		{
			pData->Data[i].Icon = 0;
		}
		pData->Data[i].Param = *(DWORD*)(src + 4);

		src += 8;
		size -= 8;
	}

// příznak - načteno OK
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// uložení do souboru (FALSE=chyba)

bool CMap::SaveFile(CText jmeno) const
{
// vytvoření souboru
	CFile file;
	file.Name(jmeno);
	if (!file.Create()) return false;

// příprava bufferu záhlaví
	int bufsize = SIZEOFMAPFILE + StdColors*4 + 4 + 10000;
	BYTE* buf = (BYTE*)MemGet(bufsize);
	if (buf == NULL)
	{
		file.Close();
		file.Delete();
		return false;
	}
	
// vyplnění záhlaví souboru
	MAPFILE* map = (MAPFILE*)buf;
	map->Ident[0] = 'P';
	map->Ident[1] = 'M';
	map->Ident[2] = 'A';
	map->Ident[3] = 'P';
	map->Verze = 1;
	map->Jazyku = JAZYKNUM - 1;			// bez JAZYK000
	map->Param = 0;
	map->Width = pData->Width;
	map->Height = pData->Height;
	map->Colors = StdColors;

// přenesení palet
	MemCopy(map->ColorTab, StdBitmapInfo->bmiColors, StdColors*4);

// offset čítače ikon
	int iconn = SIZEOFMAPFILE + StdColors*4;
	*(long*)(buf + iconn) = 0;

// příprava k uložení definic ikon
	int size = SIZEOFMAPFILE + StdColors*4 + 4;	// počet bajtů v bufferu

	CBufIndex inx;					// buffer mapování indexů ikon
	if (!inx.NumClear(Icon.Max()))
	{
		MemFree(buf);
		file.Close();
		file.Delete();
		return false;
	}

	int icons = pData->Width * pData->Height;	// počet ikon v ploše celkem
	CMultiText txt;								// jméno ikony
	CText txt0;

// cyklus přes všechny ikony plochy
	for (int i = 0; i < icons; i++)
	{

// ikona plochy
		int icon = pData->Data[i].Icon;
		ASSERT(Icon.IsValid(icon));
		if (Icon.IsNotValid(icon)) icon = 0;

// kontrola, zda je ikona již mapovaná
		if (inx[icon] < 0)
		{

// index mapované ikony
			inx[icon] = *(long*)(buf + iconn);
			(*(long*)(buf + iconn))++;

// nalezení reference na ikonu
			CBufProg* bf = &BufObj;
			int ref = bf->SrcDat(BufIcoID, icon);

			if (ref < 0)
			{
				bf = &BufLoc;
				ref = bf->SrcDat(BufIcoID, icon);
			}
			ASSERT(ref >= 0);

// příprava jména ikony (pro neplatnou ikonu vrátí prázdný řetězec)
			txt = Text.Get(bf->GetText(ref));

// cyklus přes všechny jazyky
			for (int lan = 0; lan < JAZYKNUM; lan++)
			{
				if (lan != JAZYK000)
				{
					txt0 = txt.MultiText(lan);
					int len = txt0.Length();

// zvýšení velikosti bufferu
					if (bufsize < size + 3*4 + len + ICONSIZE)
					{
						bufsize = size + 3*4 + len + ICONSIZE + 10000;
						BYTE* buf2 = (BYTE*)MemSize(buf, bufsize);
						if (buf2 == NULL)
						{
							MemFree(buf);
							file.Close();
							file.Delete();
							return false;
						}
						buf = buf2;
					}

// uložení parametrů a textu jména ikony
					*(long*)(buf + size) = JazykInfo[lan].LangID;
					size += 4;
					*(long*)(buf + size) = JazykInfo[lan].CodePage;
					size += 4;
					*(long*)(buf + size) = len;
					size += 4;
					txt0.Write((char*)(buf + size));
					size += len;
				}
			}

// uložení ikony (pro neplatnou ikonu uloží prázdnou ikonu)
			Icon.Get(bf->GetIcon(ref)).DeComp();
			MemCopy(buf + size, Icon.Get(bf->GetIcon(ref)).DataData(), ICONSIZE);
			size += ICONSIZE;
		}
	}

// zvýšení velikosti bufferu pro data plochy
	if (bufsize < size + 8*icons)
	{
		bufsize = size + 8*icons;
		BYTE* buf2 = (BYTE*)MemSize(buf, bufsize);
		if (buf2 == NULL)
		{
			MemFree(buf);
			file.Close();
			file.Delete();
			return false;
		}
		buf = buf2;
	}

// uložení plochy
	for (i = 0; i < icons; i++)
	{
		int icon = pData->Data[i].Icon;
		if (Icon.IsNotValid(icon)) icon = 0;
		*(long*)(buf + size) = inx[icon];
		size += 4;
		*(DWORD*)(buf + size) = pData->Data[i].Param;
		size += 4;
	}

// uložení souboru
	BOOL result = file.Write(buf, size);

// uzavření souboru
	file.Close();

// zrušení bufferu
	MemFree(buf);

// při chybě zápisu zrušení souboru
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

const CMap& CMap::operator= (const CMap& src)
{
	detach(pData);				// zrušení starých dat
	attach(src.pData);			// přiřazení nových dat
	return *this;
}

const CMap& CMap::operator= (MAPDATA* src)
{
	detach(pData);				// zrušení starých dat
	attach(src);				// přiřazení nových dat
	return *this;
}


/***************************************************************************\
*																			*
*								Buffer ploch								*
*																			*
\***************************************************************************/

////////////////////////////////////////////////////////////////////
// přidání záznamu o vložení položky do UNDO bufferu

template <> bool CBuffer<CMap>::UndoAddIns(int index)
{
	return Undo.AddMapIns(index);
}

////////////////////////////////////////////////////////////////////
// vytvoření nové plochy (vrací index položky, <0 = chyba)
// provádí záznam do UNDO bufferu

int _fastcall CBufMap::New(int width, int height)
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
// přidání položky (vrací index položky, <0 = chyba)
// provádí záznam do UNDO bufferu

int _fastcall CBufMap::Add(MAPDATA* data)
{
	CMap map(data);
	int result = Add(map);			// result musí být oddělen, pro inline by se špatně volal destruktor
	return result;
}

/////////////////////////////////////////////////////////////////////////////
// zrušení použití ikony ze všech ploch - nahradí ikonou 0
// ignoruje případnou chybu paměti!!!

void _fastcall CBufMap::DelIcon(int icon)
{
	ASSERT(icon > 0);

	for (int i = m_Max - 1; i >= 0; i--)
	{
		if (m_Valid[i])
		{
			m_Data[i].DelIcon(icon, i, m_Undo);
		}
	}
}
