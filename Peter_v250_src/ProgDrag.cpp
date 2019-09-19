
#include "Main.h"

/***************************************************************************\
*																			*
*							Obsluha tažení ikon								*
*																			*
\***************************************************************************/

// parametry tažení
BOOL		Draging = FALSE;			// příznak probíhajícího tažení
int			DragIconWidth;				// šířka tažené ikony
int			DragIconHeight;				// výška tažené ikony
int			DragHotSpotX;				// bod uchycení prvku X
int			DragHotSpotY;				// bod uchycení prvku Y
int			DragTestSpotX;				// testovací bod X (střed ikony)
int			DragTestSpotY;				// testovací bod Y (střed ikony)
HIMAGELIST	DragImageList;				// seznam obrázků pro tažení
BOOL		DragRight;					// příznak pravého tlačítka
BOOL		DragLeftInt;				// příznak levého tlačítka + interní prvek
BOOL		DragCopy;					// příznak kopírování položky
BOOL		DragDelete;					// příznak rušení položky

// zdroj tažení
HWND		DragSrcTree;				// strom zdroje tažení
CBufProg*	DragSrcBuf;					// buffer zdroje tažení
int			DragSrcBufID;				// ID bufferu zdroje tažení
int			DragSrcParent;				// index rodiče zdroje tažení (-1=root)
HTREEITEM	DragSrcHItem;				// handle zdroje tažení
int			DragSrcIndex;				// index zdroje tažení
PROGITEM*	DragSrcItem;				// položka zdroje taźení
int			DragSrcFunc;				// funkce zdroje tažení
int			DragSrcSrcMask;				// maska zdrojových vlastností zdroje tažení
int			DragSrcDstMask;				// maska cílových vlastností zdroje tažení
int			DragSrcParam;				// parametry položky zdroje tažení

// cíl tažení
HWND		DragDstTree;				// strom cíle tažení
CBufProg*	DragDstBuf;					// buffer cíle tažení
int			DragDstBufID;				// ID bufferu cíle tažení
int			DragDstParent;				// index rodiče cíle tažení (-1=root)
HTREEITEM	DragDstHParent;				// handle rodiče cíle tažení
int			DragDstPoz;					// index pozice cíle tažení (-1=začátek, -2=konec)
HTREEITEM	DragDstHPoz;				// handle pozice cíle tažení (NULL=začátek nebo konec)
int			DragDstFunc;				// funkce rodiče cíle tažení
int			DragDstSrcMask;				// maska zdrojových vlastností rodiče cíle tažení
int			DragDstDstMask;				// maska cílových vlastností rodiče cíle tažení
int			DragDstParam;				// parametry položky rodiče cíle tažení

/////////////////////////////////////////////////////////////////////////////
// zahájení taženi

void ProgOnBeginDrag(HWND hWnd, TV_ITEM* tvi, int x, int y, BOOL right)
{
// kontrola, zda neprobíhá tažení
	ASSERT(!Draging);
	if (Draging) return;

// kontrola, zda je režim editace
	ASSERT(ProgMode);
	if (!ProgMode) return;

// úschova příznaku pravého tlačítka
	DragRight = right;

// příprava zdrojového okna a zdrojového bufferu
	DragSrcTree = hWnd;
	ASSERT(::IsWindow(hWnd));
	DragSrcBufID = BufProgIDFromHandle(DragSrcTree);
	ASSERT(DragSrcBufID >= 0);
	if (DragSrcBufID < 0) return;
	DragSrcBuf = &BufProg[DragSrcBufID];

// úschova rozměrů ikon
	DragIconWidth = DragSrcBuf->IconWidth();
	DragIconHeight = DragSrcBuf->IconHeight();

// nalezení položky
	DragSrcHItem = tvi->hItem;
	DragSrcIndex = DragSrcBuf->Find(DragSrcHItem);
	ASSERT(DragSrcIndex >= 0);
	if (DragSrcIndex < 0) return;
	ASSERT(DragSrcBuf->IsValid(DragSrcIndex));
	DragSrcItem = &((*DragSrcBuf)[DragSrcIndex]);
	DragSrcParent = DragSrcItem->Parent;

// úschova vlastností zdrojové položky
	DragSrcFunc = DragSrcItem->Func;
	DragSrcSrcMask = DragSrcItem->SrcMask;
	DragSrcDstMask = DragSrcItem->DstMask;
	DragSrcParam = DragSrcItem->Param;

// parametry pro funkci
	if (DragSrcFunc == IDF_FNC)
	{
		DragSrcSrcMask = PR_COMMAND;

		int inx = DragSrcIndex;
		if (DragSrcBufID == BufEdiID) inx = DragSrcItem->RefIndex;

		if ((((DragSrcBufID == BufEdiID) && (DragSrcItem->RefBlok == BufObjID)) || (DragSrcBufID == BufObjID)) &&
		    BufObj.IsValid(inx) && (BufObj[inx].DatBlok == BufLocID))
		{
			inx = BufObj[inx].DatIndex;
			if (BufLoc.IsValid(inx) && (BufLoc[inx].DatBlok == BufEdiID))
			{
				inx = BufLoc[inx].Child;
				while (BufLoc.IsValid(inx))
				{
					if (BufLoc[inx].Func == IDF_OUT)
					{
						inx = BufLoc[inx].Child;
						if (BufLoc.IsValid(inx) && (BufLoc[inx].DatBlok >= 0))
						{
							DragSrcSrcMask = BufLoc[inx].SrcMask | PR_COMMAND;
						}
						break;
					}
					inx = BufLoc[inx].Next;
				}
			}
		}
	}

// kontrola, jestli to není zakázaný prvek
	if (DragSrcParam & PR_NOMOVE) return;

// načtení souřadnic textu prvku
	RECT	rc;							// souřadnice taźeného prvku
	*(HTREEITEM*)(&rc) = DragSrcHItem;
	BOOL result = ::SendMessage(DragSrcTree, TVM_GETITEMRECT, TRUE, (LPARAM)&rc);
	ASSERT(result);
	if (!result) return;

// výpočet bodu uchycení taženého prvku
	DragHotSpotX = x - (rc.left - 3 - DragIconWidth);
	DragHotSpotY = y - rc.top;

// výpočet testovacího bodu
	DragTestSpotX = DragIconWidth/4*3;
	DragTestSpotY = - DragIconHeight/4;

// vygenerování tažené ikony
//	DragImageList = (HIMAGELIST)::SendMessage(DragSrcTree, TVM_CREATEDRAGIMAGE, 0, (LPARAM)DragSrcHItem);
//	if (DragImageList == NULL) return;

	CPicture dragicon;
	if (!dragicon.New(DragIconWidth, DragIconHeight)) return;
	int iconinx = DragSrcBuf->GetIcon(DragSrcIndex);
	ASSERT(iconinx >= 0);
	CIcon icon = Icon.Get(iconinx);
	if (!icon.DeComp()) return;

	if (icon.Size() == (DragIconWidth*DragIconHeight))
	{
		if (!dragicon.CopyData(icon.DataData())) return;
	}
	else
	{
		if ((icon.Size()*9/16) == (DragIconWidth*DragIconHeight))
		{
			CBufIcon iconlist;
			iconlist.IconSize(ICONWIDTH, ICONHEIGHT);
			if (iconlist.Add(icon) != 0) return;
			BYTE* bf = iconlist.GenerBitmap(SI_MEDIUM);
			if (bf == NULL) return;
			if (!dragicon.CopyData(bf)) return;
			MemFree(bf);
		}
		else
		{
			if ((icon.Size()/4) != (DragIconWidth*DragIconHeight)) return;

			CBufIcon iconlist;
			iconlist.IconSize(ICONWIDTH, ICONHEIGHT);
			if (iconlist.Add(icon) != 0) return;
			BYTE* bf = iconlist.GenerBitmap(SI_SMALL);
			if (bf == NULL) return;
			if (!dragicon.CopyData(bf)) return;
			MemFree(bf);
		}
	}

	int textinx = DragSrcBuf->GetText(DragSrcIndex);
	ASSERT(textinx >= 0);
	DragImageList = dragicon.GenerDrag(Text.GetTxt(textinx));
	ASSERT(DragImageList != NULL);
	if (DragImageList == NULL) return;

// nastavení příznaku zahájení tažení (odteď se již nesmí přerušit)
	Draging = TRUE;

// přednastavení neplatného cíle
	DragDstTree = NULL;					// neplatný cílový strom	
	DragDstParent = -1;					// neplatná cílová položka

// výběr cílového prvku
	VERIFY(::SendMessage(DragSrcTree, TVM_SELECTITEM, TVGN_DROPHILITE, (LPARAM)DragSrcHItem));

// zapnutí zobrazení obrázku tažení (bez uzamykání okna)
	::ImageList_DragShowNolock(TRUE);

// při tažení skupiny z okna tříd se musí nejdříve zjistit, zda obsahuje potomky
	if ((DragSrcBufID == BufClsID) &&
		(DragSrcParent >= 0) && 
		(DragSrcFunc == IDF_GROUP) &&
		(DragSrcItem->Child < 0))
	{
		ProgLib::Load(DragSrcIndex);
		DragSrcItem = &((*DragSrcBuf)[DragSrcIndex]);
	}

// korekce jen kopírovaných prvků
	DragLeftInt = FALSE;			// není levé tlačítko + interní prvek
	if (DragSrcParam & (PR_INTERN | PR_LOCK | PR_LOCK_DEP))
	{
		if (!DragRight)				// bylo levé tlačítko?
		{
			DragLeftInt = TRUE;		// je levé tlačítko + interní prvek
		}
		DragRight = TRUE;			// bude kopírování jako pravé tlačítko
	}

// příprava příznaku kopírování
	DragCopy = TRUE;				// bude kopírování
	DragDelete = !DragRight;		// pro levé tlačítko zrušení staré položky

// zapnutí kurzoru pro tažení
	if (DragRight)
	{
		CurAkt = CurCopy;
	}
	else
	{
		CurAkt = CurMove;
	}
	::SetCursor(CurAkt);

// vytvoření nového obrázku tažení
	::ImageList_SetDragCursorImage(DragImageList, 0, 0, 0);

// zahájení operace tažení
	::ImageList_BeginDrag(DragImageList, 0, 0, 0);

// zrušení výběru cíle
	::SendMessage(DragSrcTree, TVM_SELECTITEM, TVGN_DROPHILITE, (LPARAM)NULL);

// zahájení operace tažení, zákaz aktualizace okna
	::ImageList_DragEnter(NULL, MouseScreen.x - DragHotSpotX, MouseScreen.y - DragHotSpotY);

// přetažení obrázku na aktuální pozici myši
	::ImageList_DragMove(MouseScreen.x - DragHotSpotX, MouseScreen.y - DragHotSpotY);

// zahájení zachytávání myši
	::SetCapture(MainFrame);
};


/////////////////////////////////////////////////////////////////////////////
// nalezení cílového okna, nad kterým je taženo (vrací FALSE=nenalezeno)

BOOL SrcDropWin(int x, int y)
{
// přednastavení neplatného cíle
	DragDstTree = NULL;					// neplatný cílový strom	
	DragDstParent = -1;					// neplatná cílová položka
	
// nalezení okna, nad kterým je taženo
	POINT pt;
	pt.x = x;
	pt.y = y;
	HWND wnd = ::ChildWindowFromPointEx(MainFrame, pt, CWP_SKIPINVISIBLE | CWP_SKIPDISABLED);
	if (wnd == NULL) return FALSE;

// při tažení interního prvku levým tlačítkem zákaz stejného okna
	if (DragLeftInt)
	{
		if (wnd == DragSrcTree)
		{
			return FALSE;
		}
	}

// okno editace
	if (wnd == TreeEdi)
	{
		DragDstTree = TreeEdi;
		DragDstBuf = &BufEdi;
		DragDstBufID = BufEdiID;
		return TRUE;
	}

// okno globálních objektů
	if (wnd == TreeObj)
	{
		DragDstTree = TreeObj;
		DragDstBuf = &BufObj;
		DragDstBufID = BufObjID;
		return TRUE;
	}

// okno lokálních objektů
	if (wnd == TreeLoc)
	{
		DragDstTree = TreeLoc;
		DragDstBuf = &BufLoc;
		DragDstBufID = BufLocID;
		return TRUE;
	}

// okno struktur
	if (wnd == TreeStr)
	{
		DragDstTree = TreeStr;
		DragDstBuf = &BufStr;
		DragDstBufID = BufStrID;
		return TRUE;
	}

// okno tříd
	if (wnd == TreeCls)
	{
		DragDstTree = TreeCls;
		DragDstBuf = &BufCls;
		DragDstBufID = BufClsID;
		return TRUE;
	}

// okno editoru plochy nebo sprajtu
	if ((wnd == MainFrame) &&
		((DragSrcBufID == BufObjID) || (DragSrcBufID == BufLocID)) &&
		(x >= EditX) &&
		(x < EditX + EditWidth) &&
		(y >= EditY) &&
		(y < EditY + EditHeight))
	{
		switch (EditMode)
		{
		case BufSprID:
			if ((DragSrcSrcMask & PR_PIC) != 0)
			{
				DragDstBufID = EditMode;
				return TRUE;
			}
			break;

		case BufMapID:
			if ((DragSrcSrcMask & PR_ICON) != 0)
			{
				DragDstBufID = EditMode;
				return TRUE;
			}
			break;
		}
	}

// okno nenalezeno
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// nalezení položky pod kurzorem ve stromu (NULL=konec ROOT, -1=začátek ROOT)

HTREEITEM ItemTreePos(int x, int y, int height)
{
// lokální proměnné
	HTREEITEM item;						// nalezená položka
	TV_HITTESTINFO hti;					// struktura pro nalezení položky

// nalezení položky na dané souřadnici
	hti.pt.x = x;
	hti.pt.y = y;
	item = (HTREEITEM) ::SendMessage(DragDstTree, TVM_HITTEST, 0, (LPARAM)&hti);
	if (item != NULL) return item;

// nový pokus, je-li kurzor příliš vlevo či příliš vpravo
	if (hti.flags & (TVHT_TOLEFT | TVHT_TORIGHT))
	{
		x = 20;
		hti.pt.x = 20;					// asi tak nějak na začátek řádku
		hti.pt.y = y;
		item = (HTREEITEM) ::SendMessage(DragDstTree, TVM_HITTEST, 0, (LPARAM)&hti);
		if (item != NULL) return item;
	}

// je-li kurzor nad horním okrajem, musíme najít předcházející skrytou položku
	if (hti.flags & TVHT_ABOVE)
	{
		item = (HTREEITEM) ::SendMessage(DragDstTree, TVM_GETNEXTITEM, TVGN_FIRSTVISIBLE, 0);
		if (item == NULL) return (HTREEITEM)-1;
		item = (HTREEITEM) ::SendMessage(DragDstTree, TVM_GETNEXTITEM, TVGN_PREVIOUSVISIBLE, (LPARAM)item);
		if (item == NULL) return (HTREEITEM)-1;
		return item;
	}

// je-li kurzor příliš dole, zkusíme ještě jeden pokus o trochu výš
	if (item == NULL)
	{
		y -= height/4*3;
		hti.pt.x = x;
		hti.pt.y = y;
		item = (HTREEITEM) ::SendMessage(DragDstTree, TVM_HITTEST, 0, (LPARAM)&hti);
	}

// návrat výsledku (až už je jakýkoliv)
	return item;
}


/////////////////////////////////////////////////////////////////////////////
// test povolení cíle tažení - operace přidání k rodiči (FALSE=nepovoleno)

BOOL TestDropItem()
{
// do zamknuté položky nelze táhnout
	if (DragDstParam & (PR_LOCK | PR_LOCK_DEP)) return FALSE;

// rozlišení podle cílového bufferu
	switch (DragDstBufID)
	{

//-------------- globální a lokální objekty
	case BufObjID:
	case BufLocID:

// z editoru nelze nikdy táhnout
		if (DragSrcBufID == BufEdiID) return FALSE;

// z druhého okna objektů nelze táhnout
		if (DragSrcBufID == (BufLocID ^ BufObjID ^ DragDstBufID)) return FALSE;

// do velikosti seznamu lze přetáhnout číslice
		if ((DragDstFunc == IDF_LIST_SIZE) &&
			(DragSrcFunc >= IDF_0) &&
			(DragSrcFunc <= IDF_9)) return TRUE;

// komentář lze položit kamkoliv (kromě 1 parametru), do komentáře nelze táhnout nic (kromě další komentář)
		if (DragSrcFunc == IDF_COMMENT)
		{
			return (((DragDstParam & PR_ONE) == 0) && 
					(DragDstFunc != IDF_PAR) &&
					(DragDstFunc != IDF_OUT) &&
					(DragDstFunc != IDF_FNC) &&
					(DragDstFunc != IDF_COMMAND_LINE) &&
					(DragDstFunc != IDF_RETURN_CODE));
		}
		if (DragDstFunc == IDF_COMMENT) return FALSE;

// z okna struktur lze přetáhnout jen skupinu a seznam (ne do seznamu ani parametrů)
		if (DragSrcBufID == BufStrID)
		{
			return (((DragSrcFunc == IDF_GROUP) ||
					(DragSrcFunc == IDF_LIST)) && 
					(DragDstFunc != IDF_PAR) &&
					(DragDstFunc != IDF_OUT) &&
					(DragDstFunc != IDF_FNC) &&
					(DragDstFunc != IDF_LIST) &&
					(DragDstFunc != IDF_COMMAND_LINE) &&
					(DragDstFunc != IDF_RETURN_CODE) &&
					((DragDstParam & PR_ONE) == 0));
		}

// do seznamu lze přetáhnout datové proměnné
		if (DragDstFunc == IDF_LIST)
		{
			switch (DragSrcFunc)
			{
			case IDF_REAL:
			case IDF_BOOL:
			case IDF_TEXT:
			case IDF_ICON:
			case IDF_NONE:
			case IDF_MAP:
			case IDF_PIC:
			case IDF_SND:
			case IDF_MUS:
			case IDF_SPRITE:
				return TRUE;
			}
			return FALSE;
		}

// skupinu, seznam ani funkci nelze umístit do vstupních/výstupních proměnných
		if (((DragSrcFunc == IDF_GROUP) || (DragSrcFunc == IDF_LIST) || (DragSrcFunc == IDF_FNC)) &&
			((DragDstFunc == IDF_PAR) || (DragDstFunc == IDF_OUT))) return FALSE;

// test, zda je povolen pouze jeden parametr (výstupní proměnná)
		if ((DragDstParam & PR_ONE) && (DragDstParent >= 0) &&
			((*DragDstBuf)[DragDstParent].Child >= 0)) return FALSE;

// do skupiny, ROOT (příp. parametrů) lze přetáhnout proměnné, skupinu a seznam
		if ((DragDstFunc == IDF_GROUP) || (DragDstParent == -1) || 
				(DragDstFunc == IDF_PAR) || (DragDstFunc == IDF_OUT))
		{
			switch (DragSrcFunc)
			{
			case IDF_GROUP:
			case IDF_LIST:
			case IDF_FNC:
			case IDF_REAL:
			case IDF_BOOL:
			case IDF_TEXT:
			case IDF_ICON:
			case IDF_NONE:
			case IDF_MAP:
			case IDF_PIC:
			case IDF_SND:
			case IDF_MUS:
			case IDF_SPRITE:
				return TRUE;
			}
		}

		return FALSE;

//-------------- editor

	case BufEdiID:

// ze tříd nelze nikdy táhnout
		if (DragSrcBufID == BufClsID) return FALSE;
		
// komentář lze položit téměř kamkoliv
		if (DragSrcFunc == IDF_COMMENT)
		{
			return (((DragDstParam & PR_ONE) == 0) && 
					(DragDstFunc != IDF_FNC));
		}

// do komentáře lze táhnout cokoliv
		if ((DragDstFunc == IDF_COMMENT)) return TRUE;

// z objektů nelze táhnout skupinu objektů a číslice z velikosti seznamu
		if (((DragSrcBufID == BufObjID) || (DragSrcBufID == BufLocID)) &&
			((DragSrcFunc == IDF_GROUP) || (DragSrcSrcMask & PR_DIGIT))) return FALSE;

// seznam objektů není povolen nikdy
		if (DragSrcFunc == IDF_LIST) return FALSE;

// test, zda má cíl povolen pouze jeden parametr a zda už je obsazen
		if ((DragDstParam & PR_ONE) && (DragDstParent >= 0) &&
			(DragDstBuf->At(DragDstParent).Child >= 0)) return FALSE;

// přetažení větve CASE
		if (DragSrcFunc == IDF_CASE_ON) return (DragDstFunc == IDF_CASE);
		if (DragSrcFunc == IDF_CASE_EQU) return (DragDstFunc == IDF_CASE_ON);

// porovnání masky zdroje a cíle tažení
		if ((DragSrcSrcMask & DragDstDstMask) == 0) return FALSE;

// zákaz přetažení položky s parametrem do parametru
		if ((DragSrcBufID == BufEdiID) &&						// problém jen s přetahováním uvnitř editoru
			(DragSrcFunc != IDF_FNC) &&							// funkce je povolena, může mít vstupní parametry
			((DragDstDstMask & PR_COMMAND) == 0) &&				// cílem nejsou příkazy
			((DragSrcSrcMask & PR_COMMAND) != 0) &&				// zdroj může být jako příkaz
			((DragSrcParam & PR_PARPAR) == 0) &&				// kromě případu, kdy má povoleny parametry vždy
			(DragSrcItem->Child >= 0)) return FALSE;			// položka má nějakého potomka

// zákaz přetažení do položky, která není příkazem
		if ((DragDstParent >= 0) &&								// je nějaký cíl
			((DragDstSrcMask & PR_COMMAND) != 0) &&				// cíl může být jako příkaz
			((DragDstParam & PR_PARPAR) == 0) &&				// kromě případu, kdy má povoleny parametry vždy
			(DragDstBuf->At(DragDstParent).Parent >= 0) &&		// cíl je něčím potomkem
			((DragDstBuf->At(DragDstBuf->At(DragDstParent).Parent).DstMask & PR_ALLDATA) != 0) &&  // rodič cíle přijímá data
			((DragDstBuf->At(DragDstBuf->At(DragDstParent).Parent).DstMask & PR_COMMAND) == 0))		// přitom rodič cíle nepřijímá příkazy (=není jako příkaz)
			return FALSE;

		return TRUE;

//---------- okno tříd
	
	case BufClsID:

// ze struktur lze táhnout jen skupinu
		if (DragSrcBufID == BufStrID)
		{
			if (DragSrcFunc != IDF_GROUP) return FALSE;
			return ((DragDstFunc == IDF_GROUP) || ((DragDstParent >= 0) &&
						(DragDstBuf->At(DragDstParent).Parent < 0)));
		}

// z objektů lze táhnout objekt s daty (bez potomků)
		if ((DragSrcBufID == BufObjID) || (DragSrcBufID == BufLocID))
		{
			if ((DragSrcItem->DatBlok >= 0) &&
				(DragSrcItem->DatIndex >= 0) &&
				((DragSrcItem->Child < 0) || (DragSrcItem->Func == IDF_SPRITE)) &&
				(DragDstParent >= 0))
			{
// příprava cílové funkce větve
				PROGITEM* paritem = &DragDstBuf->At(DragDstParent);
				while (paritem->Parent >= 0)
				{
					paritem = &DragDstBuf->At(paritem->Parent);
				}
				int dstfunc = paritem->Func;

				int srcfunc = DragSrcFunc;
				if (srcfunc == IDF_NONE)
				{
					srcfunc = IDF_ICON;
				}
				if (srcfunc == IDF_FNC) return false;

				paritem = &DragDstBuf->At(DragDstParent);
				if (paritem->Parent < 0)
				{
					return (dstfunc == srcfunc);
				}

				return ((DragDstFunc == IDF_GROUP) && (dstfunc == srcfunc));
			}
			return FALSE;
		}

// přetahování uvnitř okna tříd - jen potomci
		if ((DragSrcBufID == BufClsID) && 
			(DragSrcItem->Parent >= 0) &&
			(DragDstParent >= 0))
		{

// příprava cílové funkce větve
			PROGITEM* paritem = &DragDstBuf->At(DragDstParent);
			while (paritem->Parent >= 0)
			{
				paritem = &DragDstBuf->At(paritem->Parent);
			}
			int dstfunc = paritem->Func;

// příprava zdrojové funkce
			paritem = DragSrcItem;
			while (paritem->Parent >= 0)
			{
				paritem = &DragSrcBuf->At(paritem->Parent);
			}
			int srcfunc = paritem->Func;

// zákaz tažení do jiné větve
			if (srcfunc != dstfunc) return FALSE;

// při přesunu zákaz ponechání ve stejné větvi
//			if (!DragRight && (DragDstParent == DragSrcParent)) return FALSE;

// prvek nelze položit na prvek (kromě do základní položky)
			if (DragDstBuf->At(DragDstParent).Parent < 0) return TRUE;
			if (DragDstBuf->At(DragDstParent).Func != IDF_GROUP) return FALSE;
			return TRUE;
		}
		return FALSE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// test cyklické operace (TRUE=je OK)

BOOL TestCykl()
{
	if (DragSrcBufID == DragDstBufID)
	{
		int item = DragDstParent;

		while (item >= 0)
		{
			if (item == DragSrcIndex) return FALSE;
			item = DragSrcBuf->At(item).Parent;
		}
	}
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// nalezení platné cílové položky v cílovém bufferu (z klientských souřadnic v okně stromu, TRUE=nalezeno)

BOOL FindDragDst(int x, int y)
{
// nalezení položky stromu na dané pozici
	DragDstHPoz = ItemTreePos(x, y, DragDstBuf->IconHeight());

// položka je na začátku ROOT
	if (DragDstHPoz == (HTREEITEM)-1)			// začátek ROOT
	{
		DragDstHPoz = NULL;
		DragDstPoz = -1;							// bude položení na začátek ROOT
	}

// nalezení položky v bufferu
	else
	{
		DragDstPoz = DragDstBuf->Find(DragDstHPoz);

// položka je na konci ROOT
		if (DragDstPoz < 0)							// konec ROOT
		{
			DragDstPoz = -2;						// bude položení za konec položek
			DragDstHPoz = NULL;
		}
	}

// implicitní rodič, není-li cíl nalezen
	DragDstParent = DragDstBuf->Disp();			// implicitní rodič pro položení do ROOT

// příprava odsazování prvků v cílovém stromu (jen je-li cílem platná položka)
	if (DragDstPoz >= 0)
	{
		int indent = ::SendMessage(DragDstTree, TVM_GETINDENT, 0, 0);
		if (indent < 4) indent = 4;
		if (indent > 128) indent = 128;

// zjištění souřadnice položky pod kurzorem (ze souřadnice textu)
		RECT rc;
		*(HTREEITEM*)(&rc) = DragDstHPoz;
		if (!::SendMessage(DragDstTree, TVM_GETITEMRECT, TRUE, (LPARAM)&rc))
		{
			rc.left = DragDstBuf->IconWidth() + 3;
		}
		int pozx = rc.left - DragDstBuf->IconWidth() - 3;
		if (pozx < 0) pozx = 0;

// pokud je kurzor za položkou, bude položka rodičem, uložení bude na začátek
		if (x > (pozx + DragDstBuf->IconWidth()))
		{
			DragDstParent = DragDstPoz;
			DragDstPoz = -1;
			DragDstHPoz = NULL;
		}
		else

// jinak nalezení položky na stejné pozici X
		{
			while ((x < pozx) && 
					(DragDstBuf->At(DragDstPoz).Parent >= 0) &&
					(DragDstBuf->At(DragDstPoz).Parent != DragDstBuf->Disp()))
			{
				DragDstPoz = DragDstBuf->At(DragDstPoz).Parent;
				pozx -= indent;
			}

			DragDstParent = DragDstBuf->At(DragDstPoz).Parent;
		}
	}

// handle rodiče
	DragDstHParent = NULL;
	if (DragDstParent >= 0)
	{
		DragDstHParent = DragDstBuf->At(DragDstParent).HTree;
	}

// příprava parametrů cíle tažení, je-li platný rodič
	if ((DragDstParent >= 0) && (DragDstParent != DragDstBuf->Disp()))
	{
		DragDstFunc = DragDstBuf->At(DragDstParent).Func;
		DragDstSrcMask = DragDstBuf->At(DragDstParent).SrcMask;
		DragDstDstMask = DragDstBuf->At(DragDstParent).DstMask;
		DragDstParam = DragDstBuf->At(DragDstParent).Param;
	}
	else

// příprava parametrů cíle, je-li rodičem ROOT
	{
		DragDstSrcMask = 0;
		DragDstDstMask = 0;
		DragDstParam = 0;
		DragDstFunc = IDF_GROUP;

		switch(DragDstBufID)
		{
		case BufObjID:
		case BufLocID:
			DragDstDstMask = PR_ALLDATA;
			break;

		case BufEdiID:
			DragDstDstMask = PR_COMMAND;
			break;
		}
	}

// test povolení operace
	return (TestDropItem() && TestCykl());
}


/////////////////////////////////////////////////////////////////////////////
// posun myši při tažení

void ProgOnMoveDrag(UINT flags, int x, int y)
{
// lokální proměnné
	HWND	oldtree;					// původní cílový strom
	int		oldparent;					// původní cílový index
	POINT	ptScreen;					// kurzor myši v souřadnicích displeje
	POINT	ptTree;						// kurzor myši v souřadnicích okna stromu
	HCURSOR	curnew = CurNoDrag;			// nový kurzor myši

// kontrola, zda probíhá obsluha tažení
	if (!Draging) return;

// úschova původního cíle tažení
	oldtree = DragDstTree;				// původní cílový strom
	oldparent = DragDstParent;			// původní cílový rodič

// přednastavení neplatného cíle operace
	DragDstTree = NULL;					// není cílový strom
	DragDstBuf = NULL;					// není cílový buffer
	DragDstParent = -1;					// není cílový rodič
	DragDstHParent = NULL;				// není cílový rodič

// příprava nové souřadnice obrázku
	ptScreen.x = x - DragHotSpotX;
	ptScreen.y = y - DragHotSpotY;
	ptTree.x = ptScreen.x + DragTestSpotX;
	ptTree.y = ptScreen.y + DragTestSpotY;
	::ClientToScreen(MainFrame, &ptScreen);

// přetažení obrázku na novou pozici
	::ImageList_DragMove(ptScreen.x, ptScreen.y);

	DragCopy = FALSE;
	DragDelete = FALSE;

// nalezení okna, nad kterým je taženo
	if ((SrcDropWin(ptTree.x, ptTree.y - DragTestSpotY + DragIconHeight/2)) || 
		(SrcDropWin(ptTree.x, ptTree.y - DragTestSpotY + DragIconHeight)) || 
		(SrcDropWin(ptTree.x, ptTree.y - DragTestSpotY)))
	{

// taženi do okna editoru plochy
		if (DragDstBufID == BufMapID)
		{
			EditMap::OnMouseMove(flags, x, y);	
			if ((MouseX != MOUSEINV) && (MouseY != MOUSEINV))
			{
				DragCopy = TRUE;
				curnew = CurCopy;
			}
		}
		else

// taženi do okna editoru sprajtu
		{
			if (DragDstBufID == BufSprID)
			{
				if (DragSrcBuf->At(DragSrcIndex).DatBlok == BufPicID)
				{
					EditSprite::OnMouseMove(flags, x, y);	
					if ((MouseX != MOUSEINV) && (MouseY != MOUSEINV))
					{
						DragCopy = TRUE;
						curnew = CurCopy;
					}
				}
			}
			else

// při tažení do okna struktur bude odhození
			{
				if ((DragDstBufID == BufStrID) &&
					(DragSrcBufID != BufStrID) &&
					!DragRight &&
					((DragSrcBufID != BufClsID) || ((DragSrcParent >= 0) && (DragSrcItem->Child < 0))))
				{
					DragCopy = FALSE;
					DragDelete = TRUE;
					curnew = CurDelete;
				}
				else
				{
					if (DragDstBufID >= PROGBUFNUM)
					{
						DragCopy = FALSE;
						DragDelete = FALSE;
					}
					else
					{

// nastavení příznaků módu kopie nebo přesunu
						DragCopy = TRUE;
						DragDelete = (!DragRight && (DragSrcBufID == DragDstBufID)); 

// příprava kurzoru pro kopírování nebo přesun
						if (DragDelete)
						{
							curnew = CurMove;						// bude přesun
						}
						else
						{
							curnew = CurCopy;						// bude kopie
						}

// nalezení položky pod kurzorem (=pozice cíle tažení)
						::ClientToScreen(MainFrame, &ptTree);
						::ScreenToClient(DragDstTree, &ptTree);
						int dx = DragDstBuf->IconWidth()/2;
						int dy = DragDstBuf->IconHeight()/2;

						if (!FindDragDst(ptTree.x     , ptTree.y     ) &&
							!FindDragDst(ptTree.x - dx, ptTree.y - dy) &&
							!FindDragDst(ptTree.x - dx, ptTree.y     ) &&
							!FindDragDst(ptTree.x - dx, ptTree.y + dy) &&
							!FindDragDst(ptTree.x     , ptTree.y - dy) &&
							!FindDragDst(ptTree.x     , ptTree.y + dy) &&
							!FindDragDst(ptTree.x + dx, ptTree.y - dy) &&
							!FindDragDst(ptTree.x + dx, ptTree.y     ) &&
							!FindDragDst(ptTree.x + dx, ptTree.y + dy) &&
							!FindDragDst(ptTree.x     , ptTree.y - 2*dy) &&
							!FindDragDst(ptTree.x - 2*dx, ptTree.y - 2*dy) &&
							!FindDragDst(ptTree.x - 2*dx, ptTree.y   ) &&
							!FindDragDst(ptTree.x + 2*dx, ptTree.y - 2*dy) &&
							!FindDragDst(ptTree.x + 2*dx, ptTree.y   ))
						{
							DragDstParent = -1;
							DragDstHParent = NULL;
							DragDstPoz = -1;
							DragDstHPoz = NULL;
							DragCopy = FALSE;
							DragDelete = FALSE;
							curnew = CurNoDrag;
						}
					}
				}
			}
		}
	}

// nastavení nového kurzoru myši
	if (curnew != CurAkt)
	{
		CurAkt = curnew;
		::SetCursor(CurAkt);
	}

// zrušení výběru staré cílové položky při změně okna stromu
	if ((oldtree != NULL) && (oldtree != DragDstTree))
	{
		::ImageList_DragLeave(NULL);
		::SendMessage(oldtree, TVM_SELECTITEM, TVGN_DROPHILITE, (LPARAM)NULL);
		::ImageList_DragEnter(NULL, ptScreen.x, ptScreen.y);
	}

// výběr nové cílové položky (změnil se strom nebo položka)
	if (((DragDstParent != oldparent) || (oldtree != DragDstTree))
		&& (DragDstTree != NULL))
	{
		::ImageList_DragLeave(NULL);
		::SendMessage(DragDstTree, TVM_SELECTITEM, TVGN_DROPHILITE, (LPARAM)DragDstHParent);
		::ImageList_DragEnter(NULL, ptScreen.x, ptScreen.y);
	}
}


/////////////////////////////////////////////////////////////////////////////
// ukončení tažení

void ProgOnEndDrag()
{
// kontrola, zda probíhá obsluha tažení
	if (!Draging) return;

// přerušení tažení
	BreakDrag();

// test zrušení položky s referencemi
	if (DragDelete && !DragCopy &&
		!DragSrcBuf->TestRefer(DragSrcIndex)) return;

// tažení do editoru plochy nebo sprajtu
	if (DragCopy)
	{
		if (DragDstBufID == BufMapID)
		{
			EditMap::DragDrop(DragSrcBuf->GetIcon(DragSrcIndex));
			return;
		}

		if (DragDstBufID == BufSprID)
		{
			EditSprite::DragDrop(DragSrcBuf->At(DragSrcIndex).DatIndex);
			return;
		}
	}
	
// vypnutí překreslování cílového okna před operací
	if (DragDstTree != NULL) DragDstBuf->RedrawOff();

// provedení operace
	if (DragCopy)
	{
		int dst = -1;

		if (DragDelete)
		{
			ASSERT(DragDstBufID == DragSrcBufID);

			if (DragDstBufID == DragSrcBufID)
			{
				if (DragDstBufID == BufClsID)
				{
					if (DragDstParent != DragSrcParent)
					{
						dst = ProgLib::Move(DragDstParent, DragDstPoz, DragSrcIndex);
					}
				}
				else
				{
					dst = DragDstBuf->Move(DragDstParent, DragDstPoz, DragSrcIndex);
				}
			}
		}
		else
		{
			if (DragDstBufID == BufClsID)
			{
				dst = ProgLib::Copy(DragDstParent, DragDstPoz, DragSrcBufID, DragSrcIndex);
			}
			else
			{
				dst = DragDstBuf->Copy(DragDstParent, DragDstPoz, DragSrcBufID, DragSrcIndex);
				SetModi();
			}
		}

// aktualizace LOCK a OFF prvků v buferech
		UpdateLock();

// zajištění viditelnosti přenesené položky
		if (dst >= 0)
		{
			::SendMessage(DragDstTree, TVM_ENSUREVISIBLE, 0, (LPARAM)DragDstBuf->At(dst).HTree);
		}
	}
	else
	{
		if (DragDelete)
		{

// rušení položky v bufferu tříd
			if (DragSrcBufID == BufClsID)
			{
				ProgLib::Delete(DragSrcIndex);
			}
			else
			{
				DragSrcBuf->Del(DragSrcIndex);
				SetModi();
				if (EditMode == BufMapID) ProgOnPaint();
			}
		}
	}

// zapnutí překreslování okna po operaci
	if (DragDstTree != NULL) DragDstBuf->RedrawOn();

// aktualizace informací o aktuální položce
	ProgAktItem();

// ukončení záznamu jedné UNDO operace
	Undo.Stop();
}

/////////////////////////////////////////////////////////////////////////////
// přerušení tažení

void BreakDrag()
{

// kontrola, zda probíhá obsluha tažení
	if (Draging)
	{

// vypnutí příznaku tažení
		Draging = FALSE;

// zastavení tažení
		::ImageList_DragLeave(NULL);

// ukončení tažení
		::ImageList_EndDrag();

// zrušení seznamu obrázků tažení
		::ImageList_Destroy(DragImageList);

// zrušení výběru cíle operace u cílového okna
		if (DragDstTree)
		{
			::SendMessage(DragDstTree, TVM_SELECTITEM, TVGN_DROPHILITE, (LPARAM)NULL);
		}

// uvolněni zachytávání myši
		::ReleaseCapture();

// nastavení běžného kurzoru myši
		::SetCursor(CurArrow);
	}
}
