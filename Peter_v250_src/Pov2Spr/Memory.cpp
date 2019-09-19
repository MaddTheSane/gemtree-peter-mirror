
#include "Main.h"

/***************************************************************************\
*																			*
*								Obsluha paměti								*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// obsluha vícevláknového režimu

#ifdef _MT
#define HEAPSERIALIZE 0					// pro vícevláknový režim je serializace
#else
#define HEAPSERIALIZE HEAP_NO_SERIALIZE	// vypnutí serializace, není-li vícevláknový režim
#endif

#ifdef _MT
static	CRITICAL_SECTION	MemoryCriticalSection;	// kritická sekce pro správce paměti
#endif

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
#define NUMREGS			(0x78000000/REGSIZE-1)	// počet regionů v paměti (2039 popř. 509)
#define REGSYSTEM		(0x800000/REGSIZE)		// počet systémových regionů na začátku (8 popř. 2)
#define MAXREGS			((0x40000000/REGSIZE)*4)// počet regionů celkem (4096 popř. 1024)

/////////////////////////////////////////////////////////////////////////////
// struktury

struct MODUL_;

// struktura datového prvku - 8 bajtů
typedef struct ITEM_
{
	ITEM_*		NextItem;				// adresa dalšího volného prvku (NULL=konec)
	ITEM_*		PrevItem;				// adresa předešlého volného prvku (NULL=začátek)
} ITEM;

// struktura dat stránky - velikost PAGESIZE
typedef struct DATA_
{
	ITEM		Items[PAGEITEMS];		// data jedné stránky
} DATA;

// struktura popisovače stránky - velikost 16 bajtů
typedef struct PAGE_
{
	PAGE_*		NextPage;				// adresa další stránky (NULL=konec)
	PAGE_*		PrevPage;				// adresa předešlé stránky (NULL=začátek)
	MODUL_*		Modul;					// adresa popisovače modulu
	long		Used;					// čítač použitých položek
} PAGE;

// struktura popisovače modulu
typedef struct MODUL_
{
	ITEM*		FreeItem;				// adresa volného prvku
	PAGE*		ResPage;				// rezervní stránka (NULL=není)
	long		Size;					// velikost datové položky
} MODUL;

// struktura regionu - velikost REGSIZE
typedef struct REGION_
{
	long		Used;					// čítač použitých stránek
	long		res[3];					// rezerva pro zarovnání
	PAGE		Pages[DATAPAGES];		// popisovače stránek
	DATA		Data[DATAPAGES];		// data
} REGION;


/////////////////////////////////////////////////////////////////////////////
// proměnné

static	HANDLE	hHeap = NULL;			// handle haldy

// obsluha regionů
static	char*	UsedRegion;				// buffer příznaků použití regionů
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
					NULL,	NULL,	0x4000 };


/////////////////////////////////////////////////////////////////////////////
// prototypy lokálních funkcí

static ITEM* NewItem(MODUL* modul);				// vytvoření nového datového prvku
static void DelItem(MODUL* modul, ITEM* item);	// zrušení datového prvku
static BOOL NewPage(MODUL* modul);				// vytvoření nové stránky
static void DelPage(MODUL* modul);				// zrušení rezervní stránky
static BOOL NewRegion();						// vytvoření nového regionu
static void DelRegion();						// zrušení rezervního regionu


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
	ASSERT(FALSE == 0);
#pragma warning ( default: 4127)				// hlášení - konstantní podmínka

// funkce může být volána jen jednou
	ASSERT(hHeap == NULL);
	if (hHeap != NULL) return TRUE;

// vytvoření haldy
	hHeap = ::HeapCreate(HEAPSERIALIZE, PAGESIZE, 0);
	if (hHeap == NULL) return FALSE;

// vytvoření bufferu příznaků použití regionů
	UsedRegion = (char*) ::HeapAlloc(hHeap, HEAP_ZERO_MEMORY | HEAPSERIALIZE, MAXREGS);
	if (UsedRegion == NULL) return FALSE;

// inicializace uzamykání přístupu k paměti
#ifdef _MT
	::InitializeCriticalSection(&MemoryCriticalSection);
#endif

// vytvoření prvního regionu (pro zkoušku paměti)
	return NewRegion();
}


/////////////////////////////////////////////////////////////////////////////
// ukončení správce paměti

void MemTerm()
{
// kontrola, zda je správce paměti již ukončen
	ASSERT(hHeap != NULL);
	if (hHeap == NULL) return;

// uvolnění haldy
	::HeapDestroy(hHeap);
	hHeap = NULL;

// uvolnění uzamykání přístupu k paměti
#ifdef _MT
	::DeleteCriticalSection(&MemoryCriticalSection);
#endif
}


/////////////////////////////////////////////////////////////////////////////
// chyba paměti

void MemErr()
{
	::MessageBox(NULL, _T("Sorry, this program will be terminated due to the insufficient memory!"),
		_T("Insufficient Memory"), MB_OK | MB_ICONSTOP);

//	::MessageBox(NULL, _T("Lituji, program bude ukončen z důvodu nedostatku paměti!"),
//		_T("Nedostatek paměti"), MB_OK | MB_ICONSTOP);
	Exit(EXITCODE_MEMERR);
}


/////////////////////////////////////////////////////////////////////////////
// přidělení bloku paměti (povolena 0) NULL=chyba

void* _fastcall MemGet(int size)
{
	void* result = NULL;

// kontrola zadané velikosti bloku
	ASSERT((size >= 0) && (size < 0x7ffffff0));
	if (size <= 0) size = 1;

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

	if (result == NULL) MemErr();	// chyba paměti

	return result;
}


/////////////////////////////////////////////////////////////////////////////
// uvolnění bloku paměti (povoleno NULL)

void _fastcall MemFree(void* adr)
{
// adresa NULL se ignoruje
	if (adr == NULL) return;

// neleží-li ve vlastním regionu, uvolnění pomocí haldy (ještě není nutné uzamknutí!)
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

// kontrola, zda je stránka platná
	if (page->Used > 0)
	{

// kontrola, zda adresa leží na platné hranici
		MODUL* modul = page->Modul;
		if (((DWORD)adr & (modul->Size-1)) == 0)
		{

// uvolnění položky
			DelItem(modul, (ITEM*)adr);
		}
	}

// odemknutí přístupu k paměti
#ifdef _MT
	::LeaveCriticalSection(&MemoryCriticalSection);
#endif
}


/////////////////////////////////////////////////////////////////////////////
// změna velikosti bloku paměti (povolena adresa NULL i velikost 0, při chybě nezměněno)
// Pozn.: zde není nutné uzamykat paměť (značně to zpomaluje) !!!

void* _fastcall MemSize(void* adr, int size)
{
	int oldsize;
	void* newadr;

// při adrese NULL alokace paměti
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

// kontrola, zda je stránka platná
		if (page->Used > 0)
		{

// kontrola, zda adresa leží na platné hranici
			MODUL* modul = page->Modul;
			oldsize = modul->Size;

			if (((DWORD)adr & (oldsize-1)) == 0)
			{

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

// neplatná adresa - chybový návrat
			else
			{
				return NULL;
			}
		}

// neplatná stránka - chybový návrat
		else
		{
			return NULL;
		}
	}

// přenesení bloku z haldy do malých bloků
	if (size <= PAGESIZE/2)
	{
		oldsize = (int)::HeapSize(hHeap, HEAPSERIALIZE, adr);
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
// vytvoření nového datového prvku

static ITEM* NewItem(MODUL* modul)
{
// zajištění platného prvku
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

// inkrementace použitých proměnných stránky, zrušení rezervace stránky
	page->Used++;						// inkrementace použitých položek
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
// vytvoření rezervní stránky

static BOOL NewPage(MODUL* modul)
{
// zajištění připojitelné stránky
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

// inkremetace použitých stránek regionu, zrušení rezervace regiony
	region->Used++;						// inkrementace použitých stránek
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
	ITEM* previtem;
	ITEM* nextitem = data->Items;		// ukazatel adresy prvku
	ITEM* item = NULL;					// není předešlý prvek
	for (int i = PAGESIZE/size; i > 0; i--)
	{
		previtem = item;
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
// zrušení rezervní stránky

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
	ITEM* nextitem;
	ITEM* previtem;
	for (int i = PAGESIZE/size; i > 0; i--)
	{
		nextitem = item->NextItem;
		previtem = item->PrevItem;

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

// odpojení stránky
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
// vytvoření rezervního regionu

static BOOL NewRegion()
{
// lokální proměnné
	REGION* region;						// ukazatel adresy regionu
	PAGE*	page;						// ukazatel adresy stránky
	PAGE*	nextpage;					// pomocný ukazatel - příští stránka
	PAGE*	prevpage;					// pomocný ukazatel - předchozí stránka
	char*	used;						// ukazatel použitých regionů
	int i;								// pomocný čítač

// nalezení nepoužitého regionu
	for (i = NUMREGS;  i > 0; i--)
	{

// adresa zkoušeného regionu
		region = (REGION*)(void*)(NextRegion*REGSIZE); // adresa regionu
		used = &(UsedRegion[NextRegion]);	// příznak použití regionu

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
			region = (REGION*)::VirtualAlloc(region,
					REGSIZE, MEM_RESERVE, PAGE_READWRITE);

// vytvoření první stránky pro popisovač regionu
			if (region != NULL)
			{
				if (::VirtualAlloc(region, PAGESIZE,
						MEM_COMMIT, PAGE_READWRITE) == NULL)
				{

// při chybě uvolnění regionu a hlášení chyby
					::VirtualFree(region, 0, MEM_RELEASE);
					return FALSE;
				}

// příznak použití regionu
				*used = TRUE;		// příznak použití regionu
				ResRegion = region;	// bude teď jako rezervní region
				region->Used = 0;		// čítač použitých stránek

// napojení stránek do řetězce nepřipojených stránek
				page = NULL;
				nextpage = region->Pages;
				for (i = DATAPAGES; i > 0; i--)
				{
					prevpage = page;
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
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// zrušení rezevního regionu

static void DelRegion()
{
// kontrola, zda je rezervní region
	REGION* region = ResRegion;
	if (region == NULL) return;
	ResRegion = NULL;

// odpojení stránek z řetězce nepřipojených stránek
	PAGE* page = region->Pages;
	PAGE* nextpage;
	PAGE* prevpage;
	for (int i = DATAPAGES; i > 0; i--)
	{
		nextpage = page->NextPage;
		prevpage = page->PrevPage;

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
	UsedRegion[(DWORD)region/REGSIZE] = FALSE;

// odpojení stránky popisovače regionu
	VERIFY (::VirtualFree(region, PAGESIZE, MEM_DECOMMIT));

// uvolnění regionu
	VERIFY (::VirtualFree(region, 0, MEM_RELEASE));
}


/////////////////////////////////////////////////////////////////////////////
// vícetoková inkrementace čísla LONG INT

#ifdef _MT
#ifdef _M_IX86

void _fastcall LongIncrement(long* num)
{
	_asm 
	{
		INC		DWORD PTR [ECX]
	}
}

#endif	// _M_IX86
#endif	// _MT


/////////////////////////////////////////////////////////////////////////////
// vícetoková dekrementace čísla LONG INT (vrací TRUE = výsledek je 0)

#ifdef _MT
#ifdef _M_IX86

#pragma warning ( disable: 4035)
BOOL _fastcall LongDecrement(long* num)
{
	_asm 
	{
		XOR		EAX, EAX
		DEC		DWORD PTR [ECX]
		JNZ		X1
		INC		EAX
	X1:
	}
}
#pragma warning ( default: 4035)

#endif	// _M_IX86
#endif	// _MT
