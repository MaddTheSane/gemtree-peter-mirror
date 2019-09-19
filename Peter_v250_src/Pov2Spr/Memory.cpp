
#include "Main.h"

/***************************************************************************\
*																			*
*								Obsluha pamìti								*
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
static	CRITICAL_SECTION	MemoryCriticalSection;	// kritická sekce pro správce pamìti
#endif

/////////////////////////////////////////////////////////////////////////////
// konstanty - velikosti musí být mocnina 2 !

#ifndef _M_ALPHA
#define	PAGESIZE		0x1000			// velikost alokaèní stránky pro ostatní procesory (4096)
#else
#define	PAGESIZE		0x2000			// velikost alokaèní stránky pro procesor Alpha (8192)
#endif

#define PAGEITEMS		(PAGESIZE/sizeof(ITEM))	// poèet prvkù na stránku (512 popø. 1024)
#define	REGPAGES		(PAGESIZE/sizeof(PAGE)) // poèet stránek na region (256 popø. 512)
#define DATAPAGES		(REGPAGES-1)			// poèet datových stránek na region (255 popø. 511)
#define REGSIZE			(PAGESIZE*REGPAGES)		// velikost regionu (1 MB popø. 4 MB)
#define NUMREGS			(0x78000000/REGSIZE-1)	// poèet regionù v pamìti (2039 popø. 509)
#define REGSYSTEM		(0x800000/REGSIZE)		// poèet systémových regionù na zaèátku (8 popø. 2)
#define MAXREGS			((0x40000000/REGSIZE)*4)// poèet regionù celkem (4096 popø. 1024)

/////////////////////////////////////////////////////////////////////////////
// struktury

struct MODUL_;

// struktura datového prvku - 8 bajtù
typedef struct ITEM_
{
	ITEM_*		NextItem;				// adresa dalšího volného prvku (NULL=konec)
	ITEM_*		PrevItem;				// adresa pøedešlého volného prvku (NULL=zaèátek)
} ITEM;

// struktura dat stránky - velikost PAGESIZE
typedef struct DATA_
{
	ITEM		Items[PAGEITEMS];		// data jedné stránky
} DATA;

// struktura popisovaèe stránky - velikost 16 bajtù
typedef struct PAGE_
{
	PAGE_*		NextPage;				// adresa další stránky (NULL=konec)
	PAGE_*		PrevPage;				// adresa pøedešlé stránky (NULL=zaèátek)
	MODUL_*		Modul;					// adresa popisovaèe modulu
	long		Used;					// èítaè použitých položek
} PAGE;

// struktura popisovaèe modulu
typedef struct MODUL_
{
	ITEM*		FreeItem;				// adresa volného prvku
	PAGE*		ResPage;				// rezervní stránka (NULL=není)
	long		Size;					// velikost datové položky
} MODUL;

// struktura regionu - velikost REGSIZE
typedef struct REGION_
{
	long		Used;					// èítaè použitých stránek
	long		res[3];					// rezerva pro zarovnání
	PAGE		Pages[DATAPAGES];		// popisovaèe stránek
	DATA		Data[DATAPAGES];		// data
} REGION;


/////////////////////////////////////////////////////////////////////////////
// promìnné

static	HANDLE	hHeap = NULL;			// handle haldy

// obsluha regionù
static	char*	UsedRegion;				// buffer pøíznakù použití regionù
static	int		NextRegion = REGSYSTEM;	// index pøíštì pøidìlovaného regionu
static	REGION*	ResRegion = NULL;		// rezervní region (NULL=není)

// obsluha nepøipojených stránek (následuje øetìzec pomocí NextPage)
static	PAGE*	FreePage = NULL;		// první nepøipojená stránka (NULL=není)

// popisovaèe datových modulù
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

static ITEM* NewItem(MODUL* modul);				// vytvoøení nového datového prvku
static void DelItem(MODUL* modul, ITEM* item);	// zrušení datového prvku
static BOOL NewPage(MODUL* modul);				// vytvoøení nové stránky
static void DelPage(MODUL* modul);				// zrušení rezervní stránky
static BOOL NewRegion();						// vytvoøení nového regionu
static void DelRegion();						// zrušení rezervního regionu


/////////////////////////////////////////////////////////////////////////////
// inicializace správce pamìti (TRUE=inicializace OK)

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

// funkce mùže být volána jen jednou
	ASSERT(hHeap == NULL);
	if (hHeap != NULL) return TRUE;

// vytvoøení haldy
	hHeap = ::HeapCreate(HEAPSERIALIZE, PAGESIZE, 0);
	if (hHeap == NULL) return FALSE;

// vytvoøení bufferu pøíznakù použití regionù
	UsedRegion = (char*) ::HeapAlloc(hHeap, HEAP_ZERO_MEMORY | HEAPSERIALIZE, MAXREGS);
	if (UsedRegion == NULL) return FALSE;

// inicializace uzamykání pøístupu k pamìti
#ifdef _MT
	::InitializeCriticalSection(&MemoryCriticalSection);
#endif

// vytvoøení prvního regionu (pro zkoušku pamìti)
	return NewRegion();
}


/////////////////////////////////////////////////////////////////////////////
// ukonèení správce pamìti

void MemTerm()
{
// kontrola, zda je správce pamìti již ukonèen
	ASSERT(hHeap != NULL);
	if (hHeap == NULL) return;

// uvolnìní haldy
	::HeapDestroy(hHeap);
	hHeap = NULL;

// uvolnìní uzamykání pøístupu k pamìti
#ifdef _MT
	::DeleteCriticalSection(&MemoryCriticalSection);
#endif
}


/////////////////////////////////////////////////////////////////////////////
// chyba pamìti

void MemErr()
{
	::MessageBox(NULL, _T("Sorry, this program will be terminated due to the insufficient memory!"),
		_T("Insufficient Memory"), MB_OK | MB_ICONSTOP);

//	::MessageBox(NULL, _T("Lituji, program bude ukonèen z dùvodu nedostatku pamìti!"),
//		_T("Nedostatek pamìti"), MB_OK | MB_ICONSTOP);
	Exit(EXITCODE_MEMERR);
}


/////////////////////////////////////////////////////////////////////////////
// pøidìlení bloku pamìti (povolena 0) NULL=chyba

void* _fastcall MemGet(int size)
{
	void* result = NULL;

// kontrola zadané velikosti bloku
	ASSERT((size >= 0) && (size < 0x7ffffff0));
	if (size <= 0) size = 1;

// bude pøidìlení malého bloku
	if (size <= PAGESIZE)
	{

// uzamknutí pøístupu k pamìti
#ifdef _MT
		::EnterCriticalSection(&MemoryCriticalSection);
#endif

// urèení modulu velikosti prvku
		MODUL* modul = Modul;		// zaèátek popisovaèù modulù
		while (size > modul->Size)
		{
			modul++;				// zvýšení ukazatele modulù
		}

// vytvoøení datového prvku
		result = (void*)NewItem(modul);
		ASSERT(result != NULL);

// odemknutí pøístupu k pamìti
#ifdef _MT
		::LeaveCriticalSection(&MemoryCriticalSection);
#endif
	}

// pøidìlení bloku z haldy, pokud nebyl blok pøidìlen
	if (result == NULL)
	{
		result = ::HeapAlloc(hHeap, HEAPSERIALIZE, (size + 0xff) & ~0xff);
	}

	if (result == NULL) MemErr();	// chyba pamìti

	return result;
}


/////////////////////////////////////////////////////////////////////////////
// uvolnìní bloku pamìti (povoleno NULL)

void _fastcall MemFree(void* adr)
{
// adresa NULL se ignoruje
	if (adr == NULL) return;

// neleží-li ve vlastním regionu, uvolnìní pomocí haldy (ještì není nutné uzamknutí!)
	if (!UsedRegion[(DWORD)adr/REGSIZE])
	{
		::HeapFree(hHeap, HEAPSERIALIZE, adr);
		return;
	}

// uzamknutí pøístupu k pamìti
#ifdef _MT
	::EnterCriticalSection(&MemoryCriticalSection);
#endif

// pøíprava ukazatelù
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

// uvolnìní položky
			DelItem(modul, (ITEM*)adr);
		}
	}

// odemknutí pøístupu k pamìti
#ifdef _MT
	::LeaveCriticalSection(&MemoryCriticalSection);
#endif
}


/////////////////////////////////////////////////////////////////////////////
// zmìna velikosti bloku pamìti (povolena adresa NULL i velikost 0, pøi chybì nezmìnìno)
// Pozn.: zde není nutné uzamykat pamì (znaènì to zpomaluje) !!!

void* _fastcall MemSize(void* adr, int size)
{
	int oldsize;
	void* newadr;

// pøi adrese NULL alokace pamìti
	if (adr == NULL)
	{
		if (size <= 0) return NULL;
		return MemGet(size);
	}

// pøi velikosti 0 uvolnìní bloku
	if (size <= 0)
	{
		MemFree(adr);
		return NULL;
	}

// kontrola, zda blok leží ve vlastním regionu
	if (UsedRegion[(DWORD)adr/REGSIZE])
	{

// pøíprava ukazatelù
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

// test, zda je potøeba velikost mìnit
				if (((size <= 8) && (oldsize <= 8)) ||
					((size <= oldsize) && (size > oldsize/4)))
				{
					return adr;					// adresa nezmìnìna
				}

// vytvoøení nového bloku
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

// pøenesení bloku z haldy do malých blokù
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

// realokace bloku v haldì
	newadr = ::HeapReAlloc(hHeap, HEAPSERIALIZE, adr, (size + 0xff) & ~0xff);
	if (newadr == NULL) MemErr();	// chyba pamìti
	return newadr;
}


/////////////////////////////////////////////////////////////////////////////
// vytvoøení nového datového prvku

static ITEM* NewItem(MODUL* modul)
{
// zajištìní platného prvku
	if (modul->FreeItem == NULL)
	{
		if (!NewPage(modul))
		{
			return NULL;
		}
	}

// urèení adresy popisovaèe stránky
	ITEM* item = modul->FreeItem;		// adresa prvku
	REGION* region = (REGION*)((DWORD)item & ~(REGSIZE-1)); // adresa regionu
	DATA* data = (DATA*)((DWORD)item & ~(PAGESIZE-1)); // data
	PAGE* page = (PAGE*)((DWORD)region +
		((DWORD)data & (REGSIZE-1)) / (sizeof(DATA)/sizeof(PAGE))); // stránka

// inkrementace použitých promìnných stránky, zrušení rezervace stránky
	page->Used++;						// inkrementace použitých položek
	if (page == modul->ResPage)			// je to rezervní stránka?
	{
		modul->ResPage = NULL;			// zrušení rezervní stránky
	}

// vyøazení prvku z øetìzce volných prvkù
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
// pøíprava adres popisovaèù
	REGION* region = (REGION*)((DWORD)item & ~(REGSIZE-1)); // adresa regionu
	DATA* data = (DATA*)((DWORD)item & ~(PAGESIZE-1)); // data
	PAGE* page = (PAGE*)((DWORD)region +
		((DWORD)data & (REGSIZE-1)) / (sizeof(DATA)/sizeof(PAGE))); // stránka

// zaøazení do øetìzce volných prvkù
	item->PrevItem = NULL;
	item->NextItem = modul->FreeItem;
	if (item->NextItem != NULL)
	{
		item->NextItem->PrevItem = item;
	}
	modul->FreeItem = item;

// dekrementace použitých prvkù stránky, pøíp. uvolnìní stránky
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
// vytvoøení rezervní stránky

static BOOL NewPage(MODUL* modul)
{
// zajištìní pøipojitelné stránky
	if (FreePage == NULL)
	{
		if (!NewRegion())
		{
			return FALSE;
		}
	}

// urèení adresy dat pøipojované stránky
	PAGE* page = FreePage;				// pøipojovaná stránka
	REGION* region = (REGION*)((DWORD)page & ~(REGSIZE-1)); // adresa regionu
	DATA* data = (DATA*)((DWORD)region + 
		((DWORD)page & (REGSIZE-1)) * (sizeof(DATA)/sizeof(PAGE))); // data

// pøipojení dat stránky
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

// vyøazení stránky z øetìzce nepøipojených stránek
	PAGE* nextpage = page->NextPage;
	FreePage = nextpage;
	if (nextpage != NULL)
	{
		nextpage->PrevPage = NULL;
	}

// napojení do øetìzce volných položek modulu
	int size = modul->Size;				// velikost datové položky
	ITEM* previtem;
	ITEM* nextitem = data->Items;		// ukazatel adresy prvku
	ITEM* item = NULL;					// není pøedešlý prvek
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

// inicializace položek popisovaèe stránky
	page->Modul = modul;				// adresa modulu
	page->Used = 0;						// poèet použitých položek

// nastavení stránky jako rezervní
	modul->ResPage = page;				// bude teï jako rezervní stránka
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

// pøíprava adresy regionu a dat
	REGION* region = (REGION*)((DWORD)page & ~(REGSIZE-1)); // adresa regionu
	DATA* data = (DATA*)((DWORD)region + 
		((DWORD)page & (REGSIZE-1)) * (sizeof(DATA)/sizeof(PAGE))); // data

// odpojení z øetìzce volných položek
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

// napojení stránky do øetìzce nepøipojených stránek
	page->PrevPage = NULL;
	page->NextPage = FreePage;
	if (FreePage != NULL)
	{
		FreePage->PrevPage = page;
	}
	FreePage = page;
	
// snížení èítaèe použitých stránek regionu, pøíp. uvolnìní regionu
	region->Used--;
	if (region->Used == 0)
	{
		if (ResRegion != NULL)
		{
			DelRegion();		// uvolnìní rezervního regionu
		}
		ResRegion = region;		// je to nyní rezervní region
	}
}


/////////////////////////////////////////////////////////////////////////////
// vytvoøení rezervního regionu

static BOOL NewRegion()
{
// lokální promìnné
	REGION* region;						// ukazatel adresy regionu
	PAGE*	page;						// ukazatel adresy stránky
	PAGE*	nextpage;					// pomocný ukazatel - pøíští stránka
	PAGE*	prevpage;					// pomocný ukazatel - pøedchozí stránka
	char*	used;						// ukazatel použitých regionù
	int i;								// pomocný èítaè

// nalezení nepoužitého regionu
	for (i = NUMREGS;  i > 0; i--)
	{

// adresa zkoušeného regionu
		region = (REGION*)(void*)(NextRegion*REGSIZE); // adresa regionu
		used = &(UsedRegion[NextRegion]);	// pøíznak použití regionu

// posun indexu pøíštího regionu
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

// vytvoøení první stránky pro popisovaè regionu
			if (region != NULL)
			{
				if (::VirtualAlloc(region, PAGESIZE,
						MEM_COMMIT, PAGE_READWRITE) == NULL)
				{

// pøi chybì uvolnìní regionu a hlášení chyby
					::VirtualFree(region, 0, MEM_RELEASE);
					return FALSE;
				}

// pøíznak použití regionu
				*used = TRUE;		// pøíznak použití regionu
				ResRegion = region;	// bude teï jako rezervní region
				region->Used = 0;		// èítaè použitých stránek

// napojení stránek do øetìzce nepøipojených stránek
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

// odpojení stránek z øetìzce nepøipojených stránek
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

// zrušení pøíznaku použití regionu
	UsedRegion[(DWORD)region/REGSIZE] = FALSE;

// odpojení stránky popisovaèe regionu
	VERIFY (::VirtualFree(region, PAGESIZE, MEM_DECOMMIT));

// uvolnìní regionu
	VERIFY (::VirtualFree(region, 0, MEM_RELEASE));
}


/////////////////////////////////////////////////////////////////////////////
// vícetoková inkrementace èísla LONG INT

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
// vícetoková dekrementace èísla LONG INT (vrací TRUE = výsledek je 0)

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
