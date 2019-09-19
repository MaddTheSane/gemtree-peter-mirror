
#include "Main.h"

/***************************************************************************\
*																			*
*							Editor ikon a obrázků							*
*																			*
\***************************************************************************/

namespace EditIcon
{

/////////////////////////////////////////////////////////////////////////////
// parametry

// výběr barev
BYTE	ColLeft;						// barva pro levé tlačítko
BYTE	ColRight;						// barva pro pravé tlačítko
BYTE	ColInd = 255;					// barva indikátoru barev (255 = není)

// parametry ikony/obrázku
int		BufID = -1;						// editovaný buffer (-1 = není)
int		Index = -1;						// editovaná ikona v bufferu ikon/obrázků (-2 = sprajt)
int		Width = 32;						// šířka obrázku v bodech
int		Height = 32;					// výška obrázku v bodech
BYTE*	Data = NULL;					// data obrázku

// zadané nové rozměry obrázku (v bodech)
int		NewDimWidth;					// šířka obrázku
int		NewDimHeight;					// výška obrázku
BOOL	NewDimPoint = FALSE;			// rozměry zadány v bodech
BOOL	NewDimFill = FALSE;				// probíhá programové plnění dialogu

// změna velikosti obrázku
int		ResizeOldWidth = 640;			// původní šířka bloku
int		ResizeOldHeight = 480;			// původní výška bloku
int		ResizeWidth = 640;				// zadaná nová šířka
int		ResizeHeight = 480;				// zadaná nová výška
double	ResizeWidthProc = 100;			// nová šířka zadaná v procentech
double	ResizeHeightProc = 100;			// nová výška zadaná v procentech
BOOL	ResizeFill = FALSE;				// probíhá programové plnění dialogu
BOOL	ResizeAktWidth = TRUE;			// je aktivní pole šířky

enum {
	ResizeModeStep = 0,					// rozměry zadány v krocích
	ResizeModePoint,					// rozměry zadány v bodech
	ResizeModeProc,						// rozměry zadány v procentech
};

int		ResizeMode = ResizeModeProc;	// zadaný typ rozměrů
BOOL	ResizeModeProp = TRUE;			// zachovat poměr stran
BOOL	ResizeModeInt = TRUE;			// provádět interpolaci
BOOL	ResizeModeDith = TRUE;			// provádět dithering

// zobrazená část obrázku (v bodech obrázku, bez přepočtu na displej)
int		DispLeft = 0;					// počátek zobrazené části obrázku zleva
int		DispBottom = 0;					// počátek zobrazené části obrázku zdola
int		DispWidth = 32;					// šířka zobrazené části obrázku
int		DispHeight = 32;				// výška zobrazené části obrázku
int		MidX = -1;						// nastavený střed obrázku X
int		MidY = -1;						// nastavený střed obrázku Y
BOOL	Rastr = TRUE;					// zobrazení rastru

// aktualizace zobrazení obrázku
int		AktLeft;						// počátek vlevo
int		AktTop;							// počátek nahoře
int		AktRight;						// konec vpravo
int		AktBottom;						// konec dole

// zobrazení obrázku na displeji (v bodech displeje)
int		DLeft;							// počátek zobrazení vlevo
int		DTop;							// počátek zobrazení nahoře
int		DWidth;							// zobrazená šířka části obrázku na displeji
int		DHeight;						// zobrazená výška části obrázku na displeji
int		Zoom = 1;						// aktuální měřítko obrázku/ikony
int		ZoomIcon = -1;					// měřítko ikony
int		ZoomPic = -1;					// měřítko obrázku
#define MAXZOOM 64						// maximální měřítko (mocnina 2 !!!)

// pole výběru barev
int		ColsX;							// počet barev horizontálně
int		ColsXByte;						// šířka obrázku barev v bajtech
int		ColsY;							// počet barev vertikálně
int		ColsLeft;						// počátek pole barev X
int		ColsTop;						// počátek pole barev Y
int		ColsWidth;						// zobrazená šířka pole barev
int		ColsHeight;						// zobrazená výška pole barev
int		ColsIncX;						// přírustek barev X
int		ColsIncY;						// přírustek barev Y
#define	MINCOLSIR 6						// minimální šířka políčka k zobrazení rámu výběru barvy

// volba fontu
int		FontWeight = FW_NORMAL;			// váha fontu (BOLD)
BOOL	FontItalic = FALSE;				// font typu ITALIC
int		FontSize = 32;					// velikost fontu
CText	FontName;						// jméno fontu
int		FontCharSet = EASTEUROPE_CHARSET; //DEFAULT_CHARSET; // znaková sada
CText	FontText;						// zadaný text

// obsluha kreslení
int		Mode = IDN_PEN;					// editační mód (kód IDN_xxx)
int		OldMode = IDN_PEN;				// uschovaný editační mód (kód IDN_xxx)
int		PenWidth = 1;					// šířka pera
int		PenWidthID = IDN_WIDTH1;		// ID volby pera
BOOL	Editing = FALSE;				// příznak probíhající editování
int		OldX = 0;						// původní souřadnice kreslení X
int		OldY = 0;						// původní souřadnice kreslení Y
int		TopX = 0;						// počáteční souřadnice tažení X
int		TopY = 0;						// počáteční souřadnice tažení Y
BYTE*	PushBuf = NULL;					// buffer k meziúschově obrázku

// obsluha bloku
BOOL	Selecting = FALSE;				// příznak označení bloku
BOOL	Moving = FALSE;					// příznak přesouvání bloku
BYTE*	BlokBuf = NULL;					// buffer s blokem
int		BlokX = 0;						// počátek bloku X
int		BlokY = 0;						// počátek bloku Y
int		BlokWidth = 32;					// šířka bloku
int		BlokHeight = 32;				// výška bloku
BYTE	BlokBack;						// průhledná barva bloku

// první označení bloku
BOOL	CutMode = FALSE;				// výchozí souřadnice bloku
int		FirstBlokX = 0;					// výchozí počátek bloku X
int		FirstBlokY = 0;					// výchozí počátek bloku Y
int		FirstBlokWidth = 0;				// výchozí šířka bloku
int		FirstBlokHeight = 0;			// výchozí výška bloku

// tabulka kódů pro vykreslení tlačítek při editaci bloku
int EditBlokTab[] = {
	ButtonYFlip,		IDN_YFLIP,
	ButtonXFlip,		IDN_XFLIP,
	ButtonLRot,			IDN_LROT,
	ButtonLRot67,		IDN_LROT67,
	ButtonLRot60,		IDN_LROT60,
	ButtonLRot45,		IDN_LROT45,
	ButtonLRot30,		IDN_LROT30,
	ButtonLRot22,		IDN_LROT22,
	ButtonRRot,			IDN_RROT,
	ButtonRRot67,		IDN_RROT67,
	ButtonRRot60,		IDN_RROT60,
	ButtonRRot45,		IDN_RROT45,
	ButtonRRot30,		IDN_RROT30,
	ButtonRRot22,		IDN_RROT22,
	ButtonMul2,			IDN_MUL2,
	ButtonDiv2,			IDN_DIV2,
	ButtonXCol,			IDN_XCOL,
	ButtonSCol,			IDN_SCOL,
	ButtonMulDiv,		IDN_MULDIV,
	-1
};


// ************************** obsluha volby editace *************************


/////////////////////////////////////////////////////////////////////////////
// inicializace při startu programu

void StartInit()
{
// příprava barev
	ColLeft = WhiteCol;					// barva levého tlačítka
	ColRight = BackCol;					// barva pravého tlačítka
	BlokBack = BackCol;					// průhledná barva bloku

// příprava rozměrů obrázku barev
	ColsX = ColCol+1;					// počet barev horizontálně
	ColsXByte = (ColCol+1+3) & ~3;		// délka linky obrázku barev v bajtech
	ColsY = ColLev;						// počet barev vertikálně

// jméno fontu, zadaný text
	FontName = _T("Times New Roman");	// jméno fontu
}


/////////////////////////////////////////////////////////////////////////////
// zahájení editace (musí být platný index!) (-2 = ikona sprajtu)

void BegEdit(int bufID, int index)
{
// nulování přepínačů
	Selecting = FALSE;
	Editing = FALSE;
	Moving = FALSE;

// editace obrázků
	if (bufID == BufPicID)
	{
		ASSERT ((index == -2) || Picture.IsValid(index));

		CPicture* pic;

		if (index == -2)
		{
			EditItemSprite = EditSprite::Index;
			EditItemPic = EditSprite::IndexPic;
			pic = &Sprite[EditItemSprite][EditItemPic];
		}
		else
		{
			pic = &(Picture[index]);	// adresa obrázku
		}

		pic->CopyWrite();			// kopie před zápisem
		pic->DeComp();
		Width = pic->Width();		// šířka obrázku
		Height = pic->Height();		// výška obrázku
		Data = pic->DataData();		// adresa dat obrázku

		if (ZoomPic <= 0)
		{
			ZoomPic = (EditWidth-TRACKSIRKA-4)/Width;
			int z2 = (EditHeight*5/6-TRACKSIRKA-4)/Height;
			if (ZoomPic > z2) ZoomPic = z2;
			if (ZoomPic <= 0) ZoomPic = 1;
			if (ZoomPic > MAXZOOM) ZoomPic = MAXZOOM;
/*	
			z2 = 1;
			while ((2*z2 <= ZoomPic) && (z2 < MAXZOOM))
			{
				z2 *= 2;
			}
			ZoomPic = z2;
*/
		}

		Zoom = ZoomPic;
	}
	else

// editace ikon
	{
		ASSERT(Icon.IsValid(index));

		CIcon* editicon;

		editicon = &(Icon[index]);		// adresa ikony

		editicon->CopyWrite();			// kopie před zápisem
		editicon->DeComp();
		Width = ICONWIDTH;				// šířka ikony
		Height = ICONHEIGHT;			// výška ikony
		Data = editicon->DataData();	// adresa dat ikony

		if (ZoomIcon <= 0)
		{
			ZoomIcon = (EditWidth-TRACKSIRKA-4)/ICONWIDTH;
			int z2 = (EditHeight*5/6-TRACKSIRKA-4)/ICONHEIGHT;
			if (ZoomIcon > z2) ZoomIcon = z2;
			if (ZoomIcon <= 0) ZoomIcon = 1;
			if (ZoomIcon > MAXZOOM) ZoomIcon = MAXZOOM;
/*	
			z2 = 1;
			while ((2*z2 <= ZoomIcon) && (z2 < MAXZOOM))
			{
				z2 *= 2;
			}
			ZoomIcon = z2;
*/
		}

		Zoom = ZoomIcon;
	}

// příprava dat
	BufID = bufID;						// mód editace ikon/obrázku
	Index = index;						// index ikony/obrázku
	PushBuf = (BYTE*)MemSize(PushBuf, Width * Height * sizeof(BYTE));

// střed zvětšení
	if ((DWORD)MidX >= (DWORD)Width) MidX = Width/2;
	if ((DWORD)MidY >= (DWORD)Height) MidY = Height/2;

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

	if (NewDimPoint)
	{
		txt.Int(NewDimWidth);
	}
	else
	{
		txt.Double((double)NewDimWidth/ICONWIDTH);
	}

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

		if (NewDimPoint)
		{
			NewDimWidth = Int(txt);
		}
		else
		{
			NewDimWidth = (int)(Double(txt)*ICONWIDTH + 0.5);
		}
	}
}

// nastavení textu výšky
void DimenDialSetHeight(HWND wnd)
{
	CText txt;

	if (NewDimPoint)
	{
		txt.Int(NewDimHeight);
	}
	else
	{
		txt.Double((double)NewDimHeight/ICONHEIGHT);
	}

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

		if (NewDimPoint)
		{
			NewDimHeight = Int(txt);
		}
		else
		{
			NewDimHeight = (int)(Double(txt)*ICONHEIGHT + 0.5);
		}
	}
}

// nastavení přepínače volby jednotek
void DimenDialSetSwc(HWND wnd)
{
	NewDimFill = TRUE;
	DialogSetCheck(wnd, IDC_KROK, !NewDimPoint);
	DialogSetCheck(wnd, IDC_BOD, NewDimPoint);
	NewDimFill = FALSE;
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

			txt.Load(IDN_PICDIMEN);
			txt.SetWindowTextFont(hWnd);

			DialogCustomText(hWnd, IDN_SIRKA);
			DialogCustomText(hWnd, IDN_VYSKA);
			DialogCustomBox(hWnd, IDN_ROZMERY);
			DialogCustomRadio(hWnd, IDC_KROK);
			DialogCustomRadio(hWnd, IDC_BOD);

			txt.Load(IDN_CANCEL0);
			txt.SetDialogTextFont(hWnd, IDCANCEL);

			DimenDialSetWidth(hWnd);
			DimenDialSetHeight(hWnd);
			DimenDialSetSwc(hWnd);
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

			case IDC_KROK:
			case IDC_BOD:
				{
					BOOL old = NewDimPoint;

					DimenDialGetWidth(hWnd);
					DimenDialGetHeight(hWnd);

					NewDimPoint = (LOWORD(wParam) == IDC_BOD);
					DimenDialSetSwc(hWnd);

					if (old != NewDimPoint)
					{
						DimenDialSetWidth(hWnd);
						DimenDialSetHeight(hWnd);
					}
				}
				break;
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
// nastavení rozměrů obrázku

void OnDimen()
{
// kontrola režimu
	if (BufID != BufPicID) return;

// příprava výchozích rozměrů
	NewDimWidth = Width;
	NewDimHeight = Height;

// zadání nových rozměrů obrázku
	if (::DialogBoxParam(
			hInstance,
			MAKEINTRESOURCE(IDN_PICDIMEN),
			MainFrame,
			(DLGPROC)DimenDialogProc,
			(LPARAM)IDN_PICDIMEN) != IDOK) return;

// korekce zadaných údajů
	if (NewDimWidth < 1) NewDimWidth = Width;
	if (NewDimWidth > 0x8000) NewDimWidth = 0x8000;
	if (NewDimHeight < 1) NewDimHeight = Height;
	if (NewDimHeight > 0x8000) NewDimHeight = 0x8000;

// kontrola, zda byly rozměry změněny
	if ((NewDimWidth == Width) && (NewDimHeight == Height))
	{
		return;
	}

// zapnutí kurzoru čekání
	BeginWaitCursor();

// vypnutí označení bloku
	DispSelect();

// zrušení módu bloku
	if ((Mode == IDN_TEXT) || (Mode == IDN_SELECT))
	{
		SetMode(OldMode);
	}

// nulování přepínačů
	Selecting = FALSE;
	Editing = FALSE;
	Moving = FALSE;

// nastavení nových rozměrů obrázku
	Width = NewDimWidth;
	Height = NewDimHeight;

	if (Index == -2)
	{
		Undo.AddSprSet(EditItemSprite, Sprite[EditItemSprite]);
		Sprite[EditItemSprite].CopyWrite();

		for (int ii = Sprite[EditItemSprite].Faze()*Sprite[EditItemSprite].Smer()-1; ii >= 0; ii--)
		{
//			Sprite[EditItemSprite][ii].CopyWrite();
			Sprite[EditItemSprite][ii].Resize(Width, Height);
		}
		Sprite[EditItemSprite][EditItemPic].DeComp();
		Data = Sprite[EditItemSprite][EditItemPic].DataData();

		Undo.Stop();
	}
	else
	{
		PushUndo();

		ASSERT(Picture.IsValid(Index));
		Picture[Index].Resize(Width, Height);
		Picture[Index].DeComp();
		Data = Picture[Index].DataData();
	}

	PushBuf = (BYTE*)MemSize(PushBuf, Width * Height * sizeof(BYTE));
	if ((DWORD)MidX >= (DWORD)Width) MidX = Width/2;
	if ((DWORD)MidY >= (DWORD)Height) MidY = Height/2;

// přepočet zobrazení
	ReCalc();
	DispLeft = MidX - DispWidth/2;
	DispBottom = MidY - DispHeight/2;
	ReCalc();
	SetScroll();
	ProgOnSize();

// překreslení displeje
	Disp();

// příznak modifikace
	SetModi();

// aktualizace voleb bloku
	UpdateClipboard();

// aktualizace zobrazení stromů
	UpdateTree();

// vypnutí kurzoru čekání
	EndWaitCursor();
}


// ***************************** obsluha zobrazení **************************


/////////////////////////////////////////////////////////////////////////////
// přepočet rozměrů

void ReCalc()
{
// výpočet horizontálních parametrů pole pro výběr barev
	ColsIncX = (EditWidth - 4) / ColsX;		// přírustek barev X
	if (ColsIncX < 1) ColsIncX = 1;			// musí být přírustek min. 1
	ColsWidth = ColsIncX * ColsX;				// zobrazená šířka pole barev
	ColsLeft = EditX + (EditWidth - ColsWidth)/2; // levý okraj pole barev

// výpočet vertikálních parametrů pole pro výběr barev
	ColsIncY = (EditHeight - 4) / 6 / ColsY;	// přírustek barev Y
	if (ColsIncY < 1) ColsIncY = 1;			// musí být přírustek min. 1
	ColsHeight = ColsIncY * ColsY;				// zobrazená šířka pole barev
	ColsTop = EditY + EditHeight - 2 - ColsHeight; // horní okraj pole barev

// příprava zobrazené výšky (korekce, je-li horizontální posuvník)
	DHeight = ColsTop - EditY - 2 - 1 - 4;		// výška k zobrazení
	if (EditWidth - 4 - TRACKSIRKA < Width*Zoom)
	{
		DHeight -= TRACKSIRKA;				// snížený výšky o horizontální posuvník
	}
	if (DHeight < 1) DHeight = 1;			// minimální šířka

// příprava zobrazené šířky (korekce, je-li vertikální posuvník)
	DWidth = EditWidth - 4 - 1 - 4;			//	šířka k zobrazení
	if (DHeight < Height*Zoom)
	{
		DWidth -= TRACKSIRKA;				// snížení šířky o vertikální posuvník
	}
	if (DWidth < 1) DWidth = 1;				// minimální šířka

// počet zobrazených bodů
	DispWidth = DWidth/Zoom;				// počet zobrazených bodů horizontálně
	if (DispWidth < 1) DispWidth = 1;		// minimální počet bodů horizontálně
	if (DispWidth > Width) DispWidth = Width; // omezení na skutečnou šířku
	DispHeight = DHeight/Zoom;			// počet zobrazených bodů vertikálně
	if (DispHeight < 1) DispHeight = 1;		// minimální počet bodů vertikálně
	if (DispHeight > Height) DispHeight = Height; // omezení na skutečnou výšku

// omezení počátku zobrazení
	if (DispLeft < 0) DispLeft = 0;
	if (DispLeft > Width - DispWidth) 
		DispLeft = Width - DispWidth;
	if (DispBottom < 0) DispBottom = 0;
	if (DispBottom > Height - DispHeight)
		DispBottom = Height - DispHeight;

// upřesnění údajů k zobrazení
	DLeft = EditX + 2 + 2 + (DWidth - Zoom*DispWidth)/2;
//	if (DispWidth < Width) DLeft = EditX + 2;
	DTop = EditY + 2 + 2 + (DHeight - Zoom*DispHeight)/2;
//	if (DispHeight < Height) DTop = EditY + 2;
	DWidth = Zoom*DispWidth;
	DHeight = Zoom*DispHeight;

// příznak zapnutí posuvníků
	HScrollDisp = (DispWidth < Width);
	VScrollDisp = (DispHeight < Height);
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

// vykreslení obrázku
	AktLeft = 0;						// počátek vlevo
	AktRight = Width;					// konec vpravo
	AktTop = 0;							// počátek nahoře
	AktBottom = Height;					// konec dole
	DispAkt(dc);				// překreslení obrázku

// zobrazení rámu kolem obrázku
	DispRam(dc);

// zobrazení barev
	for (int col = StdColors-1; col >= 0; col--)
	{
		DispCol(dc, col);
	}

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
	DispAkt(dc);				// překreslení obrázku

// zobrazení rámu kolem obrázku
	DispRam(dc);

// návrat původních palet
	::SelectPalette(dc,OldPal,FALSE);

// uvolnění kontextu displeje
	::ReleaseDC(MainFrame, dc);
}


/////////////////////////////////////////////////////////////////////////////
// překreslení okrajů kolem obrázku

void DispRam(HDC dc)
{
// rozměry k zobrazení podkladu
	int dleft = DLeft;
	int dright = dleft + DWidth;
	int dtop = DTop;
	int dbottom = dtop + DHeight;

// test, zda je zapnuto zobrazení rastru
	if (Rastr)
	{

// příprava a výběr pera k zobrazení okraje
		HPEN pen = ::CreatePen(PS_SOLID, 1, 0x0000ff);
		HPEN oldpen = (HPEN)::SelectObject(dc, pen);

// zobrazení horního okraje
		if ((Zoom == 1) && (DispBottom + DispHeight == Height) && (dtop > EditY + 2))
		{
			dtop--;
			::MoveToEx(dc, dleft, dtop, NULL);
			::LineTo(dc, dright, dtop);
		}

// zobrazení pravého okraje
		if ((Zoom == 1) && (DispLeft + DispWidth == Width) && (dright < EditX + EditWidth - 2))
		{
			::MoveToEx(dc, dright, dtop, NULL);
			::LineTo(dc, dright, dbottom);
			dright++;
		}

// zobrazení levého okraje
		BOOL lleft = ((DispLeft == 0) && (dleft > EditX + 2));
		if (lleft)
		{
			dleft--;
			::MoveToEx(dc, dleft, dtop, NULL);
			::LineTo(dc, dleft, dbottom);
		}

// zobrazení spodního okraje
		BOOL lbottom = ((DispBottom == 0) && (dbottom < EditY + EditHeight - 2));
		if (lbottom)
		{
			::MoveToEx(dc, dleft, dbottom, NULL);
			::LineTo(dc, dright, dbottom);
			dbottom++;
		}

// návrat a zrušení pera k zobrazení okraje
		::SelectObject(dc, oldpen);
		::DeleteObject(pen);

// test, zda bude modrý nebo černý okraj
		if (Zoom > 1)
		{

// příprava a výběr pera k zobrazení modrěho okraje
			pen = ::CreatePen(PS_SOLID, 1, 0xff0000);
			oldpen = (HPEN)::SelectObject(dc, pen);

// zobrazení levého okraje
			if (!lleft && (DispLeft % 32 == 0) && (dleft > EditX + 2))
			{
				lleft = TRUE;
				dleft--;
				::MoveToEx(dc, dleft, dtop, NULL);
				::LineTo(dc, dleft, dbottom);
			}

// zobrazení dolního okraje
			if (!lbottom && (DispBottom % 32 == 0) && (dbottom < EditY + EditHeight - 2))
			{
				lbottom = TRUE;
				::MoveToEx(dc, dleft, dbottom, NULL);
				::LineTo(dc, dright, dbottom);
				dbottom++;
			}

// návrat a zrušení pera k zobrazení okraje
			::SelectObject(dc, oldpen);
			::DeleteObject(pen);

// test, zda bude černý okraj
			if (Zoom >= 8)
			{

// příprava a výběr pera k zobrazení černého okraje
				pen = ::CreatePen(PS_SOLID, 1, 0);
				oldpen = (HPEN)::SelectObject(dc, pen);

// zobrazení levého okraje
				if (!lleft && (dleft > EditX + 2))
				{
					dleft--;
					::MoveToEx(dc, dleft, dtop, NULL);
					::LineTo(dc, dleft, dbottom);
				}

// zobrazení dolního okraje
				if (!lbottom && (dbottom < EditY + EditHeight - 2))
				{
					::MoveToEx(dc, dleft, dbottom, NULL);
					::LineTo(dc, dright, dbottom);
					dbottom++;
				}

// návrat a zrušení pera k zobrazení okraje
				::SelectObject(dc, oldpen);
				::DeleteObject(pen);
			}
		}
	}

// příprava štětce k vymazání podkladu
	HBRUSH brush = (HBRUSH)::GetStockObject(LTGRAY_BRUSH); // štětec k vymazání plochy

// vymazání plochy nahoře nad obrázkem
	RECT rc;
	rc.left = EditX + 2;
	rc.right = EditX + EditWidth - 2;
	rc.top = EditY + 2;
	rc.bottom = dtop;
	if (rc.top < rc.bottom)
	{
		::FillRect(dc, &rc, brush);
	}

// vymazání plochy dole pod obrázkem (left a right je nastaveno)
	rc.top = dbottom;
	rc.bottom = ColsTop;
	if (rc.top < rc.bottom)
	{
		::FillRect(dc, &rc, brush);
	}

// vymazání plochy vlevo od obrázku (left je nastaveno)
	rc.right = dleft;
	rc.top = dtop;
	rc.bottom = dbottom;
	if (rc.left < rc.right)
	{
		::FillRect(dc, &rc, brush);
	}

// vymazání plochy vpravo od obrázku (top a bottom je nastaveno)
	rc.left = dright;
	rc.right = EditX + EditWidth - 2;
	if (rc.left < rc.right)
	{
		::FillRect(dc, &rc, brush);
	}

// vymazání plochy vlevo od pole barev
	rc.left = EditX + 2;
	rc.right = ColsLeft;
	rc.top = ColsTop;
	rc.bottom = EditY + EditHeight - 2;
	if (rc.left < rc.right)
	{
		::FillRect(dc, &rc, brush);
	}

// vymazání plochy vpravo od pole barev (top a bottom je nastaveno)
	rc.left = ColsLeft + ColsWidth;
	rc.right = EditX + EditWidth - 2;
	if (rc.left < rc.right)
	{
		::FillRect(dc, &rc, brush);
	}

// zrušení štětce podkladu (i když podle dokumentace rušení není nutné)
	::DeleteObject(brush);
}


/////////////////////////////////////////////////////////////////////////////
// aktualizační překreslení obrázku

void DispAkt(HDC dc)
{
// omezení hranic k zobrazení
	if (AktLeft < DispLeft) AktLeft = DispLeft;
	if (AktTop < (Height - DispBottom - DispHeight))
		AktTop = Height - DispBottom - DispHeight;
	if (AktRight > (DispLeft + DispWidth))
		AktRight = DispLeft + DispWidth;
	if (AktBottom > Height - DispBottom)
		AktBottom = Height - DispBottom;

// příprava zdrojových souřadnic k zobrazení
	int srcLeft = AktLeft;
	int srcWidth = AktRight - srcLeft;

// !!!!!!!!!!!! doplňková ochrana (havárie při UNDO po změně velikosti obrázků sprajtu)
	if (srcWidth > Width) srcWidth = Width;

	int srcTop = AktTop;
	int srcHeight = AktBottom - srcTop;

// !!!!!!!!!!!! doplňková ochrana (havárie při UNDO po změně velikosti obrázků sprajtu)
	if (srcHeight > Height) srcHeight = Height;

// zneplatnění hranic k zobrazení
	AktLeft = Width;
	AktRight = 0;
	AktTop = Height;
	AktBottom = 0;

// kontrola, zda je co zobrazit
	if ((srcWidth <= 0) || (srcHeight <= 0)) return;

// příprava cílových souřadnic k zobrazení
	int dstLeft = DLeft + (srcLeft - DispLeft)*Zoom;
	int dstTop = DTop + (srcTop - (Height - DispBottom - DispHeight))*Zoom;
	int dstWidth = srcWidth*Zoom;
	int dstHeight = srcHeight*Zoom;
	int dstWidthByte = (dstWidth + 3) & ~3;

// adresa dat k zobrazení
	if (BufID == BufPicID)
	{
		if (Index == -2)
		{
			Sprite[EditItemSprite][EditItemPic].DeComp();

// !!!!!!!!!!!! doplňková ochrana (havárie při UNDO po změně velikosti obrázků sprajtu)
			if ((Sprite[EditItemSprite][EditItemPic].Width() != Width) ||
				(Sprite[EditItemSprite][EditItemPic].Height() != Height)) return;

			Data = Sprite[EditItemSprite][EditItemPic].DataData();
		}
		else
		{
			Picture[Index].DeComp();

// !!!!!!!!!!!! doplňková ochrana (havárie při UNDO po změně velikosti obrázků sprajtu)
			if ((Picture[Index].Width() != Width) || (Picture[Index].Height() != Height)) return;

			Data = Picture[Index].DataData();
		}
	}
	else
	{
		Icon[Index].DeComp();
		Data = Icon[Index].DataData();
	}

	BYTE* data = Data + (Height - srcTop - srcHeight)*Width;

// vypnutí výběru bloku
	DispSelect();

// přednastavení parametrů záhlaví bitmapy
	StdBitmapInfo->bmiHeader.biWidth = dstWidth;
	StdBitmapInfo->bmiHeader.biHeight = dstHeight;

// vytvoření bufferu k dekódování zobrazení
	BYTE* buf;
	buf = (BYTE*)MemGet(dstWidthByte * dstHeight);
	BYTE* src = data + srcLeft;
	BYTE* dst = buf;
	BYTE col = BlackCol;

	dstWidthByte -= dstWidth;
	int i, i2, j;

// dekódování obrázku
	if ((Zoom == 1) && (dstWidthByte == 0) && (Width == dstWidth))
	{
		MemCopy(dst, src, srcWidth * srcHeight);
	}
	else
	{
		for (i = srcHeight; i > 0; i--)
		{
			switch (Zoom)
			{
			case 1:
				MemCopy(dst, src, srcWidth);
				dst += dstWidthByte + dstWidth;
				break;

			case 2:
				for (i2 = 2; i2 > 0; i2--)
				{
					for (j = srcWidth; j > 0; j--)
					{
						MemFill(dst, 2, *src);
						src++;
						dst += 2;
					}
					dst += dstWidthByte;
					src -= srcWidth;
				}
				break;

			case 3:
				for (i2 = 3; i2 > 0; i2--)
				{
					for (j = srcWidth; j > 0; j--)
					{
						MemFill(dst, 3, *src);
						src++;
						dst += 3;
					}
					dst += dstWidthByte;
					src -= srcWidth;
				}
				break;

			case 4:
				for (i2 = 4; i2 > 0; i2--)
				{
					for (j = srcWidth; j > 0; j--)
					{
						MemFill(dst, 4, *src);
						src++;
						dst += 4;
					}
					dst += dstWidthByte;
					src -= srcWidth;
				}
				break;

			case 5:
				for (i2 = 5; i2 > 0; i2--)
				{
					for (j = srcWidth; j > 0; j--)
					{
						MemFill(dst, 5, *src);
						src++;
						dst += 5;
					}
					dst += dstWidthByte;
					src -= srcWidth;
				}
				break;

			case 6:
				for (i2 = 6; i2 > 0; i2--)
				{
					for (j = srcWidth; j > 0; j--)
					{
						MemFill(dst, 6, *src);
						src++;
						dst += 6;
					}
					dst += dstWidthByte;
					src -= srcWidth;
				}
				break;

			case 7:
				for (i2 = 7; i2 > 0; i2--)
				{
					for (j = srcWidth; j > 0; j--)
					{
						MemFill(dst, 7, *src);
						src++;
						dst += 7;
					}
					dst += dstWidthByte;
					src -= srcWidth;
				}
				break;

			case 8:
				i2 = 8;
				if (Rastr) i2--;

				for (; i2 > 0; i2--)
				{
					for (j = srcWidth; j > 0; j--)
					{
						MemFill(dst, 8, *src);
						src++;
						dst += 8;
						if (Rastr)
						{
							dst[-1] = col;
						}
					}
					dst += dstWidthByte;
					src -= srcWidth;
				}

				if (Rastr)
				{
					MemFill(dst, dstWidth, col);
					dst += dstWidth + dstWidthByte;
				}
				break;
/*
			case 16:
				i2 = 16;
				if (Rastr) i2--;

				for (; i2 > 0; i2--)
				{
					for (j = srcWidth; j > 0; j--)
					{
						MemFill(dst, 16, *src);
						src++;
						dst += 16;

						if(Rastr)
						{
							dst[-1] = col;
						}
					}
					dst += dstWidthByte;
					src -= srcWidth;
				}

				if (Rastr)
				{
					MemFill(dst, dstWidth, col);
					dst += dstWidth + dstWidthByte;
				}
				break;
*/
			default:
				i2 = Zoom;
				if (Rastr) i2--;

				for (; i2 > 0; i2--)
				{
					for (j = srcWidth; j > 0; j--)
					{
// !!!!!!!!!!!! zde nastává havárie při UNDO po změně velikosti obrázků sprajtu (neplatná adresa src, Zoom=13)
						MemFill(dst, Zoom, *src);
						src++;
						dst += Zoom;
						if (Rastr)
						{
							dst[-1] = col;
						}
					}
					dst += dstWidthByte;
					src -= srcWidth;
				}

				if (Rastr)
				{
					MemFill(dst, dstWidth, col);
					dst += dstWidth + dstWidthByte;
				}
			}
			src += Width;
		}
	}

// dekódování vertikálních hrubých čar
	if (Rastr)
	{
		dstWidthByte += dstWidth;
		col = BlueCol;
		int dstwidth = dstWidth;

		for (i = (ICONWIDTH-1) - (srcLeft % ICONWIDTH); i < srcWidth; i += ICONWIDTH)
		{
			if ((i == srcWidth - 1) && (srcLeft + srcWidth == Width)) 
			{
				col = RedCol;
				dstwidth--;
			}

			dst = buf + (i+1)*Zoom - 1;
			for (j = dstHeight; j > 0; j--)
			{
				*dst = col;
				dst += dstWidthByte;
			}
		}

// dekódování horizontálních hrubých čar
		col = BlueCol;

		i = ICONHEIGHT - (Height - srcTop - srcHeight) % ICONHEIGHT;
		for (; i <= srcHeight; i += ICONHEIGHT)
		{
			if ((i == srcHeight) && (srcTop == 0)) col = RedCol;

			dst = buf + (i*Zoom-1)*dstWidthByte;
			MemFill(dst, dstwidth, col);
		}
	}

// zobrazení obrázku z bufferu
	::StretchDIBits(dc, dstLeft, dstTop, dstWidth, dstHeight,
		0, 0, dstWidth, dstHeight, buf,
		StdBitmapInfo, DIB_RGB_COLORS, SRCCOPY);

// zrušení bufferu k dekódování zobrazení
	MemFree(buf);

// zapnutí výběru bloku
	DispSelect();
}


/////////////////////////////////////////////////////////////////////////////
// vykreslení políčka výběru barvy (s vlastním kontextem zařízení)

void DispCol(int col)
{
// otevření kontextu displeje
	HDC dc = ::GetDC(MainFrame);

// nastavení vlastních palet
	HPALETTE OldPal;					// úschova původních palet
	OldPal = ::SelectPalette(dc,		// výběr vlastních palet
		StdPalette, FALSE);
	::RealizePalette(dc);				// realizace palet

// zobrazení barvy
	DispCol(dc, col);

// návrat původních palet
	::SelectPalette(dc,OldPal,FALSE);

// uvolnění kontextu displeje
	::ReleaseDC(MainFrame, dc);
}


/////////////////////////////////////////////////////////////////////////////
// vykreslení políčka výběru barvy

void DispCol(HDC dc, int col)
{
// zablokování nepovolené barvy (např. -1)
	if ((DWORD)col >= (DWORD)StdColors) return;

// přednastavení souřadnic pro podkladovou barvu nebo barvu stínu
	RECT rc;
	rc.left = ColsLeft;
	rc.top = ColsTop;
	if (col == ShadCol) rc.top += ColsHeight/2;
	rc.bottom = rc.top + ColsHeight/2;

// výpočet souřadnic pro jiné barvy než podkladová
	if ((col != BackCol) && (col != ShadCol))
	{
		rc.top += ((col-ResCols) % ColLev) * ColsIncY;
		rc.left += ((col-ResCols) / ColLev + 1) * ColsIncX;
		rc.bottom = rc.top + ColsIncY;
	}
	rc.right = rc.left + ColsIncX;

// úschova souřadnic rámu
	RECT rc2;
	rc2 = rc;

// kontrola, zda se zobrazí rám
	if ((ColsIncX >= MINCOLSIR) && (ColsIncY >= MINCOLSIR))
	{

// zobrazení rámu kolem políčka
		::DrawEdge(dc, &rc, EDGE_SUNKEN, BF_RECT);

// korekce řámu políčka pro barvu
		rc.left += 2;
		rc.top += 2;
		rc.right -= 2;
		rc.bottom -= 2;
	}

// zobrazení barvy políčka
	HBRUSH brush = ::CreateSolidBrush(PALETTEINDEX((WORD)col));
	::FillRect(dc, &rc, brush);
	::DeleteObject(brush);

// zobrazení rámečku indikátoru aktivní barvy
	if ((col == ColInd) && (rc.left < rc.right) && (rc.top < rc.bottom))
	{
		::DrawEdge(dc, &rc2, EDGE_RAISED, BF_RECT);
	}

// zobrazení rámečku indikátoru levého tlačítka
	if (col == ColLeft)
	{
		rc2.left += ColsIncX/6;
		rc2.top += ColsIncY/6;
		rc2.right -= ColsIncX/6;
		rc2.bottom -= ColsIncY/6;
		::DrawEdge(dc, &rc2, EDGE_BUMP, BF_RECT);
	}

// zobrazení rámečku indikátoru pravého tlačítka
	if (col == ColRight)
	{
		rc2.left = rc.left + ColsIncX/4;
		rc2.top = rc.top + ColsIncY/4;
		rc2.right = rc.right - ColsIncX/4;
		rc2.bottom = rc.bottom - ColsIncY/4;
		::DrawEdge(dc, &rc2, EDGE_BUMP, BF_RECT);
	}
}


/////////////////////////////////////////////////////////////////////////////
// zvětšení měřítka

void ZoomIn()
{
	if (Zoom < MAXZOOM)
	{
		Zoom++;

		switch (BufID)
		{
		case BufIcoID:
			ZoomIcon = Zoom;
			break;

		case BufPicID:
			ZoomPic = Zoom;
			break;
		}

		ReCalc();
		DispLeft = MidX - DispWidth/2;
		DispBottom = MidY - DispHeight/2;
		ReCalc();
		SetScroll();
		ProgOnSize();
		Disp();
		UpdateZoomInOut();
	}
}

/////////////////////////////////////////////////////////////////////////////
// zmenšení měřítka

void ZoomOut()
{
	if (Zoom > 1)
	{
		Zoom--;

		switch (BufID)
		{
		case BufIcoID:
			ZoomIcon = Zoom;
			break;

		case BufPicID:
			ZoomPic = Zoom;
			break;
		}

		ReCalc();
		DispLeft = MidX - DispWidth/2;
		DispBottom = MidY - DispHeight/2;
		ReCalc();
		SetScroll();
		ProgOnSize();
		Disp();
		UpdateZoomInOut();
	}
}


/////////////////////////////////////////////////////////////////////////////
// aktualizace tlačítek pro zvětšení/zmenšení měřítka

void UpdateZoomInOut()
{
	if (FocusBuf == BufEdiID)
	{
		EnableCommand(IDN_IN, Zoom < MAXZOOM);
		EnableCommand(IDN_OUT, Zoom > 1);
	}
}


/////////////////////////////////////////////////////////////////////////////
// překreslení ikon ve stromech

void UpdateTree()
{
// kontrola, zda je buffer ikon
	if ((BufID != BufIcoID) || (Index < 0)) return;

// aktualizace zobrazení ikon
	for (int buf = 0; buf < PROGBUFNUM; buf++)
	{
		BufProg[buf].ReDispIcon(Index);
	}
}


/////////////////////////////////////////////////////////////////////////////
// zapnutí/vypnutí rastru

void OnRastr()
{
	Rastr = !Rastr;
	UpdateMenu();
	Disp();
}


// ********************* obsluha posuvníků **********************************


/////////////////////////////////////////////////////////////////////////////
// přesun posuvníků při změně rozměrů okna

HDWP MoveScroll(HDWP hdwp)
{
// lokální proměnné
	int x, y, w, h;

// přepočet zobrazení
	ReCalc();

// přesun a zobrazení horizontálního posuvníku
	x = EditX + 2;
	y = ColsTop - TRACKSIRKA;
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
	h = ColsTop - EditY - 2;
	if (HScrollDisp) h -= TRACKSIRKA;
	hdwp = ::DeferWindowPos(hdwp, VScroll, NULL, x, y, w, h,
			SWP_DRAWFRAME | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER |
			((VScrollDisp && !PreviewPic) ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));

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
	if (si.nMax != Width)
	{
		si.nMax = Width;
		si.fMask |= SIF_RANGE;
	}

// velikost stránky
	if (si.nPage != (UINT)DispWidth)
	{
		si.nPage = (UINT)DispWidth;
		si.fMask |= SIF_PAGE;
	}

// pozice posuvníku
	if (si.nPos != DispLeft)
	{
		si.nPos = DispLeft;
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
	if (si.nMax != Height)
	{
		si.nMax = Height;
		si.fMask |= SIF_RANGE;
	}

// velikost stránky
	if (si.nPage != (UINT)DispHeight)
	{
		si.nPage = (UINT)DispHeight;
		si.fMask |= SIF_PAGE;
	}

// pozice posuvníku
	if (si.nPos != Height - DispHeight - DispBottom)
	{
		si.nPos = Height - DispHeight - DispBottom;
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
// horizontální posuvník

void OnHScroll(int code, int pos)
{
	switch (code)
	{
	case SB_LEFT:
		pos = 0;
		break;

	case SB_LINELEFT:
		pos -= DispWidth/20+1;
		break;

	case SB_LINERIGHT:
		pos += DispWidth/20+1;
		break;

	case SB_PAGELEFT:
		pos -= DispWidth-1;
		break;

	case SB_PAGERIGHT:
		pos += DispWidth-1;
		break;

	case SB_RIGHT:
		pos = Width - DispWidth;
		break;
	}

	if (pos < 0) pos = 0;
	if (pos >= Width - DispWidth)
		pos = Width - DispWidth;

	if (pos != DispLeft)
	{
		DispLeft = pos;
		MidX = DispLeft + DispWidth/2;
		MidY = DispBottom + DispHeight/2;
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
		pos -= DispHeight/20+1;
		break;

	case SB_LINEDOWN:
		pos += DispHeight/20+1;
		break;

	case SB_PAGEUP:
		pos -= DispHeight-1;
		break;

	case SB_PAGEDOWN:
		pos += DispHeight-1;
		break;

	case SB_BOTTOM:
		pos = Height - DispHeight;
		break;
	}

	if (pos < 0) pos = 0;
	if (pos >= Height - DispHeight)
		pos = Height - DispHeight;
	pos = Height - DispHeight - pos;

	if (pos != DispBottom)
	{
		DispBottom = pos;
		MidX = DispLeft + DispWidth/2;
		MidY = DispBottom + DispHeight/2;
		Disp();
	}
}


// **************************** obsluha editace *****************************


/////////////////////////////////////////////////////////////////////////////
// úschova obrázku do pomocného bufferu

void Push()
{
	if (BufID == BufPicID)
	{
		if (Index == -2)
		{
			Sprite[EditItemSprite][EditItemPic].DeComp();
			Data = Sprite[EditItemSprite][EditItemPic].DataData();
		}
		else
		{
			Picture[Index].DeComp();
			Data = Picture[Index].DataData();
		}
	}
	else
	{
		Icon[Index].DeComp();
		Data = Icon[Index].DataData();
	}

	PushBuf = (BYTE*)MemSize(PushBuf, Width * Height * sizeof(BYTE));
	MemCopy(PushBuf, Data, Width * Height);
}

/////////////////////////////////////////////////////////////////////////////
// návrat obrázku z pomocného bufferu

void Pop()
{
	if (BufID == BufPicID)
	{
		if (Index == -2)
		{
			Sprite[EditItemSprite][EditItemPic].DeComp();
			Data = Sprite[EditItemSprite][EditItemPic].DataData();
		}
		else
		{
			Picture[Index].DeComp();
			Data = Picture[Index].DataData();
		}
	}
	else
	{
		Icon[Index].DeComp();
		Data = Icon[Index].DataData();
	}


	MemCopy(Data, PushBuf, Width * Height);
	AktLeft = 0;
	AktTop = 0;
	AktRight = Width;
	AktBottom = Height;
}


/////////////////////////////////////////////////////////////////////////////
// načtení bodu z bufferu (nekontroluje souřadnice !)

inline BYTE _fastcall GetPush(int x, int y)
{
	return PushBuf[Width * (Height-1-y) + x];
}


/////////////////////////////////////////////////////////////////////////////
// nastavení bodu v bufferu (nekontroluje souřadnice !)

inline void _fastcall SetPush(int x, int y, BYTE col)
{
	PushBuf[Width * (Height-1-y) + x] = col;
}


/////////////////////////////////////////////////////////////////////////////
// nastavení indikátoru barvy

void SetColInd(BYTE col)
{
	BYTE oldcol = ColInd;

	if (col == oldcol) return;

	ColInd = col;

	DispCol(oldcol);
	DispCol(col);
}


/////////////////////////////////////////////////////////////////////////////
// načtení bodu obrázku (nekontroluje souřadnice !)

inline BYTE _fastcall GetPoint(int x, int y)
{
	return Data[Width*(Height-1-y) + x];
}


/////////////////////////////////////////////////////////////////////////////
// obsluha zobrazení kurzoru myši (TRUE=obslouženo)

BOOL OnSetCursor(int x, int y)
{
// pole volby barvy
	if ((x >= ColsLeft) &&
		(x < ColsLeft + ColsWidth) &&
		(y >= ColsTop) &&
		(y < ColsTop + ColsHeight) &&
		!Editing)
	{
		CurAkt = CurKapatko;
		::SetCursor(CurAkt);
		return TRUE;
	}

// pole obrázku
	if (((x >= DLeft) &&
		(x < DLeft + DWidth) &&
		(y >= DTop) &&
		(y < DTop + DHeight)) ||
		Editing)
	{
		switch(Mode)
		{
		case IDN_LINE:		CurAkt = CurLine;
							break;

		case IDN_FILLBOX:	CurAkt = CurFillRect;
							break;

		case IDN_BOX:		CurAkt = CurRect;
							break;

		case IDN_FILLCIRCLE: CurAkt = CurFillRound;
							break;

		case IDN_CIRCLE:	CurAkt = CurRound;
							break;

		case IDN_FILLELLIPSE: CurAkt = CurFillElip;
							break;

		case IDN_ELLIPSE:	CurAkt = CurElip;
							break;

		case IDN_SPHERE:	CurAkt = CurKoule;
							break;

		case IDN_PIPE:		CurAkt = CurKapatko;
							break;

		case IDN_FILL:		CurAkt = CurFill;
							break;

		case IDN_SPRAY:		CurAkt = CurSpray;
							break;

		case IDN_TEXT:
		case IDN_SELECT:	
							if (Selecting &&
								TestBlok((x - DLeft)/Zoom + DispLeft,
										(y - DTop)/Zoom + (Height - DispHeight - DispBottom)) &&
								(!Editing || Moving))
							{
								if (Moving)
								{
									CurAkt = CurRuka;
								}
								else
								{
									CurAkt = CurSelectMove;
								}
							}
							else
							{
								CurAkt = CurSelect;
							}
							break;

		default:			if (PenWidth > 7)
							{
								CurAkt = CurPaint;
							}
							else
							{
								CurAkt = CurPen;
							}
		}
		::SetCursor(CurAkt);
		return TRUE;
	}

// implicitní obsluha
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// stisk tlačítka myši (TRUE = obslouženo)

BOOL OnButtonDown(UINT flags, int x, int y, BOOL right, BOOL dclick)
{

	if (BufID == BufPicID)
	{
		if (Index == -2)
		{
			Sprite[EditItemSprite][EditItemPic].DeComp();
			Data = Sprite[EditItemSprite][EditItemPic].DataData();
		}
		else
		{
			Picture[Index].DeComp();
			Data = Picture[Index].DataData();
		}
	}
	else
	{
		Icon[Index].DeComp();
		Data = Icon[Index].DataData();
	}


// příprava adresy proměnné barvy
	BYTE col;							// zvolená aktivní barva
	BYTE* colbut;						// proměnná s aktivní barvou

	if (right)
	{
		colbut = &ColRight;				// barva pro pravé tlačítko
	}
	else
	{
		colbut = &ColLeft;				// barva pro levé tlačítko
	}

	BYTE oldcol = *colbut;				// původní barva

// volba barvy
	if ((x >= ColsLeft) &&
		(x < ColsLeft + ColsWidth) &&
		(y >= ColsTop) &&
		(y < ColsTop + ColsHeight))
	{
		if (x < (ColsLeft + ColsIncX))
		{
			col = (BYTE)((y - ColsTop)/(ColsHeight/2));
		}
		else
		{
			col = (BYTE)((x - ColsLeft - ColsIncX)/ColsIncX * ColLev + (y - ColsTop)/ColsIncY + ResCols);
		}
		ASSERT(col < (BYTE)StdColors);
		if (col != oldcol)
		{
			*colbut = col;
			DispCol(oldcol);
			DispCol(col);

// změna pozadí označeného bloku při změně barvy pravého tlačítka
			if (right && (Mode == IDN_SELECT) && Selecting && CutMode)
			{
				MoveSelect();
				ReDisp();
			}

// překreslení textu při změně barvy levého tlačítka
			if ((Mode == IDN_TEXT) && !right)
			{
				ReDispText();				// zobrazení textu
			}
		}

// v módu kapátka návrat předešlého módu
		if (Mode == IDN_PIPE)
		{
			SetMode(OldMode);
		}
		return TRUE;
	}

// příprava relativních souřadnic bodu v editačním poli
	int sx = (x - DLeft)/Zoom + DispLeft;
	int sy = (y - DTop)/Zoom + (Height - DispHeight - DispBottom);

// test, zda je editační pole
	if ((x >= DLeft) &&
		(y >= DTop) &&
		(sx < DispLeft + DispWidth) &&
		(sy < Height - DispBottom) &&
		(Data))
	{

// větvení podle editačního módu
		switch (Mode)
		{

// mód pera, čáry, obdélníku, kružnice a elipsy
		case IDN_PEN:
		case IDN_LINE:
		case IDN_BOX:
		case IDN_CIRCLE:
		case IDN_ELLIPSE:
			PushUndo();				// úschova obrázku do UNDO bufferu
			Editing = TRUE;					// příznak zahájení editace
			Push();					// úschova do pomocného bufferu
			OldX = sx;						// úschova souřadnce X
			OldY = sy;						// úschova souřadnice Y
			TopX = sx;						// úschova souřadnice X
			TopY = sy;						// úschova souřadnice Y
			SetPaint(sx, sy, *colbut);		// vykreslení výchozího bodu
			SetModi();						// nastavení příznaku modifikace
			break;

// mód vyplněného obdélníku, kruhu a oválu - neuplatní se šířka čáry
		case IDN_FILLBOX:
		case IDN_FILLCIRCLE:
		case IDN_FILLELLIPSE:
		case IDN_SPHERE:
			PushUndo();				// úschova obrázku do UNDO bufferu
			Editing = TRUE;					// příznak zahájení editace
			Push();					// úschova do pomocného bufferu
			OldX = sx;						// úschova souřadnce X
			OldY = sy;						// úschova souřadnice Y
			TopX = sx;						// úschova souřadnice X
			TopY = sy;						// úschova souřadnice Y
			SetPoint(sx, sy, *colbut);		// vykreslení výchozího bodu
			if (sx < AktLeft) AktLeft = sx;
			if (sx >= AktRight) AktRight = sx+1;
			if (sy < AktTop) AktTop = sy;
			if (sy >= AktBottom) AktBottom = sy+1;
			SetModi();						// nastavení příznaku modifikace
			break;

// mód spreje
		case IDN_SPRAY:
			PushUndo();				// úschova obrázku do UNDO bufferu
			Editing = TRUE;					// příznak zahájení editace
			OldX = sx;						// úschova souřadnice X
			OldY = sy;						// úschova souřadnice Y
			SetSpray(sx, sy, *colbut);		// vykreslení výchozího bodu
			SetModi();						// nastavení příznaku modifikace
			break;

// mód kapátka
		case IDN_PIPE:
			col = GetPoint(sx, sy);
			oldcol = *colbut;
			if (col != oldcol)
			{
				*colbut = col;
				DispCol(oldcol);
				DispCol(col);
			}
			SetMode(OldMode);
			break;

// mód výplně
		case IDN_FILL:
			col = GetPoint(sx, sy);
			if (col != *colbut)
			{
				PushUndo();
				SetFill(sx, sy, *colbut, col);
				SetModi();
				Disp();
				UpdateTree();
			}
			break;

// mód výběru a textu
		case IDN_TEXT:
		case IDN_SELECT:

// pravé tlačítko vypne označení bloku (jinak pokračuje stejně jako levé)
			if (right && Selecting)
			{
				DispSelect();				// vypnutí zobrazení výběru bloku
				Moving = FALSE;				// konec posunu bloku
				Selecting = FALSE;			// zrušení příznaku výběru bloku
				Editing = FALSE;			// zrušení příznaku editace
				if (Mode == IDN_TEXT)
				{
					SetMode(OldMode);
					return TRUE;
				}
				break;
			}

// zahájení přesunu bloku, je-li kurzor uvnitř bloku
			if (Selecting)					// je již blok označen?
			{
				if (TestBlok(sx, sy))		// je kurzor v bloku?
				{
					Editing = TRUE;			// příznak zahájení editace
					Moving = TRUE;			// příznak přesunu bloku
					OldX = sx;				// úschova souřadnice X
					OldY = sy;				// úschova souřadnice Y
					::SetCursor(CurRuka);	// zapnutí kurzoru pro přesun
					SetModi();				// příznak modifikace programu
				}

// vně bloku se blok vypne (a zahájí se nové označování)
				else
				{
					DispSelect();			// vypnutí označení bloku
					Selecting = FALSE;		// zrušení příznaku výběru bloku

					if (Mode == IDN_TEXT)
					{
						SetMode(OldMode);
						return TRUE;
					}
				}
			}

// zahájení označování bloku
			if (!Selecting)			// není blok označen?
			{
				PushUndo();			// úschova do UNDO bufferu
				Editing = TRUE;				// příznak zahájení editace
				Push();				// úschova do pomocného bufferu
				Selecting = TRUE;			// příznak označení bloku
				Moving = FALSE;				// není přesun bloku
				OldX = sx;					// úschova souřadnice X
				OldY = sy;					// úschova souřadnice Y
				TopX = sx;					// počáteční souřadnice X označeného bloku
				TopY = sy;					// počáteční souřadnice Y označeného bloku
				BlokX = sx;					// počátek bloku X
				BlokY = sy;					// počátek bloku Y
				BlokWidth = 1;				// šířka bloku
				BlokHeight = 1;				// výška bloku
				BlokBuf = (BYTE*)MemSize(BlokBuf, 1*sizeof(BYTE));			// buffer pro uchování 1 bodu
				SetBlok(0, 0, GetPoint(sx, sy)); // úschova prvního bodu
				CutMode = !right;			// příznak, zda jsou údaje k vymazání
				FirstBlokX = sx;			// výchozí počátek bloku X
				FirstBlokY = sy;			// výchozí počátek bloku Y
				FirstBlokWidth = 1;			// výchozí šířka bloku
				FirstBlokHeight = 1;		// výchozí výška bloku
				BlokBack = ColRight;		// průhledná barva bloku
				DispSelect();				// zapnutí označení bloku
			}
			break;
		}

// překreslení displeje
		ReDisp();

// aktualizace voleb bloku
		UpdateClipboard();

		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// posun myši

void OnMouseMove(UINT flags, int x, int y)
{
	if (BufID == BufPicID)
	{
		if (Index == -2)
		{
			Sprite[EditItemSprite][EditItemPic].DeComp();
			Data = Sprite[EditItemSprite][EditItemPic].DataData();
		}
		else
		{
			Picture[Index].DeComp();
			Data = Picture[Index].DataData();
		}
	}
	else
	{
		Icon[Index].DeComp();
		Data = Icon[Index].DataData();
	}


// příprava relativních souřadnic bodu v editačním poli
	int sx = (x - DLeft)/Zoom + DispLeft;
	int sy = (y - DTop)/Zoom + (Height - DispHeight - DispBottom);

// při editaci se souřadnice omezí na editační pole
	if (Editing)
	{
		if (x < DLeft)
		{
			x = DLeft;
			sx = DispLeft;
		}

		if (y < DTop)
		{
			y = DTop;
			sy = Height - DispHeight - DispBottom;
		}

		if ((DWORD)sx >= (DWORD)Width)
		{
			sx = Width - 1;
		}

		if ((DWORD)sy >= (DWORD)Height)
		{
			sy = Height - 1;
		}
	}

// souřadnice myši pro zobrazení
	MouseX = sx;
	MouseY = Height - 1 - sy;

// test, zda je editační pole
	if ((x >= DLeft) &&
		(y >= DTop) &&
		(sx < DispLeft + DispWidth) &&
		(sy < Height - DispBottom) &&
		(Data))
	{

// test, zda je mód editace a je stisknuto některé tlačítko
		if ((flags & (MK_LBUTTON | MK_RBUTTON)) && Editing)
		{

// příprava barvy ke kreslení
			BYTE col;
			if (flags & MK_LBUTTON)
			{
				col = ColLeft;
			}
			else
			{
				col = ColRight;
			}

// větvení podle editačního módu
			if ((OldX != sx) || (OldY != sy))
			{
				switch (Mode)
				{

// kreslení perem
				case IDN_PEN:
					SetLinePaint(OldX, OldY, sx, sy, col);
					break;

// kreslení čáry
				case IDN_LINE:
					Pop();
					SetLinePaint(TopX, TopY, sx, sy, col);
					break;

// kreslení sprejem
				case IDN_SPRAY:
					SetSpray(sx, sy, col);
					break;

// kreslení obdélníku
				case IDN_BOX:
					Pop();
					SetRect(TopX, TopY, sx, sy, col);
					break;

// kreslení obdélníku s výplní
				case IDN_FILLBOX:
					Pop();
					SetRectFill(TopX, TopY, sx, sy, col);
					break;

// kreslení kružnice
				case IDN_CIRCLE:
					Pop();
					SetRound(TopX, TopY, sx, sy, col);
					break;

// kreslení kruhu
				case IDN_FILLCIRCLE:
					Pop();
					SetRoundFill(TopX, TopY, sx, sy, col);
					break;

// kreslení elipsy
				case IDN_ELLIPSE:
					Pop();
					SetElip(TopX, TopY, sx, sy, col);
					break;

// kreslení oválu
				case IDN_FILLELLIPSE:
					Pop();
					SetElipFill(TopX, TopY, sx, sy, col);
					break;

// kreslení koule
				case IDN_SPHERE:
					Pop();
					SetKoule(TopX, TopY, sx, sy, col);
					break;

// výběr bloku
				case IDN_TEXT:
				case IDN_SELECT:
					if (Selecting)		// je blok označen?
					{
						DispSelect();

						if (Moving)
						{
							BlokX += sx - OldX;
							BlokY += sy - OldY;
							MoveSelect();
							DispSelect();
							SetModi();
						}
						else
						{
							if (sx >= TopX)
							{
								BlokX = TopX;
								BlokWidth = sx - TopX + 1;
							}
							else
							{
								BlokX = sx;
								BlokWidth = TopX - sx + 1;
							}

							if (sy >= TopY)
							{
								BlokY = TopY;
								BlokHeight = sy - TopY + 1;
							}
							else
							{
								BlokY = sy;
								BlokHeight = TopY - sy + 1;
							}

							DispSelect();
			
							BlokBuf = (BYTE*)MemSize(BlokBuf, BlokWidth*BlokHeight * sizeof(BYTE));

							for (int bloki = 0; bloki < BlokWidth; bloki++)
							{
								for (int blokj = 0; blokj < BlokHeight; blokj++)
								{
									SetBlok(bloki, blokj, GetPoint(BlokX + bloki, BlokY + blokj));
								}
							}

							BlokBack = ColRight;
							FirstBlokX = BlokX;
							FirstBlokY = BlokY;
							FirstBlokWidth = BlokWidth;
							FirstBlokHeight = BlokHeight;
						}
					}
				}
			}

// úschova souřadnic
			OldX = sx;
			OldY = sy;

// vypnutí indikace aktuální barvy
			SetColInd(255);
		}

// není mód editace - zobrazení barvy pod kurzorem
		else
		{
			if (Editing)
			{
				UpdateTree();
			}
			
			Editing = FALSE;
			Moving = FALSE;
			SetColInd(GetPoint(sx, sy));
		}
	}

// kurzor myši mimo editační pole - vypnutí indikace barvy
	else
	{
		SetColInd(255);
		MouseX = MOUSEINV;
		MouseY = MOUSEINV;
	}

// překreslení displeje
	ReDisp();

// zobrazení souřadnic myši
	if (((DWORD)MouseX >= (DWORD)Width) || ((DWORD)MouseY >= (DWORD)Height))
	{
		MouseX = MOUSEINV;
		MouseY = MOUSEINV;
	}
	DispMouseXY();
}


/////////////////////////////////////////////////////////////////////////////
// uvolnění tlačítka myši

void OnButtonUp(UINT keys, BOOL right)
{
	if (Editing)
	{
		UpdateTree();
	}

// v módu přesunu bloku navrácení kurzoru
	if (Moving)
	{
		::SetCursor(CurSelectMove);
	}
	Editing = FALSE;
	Moving = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// vstup znaku z klávesnice (TRUE=obslouženo)

BOOL OnChar(TCHAR znak)
{
	switch (znak)
	{
	case 8:
	case 127:
		if (Mode == IDN_TEXT)
		{
			if (FontText.IsNotEmpty())
			{
				FontText.Delete(FontText.Length() - 1);
				ReDispText();
			}
			return TRUE;
		}
		break;

	default:
		if (Mode == IDN_TEXT)
		{
			if ((DWORD)znak >= (DWORD)32)
			{
				FontText.Add(znak);
				ReDispText();
			}
			return TRUE;
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// vstup klávesy z klávesnice (TRUE=obslouženo)

BOOL OnKeyDown(int key)
{
	switch (key)
	{
// DELETE - zrušení znaku
	case VK_DELETE:
		if (Mode == IDN_TEXT)
		{
			if (FontText.IsNotEmpty())
			{
				FontText.Delete(FontText.Length()-1);
				ReDispText();
			}
			return TRUE;
		}
		break;

// ENTER - ukončení zadání textu
	case VK_RETURN:
		if (Mode == IDN_TEXT)
		{
			SetMode(OldMode);
			return TRUE;
		}

		if (Mode == IDN_SELECT)
		{
			DispSelect();
			Selecting = FALSE;
			return TRUE;
		}
		break;

// ESC - zrušení zadání textu
	case VK_ESCAPE:
		if (Mode == IDN_TEXT)
		{
			Delete();
			SetMode(OldMode);
			return TRUE;
		}

		if ((Mode == IDN_SELECT) && Selecting)
		{
			if (CutMode)
			{
				BlokX = FirstBlokX;
				BlokY = FirstBlokY;
				MoveSelect();
				DispSelect();
				Selecting = FALSE;
				Disp();
			}
			else
			{
				Delete();
			}
			return TRUE;
		}
		break;
	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// nastavení editačního módu

void SetMode(int mode)
{
// kontrola, zda je režim editace
	if (!ProgMode ||
		((EditMode != BufIcoID) && (EditMode != BufPicID)))
		return;

// volba fontu při zapnutí módu textu
	if(mode == IDN_TEXT)
	{
		FontText.Empty();

// příprava parametrů fontu
		LOGFONT	lf;										// parametry fontu
		lf.lfHeight = FontSize;							// velikost fontu
		lf.lfWidth = 0;									// šířka fontu
		lf.lfEscapement = 0;							// únikový vektor
		lf.lfOrientation = 0;							// orientace
		lf.lfWeight = FontWeight;						// váha fontu (BOLD)
		lf.lfItalic = (BYTE)FontItalic;					// kurzíva
		lf.lfCharSet = (BYTE)FontCharSet;				// znaková sada
		lf.lfOutPrecision = OUT_DEFAULT_PRECIS;			// přesnost výstupu
		lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;			// přesnost oblasti
		lf.lfQuality = DEFAULT_QUALITY;					// kvalita fontu
		lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE; // rozteč fontů

		int txtlen = FontName.Length();						// délka fontu
		if (txtlen > (LF_FACESIZE-2)) txtlen = (LF_FACESIZE-2);	// omezení délky jména fontu
		MemCopy(lf.lfFaceName, (LPCTSTR)FontName, txtlen); // přenesení jména fontu
		lf.lfFaceName[txtlen] = 0;						// označení konce textu

// příprava parametrů dialogu
		CHOOSEFONT cf;
		cf.lStructSize = sizeof(LOGFONT);
		cf.hwndOwner = MainFrame;
		cf.lpLogFont = &lf;
		cf.iPointSize = FontSize;
		cf.Flags = CF_NOVERTFONTS | CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
		cf.hInstance = hInstance;
		cf.nFontType = 0;
		if (FontWeight > ((FW_NORMAL + FW_BOLD)/2)) { cf.nFontType |= BOLD_FONTTYPE; }
		if (FontItalic) { cf.nFontType |= ITALIC_FONTTYPE; }

// volba fontu (při přerušení se nic nestane)
		if (!::ChooseFont(&cf))
		{
			return;
		}

// načtení zvolených parametrů fontu
		FontWeight = lf.lfWeight;					// váha fontu
		FontItalic = lf.lfItalic;					// příznak ITALIC
		FontName = lf.lfFaceName;					// jméno fontu
		FontSize = lf.lfHeight;
		FontCharSet = lf.lfCharSet;

// aktualizace fontu, je-li již mód textu
		if (Mode == IDN_TEXT)
		{
			ReDispText();				// zobrazení textu
			return;
		}
	}

// nastavení šířky pera
	switch (mode)
	{
	case IDN_WIDTH1: 
		PenWidth = 1;
		PenWidthID = mode;
		UpdateMenu();
		return;

	case IDN_WIDTH2: 
		PenWidth = 2;
		PenWidthID = mode;
		UpdateMenu();
		return;

	case IDN_WIDTH3: 
		PenWidth = 5;
		PenWidthID = mode;
		UpdateMenu();
		return;

	case IDN_WIDTH4: 
		PenWidth = 9;
		PenWidthID = mode;
		UpdateMenu();
		return;

	case IDN_WIDTH5: 
		PenWidth = 21;
		PenWidthID = mode;
		UpdateMenu();
		return;
	}

// editační příkazy
	if (mode == IDN_EDITBLOK)
	{
		EditBlok(-1);
		return;
	}

// vypnutí označení bloku
	DispSelect();
	Selecting = FALSE;

// kontrola, zda je změna
	if (mode == Mode) return;

// úschova starého módu
	if (((mode == IDN_PIPE) ||
		(mode == IDN_TEXT) ||
		(mode == IDN_SELECT)) &&
		(Mode != IDN_PIPE) &&
		(Mode != IDN_TEXT) &&
		(Mode != IDN_SELECT))
	{
		OldMode = Mode;
	}

// úschova nového módu
	Mode = mode;

// v módu textu zapnutí bloku
	if (mode == IDN_TEXT)
	{
		PushUndo();					// úschova obrázku do undo bufferu
		Push();						// úschova do pomocného bufferu
		Selecting = TRUE;			// příznak označení bloku
		BlokWidth = 10;				// šířka bloku
		BlokHeight = abs(FontSize)+5; // výška bloku
		CutMode = FALSE;			// nejsou údaje k vymazání
		BlokBack = ColRight;		// průhledná barva bloku
		DispSelect();				// zapnutí označení bloku

		BlokBuf = (BYTE*)MemSize(BlokBuf, BlokWidth * BlokHeight * sizeof(BYTE));	// buffer pro blok

		for (int x = 0; x < BlokWidth; x++)
		{
			for (int y = 0; y < BlokHeight; y++)
			{
				SetBlok(x, y, BlokBack);
			}
		}
		ReDispText();				// zobrazení textu
	}

// aktualizace volby módu
	UpdateMenu();

// aktualizace voleb bloku
	UpdateClipboard();
}

/////////////////////////////////////////////////////////////////////////////
// editace bloku

// callback funkce dialogu
BOOL CALLBACK BlokDialogProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_INITDIALOG:
		{
			CenterWindow(hWnd, MainFrame);
			CText txt;

			txt.Load(IDN_PICBLOK);
			txt.SetWindowTextFont(hWnd);

			DialogCustomBox(hWnd, IDN_PREVRACENI);
			DialogCustomText(hWnd, IDN_YFLIP_TXT);
			DialogCustomText(hWnd, IDN_XFLIP_TXT);
			DialogCustomBox(hWnd, IDN_OTOCENI);
			DialogCustomText(hWnd, IDN_LROT_TXT);
			DialogCustomText(hWnd, IDN_RROT_TXT);
			DialogCustomText(hWnd, IDN_LROT67_TXT);
			DialogCustomText(hWnd, IDN_RROT67_TXT);
			DialogCustomText(hWnd, IDN_LROT60_TXT);
			DialogCustomText(hWnd, IDN_RROT60_TXT);
			DialogCustomText(hWnd, IDN_LROT45_TXT);
			DialogCustomText(hWnd, IDN_RROT45_TXT);
			DialogCustomText(hWnd, IDN_LROT30_TXT);
			DialogCustomText(hWnd, IDN_RROT30_TXT);
			DialogCustomText(hWnd, IDN_LROT22_TXT);
			DialogCustomText(hWnd, IDN_RROT22_TXT);
			DialogCustomBox(hWnd, IDN_ZVETSENI);
			DialogCustomText(hWnd, IDN_MUL2_TXT);
			DialogCustomText(hWnd, IDN_DIV2_TXT);
			DialogCustomText(hWnd, IDN_MULDIV_TXT);
			DialogCustomBox(hWnd, IDN_BARVY);
			DialogCustomText(hWnd, IDN_XCOL_TXT);
			DialogCustomText(hWnd, IDN_SCOL_TXT);

			txt.Load(IDN_CANCEL0);
			txt.SetDialogTextFont(hWnd, IDCANCEL);

			for (int* tab = EditBlokTab; *tab >= 0; tab += 2)
			{
				::SetWindowPos(::GetDlgItem(hWnd, tab[1]), NULL, 0, 0, 33,29, 
					SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
			}
		}
		return TRUE;

	case WM_COMMAND:
		switch (HIWORD(wParam))
		{
		case 0:
		case 1:
			switch (LOWORD(wParam))
			{
			case IDN_YFLIP:
			case IDN_XFLIP:
			case IDN_LROT:
			case IDN_LROT67:
			case IDN_LROT60:
			case IDN_LROT45:
			case IDN_LROT30:
			case IDN_LROT22:
			case IDN_RROT:
			case IDN_RROT67:
			case IDN_RROT60:
			case IDN_RROT45:
			case IDN_RROT30:
			case IDN_RROT22:
			case IDN_MUL2:
			case IDN_DIV2:
			case IDN_MULDIV:
			case IDN_XCOL:
			case IDN_SCOL:
			case IDOK:
			case IDCANCEL:
				::EndDialog(hWnd, LOWORD(wParam));
				return TRUE;
			}
			break;
		}
		break;

	case WM_DRAWITEM:
		{
			DRAWITEMSTRUCT* di = (DRAWITEMSTRUCT*)lParam;
			int id = wParam;

			if ((id == IDCANCEL) || (di->CtlType == ODT_STATIC) ||
				(::GetWindowLong(di->hwndItem, GWL_STYLE) & BS_BOTTOM))
			{
				if (DialogDraw(hWnd, lParam)) return TRUE;
			}


			DWORD state = di->itemState;
			HDC dc = di->hDC;
			RECT* rc = &di->rcItem;
			int* tab = EditBlokTab;

			int i;
			for (i = 0; tab[2*i] >= 0; i++)
			{
				if (id == tab[2*i+1])
				{
					break;
				}
			}
			i = tab[2*i];

			HDC dc2 = ::CreateCompatibleDC(dc);
			::SelectObject(dc2, ToolBarBmp);

			int x = (rc->left + rc->right)/2-8;
			int y = (rc->top + rc->bottom)/2-8;

			if (state & ODS_SELECTED)
			{
				::DrawFrameControl(dc, rc, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_PUSHED);
				if (i >= 0)
				{
					::BitBlt(dc, x+1, y+1, 16, 16, dc2, 16*i, 0, SRCCOPY);
				}
			}
			else
			{
				::DrawFrameControl(dc, rc, DFC_BUTTON, DFCS_BUTTONPUSH);
				if (i >= 0)
				{
					::BitBlt(dc, x, y, 16, 16, dc2, 16*i, 0, SRCCOPY);
				}
			}
			::DeleteDC(dc2);

			if (state & ODS_FOCUS)
			{
				::SendMessage(hWnd, DM_SETDEFID, di->CtlID, 0);

				rc->left += 3;
				rc->top += 3;
				rc->right -= 4;
				rc->bottom -= 4;
				::DrawFocusRect(dc, rc);
			}
		}
		return TRUE;



	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// změna velikosti bloku

// aktualizace závislých parametrů
void ResizeAktText()
{
	if (ResizeWidth < 1) ResizeWidth = 1;
	if (ResizeWidth > 0x8000) ResizeWidth = 0x8000;

	if (ResizeHeight < 1) ResizeHeight = 1;
	if (ResizeHeight > 0x8000) ResizeHeight = 0x8000;

	if (ResizeModeProp)
	{
		if (ResizeAktWidth)
		{
			ResizeHeight = Round((double)ResizeWidth/ResizeOldWidth*ResizeOldHeight);
		}
		else
		{
			ResizeWidth = Round((double)ResizeHeight/ResizeOldHeight*ResizeOldWidth);
		}
	}
}

// nastavení rozměrů (all = včetně aktivních)
void ResizeSetText(HWND wnd, BOOL all)
{
	ResizeFill = TRUE;

	CText txt;

	BOOL widthall = (all || (ResizeModeProp && !ResizeAktWidth));
	BOOL heightall = (all || (ResizeModeProp && ResizeAktWidth));

	if (widthall || (ResizeMode != ResizeModeStep))
	{
		txt.Double((double)Round((double)ResizeWidth/ICONWIDTH*100)/100);
		txt.SetDialogText(wnd, IDN_PICRESIZE_STEPX);
	}

	if (heightall || (ResizeMode != ResizeModeStep))
	{
		txt.Double((double)Round((double)ResizeHeight/ICONHEIGHT*100)/100);
		txt.SetDialogText(wnd, IDN_PICRESIZE_STEPY);
	}

	if (widthall || (ResizeMode != ResizeModePoint))
	{
		txt.Int(ResizeWidth);
		txt.SetDialogText(wnd, IDN_PICRESIZE_POINTX);
	}

	if (heightall || (ResizeMode != ResizeModePoint))
	{
		txt.Int(ResizeHeight);
		txt.SetDialogText(wnd, IDN_PICRESIZE_POINTY);
	}

	if (widthall || (ResizeMode != ResizeModeProc))
	{
		if ((ResizeMode == ResizeModeProc) && ResizeModeProp && !ResizeAktWidth)
		{
			txt.Double(ResizeHeightProc);
		}
		else
		{
			if ((ResizeMode == ResizeModeProc) && widthall)
			{
				txt.Double(ResizeWidthProc);
			}
			else
			{
				txt.Double((double)Round((double)ResizeWidth/ResizeOldWidth*10000)/100);
			}
		}
		txt.SetDialogText(wnd, IDN_PICRESIZE_PROCX);
	}

	if (heightall || (ResizeMode != ResizeModeProc))
	{
		if ((ResizeMode == ResizeModeProc) && ResizeModeProp && ResizeAktWidth)
		{
			txt.Double(ResizeWidthProc);
		}
		else
		{
			if ((ResizeMode == ResizeModeProc) && heightall)
			{
				txt.Double(ResizeHeightProc);
			}
			else
			{
				txt.Double((double)Round((double)ResizeHeight/ResizeOldHeight*10000)/100);
			}
		}
		txt.SetDialogText(wnd, IDN_PICRESIZE_PROCY);
	}

	ResizeFill = FALSE;
}

// načtení zadaných rozměrů
void ResizeGetText(HWND wnd)
{
	if (!ResizeFill)
	{
		CText txt;

		switch(ResizeMode)
		{
		case ResizeModeStep:
			if (!ResizeModeProp || ResizeAktWidth)
			{
				txt.GetDialogText(wnd, IDN_PICRESIZE_STEPX);
				ResizeWidth = Round(Double(txt)*ICONWIDTH);
			}

			if (!ResizeModeProp || !ResizeAktWidth)
			{
				txt.GetDialogText(wnd, IDN_PICRESIZE_STEPY);
				ResizeHeight = Round(Double(txt)*ICONHEIGHT);
			}
			break;

		case ResizeModePoint:
			if (!ResizeModeProp || ResizeAktWidth)
			{
				txt.GetDialogText(wnd, IDN_PICRESIZE_POINTX);
				ResizeWidth = Int(txt);
			}

			if (!ResizeModeProp || !ResizeAktWidth)
			{
				txt.GetDialogText(wnd, IDN_PICRESIZE_POINTY);
				ResizeHeight = Int(txt);
			}
			break;

		case ResizeModeProc:
			if (!ResizeModeProp || ResizeAktWidth)
			{
				txt.GetDialogText(wnd, IDN_PICRESIZE_PROCX);
				ResizeWidthProc = Double(txt);
				if (ResizeModeProp) ResizeHeightProc = ResizeWidthProc;
				ResizeWidth = Round(ResizeWidthProc/100*ResizeOldWidth);
			}

			if (!ResizeModeProp || !ResizeAktWidth)
			{
				txt.GetDialogText(wnd, IDN_PICRESIZE_PROCY);
				ResizeHeightProc = Double(txt);
				if (ResizeModeProp) ResizeWidthProc = ResizeHeightProc;
				ResizeHeight = Round(ResizeHeightProc/100*ResizeOldHeight);
			}
			break;
		}

		ResizeAktText();
	}
}

// nastavení přepínačů
void ResizeSetSwc(HWND wnd)
{
	ResizeFill = TRUE;

	DialogSetCheck(wnd, IDN_PICRESIZE_STEP, (ResizeMode == ResizeModeStep));
	DialogSetCheck(wnd, IDN_PICRESIZE_POINT, (ResizeMode == ResizeModePoint));
	DialogSetCheck(wnd, IDN_PICRESIZE_PROC, (ResizeMode == ResizeModeProc));

	DialogSetCheck(wnd, IDN_PICRESIZE_PROP, ResizeModeProp);
	DialogSetCheck(wnd, IDN_PICRESIZE_INT, ResizeModeInt);
	DialogSetCheck(wnd, IDN_PICRESIZE_DITH, ResizeModeDith);

	ResizeFill = FALSE;
}

// aktualizace fokusu
void ResizeAktFocus(HWND wnd, BOOL aktwidth)
{
	int focus;

	switch(ResizeMode)
	{
	case ResizeModeStep:
		focus = IDN_PICRESIZE_STEPX;
		if (!aktwidth) focus = IDN_PICRESIZE_STEPY;
		break;

	case ResizeModePoint:
		focus = IDN_PICRESIZE_POINTX;
		if (!aktwidth) focus = IDN_PICRESIZE_POINTY;
		break;

	default:
		focus = IDN_PICRESIZE_PROCX;
		if (!aktwidth) focus = IDN_PICRESIZE_PROCY;
	}
	::SetFocus(::GetDlgItem(wnd, focus));
}


// callback funkce dialogu
BOOL CALLBACK ResizeDialogProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_INITDIALOG:
		{
			CenterWindow(hWnd, MainFrame);

			CText txt;

			txt.Load(IDN_PICRESIZE);
			txt.SetWindowTextFont(hWnd);

			DialogCustomText(hWnd, IDN_SOUCASNE_ROZMERY);
			DialogCustomText(hWnd, IDN_KROKU);
			DialogCustomText(hWnd, IDN_BODU);
			DialogCustomBox(hWnd, IDN_NOVE_ROZMERY);
			DialogCustomText(hWnd, IDN_SIRKA2);
			DialogCustomText(hWnd, IDN_VYSKA2);
			DialogCustomRadio(hWnd, IDN_PICRESIZE_STEP);
			DialogCustomRadio(hWnd, IDN_PICRESIZE_POINT);
			DialogCustomRadio(hWnd, IDN_PICRESIZE_PROC);
			DialogCustomCheck(hWnd, IDN_PICRESIZE_PROP);
			DialogCustomCheck(hWnd, IDN_PICRESIZE_INT);
			DialogCustomCheck(hWnd, IDN_PICRESIZE_DITH);

			txt.Load(IDN_CANCEL0);
			txt.SetDialogTextFont(hWnd, IDCANCEL);

			ResizeSetSwc(hWnd);

			txt.Int(ResizeOldWidth);
			txt += _T(" x ");
			txt.AddInt(ResizeOldHeight);
			txt.SetDialogTextFont(hWnd, IDN_PICRESIZE_DIMPOINT);

			txt.Double((double)Round((double)ResizeOldWidth/ICONWIDTH*100)/100);
			txt += _T(" x ");
			txt.AddDouble((double)Round((double)ResizeOldHeight/ICONHEIGHT*100)/100);
			txt.SetDialogTextFont(hWnd, IDN_PICRESIZE_DIMSTEP);

			if (ResizeMode == ResizeModeProc)
			{
				ResizeWidth = Round(ResizeWidthProc/100*ResizeOldWidth);
				ResizeHeight = Round(ResizeHeightProc/100*ResizeOldHeight);
			}
			else
			{
				ResizeWidthProc = (double)Round((double)ResizeWidth/ResizeOldWidth*10000)/100;
				ResizeHeightProc = (double)Round((double)ResizeHeight/ResizeOldHeight*10000)/100;
			}

			ResizeAktText();
			ResizeSetSwc(hWnd);
			ResizeSetText(hWnd, TRUE);
			ResizeAktFocus(hWnd, ResizeAktWidth);
		}
		return FALSE;

	case WM_DRAWITEM:				// 0x02B
		if (DialogDraw(hWnd, lParam)) return TRUE;
		return FALSE;

	case WM_SETFOCUS:
	case WM_COMMAND:
		switch (HIWORD(wParam))
		{
		case 0:
		case 1:
			switch (LOWORD(wParam))
			{
			case IDOK:
			case IDCANCEL:
				::EndDialog(hWnd, LOWORD(wParam));
				return TRUE;

			case IDN_PICRESIZE_STEP:
				ResizeMode = ResizeModeStep;
				goto picresize;

			case IDN_PICRESIZE_POINT:
				ResizeMode = ResizeModePoint;
				goto picresize;

			case IDN_PICRESIZE_PROC:
				ResizeMode = ResizeModeProc;
				goto picresize;

			case IDN_PICRESIZE_PROP:
				ResizeModeProp = !ResizeModeProp;
				goto picresize;

			case IDN_PICRESIZE_INT:
				ResizeModeInt = !ResizeModeInt;
				goto picresize;

			case IDN_PICRESIZE_DITH:
				ResizeModeDith = !ResizeModeDith;

picresize:
				if (!ResizeFill)
				{
					ResizeAktText();
					ResizeSetSwc(hWnd);
					ResizeSetText(hWnd, TRUE);
				}
				break;
			}
			break;

		case EN_CHANGE:
			if (!ResizeFill)
			{
				switch (LOWORD(wParam))
				{
				case IDN_PICRESIZE_STEPX:
					ResizeMode = ResizeModeStep;
					ResizeAktWidth = TRUE;
					break;

				case IDN_PICRESIZE_STEPY:
					ResizeMode = ResizeModeStep;
					ResizeAktWidth = FALSE;
					break;

				case IDN_PICRESIZE_POINTX:
					ResizeMode = ResizeModePoint;
					ResizeAktWidth = TRUE;
					break;

				case IDN_PICRESIZE_POINTY:
					ResizeMode = ResizeModePoint;
					ResizeAktWidth = FALSE;
					break;

				case IDN_PICRESIZE_PROCX:
					ResizeMode = ResizeModeProc;
					ResizeAktWidth = TRUE;
					break;

				case IDN_PICRESIZE_PROCY:
					ResizeMode = ResizeModeProc;
					ResizeAktWidth = FALSE;
					break;
				}

				ResizeSetSwc(hWnd);
				ResizeGetText(hWnd);

				switch (LOWORD(wParam))
				{
				case IDN_PICRESIZE_STEPX:
				case IDN_PICRESIZE_POINTX:
					ResizeWidthProc = (double)Round((double)ResizeWidth/ResizeOldWidth*10000)/100;
					if (ResizeModeProp)
					{
						ResizeHeightProc = (double)Round((double)ResizeHeight/ResizeOldHeight*10000)/100;
					}
					break;

				case IDN_PICRESIZE_STEPY:
				case IDN_PICRESIZE_POINTY:
					ResizeHeightProc = (double)Round((double)ResizeHeight/ResizeOldHeight*10000)/100;
					if (ResizeModeProp)
					{
						ResizeWidthProc = (double)Round((double)ResizeWidth/ResizeOldWidth*10000)/100;
					}
					break;
				}

				ResizeSetText(hWnd, FALSE);
			}
			break;
		}
		break;
	}
	return FALSE;
}


// změna velikosti obrázku nebo bloku
void EditPicResize(int width, int height, BOOL inter, BOOL dith)
{
// omezení zadaných údajů
	if (width < 1) width = 1;
	if (width > 0x8000) width = 0x8000;
	if (height < 1) height = 1;
	if (height > 0x8000) height = 0x8000;

// kontrola, zda se rozměry mění
	if ((width == ResizeOldWidth) && (height == ResizeOldHeight)) return;

// zapnutí kurzoru čekání
	BeginWaitCursor();

// změna velikosti obrázku, není-li označen blok
	if (!Selecting && (BufID == BufPicID))
	{
		if (Index == -2)
		{
			Undo.AddSprSet(EditItemSprite, Sprite[EditItemSprite]);
			Sprite[EditItemSprite].CopyWrite();

			for (int ii = Sprite[EditItemSprite].Faze()*Sprite[EditItemSprite].Smer()-1; ii >= 0; ii--)
			{
				Sprite[EditItemSprite][ii].Zoom(width, height, inter, dith);
			}

			Sprite[EditItemSprite][EditItemPic].DeComp();
			Data = Sprite[EditItemSprite][EditItemPic].DataData();
			Width = Sprite[EditItemSprite][EditItemPic].Width();
			Height = Sprite[EditItemSprite][EditItemPic].Height();

			Undo.Stop();
		}
		else
		{
			PushUndo();

			Picture[Index].Zoom(width, height, inter, dith);
			Picture[Index].DeComp();
			Data = Picture[Index].DataData();
			Width = Picture[Index].Width();
			Height = Picture[Index].Height();
		}

		PushBuf = (BYTE*)MemSize(PushBuf, Width * Height * sizeof(BYTE));
		if ((DWORD)MidX >= (DWORD)Width) MidX = Width/2;
		if ((DWORD)MidY >= (DWORD)Height) MidY = Height/2;

// přepočet zobrazení
		ReCalc();
		DispLeft = MidX - DispWidth/2;
		DispBottom = MidY - DispHeight/2;
		ReCalc();
		SetScroll();
		ProgOnSize();
	}
	else
	{

// úschova UNDO
		PushUndo();

// příprava bloku, není-li označen
		if (!Selecting) SelectAll();

// změna velikosti bloku
		CPicture pic;

		if (pic.New(BlokWidth, BlokHeight) &&
			pic.CopyData(BlokBuf) &&
			pic.Zoom(width, height, inter, dith) &&
			pic.DeComp() &&
			((BlokBuf = (BYTE*)MemSize(BlokBuf, pic.Size())) != NULL))
		{
			BlokWidth = pic.Width();
			BlokHeight = pic.Height();
			MemCopy(BlokBuf, pic.DataData(), pic.Size());
		}

// korekce počátku k zobrazení bloku
		if ((BlokX > (DispLeft+DispWidth-2)) ||
			(BlokY > (Height-DispBottom-2)) ||
			((BlokX + BlokWidth) < (DispLeft+2)) ||
			((BlokY + BlokHeight) < (Height - DispBottom - DispHeight + 2)))
		{
			BlokX = DispLeft + DispWidth/2 - BlokWidth/2;
			BlokY = Height - DispBottom - DispHeight/2 - BlokHeight/2;
		}

// položení bloku
		MoveSelect();
		DispSelect();
	}

// překreslení displeje
	Disp();

// příznak modifikace souboru
	SetModi();

// aktualizace voleb pro blok
	UpdateClipboard();

// aktualizace zobrazení stromů
	UpdateTree();

// vypnutí kurzoru čekání
	EndWaitCursor();
}


// provedení editace bloku (-1 = zatím neurčeno)
void EditBlok(int mode)
{
// kontrola, zda je režim editace
	if (!ProgMode ||
		((EditMode != BufIcoID) && (EditMode != BufPicID)))
		return;

// příprava bufferů
	if (BufID == BufPicID)
	{
		if (Index == -2)
		{
			Sprite[EditItemSprite][EditItemPic].DeComp();
			Data = Sprite[EditItemSprite][EditItemPic].DataData();
		}
		else
		{
			Picture[Index].DeComp();
			Data = Picture[Index].DataData();
		}
	}
	else
	{
		Icon[Index].DeComp();
		Data = Icon[Index].DataData();
	}

// zadání operace
	if (mode < 0)
	{
		mode = ::DialogBoxParam(
				hInstance,
				MAKEINTRESOURCE(IDN_PICBLOK),
				MainFrame,
				(DLGPROC)BlokDialogProc,
				(LPARAM)IDN_PICBLOK);
	}
	
	if (mode == IDCANCEL) return;

// úschova velikosti pro změnu velikosti bloku
	ResizeOldWidth = Width;
	ResizeOldHeight = Height;

	if (Selecting)
	{
		ResizeOldWidth = BlokWidth;
		ResizeOldHeight = BlokHeight;
	}

// dvojnásobná velikost
	if (mode == IDN_MUL2)
	{
		EditPicResize(ResizeOldWidth*2, ResizeOldHeight*2, FALSE, FALSE);
		return;
	}

// poloviční velikost
	if (mode == IDN_DIV2)
	{
		EditPicResize(ResizeOldWidth/2, ResizeOldHeight/2, TRUE, Dither);
		return;
	}

// změna velikosti bloku
	if (mode == IDN_MULDIV)
	{
		if (::DialogBoxParam(
			hInstance,
			MAKEINTRESOURCE(IDN_PICRESIZE),
			MainFrame,
			(DLGPROC)ResizeDialogProc,
			(LPARAM)IDN_PICRESIZE) == IDOK)
		{
			EditPicResize(ResizeWidth, ResizeHeight, ResizeModeInt, ResizeModeDith);
		}
		return;
	}

// zapnutí kurzoru čekání
	BeginWaitCursor();

// úschova do UNDO
	PushUndo();

// lokální proměnné
	int x, y;							// ukazatel souřadnic
	BYTE* blok2;						// pomocný buffer bloku
	int oldwidth;						// stará šířka bloku
	int oldheight;						// stará výška bloku
	double uhel = 0;					// úhel pro rotaci
	double uhel2;						// pomocný úhel pro rotaci
	double delka2;						// pomocná délka pro rotaci
	int dx, dy;							// vzdálenost od středu
	BYTE col;							// meziúschova barvy
	int xr, yr;							// pomocné registry
	BOOL oldsel = Selecting;			// úschova příznaku označení bloku

// příprava bloku, není-li označen
	if (!Selecting)
	{
		SelectAll();
	}

// úschova šířky a výšky bloku
	oldwidth = BlokWidth;
	oldheight = BlokHeight;

// nastavení nových rozměrů bloku
	switch (mode)
	{
	case IDN_LROT:
	case IDN_RROT:
		BlokWidth = oldheight;
		BlokHeight = oldwidth;
		break;

// úhel rotace
	case IDN_LROT67:
		uhel = (double)uhel67;
		goto ROTUHEL;

	case IDN_LROT60:
		uhel = (double)uhel60;
		goto ROTUHEL;

	case IDN_LROT45:
		uhel = (double)uhel45;
		goto ROTUHEL;

	case IDN_LROT30:
		uhel = (double)uhel30;
		goto ROTUHEL;

	case IDN_LROT22:
		uhel = (double)uhel22;
		goto ROTUHEL;

	case IDN_RROT67:
		uhel = (double)-uhel67;
		goto ROTUHEL;

	case IDN_RROT60:
		uhel = (double)-uhel60;
		goto ROTUHEL;

	case IDN_RROT45:
		uhel = (double)-uhel45;
		goto ROTUHEL;

	case IDN_RROT30:
		uhel = (double)-uhel30;
		goto ROTUHEL;

	case IDN_RROT22:
		uhel = (double)-uhel22;
ROTUHEL:
		uhel2 = atan2((double)oldheight, (double)oldwidth);
		delka2 = sqrt((double)(BlokWidth*BlokWidth + BlokHeight*BlokHeight));
		BlokHeight = (int)(delka2*sin(fabs(uhel)+uhel2)+0.9);
		BlokWidth = (int)(delka2*cos(fabs(uhel)-uhel2)+0.9);
		break;
	}

// korekce počátku bloku
	BlokX += oldwidth/2 - BlokWidth/2;
	BlokY += oldheight/2 - BlokHeight/2;
	if (BlokX > (DispLeft + DispWidth - 5)) BlokX = DispLeft + DispWidth - 5;
	if (BlokY > (Height - DispBottom - 5)) BlokY = Height - DispBottom - 5;
	if ((BlokX + BlokWidth) < (DispLeft + 5)) BlokX = DispLeft + 5 - BlokWidth;
	if ((BlokY + BlokHeight) < (Height - DispBottom - DispHeight + 5)) 
		BlokY = Height - DispBottom - DispHeight + 5 - BlokHeight;

// úschova bloku a vytvoření jiného
	blok2 = BlokBuf;						// aktuální blok
	BlokBuf = (BYTE*)MemGet(BlokWidth*BlokHeight);	// vytvoření nového bloku

// cyklus přes všechny body
	for (x = 0; x < BlokWidth; x++)
	{
		for (y = 0; y < BlokHeight; y++)
		{
			switch (mode)
			{

// horizontální převrácení
			case IDN_YFLIP:
				SetBlok(x,y,blok2[BlokWidth - 1 - x + BlokWidth*(BlokHeight-1-y)]);
				break;

// vertikální převrácení
			case IDN_XFLIP:
				SetBlok(x,y,blok2[x + BlokWidth*y]);
				break;

// otočení vpravo
			case IDN_RROT:
				SetBlok(x,y,blok2[y + BlokHeight*(BlokWidth - 1 - (BlokWidth - 1 - x))]);
				break;

// otočení vlevo
			case IDN_LROT:
				SetBlok(x,y,blok2[BlokHeight - 1 - y + BlokHeight*(BlokWidth - 1 - x)]);
				break;

// záměna barev
			case IDN_XCOL:
				col = blok2[x + BlokWidth*(BlokHeight - 1 - y)];
				if (col == ColLeft)
				{
					SetBlok(x,y,ColRight);
				}
				else
				{
					if (col == ColRight)
					{
						SetBlok(x,y,ColLeft);
					}
					else
					{
						SetBlok(x, y, col);
					}
				}
				break;

// nahrazení barvy
			case IDN_SCOL:
				col = blok2[x + BlokWidth*(BlokHeight - 1 - y)];
				if (col == ColLeft)
				{
					SetBlok(x,y,ColRight);
				}
				else
				{
					SetBlok(x, y, col);
				}
				break;

// otočení
			case IDN_LROT67:
			case IDN_LROT60:
			case IDN_LROT45:
			case IDN_LROT30:
			case IDN_LROT22:
			case IDN_RROT67:
			case IDN_RROT60:
			case IDN_RROT45:
			case IDN_RROT30:
			case IDN_RROT22:
				dx = x - BlokWidth/2;
				dy = y - BlokHeight/2;

				uhel2 = atan2((double)dy, (double)dx) + uhel;
				delka2 = sqrt((double)(dx*dx + dy*dy));
				xr = (int)(delka2*cos(uhel2)+0.5+oldwidth)-(oldwidth+1)/2;
				yr = (int)(delka2*sin(uhel2)+0.5+oldheight)-(oldheight+1)/2;

				if ((xr >= 0) && (yr >= 0) && (xr < oldwidth) && (yr < oldheight))
				{
					SetBlok(x,y,blok2[xr + oldwidth*(oldheight-1-yr)]);
				}
				else
				{
					SetBlok(x, y, BlokBack);
				}
				break;
			}
		}
	}

// zrušení pomocného bloku
	MemFree(blok2);

// překreslení bloku (pro obrázek změna rozměrů)
	DispSelect();
	if (oldsel || (Index == -2))
	{
		MoveSelect();
		DispSelect();
	}
	else
	{
		if (BufID == BufPicID)
		{
			ASSERT((Index == -2) || Picture.IsValid(Index));

			Height = BlokHeight;
			Width = BlokWidth;
			PushBuf = (BYTE*)MemSize(PushBuf, Width * Height * sizeof(BYTE));
			if ((DWORD)MidX >= (DWORD)Width) MidX = Width/2;
			if ((DWORD)MidY >= (DWORD)Height) MidY = Height/2;

			CPicture* pic;

			if (Index == -2)
			{
				EditItemSprite = EditSprite::Index;
				EditItemPic = EditSprite::IndexPic;
				pic = &Sprite[EditItemSprite][EditItemPic];
			}
			else
			{
				pic = &(Picture[Index]);	// adresa obrázku
			}

			pic->New(Width, Height);
			Data = pic->DataData();

			BlokX = 0;
			BlokY = 0;
			FirstBlokX = 0;
			FirstBlokY = 0;
			FirstBlokWidth = Width;
			FirstBlokHeight = Height;
		}
		
		MoveSelect();
		Selecting = FALSE;
		SetMode(OldMode);
	}

// překreslení displeje
	Disp();

// příznak modifikace souboru
	SetModi();

// aktualizace voleb pro blok
	UpdateClipboard();

// aktualizace zobrazení stromů
	UpdateTree();

// vypnutí kurzoru čekání
	EndWaitCursor();
}


/////////////////////////////////////////////////////////////////////////////
// aktualizace voleb výběru módu

void UpdateMenu()
{
	SelectCombo(ToolEditIcon, Mode, ToolBarEditPicTools);
	SelectCombo(ToolEditIconWidth, PenWidthID, ToolBarEditPicWidths);
	if (FocusBuf == BufEdiID)
	{
		CheckCommand(IDN_RASTR, Rastr);
	}
}


// ************************** obsluha kreslení ******************************


/////////////////////////////////////////////////////////////////////////////
// vykreslení bodu
 
void _fastcall SetPoint(int x, int y, BYTE col)
{
// kontrola souřadnic
	if ((x < 0) || (y < 0) ||
		(x >= Width) || (y >= Height))
		return;

// adresa bajtu dat
	BYTE* data = &Data[Width*(Height-1-y) + x];	// adresa začátku dat obrázku
	*data = col;
}


/////////////////////////////////////////////////////////////////////////////
// vykreslení bodu štětce

void SetPaint(int x, int y, BYTE col)
{
	int i;

	switch (PenWidth)
	{
// *
	case 1:
		SetPoint(x  , y  , col);
		if (x < AktLeft) AktLeft = x;
		if (x >= AktRight) AktRight = x+1;
		if (y < AktTop) AktTop = y;
		if (y >= AktBottom) AktBottom = y+1;
		break;

// **
// **
	case 2:
		SetPoint(x-1, y-1, col);
		SetPoint(x  , y-1, col);

		SetPoint(x-1, y  , col);
		SetPoint(x  , y  , col);

		if ((x-1) < AktLeft) AktLeft = x-1;
		if (x >= AktRight) AktRight = x+1;
		if ((y-1) < AktTop) AktTop = y-1;
		if (y >= AktBottom) AktBottom = y+1;
		break;

// ***
// ***
// ***
	case 3:
		SetPoint(x-1, y-1, col);
		SetPoint(x  , y-1, col);
		SetPoint(x+1, y-1, col);

		SetPoint(x-1, y  , col);
		SetPoint(x  , y  , col);
		SetPoint(x+1, y  , col);

		SetPoint(x-1, y+1, col);
		SetPoint(x  , y+1, col);
		SetPoint(x+1, y+1, col);

		if ((x-1) < AktLeft) AktLeft = x-1;
		if ((x+1) >= AktRight) AktRight = x+2;
		if ((y-1) < AktTop) AktTop = y-1;
		if ((y+1) >= AktBottom) AktBottom = y+2;
		break;

//  ***
// *****
// *****
// *****
//  ***
	case 5:
		SetPoint(x-1, y-2, col);
		SetPoint(x  , y-2, col);
		SetPoint(x+1, y-2, col);

		SetPoint(x-2, y-1, col);
		SetPoint(x-1, y-1, col);
		SetPoint(x  , y-1, col);
		SetPoint(x+1, y-1, col);
		SetPoint(x+2, y-1, col);

		SetPoint(x-2, y  , col);
		SetPoint(x-1, y  , col);
		SetPoint(x  , y  , col);
		SetPoint(x+1, y  , col);
		SetPoint(x+2, y  , col);

		SetPoint(x-2, y+1, col);
		SetPoint(x-1, y+1, col);
		SetPoint(x  , y+1, col);
		SetPoint(x+1, y+1, col);
		SetPoint(x+2, y+1, col);

		SetPoint(x-1, y+2, col);
		SetPoint(x  , y+2, col);
		SetPoint(x+1, y+2, col);

		if ((x-2) < AktLeft) AktLeft = x-2;
		if ((x+2) >= AktRight) AktRight = x+3;
		if ((y-2) < AktTop) AktTop = y-2;
		if ((y+2) >= AktBottom) AktBottom = y+3;
		break;

//   *****  
//  ******* 
// *********
// *********
// *********
// *********
// *********
//  ******* 
//   *****  
	case 9:
		SetPoint(x-2, y-4, col);
		SetPoint(x-1, y-4, col);
		SetPoint(x  , y-4, col);
		SetPoint(x+1, y-4, col);
		SetPoint(x+2, y-4, col);

		SetPoint(x-3, y-3, col);
		SetPoint(x-2, y-3, col);
		SetPoint(x-1, y-3, col);
		SetPoint(x  , y-3, col);
		SetPoint(x+1, y-3, col);
		SetPoint(x+2, y-3, col);
		SetPoint(x+3, y-3, col);

		for (i = y-2; i <= y+2; i++)
		{
			SetPoint(x-4, i, col);
			SetPoint(x-3, i, col);
			SetPoint(x-2, i, col);
			SetPoint(x-1, i, col);
			SetPoint(x  , i, col);
			SetPoint(x+1, i, col);
			SetPoint(x+2, i, col);
			SetPoint(x+3, i, col);
			SetPoint(x+4, i, col);
		}

		SetPoint(x-3, y+3, col);
		SetPoint(x-2, y+3, col);
		SetPoint(x-1, y+3, col);
		SetPoint(x  , y+3, col);
		SetPoint(x+1, y+3, col);
		SetPoint(x+2, y+3, col);
		SetPoint(x+3, y+3, col);

		SetPoint(x-2, y+4, col);
		SetPoint(x-1, y+4, col);
		SetPoint(x  , y+4, col);
		SetPoint(x+1, y+4, col);
		SetPoint(x+2, y+4, col);

		if ((x-4) < AktLeft) AktLeft = x-4;
		if ((x+4) >= AktRight) AktRight = x+5;
		if ((y-4) < AktTop) AktTop = y-4;
		if ((y+4) >= AktBottom) AktBottom = y+5;
		break;

//        *******          10
//      ***********        9
//     *************       8
//    ***************      7
//   *****************     6
//  *******************    5
//  *******************    4
// *********************   3
// *********************   2
// *********************   1
// *********************   0
// *********************  -1
// *********************  -2
// *********************  -3
//  *******************   -4
//  *******************   -5
//   *****************    -6
//    ***************     -7
//     *************      -8
//      ***********       -9
//        *******         -10
	case 21:
		SetPoint(x- 3, y-10, col);
		SetPoint(x- 2, y-10, col);
		SetPoint(x- 1, y-10, col);
		SetPoint(x   , y-10, col);
		SetPoint(x+ 1, y-10, col);
		SetPoint(x+ 2, y-10, col);
		SetPoint(x+ 3, y-10, col);

		SetPoint(x- 5, y- 9, col);
		SetPoint(x- 4, y- 9, col);
		SetPoint(x- 3, y- 9, col);
		SetPoint(x- 2, y- 9, col);
		SetPoint(x- 1, y- 9, col);
		SetPoint(x   , y- 9, col);
		SetPoint(x+ 1, y- 9, col);
		SetPoint(x+ 2, y- 9, col);
		SetPoint(x+ 3, y- 9, col);
		SetPoint(x+ 4, y- 9, col);
		SetPoint(x+ 5, y- 9, col);

		SetPoint(x- 6, y- 8, col);
		SetPoint(x- 5, y- 8, col);
		SetPoint(x- 4, y- 8, col);
		SetPoint(x- 3, y- 8, col);
		SetPoint(x- 2, y- 8, col);
		SetPoint(x- 1, y- 8, col);
		SetPoint(x   , y- 8, col);
		SetPoint(x+ 1, y- 8, col);
		SetPoint(x+ 2, y- 8, col);
		SetPoint(x+ 3, y- 8, col);
		SetPoint(x+ 4, y- 8, col);
		SetPoint(x+ 5, y- 8, col);
		SetPoint(x+ 6, y- 8, col);

		SetPoint(x- 7, y- 7, col);
		SetPoint(x- 6, y- 7, col);
		SetPoint(x- 5, y- 7, col);
		SetPoint(x- 4, y- 7, col);
		SetPoint(x- 3, y- 7, col);
		SetPoint(x- 2, y- 7, col);
		SetPoint(x- 1, y- 7, col);
		SetPoint(x   , y- 7, col);
		SetPoint(x+ 1, y- 7, col);
		SetPoint(x+ 2, y- 7, col);
		SetPoint(x+ 3, y- 7, col);
		SetPoint(x+ 4, y- 7, col);
		SetPoint(x+ 5, y- 7, col);
		SetPoint(x+ 6, y- 7, col);
		SetPoint(x+ 7, y- 7, col);

		SetPoint(x- 8, y- 6, col);
		SetPoint(x- 7, y- 6, col);
		SetPoint(x- 6, y- 6, col);
		SetPoint(x- 5, y- 6, col);
		SetPoint(x- 4, y- 6, col);
		SetPoint(x- 3, y- 6, col);
		SetPoint(x- 2, y- 6, col);
		SetPoint(x- 1, y- 6, col);
		SetPoint(x   , y- 6, col);
		SetPoint(x+ 1, y- 6, col);
		SetPoint(x+ 2, y- 6, col);
		SetPoint(x+ 3, y- 6, col);
		SetPoint(x+ 4, y- 6, col);
		SetPoint(x+ 5, y- 6, col);
		SetPoint(x+ 6, y- 6, col);
		SetPoint(x+ 7, y- 6, col);
		SetPoint(x+ 8, y- 6, col);

		for (i = y-5; i <= y-4; i++)
		{
			SetPoint(x- 9, i, col);
			SetPoint(x- 8, i, col);
			SetPoint(x- 7, i, col);
			SetPoint(x- 6, i, col);
			SetPoint(x- 5, i, col);
			SetPoint(x- 4, i, col);
			SetPoint(x- 3, i, col);
			SetPoint(x- 2, i, col);
			SetPoint(x- 1, i, col);
			SetPoint(x   , i, col);
			SetPoint(x+ 1, i, col);
			SetPoint(x+ 2, i, col);
			SetPoint(x+ 3, i, col);
			SetPoint(x+ 4, i, col);
			SetPoint(x+ 5, i, col);
			SetPoint(x+ 6, i, col);
			SetPoint(x+ 7, i, col);
			SetPoint(x+ 8, i, col);
			SetPoint(x+ 9, i, col);
		}

		for (i = y-3; i <= y+3; i++)
		{
			SetPoint(x-10, i, col);
			SetPoint(x- 9, i, col);
			SetPoint(x- 8, i, col);
			SetPoint(x- 7, i, col);
			SetPoint(x- 6, i, col);
			SetPoint(x- 5, i, col);
			SetPoint(x- 4, i, col);
			SetPoint(x- 3, i, col);
			SetPoint(x- 2, i, col);
			SetPoint(x- 1, i, col);
			SetPoint(x   , i, col);
			SetPoint(x+ 1, i, col);
			SetPoint(x+ 2, i, col);
			SetPoint(x+ 3, i, col);
			SetPoint(x+ 4, i, col);
			SetPoint(x+ 5, i, col);
			SetPoint(x+ 6, i, col);
			SetPoint(x+ 7, i, col);
			SetPoint(x+ 8, i, col);
			SetPoint(x+ 9, i, col);
			SetPoint(x+10, i, col);
		}

		for (i = y+4; i <= y+5; i++)
		{
			SetPoint(x- 9, i, col);
			SetPoint(x- 8, i, col);
			SetPoint(x- 7, i, col);
			SetPoint(x- 6, i, col);
			SetPoint(x- 5, i, col);
			SetPoint(x- 4, i, col);
			SetPoint(x- 3, i, col);
			SetPoint(x- 2, i, col);
			SetPoint(x- 1, i, col);
			SetPoint(x   , i, col);
			SetPoint(x+ 1, i, col);
			SetPoint(x+ 2, i, col);
			SetPoint(x+ 3, i, col);
			SetPoint(x+ 4, i, col);
			SetPoint(x+ 5, i, col);
			SetPoint(x+ 6, i, col);
			SetPoint(x+ 7, i, col);
			SetPoint(x+ 8, i, col);
			SetPoint(x+ 9, i, col);
		}

		SetPoint(x- 8, y+ 6, col);
		SetPoint(x- 7, y+ 6, col);
		SetPoint(x- 6, y+ 6, col);
		SetPoint(x- 5, y+ 6, col);
		SetPoint(x- 4, y+ 6, col);
		SetPoint(x- 3, y+ 6, col);
		SetPoint(x- 2, y+ 6, col);
		SetPoint(x- 1, y+ 6, col);
		SetPoint(x   , y+ 6, col);
		SetPoint(x+ 1, y+ 6, col);
		SetPoint(x+ 2, y+ 6, col);
		SetPoint(x+ 3, y+ 6, col);
		SetPoint(x+ 4, y+ 6, col);
		SetPoint(x+ 5, y+ 6, col);
		SetPoint(x+ 6, y+ 6, col);
		SetPoint(x+ 7, y+ 6, col);
		SetPoint(x+ 8, y+ 6, col);

		SetPoint(x- 7, y+ 7, col);
		SetPoint(x- 6, y+ 7, col);
		SetPoint(x- 5, y+ 7, col);
		SetPoint(x- 4, y+ 7, col);
		SetPoint(x- 3, y+ 7, col);
		SetPoint(x- 2, y+ 7, col);
		SetPoint(x- 1, y+ 7, col);
		SetPoint(x   , y+ 7, col);
		SetPoint(x+ 1, y+ 7, col);
		SetPoint(x+ 2, y+ 7, col);
		SetPoint(x+ 3, y+ 7, col);
		SetPoint(x+ 4, y+ 7, col);
		SetPoint(x+ 5, y+ 7, col);
		SetPoint(x+ 6, y+ 7, col);
		SetPoint(x+ 7, y+ 7, col);

		SetPoint(x- 6, y+ 8, col);
		SetPoint(x- 5, y+ 8, col);
		SetPoint(x- 4, y+ 8, col);
		SetPoint(x- 3, y+ 8, col);
		SetPoint(x- 2, y+ 8, col);
		SetPoint(x- 1, y+ 8, col);
		SetPoint(x   , y+ 8, col);
		SetPoint(x+ 1, y+ 8, col);
		SetPoint(x+ 2, y+ 8, col);
		SetPoint(x+ 3, y+ 8, col);
		SetPoint(x+ 4, y+ 8, col);
		SetPoint(x+ 5, y+ 8, col);
		SetPoint(x+ 6, y+ 8, col);

		SetPoint(x- 5, y+ 9, col);
		SetPoint(x- 4, y+ 9, col);
		SetPoint(x- 3, y+ 9, col);
		SetPoint(x- 2, y+ 9, col);
		SetPoint(x- 1, y+ 9, col);
		SetPoint(x   , y+ 9, col);
		SetPoint(x+ 1, y+ 9, col);
		SetPoint(x+ 2, y+ 9, col);
		SetPoint(x+ 3, y+ 9, col);
		SetPoint(x+ 4, y+ 9, col);
		SetPoint(x+ 5, y+ 9, col);

		SetPoint(x- 3, y+10, col);
		SetPoint(x- 2, y+10, col);
		SetPoint(x- 1, y+10, col);
		SetPoint(x   , y+10, col);
		SetPoint(x+ 1, y+10, col);
		SetPoint(x+ 2, y+10, col);
		SetPoint(x+ 3, y+10, col);

		if ((x-10) < AktLeft) AktLeft = x-10;
		if ((x+10) >= AktRight) AktRight = x+11;
		if ((y-10) < AktTop) AktTop = y-10;
		if ((y+10) >= AktBottom) AktBottom = y+11;
		break;

	default:
		ASSERTERROR;
//		int x1 = x - (m_PenWidth+1)/2;
//		int y1 = y - (m_PenWidth+1)/2;
//		int x2 = x + m_PenWidth/2;
//		int y2 = y + (m_PenWidth+1)/2;

//		SetRoundFill(x1, y, x2, y, col);

//		if (x1 < m_AktLeft) m_AktLeft = x1;
//		if (x2 >= m_AktRight) m_AktRight = x2+1;
//		if (y1 < m_AktTop) m_AktTop = y1;
//		if (y2 >= m_AktBottom) m_AktBottom = y2+1;
	}
}


/////////////////////////////////////////////////////////////////////////////
// nastavení bodu spreje

void SetSpray(int x, int y, BYTE col)
{
	int m;
	switch (PenWidth)
	{
	case 1:
		m = 4;
		break;
	case 2:
	case 3:
		m = 8;
		break;
	case 4:
	case 5:
	case 6:
		m = 16;
		break;
	case 7:
	case 8:
	case 9:
	case 10:
		m = 32;
		break;
	default:
		m = 64;
	}

	int old = PenWidth;
	PenWidth = 1;

	for (int i = (m-3)*2; i > 0; i--)
	{
		if ((rand() & 0x7f) < 10)
		{
			SetPaint(x + (rand() & (m-1)) - (m/2-1), y + (rand() & (m/2-1)) - (m/4-1), col);
		}

		if ((rand() & 0x7f) < 10)
		{
			SetPaint(x + (rand() & (m/2-1)) - (m/4-1), y + (rand() & (m-1)) - (m/2-1), col);
		}

		if ((rand() & 0x7f) < 10)
		{
			SetPaint(x + (rand() & (m-1)) - (m/2-1), y + (rand() & (m-1)) - (m/2-1), col);
		}
	}
	PenWidth = old;
}


/////////////////////////////////////////////////////////////////////////////
// nastavení čáry štětcem

void SetLinePaint(int x1, int y1, int x2, int y2, BYTE col)
{
// lokální proměnné
	int i;						// pracovní čítač
	int dx;						// rozdíl souřadnic X
	int dy;						// rozdíl souřadnic Y

// oprava souřadnic, aby čára šla vždy zleva doprava (tj. x1 vlevo, x2 vpravo)
	if (x2 < x1)
	{
		i = x2; x2 = x1; x1 = i;
		i = y2; y2 = y1; y1 = i;
	}

// rozdíl souřadnic X
	dx = x2 - x1;

// směr nahoru (tj. y1 dole, y2 nahoře)
	if (y1 >= y2)
	{

// rozdíl souřadnic Y
		dy = y1 - y2;

// strmě nahoru
		if (dx <= dy)
		{
			for (i = 0; i < dy; i++)
			{
				SetPaint(x1 + (dx*i + dy/2)/dy, y1, col);
				y1--;
			}
		}

// mírně nahoru
		else
		{
			for (i = 0; i < dx; i++)
			{
				SetPaint(x1, y1 - (dy*i + dx/2)/dx, col);
				x1++;
			}
		}
	}

// směr dolů (tj. y1 nahoře, y2 dole)
	else
	{

// rozdíl souřadnic Y
		dy = y2 - y1;

// stromě dolů
		if (dx <= dy)
		{
			for (i = 0; i < dy; i++)
			{
				SetPaint(x1 + (dx*i + dy/2)/dy, y1, col);
				y1++;
			}
		}

// mírně dolů
		else
		{
			for (i = 0; i < dx; i++)
			{
				SetPaint(x1, y1 + (dy*i + dx/2)/dx, col);
				x1++;
			}
		}
	}

// vykreslení koncového bodu (nedělat v cyklu pro případ dx,dy = 0)
	SetPaint(x2, y2, col);
}


/////////////////////////////////////////////////////////////////////////////
// vykreslení obdélníku

void SetRect(int x1, int y1, int x2, int y2, BYTE col)
{
	SetLinePaint(x1, y1, x2, y1, col);
	SetLinePaint(x1, y1, x1, y2, col);
	SetLinePaint(x1, y2, x2, y2, col);
	SetLinePaint(x2, y1, x2, y2, col);
}


/////////////////////////////////////////////////////////////////////////////
// vykreslení obdélníku s výplní

void SetRectFill(int x1, int y1, int x2, int y2, BYTE col)
{
	int width;					// šířka linky
	int height;					// počet linek
	BYTE* dst;					// ukládací adresa

// oprava pořadí bodů X, aby byl X1 před X2
	if (x1 > x2)
	{
		width = x1;
		x1 = x2;
		x2 = width;
	}

// oprava pořadí bodů Y, aby byl Y1 před Y2
	if (y1 > y2)
	{
		height = y1;
		y1 = y2;
		y2 = height;
	}

// omezení krajních bodů
	if (x1 < 0) x1 = 0;
	if (x2 >= Width) x2 = Width - 1;
	width = x2 - x1 + 1;
	if (width <= 0) return;
	if (y1 < 0) y1 = 0;
	if (y2 >= Height) y2 = Height - 1;
	height = y2 - y1 + 1;

// aktualizace displeje
	if (x1 < AktLeft) AktLeft = x1;
	if (x2 >= AktRight) AktRight = x2+1;
	if (y1 < AktTop) AktTop = y1;
	if (y2 >= AktBottom) AktBottom = y2+1;

// příprava ukládací adresy dat
	dst = Data + (Height - 1 - y2)*Width + x1;
		
// vykreslení obdélníku
	for (; height > 0; height--)
	{
		MemFill(dst, width, col);
		dst += Width;
	}
}


/////////////////////////////////////////////////////////////////////////////
// vykreslení kružnice

void SetRound(int x1, int y1, int x2, int y2, BYTE col)
{
// lokální proměnné
	int sx, sy;							// střed kružnice
	int dx,dy;							// vzdálenost bodu od středu
	double polomer;						// poloměr
	double polomer2;					// kvadrát poloměru
	double width = PenWidth - 0.5;		// šířka pera

// střed kružnice
	sx = (x1 + x2)/2;					// souřadnice středu X
	sy = (y1 + y2)/2;					// souřadnice středu Y

// počáteční poloměr kružnice (nejmenší)
	int pol = (int)(sqrt((double)((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1))) + 0.8);
	polomer = (double)(pol/2) - width/2;
	pol = pol % 2;

// cyklus přes všechny průměry
	for (; width > 0; width -= 0.51)
	{
		polomer2 = polomer*polomer;

// cyklus přes všechny body
		for (dx = (int)(polomer * 0.8 + 0.8); dx >= 0; dx--)
		{
			dy = (int)(sqrt(polomer2 - (double)dx*dx) + 0.7);

			SetPoint(sx - dx		, sy + dy + pol	, col);
			SetPoint(sx + dx + pol	, sy + dy + pol	, col);
			SetPoint(sx - dx		, sy - dy		, col);
			SetPoint(sx + dx + pol	, sy - dy		, col);

			SetPoint(sx - dy		, sy + dx + pol	, col);
			SetPoint(sx + dy + pol	, sy + dx + pol	, col);
			SetPoint(sx - dy		, sy - dx		, col);
			SetPoint(sx + dy + pol	, sy - dx		, col);
		}
		polomer += 0.52;
	}
}


/////////////////////////////////////////////////////////////////////////////
// kreslení kruhu

void SetRoundFill(int x1, int y1, int x2, int y2, BYTE col)
{
// lokální proměnné
	int x,y;							// ukazatele bodu
	int d;								// průměr kruhu
	double sx, sy;						// střed kruhu
	double dx,dy;						// vzdálenost bodu od středu

// oprava pořadí souřadnic X
	if (x1 > x2)
	{
		x = x1;
		x1 = x2;
		x2 = x;
	}

// oprava pořadí souřadnic Y
	if (y1 > y2)
	{
		y = y1;
		y1 = y2;
		y2 = y;
	}

// průměr kruhu (včetně koncových bodů)
	d = (int)(sqrt((double)((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1)))+0.4) + 1;

// střed kruhu (zatím zaokrouhlené na celé body)
	sx = (int)((x1 + x2) / 2);		// souřadnice středu X
	sy = (int)((y1 + y2) / 2);		// souřadnice středu Y

// korekce středu pro sudý průměr
	if ((d & 1) == 0)
	{
		sx += 0.5;
		sy += 0.5;
	}

// příprava minimálních a maximálních souřadnic
	x1 = (int)sx - d/2 - 2;
	x2 = (int)sx + d/2 + 2;
	y1 = (int)sy - d/2 - 2;
	y2 = (int)sy + d/2 + 2;

// vykreslení kruhu
	for (x = x1; x <= x2; x++)			// cyklus přes všechny pozice na řádku
	{
		for (y = y1; y <= y2; y++)		// cyklus přes všechny řádky
		{
			dx = fabs((double)x - sx)*2;		// vzdálenost od středu ve směru X
			dy = fabs((double)y - sy)*2;		// vzdálenost od středu ve směru Y

			if (dx*dx + dy*dy <= (double)d*d)	// leží bod uvnitř kruhu ?
			{
				SetPoint(x,y,col);		// nastavení bodu kruhu
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// kreslení elipsy

void SetElip(int x1, int y1, int x2, int y2, BYTE col)
{
// lokální proměnné
	int sx, sy;							// střed elipsy
	int dx,dy;							// vzdálenost bodu od středu
	double rx, ry;						// poloměr ve směru X a Y

// střed elipsy
	sx = (x1 + x2)/2;					// souřadnice středu X
	sy = (y1 + y2)/2;					// souřadnice středu Y

// poloměr elipsy
	rx = (double)(abs(x2 - x1) / 2);
	ry = (double)(abs(y2 - y1) / 2);
	if (rx < 0.01) rx = 0.01;
	if (ry < 0.01) ry = 0.01;

	int kx = abs(x2 - x1) % 2;
	int ky = abs(y2 - y1) % 2;

// cyklus přes všechny horizontální body
	for (dx = (int)rx; dx >= 0; dx--)
	{
		dy = (int)(sqrt(1 - (double)dx*dx/rx/rx)*ry + 0.5);

		SetPaint(sx - dx		, sy + dy + ky	, col);
		SetPaint(sx + dx + kx	, sy + dy + ky	, col);
		SetPaint(sx - dx		, sy - dy		, col);
		SetPaint(sx + dx + kx	, sy - dy		, col);
	}

// cyklus přes všechny vertikální body
	for (dy = (int)ry; dy >= 0; dy--)
	{
		dx = (int)(sqrt(1 - (double)dy*dy/ry/ry)*rx + 0.5);

		SetPaint(sx - dx		, sy + dy + ky	, col);
		SetPaint(sx + dx + kx	, sy + dy + ky	, col);
		SetPaint(sx - dx		, sy - dy		, col);
		SetPaint(sx + dx + kx	, sy - dy		, col);
	}
}


/////////////////////////////////////////////////////////////////////////////
// kreslení oválu

void SetElipFill(int x1, int y1, int x2, int y2, BYTE col)
{
// lokální proměnné
	int x,y;							// ukazatele bodu
	double sx, sy;						// střed elipsy
	double dx,dy;						// vzdálenost bodu od středu
	double rx, ry;						// poloměr ve směru X a Y

// oprava pořadí souřadnic X
	if (x1 > x2)
	{
		x = x1;
		x1 = x2;
		x2 = x;
	}

// oprava pořadí souřadnic Y
	if (y1 > y2)
	{
		y = y1;
		y1 = y2;
		y2 = y;
	}

// poloměr elipsy
	rx = (double)(x2 + 1 - x1) / 2;
	ry = (double)(y2 + 1 - y1) / 2;
	rx *= rx;
	ry *= ry;

// střed elipsy
	sx = (double)(x1 + x2) / 2;			// souřadnice středu X
	sy = (double)(y1 + y2) / 2;			// souřadnice středu Y

// vykreslení elipsy
	for (x = x1; x <= x2; x++)
	{
		for (y = y1; y <= y2; y++)
		{
			dx = fabs((double)x - sx);			// vzdálenost X od středu
			dy = fabs((double)y - sy);			// vzdálenost Y od středu

			if ((dy*dy/ry) <= (1 - dx*dx/rx))
			{
				SetPoint(x,y,col);
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// kreslení koule

void SetKoule(int x1, int y1, int x2, int y2, BYTE col0)
{
// lokální proměnné
	int i;								// pracovní čítač
	int x,y;							// ukazatele bodu
	int d;								// průměr koule
	double sx, sy;						// střed koule
	double dx,dy;						// vzdálenost bodu od středu
	int dxcol2, dycol2;					// kvadráty vzdálenosti pro barvu
	int sxcol, sycol;					// střed pro barvu

	BYTE col[20];						// buffer palet

// příprava počtu barev
	int cols;							// počet barev
	if (col0 >= StdColors - 2*ColLev)
	{
		cols = StdColors - col0;
	}
	else
	{
		cols = (col0 - ResCols + ColLev) / ColLev * ColLev + ResCols - col0 + 1;
	}

// příprava bufferu barev
	for (i = 0; i < cols-1; i++)
	{
		col[i] = (BYTE)(col0+i);
	}
	col[cols-1] = (BYTE)(StdColors - 1);

// oprava pořadí souřadnic X
	if (x1 > x2)
	{
		x = x1;
		x1 = x2;
		x2 = x;
	}

// oprava pořadí souřadnic Y
	if (y1 > y2)
	{
		y = y1;
		y1 = y2;
		y2 = y;
	}

// průměr koule (včetně koncových bodů)
	d = (int)(sqrt((double)((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1)))+0.4) + 1;

// střed koule (zatím zaokrouhlené na celé body)
	sx = (int)((x1 + x2) / 2);		// souřadnice středu X
	sy = (int)((y1 + y2) / 2);		// souřadnice středu Y

// střed pro barvu
	sxcol = (int)(sx - d/4);
	sycol = (int)(sy - d/4);

// korekce středu pro sudý průměr
	if ((d & 1) == 0)
	{
		sx += 0.5;
		sy += 0.5;
	}

// příprava minimálních a maximálních souřadnic
	x1 = (int)sx - d/2 - 2;
	x2 = (int)sx + d/2 + 2;
	y1 = (int)sy - d/2 - 2;
	y2 = (int)sy + d/2 + 2;

// vykreslení koule
	for (x = x1; x <= x2; x++)			// cyklus přes všechny pozice na řádku
	{
		for (y = y1; y <= y2; y++)		// cyklus přes všechny řádky
		{
			dx = fabs((double)x - sx)*2;		// vzdálenost od středu ve směru X
			dy = fabs((double)y - sy)*2;		// vzdálenost od středu ve směru Y

			dxcol2 = abs(x - sxcol) * 2;
			dxcol2 *= dxcol2;
			dycol2 = abs(y - sycol) * 2;
			dycol2 *= dycol2;

			if (dx*dx + dy*dy <= (double)d*d)	// leží bod uvnitř kruhu ?
			{
				i = (int)(cols * 2.2 * sqrt((double)(dxcol2 + dycol2)) /d);

				switch(i & 3)
				{
				case 0:
					i = i/4;
					break;

				case 1:
					i = i/4 + (x & y &1);
					break;

				case 2:
					i = i/4 + ((x + y) & 1);
					break;

				default:
					i = i/4 + ((x | y) & 1);
				}

				if (i >= cols) i = cols - 1;

				SetPoint(x,y,col[i]);		// nastavení bodu kruhu
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// výplň (col = nová barva, fil = podklad)

void SetFill(int x, int y, BYTE col, BYTE fil)
{
	int min = x;
	int max = x;
	int i;

	SetPoint(x, y, col);

// výplň minimálním směrem
	while ((min > 0) && (GetPoint(min-1,y) == fil))
	{
		min--;
		SetPoint(min, y, col);
	}

// výplň maximálním směrem
	while ((max < Width - 1) && (GetPoint(max+1,y) == fil))
	{
		max++;
		SetPoint(max, y, col);
	}

// výplň směrem nahoru
	if (y > 0)
	{
		y--;
		for (i = min; i <= max; i++)
		{
			if (GetPoint(i, y) == fil)
				SetFill(i,y,col,fil);
		}
		y++;
	}

// výplň směrem dolů
	if (y < Height - 1)
	{
		y++;
		for (i = min; i <= max; i++)
		{
			if (GetPoint(i, y) == fil)
				SetFill(i,y,col,fil);
		}
		y--;
	}
}


/////////////////////////////////////////////////////////////////////////////
// zobrazení textu

void ReDispText()
{
// vypnutí označení bloku
	DispSelect();

// příprava barvy
	BYTE col = ColLeft;							// barva písma
	BlokBack = 0;								// barva pozadí

// vytvoření bufferu MONO bitmapy
	int width0 = (Width + 31) & ~0x1f;			// zarovnaná šířka (vyžaduje to GetDIBits)
	int widthbyte = width0/8;
	BYTE* buf = (BYTE*)MemGet(widthbyte * Height);
	MemFill(buf, widthbyte * Height, -1);

// vytvoření mono bitmapy
	HBITMAP bmp = ::CreateBitmap(width0, Height, 1, 1, buf);
	ASSERT(bmp);

// otevření DC displeje
	HDC dc = ::GetDC(MainFrame);
	ASSERT(dc);
	if (dc && bmp)
	{

// vytvoření kompatibilního DC
		HDC dc2 = ::CreateCompatibleDC(dc);
		ASSERT(dc2);
		if (dc2)
		{

// výběr bitmapy do DC
			HBITMAP oldbmp = (HBITMAP)::SelectObject(dc2, bmp);

// vytvoření fontu
			HFONT font = ::CreateFont(FontSize, 0, 0, 0, 
				FontWeight, FontItalic, FALSE, FALSE,
				FontCharSet, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, FontName);

// výběr fontu
			HFONT oldfont = (HFONT)::SelectObject(dc2, font);

// výpis textu
			::TextOut(dc2, 0, 0, FontText, FontText.Length());

// navrácení původního fontu
			::SelectObject(dc2, oldfont);

// zrušení vytvořeného fontu
			::DeleteObject(font);

// navrácení původní bitmapy
			::SelectObject(dc2, oldbmp);

// načtení dat zpět do bufferu
			struct {BITMAPINFOHEADER bmiHeader; RGBQUAD bmiColors[2]; } bmpinfo;
			bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmpinfo.bmiHeader.biWidth = width0;
			bmpinfo.bmiHeader.biHeight = Height;
			bmpinfo.bmiHeader.biPlanes = 1;
			bmpinfo.bmiHeader.biBitCount = 1;
			bmpinfo.bmiHeader.biCompression = BI_RGB;
			bmpinfo.bmiHeader.biSizeImage = 0;
			bmpinfo.bmiHeader.biXPelsPerMeter = 0;
			bmpinfo.bmiHeader.biYPelsPerMeter = 0;
			bmpinfo.bmiHeader.biClrUsed = 0;
			bmpinfo.bmiHeader.biClrImportant = 0;
			::GetDIBits(dc2, bmp, 0, Height, buf, (BITMAPINFO*) &bmpinfo, DIB_RGB_COLORS);

// zjištění výšky textu
			BYTE* dst;
			int height = Height;
			int width = (Width + 7)/8 - 1;
			for (; height > (abs(FontSize)+4); height--)
			{
				dst = &buf[(Height - height) * widthbyte];
				if ((*dst != 0xff) || !MemCompare(dst, dst+1, width))
					break;
			}

			BlokHeight = height;

			char* src = (char*)&buf[widthbyte*(Height - height)];
			BlokWidth = Width;
			BlokBuf = (BYTE*)MemSize(BlokBuf, BlokWidth*BlokHeight * sizeof(BYTE));
			MemFill(BlokBuf, BlokWidth*BlokHeight, 0);

// přenesení dat do obrázku
			dst = BlokBuf;
			char data;

			for (int i = BlokHeight; i > 0; i--)
			{
				int k = 8;
				char* src2 = src;
				data = *src2;
				src2++;

				for (int j = BlokWidth; j > 0; j--)
				{
					if (data >= 0)
					{
						*dst = col;
					}
					dst++;
					data <<= 1;

					k--;
					if (k == 0)
					{
						data = *src2;
						src2++;
						k = 8;
					}
				}

				src += widthbyte;
			}

// zrušení pracovního DC
			::DeleteDC(dc2);
		}

// uvolnění DC
		::ReleaseDC(MainFrame, dc);
	}

// zrušení bitmapy
	::DeleteObject(bmp);

// zrušení bufferu
	buf = (BYTE*)MemSize(buf, 0);

// korekce počátku zobrazení bloku
	if (BlokX > (DispLeft + DispWidth - 5)) BlokX = DispLeft + DispWidth - 5;
	if (BlokY > (Height - DispBottom - 5)) BlokY = Height - DispBottom - 5;
	if ((BlokX + BlokWidth) < (DispLeft + 5)) BlokX = DispLeft + 5 - BlokWidth;
	if ((BlokY + BlokHeight) < (Height - DispBottom - DispHeight + 5)) 
		BlokY = Height - DispBottom - DispHeight + 5 - BlokHeight;

// překreslení bloku
	MoveSelect();

// zapnutí označení bloku
	DispSelect();

// překreslení obrazovky
	Disp();
}


// ************************* obsluha bloku **********************************


/////////////////////////////////////////////////////////////////////////////
// načtení bodu z bloku (nekontroluje souřadnice !)

inline BYTE _fastcall GetBlok(int x, int y)
{
	return BlokBuf[BlokWidth * (BlokHeight-1-y) + x];
}


/////////////////////////////////////////////////////////////////////////////
// nastavení bodu v bloku (nekontroluje souřadnice !)

inline void _fastcall SetBlok(int x, int y, BYTE col)
{
	BlokBuf[BlokWidth * (BlokHeight-1-y) + x] = col;
}


/////////////////////////////////////////////////////////////////////////////
// zapnutí/vypnutí výběru bloku

void DispSelect()
{
// kontrola, zda je blok zobrazen
	if (!Selecting) return;

// příprava DC displeje
	HDC dc = ::GetDC(MainFrame);

// omezující oblast
	RECT clip;

	clip.left = DLeft - 1;
	if (clip.left < EditX) clip.left = EditX;

	clip.top = DTop;
	if (clip.top < EditY) clip.top = EditY;

	clip.right = DLeft + DWidth;
	if (clip.right >= EditX + EditWidth) clip.right = EditX + EditWidth;

	clip.bottom = DTop + DHeight + 1;
	if (clip.bottom >= EditY + EditHeight) clip.bottom = EditY + EditHeight;

// výběr štětce
	HBRUSH oldBrush = (HBRUSH)::SelectObject(dc, HalfToneBrush);

// šířka čáry
	int s = Zoom/2;
	if (s == 0) s++;

// příprava souřadnic obdélníku
	int x1 = BlokX - DispLeft;
	int x2 = x1 + BlokWidth;
	int y1 = BlokY - (Height - DispBottom - DispHeight);
	int y2 = y1 + BlokHeight;

// výpočet souřadnic v bodech
	x1 = x1*Zoom + DLeft - 1;
	x2 = x2*Zoom + DLeft;
	y1 = y1*Zoom + DTop;
	y2 = y2*Zoom + DTop + 1;

// omezení souřadnic
	int x12 = x1; 
	if (x12 < clip.left) x12 = clip.left;

	int y12 = y1;
	if (y12 < clip.top) y12 = clip.top - s;

	int x22 = x2;
	if (x22 > clip.right) x22 = clip.right;

	int y22 = y2;
	if (y22 > clip.bottom) y22 = clip.bottom + s;

// kontrola souřadnic
	if ((x12 < x22) && (y12 < y22))
	{

// horní linka
		if (y1 >= clip.top)
		{
			::PatBlt(dc, x12, y12, x22-x12, s, PATINVERT);
		}

// spodní linka
		if (y2 <= clip.bottom)
		{
			::PatBlt(dc, x12, y22-s, x22-x12, s, PATINVERT);
		}

// levá linka
		if (x1 >= clip.left)
		{
			::PatBlt(dc, x12, y12+s, s, y22-y12-2*s, PATINVERT);
		}

// pravá linka
		if (x2 <= clip.right)
		{
			::PatBlt(dc, x22-s, y12+s, s, y22-y12-2*s, PATINVERT);
		}
	}

// navrácení původního štětce
	::SelectObject(dc, oldBrush);

// uvolnění DC displeje
	::ReleaseDC(MainFrame, dc);
}


/////////////////////////////////////////////////////////////////////////////
// zobrazení bloku při přesunu

void MoveSelect()
{
// lokální proměnné
	int x;
	int y;
	int width;
	int height;
	int x2;
	int y2;
	int i;
	BYTE col;
	BYTE back = BlokBack;

// návrat obsahu obrázku
	Pop();

// souřadnice k vymazání pozadí
	x = FirstBlokX;
	y = FirstBlokY;
	width = FirstBlokWidth;
	height = FirstBlokHeight;

// test, zda se zobrazí podklad pod blokem
	if ((CutMode) && 
		(x < Width) && 
		(y < Height) && 
		(width > 0) && 
		(height > 0) &&
		((x + width) > 0) &&
		((y + height) > 0))
	{

// minimální počátek X
		if (x < 0)
		{
			width += x;
			x = 0;
		}

// minimální počátek Y
		if (y < 0)
		{
			height += y;
			y = 0;
		}

// maximální šířka
		if ((x + width) > Width)
		{
			width = Width - x;
		}

// maximální výška
		if ((y + height) > Height)
		{
			height = Height - y;
		}

// vymazání podkladu pod blokem
		for (; height > 0; height--)
		{
			MemFill(&Data[(Height - 1 - y)*Width + x], width, ColRight);
			y++;
		}
	}

// souřadnice k zobrazení bloku
	x = BlokX;
	y = BlokY;
	width = BlokWidth;
	height = BlokHeight;
	x2 = 0;
	y2 = 0;

// test, zda se zobrazí blok
	if ((x < Width) && 
		(y < Height) && 
		(width > 0) && 
		(height > 0) &&
		((x + width) > 0) &&
		((y + height) > 0))
	{

// minimální počátek X
		if (x < 0)
		{
			width += x;
			x2 = -x;
			x = 0;
		}

// minimální počátek Y
		if (y < 0)
		{
			height += y;
			y2 = -y;
			y = 0;
		}

// maximální šířka
		if ((x + width) > Width)
		{
			width = Width - x;
		}

// maximální výška
		if ((y + height) > Height)
		{
			height = Height - y;
		}

// zobrazení bloku
		for (; height > 0; height--)
		{
			for (i = 0; i < width; i++)
			{
				col = GetBlok(x2+i, y2);
				if (col != back)
				{
					Data[(Height-1-y)*Width+x+i] = col;
				}
			}
			y++;
			y2++;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// test bodu, zda je uvnitř vybraného bloku

BOOL TestBlok(int x, int y)
{
	return (
				(x >= BlokX) &&
				(x < BlokX + BlokWidth) &&
				(y >= BlokY) &&
				(y < BlokY + BlokHeight)
			);
}


/////////////////////////////////////////////////////////////////////////////
// zrušení bloku

void Delete()
{
	if (Selecting)
	{
		DispSelect();			// vypnutí zobrazení výběru bloku
		BlokWidth = 0;			// neplatná šířka bloku
		BlokHeight = 0;			// neplatná výška bloku
		MoveSelect();			// překreslení ikony
		Moving = FALSE;			// konec posunu bloku
		Selecting = FALSE;		// zrušení příznaku výběru bloku
		Editing = FALSE;		// zrušení příznaku editace
		Disp();				// překreslení displeje
		SetModi();					// nastavení příznaku modifikace
		UpdateTree();			// překreslení ikon ve stromech
	}

// aktualizace voleb bloku
	UpdateClipboard();
}


/////////////////////////////////////////////////////////////////////////////
// otisk bloku

void OnOtisk()
{
// kontrola, zda je režim editace
	if (ProgMode &&
		((EditMode == BufIcoID) || (EditMode == BufPicID)) &&
		Selecting)
	{
		PushUndo();
		Push();				// úschova do pomocného bufferu
		CutMode = FALSE;

// příznak modifikace souboru
		SetModi();									// nastavení příznaku modifikace

// aktualizace zobrazení stromů
		UpdateTree();
	}
}


/////////////////////////////////////////////////////////////////////////////
// kopie bloku do schránky (FALSE = chyba)

BOOL Copy()
{
	if (BufID == BufPicID)
	{
		if (Index == -2)
		{
			Sprite[EditItemSprite][EditItemPic].DeComp();
			Data = Sprite[EditItemSprite][EditItemPic].DataData();
		}
		else
		{
			Picture[Index].DeComp();
			Data = Picture[Index].DataData();
		}
	}
	else
	{
		Icon[Index].DeComp();
		Data = Icon[Index].DataData();
	}


// lokální proměnné
	BYTE*		src;			// adresa zdrojových dat
	int			width;			// šířka bitmapy v bodech
	int			height;			// výška bitmapy
	int			dstinc;			// přírustek cílové adresy linky
	int			i;				// pracovní čítač
	int			delka;			// délka dat v bufferu celkem
	HGLOBAL		global;			// globální buffer s daty
	BITMAPINFO* bitmap;			// ukazatel na data v globálním bufferu

// otevření schránky
	if (!::OpenClipboard(MainFrame))
		return FALSE;

// příprava ukazatelů dat
	if (Selecting)
	{
		width = BlokWidth;		// šířka bloku
		height = BlokHeight;	// výška bloku
		src = BlokBuf;			// data jsou v bufferu bloku
	}
	else
	{
		width = Width;
		height = Height;
		src = Data;
	}

// vyprázdnění schránky
	if (!::EmptyClipboard())
	{
		::CloseClipboard();
		return FALSE;
	}

// příprava velikosti dat
	dstinc = (width+3) & ~3; // přírustek cílové adresy linky
	delka = dstinc * height + sizeof(BITMAPINFO) + sizeof(RGBQUAD)*255;

// vytvoření globálního bufferu pro data
	global = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, delka);
	if (global == NULL)
	{
		::CloseClipboard();
		return FALSE;
	}

// uzamknutí bufferu
	bitmap = (BITMAPINFO*) ::GlobalLock(global);

// kopie záhlaví bitmapy s paletami do bufferu
	MemCopy(bitmap, StdBitmapInfo, sizeof(BITMAPINFO) + sizeof(RGBQUAD)*255);

// kopie dat obrázku
	for (i = 0; i < height; i++)
	{
		MemCopy(&((BYTE*)bitmap)[sizeof(BITMAPINFO) + sizeof(RGBQUAD)*255 
			+ i*dstinc], &src[i*width], width);
	}

// nastavení parametrů obrázku
	bitmap->bmiHeader.biWidth = width;
	bitmap->bmiHeader.biHeight = height;

// odemknutí bufferu
	::GlobalUnlock(global);

// uložení dat do schránky
	::SetClipboardData(CF_DIB, global);

// uzavření schránky
	::CloseClipboard();

// aktualizace voleb bloku
	UpdateClipboard();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// vystřihnutí bloku do schránky

void Cut()
{
	if (Selecting && Copy())
	{
		Delete();
	}

// aktualizace voleb bloku
	UpdateClipboard();
}

/////////////////////////////////////////////////////////////////////////////
// návrat bloku ze schránky

void Paste()
{
// lokální proměnné
	HGLOBAL		global;			// globální buffer s daty
	BITMAPINFO* bitmap;			// ukazatel na data v globálním bufferu
	int			width;			// šířka bitmapy v bodech
	int			height;			// výška bitmapy
	int			palet;			// počet palet v bitmapě
	int			srcinc;			// přírustek zdrojové adresy linky
	int			i, j;			// pracovní čítače
	BYTE*		src;			// ukazatel zdrojové adresy
	BYTE*		dst;			// ukazatel cílové adresy
	WORD		srcdat;			// zdrojová data 16 bitů
	BYTE		r,g,b;			// složky barev

// otevření schránky
	if (!::OpenClipboard(MainFrame))
		return;

// načtení dat schránky
	global = ::GetClipboardData(CF_DIB);
	if (global == NULL)
	{
		::CloseClipboard();
		return;
	}
	
// uzamknutí bufferu
	bitmap = (BITMAPINFO*) ::GlobalLock(global);

// kontrola formátu bitmapy
	if	(TestBMP(bitmap) && (bitmap->bmiHeader.biCompression == BI_RGB))
	{

// zapnutí módu bloku
		SetMode(IDN_SELECT);

// vypnutí označení bloku
		if (Selecting)
		{
			DispSelect();			// vypnutí zobrazení výběru bloku
			Moving = FALSE;		// konec posunu bloku
			Selecting = FALSE;	// zrušení příznaku výběru bloku
			Editing = FALSE;		// zrušení příznaku editace
		}

// úschova ikony do bufferu
		PushUndo();			// úschova ikony do undo bufferu
		Push();				// úschova ikony do bufferu

// příprava parametrů bitmapy
		width = bitmap->bmiHeader.biWidth;	// šířka
		height = bitmap->bmiHeader.biHeight; // výška

// vytvoření bufferu k uložení dat
		BlokBuf = (BYTE*)MemSize(BlokBuf, width*height * sizeof(BYTE));				// vytvoření bufferu pro blok
		dst = BlokBuf;							// ukazatel cílové adresy
		src = (BYTE*)&bitmap->bmiColors[0];		// začátek zdrojových dat

// příprava bufferu odchylky pro dithering
		int* odch = NULL;
		if (Dither)
		{
			odch = (int*)MemGet((3*width + 6) * sizeof(int));
			MemFill(odch, (3*width + 6) * sizeof(int), 0);
		}

// rozlišení podle počtu bodů
		switch (bitmap->bmiHeader.biBitCount)
		{

// 1 bit
		case 1:
			srcinc = ((width+7)/8 + 3) & ~3;	// přírustek zdrojové adresy

// příprava konverzní tabulky palet
			GenKonvPal(bitmap);					// příprava konverzní tabulky

// příprava počtu palet
			palet = bitmap->bmiHeader.biClrUsed; // počet palet v tabulce
			if (palet == 0) palet = 2;			// používá se maximální počet palet
			src += sizeof(RGBQUAD) * palet;		// korekce zdrojové adresy

// kopie dat do bufferu bloku
			for (i = height-1; i >= 0; i--)		// cyklus přes všechny linky
			{
				for (j = 0; j < width; j++)
				{
					*dst = KonvPal[(src[j/8] >> (7 - (j & 7))) & 1];
					dst++;
				}
				src += srcinc;					// zvýšení zdrojové adresy
			}
			break;

// 4 bity
		case 4:
			srcinc = ((width+1)/2 + 3) & ~3;	// přírustek zdrojové adresy

// příprava konverzní tabulky palet
			GenKonvPal(bitmap);					// příprava konverzní tabulky

// příprava počtu palet
			palet = bitmap->bmiHeader.biClrUsed; // počet palet v tabulce
			if (palet == 0) palet = 16;			// používá se maximální počet palet
			src += sizeof(RGBQUAD) * palet;		// korekce zdrojové adresy

// kopie dat do bufferu bloku
			for (i = height-1; i >= 0; i--)		// cyklus přes všechny linky
			{
				for (j = 0; j < width; j++)
				{
					if (j & 1)
					{
						*dst = KonvPal[src[j/2] & 0xF];
					}
					else
					{
						*dst = KonvPal[src[j/2] >> 4];
					}
					dst++;
				}
				src += srcinc;					// zvýšení zdrojové adresy
			}
			break;

// 8 bitů
		case 8:
			srcinc = (width + 3) & ~3;			// přírustek zdrojové adresy

// příprava konverzní tabulky palet
			GenKonvPal(bitmap);					// příprava konverzní tabulky

// příprava počtu palet
			palet = bitmap->bmiHeader.biClrUsed; // počet palet v tabulce
			if (palet == 0) palet = 256;		// používá se maximální počet palet
			src += sizeof(RGBQUAD) * palet;		// korekce zdrojové adresy

// kopie dat do bufferu bloku
			for (i = height-1; i >= 0; i--)		// cyklus přes všechny linky
			{
				if (Dither)
				{
					int* odch0 = odch + 3;			// ukazatel v bufferu odchylek

					for (j = width; j > 0; j--)		// cyklus přes všechny body
					{
					// bod k zápisu
						BYTE col = *src;
						src++;
						RGBQUAD* rgb = bitmap->bmiColors + col;

					// pozadí
						if ((*(int*)rgb & 0xffffff) == (BACKCOLOR_BLUE | (BACKCOLOR_GREEN*256) | (BACKCOLOR_RED*256*256)))
						{
							col = BackCol;

							*odch0 = 0;
							odch0++;
							*odch0 = 0;
							odch0++;
							*odch0 = 0;
							odch0++;
						}
						else
						{	

				// stín
						  if ((*(int*)rgb & 0xffffff) == (SHADCOLOR_BLUE | (SHADCOLOR_GREEN*256) | (SHADCOLOR_RED*256*256)))
						  {
							col = ShadCol;

							*odch0 = 0;
							odch0++;
							*odch0 = 0;
							odch0++;
							*odch0 = 0;
							odch0++;
						  }
						  else
						  {	

					// požadovaná barva
							b = rgb->rgbBlue;			// modrá složka
							g = rgb->rgbGreen;			// zelená složka
							r = rgb->rgbRed;			// červená složka

					// zkorigovaná barva
							int b2 = b - (odch0[-3] + odch0[0] + odch0[3])*5/8;		// modrá složka
							int g2 = g - (odch0[-2] + odch0[1] + odch0[4])*5/8;		// zelená složka
							int r2 = r - (odch0[-1] + odch0[2] + odch0[5])*5/8;		// červená složka

					// omezení přetečení barvy
							if (b2 < 0) b2 = 0;
							if (b2 > 255) b2 = 255;
							if (g2 < 0) g2 = 0;
							if (g2 > 255) g2 = 255;
							if (r2 < 0) r2 = 0;
							if (r2 > 255) r2 = 255;

					// import barvy
							col = PalImport((BYTE)r2, (BYTE)g2, (BYTE)b2);

					// odchylka aktuálního bodu od požadované barvy
							rgb = StdBitmapInfo->bmiColors + col;
							*odch0 = rgb->rgbBlue - b;
							odch0++;

							*odch0 = rgb->rgbGreen - g;
							odch0++;

							*odch0 = rgb->rgbRed - r;
							odch0++;
						  }
						}

					// uložení bodu
						*dst = col;
						dst++;						// zvýšení cílové adresy
					}
					src -= width;
				}
				else
				{
					KonvCopy(dst, src, width);
					dst += width;					// zvýšení cílové adresy
				}
				src += srcinc;					// zvýšení zdrojové adresy
			}
			break;

// 16 bitů
		case 16:
			srcinc = ((2*width + 3) & ~3) - 2*width;

			for (i = height; i > 0; i--)	// cyklus přes všechny linky
			{
				if (Dither)
				{
					int* odch0 = odch + 3;			// ukazatel v bufferu odchylek

					for (j = width; j > 0; j--)		// cyklus přes všechny body na lince
					{

					// požadovaná barva
						srcdat = *(WORD*)src;		// data jednoho bodu
						b = (BYTE)(srcdat & 0x1F);	// modrá složka
						b = (BYTE)(b*8 + b/4);
						srcdat >>= 5;				// zrušení bitů modré složky
						g = (BYTE)(srcdat & 0x1F);	// zelená složka
						g = (BYTE)(g*8 + g/4);
						srcdat >>= 5;				// zrušení bitů zelené složky
						r = (BYTE)(srcdat & 0x1F);	// červená složka
						r = (BYTE)(r*8 + r/4);
						src++;						// zvýšení zdrojové adresy
						src++;						// zvýšení zdrojové adresy

					// zkorigovaná barva
						int b2 = b - (odch0[-3] + odch0[0] + odch0[3])*5/8;		// modrá složka
						int g2 = g - (odch0[-2] + odch0[1] + odch0[4])*5/8;		// zelená složka
						int r2 = r - (odch0[-1] + odch0[2] + odch0[5])*5/8;		// červená složka

					// omezení přetečení barvy
						if (b2 < 0) b2 = 0;
						if (b2 > 255) b2 = 255;
						if (g2 < 0) g2 = 0;
						if (g2 > 255) g2 = 255;
						if (r2 < 0) r2 = 0;
						if (r2 > 255) r2 = 255;

					// import barvy
						BYTE col = PalImport((BYTE)r2, (BYTE)g2, (BYTE)b2);

					// odchylka aktuálního bodu od požadované barvy
						RGBQUAD* rgb = StdBitmapInfo->bmiColors + col;
						*odch0 = rgb->rgbBlue - b;
						odch0++;

						*odch0 = rgb->rgbGreen - g;
						odch0++;

						*odch0 = rgb->rgbRed - r;
						odch0++;

					// uložení bodu
						*dst = col;	// import barvy do vlastních palet
						dst++;						// zvýšení cílové adresy
					}
				}
				else
				{
					for (j = width; j > 0; j--)		// cyklus přes všechny body
					{
						srcdat = *(WORD*)src;		// data jednoho bodu
						b = (BYTE)(srcdat & 0x1F);	// modrá složka
						b = (BYTE)(b*8 + b/4);
						srcdat >>= 5;				// zrušení bitů modré složky
						g = (BYTE)(srcdat & 0x1F);	// zelená složka
						g = (BYTE)(g*8 + g/4);
						srcdat >>= 5;				// zrušení bitů zelené složky
						r = (BYTE)(srcdat & 0x1F);	// červená složka
						r = (BYTE)(r*8 + r/4);
						*dst = PalImport(r, g, b);	// import barvy do vlastních palet
						src++;						// zvýšení zdrojové adresy
						src++;
						dst++;						// zvýšení cílové adresy
					}
				}
				src += srcinc;
			}
			break;

// 24 bitů
		case 24:
			srcinc = ((3*width + 3) & ~3) - 3*width;
			for (i = height; i > 0; i--)	// cyklus přes všechny linky
			{
				if (Dither)
				{
					int* odch0 = odch + 3;			// ukazatel v bufferu odchylek

					for (j = width; j > 0; j--)		// cyklus přes všechny body na lince
					{

					// pozadí
						if ((*(int*)src & 0xffffff) == (BACKCOLOR_BLUE | (BACKCOLOR_GREEN*256) | (BACKCOLOR_RED*256*256)))
						{
							*dst = BackCol;
							src += 3;

							*odch0 = 0;
							odch0++;
							*odch0 = 0;
							odch0++;
							*odch0 = 0;
							odch0++;
						}
						else
						{	

				// stín
						  if ((*(int*)src & 0xffffff) == (SHADCOLOR_BLUE | (SHADCOLOR_GREEN*256) | (SHADCOLOR_RED*256*256)))
						  {
							*dst = ShadCol;
							src += 3;

							*odch0 = 0;
							odch0++;
							*odch0 = 0;
							odch0++;
							*odch0 = 0;
							odch0++;
						  }
						  else
						  {	

					// požadovaná barva
							b = *src;					// modrá složka
							src++;						// zvýšení zdrojové adresy
							g = *src;					// zelená složka
							src++;						// zvýšení zdrojové adresy
							r = *src;					// červená složka
							src++;						// zvýšení zdrojové adresy

					// zkorigovaná barva
							int b2 = b - (odch0[-3] + odch0[0] + odch0[3])*5/8;		// modrá složka
							int g2 = g - (odch0[-2] + odch0[1] + odch0[4])*5/8;		// zelená složka
							int r2 = r - (odch0[-1] + odch0[2] + odch0[5])*5/8;		// červená složka

					// omezení přetečení barvy
							if (b2 < 0) b2 = 0;
							if (b2 > 255) b2 = 255;
							if (g2 < 0) g2 = 0;
							if (g2 > 255) g2 = 255;
							if (r2 < 0) r2 = 0;
							if (r2 > 255) r2 = 255;

					// import barvy
							BYTE col = PalImport((BYTE)r2, (BYTE)g2, (BYTE)b2);

					// odchylka aktuálního bodu od požadované barvy
							RGBQUAD* rgb = StdBitmapInfo->bmiColors + col;
							*odch0 = rgb->rgbBlue - b;
							odch0++;

							*odch0 = rgb->rgbGreen - g;
							odch0++;

							*odch0 = rgb->rgbRed - r;
							odch0++;

					// uložení bodu
							*dst = col;	// import barvy do vlastních palet
						  }
						}
						dst++;						// zvýšení cílové adresy
					}
				}
				else
				{
					for (j = width; j > 0; j--)		// cyklus přes všechny body
					{
						if ((*(int*)src & 0xffffff) == (BACKCOLOR_BLUE | (BACKCOLOR_GREEN*256) | (BACKCOLOR_RED*256*256)))
						{
							*dst = BackCol;
							src += 3;
						}
						else
						{
				// stín
						  if ((*(int*)src & 0xffffff) == (SHADCOLOR_BLUE | (SHADCOLOR_GREEN*256) | (SHADCOLOR_RED*256*256)))
						  {
							*dst = ShadCol;
							src += 3;
						  }
						  else
						  {	
							b = *src;					// modrá složka
							src++;						// zvýšení zdrojové adresy
							g = *src;					// zelená složka
							src++;						// zvýšení zdrojové adresy
							r = *src;					// červená složka
							src++;						// zvýšení zdrojové adresy
							*dst = PalImport(r, g, b);	// import barvy do vlastních palet
						  }
						}
						dst++;						// zvýšení cílové adresy
					}
				}
				src += srcinc;
			}
			break;

// 32 bitů
		case 32:
			for (i = height; i > 0; i--)		// cyklus přes všechny linky
			{
				if (Dither)
				{
					int* odch0 = odch + 3;			// ukazatel v bufferu odchylek

					for (j = width; j > 0; j--)		// cyklus přes všechny body na lince
					{

					// pozadí
						if ((*(int*)src & 0xffffff) == (BACKCOLOR_BLUE | (BACKCOLOR_GREEN*256) | (BACKCOLOR_RED*256*256)))
						{
							*dst = BackCol;
							src += 4;

							*odch0 = 0;
							odch0++;
							*odch0 = 0;
							odch0++;
							*odch0 = 0;
							odch0++;
						}
						else
						{
							
				// stín
						  if ((*(int*)src & 0xffffff) == (SHADCOLOR_BLUE | (SHADCOLOR_GREEN*256) | (SHADCOLOR_RED*256*256)))
						  {
							*dst = ShadCol;
							src += 4;

							*odch0 = 0;
							odch0++;
							*odch0 = 0;
							odch0++;
							*odch0 = 0;
							odch0++;
						  }
						  else
						  {	

					// požadovaná barva
							b = *src;					// modrá složka
							src++;						// zvýšení zdrojové adresy
							g = *src;					// zelená složka
							src++;						// zvýšení zdrojové adresy
							r = *src;					// červená složka
							src++;						// zvýšení zdrojové adresy
							src++;						// zvýšení zdrojové adresy

					// zkorigovaná barva
							int b2 = b - (odch0[-3] + odch0[0] + odch0[3])*5/8;		// modrá složka
							int g2 = g - (odch0[-2] + odch0[1] + odch0[4])*5/8;		// zelená složka
							int r2 = r - (odch0[-1] + odch0[2] + odch0[5])*5/8;		// červená složka

					// omezení přetečení barvy
							if (b2 < 0) b2 = 0;
							if (b2 > 255) b2 = 255;
							if (g2 < 0) g2 = 0;
							if (g2 > 255) g2 = 255;
							if (r2 < 0) r2 = 0;
							if (r2 > 255) r2 = 255;

					// import barvy
							BYTE col = PalImport((BYTE)r2, (BYTE)g2, (BYTE)b2);

					// odchylka aktuálního bodu od požadované barvy
							RGBQUAD* rgb = StdBitmapInfo->bmiColors + col;
							*odch0 = rgb->rgbBlue - b;
							odch0++;

							*odch0 = rgb->rgbGreen - g;
							odch0++;

							*odch0 = rgb->rgbRed - r;
							odch0++;

					// uložení bodu
							*dst = col;	// import barvy do vlastních palet
						  }
						}
						dst++;						// zvýšení cílové adresy
					}
				}
				else
				{
					for (j = width; j > 0; j--)		// cyklus přes všechny body
					{
						if ((*(int*)src & 0xffffff) == (BACKCOLOR_BLUE | (BACKCOLOR_GREEN*256) | (BACKCOLOR_RED*256*256)))
						{
							*dst = BackCol;
							src += 4;
						}
						else
						{
				// stín
						  if ((*(int*)src & 0xffffff) == (SHADCOLOR_BLUE | (SHADCOLOR_GREEN*256) | (SHADCOLOR_RED*256*256)))
						  {
							*dst = ShadCol;
							src += 4;
						  }
						  else
						  {	
							b = *src;					// modrá složka
							src++;						// zvýšení zdrojové adresy
							g = *src;					// zelená složka
							src++;						// zvýšení zdrojové adresy
							r = *src;					// červená složka
							src++;						// zvýšení zdrojové adresy
							src++;						// zvýšení zdrojové adresy
							*dst = PalImport(r, g, b);	// import barvy do vlastních palet
						  }
						}
						dst++;						// zvýšení cílové adresy
					}
				}
			}
			break;
		}

// korekce počátku k zobrazení bloku
		if ((BlokX > (DispLeft+DispWidth-5)) ||
			(BlokY > (Height-DispBottom-5)) ||
			((BlokX + BlokWidth) < (DispLeft+5)) ||
			((BlokY + BlokHeight) < (Height - DispBottom - DispHeight + 5)))
		{
			BlokX = DispLeft + DispWidth/2 - width/2;
			BlokY = Height - DispBottom - DispHeight/2 - height/2;
		}
		BlokWidth = width;
		BlokHeight = height;

// zahájení označování bloku
		BlokBack = ColRight;// průhledná barva bloku
		CutMode = FALSE;		// není vymazání bloku
		Selecting = TRUE;		// je mód výběru bloku
		Mode = IDN_SELECT; // zapnutí módu editace
		MoveSelect();			// překreslení ikony
		DispSelect();			// vypnutí zobrazení výběru bloku
		Disp();				// překreslení displeje

// uvolnění bufferu odchylky pro dithering
		MemFree(odch);
	}

// odemknutí bufferu
	::GlobalUnlock(global);

// uzavření schránky
	::CloseClipboard();

// příznak modifikace souboru
	SetModi();									// nastavení příznaku modifikace

// aktualizace zobrazení stromů
	UpdateTree();

// aktualizace voleb bloku
	UpdateClipboard();
}


/////////////////////////////////////////////////////////////////////////////
// označení celého bloku

void SelectAll()
{
// zapnutí módu bloku
	SetMode(IDN_SELECT);

// nastavení údajů pro označení všeho
	PushUndo();					// úschova obrázku do undo bufferu
	Push();						// úschova do pomocného bufferu
	Selecting = TRUE;			// příznak označení bloku
	BlokX = 0;					// počátek bloku X
	BlokY = 0;					// počátek bloku Y
	BlokWidth = Width;			// šířka bloku
	BlokHeight = Height;		// výška bloku
	CutMode = TRUE;				// jsou údaje k vymazání
	FirstBlokX = 0;				// výchozí počátek bloku X
	FirstBlokY = 0;				// výchozí počátek bloku Y
	FirstBlokWidth = Width;		// výchozí šířka bloku
	FirstBlokHeight = Height;	// výchozí výška bloku
	BlokBack = ColRight;		// průhledná barva bloku

// zobrazení označení bloku
	DispSelect();				// zapnutí označení bloku

// úschova dat do bufferu
	BlokBuf = (BYTE*)MemSize(BlokBuf, Width * Height * sizeof(BYTE));	// buffer pro blok

	for (int x = 0; x < Width; x++)
	{
		for (int y = 0; y < Height; y++)
		{
			SetBlok(x, y, GetPoint(x, y));
		}
	}

// aktualizace voleb bloku
	UpdateClipboard();
}


/////////////////////////////////////////////////////////////////////////////
// aktualizace voleb bloku

void UpdateClipboard()
{
	if (FocusBuf == BufEdiID)
	{
		EnableCommand(IDN_CUT, Selecting);
		EnableCommand(IDN_COPY, TRUE);
		EnableCommand(IDN_PASTE, ::IsClipboardFormatAvailable(CF_DIB));
		EnableCommand(IDN_CLEAR, Selecting);
		EnableCommand(IDN_ALL, TRUE);

		EnableCommand(IDN_OTISK, Selecting);
	}
}


// ****************************** obsluha UNDO ******************************


/////////////////////////////////////////////////////////////////////////////
// uložení obrázku do undo bufferu

void PushUndo()
{
	if (BufID == BufPicID)
	{
		if (Index == -2)
		{
			Undo.AddSprPicSet(EditItemSprite, EditItemPic, Sprite[EditItemSprite][EditItemPic]);
			Sprite[EditItemSprite].CopyWrite();
			Sprite[EditItemSprite][EditItemPic].CopyWrite();
			Sprite[EditItemSprite][EditItemPic].DeComp();
			Data = Sprite[EditItemSprite][EditItemPic].DataData();
		}
		else
		{
			Undo.AddPicSet(Index, Picture[Index]);
			Picture[Index].CopyWrite();
			Picture[Index].DeComp();
			Data = Picture[Index].DataData();
		}
	}
	else
	{
		Undo.AddIconSet(Index, Icon[Index]);
		Icon[Index].CopyWrite();
		Icon[Index].DeComp();
		Data = Icon[Index].DataData();
	}
	Undo.Stop();
}

}
