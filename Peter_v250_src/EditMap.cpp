
#include "Main.h"

/***************************************************************************\
*																			*
*								Editor ploch								*
*																			*
\***************************************************************************/

namespace EditMap
{

/////////////////////////////////////////////////////////////////////////////
// parametry

// parametry editované plochy
int		Index = 0;						// editovaná plocha
int		Width;							// šířka plochy v políčkách
int		Height;							// výška plochy v políčkách
int		MWidth;							// šířka plochy v bodech
int		MHeight;						// výška plochy v bodech

// zobrazená část plochy
int		TLeft;							// počáteční bod plochy vlevo
int		TBottom;						// počáteční bod plochy dole

// zobrazení plochy na displeji (v bodech displeje)
int		DLeft;							// počátek zobrazení vlevo
int		DTop;							// počátek zobrazení nahoře
int		DWidth;							// zobrazená šířka části plochy na displeji
int		DHeight;						// zobrazená výška části plochy na displeji

int		ILeft;							// index ikony vlevo
int		IRight;							// index ikony vpravo
int		ITop;							// index ikony nahoře
int		IBottom;						// index ikony dole

int		OLeft;							// zobrazená část ikony vlevo
int		ORight;							// zobrazená část ikony vpravo
int		OTop;							// zobrazená část ikony nahoře
int		OBottom;						// zobrazená část ikony dole

// nastavení nových rozměrů
int		NewDimWidth;					// nová šířka
int		NewDimHeight;					// nová výška
BOOL	NewDimFill = FALSE;				// probíhá plnění textů

// editační mód
int		Mode = IDN_MAP;					// editační mód
BOOL	Rastr = TRUE;					// příznak zobrazení rastru
BOOL	EditMap = FALSE;				// editační mód
BOOL	FillMap = FALSE;				// mód výplně
BOOL	MFilling = FALSE;				// označování bloku pro výplň
int		FillX;							// počáteční ikona výplně X
int		FillY;							// počáteční ikona výplně Y

// obsluha tažení
BOOL	BegDrag = FALSE;				// příznak zahájení tažení
BOOL	MDraging = FALSE;				// příznak tažení ikony editoru plochy
int		BegX;							// souřadnice myši X před zahájením tažení
int		BegY;							// souřadnice myši Y před zahájením tažení
BOOL	CopyMode = FALSE;				// příznak módu kopírování
int		DragX = MOUSEINV;				// souřadnice X tažené ikony (MOUSEINV = není)
int		DragY = MOUSEINV;				// souřadnice Y tažené ikony (MOUSEINV = není)
HIMAGELIST DragImageList;				// obrázek tažení
int		HotSpotX;						// bod dotyku X
int		HotSpotY;						// bod dotyku Y

// ikony přepínačů
CBufIcon	MapSwc;						// ikony přepínačů
CBufIcon	MapSwcNum;					// ikony čísel

// editace číselné položky
int		EditNumX;						// souřadnice X editované ikony
int		EditNumY;						// souřadnice Y editované ikony
BOOL	EditNumFill = FALSE;			// probíhá plnění číselné položky
#define	EDITNUMHEIGHT 16				// výška editačního pole

// ************************** obsluha volby editace *************************

/////////////////////////////////////////////////////////////////////////////
// inicializace při startu programu

void StartInit()
{
// načtení ikon přepínačů
	MapSwc.IconSize(12, 12);
	MapSwc.Load(IDN_MAPSWC, 10);
	ASSERT(MapSwc.Num() == 10);

// načtení ikon čísel
	MapSwcNum.IconSize(8, 12);
	MapSwcNum.Load(IDN_MAPSWCNUM, 10);
	ASSERT(MapSwcNum.Num() == 10);
}


/////////////////////////////////////////////////////////////////////////////
// zahájení editace (musí být platný index!)

void BegEdit(int index)
{
// ukončení editace číselného parametru
	EndEditSwcNum();

// úschova indexu plochy
	Index = index;						// index ikony/obrázku
	ASSERT(Map.IsValid(index));
	Map[index].CopyWrite();				// kopie před zápisem

// úschova parametrů plochy
	Width = Map[index].Width();			// šířka plochy v políčkách
	Height = Map[index].Height();		// výška plochy v políčkách

	MDraging = FALSE;					// není tažení

// přepočet rozložení a zobrazení obrázku
	Disp();

// aktualizace voleb bloku
	UpdateClipboard();
}

/////////////////////////////////////////////////////////////////////////////
// obsluha dialogu pro zadání rozměrů

// nastavení textu šířky
void DimenDialSetWidth(HWND wnd)
{
	CText txt;
	txt.Int(NewDimWidth);
	NewDimFill = TRUE;
	txt.SetDialogText(wnd, IDC_SIRKA);
	NewDimFill = FALSE;
}

// načtení šířky
void DimenDialGetWidth(HWND wnd)
{
	if (!NewDimFill)
	{
		CText txt;
		txt.GetDialogText(wnd, IDC_SIRKA);
		NewDimWidth = Int(txt);
		if (NewDimWidth < 1) NewDimWidth = Width;
		if (NewDimWidth > MAPMAXWIDTH) NewDimWidth = MAPMAXWIDTH;
	}
}

// nastavení textu výšky
void DimenDialSetHeight(HWND wnd)
{
	CText txt;
	txt.Int(NewDimHeight);
	NewDimFill = TRUE;
	txt.SetDialogText(wnd, IDC_VYSKA);
	NewDimFill = FALSE;
}

// načtení výšky
void DimenDialGetHeight(HWND wnd)
{
	if (!NewDimFill)
	{
		CText txt;
		txt.GetDialogText(wnd, IDC_VYSKA);
		NewDimHeight = Int(txt);
		if (NewDimHeight < 1) NewDimHeight = Height;
		if (NewDimHeight > MAPMAXHEIGHT) NewDimHeight = MAPMAXHEIGHT;
	}
}

// callback funkce dialogu
BOOL CALLBACK DimenDialogProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_INITDIALOG:
		{
			CenterWindow(hWnd, MainFrame);
			CText txt;

			txt.Load(IDN_MAPDIMEN);
			txt.SetWindowTextFont(hWnd);

			DialogCustomText(hWnd, IDN_SIRKA);
			DialogCustomText(hWnd, IDN_VYSKA);

			txt.Load(IDN_CANCEL0);
			txt.SetDialogTextFont(hWnd, IDCANCEL);

			DimenDialSetWidth(hWnd);
			DimenDialSetHeight(hWnd);
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
				DimenDialGetWidth(hWnd);
				DimenDialGetHeight(hWnd);
				::EndDialog(hWnd, LOWORD(wParam));
				return TRUE;
			}
			break;

		case EN_KILLFOCUS:
			if (LOWORD(wParam) == IDC_SIRKA)
			{
				DimenDialGetWidth(hWnd);
				DimenDialSetWidth(hWnd);
			}

			if (LOWORD(wParam) == IDC_VYSKA)
			{
				DimenDialGetHeight(hWnd);
				DimenDialSetHeight(hWnd);
			}
			break;
		}
		break;
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// nastavení rozměrů plochy

void OnDimen()
{
// příprava výchozích rozměrů
	NewDimWidth = Width;
	NewDimHeight = Height;

// ukončení editace číselného parametru
	EndEditSwcNum();

// zadání nových rozměrů plochy
	if (::DialogBoxParam(
			hInstance,
			MAKEINTRESOURCE(IDN_MAPDIMEN),
			MainFrame,
			(DLGPROC)DimenDialogProc,
			(LPARAM)IDN_MAPDIMEN) != IDOK) return;

// kontrola, zda byly rozměry změněny
	if ((NewDimWidth == Width) && (NewDimHeight == Height))
	{
		return;
	}

// zapnutí kurzoru čekání
	BeginWaitCursor();

// úschova plochy pro UNDO
	PushUndo();

// odpojení referencí plochy
	Map[Index].RefDec();

// nastavení nových rozměrů obrázku
	ASSERT(Map.IsValid(Index));
	Width = NewDimWidth;
	Height = NewDimHeight;
	Map[Index].Resize(Width, Height);

// napojení referencí plochy
	Map[Index].RefInc();

// aktualizace aktivní položky (zobrazení referencí)
	ProgAktItem();

// překreslení displeje
	Disp();

// příznak modifikace
	SetModi();

// aktualizace voleb bloku
	UpdateClipboard();

// vypnutí kurzoru čekání
	EndWaitCursor();
}



// ***************************** obsluha zobrazení **************************

/////////////////////////////////////////////////////////////////////////////
// přepočet rozměrů

void ReCalc()
{
// šířka a výška plochy v bodech
	MWidth = Width * ICONWIDTH;
	MHeight = Height * ICONHEIGHT;

// příprava zobrazené výšky (korekce, je-li horizontální posuvník)
	DHeight = EditHeight - 2 - 1;				// výška k zobrazení
	if (EditWidth - 4 - TRACKSIRKA < MWidth)
	{
		DHeight -= TRACKSIRKA;					// snížený výšky o horizontální posuvník
		if (DHeight < 1) DHeight = 1;			// minimální šířka
	}

// příprava zobrazené šířky (korekce, je-li vertikální posuvník)
	DWidth = EditWidth - 4 - 1;					//	šířka k zobrazení
	if (DHeight < MHeight)
	{
		DWidth -= TRACKSIRKA;					// snížení šířky o vertikální posuvník
		if (DWidth < 1) DWidth = 1;				// minimální šířka
	}

// zobrazený počátek
	DLeft = EditX + 2 + (DWidth - MWidth)/2;
	if (DWidth < MWidth) DLeft = EditX + 2;
	DTop = EditY + 2 + (DHeight - MHeight)/2;
	if (DHeight < MHeight) DTop = EditY + 2;

// omezení na skutečnou výšku a šířku
	if (DHeight > MHeight) DHeight = MHeight;	// omezení na skutečné rozměry
	if (DWidth > MWidth) DWidth = MWidth;		// omezení na skutečné rozměry

// omezení počátku při zobrazení editačního pole
	if (EditMapNumWnd)
	{
		int i = (EditNumX+1)*ICONWIDTH - DWidth;
		if (TLeft < i) TLeft = i;
		i = EditNumX*ICONWIDTH;
		if (TLeft > i) TLeft = i;

		i = (EditNumY+1)*ICONHEIGHT-EDITNUMHEIGHT-1;
		if (TBottom > i) TBottom = i;
		i = (EditNumY+1)*ICONHEIGHT - DHeight;
		if (TBottom < i) TBottom = i;
	}

// omezení počátečního bodu
	if (TLeft < 0) TLeft = 0;
	if (TLeft > MWidth - DWidth) TLeft = MWidth - DWidth;
	if (TBottom < 0) TBottom = 0;
	if (TBottom > MHeight - DHeight) TBottom = MHeight - DHeight;

// přepočet zobrazeného počátku na ikony a body
	ILeft = TLeft / ICONWIDTH;					// index ikony vlevo
	OLeft = ICONWIDTH - (TLeft % ICONWIDTH);	// zobrazená část ikony vlevo
	IBottom = TBottom / ICONHEIGHT;				// index ikony dole
	OBottom = ICONHEIGHT - (TBottom % ICONHEIGHT); // zobrazená část ikony dole

// zobrazená část ikon vpravo a nahoře
	ORight = ICONWIDTH - ((MWidth - TLeft - DWidth) % ICONWIDTH);
	OTop = ICONHEIGHT - ((MHeight - TBottom - DHeight) % ICONHEIGHT);

// index ikon vpravo a dole
	IRight = ILeft + (DWidth - OLeft + ICONWIDTH - 1) / ICONWIDTH;
	ITop = IBottom + (DHeight - OBottom + ICONHEIGHT - 1) / ICONHEIGHT;

// příznak zapnutí posuvníků
	HScrollDisp = (DWidth < MWidth);
	VScrollDisp = (DHeight < MHeight);
}


/////////////////////////////////////////////////////////////////////////////
// zobrazení celého okna editoru

void Disp()
{
// přepočet zobrazení
	ReCalc();

// nastavení posuvníků
	SetScroll();

// otevření kontextu displeje
	HDC dc = ::GetDC(MainFrame);

// nastavení vlastních palet
	HPALETTE OldPal;					// úschova původních palet
	OldPal = ::SelectPalette(dc,		// výběr vlastních palet
		StdPalette, FALSE);
	::RealizePalette(dc);				// realizace palet

// příprava štětce k vymazání podkladu
	HBRUSH brush = (HBRUSH)::GetStockObject(LTGRAY_BRUSH); // štětec k vymazání plochy

// vymazání plochy nahoře nad obrázkem
	RECT rc;
	rc.left = EditX + 2;
	rc.right = EditX + EditWidth - 2;
	rc.top = EditY + 2;
	rc.bottom = DTop;
	if (rc.top < rc.bottom)
	{
		::FillRect(dc, &rc, brush);
	}

// vymazání plochy dole pod obrázkem (left a right je nastaveno)
	rc.top = DTop + DHeight;
	rc.bottom = EditY + EditHeight - 2;
	if (rc.top < rc.bottom)
	{
		::FillRect(dc, &rc, brush);
	}

// vymazání plochy vlevo od obrázku (left je nastaveno)
	rc.right = DLeft;
	rc.top = DTop;
	rc.bottom = DTop + DHeight;
	if (rc.left < rc.right)
	{
		::FillRect(dc, &rc, brush);
	}

// vymazání plochy vpravo od obrázku (top a bottom je nastaveno)
	rc.left = DLeft + DWidth;
	rc.right = EditX + EditWidth - 2;
	if (rc.left < rc.right)
	{
		::FillRect(dc, &rc, brush);
	}

// vykreslení obrázku
	DispAkt(dc);

// zrušení štětce podkladu (i když podle dokumentace rušení není nutné)
	::DeleteObject(brush);

// návrat původních palet
	::SelectPalette(dc,OldPal,FALSE);

// uvolnění kontextu displeje
	::ReleaseDC(MainFrame, dc);
}


/////////////////////////////////////////////////////////////////////////////
// aktualizační překreslení obrázku (s přepočtem rozměrů)

void ReDisp()
{
// přepočet zobrazení
	ReCalc();

// nastavení posuvníků
	SetScroll();

// otevření kontextu displeje
	HDC dc = ::GetDC(MainFrame);

// nastavení vlastních palet
	HPALETTE OldPal;					// úschova původních palet
	OldPal = ::SelectPalette(dc,		// výběr vlastních palet
		StdPalette, FALSE);
	::RealizePalette(dc);				// realizace palet

// vykreslení obrázku
	DispAkt(dc);

// návrat původních palet
	::SelectPalette(dc,OldPal,FALSE);

// uvolnění kontextu displeje
	::ReleaseDC(MainFrame, dc);
}


/////////////////////////////////////////////////////////////////////////////
// aktualizační překreslení obrázku

void DispAkt(HDC dc)
{
// lokální proměnné
	int i,j, k, l;
	int width = (IRight - ILeft + 1)*ICONWIDTH;
	int height = (ITop - IBottom + 1)*ICONHEIGHT;
	BYTE* dst;
	BYTE* src;
	MAPITEM* data = Map[Index].DataData();
	int cislic;

// vytvoření bufferu k zobrazení
	BYTE* buf = (BYTE*)MemGet(width*height);

// zobrazení ikon
	for (i = ILeft; i <= IRight; i++)
	{
		for (j = IBottom; j <= ITop; j++)
		{
			(Icon.Get(data[i + j*Width].Icon)).DeComp();
			src = (Icon.Get(data[i + j*Width].Icon)).DataData();
			dst = buf + (i-ILeft)*ICONWIDTH + (j-IBottom)*ICONHEIGHT*width;

			for (k = ICONHEIGHT; k > 0; k--)
			{
				MemCopy(dst, src, ICONWIDTH);
				dst += width;
				src += ICONWIDTH;
			}
			dst -= width * ICONHEIGHT;

// zobrazení přepínače
			k = -1;
			switch (Mode)
			{
			case IDN_SWC5: k++;		// 4
			case IDN_SWC4: k++;		// 3
			case IDN_SWC3: k++;		// 2
			case IDN_SWC2: k++;		// 1
			case IDN_SWC1: k++;		// 0
				if (data[i + j*Width].Param & ((DWORD)MAP_FLAG1 << k))
				{
					k = 2*k;
				}
				else
				{
					k = 2*k +1;
				}

				dst += width * (ICONHEIGHT - 12);
				src = MapSwc[k].DataData();
				for (k = 12; k > 0; k--)
				{
					MemCopy(dst, src, 12);
					dst += width;
					src += 12;
				}
				break;

// zobrazení číselné hodnoty
			case IDN_SWCN1:	k = ((data[i + j*Width].Param & MAP_VALUE1_MASK) >> MAP_VALUE1_ROT);
							goto DISPAKTSWCNUM;

			case IDN_SWCN2:	k = ((data[i + j*Width].Param & MAP_VALUE2_MASK) >> MAP_VALUE2_ROT);
							goto DISPAKTSWCNUM;

			case IDN_SWCN3:	k = ((data[i + j*Width].Param & MAP_VALUE3_MASK) >> MAP_VALUE3_ROT);

		DISPAKTSWCNUM:
				cislic = 1;				// počet číslic
				if (k >= 10) cislic++;
				if (k >= 100) cislic++;
				if (k >= 1000) cislic++;

				dst += width * (ICONHEIGHT - 12) + (cislic-1)*8;

				for (; cislic > 0; cislic--)
				{
					src = MapSwcNum[k % 10].DataData();

					for (l = 12; l > 0; l--)
					{
						MemCopy(dst, src, 8);
						dst += width;
						src += 8;
					}

					k /= 10;
					dst -= 12*width + 8;
				}
			}
		}
	}

// zobrazení rastru
	if (Rastr)
	{
		BYTE col = BlueCol;

		dst = buf + ICONHEIGHT*width;
		for (i = ITop - IBottom; i > 0; i--)
		{
			MemFill(dst, width, col);
			dst += ICONHEIGHT*width;
		}

		for (i = IRight - ILeft; i > 0; i--)
		{
			dst = buf + ICONWIDTH*i - 1;
			for (j = height; j > 0; j--)
			{
				*dst = col;
				dst += width;
			}
		}
	}

// přednastavení parametrů záhlaví bitmapy
	StdBitmapInfo->bmiHeader.biWidth = width;
	StdBitmapInfo->bmiHeader.biHeight = DHeight;

// zobrazení
	::StretchDIBits(dc, DLeft, DTop, DWidth, DHeight, ICONWIDTH - OLeft,
			0, DWidth, DHeight, buf + (ICONHEIGHT-OBottom)*width,
			StdBitmapInfo, DIB_RGB_COLORS, SRCCOPY);

// zrušení bufferu
	MemFree(buf);
}


/////////////////////////////////////////////////////////////////////////////
// zapnutí/vypnutí rastru

void OnRastr()
{
	Rastr = !Rastr;
	ReDisp();
	UpdateMenu();
}


/////////////////////////////////////////////////////////////////////////////
// obsluha zobrazení kurzoru myši (TRUE=obslouženo)

BOOL OnSetCursor(int x, int y)
{
// pole obrázku
	if (((x >= DLeft) &&
		(x < DLeft + DWidth) &&
		(y >= DTop) &&
		(y < DTop + DHeight)))
	{
		if (EditMap)
		{
			CurAkt = CurPen;
		}
		else
		{
			if (FillMap)
			{
				CurAkt = CurFill;
			}
			else
			{
				if (MDraging)
				{
					if ((MouseX == MOUSEINV) || (MouseY == MOUSEINV))
					{
						if (CopyMode)
						{
							CurAkt = CurNoDrag;
						}
						else
						{
							CurAkt = CurDelete;
						}
					}
					else
					{
						if (CopyMode)
						{
							CurAkt = CurCopy;
						}
						else
						{
							CurAkt = CurMove;
						}
					}
				}
				else
				{
					CurAkt = CurRuka;
				}
			}
		}
		::SetCursor(CurAkt);
		return TRUE;
	}

// implicitní obsluha
	return FALSE;
}


// ********************* obsluha posuvníků **********************************


/////////////////////////////////////////////////////////////////////////////
// přesun posuvníků a editačního pole při změně rozměrů okna

HDWP MoveScroll(HDWP hdwp)
{
// lokální proměnné
	int x, y, w, h;

// přepočet zobrazení
	ReCalc();

// přesun a zobrazení horizontálního posuvníku
	x = EditX + 2;
	y = EditY + EditHeight - 2 - TRACKSIRKA;
	w = EditWidth - 4;
	h = TRACKSIRKA;
	if (VScrollDisp) w -= TRACKSIRKA;
	hdwp = ::DeferWindowPos(hdwp, HScroll, NULL, x, y, w, h,
			SWP_DRAWFRAME | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER |
			((HScrollDisp && !PreviewPic) ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));

// přesun a zobrazení vertikálního posuvníku
	x = EditX + EditWidth - 2 - TRACKSIRKA;
	y = EditY + 2;
	w = TRACKSIRKA;
	h = EditHeight - 4;
	if (HScrollDisp) h -= TRACKSIRKA;
	hdwp = ::DeferWindowPos(hdwp, VScroll, NULL, x, y, w, h,
			SWP_DRAWFRAME | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER |
			((VScrollDisp && !PreviewPic) ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));

// přesun editačního pole
	if (EditMapNumWnd)
	{
		x = DLeft + (EditNumX - ILeft)*ICONWIDTH - (ICONWIDTH - OLeft);
		y = DTop + (ITop - EditNumY - 1)*ICONHEIGHT + OTop;
		w = ICONWIDTH;
		h = EDITNUMHEIGHT;
		if (w > DWidth) w = DWidth;
		if (h > DHeight) h = DHeight;
		hdwp = ::DeferWindowPos(hdwp, EditMapNumWnd, NULL, x, y, w, h, 
			SWP_NOOWNERZORDER | SWP_NOZORDER);
	}

// nastavení posuvníků
	SetScroll();

// nová hodnota HDWP
	return hdwp;
}


/////////////////////////////////////////////////////////////////////////////
// aktualizace zobrazení posuvníků

void SetScroll()
{
// lokální proměnné
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);

// -------- horizontální posuvník -------------

// načtení stavu horizontálního posuvníku
	si.fMask = SIF_ALL;
	::GetScrollInfo(HScroll, SB_CTL, &si);	// načtení stavu posuvníku
	si.fMask = 0;							// přednastavení - nic se nemění

// minimální pozice
	if (si.nMin != 0)
	{
		si.nMin = 0;
		si.fMask |= SIF_RANGE;
	}

// maximální pozice
	if (si.nMax != MWidth)
	{
		si.nMax = MWidth;
		si.fMask |= SIF_RANGE;
	}

// velikost stránky
	if (si.nPage != (UINT)DWidth)
	{
		si.nPage = (UINT)DWidth;
		si.fMask |= SIF_PAGE;
	}

// pozice posuvníku
	if (si.nPos != TLeft)
	{
		si.nPos = TLeft;
		si.fMask |= SIF_POS;
	}

// nastavení nových parametrů posuvníku
	if (si.fMask)
	{
		::SetScrollInfo(HScroll, SB_CTL, &si, TRUE);
	}

// zobrazení horizontálního posuvníku
	::ShowWindow(HScroll, (HScrollDisp && !PreviewPic) ? SW_SHOWNA : SW_HIDE);


// -------- vertikální posuvník -------------

// načtení stavu vertikálního posuvníku
	si.fMask = SIF_ALL;
	::GetScrollInfo(VScroll, SB_CTL, &si);
	si.fMask = 0;							// přednastavení - nic se nemění

// minimální pozice
	if (si.nMin != 0)
	{
		si.nMin = 0;
		si.fMask |= SIF_RANGE;
	}

// maximální pozice
	if (si.nMax != MHeight)
	{
		si.nMax = MHeight;
		si.fMask |= SIF_RANGE;
	}

// velikost stránky
	if (si.nPage != (UINT)DHeight)
	{
		si.nPage = (UINT)DHeight;
		si.fMask |= SIF_PAGE;
	}

// pozice posuvníku
	if (si.nPos != MHeight - DHeight - TBottom)
	{
		si.nPos = MHeight - DHeight - TBottom;
		si.fMask |= SIF_POS;
	}

// nastavení nových parametrů posuvníku
	if (si.fMask)
	{
		::SetScrollInfo(VScroll, SB_CTL, &si, TRUE);
	}

// zobrazení vertikálního posuvníku
	::ShowWindow(VScroll, (VScrollDisp && !PreviewPic) ? SW_SHOWNA : SW_HIDE);
}

/////////////////////////////////////////////////////////////////////////////
// aktualizace pozice editačního pole

void SetEditMapNum()
{
	if (EditMapNumWnd)
	{
		ReCalc();

		int x = DLeft + (EditNumX - ILeft)*ICONWIDTH - (ICONWIDTH - OLeft);
		int y = DTop + (ITop - EditNumY - 1)*ICONHEIGHT + OTop;
		int w = ICONWIDTH;
		int h = EDITNUMHEIGHT;
		if (w > DWidth) w = DWidth;
		if (h > DHeight) h = DHeight;
		::SetWindowPos(EditMapNumWnd, NULL, x, y, w, h, SWP_NOOWNERZORDER | SWP_NOZORDER);
	}
}


/////////////////////////////////////////////////////////////////////////////
// horizontální posuvník

void OnHScroll(int code, int pos)
{
	switch (code)
	{
	case SB_LEFT:
		pos = 0;
		break;

	case SB_LINELEFT:
		pos -= DWidth/20+1;
		break;

	case SB_LINERIGHT:
		pos += DWidth/20+1;
		break;

	case SB_PAGELEFT:
		pos -= DWidth-16;
		break;

	case SB_PAGERIGHT:
		pos += DWidth-16;
		break;

	case SB_RIGHT:
		pos = MWidth - DWidth;
		break;
	}

	if (pos < 0) pos = 0;
	if (pos >= MWidth - DWidth)
		pos = MWidth - DWidth;

	if (pos != TLeft)
	{
		TLeft = pos;
		SetEditMapNum();
		Disp();
	}
}


/////////////////////////////////////////////////////////////////////////////
// vertikální posuvník

void OnVScroll(int code, int pos)
{
	switch (code)
	{
	case SB_TOP:
		pos = 0;
		break;

	case SB_LINEUP:
		pos -= DHeight/20+1;
		break;

	case SB_LINEDOWN:
		pos += DHeight/20+1;
		break;

	case SB_PAGEUP:
		pos -= DHeight-16;
		break;

	case SB_PAGEDOWN:
		pos += DHeight-16;
		break;

	case SB_BOTTOM:
		pos = MHeight - DHeight;
		break;
	}

	if (pos < 0) pos = 0;
	if (pos >= MHeight - DHeight)
		pos = MHeight - DHeight;
	pos = MHeight - DHeight - pos;

	if (pos != TBottom)
	{
		TBottom = pos;
		SetEditMapNum();
		Disp();
	}
}


// **************************** obsluha editace *****************************

/////////////////////////////////////////////////////////////////////////////
// zahájení editace číselného parametru

void BegEditSwcNum(int x, int y)
{
// kontrola položky
	if (((DWORD)x >= (DWORD)Width) || ((DWORD)y >= (DWORD)Height))
	{
		return;
	}

// ukončení editace číselného parametru
	EndEditSwcNum();

// úschova parametrů
	EditNumX = x;
	EditNumY = y;
	EditNumFill = TRUE;

// příprava souřadnic položky
	int left = DLeft + (EditNumX - ILeft)*ICONWIDTH - (ICONWIDTH - OLeft);
	int top = DTop + (ITop - EditNumY - 1)*ICONHEIGHT + OTop;

// vytvoření editačního okna
	EditMapNumWnd = ::CreateWindowEx(
		0,								// rozšířený styl
		_T("EDIT"),						// třída
		NULL,							// text
		WS_CHILD | ES_AUTOHSCROLL | ES_NUMBER,	// styl
		left,							// X
		top,							// Y
		ICONWIDTH,						// šířka
		EDITNUMHEIGHT,					// výška
		MainFrame,						// rodič
		NULL,							// ID
		hInstance,						// instance
		NULL);							// data

// příprava čísla
	int cislo = (Map[Index].DataData())[EditNumX + EditNumY*Width].Param;
	switch (Mode)
	{
	case IDN_SWCN1: cislo &= MAP_VALUE1_MASK;
					cislo >>= MAP_VALUE1_ROT;
					break;

	case IDN_SWCN2: cislo &= MAP_VALUE2_MASK;
					cislo >>= MAP_VALUE2_ROT;
					break;

	case IDN_SWCN3: cislo &= MAP_VALUE3_MASK;
					cislo >>= MAP_VALUE3_ROT;
					break;
	}

// nastavení čísla
	CText txt;
	txt.Int(cislo);
	txt.SetWindowText(EditMapNumWnd);

// zobrazení okna
	::ShowWindow(EditMapNumWnd, SW_SHOW);
	::SetFocus(EditMapNumWnd);
	::SendMessage(EditMapNumWnd, EM_SETSEL, 0, (LPARAM)-1);
	EditNumFill = FALSE;

// aktualizace menu
	UpdateClipboard();
	Disp();
}


/////////////////////////////////////////////////////////////////////////////
// změna editovaného čísla

void OnChangeEditSwcNum()
{
// test platnosti
	if (ProgMode && EditMapNumWnd && !EditNumFill && (EditMode == BufMapID))
	{

// načtení zadaného čísla
		CText txt;
		txt.GetWindowText(EditMapNumWnd);
		int cislo = Int(txt);
		if (cislo < 0) cislo = 0;

// adresa položky
		MAPITEM* item = Map[Index].DataData() + (EditNumX + EditNumY * Width);

// nastavení čísla
		int max = 0;
		DWORD mask = 0;
		int rot = 0;

		switch(Mode)
		{
		case IDN_SWCN1:
			max = MAP_VALUE1_MAX;
			mask = MAP_VALUE1_MASK;
			rot = MAP_VALUE1_ROT;
			break;

		case IDN_SWCN2:
			max = MAP_VALUE2_MAX;
			mask = MAP_VALUE2_MASK;
			rot = MAP_VALUE2_ROT;
			break;

		case IDN_SWCN3:
			max = MAP_VALUE3_MAX;
			mask = MAP_VALUE3_MASK;
			rot = MAP_VALUE3_ROT;
			break;
		}

		if (cislo > max)
		{
			cislo = max;
		}

		cislo <<= rot;

		if ((DWORD)cislo != (item->Param & mask))
		{
			item->Param &= ~mask;
			item->Param |= (DWORD)cislo;
			SetModi();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// ukončení editace čísla

void EndEditSwcNum()
{
	if (EditMapNumWnd)
	{
		EditNumFill = TRUE;
		::DestroyWindow(EditMapNumWnd);
		EditMapNumWnd = NULL;
		EditNumFill = FALSE;
		Disp();
		ProgUpdateUndoRedo();
	}
}


/////////////////////////////////////////////////////////////////////////////
// ukončení tažení

void EndDrag()
{
	BegDrag = FALSE;

	if (MDraging)
	{
		MDraging = FALSE;

// zastavení tažení
		::ImageList_DragLeave(NULL);

// ukončení tažení
		::ImageList_EndDrag();

// zrušení seznamu obrázků tažení
		::ImageList_Destroy(DragImageList);

// uvolněni zachytávání myši
		::ReleaseCapture();

// nastavení běžného kurzoru myši
		::SetCursor(CurArrow);
	}
}


/////////////////////////////////////////////////////////////////////////////
// zobrazení výběru výplně

void DispFill()
{
	if ((DragX == MOUSEINV) || (DragY == MOUSEINV)) return;

// příprava DC displeje
	HDC dc = ::GetDC(MainFrame);

// výběr štětce
	HBRUSH oldBrush = (HBRUSH)::SelectObject(dc, HalfToneBrush);

// příprava souřadnic
	int x1 = (FillX - ILeft)*ICONWIDTH + DLeft + OLeft - ICONWIDTH;
	int y1 = (ITop - FillY)*ICONHEIGHT + DTop + OTop - ICONHEIGHT;
	int x2 = (DragX - FillX)*ICONWIDTH + x1;
	int y2 = (FillY - DragY)*ICONHEIGHT + y1;

	if (x1 > x2)
	{
		int n = x2;
		x2 = x1;
		x1 = n;
	}

	if (y1 > y2)
	{
		int n = y2;
		y2 = y1;
		y1 = n;
	}

	x2 += ICONWIDTH;
	y2 += ICONHEIGHT;

	if (x1 < DLeft) x1 = DLeft;
	if (y1 < DTop) y1 = DTop;
	if (x2 >= DLeft + DWidth) x2 = DLeft + DWidth;
	if (y2 >= DTop + DHeight) y2 = DTop + DHeight;

	::PatBlt(dc, x1, y1, x2 - x1, y2 - y1, PATINVERT);

// navrácení původního štětce
	::SelectObject(dc, oldBrush);

// uvolnění DC displeje
	::ReleaseDC(MainFrame, dc);
}


/////////////////////////////////////////////////////////////////////////////
// ukončení tažení výplně

void EndFill()
{
	DispFill();
	MFilling = FALSE;
	if ((DragX == MOUSEINV) || (DragY == MOUSEINV)) return;

	int x1 = FillX;
	int y1 = FillY;
	int x2 = DragX;
	int y2 = DragY;

	int n;

	if (x1 > x2)
	{
		n = x2;
		x2 = x1;
		x1 = n;
	}

	if (y1 > y2)
	{
		n = y2;
		y2 = y1;
		y1 = n;
	}

	if ((x1 == x2) && (y1 == y2)) return;

	PushUndo();

	MAPITEM* data = Map[Index].DataData();
	MAPITEM* srcdata = data + (FillX + FillY*Width);

	for (int x = x1; x <= x2; x++)
	{
		for (int y = y1; y <= y2; y++)
		{
			MAPITEM* dstdata = data + (x + y*Width);

//			Map[Index].
				RefIconDec(dstdata->Icon);
//			Map[Index].
				RefIconInc(srcdata->Icon);
			dstdata->Icon = srcdata->Icon;
			dstdata->Param = srcdata->Param;
		}
	}

	SetModi();
	ProgAktItem();
	Disp();
}


/////////////////////////////////////////////////////////////////////////////
// stisk tlačítka myši (TRUE = obslouženo)

BOOL OnButtonDown(UINT flags, int x, int y, BOOL right, BOOL dclick)
{
// ukončení editace číselného parametru
	EndEditSwcNum();

// ukončení tažení
	if (MFilling)
	{
		EndFill();
		return TRUE;
	}

	if (MDraging)
	{
		EndDrag();
		return TRUE;
	}

// kontrola, zda je platné editační pole
	if ((MouseX == MOUSEINV) || (MouseY == MOUSEINV))
	{
		return FALSE;
	}

// úschova souřadnic před zahájením tažení
	BegX = x;
	BegY = y;
	DragX = MouseX;
	DragY = MouseY;

// výplň plochy
	if (FillMap)
	{
		FillX = MouseX;
		FillY = MouseY;
		MFilling = TRUE;
		DispFill();
		return TRUE;
	}

	BegDrag = TRUE;
	CopyMode = FALSE;

// úschova plochy pro UNDO
	if (EditMap && (Mode != IDN_MAP))
	{
		PushUndo();
	}

// adresa položky
	MAPITEM* data = Map[Index].DataData() + (MouseX + MouseY*Width);

// index ikony
	int icon = data->Icon;

// zahájení editace
	int k = -1;

	if (EditMap)
	{
		switch (Mode)
		{
		case IDN_MAP:
			if (Icon.IsValid(icon))
			{
				CBufProg* buf = &BufObj;
				int inx = BufObj.SrcDat(BufIcoID, icon);
				if (inx < 0)
				{
					buf = &BufLoc;
					inx = BufLoc.SrcDat(BufIcoID, icon);
				}

				if (buf->IsValid(inx))
				{
					if (SetEditMode(BufIcoID, icon))
					{
						SetToolMode(BufIcoID);
						BufEdi.Nadpis(Text.GetTxt(buf->GetText(inx)));
						SetFocusEdit();
//						ProgUpdateInOut();
					}
				}
			}
			break;

		case IDN_SWC5:	k++;	// 4
		case IDN_SWC4:	k++;	// 3
		case IDN_SWC3:	k++;	// 2
		case IDN_SWC2:	k++;	// 1
		case IDN_SWC1:	k++;	// 0
			data->Param ^= ((DWORD)MAP_FLAG1 << k);
			SetModi();
			ReDisp();
			break;

		case IDN_SWCN1:
		case IDN_SWCN2:
		case IDN_SWCN3:
			BegEditSwcNum(MouseX, MouseY);
			break;
		}
	}
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// posun myši

void OnMouseMove(UINT flags, int x, int y)
{
// korekce souřadnic při tažení (na střed ikony)
	if (MDraging)
	{
		x -= HotSpotX;	x += ICONWIDTH/2;
		y -= HotSpotY;	y += ICONHEIGHT/2;
	}

	if (Draging)
	{
		x -= DragHotSpotX;	x += DragIconWidth/2;
		y -= DragHotSpotY;	y += DragIconHeight/2;
	}

// příprava relativních souřadnic bodu v editačním poli
	int sx = (x - DLeft - OLeft + ICONWIDTH)/ICONWIDTH + ILeft;
	int sy = ITop - (y - DTop - OTop + ICONHEIGHT)/ICONHEIGHT;

// souřadnice myši pro zobrazení
	if ((x >= DLeft) && (y >= DTop) &&
		(x < DLeft + DWidth) && (y < DTop + DHeight))
	{
		MouseX = sx;
		MouseY = sy;
	}
	else
	{
		MouseX = MOUSEINV;
		MouseY = MOUSEINV;
	}

// tažení výplně
	if (MFilling)
	{
		MouseX = sx;
		MouseY = sy;
		if (x < DLeft) MouseX = ILeft;
		if (y < DTop) MouseY = ITop;
		if (x >= DLeft + DWidth) MouseX = IRight;
		if (y >= DTop + DHeight) MouseY = IBottom;

		if ((MouseX != DragX) || (MouseY != DragY))
		{
			DispFill();
			DragX = MouseX;
			DragY = MouseY;
			DispFill();
		}
		DispMouseXY();
		return;
	}

// zahájení tažení
	if ((flags & (MK_LBUTTON | MK_RBUTTON)) &&
		!MDraging &&
		!EditMap &&
		BegDrag &&
		(DragX != MOUSEINV) &&
		(DragY != MOUSEINV) &&
		(abs(x - BegX) + abs(y - BegY) >= 3))
	{
		MDraging = TRUE;

// bod styku pro tažení
		HotSpotX = BegX - (DLeft + (DragX*ICONWIDTH - TLeft));
		HotSpotY = BegY - (DTop + DHeight - ((DragY+1)*ICONHEIGHT - TBottom));

// vygenerování tažené ikony
		CBufIcon dragicon;
		dragicon.Add(Icon.Get((Map[Index].DataData())[DragX + DragY*Width].Icon));
		DragImageList = dragicon.GenerList(TRUE, SI_BIG, NULL);

// zapnutí zobrazení obrázku tažení (bez uzamykání okna)
		::ImageList_DragShowNolock(TRUE);

// zapnutí kurzoru pro tažení
		CopyMode = (flags & (MK_RBUTTON));
		if (CopyMode)
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

// načtení souřadnice kurzoru myši
//		POINT pt;
//		::GetCursorPos(&pt);

// zahájení operace tažení, zákaz aktualizace okna
		::ImageList_DragEnter(NULL, MouseScreen.x - HotSpotX, MouseScreen.y - HotSpotY);

// přetažení obrázku na aktuální pozici myši
		::ImageList_DragMove(MouseScreen.x - HotSpotX, MouseScreen.y - HotSpotY);

// zahájení zachytávání myši
		::SetCapture(MainFrame);
	}
	else
	{

// ukončení tažení, pokud bylo tlačítko uvolněno mimo oblast
		if (MDraging && ((flags & (MK_LBUTTON | MK_RBUTTON)) == 0))
		{
			EndDrag();
		}
		else
		{

// pokračování v tažení
			if (MDraging)
			{

// načtení souřadnice kurzoru myši
//				POINT pt;
//				::GetCursorPos(&pt);

// přetažení obrázku na aktuální pozici myši
				::ImageList_DragMove(MouseScreen.x - HotSpotX, MouseScreen.y - HotSpotY);

// nastavení kurzoru myši
				HCURSOR curnew = CurAkt;

				if ((MouseX == MOUSEINV) || (MouseY == MOUSEINV))
				{
					if (CopyMode)
					{
						curnew = CurNoDrag;
					}
					else
					{
						curnew = CurDelete;
					}
				}
				else
				{
					if (CopyMode)
					{
						curnew = CurCopy;
					}
					else
					{
						curnew = CurMove;
					}
				}
						
				if (curnew != CurAkt)
				{
					CurAkt = curnew;
					::SetCursor(CurAkt);
				}
			}
		}
	}

// zobrazení souřadnic myši
	DispMouseXY();
}

/////////////////////////////////////////////////////////////////////////////
// uvolnění tlačítka myši

void OnButtonUp(UINT keys, BOOL right)
{
	if (MFilling)
	{
		EndFill();
		return;
	}

	if (MDraging)
	{
		MAPITEM* data = Map[Index].DataData();
		MAPITEM* olddata = data + (DragX + DragY*Width);
		BOOL modi = FALSE;

		if ((DragX != MouseX) || (DragY != MouseY))
		{
			int olditem = 0;
			DWORD oldparam = 0;

			if ((MouseX != MOUSEINV) && (MouseY != MOUSEINV))
			{
				MAPITEM* newdata = data + (MouseX + MouseY*Width);

				olditem = newdata->Icon;
				oldparam = newdata->Param;

				if ((newdata->Icon != olddata->Icon) ||
					(newdata->Param != olddata->Param))
				{
					PushUndo();
					data = Map[Index].DataData();
					olddata = data + (DragX + DragY*Width);
					newdata = data + (MouseX + MouseY*Width);

//					Map[Index].
						RefIconDec(olditem);
//					Map[Index].
						RefIconInc(olddata->Icon);

					newdata->Icon = olddata->Icon;
					newdata->Param = olddata->Param;
					modi = TRUE;
				}
			}

			if (!CopyMode)
			{
				if ((olddata->Icon != olditem) || 
					(olddata->Param != oldparam))
				{
					if (!modi)
					{
						PushUndo();
						data = Map[Index].DataData();
						olddata = data + (DragX + DragY*Width);
					}

//					Map[Index].
						RefIconDec(olddata->Icon);
//					Map[Index].
						RefIconInc(olditem);

					olddata->Icon = olditem;
					olddata->Param = oldparam;
					modi = TRUE;
				}
			}

			if (modi)
			{
				SetModi();
				ProgAktItem();
			}
		}

		EndDrag();
		Disp();
	}
	else

// navigace ikony (není-li tažení a není-li přerušení druhým tlačítkem)
	{
		if (!EditMap && BegDrag && (MouseX != MOUSEINV) && (MouseY != MOUSEINV))
		{
			int icon = (Map[Index].DataData() + (MouseX + MouseY*Width))->Icon;
			BufObj.Select(BufObj.SrcDat(BufIcoID, icon));
			BufLoc.Select(BufLoc.SrcDat(BufIcoID, icon));
		}
	}

	BegDrag = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// položení ikony zvnějšku

void DragDrop(int icon)
{
// ukončení editace číselného parametru
	EndEditSwcNum();

	int inx = MouseX + MouseY*Width;

	if ((EditMode != BufMapID) ||
		Map.IsNotValid(Index) ||
		Icon.IsNotValid(icon) ||
		((DWORD)MouseX >= (DWORD)Width) ||
		((DWORD)MouseY >= (DWORD)Height) ||
		((Map[Index].DataData())[inx].Icon == icon))
	{
		return;
	}

	PushUndo();

//	Map[Index].
		RefIconDec((Map[Index].DataData())[inx].Icon);
//	Map[Index].
		RefIconInc(icon);

	(Map[Index].DataData())[inx].Icon = icon;

	SetModi();
	ProgAktItem();
	EndDrag();
	Disp();
}


/////////////////////////////////////////////////////////////////////////////
// vstup klávesy z klávesnice (TRUE=obslouženo)

BOOL OnKeyDown(int key)
{
// obsluha editačního pole číselného parametru
	if (EditMapNumWnd)
	{
		switch (key)
		{
		case VK_RETURN:
		case VK_TAB:
		case VK_ESCAPE:
			EndEditSwcNum();
			return TRUE;

		case VK_LEFT:
			BegEditSwcNum(EditNumX-1, EditNumY);
			return TRUE;

		case VK_RIGHT:
			BegEditSwcNum(EditNumX+1, EditNumY);
			return TRUE;

		case VK_UP:
			BegEditSwcNum(EditNumX, EditNumY+1);
			return TRUE;

		case VK_DOWN:
			BegEditSwcNum(EditNumX, EditNumY-1);
			return TRUE;
		}
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// nastavení editačního módu

void SetMode(int mode)
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

// ukončení editace číselného parametru
	EndEditSwcNum();

// kontrola, zda je mód již nastaven
	if (Mode == mode) return;

// nastavení módu
	Mode = mode;

// překreslení displeje
	ReDisp();

// aktualizace voleb
	UpdateMenu();

// aktualizace voleb bloku
	UpdateClipboard();
}


/////////////////////////////////////////////////////////////////////////////
// aktualizace voleb výběru módu

void UpdateMenu()
{
	SelectCombo(ToolEditMap, Mode, ToolBarEditMapTools);
	CheckCommand(IDN_RASTR, Rastr);
	CheckCommand(IDN_FILLMAP, FillMap);
	CheckCommand(IDN_EDITMAP, EditMap);
}


/////////////////////////////////////////////////////////////////////////////
// mód výplně

void OnFillMap()
{
	if (!ProgMode) return;
	EndEditSwcNum();
	FillMap = !FillMap;
	EditMap = FALSE;
	UpdateMenu();
}


/////////////////////////////////////////////////////////////////////////////
// editační mód

void OnEditMap()
{
	if (!ProgMode) return;
	EndEditSwcNum();
	EditMap = !EditMap;
	FillMap = FALSE;
	UpdateMenu();
}


// ****************************** obsluha bloku ******************************

/////////////////////////////////////////////////////////////////////////////
// výběr všeho

void SelectAll()
{
// výběr všeho
	if (EditMapNumWnd)
	{
		::SendMessage(EditMapNumWnd, EM_SETSEL, 0, (LPARAM)-1);
	}

// aktualizace voleb bloku
	UpdateClipboard();
}

/////////////////////////////////////////////////////////////////////////////
// zrušení bloku

void Delete()
{
// operace CLEAR
	if (EditMapNumWnd)
	{
		::SendMessage(EditMapNumWnd, WM_CLEAR, 0, 0);
	}

// aktualizace voleb bloku
	UpdateClipboard();
}

/////////////////////////////////////////////////////////////////////////////
// kopie bloku

void Copy()
{
// operace COPY
	if (EditMapNumWnd)
	{
		::SendMessage(EditMapNumWnd, WM_COPY, 0, 0);
	}

// aktualizace voleb bloku
	UpdateClipboard();
}

/////////////////////////////////////////////////////////////////////////////
// vystřížení bloku

void Cut()
{
// operaceCUT
	if (EditMapNumWnd)
	{
		::SendMessage(EditMapNumWnd, WM_CUT, 0, 0);
	}

// aktualizace voleb bloku
	UpdateClipboard();
}

/////////////////////////////////////////////////////////////////////////////
// navrácení bloku

void Paste()
{
// operace PASTE
	if (EditMapNumWnd)
	{
		::SendMessage(EditMapNumWnd, WM_PASTE, 0, 0);
	}

// aktualizace voleb bloku
	UpdateClipboard();
}


/////////////////////////////////////////////////////////////////////////////
// aktualizace voleb bloku

void UpdateClipboard()
{
	if ((FocusBuf == BufEdiID) && (EditMode == BufMapID))
	{
		if (EditMapNumWnd)
		{
			int start, end;
			::SendMessage(EditMapNumWnd, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
			BOOL sel = (start != end);
			EnableCommand(IDN_CUT, sel);
			EnableCommand(IDN_COPY, sel);
			EnableCommand(IDN_PASTE, ::IsClipboardFormatAvailable(CF_TEXT));
			EnableCommand(IDN_CLEAR, sel);
			EnableCommand(IDN_ALL, ::GetWindowTextLength(EditMapNumWnd) > 0);
		}
		else
		{
			EnableCommand(IDN_CUT, FALSE);
			EnableCommand(IDN_COPY, FALSE);
			EnableCommand(IDN_PASTE, FALSE);
			EnableCommand(IDN_CLEAR, FALSE);
			EnableCommand(IDN_ALL, FALSE);
		}
	}
}


// ****************************** obsluha UNDO ******************************


/////////////////////////////////////////////////////////////////////////////
// úschova plochu pro UNDO

void PushUndo()
{
	Undo.AddMapSet(Index, Map[Index], Mode);
	Map[Index].CopyWrite();
	Undo.Stop();
}

}
