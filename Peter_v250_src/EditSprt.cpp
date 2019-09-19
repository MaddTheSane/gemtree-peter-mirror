
#include "Main.h"

/***************************************************************************\
*																			*
*								Editor sprajtů								*
*																			*
\***************************************************************************/

namespace EditSprite
{

/////////////////////////////////////////////////////////////////////////////
// parametry

// parametry editovaného sprajtu
int		Index = 0;						// editovaný sprajt
int		IndexPic = 0;					// index editovaného obrázku
int		Width;							// šířka plochy v políčkách (bez okrajů)
int		Height;							// výška plochy v políčkách (bez okrajů)
int		PicWidth;						// šířka obrázku v bodech
int		PicHeight;						// výška obrázku v bodech
int		KorWidth;						// zkorigovaná šířka obrázku (minimálně ICONWIDTH)
int		KorHeight;						// zkorigovaná výška obrázku (minimálně ICONHEIGHT)
int		MWidth;							// šířka plochy v bodech + okraj vlevo
int		MHeight;						// výška plochy v bodech + okraj nahoře

// zobrazená část plochy
int		TLeft;							// počáteční bod plochy vlevo
int		TTop;							// počáteční bod plochy nahoře

// zobrazení plochy na displeji včetně záhlaví (v bodech displeje)
int		DLeft;							// počátek zobrazení vlevo
int		DTop;							// počátek zobrazení nahoře
int		DWidth;							// zobrazená šířka části plochy na displeji
int		DHeight;						// zobrazená výška části plochy na displeji

int		ILeft;							// index ikony vlevo (0 = levý okraj)
int		IRight;							// index ikony vpravo (0 = levý okraj)
int		ITop;							// index ikony nahoře (0 = horní okraj)
int		IBottom;						// index ikony dole (0 = horní okraj)

int		OLeft;							// zobrazená část ikony vlevo
int		ORight;							// zobrazená část ikony vpravo
int		OTop;							// zobrazená část ikony nahoře
int		OBottom;						// zobrazená část ikony dole

// nastavení nových rozměrů a vlastností
int		NewDelay;						// nová prodleva mezi fázemi
double	NewStep;						// nový počet fází na krok
int		NewLevel;						// nová hladina k zobrazení
int		NewKlid;						// nový počet fází pro klid
int		NewWidth;						// nová šířka obrázku
int		NewHeight;						// nová výška obrázku
int		NewPohyb;						// nový počet fází pro pohyb
int		NewSmer;						// nový počet směrů
BOOL	NewFill = FALSE;				// probíhá plnění textů

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

CBufIcon	MapSwcNum;					// ikony čísel
CBufIcon	SpriteIcon;					// ikony sprajtů

// testovací mód
BOOL	TestMode = FALSE;				// je testovací mód
int		TestLeft = 0;					// levý okraj pro test
int		TestTop = 0;					// horní okraj pro test
#define	TestWidthN 15					// počet ikon horizontálně
#define	TestHeightN 11					// počet ikon vertikálně
#define TestWidth (TestWidthN*ICONWIDTH)	// šířka pro test
#define	TestHeight (TestHeightN*ICONHEIGHT)	// výška pro test
UINT	TestTimer = 0;					// časovač pro test
BYTE*	TestBuff = NULL;				// buffer s výstupním obrázkem
#define	TestTimerID 1434l				// ID časovače

int		TestKlid;						// počet klidových fází
int		TestSmeru;						// počet směrů
int		TestFazi;						// počet fází
int		TestDelay;						// prodleva mezi fázemi (v impulsech)
double	TestKroku;						// počet kroků na jednotku

int		TestFaze = 0;					// aktuální fáze sprajtu
int		TestSmer = 0;					// aktuální směr sprajtu
int		TestNext = 0;					// čítač do příštího kroku

int		TestPicWidth;					// šířka obrázku sprajtu
int		TestPicHeight;					// výška obrázku sprajtu
double	TestX;							// aktuální souřadnice X
double	TestY;							// aktuální souřadnice Y
double	TestEndX;						// cílová souřadnice X
double	TestEndY;						// cílová souřadnice Y

// ************************** obsluha volby editace *************************

/////////////////////////////////////////////////////////////////////////////
// inicializace při startu programu

void StartInit()
{
// načtení ikon čísel
	MapSwcNum.IconSize(8, 12);
	MapSwcNum.Load(IDN_MAPSWCNUM, 10);
	ASSERT(MapSwcNum.Num() == 10);

// načtení pomocných ikon sprajtů (záhlaví)
	SpriteIcon.Load(IDN_SPRITE_ICON, 26);
	ASSERT(SpriteIcon.Num() == 26);

// nastavení obrázků pro sprajty 0 a 1 (Petr a Petra) (mají velikost ICONSIZE)
	CPicture* pic = Sprite0.DataData();
	int j = 2;
	for (int i = 0; i < 8; i++)
	{
		SpriteIcon[j  ].DeComp();
		SpriteIcon[j+1].DeComp();
		SpriteIcon[j+2].DeComp();
		pic->CopyData(SpriteIcon[j  ].DataData()); pic++;
		pic->CopyData(SpriteIcon[j+1].DataData()); pic++;
		pic->CopyData(SpriteIcon[j  ].DataData()); pic++;
		pic->CopyData(SpriteIcon[j+2].DataData()); pic++;
		pic->CopyData(SpriteIcon[j  ].DataData()); pic++;

		if (i == 3) pic = Sprite1.DataData();

		j += 3;
	}
}

/////////////////////////////////////////////////////////////////////////////
// zahájení editace (musí být platný index!)

void BegEdit(int index)
{
	EndDrag();

// úschova indexu sprajtu
	Index = index;						// index sprajtu
	ASSERT(Sprite.IsValid(index));
	Sprite[index].CopyWrite();			// kopie před zápisem

// úschova parametrů sprajtu
	Width = Sprite[index].Faze();		// šířka plochy v políčkách
	Height = Sprite[index].Smer();		// výška plochy v políčkách
	PicWidth = Sprite[index].Width();	// šířka obrázku sprajtu
	KorWidth = PicWidth;				// zkorigovaná šířka obrázku sprajtu
	if (KorWidth < ICONWIDTH) KorWidth = ICONWIDTH;
	PicHeight = Sprite[index].Height();	// výška obrázku sprajtu
	KorHeight = PicHeight;				// zkorigovaná výška sprajtu
	if (KorHeight < ICONHEIGHT) KorHeight = ICONHEIGHT;

// přepočet rozložení a zobrazení obrázku
	Disp();
}

/////////////////////////////////////////////////////////////////////////////
// obsluha dialogu pro zadání vlastností

// nastavení prodlevy
void DimenSetDelay(HWND wnd)
{
	CText txt;
	txt.Int(NewDelay);
	NewFill = TRUE;
	txt.SetDialogText(wnd, IDC_DELAY);
	NewFill = FALSE;
}

// načtení prodlevy
void DimenGetDelay(HWND wnd)
{
	if (!NewFill)
	{
		CText txt;
		txt.GetDialogText(wnd, IDC_DELAY);
		NewDelay = Int(txt);
		if (NewDelay < 55) NewDelay = 55;
		if (NewDelay > 4000000) NewDelay = 4000000;
		NewDelay = (NewDelay + 22) / 55 * 55;
		if (NewDelay > SPRITEMAXDELAY) NewDelay = SPRITEMAXDELAY;
	}
}

// nastavení fází na krok
void DimenSetStep(HWND wnd)
{
	CText txt;
	txt.Double(NewStep);
	NewFill = TRUE;
	txt.SetDialogText(wnd, IDC_STEP);
	NewFill = FALSE;
}

// načtení fází na krok
void DimenGetStep(HWND wnd)
{
	if (!NewFill)
	{
		CText txt;
		txt.GetDialogText(wnd, IDC_STEP);
		NewStep = Double(txt);
		if (NewStep < 0)
		{
			NewStep = 0;
		}
		if (NewStep > SPRITEMAXKROKU)
		{
			NewStep = SPRITEMAXKROKU;
		}
	}
}

// nastavení hladiny
void DimenSetLevel(HWND wnd)
{
	CText txt;
	txt.Int(NewLevel);
	NewFill = TRUE;
	txt.SetDialogText(wnd, IDC_LEVEL);
	NewFill = FALSE;
}

// načtení hladiny
void DimenGetLevel(HWND wnd)
{
	if (!NewFill)
	{
		CText txt;
		txt.GetDialogText(wnd, IDC_LEVEL);
		NewLevel = Int(txt);
	}
}

// nastavení klidových fází
void DimenSetKlid(HWND wnd)
{
	CText txt;
	txt.Int(NewKlid);
	NewFill = TRUE;
	txt.SetDialogText(wnd, IDC_KLID);
	NewFill = FALSE;
}

// načtení klidových fází
void DimenGetKlid(HWND wnd)
{
	if (!NewFill)
	{
		CText txt;
		txt.GetDialogText(wnd, IDC_KLID);
		NewKlid = Int(txt);
		if (NewKlid < 1) NewKlid = 1;
		if (NewKlid > SPRITEMAXKLID) NewKlid = SPRITEMAXKLID;
	}
}

// nastavení pohybových fází
void DimenSetPohyb(HWND wnd)
{
	CText txt;
	txt.Int(NewPohyb);
	NewFill = TRUE;
	txt.SetDialogText(wnd, IDC_POHYB);
	NewFill = FALSE;
}

// načtení pohybových fází
void DimenGetPohyb(HWND wnd)
{
	if (!NewFill)
	{
		CText txt;
		txt.GetDialogText(wnd, IDC_POHYB);
		NewPohyb = Int(txt);
		if (NewPohyb < 0) NewPohyb = 0;
		if (NewPohyb > SPRITEMAXPOHYB) NewPohyb = SPRITEMAXPOHYB;
	}
}

// nastavení směrů
void DimenSetSmer(HWND wnd)
{
	CText txt;
	txt.Int(NewSmer);
	NewFill = TRUE;
	txt.SetDialogText(wnd, IDC_SMER);
	NewFill = FALSE;
}

// načtení směrů
void DimenGetSmer(HWND wnd)
{
	if (!NewFill)
	{
		CText txt;
		txt.GetDialogText(wnd, IDC_SMER);
		NewSmer = Int(txt);
		if (NewSmer < 1) NewSmer = 1;
		if (NewSmer > SPRITEMAXSMER) NewSmer = SPRITEMAXSMER;
	}
}

// nastavení šířky
void DimenSetWidth(HWND wnd)
{
	CText txt;
	txt.Double((double)NewWidth/ICONWIDTH);
	NewFill = TRUE;
	txt.SetDialogText(wnd, IDC_WIDTH);
	NewFill = FALSE;
}

// načtení šířky
void DimenGetWidth(HWND wnd)
{
	if (!NewFill)
	{
		CText txt;
		txt.GetDialogText(wnd, IDC_WIDTH);
		NewWidth = (int)(Double(txt)*ICONWIDTH + 0.5);
		if (NewWidth <= 0) NewWidth = 1;
		if (NewWidth > 0x1000) NewWidth = 0x1000;
	}
}

// nastavení výšky
void DimenSetHeight(HWND wnd)
{
	CText txt;
	txt.Double((double)NewHeight/ICONHEIGHT);
	NewFill = TRUE;
	txt.SetDialogText(wnd, IDC_HEIGHT);
	NewFill = FALSE;
}

// načtení výšky
void DimenGetHeight(HWND wnd)
{
	if (!NewFill)
	{
		CText txt;
		txt.GetDialogText(wnd, IDC_HEIGHT);
		NewHeight = (int)(Double(txt)*ICONHEIGHT + 0.5);
		if (NewHeight <= 0) NewHeight = 1;
		if (NewHeight > 0x1000) NewHeight = 0x1000;
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

			txt.Load(IDN_SPRITEDIMEN);
			txt.SetWindowTextFont(hWnd);

			DialogCustomText(hWnd, IDN_PRODLEVA_MEZI_FAZEMI);
			DialogCustomText(hWnd, IDN_MILISEKUND);
			DialogCustomText(hWnd, IDN_FAZI_NA_KROK);
			DialogCustomText(hWnd, IDN_HLADINA_K_ZOBRAZENI);
			DialogCustomText(hWnd, IDN_FAZI_PRO_KLID);
			DialogCustomText(hWnd, IDN_FAZI_PRO_POHYB);
			DialogCustomText(hWnd, IDN_SMERU);
			DialogCustomText(hWnd, IDN_SIRKA_OBRAZKU);
			DialogCustomText(hWnd, IDN_VYSKA_OBRAZKU);
			DialogCustomText(hWnd, IDN_KROKU);
			DialogCustomText(hWnd, IDN_BODU);

			txt.Load(IDN_CANCEL0);
			txt.SetDialogTextFont(hWnd, IDCANCEL);

			DimenSetDelay(hWnd);
			DimenSetStep(hWnd);
			DimenSetLevel(hWnd);
			DimenSetKlid(hWnd);
			DimenSetPohyb(hWnd);
			DimenSetSmer(hWnd);
			DimenSetWidth(hWnd);
			DimenSetHeight(hWnd);

			DialogCustomBox(hWnd, IDN_ANIMACE);
			DialogCustomBox(hWnd, IDN_POCET_OBRAZKU);
			DialogCustomBox(hWnd, IDN_ROZMER_OBRAZKU);
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
				DimenGetDelay(hWnd);
				DimenGetStep(hWnd);
				DimenGetLevel(hWnd);
				DimenGetKlid(hWnd);
				DimenGetPohyb(hWnd);
				DimenGetSmer(hWnd);
				DimenGetWidth(hWnd);
				DimenGetHeight(hWnd);
				::EndDialog(hWnd, LOWORD(wParam));
				return TRUE;
			}
			break;

		case EN_KILLFOCUS:
			switch (LOWORD(wParam))
			{
			case IDC_DELAY:
				DimenGetDelay(hWnd);
				DimenSetDelay(hWnd);
				break;

			case IDC_STEP:
				DimenGetStep(hWnd);
				DimenSetStep(hWnd);
				break;

			case IDC_LEVEL:
				DimenGetLevel(hWnd);
				DimenSetLevel(hWnd);
				break;

			case IDC_KLID:
				DimenGetKlid(hWnd);
				DimenSetKlid(hWnd);
				break;

			case IDC_POHYB:
				DimenGetPohyb(hWnd);
				DimenSetPohyb(hWnd);
				break;

			case IDC_SMER:
				DimenGetSmer(hWnd);
				DimenSetSmer(hWnd);
				break;

			case IDC_WIDTH:
				DimenGetWidth(hWnd);
				DimenSetWidth(hWnd);
				break;

			case IDC_HEIGHT:
				DimenGetHeight(hWnd);
				DimenSetHeight(hWnd);
				break;
			}
		}
		break;
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// nastavení parametrů sprajtu

void OnDimen()
{
// příprava výchozích rozměrů
	NewDelay = Sprite[Index].Delay();
	NewStep = Sprite[Index].Kroku();
	NewLevel = Sprite[Index].Level();
	NewKlid = Sprite[Index].Klid();
	NewPohyb = Width - NewKlid;
	NewSmer = Height;
	NewWidth = PicWidth;
	NewHeight = PicHeight;

// zadání nových rozměrů sprajtu
	if (::DialogBoxParam(
			hInstance,
			MAKEINTRESOURCE(IDN_SPRITEDIMEN),
			MainFrame,
			(DLGPROC)DimenDialogProc,
			(LPARAM)IDN_SPRITEDIMEN) != IDOK) return;

// kontrola, zda byly parametry změněny
	if ((NewDelay == Sprite[Index].Delay()) && 
		(NewStep == Sprite[Index].Kroku()) &&
		(NewLevel == Sprite[Index].Level()) &&
		(NewKlid == Sprite[Index].Klid()) &&
		(NewPohyb == Width - NewKlid) &&
		(NewWidth == PicWidth) &&
		(NewHeight == PicHeight) &&
		(NewSmer == Height))
	{
		return;
	}

// zapnutí kurzoru čekání
	BeginWaitCursor();

// úschova pro UNDO
	PushUndo();

// nastavení nových parametrů sprajtu
	ASSERT(Sprite.IsValid(Index));
	if (((NewKlid + NewPohyb) < Width) || (NewSmer < Height))
	{
		HistEdit::Del(BufPicID, -2, Index, -1);
	}
	Sprite[Index].ReSize(NewKlid, NewPohyb, NewSmer);
	Sprite[Index].Delay(NewDelay);
	Sprite[Index].Kroku(NewStep);
	Sprite[Index].Level(NewLevel);

	Width = Sprite[Index].Faze();		// šířka plochy v políčkách
	Height = Sprite[Index].Smer();		// výška plochy v políčkách

	for (int i = Width*Height-1; i >= 0; i--)
	{
		Sprite[Index][i].CopyWrite();
		Sprite[Index][i].Resize(NewWidth, NewHeight);
	}

	PicWidth = NewWidth;				// šířka obrázku sprajtu
	KorWidth = PicWidth;				// zkorigovaná šířka obrázku sprajtu
	if (KorWidth < ICONWIDTH) KorWidth = ICONWIDTH;
	PicHeight = NewHeight;				// výška obrázku sprajtu
	KorHeight = PicHeight;				// zkorigovaná výška sprajtu
	if (KorHeight < ICONHEIGHT) KorHeight = ICONHEIGHT;

// překreslení displeje (a přepočet parametrů)
	Disp();

// příznak modifikace
	SetModi();

// vypnutí kurzoru čekání
	EndWaitCursor();
}



// ***************************** obsluha zobrazení **************************

/////////////////////////////////////////////////////////////////////////////
// přepočet rozměrů

void ReCalc()
{
// šířka a výška plochy v bodech (včetně záhlaví)
	MWidth = Width*KorWidth + ICONWIDTH;
	MHeight = Height*KorHeight + ICONHEIGHT;

// příprava zobrazené výšky (korekce, je-li horizontální posuvník)
	DHeight = EditHeight - 4;					// výška k zobrazení
	if (EditWidth - 4 - TRACKSIRKA < MWidth)
	{
		DHeight -= TRACKSIRKA;					// snížený výšky o horizontální posuvník
		if (DHeight < 1) DHeight = 1;			// minimální šířka
	}

// příprava zobrazené šířky (korekce, je-li vertikální posuvník)
	DWidth = EditWidth - 4;						//	šířka k zobrazení
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

// omezení počátečního bodu
	if (TLeft < 0) TLeft = 0;
	if (TLeft > MWidth - DWidth) TLeft = MWidth - DWidth;
	if (TTop < 0) TTop = 0;
	if (TTop > MHeight - DHeight) TTop = MHeight - DHeight;

// přepočet zobrazeného počátku na ikony a body
	if (TLeft < ICONWIDTH)
	{
		ILeft = 0;
		OLeft = ICONWIDTH - TLeft;				// zobrazená část ikony vlevo
	}
	else
	{
		ILeft = (TLeft-ICONWIDTH) / KorWidth + 1;	// index ikony vlevo
		OLeft = KorWidth - ((TLeft-ICONWIDTH) % KorWidth);	// zobrazená část ikony vlevo
	}

	if (TTop < ICONHEIGHT)
	{
		ITop = 0;
		OTop = ICONHEIGHT - TTop;
	}
	else
	{
		ITop = (TTop-ICONHEIGHT) / KorHeight + 1;	// index ikony nahoře
		OTop = KorHeight - ((TTop-ICONHEIGHT) % KorHeight);	// zobrazená část ikony nahoře
	}

// zobrazená část ikon vpravo a dole
	ORight = KorWidth - ((MWidth - TLeft - DWidth) % KorWidth);
	OBottom = KorHeight - ((MHeight - TTop - DHeight) % KorHeight);

// index ikon vpravo a dole
	IRight = ILeft + (DWidth - OLeft + KorWidth - 1) / KorWidth;
	IBottom = ITop + (DHeight - OTop + KorHeight - 1) / KorHeight;

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

// vykreslení obrázku
	DispAkt(dc);

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
	int width = ((IRight - ILeft + 1)*KorWidth + 7) & ~7;
	int height = (IBottom - ITop + 1)*KorHeight;
	BYTE* dst;
	BYTE* src;
	BYTE col;
	MapSwcNum[0].DeComp();
	BYTE numcol = (MapSwcNum[0].DataData())[0];		// barva podkladu číslic
	int cislo;										// číslo fáze v záhlaví
	int cislic;										// počet číslic čísla v záhlaví
	double uhel;									// směr úhlu
	double uhelsin;									// sinus úhlu
	double uhelcos;									// cosinus úhlu
	CPicture* data = Sprite[Index].DataData();

// vytvoření bufferu k zobrazení
	BYTE* buf = (BYTE*)MemGet(width*height);

// zobrazení ikon
	for (i = ILeft; i <= IRight; i++)
	{
		for (j = ITop; j <= IBottom; j++)
		{

// cílová adresa k uložení ikony
			dst = buf + (i-ILeft)*KorWidth + (IBottom-j)*KorHeight*width;
			if ((i > 0) && (ILeft == 0)) dst += ICONWIDTH - KorWidth;

// je to horní řádek (záhlaví s číslem fáze)?
			if (j == 0)
			{

// dekódování rohu záhlaví
				if (i == 0)
				{
					SpriteIcon[1].DeComp();
					src = SpriteIcon[1].DataData();

					for (k = ICONHEIGHT; k > 0; k--)
					{
						MemCopy(dst, src, ICONWIDTH);
						dst += width;
						src += ICONWIDTH;
					}
				}

// jinak to bude číslo fáze
				else
				{

// vymazání ikony pro číslo fáze
					col = OrangeCol;

					for (k = ICONHEIGHT; k > 0; k--)
					{
						MemFill(dst, KorWidth, col);
						dst += width;
					}
					dst -= width*(ICONHEIGHT/2 + 6);

// příprava barvy a čísla fáze (s rozlišením fáze pro klid)
					col = RedCol;
					cislo = i - 1;
					if (cislo >= Sprite[Index].Klid())
					{
						cislo -= Sprite[Index].Klid();
						col = BlackCol;
					}

// příprava počtu číslic a ukládaci adresy
					cislic = 1;
					if (cislo >= 10) cislic++;
					if (cislo >= 100) cislic++;
					if (cislo >= 1000) cislic++;
					if (cislo >= 10000) cislo = 9999;
					dst += KorWidth/2 - 8 + cislic*4;

// dekódování znaků číslic
					for (; cislic > 0; cislic--)
					{
						MapSwcNum[cislo % 10].DeComp();
						src = MapSwcNum[cislo % 10].DataData();

						for (k = 12; k > 0; k--)
						{
							for (l = 8; l > 0; l--)
							{
								if (*src != numcol)
								{
									*dst = col;
								}
								src++;
								dst++;
							}
							dst += width - 8;
						}
						cislo /= 10;
						dst -= 12*width + 8;
					}
				}
			}

// není horní řádek
			else
			{

// je to záhlaví vlevo (směr)?
				if (i == 0)
				{

// vymazání ikony pro směr
					col = OrangeCol;

					for (k = KorHeight; k > 0; k--)
					{
						MemFill(dst, ICONWIDTH, col);
						dst += width;
					}
					dst -= width*((KorHeight+ICONHEIGHT)/2);

// dekódování podkladu záhlaví pro směr
					SpriteIcon[0].DeComp();
					src = SpriteIcon[0].DataData();

					for (k = ICONHEIGHT; k > 0; k--)
					{
						MemCopy(dst, src, ICONWIDTH);
						dst += width;
						src += ICONWIDTH;
					}
					dst -= width*(ICONHEIGHT/2) - ICONWIDTH/2;

// příprava úhlu směru
					uhel = ((double)pi2 / Height) * (j - 1);
					uhelsin = sin(uhel);
					uhelcos = cos(uhel);

// dekódování směru
					col = BlueCol;
					for (k = ICONWIDTH/2 - 3; k >= 0; k--)
					{
						l = (int)(uhelcos*k) + width*(int)(uhelsin*k);

						dst[l] = col;
						dst[l - 1] = col;
						dst[l - width] = col;
						dst[l - 1 - width] = col;
					}
				}

// jinak jsou platná data
				else
				{

// vymazání ikony pro obrázek
					col = BackCol;

					for (k = KorHeight; k > 0; k--)
					{
						MemFill(dst, KorWidth, col);
						dst += width;
					}
					dst -= width*((KorHeight+PicHeight)/2) - (KorWidth-PicWidth)/2;

// dekódování platné ikony
					data[(i-1) + (j-1)*Width].DeComp();
					src = data[(i-1) + (j-1)*Width].DataData();
	
					for (k = PicHeight; k > 0; k--)
					{
						MemCopy(dst, src, PicWidth);
						dst += width;
						src += PicWidth;
					}
				}
			}
		}
	}

// zobrazení horizontálních čar rastru
	col = BlackCol;
	dst = buf;
	for (i = IBottom - ITop; i >= 0; i--)
	{
		MemFill(dst, width, col);
		dst += KorHeight*width;
	}

	if (ITop == 0)
	{
		dst += (ICONHEIGHT-KorHeight-1)*width;
		MemFill(dst, width, col);
	}

// zobrazení vertikálních čar rastru
	for (i = IRight - ILeft + 1; i > 0; i--)
	{
		dst = buf + KorWidth*i - 1;
		if (ILeft == 0) dst += ICONWIDTH-KorWidth;

		if (i + ILeft - 1 == Sprite[Index].Klid())
		{
			col = RedCol;
		}

		for (j = height; j > 0; j--)
		{
			*dst = col;
			dst += width;
		}

		col = BlackCol;
	}

	if (ILeft == 0)
	{
		dst = buf;

		for (j = height; j > 0; j--)
		{
			*dst = col;
			dst += width;
		}
	}

// přednastavení parametrů záhlaví bitmapy
	StdBitmapInfo->bmiHeader.biWidth = width;
	StdBitmapInfo->bmiHeader.biHeight = DHeight;

// zobrazení
	::StretchDIBits(dc, DLeft, DTop, DWidth, DHeight, 
		(ILeft == 0) ? (ICONWIDTH - OLeft) : (KorWidth - OLeft),
			0, DWidth, DHeight, buf + (KorHeight-OBottom)*width,
			StdBitmapInfo, DIB_RGB_COLORS, SRCCOPY);

// zrušení bufferu
	MemFree(buf);
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
	if (si.nPos != TTop)
	{
		si.nPos = TTop;
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

	if (pos != TTop)
	{
		TTop = pos;
		Disp();
	}
}


// **************************** obsluha editace *****************************


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
// stisk tlačítka myši (TRUE = obslouženo)

BOOL OnButtonDown(UINT flags, int x, int y, BOOL right, BOOL dclick)
{
// ukončení tažení
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
	BegDrag = TRUE;
	CopyMode = FALSE;
	DragX = MouseX;
	DragY = MouseY;

// zahájení editace při dvojkliku
	if (dclick)
	{
		IndexPic = MouseX + MouseY * Width;
		Sprite[Index][IndexPic].CopyWrite();
		if (SetEditMode(BufPicID, -2))
		{
			ProgSetMenu();
			SetToolMode(BufPicID);
			SetFocusEdit();
			EditIcon::UpdateMenu();
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
		x -= HotSpotX;	x += KorWidth/2;
		y -= HotSpotY;	y += KorHeight/2;
	}

	if (Draging)
	{
		x -= DragHotSpotX;	x += DragIconWidth/2;
		y -= DragHotSpotY;	y += DragIconHeight/2;
	}

// příprava relativních souřadnic bodu v editačním poli
	int	sx = (x - DLeft - OLeft + KorWidth)/KorWidth + ILeft - 1;
	int sy = (y - DTop - OTop + KorHeight)/KorHeight + ITop - 1;

// souřadnice myši pro zobrazení
	if ((x >= DLeft) && (y >= DTop) && 
		((DWORD)sx < (DWORD)Width) && ((DWORD)sy < (DWORD)Height) &&
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

// zahájení tažení
	if ((flags & (MK_LBUTTON | MK_RBUTTON)) &&
		!MDraging &&
		BegDrag &&
		(DragX != MOUSEINV) &&
		(DragY != MOUSEINV) &&
		(abs(x - BegX) + abs(y - BegY) >= 5))
	{
		MDraging = TRUE;

// bod styku pro tažení
		HotSpotX = BegX - DLeft - OLeft - (DragX-ILeft)*KorWidth;
		HotSpotY = BegY - DTop - OTop - (DragY-ITop)*KorHeight;

// vygenerování tažené ikony
		DragImageList = (Sprite[Index].DataData())[DragX + DragY*Width].GenerDrag(EmptyText);

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
	if (MDraging)
	{
		if ((DragX != MouseX) || (DragY != MouseY))
		{
			PushUndo();

			CPicture* data = Sprite[Index].DataData();
			CPicture* olddata = data + (DragX + DragY*Width);
//			CPicture olditem(PicWidth, PicHeight);
			CPicture olditem;
			olditem.New(PicWidth, PicHeight);
			olditem.Clear();
	
			if ((MouseX != MOUSEINV) && (MouseY != MOUSEINV))
			{
				CPicture* newdata = data + (MouseX + MouseY*Width);

				olditem = *newdata;

				*newdata = *olddata;
			}

			if (!CopyMode)
			{
				*olddata = olditem;
			}

			SetModi();
		}

		EndDrag();
		Disp();
	}

	BegDrag = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// položení obrázku zvnějšku

void DragDrop(int pic)
{
// ukončení editace číselného parametru
	if ((EditMode != BufSprID) ||
		Sprite.IsNotValid(Index) ||
		Picture.IsNotValid(pic) ||
		((DWORD)MouseX >= (DWORD)Width) ||
		((DWORD)MouseY >= (DWORD)Height))
	{
		return;
	}

// úschova pro UNDO
	PushUndo();

	(Sprite[Index].DataData())[MouseX + MouseY*Width] = Picture[pic];
	(Sprite[Index].DataData())[MouseX + MouseY*Width].Resize(PicWidth, PicHeight);

	SetModi();
	EndDrag();
	Disp();
}


/////////////////////////////////////////////////////////////////////////////
// test

void TestPaint(HWND hWnd)
{
	int i, j;

// vymazání bufferu ikonami pozadí
	BYTE* src;
	ASSERT(IconFnc.IsValid(0));
	if (Icon.IsValid(0))
	{
		Icon[0].DeComp();
		src = Icon[0].DataData();
	}
	else
	{
		IconFnc[0].DeComp();
		src = IconFnc[0].DataData();
	}
	BYTE* dst = TestBuff;

	for (i = ICONHEIGHT; i > 0; i--)
	{
		MemCopy(dst, src, ICONWIDTH);
		src += ICONWIDTH;
		MemCopy(dst + ICONWIDTH, dst, TestWidth - ICONWIDTH);
		dst += TestWidth;
	}

	MemCopy(dst, TestBuff, TestWidth * (TestHeight - ICONHEIGHT));

// příprava fáze sprajtu
	int faze = TestFaze;

	if (((TestX != TestEndX) || (TestY != TestEndY)) && 
		(TestFazi > TestKlid))
	{
		faze += TestKlid;
		if (faze >= TestFazi)
		{
			faze = TestKlid;
			TestFaze = 0;
		}
	}
	else
	{
		if (faze >= TestKlid)
		{
			faze = 0;
			TestFaze = 0;
		}
	}

// souřadnice a rozměry k zobrazení sprajtu
	int x = Round(TestX);
	int y = Round(TestY);
	int width = TestPicWidth;
	int height = TestPicHeight;

// zdrojová adresa sprajtu
	Sprite[Index].At(faze, TestSmer).DeComp();
	src = Sprite[Index].At(faze, TestSmer).DataData();

// omezení souřadnic a rozměrů
	if (x < 0)
	{
		width += x;
		src -= x;
		x = 0;
	}

	if (y < 0)
	{
		height += y;
		src -= y*TestPicWidth;
		y = 0;
	}

	if (x + width > TestWidth) width = TestWidth - x;

	if (y + height > TestHeight) height = TestHeight - y;

// cílová adresa k dekódování sprajtu
	dst = TestBuff + x + y*TestWidth;

// dekódování sprajtu
	if ((width > 0) && (height > 0))
	{
		for (i = height; i > 0; i--)
		{
			for (j = width; j > 0; j--)
			{
				if (*src != BackCol)
				{
					if (*src == ShadCol)
					{
						int c = *dst + 1;
						if ((c < ResCols) || (((c - ResCols) % ColLev) == 0))
						{
							c = BlackCol;
						}
						*dst = (BYTE)c;
					}
					else
					{
						*dst = *src;
					}
				}
				dst++;
				src++;
			}
			dst += TestWidth - width;
			src += TestPicWidth - width;
		}
	}

// otevření DC
	HDC dc = ::GetDC(hWnd);

// nastavení vlastních palet
	HPALETTE OldPal;					// úschova původních palet
	OldPal = ::SelectPalette(dc,		// výběr vlastních palet
		StdPalette, FALSE);
	::RealizePalette(dc);				// realizace palet

// přednastavení parametrů záhlaví bitmapy
	StdBitmapInfo->bmiHeader.biWidth = TestWidth;
	StdBitmapInfo->bmiHeader.biHeight = TestHeight;

// zobrazení obrázku
	::StretchDIBits(dc, TestLeft, TestTop, TestWidth, TestHeight, 0,
			0, TestWidth, TestHeight, TestBuff,
			StdBitmapInfo, DIB_RGB_COLORS, SRCCOPY);

// návrat původních palet
	::SelectPalette(dc,OldPal,FALSE);

// uvolnění DC
	::ReleaseDC(hWnd, dc);

// posun souřadnic
	TestNext--;
	if (TestNext <= 0)
	{
		TestFaze++;
		TestNext = TestDelay;

		if ((TestEndX != TestX) || (TestEndY != TestY))
		{
			double dx = TestEndX - TestX;
			double dy = TestEndY - TestY;
			double uhel = atan2(dy, dx) + pi2;
			double delka = sqrt(dx*dx +	dy*dy);
			int kroku = Round(delka * TestKroku / ICONWIDTH);
			if (kroku < 1) kroku = 1;
			double delka1 = delka / kroku;
			if (delka < delka1 + 0.01)
			{
				TestX = TestEndX;
				TestY = TestEndY;
			}
			else
			{
				TestX += cos(uhel) * delka1;
				TestY += sin(uhel) * delka1;
			}
		}
	}
}


// callback funkce dialogu
BOOL CALLBACK SpriteDialogProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_INITDIALOG:
		{
			::MoveWindow(hWnd, 0, 0, TestWidth + 11, TestHeight + 70, FALSE);
			CenterWindow(hWnd, MainFrame);

			CText txt;

			txt.Load(IDN_SPRITETEST);
			txt.SetWindowTextFont(hWnd);

			txt.Load(IDN_CANCEL0);
			txt.SetDialogTextFont(hWnd, IDCANCEL);

			RECT rcClient;
			::GetClientRect(hWnd, &rcClient);
			TestLeft = (rcClient.right - TestWidth)/2;
			TestTop = 3;

			HWND item = ::GetDlgItem(hWnd, IDCANCEL);
			if (item)
			{
				RECT rcItem;
				::GetWindowRect(item, &rcItem);

				int itemWidth = rcItem.right - rcItem.left;
				int itemHeight = rcItem.bottom - rcItem.top;

				::MoveWindow(item, (rcClient.right - itemWidth)/2,
					rcClient.bottom - itemHeight - 8,
					itemWidth, itemHeight, TRUE);
			}

			TestTimer = ::SetTimer(hWnd, TestTimerID, TimerConst, NULL);
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
			case IDCANCEL:
				::KillTimer(hWnd, TestTimer);
				::EndDialog(hWnd, LOWORD(wParam));
				return TRUE;
			}
			break;
		}
		break;

	case WM_RBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
		{
			TestEndX = (int)LOWORD(lParam) - TestLeft - TestPicWidth/2;
			TestEndY = TestHeight - (int)HIWORD(lParam) + TestTop - TestPicHeight/2;

			if ((TestEndX != TestX) || (TestEndY != TestY))
			{
				double uhel = atan2(TestEndY - TestY, TestEndX - TestX) + pi2;
				TestSmer = Round(uhel * TestSmeru / pi2);
				while (TestSmer >= TestSmeru) TestSmer -= TestSmeru;
				while (TestSmer < 0) TestSmer += TestSmeru;
				TestNext = 1;
			}
		}
		break;

	case WM_TIMER:
		TestPaint(hWnd);
		break;
	}
	return FALSE;
}

void OnTest()
{
// kontrola, zda je režim editace
	if (!ProgMode ||
		(EditMode != BufSprID) ||
		Sprite.IsNotValid(Index) ||
		TestMode)
	{
		return;
	}

// vytvoření bufferu
	TestBuff = (BYTE*) MemGet(TestWidth * TestHeight);

// zahájení testovacího módu
	TestPicWidth = Sprite[Index].Width();
	TestPicHeight = Sprite[Index].Height();
	TestX = (TestWidth - TestPicWidth)/2;
	TestEndX = TestX;
	TestY = (TestHeight - TestPicHeight)/2;
	TestEndY = TestY;
	TestFaze = 0;
	TestSmer = 0;
	TestFazi = Sprite[Index].Faze();
	TestKlid = Sprite[Index].Klid();
	TestSmeru = Sprite[Index].Smer();
	TestDelay = (Sprite[Index].Delay() + 22)/55;
	if (TestDelay < 1) TestDelay = 1;
	TestKroku = Sprite[Index].Kroku();

	TestMode = TRUE;

// test sprajtu
	::DialogBoxParam(
			hInstance,
			MAKEINTRESOURCE(IDN_SPRITETEST),
			MainFrame,
			(DLGPROC)SpriteDialogProc,
			(LPARAM)IDN_SPRITETEST);

// ukončení testovacího módu
	TestMode = FALSE;

// zrušení bufferu
	MemFree(TestBuff);
	TestBuff = NULL;
}

// ****************************** obsluha UNDO ******************************


/////////////////////////////////////////////////////////////////////////////
// úschova plochu pro UNDO

void PushUndo()
{
	Undo.AddSprSet(Index, Sprite[Index]);
	Sprite[Index].CopyWrite();
	Undo.Stop();
}


}
