
#include "Main.h"

/***************************************************************************\
*																			*
*								Obsluha paměti								*
*																			*
\***************************************************************************/

/*
Organizace správce paměti:
--------------------------
Pro dosažení maximální rychlosti přidělování a uvolňování datových bloků se
datové bloky nepřidělují se skutečnou velikostí, ale přidělí se blok o velikosti
nejbližší vyšší mocniny 2. Používají se bloky o velikosti 8, 16, 32, 64, 128,
256, 512, 1024, 2048 a 4096 Bajtů - tedy 10 typů bloků (pro procesory Alpha 
ještě 8192 Bajtů). Datové bloky o stejné velikosti jsou organizovány do datových
stránek s evidencí volných bloků. To umožňuje snadné a rychlé uvolňování a 
přidělování bloků bez nebezpečí neefektivní fragmentace paměti.

Při přidělování nového datového bloku se použije tabulka popisovačů datových
modulů. Každý popisovač popisuje jeden typ (velikost) datového bloku, je tedy
použito 10 (pro Alpha 11) popisovačů. Popisovač pro příslušnou velikost datového
bloku obsahuje ukazatel na první volný datový blok dané velikosti. Na začátku
každého volného datového bloku je ukazatel na další a předcházející volný datový
blok stejné velikosti. Je-li datový blok použit, vyřadí se z evidence volných
bloků přesměrováním ukazatelů mimo blok.

Není-li k dispozici žádný další volný datový blok, vytvoří se nová stránka
(stejných) datových bloků. Použije se buď rezervní stránka uvolněná při
předešlém rušení nebo je vytvořena nová stránka.

Oblast celého adresového prostoru (2 GB) je rozdělena na oblasti - regiony.
Pro x86 je velikost regionu 1 MB, adresový prostor tedy zahrnuje 2048 regionů.
Pro každý region si správce paměti eviduje, zda je použit či zda je volný,
Při požadavku přidělení další paměti je od systému vyžádána rezervace
regionu, který by měl být podle evidence volný (tj. je požadován určitý adresový
prostor). Pokud systém tento prostor používá, je požadována další adresa až
do nalezení volného prostoru.

Region je rozdělen do stránek (pro x86 velikost 4 KB). První stránka je vždy
evidenční stránka. Obsahuje čítač použitých stránek a ukazatele pro spojení
datových stránek do řetězce volných stránek. Při přidělení nové datové stránky
je stránka odpojena z řetězce volných datových stránek a od systému je vyžádáno,
aby na adresu stránky připojil fyzickou paměť. Poté jsou všechny datové bloky
na stránce zařazeny do řetězce datových bloků pro požadovanou velikost bloku.
*/

/////////////////////////////////////////////////////////////////////////////
// obsluha vícetokového režimu

#ifdef _MT
#define HEAPSERIALIZE 0					// pro vícetokový režim je serializace
#else
#define HEAPSERIALIZE HEAP_NO_SERIALIZE	// vypnutí serializace, není-li vícetokový režim
#endif

#ifdef _MT
static	CRITICAL_SECTION	MemoryCriticalSection;	// kritická sekce pro správce paměti
#endif

/////////////////////////////////////////////////////////////////////////////
// debugger

#ifndef ASSERT

#ifdef _M_IX86
#define debugbreak _asm{int 3}
#else	// _M_IX86
#define debugbreak DebugBreak()
#endif	// _M_IX86

#if defined(_DEBUG) && !defined(_OPTIM)
#define ASSERT(f)			{ if (!(f)) debugbreak; }
#define VERIFY(f)			ASSERT(f)
#define ASSERTERROR			debugbreak
#else	// _DEBUG _OPTIM
#define ASSERT(f)			((void)0)
#define VERIFY(f)			((void)(f))
#define ASSERTERROR			((void)0)
#endif	// _DEBUG _OPTIM

#endif	// ASSERT

/////////////////////////////////////////////////////////////////////////////
// konstanty - velikosti musí být mocnina 2 !

#ifndef _M_ALPHA
#define	PAGESIZE		0x1000			// velikost alokační stránky pro ostatní procesory (4096)
#else
#define	PAGESIZE		0x2000			// velikost alokační stránky pro procesor Alpha (8192)
#endif

#define PAGEITEMS		(PAGESIZE/sizeof(ITEM))	// počet prvků na stránku (512 popř. 1024)
#define	REGPAGES		(PAGESIZE/sizeof(PAGE)) // počet stránek na region (256 popř. 512)
#define DATAPAGES		(REGPAGES-1)			// počet datových stránek na region (255 popř. 511)
#define REGSIZE			(PAGESIZE*REGPAGES)		// velikost regionu (1 MB popř. 4 MB)
#define NUMREGS			(0x78000000/REGSIZE-1)	// počet regionů v paměti - bez systémových regionů (2039 popř. 509)
#define REGSYSTEM		(0x800000/REGSIZE)		// počet systémových regionů na začátku (8 popř. 2)
#define MAXREGS			((0x40000000/REGSIZE)*4)// počet regionů celkem (4096 popř. 1024)

/////////////////////////////////////////////////////////////////////////////
// struktury

typedef struct ITEM_ ITEM;
typedef struct DATA_ DATA;
typedef struct PAGE_ PAGE;
typedef struct MODUL_ MODUL;
typedef struct REGION_ REGION;

// struktura datového prvku - 8 bajtů
struct ITEM_
{
	ITEM*		NextItem;				// adresa dalšího volného prvku (NULL=konec)
	ITEM*		PrevItem;				// adresa předešlého volného prvku (NULL=začátek)
};

// struktura dat stránky - velikost PAGESIZE
struct DATA_
{
	ITEM		Items[PAGEITEMS];		// data jedné stránky
};

// struktura popisovače stránky - velikost 16 bajtů
// U volné stránky je neplatný ukazatel modulu, u použité ukazatele volných stránek
struct PAGE_
{
	PAGE*		NextPage;				// adresa další stránky (NULL=konec)
	PAGE*		PrevPage;				// adresa předešlé stránky (NULL=začátek)
	MODUL*		Modul;					// adresa popisovače modulu
	long		Used;					// čítač použitých položek
};

// struktura popisovače modulu
struct MODUL_
{
	ITEM*		FreeItem;				// adresa volného prvku (NULL=není)
	PAGE*		ResPage;				// rezervní stránka (NULL=není)
	long		Size;					// velikost datové položky
};

// struktura regionu - velikost REGSIZE
struct REGION_
{
	long		Used;					// čítač použitých stránek
	long		res[3];					// rezerva pro zarovnání na velikost popisovače stránky
	PAGE		Pages[DATAPAGES];		// popisovače stránek
	DATA		Data[DATAPAGES];		// data (datové stránky)
};


/////////////////////////////////////////////////////////////////////////////
// proměnné

BOOL MemoryError = FALSE;				// příznak chyby paměti

BOOL MemoryOK = FALSE;					// příznak platného správce paměti

static	HANDLE	hHeap = NULL;			// handle haldy

// obsluha regionů
static	bool*	UsedRegion;				// buffer příznaků použití regionů
static	int		NextRegion = REGSYSTEM;	// index příště přidělovaného regionu
static	REGION*	ResRegion = NULL;		// rezervní region (NULL=není)

// obsluha nepřipojených stránek (následuje řetězec pomocí NextPage)
static	PAGE*	FreePage = NULL;		// první nepřipojená stránka (NULL=není)

// popisovače datových modulů
static	MODUL	Modul[12] = {
					NULL,	NULL,	0x0008,
					NULL,	NULL,	0x0010,
					NULL,	NULL,	0x0020,
					NULL,	NULL,	0x0040,
					NULL,	NULL,	0x0080,
					NULL,	NULL,	0x0100,
					NULL,	NULL,	0x0200,
					NULL,	NULL,	0x0400,
					NULL,	NULL,	0x0800,
					NULL,	NULL,	0x1000,
					NULL,	NULL,	0x2000,
					NULL,	NULL,	0x4000 };	// modul 0x4000 je jen pro rezervu


/////////////////////////////////////////////////////////////////////////////
// vytvoření rezervního regionu (při chybě paměti vrací FALSE)

static BOOL NewRegion()
{
// nalezení nepoužitého regionu
	for (int i = NUMREGS;  i > 0; i--)
	{

// adresa zkoušeného regionu
		REGION* region = (REGION*)(NextRegion*REGSIZE); // adresa regionu
		bool* used = UsedRegion + NextRegion;	// příznak použití regionu

// posun indexu příštího regionu
		NextRegion++;				// zvýšení indexu regionu
		if (NextRegion >= REGSYSTEM+NUMREGS)
		{
			NextRegion = REGSYSTEM;
		}
		
// test, zda je region použitý		
		if (!*used)
		{

// rezervování nového regionu
			REGION* newregion = (REGION*)::VirtualAlloc(region,
					REGSIZE, MEM_RESERVE, PAGE_READWRITE);

// vytvoření první stránky pro popisovač regionu
			if (newregion != NULL)
			{
				if ((newregion != region) || (::VirtualAlloc(region, PAGESIZE,
						MEM_COMMIT, PAGE_READWRITE) == NULL))
				{

// při chybě uvolnění regionu, bude pokračováno dalším regionem
					::VirtualFree(newregion, 0, MEM_RELEASE);
				}
				else

// příznak použití regionu
				{
					*used = true;		// příznak použití regionu
					ResRegion = region;	// bude teď jako rezervní region
					region->Used = 0;	// čítač použitých stránek

// napojení stránek do řetězce nepřipojených stránek
					PAGE* page = NULL;
					PAGE* nextpage = region->Pages;

					for (i = DATAPAGES; i > 0; i--)
					{
						PAGE* prevpage = page;
						page = nextpage;
						nextpage++;
						page->PrevPage = prevpage;
						page->NextPage = nextpage;
					}

					page->NextPage = FreePage;
					FreePage = region->Pages;
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// zrušení rezevního regionu (stránky musí být uvolněny a odpojeny!)

static void DelRegion()
{
// kontrola, zda je rezervní region
	REGION* region = ResRegion;
	if (region == NULL) return;
	ResRegion = NULL;

// odpojení stránek z řetězce nepřipojených stránek
	PAGE* page = region->Pages;
	for (int i = DATAPAGES; i > 0; i--)
	{
		PAGE* nextpage = page->NextPage;
		PAGE* prevpage = page->PrevPage;

		if (prevpage != NULL)
		{
			prevpage->NextPage = nextpage;
		}
		else
		{
			FreePage = nextpage;
		}

		if (nextpage != NULL)
		{
			nextpage->PrevPage = prevpage;
		}
		page++;
	}

// zrušení příznaku použití regionu
	UsedRegion[(DWORD)region/REGSIZE] = false;

// odpojení stránky popisovače regionu
	VERIFY (::VirtualFree(region, PAGESIZE, MEM_DECOMMIT));

// uvolnění regionu
	VERIFY (::VirtualFree(region, 0, MEM_RELEASE));
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření rezervní stránky pro daný modul (při chybě paměti vrací FALSE)

static BOOL NewPage(MODUL* modul)
{
// vytvoření nového regionu, není-li další volná stránka
	if (FreePage == NULL)
	{
		if (!NewRegion())
		{
			return FALSE;
		}
	}

// určení adresy dat připojované stránky
	PAGE* page = FreePage;				// připojovaná stránka
	REGION* region = (REGION*)((DWORD)page & ~(REGSIZE-1)); // adresa regionu
	DATA* data = (DATA*)((DWORD)region + 
		((DWORD)page & (REGSIZE-1)) * (sizeof(DATA)/sizeof(PAGE))); // data

// připojení dat stránky
	if (::VirtualAlloc(data, PAGESIZE,
			MEM_COMMIT, PAGE_READWRITE) == NULL)
	{
		return FALSE;
	}

// inkremetace použitých stránek regionu
	region->Used++;						// inkrementace použitých stránek

// zrušení rezervace regiony
	if (region == ResRegion)			// je to rezervní region?
	{
		ResRegion = NULL;				// zrušení rezervního regionu
	}

// vyřazení stránky z řetězce nepřipojených stránek
	PAGE* nextpage = page->NextPage;
	FreePage = nextpage;
	if (nextpage != NULL)
	{
		nextpage->PrevPage = NULL;
	}

// napojení do řetězce volných položek modulu
	int size = modul->Size;				// velikost datové položky
	ITEM* nextitem = data->Items;		// ukazatel adresy prvku
	ITEM* item = NULL;					// není předešlý prvek
	for (int i = PAGESIZE/size; i > 0; i--)
	{
		ITEM* previtem = item;
		item = nextitem;
		nextitem = (ITEM*)((char*)nextitem + size);
		item->PrevItem = previtem;
		item->NextItem = nextitem;
	}
	item->NextItem = modul->FreeItem;
	modul->FreeItem = data->Items;

// inicializace položek popisovače stránky
	page->Modul = modul;				// adresa modulu
	page->Used = 0;						// počet použitých položek

// nastavení stránky jako rezervní
	modul->ResPage = page;				// bude teď jako rezervní stránka
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// zrušení rezervní stránky (všechny položky musí být uvolněny!)

static void DelPage(MODUL* modul)
{
// kontrola, zda je rezervní stránka
	PAGE* page = modul->ResPage;
	if (page == NULL) return;
	modul->ResPage = NULL;

// příprava adresy regionu a dat
	REGION* region = (REGION*)((DWORD)page & ~(REGSIZE-1)); // adresa regionu
	DATA* data = (DATA*)((DWORD)region + 
		((DWORD)page & (REGSIZE-1)) * (sizeof(DATA)/sizeof(PAGE))); // data

// odpojení z řetězce volných položek
	int size = modul->Size;
	ITEM* item = data->Items;
	for (int i = PAGESIZE/size; i > 0; i--)
	{
		ITEM* nextitem = item->NextItem;
		ITEM* previtem = item->PrevItem;

		if (previtem != NULL)
		{
			previtem->NextItem = nextitem;
		}
		else
		{
			modul->FreeItem = nextitem;
		}

		if (nextitem != NULL)
		{
			nextitem->PrevItem = previtem;
		}
		item = (ITEM*)((char*)item + size);
	}

// odpojení dat stránky
	VERIFY (::VirtualFree(data, PAGESIZE, MEM_DECOMMIT));

// napojení stránky do řetězce nepřipojených stránek
	page->PrevPage = NULL;
	page->NextPage = FreePage;
	if (FreePage != NULL)
	{
		FreePage->PrevPage = page;
	}
	FreePage = page;
	
// snížení čítače použitých stránek regionu, příp. uvolnění regionu
	region->Used--;
	if (region->Used == 0)
	{
		if (ResRegion != NULL)
		{
			DelRegion();		// uvolnění rezervního regionu
		}
		ResRegion = region;		// je to nyní rezervní region
	}
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření nového datového prvku pro daný modul (při chybě paměti vrací NULL)

static ITEM* NewItem(MODUL* modul)
{
// vytvoření nové stránky, není-li volný prvek
	if (modul->FreeItem == NULL)
	{
		if (!NewPage(modul))
		{
			return NULL;
		}
	}

// určení adresy popisovače stránky
	ITEM* item = modul->FreeItem;		// adresa prvku
	REGION* region = (REGION*)((DWORD)item & ~(REGSIZE-1)); // adresa regionu
	DATA* data = (DATA*)((DWORD)item & ~(PAGESIZE-1)); // data
	PAGE* page = (PAGE*)((DWORD)region +
		((DWORD)data & (REGSIZE-1)) / (sizeof(DATA)/sizeof(PAGE))); // stránka

// inkrementace použitých položek stránky
	page->Used++;						// inkrementace použitých položek

// zrušení rezervace stránky
	if (page == modul->ResPage)			// je to rezervní stránka?
	{
		modul->ResPage = NULL;			// zrušení rezervní stránky
	}

// vyřazení prvku z řetězce volných prvků
	ITEM* nextitem = item->NextItem;	// adresa dalšího prvku
	modul->FreeItem = nextitem;
	if (nextitem != NULL)
	{
		nextitem->PrevItem = NULL;
	}
	return item;
}


/////////////////////////////////////////////////////////////////////////////
// zrušení datového prvku

static void DelItem(MODUL* modul, ITEM* item)
{
// příprava adres popisovačů
	REGION* region = (REGION*)((DWORD)item & ~(REGSIZE-1)); // adresa regionu
	DATA* data = (DATA*)((DWORD)item & ~(PAGESIZE-1)); // data
	PAGE* page = (PAGE*)((DWORD)region +
		((DWORD)data & (REGSIZE-1)) / (sizeof(DATA)/sizeof(PAGE))); // stránka

// zařazení do řetězce volných prvků
	item->PrevItem = NULL;
	item->NextItem = modul->FreeItem;
	if (item->NextItem != NULL)
	{
		item->NextItem->PrevItem = item;
	}
	modul->FreeItem = item;

// dekrementace použitých prvků stránky, příp. uvolnění stránky
	page->Used--;
	if (page->Used == 0)
	{
		if (modul->ResPage != NULL)
		{
			DelPage(modul);
		}
		modul->ResPage = page;
	}
}


/////////////////////////////////////////////////////////////////////////////
// chybové hlášení - chyba paměti

static void MemErr()
{
	if (MemoryError) return;
	MemoryError = TRUE;

	::MessageBox(MainFrame, _T("Insufficient memory, terminating."),
		_T("Memory Error"), MB_OK | MB_ICONSTOP);

	Exit();
}


/////////////////////////////////////////////////////////////////////////////
// přidělení bloku paměti (povolena 0 - vrátí malý blok) (vrací NULL=chyba)

void* _fastcall MemGet(int size)
{
// přednastavení výsledku operace na příznak neúspěchu
	void* result = NULL;

// kontrola zadané velikosti bloku
	ASSERT((DWORD)size < (DWORD)0x7ffffff0);
	if ((DWORD)size >= (DWORD)0x7ffffff0) 
	{
		MemErr();
		return NULL;
	}
	if (size == 0) size = 1;

// bude přidělení malého bloku
	if (size <= PAGESIZE)
	{

// uzamknutí přístupu k paměti
#ifdef _MT
		::EnterCriticalSection(&MemoryCriticalSection);
#endif

// určení modulu velikosti prvku
		MODUL* modul = Modul;		// začátek popisovačů modulů
		while (size > modul->Size)
		{
			modul++;				// zvýšení ukazatele modulů
		}

// vytvoření datového prvku
		result = (void*)NewItem(modul);
		ASSERT(result != NULL);

// odemknutí přístupu k paměti
#ifdef _MT
		::LeaveCriticalSection(&MemoryCriticalSection);
#endif
	}

// přidělení bloku z haldy, pokud nebyl blok přidělen
	if (result == NULL)
	{
		result = ::HeapAlloc(hHeap, HEAPSERIALIZE, (size + 0xff) & ~0xff);
	}

// nedostatek paměti
	if (result == NULL)
	{
		MemErr();
	}

	return result;
}


/////////////////////////////////////////////////////////////////////////////
// uvolnění bloku paměti (povolena adresa NULL - ignoruje se)

void _fastcall MemFree(void* adr)
{
// adresa NULL se ignoruje
	if (adr == NULL) return;

// neleží-li ve vlastním regionu, uvolnění pomocí haldy
	if (!UsedRegion[(DWORD)adr/REGSIZE])
	{
		::HeapFree(hHeap, HEAPSERIALIZE, adr);
		return;
	}

// uzamknutí přístupu k paměti
#ifdef _MT
	::EnterCriticalSection(&MemoryCriticalSection);
#endif

// příprava ukazatelů
	REGION* region = (REGION*)((DWORD)adr & ~(REGSIZE-1)); // adresa regionu
	DATA* data = (DATA*)((DWORD)adr & ~(PAGESIZE-1)); // data
	PAGE* page = (PAGE*)((DWORD)region +
		((DWORD)data & (REGSIZE-1)) / (sizeof(DATA)/sizeof(PAGE))); // stránka

// uvolnění položky
	ASSERT(page->Used > 0);
	MODUL* modul = page->Modul;
	ASSERT(((DWORD)adr & (modul->Size-1)) == 0);
	DelItem(modul, (ITEM*)adr);

// odemknutí přístupu k paměti
#ifdef _MT
	::LeaveCriticalSection(&MemoryCriticalSection);
#endif
}


/////////////////////////////////////////////////////////////////////////////
// změna velikosti bloku paměti (adresa NULL = vytvoření, velikost 0 = zrušení)
// vrací NULL=příznak chyby, starý blok při chybě nebude změněn

void* _fastcall MemSize(void* adr, int size)
{
	void* newadr;

// při adrese NULL vytvoření nové paměti
	if (adr == NULL)
	{
		if (size <= 0) return NULL;
		return MemGet(size);
	}

// při velikosti 0 uvolnění bloku
	if (size <= 0)
	{
		MemFree(adr);
		return NULL;
	}

// kontrola, zda blok leží ve vlastním regionu
	if (UsedRegion[(DWORD)adr/REGSIZE])
	{

// příprava ukazatelů
		REGION* region = (REGION*)((DWORD)adr & ~(REGSIZE-1)); // adresa regionu
		DATA* data = (DATA*)((DWORD)adr & ~(PAGESIZE-1)); // data
		PAGE* page = (PAGE*)((DWORD)region +
			((DWORD)data & (REGSIZE-1)) / (sizeof(DATA)/sizeof(PAGE))); // stránka

// kontrola, zda je adresa platná
		ASSERT(page->Used > 0);
		MODUL* modul = page->Modul;
		int oldsize = modul->Size;
		ASSERT(((DWORD)adr & (oldsize-1)) == 0);

// test, zda je potřeba velikost měnit
		if (((size <= 8) && (oldsize <= 8)) ||
			((size <= oldsize) && (size > oldsize/4)))
		{
			return adr;					// adresa nezměněna
		}

// vytvoření nového bloku
		newadr = MemGet(size);

// kopie starých dat bloku
		if (newadr != NULL)
		{
			if (size >= oldsize)
			{
				MemCopy(newadr, adr, oldsize);
			}
			else
			{
				MemCopy(newadr, adr, size);
			}
			MemFree(adr);
		}
		return newadr;
	}

// přenesení bloku z haldy do malých bloků
	if (size <= PAGESIZE/2)
	{
		int oldsize = (int)::HeapSize(hHeap, HEAPSERIALIZE, adr);
		ASSERT(oldsize != -1);
		if (oldsize == -1)
		{
			MemErr();
			return NULL;
		}

		newadr = MemGet(size);

		if (newadr != NULL)
		{
			if (size >= oldsize)
			{
				MemCopy(newadr, adr, oldsize);
			}
			else
			{
				MemCopy(newadr, adr, size);
			}
			::HeapFree(hHeap, HEAPSERIALIZE, adr);
		}
		return newadr;
	}

// realokace bloku v haldě
	newadr = ::HeapReAlloc(hHeap, HEAPSERIALIZE, adr, (size + 0xff) & ~0xff);
	if (newadr == NULL) MemErr();	// chyba paměti

	return newadr;
}


/////////////////////////////////////////////////////////////////////////////
// inicializace správce paměti (TRUE=inicializace OK)

BOOL MemInit()
{
// kontrola konstant
#pragma warning ( disable: 4127)				// hlášení - konstantní podmínka
	ASSERT(sizeof(ITEM) == 8);
	ASSERT(sizeof(PAGE) == 16);
	ASSERT(sizeof(REGION) == REGSIZE);
	ASSERT((REGSIZE & 0xfffff) == 0);
	ASSERT(PAGESIZE <= 0x4000);
#pragma warning ( default: 4127)				// hlášení - konstantní podmínka

// funkce může být volána jen jednou
	ASSERT(hHeap == NULL);
	ASSERT(UsedRegion == NULL);
	if ((hHeap != NULL) || (UsedRegion != NULL)) return FALSE;

// vytvoření haldy
	hHeap = ::HeapCreate(HEAPSERIALIZE, PAGESIZE, 0);
	if (hHeap == NULL) return FALSE;

// vytvoření bufferu příznaků použití regionů
	UsedRegion = (bool*) ::HeapAlloc(hHeap, HEAPSERIALIZE, MAXREGS * sizeof(bool));
	if (UsedRegion == NULL) return FALSE;
	for (int i = 0; i < MAXREGS; i++) { UsedRegion[i] = false; };

// inicializace uzamykání přístupu k paměti
#ifdef _MT
	::InitializeCriticalSection(&MemoryCriticalSection);
#endif

// vytvoření prvního regionu (pro zkoušku paměti)
	MemoryOK = NewRegion();
	return MemoryOK;
}


/////////////////////////////////////////////////////////////////////////////
// ukončení správce paměti

void MemTerm()
{
	MemoryOK = FALSE;

// kontrola, zda je správce paměti již ukončen
	ASSERT(hHeap != NULL);
	if (hHeap == NULL) return;

// uvolnění haldy
	::HeapDestroy(hHeap);

// uvolnění uzamykání přístupu k paměti
#ifdef _MT
	::DeleteCriticalSection(&MemoryCriticalSection);
#endif
}


/////////////////////////////////////////////////////////////////////////////
// vícetoková inkrementace čísla LONG INT

#if defined(_MT) && defined(_M_IX86)

void _fastcall LongIncrement(long* num)
{
	_asm 
	{
		INC		DWORD PTR [ECX]
	}
}

#endif


/////////////////////////////////////////////////////////////////////////////
// vícetoková dekrementace čísla LONG INT (vrací TRUE = výsledek je 0)

#if defined(_MT) && defined(_M_IX86)
#pragma warning ( disable: 4035)

BOOL _fastcall LongDecrement(long* num)
{
	_asm 
	{
		XOR		EAX, EAX
		DEC		DWORD PTR [ECX]
		SETE	AL
	}
}

#pragma warning ( default: 4035)
#endif
