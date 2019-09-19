
#include "Main.h"

/***************************************************************************\
*																			*
*							Výběr souboru k editaci							*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// seznam položek

// položka seznamu
typedef struct SELECTITEM_ 
{
	CIcon		Icon;								// ikona
	CMultiText	Name;								// jméno položky
	CText		NameLoad;							// plná specifikace položky pro čtení
	CText		NameSave;							// plná specifikace položky pro zápis
	int			Typ;								// typ položky
	BOOL		Altern;								// je alternativní (vzorový) adresář
													//	- u programů má přednost lokální program (kvůli spouštění modifikací)
													//	- u adresářů má přednost vzorový adresář (zákaz přejmenování)
	int			Index;								// index položky v seznamu (-1 = neplatný)
} SELECTITEM;

SELECTITEM*		SelectItem = NULL;					// buffer položek seznamu
int				SelectItemMax = 0;					// velikost bufferu položek seznamu
int				SelectItemNum = 0;					// počet položek v seznamu

// typy položek (určuje třídění položek !!!!)
#define TYP0 0										// není platná položka
#define	TYPUPDIR 1									// položka je nadadresář ".."
#define TYPSUBDIR 2									// položa je podadresář
#define TYPFILE 3									// položka je soubor

int				SelectInx = -1;						// index aktivní vybrané položky (-1 = není)
int				SelectIndex = -1;					// index aktivní položky seznamu (-1 = neplatná)
int				SelectTyp = TYP0;					// typ vybrané položky (TYP0 = není žádná)
BOOL			SelectAltern = FALSE;				// aktivní položka je v alternativním (vzorovém) adresáři

BOOL			SelectFill = FALSE;					// příznak načítání obsahu okna

CBufIcon	SelectIcon0;							// vzor ikon adresářů

CText		SelectAliases;							// jméno souboru ALIASES.INI
CText		SelectAliases2;							// jméno souboru ALIASES.INI ve vzorovém adresáři

/////////////////////////////////////////////////////////////////////////////
// globální a lokální proměnné

BOOL		SelectMode = FALSE;						// příznak módu výběru souborů
HWND		SelectView = NULL;						// seznam souborů
HIMAGELIST	SelectImageList = NULL;					// seznam obrázků

HFONT		SelFont = NULL;							// nastavený font v okně

int			SelectStatusWidth = 400;				// šířka druhého pole stavového řádku

CText		SelectNewText;							// zadané jméno nového programu
CMultiText	SelectNewMultiText;						// zadané jméno vícejazyčně

BOOL		SelectNewFill = FALSE;					// plní se editor jména nového programu
HWND		SelectNewEdit = NULL;					// handle editoru jména nového programu

BOOL		SelEditName = FALSE;					// příznak probíhající editace
HWND		SelEditNameWnd = NULL;					// editační okno

BOOL		IsWaitCursor = FALSE;					// zapnut čekací kurzor

#define		ICONSUBDIR	0							// index ikony podadresáře
#define		ICONUPDIR	1							// index ikony nadadresáře

/////////////////////////////////////////////////////////////////////////////
// hlášení o změně adresáře

HANDLE		SelectDirChange = INVALID_HANDLE_VALUE;	// hlášení o změně adresáře
HANDLE		SelectDirChange2 = INVALID_HANDLE_VALUE;// hlášení o změně adresáře 2
#define		SelectDirChangeTimerID 19112			// ID časovače hlášení změny adresáře
UINT		SelectDirChangeTimer = 0;				// časovač pro hlášení změn adresáře


/////////////////////////////////////////////////////////////////////////////
// opožděné zapnutí editace

#define		SelectTimerID 19125						// ID časovače pro zapnutí editace
UINT		SelectTimer = 0;						// časovač pro zapnutí editace
int			SelectTimerN = 0;						// čítač pro zapnutí editace


/////////////////////////////////////////////////////////////////////////////
// definice tlačítek pro toolbar Select

TBBUTTON ToolBarSelect[] = {
	{ ButtonStart,	IDN_START,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{ ButtonOpen,	IDN_OPEN,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{ 0,			0,				TBSTATE_ENABLED, TBSTYLE_SEP,		0, 0},
	{ ButtonNew,	IDN_NEW,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{ ButtonDirNew,	IDN_PATH,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{ ButtonCopy,	IDN_NEWCOPY,	TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{ ButtonEditName,IDN_RENAME,	TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
	{ ButtonClear,	IDN_DELETE,		TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0},
};

#define ToolBarSelectNum (sizeof(ToolBarSelect)/sizeof(TBBUTTON))

void SelectLoad();				// načtení seznamu souborů

/////////////////////////////////////////////////////////////////////////////
// definice menu

MENUITEM* MenuSelect[] =
{
	MenuSoubor1,
	MenuHlp,
	NULL
};

/////////////////////////////////////////////////////////////////////////////
// záhlaví nového programu

const struct { 
	char ident[3];			// (3) identifikace = "PET"
	BYTE verze;				// (1) verze = 1
	WORD Editor;			// (2) verze editoru v tisícinách
	WORD Param;				// (2) parametry = 0
	long data;				// (4) offset začátku dat od začátku záhlaví (= délka záhlaví)
	long pocet;				// (4) počet datových bloků
} NewHead =
{
	'P', 'E', 'T',			// (3) identifikace = "PET"
	1,						// (1) verze = 1
	VerzeCom + VerzeRel*10 + VerzeMin*100 + VerzeMaj*1000, // verze editoru
	0,						// (4) parametry = 0
	3 + 1 + 4 + 4 + 4,		// (4) offset začátku dat od začátku záhlaví (= délka záhlaví)
	0,						// (4) počet datových bloků
};

/////////////////////////////////////////////////////////////////////////////
//	zrušení buffer položek

void SelectItemDelAll()
{
	for (int i = 0; i < SelectItemNum; i++)
	{
		SelectItem[i].Icon.Term();
		SelectItem[i].Name.Term();
	}

	MemFree(SelectItem);
	SelectItem = NULL;
	SelectItemMax = 0;
	SelectItemNum = 0;
	SelectInx = -1;
	SelectIndex = -1;
	SelectTyp = TYP0;
	SelectAltern = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// úschova parametrů vybrané položky (-1 = žádná)

void SelectSelect(int inx)
{
	if ((DWORD)inx >= (DWORD)SelectItemNum)
	{
		SelectInx = -1;
		SelectIndex = -1;
		SelectTyp = TYP0;
		SelectAltern = FALSE;
		Jmeno.Empty();
		JmenoLoad.Empty();
		JmenoSave.Empty();
	}
	else
	{
		SELECTITEM* item = SelectItem + inx;
		SelectInx = inx;
		SelectIndex = item->Index;
		SelectTyp = item->Typ;
		SelectAltern = item->Altern;
		Jmeno = item->Name;
		JmenoLoad = item->NameLoad;
		JmenoSave = item->NameSave;
	}
}

/////////////////////////////////////////////////////////////////////////////
// přidání položky do bufferu položek (vrací index položky, při chybě paměti vrací -1)

int SelectItemAdd(CIcon& icon, CMultiText& name, int typ, BOOL altern)
{
	SELECTITEM* item;
	int i = SelectItemNum;

	if (i >= SelectItemMax)
	{
		item = (SELECTITEM*)MemSize(SelectItem, (SelectItemMax + 64)*sizeof(SELECTITEM));
		if (item == NULL) return -1;
		SelectItem = item;
		SelectItemMax += 64;
	}

	item = SelectItem + i;

	item->Icon.Init(icon);
	item->Name.Init(name);
	item->Typ = typ;
	item->Altern = altern;
	item->Index = i;

	item->NameSave.Init(ProgramPath + Cesta + name.MultiText((int)JAZYK000));
	if (altern)
	{
		item->NameLoad.Init(ProgramPath2 + Cesta + name.MultiText((int)JAZYK000));
	}
	else
	{
		item->NameLoad.Init(item->NameSave);
	}

	SelectItemNum = i + 1;

	return i;
}

/////////////////////////////////////////////////////////////////////////////
// nastavení menu a fontů programu

void SelectSetMenu()
{
// nastavení menu
	SetNewMenu(MenuSelect);

// nastavení fontu okna
	HFONT oldfont = SelFont;
	SelFont = GetFont(&SelectFont);
	::SendMessage(SelectView, WM_SETFONT, (WPARAM)SelFont, TRUE);
	DelFont(oldfont);

// aktualizace stavového řádku
	SetStatusFont();
	UpdateStartOpen();
}


/////////////////////////////////////////////////////////////////////////////
// první inicializace při startu programu (hlavní okno je již vytvořeno)

void SelectStartInit()
{
}


/////////////////////////////////////////////////////////////////////////////
// zahájení režimu výběru souboru k editaci

void SelectInit()
{
// kontrola, zda není režim výběru souboru
	if (SelectMode) return;			// je již režim výběru souborů
	SelectMode = TRUE;
	SelEditName = FALSE;			// není editace jména souboru

// nastavení tlačítek pro režim výběru
	ToolBarAdd(ToolBarSelect, ToolBarSelectNum); // přidání tlačítek

// posun panelu nástrojů
	ToolBarResize();

// vytvoření seznamu souborů
	SelectView = ::CreateWindowEx(
		WS_EX_CLIENTEDGE,
		WC_LISTVIEW,
		NULL,
		WS_VISIBLE | WS_CHILD | 
		LVS_ALIGNTOP | LVS_ICON | LVS_SINGLESEL | LVS_SHAREIMAGELISTS 
				| LVS_SHOWSELALWAYS | LVS_EDITLABELS,
		ClientRect.left,
		ClientRect.top,
		ClientWidth,
		ClientHeight,
		MainFrame,
		(HMENU)IDN_SELECT1,
		hInstance,
		NULL);
	ASSERT(SelectView);

// nastavení nového menu a fontů
	SelectSetMenu();

// vytvoření seznamu obrázků a připojení k seznamu
	SelectImageList = ::ImageList_Create(32, 32, ILC_COLORDDB | ILC_MASK, 1, 1);
	ASSERT(SelectImageList);
	::SendMessage(SelectView, LVM_SETIMAGELIST, LVSIL_NORMAL, (LPARAM)SelectImageList);

// aktualizace rozměrů okna
	SelectOnSize();

// příprava vzorových ikon
	SelectIcon0.DelAll();
	SelectIcon0.Load(IDN_SELECT, 2);
	ASSERT(SelectIcon0.Num() == 2);

// načtení seznamu souborů
	SelectLoad();

// nastavení fokusu na aktivni okno
	SelectSetFocus();
}


/////////////////////////////////////////////////////////////////////////////
// opožděné zahájení editace

void SelectEdit()
{
	if (!IsWaitCursor)
	{
		IsWaitCursor = TRUE;
		BeginWaitCursor();
	}

	SelectTimerN = 2;
	if (SelectTimer == 0)
	{
		SelectTimer = ::SetTimer(MainFrame, SelectTimerID, TimerConst, NULL);
	}
}


/////////////////////////////////////////////////////////////////////////////
// ukončení režimu výběru souboru

void SelectTerm()
{
// kontrola, zda je režim výběru
	if (!SelectMode) return;
	SelectMode = FALSE;

// vypnutí automatické editace
	SelectTimerN = 0;
	if (SelectTimer)
	{
		::KillTimer(MainFrame, SelectTimer);
		SelectTimer = 0;
	}

// zrušení okna pro výběr souborů
	::DestroyWindow(SelectView);

// zrušení seznamu obrázků
	::ImageList_Destroy(SelectImageList);
	SelectImageList = NULL;

// zrušení hlášení o změně adresáře
	if (SelectDirChangeTimer)
	{
		::KillTimer(MainFrame, SelectDirChangeTimer);
		SelectDirChangeTimer = 0;
	}

	if (SelectDirChange != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(SelectDirChange);
		SelectDirChange = INVALID_HANDLE_VALUE;
	}

	if (SelectDirChange2 != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(SelectDirChange2);
		SelectDirChange2 = INVALID_HANDLE_VALUE;
	}

// zrušení všech tlačítek v panelu nástrojů
	ToolBarClear(0);

	SetStatusText2(EmptyText);
}


/////////////////////////////////////////////////////////////////////////////
// zjištění informace o ikoně k zobrazení

void SelectOnGetDispInfo(LV_DISPINFO* lvd)
{
// kontrola, zda je režim výběru
	if (!SelectMode) return;

// kontrola, zda je vytvořen platný seznam obrázků
	if (SelectImageList == NULL) return;

// načtení ikony
	Icon1.DelAll();

	int inx = lvd->item.lParam;
	if ((DWORD)inx < (DWORD)SelectItemNum)
	{
		Icon1.Add(SelectItem[inx].Icon);
	}
	else
	{
		Icon1.Add(SelectIcon0[0]);
	}

// vygenerování nového seznamu ikon
	::ImageList_Remove(SelectImageList, -1);
	Icon1.GenerList(FALSE, SI_BIG, SelectImageList);

// nastavení ikony k zobrazení
	lvd->item.iImage = 0;
}

/////////////////////////////////////////////////////////////////////////////
// nastavení fokusu na okno výběru

void SelectSetFocus()
{
// kontrola, zda je režim výběru
	if (!SelectMode) return;

// nastavení fokusu na okno
	::SetFocus(SelectView);
}


/////////////////////////////////////////////////////////////////////////////
// změna velikosti okna (WM_SIZE)

void SelectOnSize()
{
// kontrola, zda je režim výběru
	if (!SelectMode) return;

// posun panelu nástrojů
	ToolBarResize();

// zahájení přesouvání oken
	HDWP hdwp = ::BeginDeferWindowPos(10);

// načtení výšky stavové lišty (i když to zřejmě není potřeba)
	RECT rcStat;
	::GetWindowRect(StatusBar, &rcStat);

// posun stavového okna
	hdwp = ::DeferWindowPos(hdwp, StatusBar, NULL, ClientRect.left, ClientRect.bottom, ClientWidth, rcStat.bottom - rcStat.top,
			SWP_DRAWFRAME | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);

// inicializace klientské oblasti
	InitClientRect();

// aktualizace předělů stavového okna
	SetStatusWidth(SelectStatusWidth);

// změna velikosti okna výběru
	hdwp = ::DeferWindowPos(hdwp, SelectView, NULL, ClientRect.left, ClientRect.top, ClientWidth, ClientHeight,
			SWP_DRAWFRAME | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);

// konec přesouvání oken
	::EndDeferWindowPos(hdwp);

// přearanžování ikon v seznamu
	::SendMessage(SelectView, LVM_ARRANGE, LVA_ALIGNTOP, 0);

// aktualizace stavového řádku
	UpdateStartOpen();

// překreslení okna
	::UpdateWindow(MainFrame);
}


/////////////////////////////////////////////////////////////////////////////
// načtení ALIAS jména souboru

CText ReadAliasName(int lang, LPCTSTR key, LPCTSTR aliases)
{
	TCHAR buf[1024];
	buf[0] = 0;
	return CText(buf, ::GetPrivateProfileString(JazykInfo[lang].LangIDini, key, EmptyText, buf, 1023, aliases));
}

/////////////////////////////////////////////////////////////////////////////
// porovnání dvou položek seznamu

int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	if (((DWORD)lParam1 >= (DWORD)SelectItemNum) || 
			((DWORD)lParam2 >= (DWORD)SelectItemNum))
	{
		return 0;
	}

	SELECTITEM* item1 = SelectItem + (int)lParam1;
	SELECTITEM* item2 = SelectItem + (int)lParam2;

	if (item1->Typ < item2->Typ) return -1;
	if (item1->Typ > item2->Typ) return 1;

	if (item1->Name.MultiText() < item2->Name.MultiText()) return -1;
	if (item1->Name.MultiText() > item2->Name.MultiText()) return 1;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// načtení seznamu souborů

void SelectLoad()
{
// příznak plnění okna
	SelectFill = TRUE;

// inicializace seznamu
	SelectItemDelAll();

// není vybraná položka
	SelectInx = -1;
	SelectIndex = -1;
	SelectTyp = TYP0;
	SelectAltern = FALSE;

// normalizace cesty - ořezání mezer a "\" ze začátku a konce
	Cesta.TrimLeft();
	Cesta.TrimRight();
	while (Cesta.Get(0) == _T('\\'))
	{
		Cesta.Delete(0,1);
		Cesta.TrimLeft();
	}
	while (Cesta.LastChar() == _T('\\'))
	{
		Cesta.Delete(Cesta.Length()-1);
		Cesta.TrimRight();
	}

// vytvoření adresáře pro programy
	CreatePath(ProgramPath);
	CreatePath(ProgramPath + Cesta);

// zjištění platnosti cesty
	BOOL ok = FALSE;
	int attrib;
	while (!ok && Cesta.IsNotEmpty())
	{
		attrib = (int)::GetFileAttributes(ProgramPath + Cesta);
		if ((attrib != -1) && ((attrib & FILE_ATTRIBUTE_DIRECTORY) != 0))
		{
			ok = TRUE;
		}
		else
		{
			Cesta.Delete(Cesta.RevFind(_T('\\')));
			Cesta.TrimRight();
		}
	}

// zajištění znaku "\" na konci cesty
	if (Cesta.IsNotEmpty()) Cesta.Add(_T('\\'));

// cesta do adresáře, příznak alternativního adresáře
	CText path = ProgramPath + Cesta;
	BOOL altern = FALSE;

// příprava souborů ALIASES
	SelectAliases = path + _T("ALIASES.INI");
	SelectAliases2 = ProgramPath2 + Cesta + _T("ALIASES.INI");

// vypnutí překreslování okna
	::SendMessage(SelectView, WM_SETREDRAW, FALSE, 0);

// zrušení všech položek v seznamu
	::SendMessage(SelectView, LVM_DELETEALLITEMS, 0, 0);

// proměnné pro načtení seznamu
	WIN32_FIND_DATA wfd;			// struktura pro hledání souborů
	HANDLE handle;					// handle hledání souborů
	CText jmeno;					// jméno nalezeného souboru
	LV_ITEM lvi;					// vkládaná položka do seznamu
	CIcon icon;						// ikona
	CFileMap file;					// paměťové mapovaný soubor
	CMultiText name;				// jméno položky
	CText aliases(SelectAliases);	// jméno souboru aliases
	BOOL aliases2 = (ProgramPath2.IsNotEmpty() && ((int)::GetFileAttributes(SelectAliases2) != -1)); // povoleno načtení aliases 2

// příprava položky seznamu
	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lvi.iSubItem = 0;
	lvi.iImage = I_IMAGECALLBACK;

// doplnění položky "..", je-li podadresář
	if (Cesta.IsNotEmpty())
	{
		name.MultiText(JAZYK000, CText(_T("..")));
		lvi.lParam = SelectItemAdd(SelectIcon0[ICONUPDIR], name, TYPUPDIR, false);
		lvi.iItem = lvi.lParam;
		lvi.pszText = (LPTSTR)(LPCTSTR)(name.MultiText((int)JAZYK000));
		::SendMessage(SelectView, LVM_INSERTITEM, 0, (LPARAM)&lvi);
	}

// zde bude druhý pokus - pro vzorový adresář
DRUHYPOKUS:

// načtení všech podadresářů
	handle = ::FindFirstFile(path + _T("*.*"), &wfd);
	if (handle != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) &&
				((wfd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0))
			{
				jmeno = wfd.cFileName;

				if ((jmeno != _T(".")) && (jmeno != _T("..")))
				{
					name.Empty();
					name.MultiText(JAZYK000, jmeno);

// načtení aliases jména adresáře
					BOOL nameok = FALSE;

					for (int lang = 0; lang < JAZYKNUM; lang++)
					{
						if (lang != JAZYK000)
						{
							name.MultiText(lang, ReadAliasName(lang, jmeno, aliases));
							if (name.MultiText(lang).IsNotEmpty()) nameok = TRUE;
						}
					}

					if (!nameok && aliases2)
					{
						for (lang = 0; lang < JAZYKNUM; lang++)
						{
							if (lang != JAZYK000)
							{
								name.MultiText(lang, ReadAliasName(lang, jmeno, SelectAliases2));
							}
						}
					}

// test duplikace položky ze vzorového adresáře
					BOOL duplik = FALSE;
					SELECTITEM* item = SelectItem;
					int i = 0;

					if (altern)
					{
						CText name0 = name.MultiText((int)JAZYK000);

						for (; i < SelectItemNum; i++)
						{
							if (!item->Altern && (item->Typ == TYPSUBDIR) && (item->Name.MultiText((int)JAZYK000) == name0))
							{
								duplik = TRUE;
								break;
							}
							item++;
						}		   
					}

// přidání položky do seznamu
					if (!duplik)
					{
						i = SelectItemAdd(SelectIcon0[ICONSUBDIR], name, TYPSUBDIR, altern);
						lvi.lParam = i;
						lvi.iItem = i;
						lvi.pszText = (LPTSTR)(LPCTSTR)(name.MultiText());
						::SendMessage(SelectView, LVM_INSERTITEM, 0, (LPARAM)&lvi);
					}
					else
					{
						item->Name = name;
						item->NameLoad = ProgramPath2 + Cesta + name.MultiText((int)JAZYK000);
						item->Altern = TRUE;
						lvi.lParam = i;
						lvi.iItem = i;
						lvi.pszText = (LPTSTR)(LPCTSTR)(name.MultiText());
						::SendMessage(SelectView, LVM_SETITEM, 0, (LPARAM)&lvi);
					}
				}
			}
			
		} while (::FindNextFile(handle, &wfd));
		::FindClose(handle);
	}

// vyhledání souborů
	handle = ::FindFirstFile(path + _T("*.exe"), &wfd);
	if (handle != INVALID_HANDLE_VALUE)
	{
		do
		{

// otevření souboru
			jmeno = wfd.cFileName;

			if (file.Open(path + jmeno))
			{

// příprava NT záhlaví souboru EXE
				IMAGE_NT_HEADERS* hdr = file.NTHeader();
				if (hdr != NULL)
				{

// nalezení NT sekce Petříka
					IMAGE_SECTION_HEADER* sec = file.NTSection(hdr, ".petprg");
					if (sec != NULL)
					{

// adresa záhlaví Petříka
						PETHEAD* buf = (PETHEAD*)(file.Adr() + sec->PointerToRawData);
						if (file.IsValid(buf, sizeof(NewHead)))
						{

// kontrola záhlaví Petříka
							if ((buf->Ident[0] == 'P') &&
								(buf->Ident[1] == 'E') &&
								(buf->Ident[2] == 'T') &&
								(buf->Verze == 1) &&
								(buf->Pocet < 20))
							{

// uzavření souboru (aby se uvolnil pro načtení ikony)
								file.Close();

// načtení ikony ze souboru
								if (icon.Extract(path + jmeno))
								{

// úschova jména programu
									name.Empty();
									name.MultiText(JAZYK000, jmeno);

									BOOL nameok = FALSE;
// načtení aliases
									for (int lang = 0; lang < JAZYKNUM; lang++)
									{
										if (lang != JAZYK000)
										{
											name.MultiText(lang, ReadAliasName(lang, jmeno, aliases));
											if (name.MultiText(lang).IsNotEmpty()) nameok = TRUE;
										}
									}

									if (!nameok && aliases2)
									{
										for (lang = 0; lang < JAZYKNUM; lang++)
										{
											if (lang != JAZYK000)
											{
												name.MultiText(lang, ReadAliasName(lang, jmeno, SelectAliases2));
												if (name.MultiText(lang).IsNotEmpty()) nameok = TRUE;
											}
										}
									}

// zrušení přípony jména souboru
									if ((jmeno.Length() > 4) && (jmeno[jmeno.Length() - 4] == _T('.')))
									{
										jmeno.Delete(jmeno.Length()-4);
									}

									if (!nameok) name.MultiText(jmeno);

// test duplikace položky (vzorový adresář)
									BOOL duplik = FALSE;

									if (altern)
									{
										CText name0 = name.MultiText((int)JAZYK000);
										SELECTITEM* item = SelectItem;

										for (int i = 0; i < SelectItemNum; i++)
										{
											if (!item->Altern && (item->Typ == TYPFILE) && (item->Name.MultiText((int)JAZYK000) == name0))
											{
												duplik = TRUE;
												break;
											}
											item++;
										}		   
									}

// přidání položky do seznamu
									if (!duplik)
									{
										int i = SelectItemAdd(icon, name, TYPFILE, altern);
										lvi.lParam = i;
										lvi.iItem = i;
										lvi.pszText = (LPTSTR)(LPCTSTR)(name.MultiText());
										::SendMessage(SelectView, LVM_INSERTITEM, 0, (LPARAM)&lvi);
									}
								}
								continue;
							}
						}
					}
				}

// uzavření souboru
				file.Close();
			}

// nalezení dalšího souboru
		} while (::FindNextFile(handle, &wfd));
		::FindClose(handle);
	}

// prohledání vzorového adresáře
	if (!altern && ProgramPath2.IsNotEmpty())
	{
		path = ProgramPath2 + Cesta;
		aliases = SelectAliases2;
		altern = TRUE;
		aliases2 = FALSE;
		goto DRUHYPOKUS;
	}

// přednastavení na první jméno, není-li žádné jméno přednastaveno
	if (Jmeno.MultiText((int)JAZYK000).IsEmpty())
	{
		SelectSelect(0);
	}
	else
	{

// nalezení položky se zadaným jménem
		int i;
		for (i = 0; i < SelectItemNum; i++)
		{
			if (Jmeno.MultiText((int)JAZYK000) == SelectItem[i].Name.MultiText((int)JAZYK000)) break;
		}

// při neúspěchu se použije první jméno v seznamu
		if (i >= SelectItemNum) i = 0;

// nastavení indexu a jména položky
		SelectSelect(i);
	}

// nastavení kurzoru položky
	if ((DWORD)SelectInx < (DWORD)SelectItemNum)
	{
		lvi.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
		lvi.state = LVIS_FOCUSED | LVIS_SELECTED;
		lvi.mask = LVIF_STATE;
		lvi.iSubItem = 0;
		lvi.iItem = SelectInx;
		::SendMessage(SelectView, LVM_SETITEMSTATE, SelectInx, (LPARAM) &lvi);
	}

// setřídění položek
	::SendMessage(SelectView, LVM_SORTITEMS, 0, (LPARAM)(PFNLVCOMPARE)CompareFunc);

// zjištění nových indexů položek
	for (int i = 0; i < SelectItemNum; i++)
	{
		SelectItem[i].Index = -1;
	}

	ASSERT(SelectItemNum == ::SendMessage(SelectView, LVM_GETITEMCOUNT, 0, 0));

	for (i = 0; i < SelectItemNum; i++)
	{
		lvi.mask = LVIF_PARAM;
		lvi.iSubItem = 0;
		lvi.iItem = i;
		lvi.lParam = -1;
		::SendMessage(SelectView, LVM_GETITEM, 0, (LPARAM)&lvi);
		if ((DWORD)lvi.lParam < (DWORD)SelectItemNum)
		{
			SelectItem[lvi.lParam].Index = i;
		}
	}

// zapnutí překreslování okna
	::SendMessage(SelectView, WM_SETREDRAW, TRUE, 0);

// aktualizace zobrazení jména v titulku okna
	ZobrazJmeno();

// aktualizace tlačítek a položek menu
	UpdateStartOpen();

// zrušení hlášení o změně adresáře
	if (SelectDirChange != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(SelectDirChange);
		SelectDirChange = INVALID_HANDLE_VALUE;
	}

// zrušení hlášení o změně adresáře
	if (SelectDirChange2 != INVALID_HANDLE_VALUE)
	{
		::FindCloseChangeNotification(SelectDirChange2);
		SelectDirChange2 = INVALID_HANDLE_VALUE;
	}

// zahájení hlášení změn adresáře
	SelectDirChange = ::FindFirstChangeNotification(
		ProgramPath + Cesta,			// cesta k adresáři
		FALSE,							// nehlásit podadredáře
		FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME);

	if (ProgramPath2.IsNotEmpty())
	{
		SelectDirChange2 = ::FindFirstChangeNotification(
			ProgramPath2 + Cesta,			// cesta k adresáři
			FALSE,							// nehlásit podadredáře
			FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME);
	}

	if (SelectDirChangeTimer == 0)
	{
		SelectDirChangeTimer = ::SetTimer(MainFrame, SelectDirChangeTimerID, 2000, NULL);
	}

// zajištění překreslení okna
	TimeRepaint(-1);

// ukončení plnění okna
	SelectFill = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// aktualizace volby Start, Open, Rename, Kopie a Delete

void UpdateStartOpen()
{
	CText txt;
	CText txt2;

// start, otevření, rušení, kopie - musí být platné jméno pod kurzorem
	BOOL enable = (SelectTyp != TYP0);
	EnableCommand(IDN_START, enable);
	EnableCommand(IDN_OPEN, enable);

// položku ".." ani vzorový program nelze rušit, přejmenovat ani kopírovat
	if (SelectTyp == TYPUPDIR) enable = FALSE;
	EnableCommand(IDN_RENAME, enable && !SelectAltern);
	EnableCommand(IDN_DELETE, enable && !SelectAltern);

// adresář nelze kopírovat
	if (SelectTyp != TYPFILE) enable = FALSE;
	EnableCommand(IDN_NEWCOPY, enable);

// zkrácení stavového řádku
	int stat2 = SelectStatusWidth - StatusWidth;

// příprava data a času souboru/adresáře
	if ((SelectTyp == TYPSUBDIR) || (SelectTyp == TYPFILE))
	{
		WIN32_FIND_DATA wfd;
		HANDLE fnd;
		fnd = ::FindFirstFile(JmenoLoad, &wfd);

		if (fnd != INVALID_HANDLE_VALUE)
		{

// není-li adresář, je větší rezerva ve stavovém řádku (není velikost)
			if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
			{
				stat2 -= 80;
			}			

// příprava lokálního data a času
			FILETIME loc;
			::FileTimeToLocalFileTime(&wfd.ftLastWriteTime, &loc);

			SYSTEMTIME sys;
			::FileTimeToSystemTime(&loc, &sys);

// datum souboru
			if (stat2 < 5)
			{
				txt.Load(IDN_DATUM);
				txt.Add(_T(' '));
			}

			switch (Jazyk)
			{
// polsky
			case JAZYKPOL:
				txt2.Int(sys.wYear);
				txt.Add(txt2);
				txt.Add(_T('-'));
				txt2.Int(sys.wMonth);
				txt.Add(txt2);
				txt.Add(_T('-'));
				txt2.Int(sys.wDay);
				txt.Add(txt2);
				break;

// anglicky
			case JAZYKENG:
				txt2.Int(sys.wMonth);
				txt.Add(txt2);
				txt.Add(_T('/'));
				txt2.Int(sys.wDay);
				txt.Add(txt2);
				txt.Add(_T('/'));
				txt2.Int(sys.wYear);
				txt.Add(txt2);
				break;

// francouzsky, španělsky, italsky
			case JAZYKFRA:
			case JAZYKSPA:
			case JAZYKITA:
				txt2.Int(sys.wDay);
				txt.Add(txt2);
				txt.Add(_T('/'));
				txt2.Int(sys.wMonth);
				txt.Add(txt2);
				txt.Add(_T('/'));
				txt2.Int(sys.wYear);
				txt.Add(txt2);
				break;

// česky, slovensky, německy
			default:
				txt2.Int(sys.wDay);
				txt.Add(txt2);
				txt.Add(_T('.'));
				txt2.Int(sys.wMonth);
				txt.Add(txt2);
				txt.Add(_T('.'));
				txt2.Int(sys.wYear);
				txt.Add(txt2);
			}

// čas souboru
			txt.Add(_T(", "));
			if (stat2 < 5)
			{
				txt2.Load(IDN_CAS);
				txt.Add(txt2);
				txt.Add(_T(' '));
			}

			txt2.Int(sys.wHour);
			txt.Add(txt2);
			txt.Add(_T(':'));
			if (sys.wMinute < 10)
			{
				txt.Add(_T('0'));
			}
			txt2.Int(sys.wMinute);
			txt.Add(txt2);

// velikost souboru
			if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				txt.Add(_T(", "));
				if (stat2 < 60)
				{
					txt2.Load(IDN_VELIKOST);
					txt.Add(txt2);
					txt.Add(_T(' '));
				}
				txt2.Int(((int)wfd.nFileSizeLow + 1023)/1024);
				txt.Add(txt2);
				txt.Add(_T(' '));
				txt2.Load(IDN_KB);
				txt.Add(txt2);
			}

			::FindClose(fnd);
			txt.Add(_T(" / "));
		}
	}

// volné místo disku
	GetDiskSpace(ProgramPath);

	if (stat2 < 90)
	{
		txt2.Load(IDN_VOLNE);
		txt.Add(txt2);
		txt += _T(' ');
	}
	txt2.Int(Round0((double)DiskFree/(1024*1024)));
	txt.Add(txt2);
	txt.Add(_T(' '));
	txt2.Load(IDN_MB);
	txt.Add(txt2);

// text data souboru
	SetStatusText2(txt);
}


/////////////////////////////////////////////////////////////////////////////
// změna aktuální položky (LVN_ITEMCHANGED)

void SelectOnChange(HWND hWnd, NM_LISTVIEW* nmlv)
{
// kontrola, zda je režim výběru
	if (!SelectMode) return;

// kontrola, zda je zpráva od okna výběru
	if (hWnd != SelectView) return;

// kontrola platnosti položky
	if ((DWORD)nmlv->lParam < (DWORD)SelectItemNum)
	{

// kontrola, zda je změna zaměření ze staré položky na novou
		if (((nmlv->uNewState & LVIS_FOCUSED) != 0) &&
			((nmlv->uOldState & LVIS_FOCUSED) == 0))
		{

// nastavení nového jména souboru
			if (!SelectFill) SelectSelect(nmlv->lParam);
			ZobrazJmeno();
		}
	}

// aktualizace tlačítek a položek menu
	UpdateStartOpen();
}


/////////////////////////////////////////////////////////////////////////////
// test, zda je spustitelný zavaděč programu (TRUE=je)

BOOL SelectTestExe()
{
	if (SelectTyp != TYPFILE) return FALSE;

	CFileMap file;

// otevření souboru
	if (file.Open(JmenoLoad))
	{

// příprava NT záhlaví souboru EXE
		IMAGE_NT_HEADERS* hdr = file.NTHeader();
		if (hdr != NULL)
		{

// kontrola velikosti kódu programu
			return (hdr->OptionalHeader.SizeOfCode > 0x4000);
		}
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// otevření souboru k editaci

void SelectOnOpen()
{
// kontrola, zda je režim výběru
	if (!SelectMode) return;

// pro adresář se provede změna adresáře
	if (SelectTyp != TYPFILE)
	{
		SelectOnStart();
	}

// pro soubor otevření k editaci
	else
	{

// zahájení režimu editace souborů
		SelectEdit();
	}
}


/////////////////////////////////////////////////////////////////////////////
// dvojklik myší - start programu

void SelectOnDblClk(HWND hWnd)
{
// kontrola, zda je režim výběru
	if (!SelectMode) return;

// kontrola, zda je zpráva od okna výběru
	if (hWnd != SelectView) return;

// start aktivního programu
	SelectOnStart();
}


/////////////////////////////////////////////////////////////////////////////
// start programu nebo změna adresáře

void SelectOnStart()
{
// kontrola, zda je režim výběru
	if (!SelectMode) return;

// pouze je-li nějaká platná položka
	if (SelectTyp != TYP0)
	{

// kontrola, zda to je adresář
		if (SelectTyp != TYPFILE)
		{

// vynoření z adresáře
			if (SelectTyp == TYPUPDIR)
			{
				Cesta.Delete(Cesta.Length()-1);		// odstranění koncového "\"
				int index = Cesta.RevFind(_T('\\'))+1;	// nalezení předešlého "\" nebo 0
				Jmeno.Empty();
				Jmeno.MultiText(JAZYK000, Cesta.Right(Cesta.Length()-index)); // poslední adresář
				Cesta.Delete(index);				// odstranění posledního adresáře
			}

// vnoření do adresáře
			else
			{
				Cesta.Add(Jmeno.MultiText((int)JAZYK000) + _T('\\')); // nová cesta
				Jmeno.Empty();
				Jmeno.MultiText(JAZYK000, CText(_T("..")));
			}
			SelectLoad();							// znovunačtení adresáře
		}

// je to soubor - start programu
		else
		{
			if (SelectTestExe())
			{
				SetJazykReg(LangID);

				Exec(CText(_T('"')) + JmenoLoad + _T('"'), 
					ProgramPath + Cesta,
					FALSE);	// spuštění programu
			}

// bez zavaděče se program otevře k editaci
			else
			{
				SelectEdit();
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// nalezení jedinečného nového jména k zadání (over = přeskakovaná položka, -1=není)

void SelectNewNameUnique(int over)
{
	int nn = 0;

	for (int i = 0; i < SelectItemNum; i++)
	{
		if ((SelectItem[i].Name.MultiText() == SelectNewText) && (over != i))
		{
			nn++;
			if (nn >= 10000) break;
			SelectNewText.SetNumObj(nn);
			SelectNewMultiText.SetNumObj(nn);
			i = -1;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// nalezení jedinečného zadaného jména souboru/adresáře (over = přeskakovaná položka, -1=není)

CText SelectNewFileUnique(int over)
{
// příprava jedinečného jména (pro nezkrácená jména se nastavuje v cyklu)
	int nn = 0;
	if (ShortName)
	{
		nn = 1;
		SelectNewNameUnique(over);
	}

// příprava krátkého jména souboru
	CText shortname = SelectNewText;
	shortname.KorigShort();

// nalezení neexistujícího jména souboru/adresáře
	CText txtint;
	CText txt = shortname;

	CText overname;

	if ((DWORD)over < (DWORD)SelectItemNum)
	{
		overname = SelectItem[over].Name.MultiText((int)JAZYK000);
		if (SelectItem[over].Typ == TYPFILE)
		{
			overname.Delete(overname.Length() - 4);
		}
	}

	while (((int)::GetFileAttributes(ProgramPath + Cesta + txt + TextExe) != -1) ||
			((int)::GetFileAttributes(ProgramPath + Cesta + txt) != -1) ||
			(ProgramPath2.IsNotEmpty() && 
					(((int)::GetFileAttributes(ProgramPath2 + Cesta + txt + TextExe) != -1) ||
					((int)::GetFileAttributes(ProgramPath2 + Cesta + txt) != -1))
				)
			)
	{
		nn++;
		if (nn >= 10000) break;

		if (txt == overname) break;

		if (ShortName)
		{
			txtint.Int(nn);

			while ((shortname.Length() + txtint.Length()) > 8)
			{
				shortname.Delete(shortname.Length() - 1);
			}
			txt = shortname + txtint;
		}
		else
		{
			txt = shortname;
			txt.SetNumObj(nn);
			SelectNewText.SetNumObj(nn);
			SelectNewMultiText.SetNumObj(nn);
		}
	}
	return txt;
}


/////////////////////////////////////////////////////////////////////////////
// obsluha dialogu

BOOL CALLBACK SelectDialogProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_INITDIALOG:
		{
			CenterWindow(hWnd, MainFrame);

			CText txt;
			txt.Load(lParam);
			txt.SetWindowTextFont(hWnd);

			switch (lParam)
			{
			case IDN_SELECT_CREATE:
				txt.Load(IDN_CANCEL0);
				txt.SetDialogTextFont(hWnd, IDCANCEL);

				DialogCustomText(hWnd, IDN_SELECT_CREATE1);
				DialogCustomText(hWnd, IDN_JINE_JMENO);
				break;

			case IDN_SELECT_DEL:
				txt.Load(IDN_CANCEL0);
				txt.SetDialogTextFont(hWnd, IDCANCEL);

				DialogCustomText(hWnd, IDN_SELECT_DEL1);
				DialogCustomText(hWnd, IDN_SELECT_DEL2);
				break;

			case IDN_SELECT_DELDIR:
				txt.Load(IDN_CANCEL0);
				txt.SetDialogTextFont(hWnd, IDCANCEL);

				DialogCustomText(hWnd, IDN_SELECT_DELDIR1);
				break;

			case IDN_SELECT_DELDIRERROR:
				DialogCustomText(hWnd, IDN_SELECT_DELDIRERROR1);
				break;

			case IDN_SELECT_DELERROR:
				DialogCustomText(hWnd, IDN_SELECT_DELERROR1);
				break;

			case IDN_SELECT_DIR:
				txt.Load(IDN_CANCEL0);
				txt.SetDialogTextFont(hWnd, IDCANCEL);

				DialogCustomText(hWnd, IDN_SELECT_DIR1);
				break;

			case IDN_SELECT_DIRCREATE:
				txt.Load(IDN_CANCEL0);
				txt.SetDialogTextFont(hWnd, IDCANCEL);

				DialogCustomText(hWnd, IDN_SELECT_DIRCREATE1);
				DialogCustomText(hWnd, IDN_JINE_JMENO);
				break;

			case IDN_SELECT_DIREXIST:
				txt.Load(IDN_CANCEL0);
				txt.SetDialogTextFont(hWnd, IDCANCEL);

				DialogCustomText(hWnd, IDN_SELECT_DIREXIST1);
				DialogCustomText(hWnd, IDN_JINE_JMENO);
				break;

			case IDN_SELECT_EXIST:
				txt.Load(IDN_CANCEL0);
				txt.SetDialogTextFont(hWnd, IDCANCEL);

				DialogCustomText(hWnd, IDN_SELECT_EXIST1);
				DialogCustomText(hWnd, IDN_JINE_JMENO);
				break;

			case IDN_SELECT_NEW:
				txt.Load(IDN_CANCEL0);
				txt.SetDialogTextFont(hWnd, IDCANCEL);

				DialogCustomText(hWnd, IDN_SELECT_NEW1);
				break;

			case IDN_SELECT_NEWCOPY:
				txt.Load(IDN_CANCEL0);
				txt.SetDialogTextFont(hWnd, IDCANCEL);

				DialogCustomText(hWnd, IDN_SELECT_NEWCOPY1);
				break;
			}

			SelectNewEdit = ::GetDlgItem(hWnd, IDN_SELECT_EDIT);
			SetFontBezny(SelectNewEdit);
			::SetWindowText(SelectNewEdit, SelectNewText);
			if ((lParam == IDN_SELECT_DEL) || (lParam == IDN_SELECT_DELDIR))
			{
				::SetFocus(::GetDlgItem(hWnd, IDCANCEL));
				return FALSE;
			}
		}
		return TRUE;

	case WM_DRAWITEM:				// 0x02B
		if (DialogDraw(hWnd, lParam)) return TRUE;
		return FALSE;

	case WM_COMMAND:
		switch (HIWORD(wParam))
		{
		case 0:
		case 1:
			switch (LOWORD(wParam))
			{
			case IDOK:
			case IDCANCEL:
				SelectNewText.GetWindowText(SelectNewEdit);
				::EndDialog(hWnd, LOWORD(wParam));
				return TRUE;
			}
			break;

//		case EN_CHANGE:						// není potřeba již obsluhovat (používají se aliases)
//			if (!SelectNewFill)
//			{
//				SelectNewFill = TRUE;
//	
//				int selstart;
//				::SendMessage(SelectNewEdit, EM_GETSEL, (WPARAM)&selstart, NULL);
//				SelectNewText.GetWindowText(SelectNewEdit);
//
//				int oldlen = SelectNewText.Length();
//				selstart = SelectNewText.FileName(selstart);
//  
//				if (oldlen != SelectNewText.Length())
//				{
//					SelectNewText.SetWindowText(SelectNewEdit);
//					::SendMessage(SelectNewEdit, EM_SETSEL, selstart, selstart);
//				}
//
//				SelectNewFill = FALSE;
//			}
//			break;
		}
		break;

	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření nového souboru

// velikost stránky zarovnávání paměti
#ifndef _M_ALPHA
#define	PAGESIZE		0x1000			// velikost alokační stránky pro ostatní procesory (4096)
#define PAGEFILE		0x1000			// velikost stránky v souboru
#else
#define	PAGESIZE		0x2000			// velikost alokační stránky pro procesor Alpha (8192)
#define PAGEFILE		0x2000			// velikost stránky v souboru
#endif

void SelectOnNew()
{
// kontrola, zda je režim výběru
	if (!SelectMode) return;

// příprava zadaného jména souboru
	if (SelectTyp == TYPFILE)
	{
		SelectNewText = Jmeno.MultiText();
	}
	else
	{
		SelectNewText.Empty();
	}

// implicitní jméno, není-li žádný vzor
	if (SelectNewText.IsEmpty()) SelectNewText.Load(IDN_NEWFILE_NAME);

// nalezení neexistujícího souboru
	SelectNewNameUnique(-1);

// cyklus opakování zadání
	int dialogID = IDN_SELECT_NEW;

	for (;;)
	{

// zadání jména programu
		if (::DialogBoxParam(
					hInstance,
					MAKEINTRESOURCE(dialogID),
					MainFrame,
					(DLGPROC)SelectDialogProc,
					(LPARAM)dialogID) != IDOK) return;

// přednastavení příštího dialogu
		dialogID = IDN_SELECT_CREATE;

// korekce zadaného textu
		SelectNewText.TrimLeft();
		SelectNewText.TrimRight();
		if (SelectNewText.IsEmpty()) return;

// příprava jména souboru
		CText txt = SelectNewFileUnique(-1);

// jméno souboru
		if (ShortName)
		{
			txt += TextExe2;
		}
		else
		{
			txt += TextExe;
		}
		CText filename = ProgramPath + Cesta + txt;

// vytvoření souboru
		HANDLE file = ::CreateFile(filename, GENERIC_WRITE,
			0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

// chyba vytvoření - test, zda soubor již existuje (jinak hlášena neznámá chyba)
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! to již nemusí být !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		if (file == INVALID_HANDLE_VALUE)
		{
			if ((int) :: GetFileAttributes(filename) != -1)
			{
				dialogID = IDN_SELECT_EXIST;
			}
		}

// příprava malého zavaděče programu
		else
		{
			CResource loader;
			if (loader.OpenCopy(IDN_LOADERM, _T("LOADER")))
			{

// adresa záhlaví NT
				IMAGE_NT_HEADERS* hdr = loader.NTHeader();
				if (hdr != NULL)
				{

// offset začátku dat v souboru (= zarovnaná velikost zavaděče)
					int SaveDataOff = (loader.Size() + (PAGEFILE-1)) & ~(PAGEFILE-1);

// virtuální adresa dat
					int SaveDataVirt = (hdr->OptionalHeader.SizeOfImage + PAGESIZE-1) & ~(PAGESIZE-1);

// velikost obrazu všech dat
					hdr->OptionalHeader.SizeOfImage = (SaveDataVirt + sizeof(NewHead) + PAGESIZE-1) & ~(PAGESIZE-1);

// adresa vkládané sekce
					IMAGE_SECTION_HEADER* sec = (IMAGE_SECTION_HEADER*)((BYTE*)(&hdr->OptionalHeader) 
							+ hdr->FileHeader.SizeOfOptionalHeader + hdr->FileHeader.NumberOfSections 
							* sizeof(IMAGE_SECTION_HEADER));

// zvýšení čítače sekcí
					hdr->FileHeader.NumberOfSections++;

// nastavení sekce
					MemCopy(sec, &PetProgHeader, sizeof(IMAGE_SECTION_HEADER));

// nastavení offsetu začátku dat v souboru
					sec->PointerToRawData = SaveDataOff;
					sec->VirtualAddress = SaveDataVirt;

// zarovnaná velikost souboru
					int oldsize = SaveDataOff + sizeof(NewHead);
					int newsize = (oldsize + 0x1ff) & ~0x1ff;

// velikost dat programu v souboru
					sec->SizeOfRawData = newsize - SaveDataOff;

// virtuální velikost dat programu
					sec->Misc.VirtualSize = sizeof(NewHead);

// zápis zavaděče
					DWORD writen;				// počet zapsaných dat
					if ((!::WriteFile(file, loader.Adr(), loader.Size(), &writen, NULL)) ||
						(loader.Size() != (int)writen))
					{
						loader.CloseCopy();
					}					
					else
					{

// zarovnání délky zavaděče na souborovou stránku
						int i = SaveDataOff - loader.Size();
						if (i > 0)
						{
							BYTE* dst = (BYTE*)MemGet(i);
							if (dst != NULL)
							{
								MemFill(dst, i, 0);
								::WriteFile(file, dst, i, &writen, NULL);
							}
						}

// zrušení bufferu zavaděče
						loader.CloseCopy();

// zápis programu
						::WriteFile(file, &NewHead, sizeof(NewHead), &writen, NULL);

// zarovnání na sektor
						i = newsize - oldsize;
						if (i > 0)
						{
							BYTE* dst = (BYTE*)MemGet(i);
							if (dst != NULL)
							{
								MemFill(dst, i, 0);
								::WriteFile(file, dst, i, &writen, NULL);
							}
						}

// uzavření souboru
						::CloseHandle(file);

// nastavení nového jména souboru
						Jmeno.Empty();
						Jmeno.MultiText(JAZYK000, txt);
						Jmeno.MultiText(SelectNewText);
						JmenoLoad = filename;
						JmenoSave = filename;
						::WritePrivateProfileString(JazykInfo[Jazyk].LangIDini, txt, SelectNewText, SelectAliases);
						ZobrazJmeno();

// zahájení editace programu (nepoužívat SelectOnOpen - nesouhlasí parametry aktivního programu!)
						SelectEdit();
						return;
					}
				}
			}

// uzavření a zrušení souboru (a pokračování chybovým hlášením)
			::CloseHandle(file);
			::DeleteFile(filename);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// operace PASTE při editaci jména souboru

void SelectOnPaste()
{
// kontrola, zda je režim výběru
	if (!SelectMode) return;

// editace jména souboru
	if (SelEditName)
	{
		::SendMessage(SelEditNameWnd, WM_PASTE, 0, 0);
		return;
	}
}


/////////////////////////////////////////////////////////////////////////////
// operace CUT při editaci jména souboru

void SelectOnCut()
{
// kontrola, zda je režim výběru
	if (!SelectMode) return;

// editace jména souboru
	if (SelEditName)
	{
		::SendMessage(SelEditNameWnd, WM_CUT, 0, 0);
		return;
	}
}

/////////////////////////////////////////////////////////////////////////////
// operace UNDO při editaci jména souboru

void SelectOnUndo()
{
// kontrola, zda je režim výběru
	if (!SelectMode) return;

// editace jména souboru
	if (SelEditName)
	{
		::SendMessage(SelEditNameWnd, EM_UNDO, 0, 0);
		return;
	}
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření nového souboru kopií

void SelectOnNewCopy()
{
// kontrola, zda je režim výběru
	if (!SelectMode) return;

// editace jména souboru
	if (SelEditName)
	{
		::SendMessage(SelEditNameWnd, WM_COPY, 0, 0);
		return;
	}

// kontrola, zda je platné jméno souboru pod kurzorem
	if (SelectTyp != TYPFILE) return;

// příprava zadaného jména souboru
	SelectNewText = Jmeno.MultiText();
	SelectNewMultiText = Jmeno;

// nalezení neexistujícího souboru
	SelectNewNameUnique(-1);

// cyklus opakování zadání
	int dialogID = IDN_SELECT_NEWCOPY;

	for (;;)
	{

// zadání jména programu
		if (::DialogBoxParam(
					hInstance,
					MAKEINTRESOURCE(dialogID),
					MainFrame,
					(DLGPROC)SelectDialogProc,
					(LPARAM)dialogID) != IDOK) return;

// přednastavení příštího dialogu
		dialogID = IDN_SELECT_CREATE;

// korekce zadaného textu
		SelectNewText.TrimLeft();
		SelectNewText.TrimRight();
		if (SelectNewText.IsEmpty()) return;

// příprava jména souboru
		CText txt = SelectNewFileUnique(-1);

// jméno souboru
		if (ShortName)
		{
			txt += TextExe2;
		}
		else
		{
			txt += TextExe;
		}
		CText newname = ProgramPath + Cesta + txt;

// kopie souboru
		if (::CopyFile(JmenoLoad, newname, TRUE))
		{

// zrušení atributu R/O u kopie
			::SetFileAttributes(newname, FILE_ATTRIBUTE_NORMAL);

// nastavení nového jména souboru
			Jmeno = SelectNewMultiText;
			Jmeno.MultiText(JAZYK000, txt);
			Jmeno.MultiText(SelectNewText);
			JmenoLoad = newname;
			JmenoSave = newname;

			for (int lang = 0; lang < JAZYKNUM; lang++)
			{
				if (lang != JAZYK000)
				{
					if ((Jmeno.MultiText(lang)).IsNotEmpty())
					{
						::WritePrivateProfileString(JazykInfo[lang].LangIDini, txt, Jmeno.MultiText(lang), SelectAliases);
					}
				}
			}

			ZobrazJmeno();

// zahájení editace programu (nepoužívat SelectOnOpen - nesouhlasí parametry aktivního programu!)
			SelectEdit();
			return;
		}

// chyba vytvoření - test, zda soubor již existuje (jinak hlášena neznámá chyba)
		if ((int)::GetFileAttributes(newname) != -1)
		{
			dialogID = IDN_SELECT_EXIST;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření adresáře

void SelectOnPath()
{
// kontrola, zda je režim výběru
	if (!SelectMode) return;

// příprava zadaného jména adresáře
	if (SelectTyp == TYPUPDIR)
	{
		SelectNewText = Jmeno.MultiText();
	}
	else
	{
		SelectNewText.Empty();
	}

// implicitní jméno, není-li žádný vzor
	if (SelectNewText.IsEmpty()) SelectNewText.Load(IDN_NEWDIR_NAME);

// nalezení neexistujícího adresáře
	SelectNewNameUnique(-1);

// cyklus opakování zadání
	int dialogID = IDN_SELECT_DIR;

	for (;;)
	{

// zadání jména adresáře
		if (::DialogBoxParam(
					hInstance,
					MAKEINTRESOURCE(dialogID),
					MainFrame,
					(DLGPROC)SelectDialogProc,
					(LPARAM)dialogID) != IDOK) return;

// přednastavení příštího dialogu - adresář existuje
		dialogID = IDN_SELECT_DIREXIST;

// korekce zadaného textu
		SelectNewText.TrimLeft();
		SelectNewText.TrimRight();
		if (SelectNewText.IsEmpty()) return;

// příprava jména adresáře
		CText txt = SelectNewFileUnique(-1);
		CText dirname = ProgramPath + Cesta + txt;

// vytvoření adresáře
		if (CreateDir(dirname))
		{

// nastavení nového jména souboru
			Jmeno.Empty();
			Jmeno.MultiText(JAZYK000, txt);
			Jmeno.MultiText(SelectNewText);
			JmenoLoad = dirname;
			JmenoSave = dirname;
			::WritePrivateProfileString(JazykInfo[Jazyk].LangIDini, txt, SelectNewText, SelectAliases);
			ZobrazJmeno();

// znovunačtení obsahu adresáře
			SelectLoad();
			return;
		}

// při chybě test, zda adresář již existuje (pokud ne, je nějaká jiná chyba)
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! to již nemusí být !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		if ((int)::GetFileAttributes(dirname) == -1)
		{
			dialogID = IDN_SELECT_DIRCREATE;
		}
	}		
}		


/////////////////////////////////////////////////////////////////////////////
// zrušení souboru nebo adresáře

void SelectOnDelete()
{
// kontrola, zda je režim výběru
	if (!SelectMode) return;

// editace jména prvku
	if (SelEditName)
	{
		long int start, end;
		::SendMessage(SelEditNameWnd, EM_GETSEL, (WPARAM)&start, (WPARAM)&end);

		if (start == end)
		{
			::SendMessage(SelEditNameWnd, WM_KEYDOWN,	VK_DELETE, 0x01530001);
			::SendMessage(SelEditNameWnd, WM_KEYUP,	VK_DELETE, 0xC1530001);
		}
		else
		{
			::SendMessage(SelEditNameWnd, WM_CLEAR, 0, 0);
		}
		return;
	}

// kontrola, zda je platné jméno
	if ((SelectTyp == TYP0) || (SelectTyp == TYPUPDIR) || SelectAltern) return;

// krátké jméno položky
	CText txt0 = Jmeno.MultiText((int)JAZYK000);

// rozlišení, zda to je adresář nebo soubor
	if (SelectTyp == TYPSUBDIR)
	{

// zobrazení dialogu k potvrzení rušení adresáře
		if (::DialogBoxParam(
					hInstance,
					MAKEINTRESOURCE(IDN_SELECT_DELDIR),
					MainFrame,
					(DLGPROC)SelectDialogProc,
					(LPARAM)IDN_SELECT_DELDIR) != IDOK) return;

// zrušení aliases, neobsahuje-li složka žádný program
		WIN32_FIND_DATA wfd;
		HANDLE handle = ::FindFirstFile(ProgramPath + Cesta + txt0 + _T("\\*.exe"), &wfd);
		if (handle != INVALID_HANDLE_VALUE)
		{
			::FindClose(handle);
		}
		else
		{
			::DeleteFile(ProgramPath + Cesta + txt0 + _T("\\ALIASES.INI"));
		}

// zrušení adresáře
		if (!::RemoveDirectory(JmenoSave))
		{

// adresář nelze zrušit
			::DialogBoxParam(
					hInstance,
					MAKEINTRESOURCE(IDN_SELECT_DELDIRERROR),
					MainFrame,
					(DLGPROC)SelectDialogProc,
					(LPARAM)IDN_SELECT_DELDIRERROR);
			return;
		}
	}
	else
	{

// zobrazení dialogu k potvrzení rušení souboru
		if (::DialogBoxParam(
					hInstance,
					MAKEINTRESOURCE(IDN_SELECT_DEL),
					MainFrame,
					(DLGPROC)SelectDialogProc,
					(LPARAM)IDN_SELECT_DEL) != IDOK) return;

// kontrola, zda rušený soubor není chráněn proti zápisu
		int attrib = (int)::GetFileAttributes(JmenoSave);
		if ((attrib != -1) && ((attrib & FILE_ATTRIBUTE_READONLY) != 0))
		{

// soubor nelze zrušit
			::DialogBoxParam(
					hInstance,
					MAKEINTRESOURCE(IDN_SELECT_DELERROR),
					MainFrame,
					(DLGPROC)SelectDialogProc,
					(LPARAM)IDN_SELECT_DELERROR);
			return;
		}

// zálohování a zrušení souboru
		CText backpath = BackupPath + Cesta;
		CreatePath(backpath);
		CText backname = backpath + txt0;
		::DeleteFile(backname);
		::MoveFile(JmenoSave, backname);
		::DeleteFile(JmenoSave);

// zálohování aliases
		CText aliases = backpath + _T("\\ALIASES.INI");
		for (int lang = 0; lang < JAZYKNUM; lang++)
		{
			if (lang != JAZYK000)
			{
				if ((Jmeno.MultiText(lang)).IsEmpty())
				{
					::WritePrivateProfileString(JazykInfo[lang].LangIDini, txt0, NULL, aliases);
				}
				else
				{
					::WritePrivateProfileString(JazykInfo[lang].LangIDini, txt0, Jmeno.MultiText(lang), aliases);
				}
			}
		}
	}

// zrušení aliases
	for (int lang = 0; lang < JAZYKNUM; lang++)
	{
		if (lang != JAZYK000)
		{
			::WritePrivateProfileString(JazykInfo[lang].LangIDini, txt0, NULL, SelectAliases);
		}
	}

// test, zda je vzorová položka (zda zůstane jméno nezměněno)
	if (ProgramPath2.IsEmpty() || 
		((int)::GetFileAttributes(ProgramPath2 + Cesta + txt0) == -1))
	{
		int index = SelectItem[SelectInx].Index + 1;
		if (index >= SelectItemNum) index -= 2;

		for (int i = 0; i < SelectItemNum; i++)
		{
			if (SelectItem[i].Index == index)
			{
				Jmeno = SelectItem[i].Name;
			}
		}

		ZobrazJmeno();
	}

// znovunačtení souborů
	SelectLoad();
}


/////////////////////////////////////////////////////////////////////////////
// obsluha klávesy (vrací TRUE=zrušení klávesy)

BOOL SelectOnKeyDown(HWND hWnd, int key, int data)
{
// kontrola, zda je režim výběru
	if (!SelectMode || (hWnd != SelectView)) return FALSE;

// rozlišení podle klávesy
	switch (key)
	{

// Alt-Enter = přejmenování
	case VK_RETURN:
		if ((!SelEditName) && (::GetKeyState(VK_MENU) < 0) && 
			(SelectTyp != TYP0) && (SelectTyp != TYPUPDIR) && !SelectAltern)
		{

// zahájení editace jména prvku
			::PostMessage(SelectView, LVM_EDITLABEL, (LPARAM)SelectItem[SelectInx].Index, 0);
		}
		break;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// zahájení editace jména souboru/adresáře

void SelectOnRename()
{
// kontrola, zda je režim výběru a zda není editace
	if (!SelectMode || SelEditName) return;

// zahájení editace jména prvku
	if ((SelectTyp != TYP0) && (SelectTyp != TYPUPDIR) && !SelectAltern)
	{
		::PostMessage(SelectView, LVM_EDITLABEL, (LPARAM)SelectItem[SelectInx].Index, 0);
	}
}


/////////////////////////////////////////////////////////////////////////////
// zahájení editace jména prvku (vrací TRUE=přerušit editaci)

BOOL SelectOnBeginLabelEdit(HWND hWnd, int iItem)
{
// kontrola, zda je režim výběru
	if (!SelectMode || SelEditName || 
		(hWnd != SelectView) || 
		(SelectTyp == TYP0) || 
		(SelectTyp == TYPUPDIR) || 
		SelectAltern)
	{
		return TRUE;
	}

// načtení handle editačního okna
	SelEditNameWnd = (HWND)::SendMessage(SelectView, LVM_GETEDITCONTROL, 0, 0);
	if (SelEditNameWnd == NULL) return TRUE;

// příznak zahájení editace
	SelEditName = TRUE;

// příznak povolení editace
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// ukončení editace jména prvku

void SelectOnEndLabelEdit(HWND hWnd, LV_ITEM* lvi)
{
// kontrola, zda je režim výběru
	if (!SelectMode || !SelEditName || (hWnd != SelectView)) return;

// test, zda je text platný
	if ((lvi->pszText != NULL) && (SelectTyp != TYP0) && (SelectTyp != TYPUPDIR) && !SelectAltern)
	{

// načtení zadaného textu
		SelectNewText = lvi->pszText;
		SelectNewText.TrimLeft();
		SelectNewText.TrimRight();
		SelectNewMultiText = Jmeno;

// aktuální jméno bez přípony
		CText oldtxt = Jmeno.MultiText((int)JAZYK000);
		if (SelectTyp == TYPFILE) oldtxt.Delete(oldtxt.Length() - 4);

// korekce implicitního jména
		BOOL impl = TRUE;
		int lan;
		for (lan = 0; lan < JAZYKNUM; lan++)
		{
			if ((lan != JAZYK000) && SelectNewMultiText.MultiText(lan).IsNotEmpty())
			{
				impl = FALSE;
				break;
			}
		}
		if (impl) SelectNewText = oldtxt;

		SelectNewMultiText.MultiText(SelectNewText);

// příprava nového jména souboru
//		CText txt = SelectNewMultiText.MultiText();
//		txt.KorigShort();
//		if (txt != oldtxt) txt = SelectNewFileUnique();
		CText txt = SelectNewFileUnique(SelectInx);

// přidání přípony
		if (SelectTyp == TYPFILE)
		{
			if (ShortName)
			{
				txt += TextExe2;
			}
			else
			{
				txt += TextExe;
			}
		}

		oldtxt = Jmeno.MultiText((int)JAZYK000);
		CText newname = ProgramPath + Cesta + txt;

// přejmenování souboru nebo adresáře (adresář se po chvíli automaticky načte!!!!!)
		if ((oldtxt == txt) || ::MoveFile(JmenoSave, newname))
		{
			if (oldtxt == txt)
			{
				::WritePrivateProfileString(JazykInfo[Jazyk].LangIDini, txt, SelectNewMultiText.MultiText(Jazyk), SelectAliases);
			}
			else
			{
				SelectNewMultiText.MultiText(JAZYK000, txt);

				for (int lang = 0; lang < JAZYKNUM; lang++)
				{
					if (lang != JAZYK000)
					{
						::WritePrivateProfileString(JazykInfo[lang].LangIDini, oldtxt, NULL, SelectAliases);

						if ((SelectNewMultiText.MultiText(lang)).IsEmpty())
						{
							::WritePrivateProfileString(JazykInfo[lang].LangIDini, txt, NULL, SelectAliases);
						}
						else
						{
							::WritePrivateProfileString(JazykInfo[lang].LangIDini, txt, SelectNewMultiText.MultiText(lang), SelectAliases);
						}
					}
				}
			}

			SelectItem[SelectInx].Name = SelectNewMultiText;
			SelectItem[SelectInx].NameLoad = ProgramPath + Cesta + SelectNewMultiText.MultiText((int)JAZYK000);
			SelectItem[SelectInx].NameSave = SelectItem[SelectInx].NameLoad;

			SelectSelect(SelectInx);

			lvi->pszText = (LPTSTR)(LPCTSTR)Jmeno.MultiText();
		}
		ZobrazJmeno();
	}

// příznak ukončení editace
	SelEditName = FALSE;
	SelEditNameWnd = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// obsluha časovače (TRUE=obslouženo)

BOOL SelectOnTimer(UINT timerID)
{
// kontrola, zda je režim výběru
	if (!SelectMode) return FALSE;

// nastavení automatického fokusu
	if (timerID == SelectTimerID)
	{
		if (SelectTimerN > 0)
		{
			SelectTimerN--;
			if (SelectTimerN == 0)
			{

// ukončení režimu výběru souborů
				SelectTerm();

// zahájení režimu editace souborů
				ProgInit();

				if (IsWaitCursor)
				{
					IsWaitCursor = FALSE;
					EndWaitCursor();
				}
			}
			return TRUE;
		}
	}

// hlášení o změně adresáře
	if (timerID == SelectDirChangeTimerID)
	{
		BOOL load = FALSE;

		if (SelectDirChange != INVALID_HANDLE_VALUE)
		{
			while (::WaitForSingleObject(SelectDirChange, 0) == WAIT_OBJECT_0)
			{
				::FindNextChangeNotification(SelectDirChange);
				load = TRUE;
			}
		}

		if (SelectDirChange2 != INVALID_HANDLE_VALUE)
		{
			while (::WaitForSingleObject(SelectDirChange2, 0) == WAIT_OBJECT_0)
			{
				::FindNextChangeNotification(SelectDirChange2);
				load = TRUE;
			}
		}

		if (load)
		{
			SelectLoad();
		}
	}

	return FALSE;
}
