
#include "Main.h"

/***************************************************************************\
*																			*
*							Buffer programu									*
*																			*
\***************************************************************************/

////////////////////////////////////////////////////////////////////
// Buffer přistupuje ke globálním datovým seznamům (Text, Icon, ...) !
////////////////////////////////////////////////////////////////////

// inicializační prázdná položka (pro vytvoření nové položky)
PROGITEM	InitProgItem = {
	IDF_COMMENT,				// funkce

	0,							// zdrojové vlastností
	0,							// cílové vlastností
	0,							// parametry

	-1,							// referenční blok
	-1,							// referenční index
	-1,							// datový blok
	-1,							// datový index

	-1,							// rodič
	-1,							// následující položka
	-1,							// předcházející položka
	-1,							// potomek

	-1,							// ikona
	-1,							// jméno

	NULL,						// položka stromu

	0							// čítač referencí
};


////////////////////////////////////////////////////////////////////
// vytvoření nových dat (vrací TRUE=operace OK) (odděleno kvůli lepší optimalizaci)

#define NEWDATANUM 256				// počet nově vytvořených položek (16 KB)

bool CBufProg::NewData()
{
// nový počet položek
	int next = m_Max;				// příští položka - 1
	int max = next + NEWDATANUM;	// nový počet položek

// zvětšení bufferu dat
	PROGITEM* newdata = (PROGITEM*)MemSize(m_Data, max*sizeof(PROGITEM));
	if (newdata == NULL) return false;
	m_Data = newdata;

// zvětšení bufferu platnosti
	bool* newvalid = (bool*)MemSize(m_Valid, max*sizeof(bool));
	if (newvalid == NULL) return false;
	m_Valid = newvalid;

// nový maximální počet položek v bufferu
	m_Max = max;

// vymazání příznaků platnosti položek (nastavení na příznak neplatnosti)
	int i;
	newvalid += next;
	for (i = NEWDATANUM; i > 0; i--) { *newvalid = false; newvalid++; }

// začlenění do řetězce volných položek
	newdata += next - 1;			// ukazatel dat - 1
	for (i = NEWDATANUM; i > 0; i--)
	{
		newdata++;					// zvýšení ukazatele položek
		next++;						// zvýšení indexu příští položky
		*(int*)newdata = next;		// odkaz na příští položku
	}
	*(int*)newdata = m_Next;		// navázání na další položku
	m_Next = m_Max-NEWDATANUM;		// odkaz na první novou položku

// příznak operace OK
	return true;
};


////////////////////////////////////////////////////////////////////
// vytvoření nové položky (vrací index položky, při chybě vrací -1)

int CBufProg::NewItem()
{
// vytvoření nových dat, není-li volná další položka
	if (m_Next < 0)				// není další položka?
	{
		if (!NewData()) return -1;	// vytvoření nových dat
	}

// vyjmutí položky z řetězce volných položek
	int i = m_Next;				// příští volná položka
	m_Next = *(int*)(m_Data + i); // další položka
	m_Valid[i] = true;			// nastavení příznaku platnosti položky
	m_Num++;					// zvýšení čítače platných položek

// nově vytvořená položka
	return i;
};


////////////////////////////////////////////////////////////////////
// zrušení položky - zařazení do volných položek (nekontroluje index a neruší objekt)

void _fastcall CBufProg::DelItem(const int index)
{
	*(int*)(m_Data + index) = m_Next;	// příští volná položka
	m_Valid[index] = false;				// zrušení příznaku platnosti
	m_Num--;							// snížení čítače platných položek
	m_Next = index;						// odkaz na tuto položku
}

////////////////////////////////////////////////////////////////////
// aktualizace fontu v okně

void CBufProg::AktFont()
{
	if (m_Tree != NULL)
	{
		if ((m_Font == NULL) ||
			!(TreeFont == m_FontSet) ||
			(CharSet != m_CharSet))
		{
			HFONT oldfont = m_Font;
			m_Font = GetFont(&TreeFont);
			m_FontSet = TreeFont;
			m_CharSet = CharSet;
			::SendMessage(m_Tree, WM_SETFONT, (WPARAM)m_Font, TRUE);
			DelFont(oldfont);
		}
	}
	else
	{
		DelFont(m_Font);
		m_Font = NULL;
		m_FontSet.Height = -1;
	}
}

////////////////////////////////////////////////////////////////////
// příprava indexu stavové ikony (upravit s INDEXTOSTATEIMAGEMASK() !)

int CBufProg::GetStateImage(int index)
{
// implicitní stav - nic
	int state = STATE_NONE;

// je položka platná?
	if (IsValid(index))
	{

// adresa položky
		PROGITEM* item = m_Data + index;

// obsluha okna editoru (požadavek nastavení proměnné) ...
		if ((m_BufID == BufEdiID) &&

// ... indikuje se v případě, že není žádný parametr ,,,
			(item->Child < 0) &&

// ... položka vyžaduje nějaká data ...
			(item->DstMask & PR_ALLDATA) && 

// ... ale nesmí vyžadovat i příkazy ...				
			((item->DstMask & PR_COMMAND) == 0) &&

// ... položka musí mít platného rodiče ...
			(item->Parent >= 0)
			)
		{

// adresa rodiče
			PROGITEM* parent = m_Data + item->Parent;

// položka musí být použita buď jako příkaz ...
			if ((parent->DstMask & PR_COMMAND) || 

// ... nebo jako parametr funkce ...
				(parent->Func == IDF_FNC) ||

// ... nebo jsou parametry vyžadovány vždy
				(item->Param & PR_PARPAR) ||

// ... nebo nelze použít jako příkaz - je to tedy parametr něčeho
				((item->SrcMask & PR_COMMAND) == 0))
			{
				if (item->Param & PR_SETPAR)
				{
					state = STATE_SET;
				}
				else
				{
					state = STATE_SET_OPT;
				}
			}
		}

// příprava stavu pro okna objektů (definovaná položka)
		if (index == m_Def)
		{
			state = STATE_DEF;
		}
	}

// index stavové ikony
	return state;
}


////////////////////////////////////////////////////////////////////
// nové zobrazení položky

void CBufProg::DispNewItem(int index)
{
// kontrola platnosti položky a stromu
	if (IsNotValid(index) || (m_Tree == NULL)) return;

// lokální proměnné
	TV_INSERTSTRUCT tvins;					// struktura k vložení položky
	PROGITEM* item = m_Data + index;		// adresa položky k zobrazení

// adresa položky k zobrazení
	item = m_Data + index;

// maska parametrů položky
	tvins.item.mask = TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_TEXT;

// příprava textu položky
	tvins.item.pszText = (LPTSTR)(LPCTSTR)Text.GetTxt(GetText(index));

// ikona položky - zpětné volání
	tvins.item.iImage =  I_IMAGECALLBACK;
	tvins.item.iSelectedImage =  I_IMAGECALLBACK;

// přednastavení stavu
	tvins.item.stateMask = TVIS_EXPANDED | TVIS_BOLD | TVIS_CUT | TVIS_STATEIMAGEMASK;

// příprava stavové ikony
	tvins.item.state = INDEXTOSTATEIMAGEMASK(GetStateImage(index));

// příprava příznaku rozvinutí položky
	if (item->Param & PR_EXP)
	{
		tvins.item.state |= TVIS_EXPANDED;
	}

// příznak uzamčení položky
	if (item->Param & (PR_LOCK | PR_LOCK_DEP))
	{
		tvins.item.state |= TVIS_BOLD;
	}

// příznak vypnutí položky
	if (item->Param & (PR_OFF | PR_OFF_DEP))
	{
		tvins.item.state |= TVIS_CUT;
	}

// příprava položky potomků
	if ((item->Child >= 0) || ((m_BufID == BufClsID) && ((item->Func == IDF_GROUP) || ((item->Parent < 0) && (item->Func != IDF_FNC)))))
	{
		tvins.item.cChildren = 1;
	}
	else
	{
		tvins.item.cChildren = 0;
	}

// příprava rodiče položky
	if (item->Parent >= 0)
	{
		tvins.hParent = m_Data[item->Parent].HTree;
	}
	else
	{
		tvins.hParent = TVI_ROOT;
	}

// příprava předcházející položky
	if (item->Prev >= 0)
	{
		tvins.hInsertAfter = m_Data[item->Prev].HTree;
	}
	else
	{
		tvins.hInsertAfter = TVI_FIRST;
	}

// zobrazení položky ve stromu a úschova handle položky
	item->HTree = (HTREEITEM)::SendMessage(m_Tree, TVM_INSERTITEM, 0, (LPARAM)&tvins);
}


////////////////////////////////////////////////////////////////////
// konstruktor a destruktor

CBufProg::CBufProg()
{
	m_Data = NULL;			// není buffer dat
	m_Valid = NULL;			// není buffer platnosti
	m_Num = 0;				// není žádná platná položka
	m_Max = 0;				// není buffer položek
	m_Next = -1;			// přiští volná položka (-1=není)
	m_Undo = false;			// neregistrovat změny pro UNDO

	m_Redraw = 0;			// překreslování okna zapnuto
	m_First = -1;			// není první (ROOT) položka)
	m_Tree = NULL;			// není připojený strom
	m_Disp = -2;			// není nic zobrazeno
	m_IconWidth = 0;		// šířka ikon (0 = není)
	m_IconHeight = 0;		// výška ikon (0 = není)
	m_Def = -1;				// definovaná položka (-1 = není)

	m_CharSet = DEFAULT_CHARSET; // implicitní znaková sada
	m_Font = NULL;			// font není
}

CBufProg::~CBufProg()
{
	DelAll();				// zrušení bufferu
}


////////////////////////////////////////////////////////////////////
// statický konstruktor a destruktor

void CBufProg::Init()
{
	m_Data = NULL;			// není buffer dat
	m_Valid = NULL;			// není buffer platnosti
	m_Num = 0;				// není žádná platná položka
	m_Max = 0;				// není buffer položek
	m_Next = -1;			// přiští volná položka (-1=není)
	m_Undo = false;			// neregistrovat změny pro UNDO

	m_Redraw = 0;			// překreslování okna zapnuto
	m_First = -1;			// není první (ROOT) položka)
	m_Tree = NULL;			// není připojený strom
	m_Disp = -2;			// není nic zobrazeno
	m_IconWidth = 0;		// šířka ikon (0 = není)
	m_IconHeight = 0;		// výška ikon (0 = není)
	m_Def = -1;				// definovaná položka (-1 = není)

	m_Nadpis.Init();		// inicializace nadpisu okna

	m_FontSet.Init();		// inicializace standardního fontu
	m_FontSet.Height = -1;	// výška fontu neurčena
	m_CharSet = DEFAULT_CHARSET; // implicitní znaková sada
	m_Font = NULL;			// font není
}

void CBufProg::Term()
{
	DelAll();				// zrušení bufferu
	m_Nadpis.Term();		// deinicializace nadpisu okna
	m_FontSet.Term();		// deinicializace defincie fontu
	DelFont(m_Font);		// zrušení fontu
}


////////////////////////////////////////////////////////////////////
// zrušení všech položek v bufferu (ukládání začne opět po řadě, nemění obsah okna)

void CBufProg::DelAll()
{
	MemFree(m_Data);		// zrušení datového bufferu
	m_Data = NULL;			// není datový buffer
	MemFree(m_Valid);		// zrušení bufferu platnosti
	m_Valid = NULL;			// není buffer platnosti
	m_Num = 0;				// není žádná platná položka
	m_Max = 0;				// není žádná položka v bufferu
	m_Next = -1;			// není příští položka

	m_First = -1;			// není první (ROOT) položka)
	m_Disp = -2;			// není nic zobrazeno
	m_Def = -1;				// definovaná položka (-1 = není)
}


////////////////////////////////////////////////////////////////////
// vypnutí překreslování okna stromu

void CBufProg::RedrawOff()
{
// test, zda bylo překreslování zapnuto
	if (m_Redraw == 0)
	{

// vypnutí aktualizace stromu
		if (m_Tree != NULL)
		{
			::SendMessage(m_Tree, WM_SETREDRAW, FALSE, 0);
		}
	}

// zvýšení čítače úrovně vypnutí překreslování
	m_Redraw++;
}


////////////////////////////////////////////////////////////////////
// zapnutí překreslování okna stromu

void CBufProg::RedrawOn()
{
// snížení čítače úrovně vypnutí překreslování
	m_Redraw--;

// test, zda bude překreslování zapnuto
	if (m_Redraw == 0)
	{

// zapnutí aktualizace stromu
		if (m_Tree != NULL)
		{
			::SendMessage(m_Tree, WM_SETREDRAW, TRUE, 0);
		}
	}
}


////////////////////////////////////////////////////////////////////
// nastavení rodiče zobrazení položek + zobrazení položek (-1 = vše, -2 = nic)

void CBufProg::Disp(int disp)
{
// test, zda se rodič zobrazení položek mění
	if (disp == m_Disp) return;

// vypnutí aktualizace stromu
	RedrawOff();

// lokální proměnné
	PROGITEM* item;
	int index;

// vymazání aktuálně zobrazeného obsahu stromu (je-li něco zobrazeno)
	if (m_Disp != -2)
	{
		if (m_Tree != NULL)
		{
			::SendMessage(m_Tree, TVM_DELETEITEM, 0, (LPARAM)TVI_ROOT);
		}

		for (index = m_Max - 1; index >= 0; index--)
		{
			if (m_Valid[index]) m_Data[index].HTree = NULL;
		}
	}

// kontrola platnosti nového rodiče položek k zobrazení
	if ((disp != -1) && IsNotValid(disp))
	{
		m_Disp = -2;		
		RedrawOn();
		return;
	}

// nastavení nového rodiče zobrazení položek
	m_Disp = disp;

// příprava první položky k zobrazení
	if (disp < 0)
	{
		index = m_First;
	}
	else
	{
		index = m_Data[disp].Child;
	}

// cyklus zobrazení položek
	if (index >= 0)
	{
		do {

// nové zobrazení položky
			DispNewItem(index);

// adresa položky
			item = m_Data + index;

// nalezení následující položky
			index = item->Child;			// vnoření na potomka

			if (index < 0)					// je potomek?
			{
				index = item->Next;			// pokračování další položkou

				while ((index < 0) && (item->Parent >= 0))
				{
					index = item->Parent;	// návrat k rodiči
					if (index == disp) break; // je již opět výchozí položka
					item = m_Data + index;	// adresa rodiče
					index = item->Next;		// další položka za rodičem
				}
			}

// dokud je další platná položka
		} while (index != disp);
	}

// zapnutí aktualizace stromu
	RedrawOn();
};


////////////////////////////////////////////////////////////////////
// nastaveni ikon ve stromu (zoom: TRUE = ikony 32*32)

void CBufProg::IconList(SMALLICON zoom)
{
// lokální proměnné
	HIMAGELIST img;			// seznam ikon
	HIMAGELIST imgs;		// seznam stavových ikon
	int icon;				// rozměr ikon

// použity velké ikony
	switch (zoom)
	{
	case SI_BIG:
		icon = 32;
		img = ProgImageList;
		imgs = ProgStateList;
		break;

	case SI_MEDIUM:
		icon = 24;
		img = ProgImageListMedium;
		imgs = ProgStateListMedium;
		break;

	default:
		icon = 16;
		img = ProgImageListSmall;
		imgs = ProgStateListSmall;
	}

// test, zda je rozměr ikon již nastaven
	if ((icon == m_IconWidth) && (icon == m_IconHeight)) return;

// nastavení nového rozměru ikon
	m_IconWidth = icon;
	m_IconHeight = icon;

	if (m_Tree != NULL)
	{
		::SendMessage(m_Tree, TVM_SETIMAGELIST, TVSIL_NORMAL, (LPARAM)img);
		::SendMessage(m_Tree, TVM_SETIMAGELIST, TVSIL_STATE, (LPARAM)imgs);
		::SendMessage(m_Tree, TVM_SETINDENT, icon+3, 0);
	}
}


////////////////////////////////////////////////////////////////////
// nastavení definované položky (-1 = odznačení)

void CBufProg::Def(int index)
{
// kontrola indexu
	if (IsNotValid(index)) index = -1;

// kontrola, zda se definovaná položka mění
	if (index == m_Def) return;

// test, zda je zobrazen strom
	if (m_Tree != NULL)
	{

// lokální proměnné
		TV_ITEM tvi;

// zrušení označení staré položky
		if (IsValid(m_Def) && (m_Data[m_Def].HTree != NULL))
		{
			tvi.mask = TVIF_STATE;
			tvi.stateMask = TVIS_STATEIMAGEMASK;
			tvi.hItem = m_Data[m_Def].HTree;
			tvi.state = INDEXTOSTATEIMAGEMASK(STATE_NONE);
			::SendMessage(m_Tree, TVM_SETITEM, 0, (LPARAM)&tvi);
		}

// uložení nové položky
		m_Def = index;

// bude zobrazení nové položky
		if (IsValid(index) && (m_Data[index].HTree != NULL))
		{

// příprava klientských rozměrů okna
			RECT wrc;
			::GetClientRect(m_Tree, &wrc);

// zjištění, zda je položka viditelná (zda je rychlejší nevypínat překreslování)
			RECT rc;
			*(HTREEITEM*)&rc = m_Data[index].HTree;
			BOOL visible = ::SendMessage(m_Tree, TVM_GETITEMRECT, FALSE, (LPARAM) &rc);

// upřesnění viditelnosti položky
			if (visible)
			{
				visible = ((rc.top >= 0) && (rc.bottom <= wrc.bottom));
			}

// vypnutí překreslování, není-li položka viditelná
			if (!visible) RedrawOff();

// nastavení označení nové položky
			tvi.mask = TVIF_STATE;
			tvi.stateMask = TVIS_STATEIMAGEMASK;
			tvi.hItem = m_Data[index].HTree;
			tvi.state = INDEXTOSTATEIMAGEMASK(STATE_DEF);
			::SendMessage(m_Tree, TVM_SETITEM, 0, (LPARAM)&tvi);

// zajištění viditelnosti položky
			::SendMessage(m_Tree, TVM_ENSUREVISIBLE, 0, (LPARAM)m_Data[index].HTree);

// zapnutí překreslování
			if (!visible) RedrawOn();
		}
	}
	else

// jinak nastavení indexu položky bez překreslení okna
	{
		m_Def = index;
	}
}


////////////////////////////////////////////////////////////////////
// nastavení nadpisu okna

void CBufProg::Nadpis(const CText& text)
{
// kontrola, zda se text mění
	if (m_Nadpis == text) return;

// nastavení nového nadpisu
	m_Nadpis = text;

// zobrazení nového nadpisu
	if (m_Tree != NULL)
	{
		ProgDispNadpis();
	}
}


////////////////////////////////////////////////////////////////////
// poskytnutí první zobrazené položky (-1 = není)

int CBufProg::Top()
{
// kontrola, zda je strom platný
	if (m_Tree == NULL) return -1;

// načtení zobrazené položky
	HTREEITEM htree = (HTREEITEM)::SendMessage(m_Tree, TVM_GETNEXTITEM, TVGN_FIRSTVISIBLE, 0);

// nalezení indexu položky
	return Find(htree);
}


////////////////////////////////////////////////////////////////////
// nastavení první zobrazené položky

void CBufProg::Top(int index)
{
// kontrola, zda je strom platný
	if (m_Tree == NULL) return;

// kontrola, zda je položka platná
	if (IsNotValid(index)) return;

// úschova současně zobrazené první položky
	HTREEITEM htree = (HTREEITEM)::SendMessage(m_Tree, TVM_GETNEXTITEM, TVGN_FIRSTVISIBLE, 0);

// je-li již položka nastavená, nic se neprovede
	if (m_Data[index].HTree == htree) return;

// nastavení položky
	RedrawOff();
	::SendMessage(m_Tree, TVM_SELECTITEM, TVGN_FIRSTVISIBLE, (LPARAM)m_Data[index].HTree);
	RedrawOn();
}


////////////////////////////////////////////////////////////////////
// poskytnutí vybrané položky (-1 = není)

int CBufProg::Select()
{
// kontrola, zda je strom platný
	if (m_Tree == NULL) return -1;

// načtení vybrané položky
	HTREEITEM htree = (HTREEITEM)::SendMessage(m_Tree, TVM_GETNEXTITEM, TVGN_CARET, 0);

// nalezení indexu položky
	return Find(htree);
}


////////////////////////////////////////////////////////////////////
// nastavení vybrané položky (-1 = není)

void CBufProg::Select(int index)
{
// kontrola, zda je strom platný
	if (m_Tree == NULL) return;

// je-li již položka vybraná, nic se neprovede
	if (index == Select()) return;

// není-li položka platná, vypnutí vybrané položky
	if (IsNotValid(index) || (m_Data[index].HTree == NULL))
	{
		::SendMessage(m_Tree, TVM_SELECTITEM, TVGN_CARET, 0);
	}
	else
	{

// příprava klientských rozměrů okna
		RECT wrc;
		::GetClientRect(m_Tree, &wrc);

// zjištění, zda je položka viditelná (zda je rychlejší nevypínat překreslování)
		RECT rc;
		*(HTREEITEM*)&rc = m_Data[index].HTree;
		BOOL visible = ::SendMessage(m_Tree, TVM_GETITEMRECT, FALSE, (LPARAM) &rc);

// upřesnění viditelnosti položky
		if (visible)
		{
			visible = ((rc.top >= 0) && (rc.bottom <= wrc.bottom));
		}

// vypnutí překreslování, není-li položka viditelná
		if (!visible) RedrawOff();

// změna vybrané položky
		::SendMessage(m_Tree, TVM_SELECTITEM, TVGN_CARET, (LPARAM)m_Data[index].HTree);

// zajištění viditelnosti položky
		::SendMessage(m_Tree, TVM_ENSUREVISIBLE, 0, (LPARAM)m_Data[index].HTree);

// zapnutí překreslování
		if (!visible) RedrawOn();
	}
}


////////////////////////////////////////////////////////////////////
// aktualizace zobrazení stavové ikony položky pro okno editoru programu

void _fastcall CBufProg::AktStateImage(int index)
{
// je položka platná (a je to okno editoru programu)?
	if ((m_BufID == BufEdiID) && 
		IsValid(index) && 
		(m_Data[index].HTree != NULL) && 
		(m_Tree != NULL))
	{

// požadovaný nový stav položky
		DWORD newstate = INDEXTOSTATEIMAGEMASK(GetStateImage(index));

// načtení starého stavu položky
		TV_ITEM tvi;
		tvi.hItem = m_Data[index].HTree;
		tvi.mask = TVIF_STATE;
		tvi.stateMask = TVIS_STATEIMAGEMASK;
		tvi.state = newstate;
		::SendMessage(m_Tree, TVM_GETITEM, 0, (LPARAM)&tvi);
		
// nastavení nového stavu položky, pokud se mění
		if (newstate != (tvi.state & TVIS_STATEIMAGEMASK))
		{
			tvi.mask = TVIF_STATE;
			tvi.stateMask = TVIS_STATEIMAGEMASK;
			tvi.state = newstate;
			::SendMessage(m_Tree, TVM_SETITEM, 0, (LPARAM)&tvi);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// index následující položky ve stromu (cyklicky, -1 = navrátí se první položka)

int _fastcall CBufProg::NextItem(int index)
{
// kontrola platnosti položky
	if (IsNotValid(index)) return m_First;

// adresa položky
	PROGITEM* item = m_Data + index;

// přesun na potomka
	index = item->Child;				// potomek

// není-li potomek, přesun na následující prvek
	if (index < 0)						// je nějaký potomek?
	{
		index = item->Next;				// následující prvek

// není-li následující prvek, přesun na rodiče
		while (index < 0)				// je následující prvek?
		{
			index = item->Parent;		// index rodiče

			if (index < 0)				// je platný rodič ?
			{
				index = m_First;		// první položka v ROOT
				ASSERT(index >= 0);
			}
			else
			{
				item = m_Data + index;	// adresa rodiče
				index = item->Next;		// následující prvek
			}
		}
	}
	return index;
}


/////////////////////////////////////////////////////////////////////////////
// index předcházející položky ve stromu (cyklicky, -1 = navrátí se poslední položka)

int _fastcall CBufProg::PrevItem(int index)
{
// kontrola platnosti položky
	if (IsNotValid(index)) index = m_First;

// není žádná položka
	if (IsNotValid(index)) return -1;

// adresa položky
	PROGITEM* item = m_Data + index;

// přesun na předcházející položku
	index = item->Prev;				// předcházející položka

// kontrola, zda je předcházející položka
	if (index >= 0)					// je předcházející položka?
	{
		item = m_Data + index;		// adresa předcházející položky

// má-li předcházející položka potomka, přesun na potomka
		if (item->Child >= 0)		// je nějaký potomek?
		{
			index = item->Child;	// potomek
			item = m_Data + index;	// adresa potomka

// nalezení poslední položky potomka
			while ((item->Next >= 0) || (item->Child >= 0))
			{
				while (item->Next >= 0)
				{
					index = item->Next;	// další položka
					item = m_Data + index; // adresa další položky
				}

				if (item->Child >= 0)	// je nějaký potomek?
				{
					index = item->Child; // potomek
					item = m_Data + index; // adresa potomka
				}
			}
		}
	}

// není předcházející položka, přesun na rodiče
	else
	{
		index = item->Parent;		// index rodiče

// není-li další rodič, přesun na první položku ROOT
		if (index < 0)				// je rodič?
		{
			index = m_First;		// první položka v ROOT
			ASSERT(index >= 0);
			item = m_Data + index;	// adresa první položky

// nalezení úplně poslední položky stromu
			while ((item->Next >= 0) || (item->Child >= 0))
			{
				while (item->Next >= 0)
				{
					index = item->Next;	// další položka
					item = m_Data + index; // adresa další položky
				}

				if (item->Child >= 0)	// je nějaký potomek?
				{
					index = item->Child; // potomek
					item = m_Data + index; // adresa potomka
				}
			}
		}
	}
	return index;
}


////////////////////////////////////////////////////////////////////
// navrácení položky v UNDO operaci (vrací TRUE=operace OK)
// provádí záznam do UNDO bufferu

bool CBufProg::UndoIns(const PROGITEM* item, const int index)
{
// vytvoření nových dat (zajištění existence položky)
	ASSERT(index >= 0);
	while (index >= m_Max)
	{
		if (!NewData()) return false;
	}
	ASSERT(IsNotValid(index));					// položka musí být neplatná

// lokální proměnné
	PROGITEM*	itemNew = m_Data + index;			// adresa nové položky
	int			parent = item->Parent;				// rodič položky
	PROGITEM*	itemPar = m_Data + parent;			// adresa rodiče
	int			prev = item->Prev;					// předcházející položka
	PROGITEM*	itemAft = m_Data + prev;			// adresa předcházející položky
	int			next = item->Next;					// následující položka
	PROGITEM*	itemNxt = m_Data + next;			// adresa následující položky

	ASSERT((parent == -1) || (IsValid(parent)));

// záznam operace pro UNDO
	if (m_Undo)
	{
		if (!Undo.AddProgIns(m_BufID, index)) return false;
	}

// vyjmutí položky z volných položek
	int i = m_Next;								// ukazatel řetězce volných položek

// navrácena hned první položka z řetězce volných položek
	if (i == index)								// je to hned první položka?
	{
		m_Next = *(int*)itemNew;				// odkaz na další položku
	}
	else
	{

// nalezení položky v řetězci volných položek
		while (*(int*)(m_Data + i) != index)
		{
			i = *(int*)(m_Data + i);

// tento případ nesmí nikdy nastat - položka nebyla nalezena!!!
			ASSERT(i >= 0);
			if (i < 0)								// to je chyba!!!!!!!
			{
				Undo.DelAll();
				return false;
			}
		}

// přeskočení odkazu na další položku
		*(int*)(m_Data + i) = *(int*)itemNew;
	}

// nastavení příznaku platnosti položky
	m_Num++;									// zvýšení čítače položek
	m_Valid[index] = true;						// příznak platnosti položky

// přenesení dat položky
	MemCopy(itemNew, item, SIZEOFPROGITEM);

// napojení na předcházející položku a na rodiče
	if (IsValid(prev))
	{
		itemNew->Next = itemAft->Next;
		itemAft->Next = index;
	}
	else
	{
		itemNew->Prev = -1;
		if (IsValid(parent))
		{
			itemNew->Next = itemPar->Child;
			itemPar->Child = index;

			if ((itemNew->Next < 0) && (itemPar->HTree != NULL) && (m_Tree != NULL))	// první potomek rodiče?
			{
				TV_ITEM tvi;
				tvi.hItem = itemPar->HTree;
				tvi.mask = TVIF_CHILDREN;
				tvi.cChildren = 1;
				::SendMessage(m_Tree, TVM_SETITEM, 0, (LPARAM)&tvi);

				AktStateImage(parent);		// aktualizace stavové ikony pro okno editoru
			}
		}
		else
		{
			itemNew->Parent = -1;			// není rodič
			itemNew->Next = m_First;		// předřazení před první ROOT položku
			m_First = index;				// položka bude první ROOT položkou
		}
	}

// napojení před následující položku
	if (IsValid(next))
	{
		itemNxt->Prev = index;
	}
	else
	{
		itemNew->Next = -1;
	}

// inicializace ostatních parametrů
	itemNew->Child = -1;					// není žádný potomek
	itemNew->HTree = NULL;					// není zobrazení položky stromu
	itemNew->Refer = 0;						// nejsou žádné reference na položku

// zvýšení čítače u referenční položky (kromě bufferu tříd)
	if (((DWORD)(itemNew->RefBlok) < (DWORD)PROGBUFNUM) && (m_BufID != BufClsID))
	{
		if (BufProg[itemNew->RefBlok].IsValid(itemNew->RefIndex))
		{
			BufProg[itemNew->RefBlok][itemNew->RefIndex].Refer++;
		}
	}

// zobrazení položky, je-li v zobrazené větvi
	if ((((parent >= 0) && (itemPar->HTree != NULL)) ||
		(m_Disp == parent)))
	{
		DispNewItem(index);
	}

	return true;
}


////////////////////////////////////////////////////////////////////
// vložení položky bez navázání dat (pro UNDO buffer)
// vrací index položky, při chybě paměti vrací -1

int CBufProg::Insert0(const PROGITEM* item)
{
	int result = NewItem();		// vytvoření nové položky
	if (result >= 0)
	{
		MemCopy(m_Data + result, item, SIZEOFPROGITEM);
	}
	return result;
}


////////////////////////////////////////////////////////////////////
// zrušení položky bez navázání dat (pro UNDO buffer)

void CBufProg::Del0(int index)
{
	if (IsValid(index)) DelItem(index);
}


////////////////////////////////////////////////////////////////////
// vložení položky (vrací index položky, při chybě paměti vrací -1) - parametry nemusí být platné
//  parent = rodič (-1=ROOT), after=předcházející položka 
//		(-1=první, -2=poslední, -3=abecedně, -4=abecedně s předností skupin)
//  Položky nastavené funkcí: Parent, Next, Prev, Child, HTree, Refer,

int CBufProg::Insert(const PROGITEM* item, int parent, int after)
{
// založení nové položky
	int index = NewItem();					// vytvoření nové položky
	if (index < 0) return -1;				// chyba paměti

// záznam operace pro UNDO
	if (m_Undo)
	{
		if (!Undo.AddProgIns(m_BufID, index))
		{
			DelItem(index);
			return -1;
		}
	}

// kopie obsahu položky
	PROGITEM* itemNew = m_Data + index;		// adresa nové položky
	MemCopy(itemNew, item, SIZEOFPROGITEM);	// kopie položky

// kontrola platnosti rodiče, příprava adresy rodiče
	if (IsNotValid(parent)) parent = -1;	// není platný - bude to ROOT položka
	PROGITEM* itemPar = m_Data + parent;	// adresa rodiče

// nastavení odkazu na rodiče
	itemNew->Parent = parent;				// odkaz na rodiče (pro ROOT bude = -1)

// nalezení předchozí položky abecedně
	if (after == -3)						// je abecední třídění?
	{
		after = -1;							// uložení na začátek

// příprava textu vkládané položky
		CText txt;
		if (item->Name >= 0)
		{
			txt = Text.GetTxt(item->Name);
		}
		else
		{
			if (item->RefBlok >= 0)
			{
				txt = Text.GetTxt(BufProg[item->RefBlok].GetText(item->RefIndex));
			}
		}

// příprava indexu první položky
		int inx = m_First;					// první položka, není-li rodič platný
		if (IsValid(parent))				// je rodič platný?
		{
			inx = itemPar->Child;			// první potomek rodiče
		}

// nalezení pozice
		while (IsValid(inx) && (txt >= Text.GetTxt(GetText(inx))))
		{
			after = inx;					// položka bude předcházející
			inx = m_Data[inx].Next;			// posun na další položku
		}
	}

// nalezení předchozí položky abecedně s předností skupin
	if (after == -4)						// je abecední třídění?
	{
		after = -1;							// uložení na začátek

// příprava textu vkládané položky
		CText txt;
		if (item->Name >= 0)
		{
			txt = Text.GetTxt(item->Name);
		}
		else
		{
			if (item->RefBlok >= 0)
			{
				txt = Text.GetTxt(BufProg[item->RefBlok].GetText(item->RefIndex));
			}
		}

// příprava indexu první položky
		int inx = m_First;					// první položka, není-li rodič platný
		if (IsValid(parent))				// je rodič platný?
		{
			inx = itemPar->Child;			// první potomek rodiče
		}

// nalezení pozice
		while (IsValid(inx) &&				// položka je platná
				(
					(
						(m_Data[inx].Func == IDF_GROUP) && // ne-skupina se řadí vždy za skupinu
						(item->Func != IDF_GROUP)
					) ||
					(
						(m_Data[inx].Func == IDF_GROUP) &&	// abecední setřídění skupin
						(item->Func == IDF_GROUP) &&
						(txt >= Text.GetTxt(GetText(inx)))
					) ||
					(
						(m_Data[inx].Func != IDF_GROUP) &&	// abecední setřídění ne-skupin
						(item->Func != IDF_GROUP) &&
						(txt >= Text.GetTxt(GetText(inx)))
					)
				)
			)
		{
			after = inx;					// položka bude předcházející
			inx = m_Data[inx].Next;			// posun na další položku
		}
	}

// příprava adresy předešlé položky (i když je neplatná)
	PROGITEM* itemAft = m_Data + after;				// adresa předcházející položky

// nemá-li to být první položka, bude se hledat napojení na předcházející položku
	if (after != -1)
	{

// nalezení stejného rodiče, je-li předešlá položka v podvětvi
		if (IsValid(after))					// je předešlá položka platná?
		{
			while ((itemAft->Parent != parent) && (itemAft->Parent >= 0))
			{
				after = itemAft->Parent;
				itemAft = m_Data + after;			// adresa předcházející položky
			}
		}

// ověření, zda je předcházející položka platná (musí mít stejného rodiče)
		if (IsNotValid(after) || (itemAft->Parent != parent))
		{	

// příprava výchozí položky - buď první potomek rodiče nebo první položka ROOT
			if (parent >= 0)					// je rodič platný?
			{
				after = itemPar->Child;			// první potomek rodiče
			}
			else
			{
				after = m_First;				// jinak první položka v ROOT
			}

// nalezení poslední položky
			if (after >= 0)						// je nějaká položka?
			{
				itemAft = m_Data + after;		// adresa položky

				while (itemAft->Next >= 0)		// je platná další položka?
				{
					after = itemAft->Next;		// posun na další položku
					itemAft = m_Data + after;	// adresa další položky
				}
			}
		}
	}

// napojení položky na předcházející položku (-1 = první)
	itemNew->Prev = after;					// odkaz na předcházející položku
	if (after >= 0)							// je předcházející položka platná?
	{
		itemNew->Next = itemAft->Next;		// přenesení následující položky
		itemAft->Next = index;				// navázání odkazu na tuto položku
	}
	else
	{
		if (parent >= 0)					// je rodič platný?
		{
			itemNew->Next = itemPar->Child;	// předřazení před prvního potomka
			itemPar->Child = index;			// položka bude prvním potomkem rodiče

			if ((itemNew->Next < 0) && (itemPar->HTree != NULL) && (m_Tree != NULL))	// první potomek rodiče?
			{
				TV_ITEM tvi;
				tvi.hItem = itemPar->HTree;
				tvi.mask = TVIF_CHILDREN;
				tvi.cChildren = 1;
				::SendMessage(m_Tree, TVM_SETITEM, 0, (LPARAM)&tvi);

				AktStateImage(parent);		// aktualizace stavové ikony
			}
		}
		else
		{
			itemNew->Next = m_First;		// předřazení před první ROOT položku
			m_First = index;				// položka bude první ROOT položkou
		}
	}

// napojení před následující položku
	if (itemNew->Next >= 0)
	{
		m_Data[itemNew->Next].Prev = index;
	}

// inicializace ostatních parametrů
	itemNew->Child = -1;					// není žádný potomek
	itemNew->HTree = NULL;					// není zobrazení položky stromu
	itemNew->Refer = 0;						// nejsou žádné reference na položku

// zvýšení čítače u referenční položky (ne pro buffer tříd)
	if (((DWORD)(itemNew->RefBlok) < (DWORD)PROGBUFNUM) && (m_BufID != BufClsID))
	{
		if (BufProg[itemNew->RefBlok].IsValid(itemNew->RefIndex))
		{
			BufProg[itemNew->RefBlok][itemNew->RefIndex].Refer++;
		}
	}

// zobrazení položky, je-li v zobrazené větvi
	if ((((parent >= 0) && (itemPar->HTree != NULL)) ||
		(m_Disp == parent)))
	{
		DispNewItem(index);
	}

// navrácení indexu nové položky
	return index;
}


////////////////////////////////////////////////////////////////////
// načtení inicializační tabulky bufferu (starý obsah bufferu se neruší - lze přidávat)
// při chybě paměti vrací FALSE

bool CBufProg::InitTab(const INITTREE* tab, const int num)
{
// vypnutí překreslování okna
	RedrawOff();

// příprava bufferu rodičů
	int ParBuf[30];							// to snad bude vždy dostačující
	for (int i = 0; i < 30; i++) ParBuf[i] = -1; // rodič je neplatný

// inicializace neměnných parametrů položky
	PROGITEM item;							// vytvářená nová položka
	item.RefBlok = BufIntID;				// reference na buffer interních funkcí
	item.DatBlok = -1;						// datový blok není
	item.DatIndex = -1;						// datový index
	item.Icon = -1;							// ikona implicitní
	item.Name = -1;							// jméno implicitní

// cyklus přes položky tabulky
	for (i = 0; i < num; i++)
	{

// příprava odkazu na deklarační funkci
		item.Func = tab[i].Func;			// identifikační kód funkce
		int fnc = tab[i].Func - IDF;		// index funkce
		ASSERT(BufInt.IsValid(fnc));
		PROGITEM* itemfnc = &BufInt[fnc];	// adresa deklarační funkce

// přenesení parametrů z deklarační funkce
		item.SrcMask = itemfnc->SrcMask;	// zdrojové vlastnosti
		item.DstMask = itemfnc->DstMask;	// cílové vlastnosti
		item.Param = itemfnc->Param;		// parametry

// odkaz na deklarační funkci
		item.RefIndex = fnc;				// referenční index

// přidání položky do bufferu (a aktualizace bufferu rodičů)
		int level = tab[i].Level;
		if (level & NOREFER) item.Param |= PR_NOREFER;
		level = level & ~NOREFER;

		ASSERT(level < 28);
		int ii = Insert(&item, ParBuf[level], -2);
		if (ii < 0)
		{
			RedrawOn();
			return false;
		}
		ParBuf[level + 1] = ii;
	}
// zapnutí překreslování okna
	RedrawOn();

	return true;
}


////////////////////////////////////////////////////////////////////
// vytvoření prázdné položky (vrací index položky, -1=chyba paměti)

int CBufProg::New()
{
	int result = NewItem();		// vytvoření nové položky
	if (result >= 0)
	{
		MemCopy(m_Data + result, &InitProgItem, SIZEOFPROGITEM);
	}
	return result;
}


////////////////////////////////////////////////////////////////////
// nalezení položky podle handle (-1 = není)

int CBufProg::Find(HTREEITEM htree)
{
// je to platný handle?
	if (htree == NULL) return -1;

// příprava výchozí zobrazené položky
	int index;
	if (m_Disp < 0)
	{
		index = m_First;
	}
	else
	{
		index = m_Data[m_Disp].Child;
	}

// kontrola, zda je platná výchozí položka
	if (IsNotValid(index)) return -1;

// příprava koncového indexu
	int last = m_Data[index].Parent;		// koncový index

// cyklus nalezení položky
	for (;;)
	{

// adresa testované položky
		PROGITEM* item = m_Data + index;			// adresa testované položky

// test, zda je to hledaná položka
		if (item->HTree == htree) return index;

// pokus o vnoření do potomků
		index = item->Child;			// vnoření na potomka
		if (index < 0)					// není žádný potomek?
		{

// nejsou-li potomci, zkusí se následující položka
			index = item->Next;			// pokračování další položkou

// není-li ani další položka, vrátí se k rodiči
			while (index < 0)
			{
				index = item->Parent;	// návrat k rodiči
				if (index == last) return -1;	// položka nenalezena
				item = m_Data + index;	// adresa rodiče
				index = item->Next;		// další položka za rodičem
			}
		}
	}
}


////////////////////////////////////////////////////////////////////
// nalezení položky s ukazatelem na data (-1 = není)

int CBufProg::SrcDat(int bufID, int index)
{
// kontrola platnosti údajů
	if ((bufID < 0) || (index < 0)) return -1;

// nalezení položky
	int i = m_Max - 1;
	PROGITEM* item = m_Data + i;
	for (; i >= 0; i--)
	{
		if ((m_Valid[i]) &&
			(item->DatIndex == index) &&
			(item->DatBlok == bufID))
		{
			return i;
		}
		item--;
	}
	return -1;
}


/////////////////////////////////////////////////////////////////////////////
// nalezení referující položky od aktuální položky cyklicky (-1 = nenalezeno)

int CBufProg::SrcRef(int RefBlok, int RefIndex, BOOL next)
{
// příprava výchozí položky
	int index = Select();						// vybraná položka
	PROGITEM* item;								// adresa položky

// test první položky, není-li žádná položka vybraná (jen při směru vpřed)
	if (IsNotValid(index))						// hledá se od začátku?
	{		
		index = m_First;						// index první položky
		if (index < 0) return -1;				// není první položka

		if (next)								// hledá se směrem vpřed?
		{
			item = m_Data + index;				// adresa položky
			if ((item->RefBlok == RefBlok) &&	// souhlasí referenční buffer ?
				(item->RefIndex == RefIndex))	// souhlasí referenční index ?
			{
				return index;					// index nalezené položky
			}
		}
	}

// cyklus vyhledání položky
	for (int i = m_Num; i > 0; i--)
	{

// index další položky (cyklicky, až opět po vybranou položku)
		if (next)
		{
			index = NextItem(index);
		}
		else
		{
			index = PrevItem(index);
		}

// adresa položky
		item = m_Data + index;

// test, zda to je hledaná položka
		if ((item->RefBlok == RefBlok) &&		// souhlasí referenční buffer ?
			(item->RefIndex == RefIndex))		// souhlasí referenční index ?
		{
			return index;						// index nalezené položky
		}
	}

	return -1;									// položka nenalezena
}


////////////////////////////////////////////////////////////////////
// nalezení indexu textu k položce (-1 = neplatný)

int _fastcall CBufProg::GetText(const int index)
{
	if (IsNotValid(index)) return -1;

	PROGITEM* item = m_Data + index;

	while ((item->Name < 0) && ((DWORD)item->RefBlok < (DWORD)PROGBUFNUM))
	{
		item = &(BufProg[item->RefBlok][item->RefIndex]);
	}

	return item->Name;
}

////////////////////////////////////////////////////////////////////
// nalezení indexu ikony k položce (-1 = neplatný)

int _fastcall CBufProg::GetIcon(const int index)
{
	if (IsNotValid(index)) return -1;

	PROGITEM* item = m_Data + index;

	while ((item->Icon < 0) && ((DWORD)item->RefBlok < (DWORD)PROGBUFNUM))
	{
		item = &(BufProg[item->RefBlok][item->RefIndex]);
	}

	return item->Icon;
}


////////////////////////////////////////////////////////////////////
// poskytnutí počtu referencí pro uživatele

int CBufProg::GetRefer(int index)
{
// kontrola platnosti položky
	int ref = 0;
	if (IsValid(index))
	{

// adresa položky
		PROGITEM* item = m_Data + index;

// počet referencí na položku
		ref = item->Refer;

// z editoru přesměrování na deklarační buffer
		if ((m_BufID == BufEdiID) && ((DWORD)item->RefBlok < (DWORD)PROGBUFNUM) && (item->RefBlok != BufEdiID))
		{
			ref = BufProg[item->RefBlok].GetRefer(item->RefIndex);
		}

// v lokálním bufferu korekce vstupních proměnných
		if ((m_BufID == BufLocID) && (ref > 0) && (item->DatBlok >= 0))
		{
			int inx2 = item->Parent;

			if (inx2 >= 0)
			{
				PROGITEM* item2 = m_Data + inx2;
				if (item2->Func == IDF_PAR)
				{
					while (item2->Parent >= 0)
					{
						inx2 = item2->Parent;
						item2 = m_Data + inx2;
					}

					inx2 = BufObj.SrcDat(BufLocID, inx2);
					if (BufObj.IsValid(inx2))
					{
						ref -= BufObj[inx2].Refer;
					}
					ASSERT(ref >= 0);
					if (ref < 0) ref = 0;
				}
			}
		}
	}
	return ref;
}


////////////////////////////////////////////////////////////////////
// test povolení zrušení větve z hlediska referencí na položky (TRUE=OK)

bool CBufProg::TestRefer(int index)
{
// kontrola, zda je položka platná a zda je to buffer objektů
	if (IsNotValid(index) || ((m_BufID != BufObjID) && (m_BufID != BufLocID))) return true;

// úschova výchozí položky
	int first = index;						// úschova výchozí položky
	PROGITEM* item;

// cyklus přes celou větev
	for (;;)
	{

// test, zda jsou na položku nějaké reference
		if (GetRefer(index) > 0)
		{

// potvrzení operace
			CText text;
			CText nadpis;
			if (!text.Load(IDN_TESTREFER) ||
				!nadpis.Load(IDN_TESTREFER2)) return false;

			return (::MessageBox(MainFrame, text, nadpis,
				MB_OKCANCEL | MB_ICONEXCLAMATION) == IDOK);
		}

// adresa položky
		item = m_Data + index;

// nalezení následující položky
		if ((index == first) && (item->Child < 0)) return true;

		index = item->Child;			// vnoření na potomka
		if (index < 0)					// není žádný potomek?
		{
			index = item->Next;			// pokračování další položkou

			while (index < 0)
			{
				index = item->Parent;	// návrat k rodiči
				if (index == first) return true;
				ASSERT(index >= 0);
				item = m_Data + index;	// adresa rodiče
				index = item->Next;		// další položka za rodičem
			}
		}
	}
}


////////////////////////////////////////////////////////////////////
// překreslení ikony ve stromu

void CBufProg::ReDispIcon(int icon)
{
// kontrola platnosti ikony a je-li nějaký strom
	if (Icon.IsNotValid(icon) || (m_Tree == NULL)) return;

// příprava indexu položky
	int index = m_Max-1;
	PROGITEM* item = m_Data + index;
	int icon2;
	TV_ITEM tvi;
	tvi.mask = TVIF_IMAGE;

// cyklus přes všechny položky
	for (; index >= 0; index--)
	{

// kontrola, zda je položka platná a zda je zobrazena
		if (m_Valid[index] && (item->HTree != NULL))
		{

// nalezení indexu ikony
			icon2 = GetIcon(index);

// kontrola, zda je to hledaná ikona
			if (icon2 == icon)
			{

// zajištění překreslení položky
				tvi.hItem = item->HTree;
				tvi.iImage = 0;
				::SendMessage(m_Tree, TVM_SETITEM, 0, (LPARAM)&tvi);
				tvi.iImage = I_IMAGECALLBACK;
				::SendMessage(m_Tree, TVM_SETITEM, 0, (LPARAM)&tvi);
			}
		}

// posun adresy položky
		item--;
	}
}


////////////////////////////////////////////////////////////////////
// test čísla funkce, zda je to objekt (s jedinečným jménem)

bool _fastcall CBufProg::TestObj(int func)
{
	switch(func)
	{
	case IDF_FNC:
	case IDF_REAL:
	case IDF_BOOL:
	case IDF_TEXT:
	case IDF_ICON:
	case IDF_MAP:
	case IDF_PIC:
	case IDF_SND:
	case IDF_MUS:
	case IDF_SPRITE:
	case IDF_LIST:
	case IDF_LIST_SIZE:
	case IDF_LIST_INDEX:
	case IDF_LIST_AUTO:
	case IDF_SPRITE_X:
	case IDF_SPRITE_Y:
	case IDF_SPRITE_SMER:
	case IDF_SPRITE_MOVE:
	case IDF_SPRITE_VISIBLE:
	case IDF_SPRITE_FAZE:
	case IDF_SPRITE_STATUS:
	case IDF_RETURN_CODE:
	case IDF_COMMAND_LINE:
	case IDF_NONE:
		return true;
	}
	return false;
}


////////////////////////////////////////////////////////////////////
// nalezení objektu daného jména (-1 = nenalezeno) - rozlišuje velká/malá písmena a mezery
//		name ..... hledaný text
//		over ..... přeskočená položka (-1 = není)
//		first .... první položka (-1 = od začátku)
//		next ..... příští položka pro zastavení (-1 = po konec)

int CBufProg::FindObj(const CMultiText& name, int over, int first, int next)
{
// prázdný text - položka nenalezena
	if (name.MultiText().IsEmpty()) return -1;

// korekce první položky
	if (IsNotValid(first))
	{
		first = m_First;
		if (IsNotValid(first)) return -1;
	}

// korekce příští položky
	if (IsNotValid(next))
	{
		next = m_First;
	}

// cyklus přes všechny položky
	CMultiText txt;
	do
	{

// adresa položky
		PROGITEM* item = m_Data + first;

// test, zda je to položka objektu
		if (TestObj(item->Func) &&
			(first != over))
		{
			CBufProg* buf = this;
			int inx = first;

// přesměrování z editoru na objekty
			if ((item->RefBlok == BufObjID) ||
				(item->RefBlok == BufLocID))
			{
				inx = item->RefIndex;
				buf = BufProg + item->RefBlok;
				ASSERT(buf->IsValid(inx));
			}

// netestuje se ROOT lokální funkce
			if ((m_BufID != BufLocID) || (item->Parent >= 0))
			{

// porovnání jména objektu (stačí shodnost jednoho v kterémkoliv z jazyků)
				int itx = buf->GetText(inx);
				if (itx >= 0)
				{
					txt = Text.Get(itx);

					BOOL ok = false;

					for (int lan = 0; lan < JAZYKNUM; lan++)
					{
						if (name.MultiText(lan).IsNotEmpty() &&
							(txt.MultiText(lan) == name.MultiText(lan)))
						{
							ok = true;
							break;
						}
					}

					if (ok) return first;
				}
			}
		}

// dalši položka
		first = NextItem(first);
		ASSERT(first >= 0);

// až po poslední položku
	} while (first != next);

// objekt nenalezen
	return -1;
}


////////////////////////////////////////////////////////////////////
// zrušení větve stromu (při chybě paměti vrací FALSE - stav není navrácen!)

bool CBufProg::Del(int index)
{
// kontrola platnosti indexu
	if (IsNotValid(index)) return true;

// lokální proměnné
	int first = index;					// úschova indexu výchozí položky
	PROGITEM* item;						// adresa rušené položky
	CBufProg* buf;						// adresa referujícího bufferu
	PROGITEM* item2;					// adresa referující položky
	int i;								// pomocný čítač (index referující položky)
	int next = index;					// index příští položky k rušení
	int parent;							// index rodiče rušené položky

// cyklus přes všechny položky - začíná od nejhlubších položek
	do {

// příprava adresy položky
		index = next;					// index rušené položky
		item = m_Data + index;			// adresa rušené položky
		parent = item->Parent;			// index rodiče rušené položky

// pokud je nějaký potomek, vnoří se nejdříve do potomka
		if (item->Child >= 0)			// má položka potomky?
		{
			next = item->Child;			// příště se ruší potomek položky
			index = next;				// index aktuální položky (kvůli kontrole WHILE)
		}

// položka je bez dalších potomků, může se zrušit
		else
		{

// kontrola, jestli jsou na položku nějaké reference
			if (item->Refer > 0)
			{

// zrušení referencí z bufferu editoru programu
				ASSERT((m_BufID == BufObjID) || (m_BufID == BufLocID));

				buf = &BufEdi;			// adresa bufferu editoru programu
				i = buf->Max()-1;		// index poslední položky v bufferu
				item2 = buf->Data() + i; // adresa poslední položky

				for (; i >= 0; i--)		// cyklus přes všechny položky bufferu
				{
					if (buf->m_Valid[i])	// je to platná položka?
					{
						if ((item2->RefIndex == index) && // je to hledaná reference?
							(item2->RefBlok == m_BufID))
						{
							if (!buf->Del(i)) return false;	// zrušení referující položky
						}
					}
					item2--;			// posun adresy referující položky
				}

// pro ikonu zrušení její použití ve všech plochách
				if ((item->Refer > 0) && (item->DatBlok == BufIcoID))
				{
					Map.DelIcon(item->DatIndex);
				}

				ASSERT(item->Refer == 0);	// teď již nesmí nic referovat!
			}

// zrušení programových dat
			if ((item->DatIndex >= 0) && ((DWORD)item->DatBlok < (DWORD)PROGBUFNUM))
			{
				if (!HistEdit::Del(item->DatBlok, item->DatIndex, -1, -1)) return false;

				switch (item->DatBlok)
				{
				case BufLocID:	if (!BufLoc.Del(item->DatIndex)) return false;
								break;

				case BufEdiID:	if (!BufEdi.Del(item->DatIndex)) return false;
								break;
				}
			}

// záznam UNDO operace (data se při UNDO musí navracet dříve)
			if (m_Undo)
			{
				if (!Undo.AddProgDel(m_BufID, index, item))
				{
					Undo.DelAll();
					return false;
				}
			}

// zrušení dat položky
			if ((item->DatIndex >= 0) && ((DWORD)item->DatBlok >= (DWORD)PROGBUFNUM))
			{
				if (!HistEdit::Del(item->DatBlok, item->DatIndex, -1, -1))
				{
					Undo.DelAll();
					return false;
				}

				switch (item->DatBlok)
				{
				case BufNumID:	if (!Real.Del(item->DatIndex))
								{
									Undo.DelAll();
									return false;
								}
								break;

				case BufTxtID:	if (!Text.Del(item->DatIndex))
								{
									Undo.DelAll();
									return false;
								}
								break;

				case BufLogID:	if (!Bool.Del(item->DatIndex))
								{
									Undo.DelAll();
									return false;
								}
								break;

				case BufIcoID:	if (!Icon.Del(item->DatIndex))
								{
									Undo.DelAll();
									return false;
								}
								break;

				case BufMapID:	
								Map[item->DatIndex].RefDec();
								if (!Map.Del(item->DatIndex))
								{
									Undo.DelAll();
									return false;
								}
								break;

				case BufPicID:	if (!Picture.Del(item->DatIndex))
								{
									Undo.DelAll();
									return false;
								}
								break;

				case BufSprID:	
								if (!HistEdit::Del(BufPicID, -2, item->DatIndex, -1) ||
								    !Sprite.Del(item->DatIndex))
								{
									Undo.DelAll();
									return false;
								}
								break;

				case BufSndID:	if (!Sound.Del(item->DatIndex))
								{
									Undo.DelAll();
									return false;
								}
								break;

				case BufMusID:	if (!Music.Del(item->DatIndex))
								{
									Undo.DelAll();
									return false;
								}
								break;

				default:
					ASSERTERROR;
				}
			}

// zrušení ikony položky
			if ((item->Icon >= 0) && (item->DatBlok != BufIcoID))
			{
				if (!HistEdit::Del(BufIcoID, item->Icon, -1, -1))
				{
					Undo.DelAll();
					return false;
				}
				
				ASSERT(Icon.IsValid(item->Icon));
				ASSERT(item->Icon >= FncNum);
				if (!Icon.Del(item->Icon))
				{
					Undo.DelAll();
					return false;
				}
			}

// zrušení textu položky
			if (item->Name >= 0)
			{
				if (!HistEdit::Del(BufTxtID, item->Name, -1, -1))
				{
					Undo.DelAll();
					return false;
				}

				ASSERT(Text.IsValid(item->Name));
				if (!Text.Del(item->Name))
				{
					Undo.DelAll();
					return false;
				}
			}

// zrušení zobrazení položky
			if (item->HTree != NULL)
			{
				::SendMessage(m_Tree, TVM_DELETEITEM, 0, (LPARAM)item->HTree);
			}

// úschova příští položky
			next = item->Next;

// napojení předcházející položky na následující
			if (next >= 0)
			{
				m_Data[next].Prev = item->Prev;
			}

// napojení následující položky na předcházejíci
			if (item->Prev >= 0)
			{
				m_Data[item->Prev].Next = next;
			}

// oprava odkazu z rodiče
			if ((parent >= 0) && (m_Data[parent].Child == index))
			{
				m_Data[parent].Child = next;
			}

// přesměrování ukazatele na první položku
			if (index == m_First)
			{
				m_First = next;
			}

// přesměrování rodiče zobrazených položek
			if (index == m_Disp)
			{
				m_Disp = -2;
			}

// není-li další položka, poukračuje se rodičem
			if ((next < 0) && (index != first))
			{
				next = parent;
			}

// snížení čítače referencí
			if (((DWORD)item->RefBlok < (DWORD)PROGBUFNUM) && (m_BufID != BufClsID) &&
				BufProg[item->RefBlok].IsValid(item->RefIndex))
			{
				BufProg[item->RefBlok][item->RefIndex].Refer--;
			}

// zrušení definované položky
			if (index == m_Def) m_Def = -1;

// zrušení položky
			DelItem(index);
		}

// dokud nebyla zrušena výchozí položka
	} while (index != first);

// korekce zobrazení potomků u rodiče
	if (parent >= 0)
	{
		item = m_Data + parent;

		if ((item->Child < 0) && (item->HTree != NULL))
		{
			TV_ITEM tvi;
			tvi.hItem = item->HTree;

			if (m_BufID == BufClsID)
			{
				item->Param &= ~PR_EXP;
				tvi.mask = TVIF_STATE;
				tvi.stateMask = TVIS_EXPANDED;
				tvi.state = 0;
			}
			else
			{
				tvi.mask = TVIF_CHILDREN;
				tvi.cChildren = 0;
			}

			::SendMessage(m_Tree, TVM_SETITEM, 0, (LPARAM)&tvi);
			AktStateImage(parent);
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////////
// přesun větve stromu (vrací nový index položky) 
// (parent: -1 = ROOT, after: -1 = začátek, -2 = konec)

int CBufProg::Move(int parent, int after, int src)
{
// lokální proměnné
	PROGITEM*	item;						// adresa přenášené položky
	PROGITEM*	itemPar;					// adresa cílového rodiče
	PROGITEM*	itemAft;					// adresa cílové předcházející položky
	CBufProg*	bufedi = &BufEdi;			// editační buffer
	int			inxedi;						// index položky v editačním bufferu
	PROGITEM*	itemedi;					// adresa položky v editačním bufferu
	int oldparent;							// úschovaný rodič zdrojové položky
	int first;								// uschovaná výchozí zdrojová položka

// kontrola platnosti zdrojového indexu
	ASSERT(IsValid(src));
	if (IsNotValid(src)) return src;		// nic se nemění
	item = m_Data + src;					// adresa položky
	oldparent = item->Parent;				// původní rodič zdrojové položky


// -------------- korekce ukazatelů

// kontrola platnosti nového rodiče
	if (IsNotValid(parent))					// je platný rodič?
	{
		parent = -1;						// není platný - bude to ROOT položka
	}

// adresa cílového rodiče (i neplatný)
	itemPar = m_Data + parent;				// adresa rodiče (i když je neplatný)

// adresa cílové předcházející položky (i neplatné)
	itemAft = m_Data + after;				// adresa předcházející položky (i když je neplatná)

// není-li požadována první položka, bude se hledat napojení na předcházející položku
	if (after != -1)
	{

// korekce předešlé cílové položky, je-li v podvětvi
		if (IsValid(after))					// je předešlá položka platná?
		{
			while ((itemAft->Parent != parent) && (itemAft->Parent >= 0))
			{
				after = itemAft->Parent;
				itemAft = m_Data + after;			// adresa předcházející položky
			}
		}

// ověření, zda je předcházející položka platná (musí mít stejného rodiče)
		if (IsNotValid(after) || (itemAft->Parent != parent))
		{	

// příprava výchozí předešlé položky - buď první potomek rodiče nebo první položka ROOT
			if (parent >= 0)					// je rodič platný?
			{
				after = itemPar->Child;			// první potomek rodiče
			}
			else
			{
				after = m_First;				// jinak první položka v ROOT
			}

// nalezení poslední položky
			if (after >= 0)						// je nějaká položka?
			{
				itemAft = m_Data + after;		// adresa položky

				while (itemAft->Next >= 0)		// je platná další položka?
				{
					after = itemAft->Next;		// posun na další položku
					itemAft = m_Data + after;	// adresa další položky
				}
			}
		}
	}

// kontrola, zda se vůbec něco mění
	if ((oldparent == parent) && 
		((item->Prev == after) || (src == after)))
	{
		return src;
	}

// příznak modifikace souboru
	SetModi();

// záznam pro UNDO
	if (m_Undo)
	{
		if (!Undo.AddProgMove(m_BufID, src, oldparent, item->Prev))
		{
			Undo.DelAll();
		}
	}

// ---------------- odpojení větve

// odpojení od předcházející položky
	if (item->Prev >= 0)
	{
		m_Data[item->Prev].Next = item->Next;
	}
	else
	{

// odpojení od rodiče
		if (oldparent >= 0)
		{
			m_Data[oldparent].Child = item->Next;

// aktualizace rodiče, pokud to byl jeho poslední potomek
			if ((item->Next < 0) && (m_Data[oldparent].HTree != NULL))
			{
				TV_ITEM tvi;
				tvi.hItem = m_Data[oldparent].HTree;
				tvi.mask = TVIF_CHILDREN;
				tvi.cChildren = 0;
				::SendMessage(m_Tree, TVM_SETITEM, 0, (LPARAM)&tvi);
			}
		}
	}

// oprava ukazatele na první položku
	if (m_First == src)
	{
		m_First = item->Next;
	}

// odpojení od následující položky
	if (item->Next >= 0)
	{
		m_Data[item->Next].Prev = item->Prev;
	}

// zrušení zobrazení položky
	if (item->HTree)
	{
		::SendMessage(m_Tree, TVM_DELETEITEM, 0, (LPARAM)item->HTree);
	}

// ----------------- odsun vstupního parametru

// kontrola, zda je odsunut vstupní parametr mezi lokální proměnné
	if ((m_BufID == BufLocID) &&
		(oldparent >= 0) &&
		(m_Data[oldparent].Func == IDF_PAR) &&
		(parent != oldparent))
	{

// nalezení parametru funkce v editačním bufferu (odlišovat od ROOT funkce v editoru)
		inxedi = bufedi->Max() - 1;			// index poslední položky
		itemedi = bufedi->Data() + inxedi;	// adresa poslední položky
		for (; inxedi >= 0; inxedi--)
		{

// je to parametr funkce?
			if (bufedi->m_Valid[inxedi] &&
				(itemedi->RefIndex == src) &&
				(itemedi->RefBlok == BufLocID) &&
				(itemedi->Parent >= 0) &&
				(bufedi->At(itemedi->Parent).Func == IDF_FNC) &&
				((bufedi->At(itemedi->Parent).DstMask & PR_ALL) == 0) &&
				(bufedi->At(itemedi->Parent).RefBlok == BufObjID))

// zrušení parametru funkce
			{
				bufedi->Del(inxedi);
			}
			itemedi--;
		}
	}

// --------------- připojení větve

// nastavení odkazu na nového rodiče
	item->Parent = parent;				// odkaz na rodiče (pro ROOT bude = -1)

// napojení položky na předcházející položku
	item->Prev = after;					// odkaz na předcházející položku
	if (after >= 0)							// je předcházející položka platná?
	{
		item->Next = itemAft->Next;			// přenesení následující položky
		itemAft->Next = src;				// navázání odkazu na tuto položku
	}
	else
	{
		if (parent >= 0)					// je rodič platný?
		{
			if ((itemPar->Child < 0) && itemPar->HTree)	// je to první potomek rodiče
			{
				TV_ITEM tvi;
				tvi.hItem = itemPar->HTree;
				tvi.mask = TVIF_CHILDREN;
				tvi.cChildren = 1;
				::SendMessage(m_Tree, TVM_SETITEM, 0, (LPARAM)&tvi);
			}

			item->Next = itemPar->Child;	// předřazení před prvního potomka
			itemPar->Child = src;			// položka bude prvním potomkem rodiče
		}
		else
		{
			item->Next = m_First;			// předřazení před první ROOT položku
			m_First = src;					// položka bude první ROOT položkou
		}
	}

// napojení položky na následující položku
	if (item->Next >= 0)
	{
		m_Data[item->Next].Prev = src;
	}

// --------------- přísun vstupního parametru

// kontrola, zda je přisunut vstupní parametr z lokálních proměnných
	if ((m_BufID == BufLocID) &&
		(parent >= 0) &&
		(itemPar->Func == IDF_PAR) &&
		(parent != oldparent))
	{

// příprava indexu globální funkce
		int reffnc = BufObj.SrcDat(BufLocID, itemPar->Parent);
		if (reffnc >= 0)
		{

// nalezení funkce v editačním bufferu
			inxedi = bufedi->Max() - 1;			// index poslední položky
			itemedi = bufedi->Data() + inxedi;	// adresa poslední položky
			for (; inxedi >= 0; inxedi--)
			{

// je to použití funkce?
				if (bufedi->m_Valid[inxedi] &&
					(itemedi->RefIndex == reffnc) &&
					(itemedi->RefBlok == BufObjID) &&
					(itemedi->Func == IDF_FNC) &&
					((itemedi->DstMask & PR_ALL) == 0))
				{

// kopie parametru funkce
					int ii = bufedi->Copy(inxedi, -2, BufLocID, src);
					ASSERT(ii >= 0);

// zákaz přesunu parametru
					if (ii >= 0)
					{
						bufedi->At(ii).Param |= PR_NOMOVE;
					}
				}
				itemedi--;
			}
		}
	}

// --------------- zobrazení větve na nové pozici

// zobrazení přenesených položek, jsou-li v zobrazené větvi
	if ((((parent >= 0) && (m_Data[parent].HTree != NULL)) || (m_Disp == parent)))
	{
		first = src;							// výchozí položka
		do {

// zobrazení položky
			DispNewItem(src);

// adresa položky
			item = m_Data + src;

// nalezení následující položky
			if ((src != first) || (item->Child >= 0))
			{
				src = item->Child;			// vnoření na potomka
			}

			if (src < 0)
			{
				src = item->Next;			// pokračování další položkou

				while (src < 0)
				{
					src = item->Parent;	// návrat k rodiči
					ASSERT(src >= 0);
					if (src == first) break;
					item = m_Data + src;	// adresa rodiče
					src = item->Next;		// další položka za rodičem
				}
			}

// dokud je další platná položka
		} while (src != first);
	}

// aktualizace stavových ikon rodičů
	AktStateImage(parent);
	if (parent != oldparent)
	{
		AktStateImage(oldparent);
	}

	return src;
}


////////////////////////////////////////////////////////////////////
// kopie větve stromu - vrací cílový index položky
// (parent: -1 = ROOT, pos: -1 = začátek, -2 = konec)

int CBufProg::Copy(int parent, int pos, int bufID, int src)
{
// lokální proměnné
	ASSERT((DWORD)bufID < (DWORD)PROGBUFNUM);
	CBufProg* buf = BufProg + bufID;			// zdrojový buffer
	PROGITEM item;								// pracovní buffer položky
	PROGITEM* itemsrc;							// adresa zdrojové položky
	PROGITEM* itemdst;							// adresa cílové položky
	PROGITEM*	itemPar;						// adresa cílového rodiče
	PROGITEM*	itemAft;						// adresa cílové předcházející položky
	int dst;									// index cílové položky
	int first = src;							// úschova výchozí zdrojové položky
	int result;									// výsledek
	int newparent;								// úschova cílového rodiče

// kontrola zdrojové položky
	ASSERT(buf->IsValid(src));
	if(buf->IsNotValid(src)) return -1;

// zapnutí čekacího kurzoru
	BeginWaitCursor();

// -------------- korekce ukazatelů

// kontrola platnosti nového rodiče
	if (IsNotValid(parent))						// je platný rodič?
	{
		parent = -1;							// není platný - bude to ROOT položka
	}
	result = parent;							// výsledek
	newparent = parent;							// úschova cílového rodiče

// adresa cílového rodiče (i neplatný)
	itemPar = m_Data + parent;				// adresa rodiče (i když je neplatný)

// adresa cílové předcházející položky (i neplatné)
	itemAft = m_Data + pos;					// adresa předcházející položky (i když je neplatná)

// není-li požadována první položka, bude se hledat napojení na předcházející položku
	if (pos != -1)
	{

// korekce předešlé cílové položky, je-li v podvětvi
		if (IsValid(pos))					// je předešlá položka platná?
		{
			while ((itemAft->Parent != parent) && (itemAft->Parent >= 0))
			{
				pos = itemAft->Parent;
				itemAft = m_Data + pos;			// adresa předcházející položky
			}
		}

// ověření, zda je předcházející položka platná (musí mít stejného rodiče)
		if (IsNotValid(pos) || (itemAft->Parent != parent))
		{	

// příprava výchozí předešlé položky - buď první potomek rodiče nebo první položka ROOT
			if (parent >= 0)					// je rodič platný?
			{
				pos = itemPar->Child;			// první potomek rodiče
			}
			else
			{
				pos = m_First;				// jinak první položka v ROOT
			}

// nalezení poslední položky
			if (pos >= 0)						// je nějaká položka?
			{
				itemAft = m_Data + pos;		// adresa položky

				while (itemAft->Next >= 0)		// je platná další položka?
				{
					pos = itemAft->Next;		// posun na další položku
					itemAft = m_Data + pos;	// adresa další položky
				}
			}
		}
	}

// ------------ příprava parametrů kopírované položky

// cyklus přes všechny položky zdrojové větve
	do {

// při kopii skupiny z okna tříd zajištění načtení obsahu skupiny
		if ((bufID == BufClsID) && 
			(buf->At(src).Func == IDF_GROUP) &&
			(buf->At(src).Child < 0))
		{
			ProgLib::Load(src);
		}

// kopie obsahu položky do pracovního bufferu
		MemCopy(&item, &(buf->At(src)), SIZEOFPROGITEM);	// kopie položky

// test, zda se mají kopírovat potomci
		if (	(	(bufID == BufStrID) &&				// kopie z bufferu interních struktur
					(	(item.Func == IDF_NUM)// ||		// číslo se kopíruje bez obsahu
//						(item.Func == IDF_WHILE_BODY)	// cyklus se kopíruje bez přerušení
					)
				) ||
				((bufID == BufClsID) && (item.Func != IDF_GROUP)) || // třídy se kopírují bez potomků
				((bufID == BufLocID) && ((m_BufID == BufEdiID) || (m_BufID == BufClsID))) ||	// z lokálních objektů vše bez potomků
				((bufID == BufObjID) && ((m_BufID == BufEdiID) || (m_BufID == BufClsID)))	// z globálních objektů vše bez potomků
			)
		{
			item.Child = -1;							// zrušení potomků
		}
	
// změna reference na zdrojové okno, je-li kopie mezi okny
		if (bufID != m_BufID)
		{
			item.RefBlok = bufID;						// blok s deklarací
			item.RefIndex = src;						// položka s deklarací
			item.DatBlok = -1;							// nejsou data
			item.DatIndex = -1;							// nejsou data

// oprava reference při přetažení z okna tříd
			if (bufID == BufClsID)
			{
				if (item.Func == IDF_GROUP)
				{
					item.RefBlok = BufStrID;
					item.RefIndex = GroupIndex;
				}
				else
				{
					while (BufCls[item.RefIndex].Parent >= 0)
					{
						item.RefIndex = BufCls[item.RefIndex].Parent;
					}
				}
			}
			else

// implicitní jméno a ikona (ne při kopii z okna tříd)
			{
				if ((m_BufID == BufClsID) && (item.Func != IDF_GROUP))
				{
					if (item.Name >= 0)
					{
						item.Name = Text.Dup(buf->GetText(src));
					}

					if ((item.Icon >= 0) && (item.Func != IDF_ICON))
					{
						item.Icon = Icon.Dup(item.Icon);
					}
					else
					{
						item.Icon = -1;
					}

					item.RefBlok = BufIntID;
					item.RefIndex = item.Func - IDF;
				}
				else
				{
					item.Name = -1;								// implicitní jméno
					item.Icon = -1;								// implicitní ikona
				}
			}
		}

// při kopii z okna struktur rozvinutí položky
		if ((bufID == BufStrID) && (item.Child >= 0))
		{
			item.Param |= PR_EXP;
		}

// při kopii z okna tříd zrušení zámku a vypnutí
		if (bufID == BufClsID)
		{
			item.Param &= ~(PR_LOCK | PR_OFF);

			if (m_BufID != BufClsID)
			{
				item.Param &= ~PR_ALTEXT;
			}
		}

// u první položky zrušení zámku
		if (src == first)
		{
			item.Param &= ~PR_LOCK;
		}

// zrušení interního příznaku a příznaku nové položky a příznaku alternativní knihovny
		item.Param &= ~(PR_INTERN | PR_NEW | PR_CD | PR_CD2);

// korekce parametrů indexu seznamu a sprajtu
		if ((item.Func == IDF_LIST_INDEX) ||
			(item.Func == IDF_LIST_AUTO) ||
			(item.Func == IDF_LIST_SIZE) ||
			(item.Func == IDF_SPRITE_X) ||
			(item.Func == IDF_SPRITE_Y) ||
			(item.Func == IDF_SPRITE_SMER) ||
			(item.Func == IDF_SPRITE_MOVE) ||
			(item.Func == IDF_SPRITE_VISIBLE) ||
			(item.Func == IDF_SPRITE_FAZE) ||
			(item.Func == IDF_SPRITE_STATUS))
		{
			item.Param &= ~PR_NOMOVE;

			if (m_BufID != BufEdiID)
			{
				item.Param |= PR_INTERN;
			}
		}

// základní ikona se mění na běžnou ikonu
		if (item.Func == IDF_NONE)
		{
			item.Func = IDF_ICON;
		}

// ----------- duplikace dat položky

// zrušení textu textové a číselné konstanty a komentáře při kopii z okna struktur
		if (((item.Func == IDF_TEXT_CONST) || 
			(item.Func == IDF_NUM) || 
			(item.Func == IDF_COMMENT)) && 
			(bufID == BufStrID))
		{
			item.Name = Text.New();
		}
		else

// duplikace textu, je-li platný (ne pro textovou konstantu)
		{
			if (item.Name >= 0)
			{
				item.Name = Text.Dup(item.Name);
			}
		}

// duplikace dat obrázku při kopii uvnitř/do okna objektů
		if ((item.Func == IDF_ICON) && ((m_BufID == BufObjID) || (m_BufID == BufLocID)))
		{
			if ((bufID == BufClsID) && (item.Icon >= 0))
			{
				item.DatIndex = item.Icon;
			}

			item.DatBlok = BufIcoID;			// data v bufferu obrázků
			if (item.DatIndex < 0)				// je nějaký obrázek přidělen?
			{
				item.DatIndex = 0;				// implicitní ikona 0 (podklad)
				item.Param |= PR_NEW;			// příznak nové položky
			}
			item.DatIndex = Icon.Dup(item.DatIndex); // duplikace obrázku
			item.Icon = item.DatIndex;			// bude to současně ikona
		}

// duplikace ikony barevné konstanty
		if (item.Func == IDF_COLOR)
		{
			item.Icon = Icon.Dup(buf->GetIcon(src));
		}
		else

// duplikace ikony při kopii uvnitř okna (ne pro barevnou konstantu ani pro předmět)
		{
			if ((item.Icon >= 0) && ((item.Func != IDF_ICON) || (m_BufID == BufClsID)))
			{
				item.Icon = Icon.Dup(item.Icon);
			}
		}

// zajištění dat při kopii z knihovny tříd (ne pro ikonu - je již načtena)
		if ((bufID == BufClsID) && 
			(item.Func != IDF_GROUP) &&
			(item.Func != IDF_ICON) &&
			(item.Name >= 0) &&
			(item.DatIndex < 0) &&
			(BufCls[src].Parent >= 0))
		{
			ProgLib::InitPath(BufCls[src].Parent);
			CText name;

			name = ProgLib::m_Path + (Text.Get(item.Name)).MultiText(JAZYK000);

			item.DatBlok = -1;

			switch(ProgLib::m_Func)
			{
			case IDF_FNC:
				break;

			case IDF_REAL:
				{
					double num;
					if (Real.LoadFile(num, name))
					{
						item.DatBlok = BufNumID;
						item.DatIndex = Real.Add(num);
					}
				}
				break;

			case IDF_TEXT:
				{
					CText txt;
					if (txt.LoadFile(name))
					{
						item.DatBlok = BufTxtID;
						item.DatIndex = Text.AddTxt(txt);
					}
				}
				break;

			case IDF_BOOL:
				{
					BOOL log;
					if (Bool.LoadFile(log, name))
					{
						item.DatBlok = BufLogID;
						item.DatIndex = Bool.Add(log);
					}
				}
				break;

			case IDF_MAP:
				{
					CMap map;
					if (map.LoadFile(name))
					{
						item.DatBlok = BufMapID;
						item.DatIndex = Map.Add(map);
						Map[item.DatIndex].RefInc();
					}
				}
				break;

			case IDF_PIC:
				{
					CPicture pic;
					if (pic.LoadFile(name, TRUE))
					{
						item.DatBlok = BufPicID;
						item.DatIndex = Picture.Add(pic);
					}
				}
				break;

			case IDF_SPRITE:
				{
					CSprite sprite;
					if (sprite.LoadFile(name))
					{
						item.DatBlok = BufSprID;
						item.DatIndex = Sprite.Add(sprite);
					}
				}
				break;

			case IDF_SND:
				{
					CSound sound;
					if (sound.LoadFile(name))
					{
						item.DatBlok = BufSndID;
						item.DatIndex = Sound.Add(sound);
					}
				}
				break;

			case IDF_MUS:
				{
					CMusic music;
					if (music.LoadFile(name))
					{
						item.DatBlok = BufMusID;
						item.DatIndex = Music.Add(music);
					}
				}
				break;
			}
		}
		else

// duplikace dat (pokud jsou) - včetně obsahu funkce
		{
			CopyDupData(&item);
		}

// jméno souboru při přetažení z okna tříd se může zrušit
		if ((item.Name >= 0) && (m_BufID != BufClsID))
		{
			Text[item.Name].MultiText(JAZYK000, EmptyText);
		}

// zajištění nových dat pro položku - včetně obsahu funkce
		CopyNewData(&item);

// -------------- zajištění jedinečnosti jména

		if (((m_BufID == BufObjID) || (m_BufID == BufLocID)) &&
			TestObj(item.Func))
		{
			CMultiText txt;

			if (item.Name >= 0)
			{
				txt = Text.Get(item.Name);
			}
			else
			{
				if (item.RefBlok >= 0)
				{
					CText txt0;
					int texti = BufProg[item.RefBlok].GetText(item.RefIndex);
					txt0 = Text.GetTxt(texti);
					txt.MultiText(txt0);

					// implicitní jméno parametrů sprajtu (při kopii z okna tříd)
					switch (item.Func)
					{
					case IDF_SPRITE_X:
					case IDF_SPRITE_Y:
					case IDF_SPRITE_SMER:
					case IDF_SPRITE_VISIBLE:
					case IDF_SPRITE_MOVE:
					case IDF_SPRITE_FAZE:
					case IDF_SPRITE_STATUS:
						if (IsValid(parent))
						{
							int pari = GetText(parent);
							if ((pari >= 0) && (texti >= 0))
							{
								for (int lan = 0; lan < JAZYKNUM; lan++)
								{
									txt0 = Text[pari].MultiText(lan);
									if (txt0.IsNotEmpty())
									{
										txt.MultiText(lan, txt0 + _T(' ') + Text[texti].MultiText(lan));
									}
								}
								item.Name = Text.Add(txt);
							}
						}
					}
				}
			}

			int nn = 1;
			int reffrst = -1;
			int refnext = -1;

			if (m_BufID == BufLocID)
			{
				reffrst = parent;

				if (IsValid(reffrst))
				{
					while (At(reffrst).Parent >= 0)
					{
						reffrst = At(reffrst).Parent;
					}
					refnext = At(reffrst).Next;
				}

			}

			while ((BufLoc.FindObj(txt, -1, reffrst, refnext) >= 0) || 
				   (BufObj.FindObj(txt, -1, -1, -1) >= 0))
			{
				nn++;
				txt.SetNumObj(nn);
			}

			if (nn > 1)
			{
				if (item.Name < 0)
				{
					item.Name = Text.Add(txt);
				}
				else
				{
					Text.Set(item.Name, txt);
				}
			}
		}

// -------------- uložení položky

// kopie položky do cílového bufferu (pozor - Insert může pohnout zdrojovou i cílovou adresou)
		dst = Insert(&item, parent, pos);
		if (src == first) result = dst;			// výsledný index
		pos = -2;								// příště již budou položky na konec

// -------------- vytvoření parametrů sprajtu

// při kopii sprajtu z okna tříd přidání parametrů
		if ((item.Func == IDF_SPRITE) && (item.Child == -1) && 
			((m_BufID == BufObjID) || (m_BufID == BufLocID)))
		{
			Copy(dst, -2, BufIntID, IDF_SPRITE_X - IDF);
			Copy(dst, -2, BufIntID, IDF_SPRITE_Y - IDF);
			Copy(dst, -2, BufIntID, IDF_SPRITE_SMER - IDF);
			Copy(dst, -2, BufIntID, IDF_SPRITE_VISIBLE - IDF);
			Copy(dst, -2, BufIntID, IDF_SPRITE_MOVE - IDF);
			Copy(dst, -2, BufIntID, IDF_SPRITE_FAZE - IDF);
			Copy(dst, -2, BufIntID, IDF_SPRITE_STATUS - IDF);
		}

// -------------- při kopii globální funkce kopie též vstupních parametrů

// kontrola, zda se kopíruje globální funkce z objektů do editoru
		if ((item.Func == IDF_FNC) &&
			(m_BufID == BufEdiID) && 
			(bufID == BufObjID) &&
			(BufObj[src].DatBlok == BufLocID) &&
			(BufObj[src].DatIndex >= 0))
		{

// nalezení definice vstupních proměnných
			int inx = BufObj[src].DatIndex;
			PROGITEM* itm;
			do 
			{
				inx = BufLoc.NextItem(inx);
				itm = &BufLoc[inx];
			} while ((itm->Func != IDF_PAR) && (itm->Parent >= 0));
			inx = itm->Child;

// kopie vstupních proměnných
			if (itm->Func == IDF_PAR)
			{
				while (inx >= 0)
				{
					int ii = BufEdi.Copy(dst, -2, BufLocID, inx);
					if (BufEdi.IsValid(ii)) BufEdi[ii].Param |= PR_NOMOVE;
					itm = &BufLoc[inx];
					inx = itm->Next;
				}
			}
		}

// --------------- vytvoření vstupního parametru (pozor na kopii celé lokální definice)

// kontrola, zda je vytvořen vstupní parametr 
		if ((m_BufID == BufLocID) &&
			(parent >= 0) &&
			(m_Data[parent].Func == IDF_PAR))
		{

// příprava indexu globální funkce
			int reffnc = BufObj.SrcDat(BufLocID, m_Data[parent].Parent);
			if (reffnc >= 0)
			{

// nalezení funkce v editačním bufferu
				CBufProg* bufedi = &BufEdi;
				int inxedi = bufedi->Max() - 1;					// index poslední položky
				PROGITEM* itemedi = bufedi->Data() + inxedi;	// adresa poslední položky
				for (; inxedi >= 0; inxedi--)
				{

// je to použití funkce?
					if (bufedi->m_Valid[inxedi] &&
						(itemedi->RefIndex == reffnc) &&
						(itemedi->RefBlok == BufObjID) &&
						(itemedi->Func == IDF_FNC) &&
						((itemedi->DstMask & PR_ALL) == 0))
					{

// kopie parametru funkce
						int ii = bufedi->Copy(inxedi, -2, BufLocID, dst);
						ASSERT(ii >= 0);

// zákaz přesunu parametru
						if (ii >= 0)
						{
							bufedi->At(ii).Param |= PR_NOMOVE;
						}
					}
					itemedi--;
				}
			}
		}

// ---------- kopie definice programu

// při kopii funkce kopie definice programu
		if (((item.DatBlok == BufLocID) || (item.DatBlok == BufEdiID)) &&
			(m_BufID == bufID) && (item.DatIndex == m_Data[dst].DatIndex))
		{
			int srcroot = item.DatIndex;
			ASSERT(srcroot >= 0);
			int dstroot = BufProg[item.DatBlok].Copy(-1, -2, item.DatBlok, srcroot);
			m_Data[dst].DatIndex = dstroot;
			ASSERT(dstroot >= 0);

// při kopii globální funkce korekce referencí na lokální proměnné
			if ((item.DatBlok == BufLocID) && (m_BufID == BufObjID))
			{

// příprava ukazatelů pro vyhledávání definic funkcí
				CBufProg* bufloc = &BufLoc;		// adresa bufferu lokálních objektů
				int locinx = dstroot;			// index v lokální definici při vyhledávání funkcí
				PROGITEM* locitem;				// položka v lokální definici při vyhledávání
				locitem = &bufloc->At(locinx);	// adresa položky

// příprava ukazatelů pro vyhledávání lokálních proměnných v editoru
				CBufProg* bufedi = &BufEdi;		// adresa bufferu editoru
				int ediinx;						// index v bufferu editoru
				PROGITEM* ediitem;				// adresa v bufferu editoru

// příprava ukazatelů pro reindexaci odkazu na lokální proměnnou
				int refinx;						// referenční index hledané položky
				int num;						// relativní pozice položky v lokální definici
				int inx;						// ukazatel indexu v lokální definici
				PROGITEM* itm;					// položka v lokální definici

// cyklus vyhledávání definic funkcí v cílové lokální definici (pozor - obsloužit i root)
				do 
				{

// test, zda je to definice funkce
					ediinx = locitem->DatIndex;
					if ((locitem->DatBlok == BufEdiID) && 
						(bufedi->IsValid(ediinx)))
					{
						ediitem = &bufedi->At(ediinx);		// adresa položky

// cyklus vyhledávání lokálních proměnných v editoru
						do 
						{

// test, zda je to odkaz na lokální položku a zda to není parametr funkce
							refinx = ediitem->RefIndex;
							if ((ediitem->RefBlok == BufLocID) &&
								bufloc->IsValid(refinx) &&
								(ediitem->Parent >= 0) &&
								(((bufedi->At(ediitem->Parent).DstMask & PR_ALL) != 0) || (bufedi->At(ediitem->Parent).Func != IDF_FNC)))
							{

// zjištění relativní pozice položky ve zdrojové lokální definici
								num = 0;
								inx = srcroot;
								itm = &bufloc->At(inx);

								while (inx != refinx)
								{
									num++;
									inx = bufloc->NextItem(inx);
									itm = &bufloc->At(inx);
									if (itm->Parent < 0) break;
								}
				
// kontrola nalezení položky
								if (itm->Parent >= 0)
								{

// nalezení odpovídající zkopírované položky
									inx = dstroot;
									for (; num > 0; num--)
									{
										inx = bufloc->NextItem(inx);
									}

// přesměrování ukazatele
									itm->Refer--;
									ediitem->RefIndex = inx;
									bufloc->At(inx).Refer++;
								}
							}

// posun ukazatele v editoru na další položku
							ediinx = bufedi->NextItem(ediinx);	// index další položky
							ASSERT(ediinx >= 0);
							ediitem = &bufedi->At(ediinx);		// adresa položky
						} while (ediitem->Parent >= 0);			// dokud není další definice
					}

// posun ukazatele v lokální definici na další položku
					locinx = bufloc->NextItem(locinx);	// index další položky
					ASSERT(locinx >= 0);
					locitem = &bufloc->At(locinx);		// adresa položky
				} while (locitem->Parent >= 0);			// dokud není další definice
			}
		}

// ---------- další položka

// příprava indexu další položky
		itemdst = &At(dst);
		itemsrc = &buf->At(src);
		if ((src != first) || (item.Child >= 0))
		{
			src = item.Child;				// vnoření na potomka
			if (src < 0)					// není žádný potomek?
			{
				src = itemsrc->Next;		// pokračování další položkou

				while ((src < 0) && (itemsrc->Parent >= 0))
				{
					src = itemsrc->Parent;	// návrat zdroje k rodiči
					dst = parent;			// návrat cíle k rodiči
					itemdst = &At(dst);		// adresa cílové položky
					parent = itemdst->Parent; // nový rodič
					if (src == first) break; // je celá větev
					itemsrc = &buf->At(src); // adresa rodiče
					src = itemsrc->Next;	// další položka za rodičem
				}
			}
			else
			{
				parent = dst;				// jinak tato položka bude rodičem
			}

// při kopii z okna tříd přeskočení skupin
			if (bufID == BufClsID)
			{
				while ((src >= 0) && (src != first) && (buf->At(src).Func == IDF_GROUP))
				{
					src = buf->At(src).Next;
				}

				if (src < 0) src = first;
			}
		}

// až po výchozí položku
	} while (src != first);

// aktualizace stavové ikony rodiče
	AktStateImage(newparent);

// vypnutí čekacího kurzoru
	EndWaitCursor();

	return result;
}


////////////////////////////////////////////////////////////////////
// duplikace dat u kopírované položky (ne pro ikonu a ne pro funkci)

void CBufProg::CopyDupData(PROGITEM* item)
{
// duplikace dat
	switch (item->DatBlok)
	{
	case BufTxtID:	item->DatIndex = Text.Dup(item->DatIndex);
					break;

	case BufNumID:	item->DatIndex = Real.Dup(item->DatIndex);
					break;

	case BufLogID:	item->DatIndex = Bool.Dup(item->DatIndex);
					break;

	case BufMapID:	item->DatIndex = Map.Dup(item->DatIndex);
					Map[item->DatIndex].RefInc();
					break;

	case BufPicID:	item->DatIndex = Picture.Dup(item->DatIndex);
					break;

	case BufSprID:	item->DatIndex = Sprite.Dup(item->DatIndex);
					break;

	case BufSndID:	item->DatIndex = Sound.Dup(item->DatIndex);
					break;

	case BufMusID:	item->DatIndex = Music.Dup(item->DatIndex);
					break;
	}
}


////////////////////////////////////////////////////////////////////
// zajištění nových dat u kopírované položky (je voláno i z Clipboard::Paste)

void CBufProg::CopyNewData(PROGITEM* item)
{
// test, zda data nejsou vytvořena a zda je to buffer objektů
	if (((item->DatBlok >= 0) && (item->DatIndex >= 0)) ||
		((m_BufID != BufObjID) && (m_BufID != BufLocID)))
		return;

// vytvoření nových dat pro proměnné
	switch (item->Func)
	{

// číselná proměnná
	case IDF_LIST_AUTO:
	case IDF_LIST_INDEX:
	case IDF_SPRITE_X:
	case IDF_SPRITE_Y:
	case IDF_SPRITE_SMER:
	case IDF_SPRITE_FAZE:
	case IDF_SPRITE_STATUS:
	case IDF_REAL:		item->DatBlok = BufNumID;
						item->DatIndex = Real.New();
						item->Param |= PR_NEW;
						break;

// textová proměnná
	case IDF_TEXT:		item->DatBlok = BufTxtID;
						item->DatIndex = Text.New();
						item->Param |= PR_NEW;
						break;

// logická proměnná
	case IDF_SPRITE_VISIBLE:
	case IDF_SPRITE_MOVE:
	case IDF_BOOL:		item->DatBlok = BufLogID;
						item->DatIndex = Bool.New();
						item->Param |= PR_NEW;
						break;

// proměnná plochy
	case IDF_MAP:		item->DatBlok = BufMapID;
						ASSERT(Map.IsValid(0));
						item->DatIndex = Map.New(Map[0].Width(), Map[0].Height());
						Map[item->DatIndex].RefInc();
						item->Param |= PR_NEW;
						break;

// proměnná obrázku
	case IDF_PIC:		item->DatBlok = BufPicID;
						ASSERT(Map.IsValid(0));
						item->DatIndex = Picture.New(Map[0].Width()*ICONWIDTH, Map[0].Height()*ICONHEIGHT);
						item->Param |= PR_NEW;
						break;

// proměnná sprajtu
	case IDF_SPRITE:	item->DatBlok = BufSprID;
						item->DatIndex = Sprite.New();
						item->Param |= PR_NEW;
						break;

// proměnná zvuku
	case IDF_SND:		item->DatBlok = BufSndID;
						item->DatIndex = Sound.New();
						item->Param |= PR_NEW;
						break;

// proměnná hudby
	case IDF_MUS:		item->DatBlok = BufMusID;
						item->DatIndex = Music.New();
						item->Param |= PR_NEW;
						break;

// funkce
	case IDF_FNC:
		{
			item->Param |= PR_NEW;

// vytvoření definice v editačním bufferu
			PROGITEM item2;								// pracovní položka
			MemFill(&item2, SIZEOFPROGITEM, -1);		// implicitní hodnoty položek
			item2.Func = IDF_FNC;						// identifikace funkce
			item2.SrcMask = 0;							// maska zdrojových vlastností
			item2.DstMask = PR_COMMAND;					// maska cílových vlastností
			item2.Param = PR_EXP | PR_NOMOVE;			// parametry
			item2.RefBlok = BufIntID;					// reference na interní buffer
			item2.RefIndex = IDF_FNC - IDF;				// referenční index
			item2.DatIndex = BufEdi.Insert(&item2, -1, -2);	// vložení položky do bufferu

// inicializace odkazu na definici z lokálního bufferu
			if (m_BufID == BufLocID)
			{
				item->DatBlok = BufEdiID;				// index bufferu
				item->DatIndex = item2.DatIndex;		// index položky definice
			}
			else

// vytvoření deklarace v lokálním bufferu (je-li kopie do globálního bufferu)
			{
				item2.DstMask = PR_ALLDATA;				// maska cílových vlastností
				item2.DatBlok = BufEdiID;				// odkaz na editační buffer
				item->DatIndex = BufLoc.Insert(&item2, -1, -2); // vložení položky do bufferu
				item->DatBlok = BufLocID;				// lokální buffer

// vložení záhlaví pro vstupní parametry
				item2.Func = IDF_PAR;					// vstupní parametry
				item2.DstMask = PR_ALLDATA;				// maska cílových parametrů
				item2.Param = PR_NOMOVE;				// parametry
				item2.DatBlok = -1;						// není blok s daty
				item2.DatIndex = -1;					// nejsou data
				item2.RefIndex = IDF_PAR - IDF;			// index v interním bufferu
				BufLoc.Insert(&item2, item->DatIndex, -2);	// vložení položky do bufferu

// vložení záhlaví pro výstupní parametr
				item2.Func = IDF_OUT;					// výstupní parametr
				item2.Param = PR_NOMOVE | PR_ONE;		// parametry
				item2.RefIndex = IDF_OUT - IDF;			// index v interním bufferu
				BufLoc.Insert(&item2, item->DatIndex, -2);	// vložení položky do bufferu
			}
		}
	}
}
