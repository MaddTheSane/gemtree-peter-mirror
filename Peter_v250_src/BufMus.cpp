
#include "Main.h"

/***************************************************************************\
*																			*
*									Hudba									*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// inicializační prázdná hudba (modifikuje se počet referencí!)

BYTE	EmptyMusicData0 = 0;

MUSICDATA EmptyMusicData = { 1, 0, 0, &EmptyMusicData0 };
const CMusic		EmptyMusic;				// prázdná hudba


/////////////////////////////////////////////////////////////////////////////
// vytvoření dat hudby (při chybě paměti vrací NULL)

MUSICDATA* _fastcall NewMusicData(int size)
{
	ASSERT(size >= 0);

// vytvoření záhlaví hudby
	MUSICDATA* data = (MUSICDATA*)MemGet(SIZEOFMUSICDATA);
	if (data != NULL)
	{

// nastavení dat hudby
		data->Refer = 1;				// počet referencí
		data->Size = size;				// velikost dat

// vytvoření bufferu dat hudby
		BYTE* datadata = (BYTE*)MemGet(size);
		data->Data = datadata;

// při chybě paměti zrušení záhlaví hudby
		if (datadata == NULL)
		{
			MemFree(data);
			data = NULL;
		}
	}
	return data;
}

/////////////////////////////////////////////////////////////////////////////
// zrušení dat hudby (odděleno kvůli lepší optimalizaci)

void _fastcall DelMusicData(MUSICDATA* data)
{
	ASSERT(data != NULL);
	ASSERT(data->Data != NULL);

	MemFree(data->Data);		// zrušení dat hudby
	MemFree(data);				// zrušení záhlaví hudby
}

/////////////////////////////////////////////////////////////////////////////
// konstruktor a destruktor

CMusic::CMusic() 
{ 
	attach(&EmptyMusicData); 
};

CMusic::CMusic(const CMusic& src) 
{ 
	attach(src.pData); 
};

CMusic::CMusic(MUSICDATA* data) 
{ 
	attach(data); 
};

CMusic::~CMusic() 
{ 
	detach(pData);
};


/////////////////////////////////////////////////////////////////////////////
// statický konstruktor a destruktor

void CMusic::Init()
{ 
	attach(&EmptyMusicData); 
};

void CMusic::Init(const CMusic& src) 
{ 
	attach(src.pData); 
};

void CMusic::Init(MUSICDATA* data)
{ 
	attach(data); 
};

bool CMusic::Init(int size)		// při chybě paměti vrací FALSE, hudba není vytvořena
{
	pData = NewMusicData(size);
	return (pData != NULL);
};

void CMusic::Term()
{ 
	detach(pData); 
};


/////////////////////////////////////////////////////////////////////////////
// vyprázdnění hudby (uvolnění dat) - připojí se standardní prázdná hudba

void CMusic::Empty()
{ 
	Term();
	Init();
}


/////////////////////////////////////////////////////////////////////////////
// kopie do vlastního bufferu před modifikací
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool CMusic::CopyWrite()
{
// úschova ukazatelů
	MUSICDATA* olddata = pData;			// adresa starých dat
	long* refer = &(olddata->Refer);	// počet referencí

// test, zda je nutné přivlastnění
	if (*refer > 1)						// je nějaký jiný majitel?
	{

// vytvoření nového bufferu
		MUSICDATA* newdata = NewMusicData(olddata->Size);
		if (newdata == NULL) return false;

// přenesení dat
		MemCopy(newdata->Data, olddata->Data, olddata->Size);

// odpojení starých dat
		detach(olddata);

// připojení nových dat
		pData = newdata;
	}

// operace OK
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření nové hudby (připraveno k zápisu, data jsou náhodná)
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool CMusic::New(int size)
{
// ukazatel na stará data
	MUSICDATA* olddata = pData;

// test, zda je nutné vytvoření nového bufferu
	if ((olddata->Refer > 1) ||		// na buffer je více referencí
		(olddata->Size != size))	// nesouhlasí velikost
	{

// vytvoření nového bufferu
		MUSICDATA* newdata = NewMusicData(size);
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
// načtení hudby ze souboru (TRUE=operace OK)

bool CMusic::LoadFile(CText jmeno)
{
// otevření souboru mapovaného do paměti (uzavřen při destrukci!)
	CFileMap file;
	if (!file.Open(jmeno)) return false;

// vytvoření nového bufferu
	MUSICDATA* newdata = NewMusicData(file.Size());
	if (newdata == NULL) return false;

// přenesení dat do bufferu
	MemCopy(newdata->Data, file.Adr(), file.Size());

// odpojení starých dat
	detach(pData);

// připojení nových dat
	pData = newdata;

// příznak operace OK
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// uložení do souboru formátu MID (FALSE=chyba)

bool CMusic::SaveFile(CText jmeno) const
{
// vytvoření souboru
	CFile file;
	file.Name(jmeno);
	if (!file.Create()) return false;

// uložení souboru
	BOOL result = file.Write(pData->Data, pData->Size);

// uzavření souboru
	file.Close();

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

const CMusic& CMusic::operator= (const CMusic& src)
{
	detach(pData);			// zrušení starých dat
	attach(src.pData);		// přiřazení nových dat
	return *this;
}

const CMusic& CMusic::operator= (MUSICDATA* src)
{
	detach(pData);			// zrušení starých dat
	attach(src);			// přiřazení nových dat
	return *this;
}


/***************************************************************************\
*																			*
*								Buffer hudby								*
*																			*
\***************************************************************************/

////////////////////////////////////////////////////////////////////
// přidání záznamu o vložení položky do UNDO bufferu

template <> bool CBuffer<CMusic>::UndoAddIns(int index)
{
	return Undo.AddMusIns(index);
}

////////////////////////////////////////////////////////////////////
// vytvoření nové položky - obsah náhodný (vrací index položky, <0 = chyba)
// provádí záznam do UNDO bufferu

int CBufMusic::New(int size)
{
// vytvoření nové položky
	int result = New();				// vytvoření nové položky
	if (result >= 0)
	{

// inicializace objektu
		if (!m_Data[result].New(size))
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

int _fastcall CBufMusic::Add(MUSICDATA* data)
{
	CMusic mus(data);
	int result = Add(mus);		// result musí být oddělen, pro inline by se špatně volal destruktor
	return result;
}
