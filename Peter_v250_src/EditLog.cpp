
#include "Main.h"

/***************************************************************************\
*																			*
*								Editor logických hodnot						*
*																			*
\***************************************************************************/

namespace EditLog
{

/////////////////////////////////////////////////////////////////////////////
// parametry

int		Index = 0;						// editovaná položka
int		DispLeft;						// souřadnice X editačního pole
int		DispTop;						// souřadnice Y editačního pole
int		DispWidth;						// šířka editačního pole
int		DispHeight;						// výška editačního pole

/////////////////////////////////////////////////////////////////////////////
// inicializace při startu programu

void StartInit()
{
	CText txt;
	txt.Load(IDN_ZAPNUTO);

// vytvoření okna přepínače
	EditLogWnd = ::CreateWindowEx(
		0,								// rozšířený styl
		_T("BUTTON"),					// třída
		txt,							// text
		WS_CHILD | BS_CHECKBOX,			// styl
		300,							// X
		200,							// Y
		150,							// šířka
		30,								// výška
		MainFrame,						// rodič
		(HMENU)EDITLOGID,				// ID
		hInstance,						// instance
		NULL);							// data
}


/////////////////////////////////////////////////////////////////////////////
// zahájení editace (musí být platný index!)

void BegEdit(int index)
{
// index prvku
	Index = index;
	ASSERT(Bool.IsValid(index));

// zobrazení okna
	::ShowWindow(EditLogWnd, SW_SHOW);

// nastavení stavu tlačítka
	::SendMessage(EditLogWnd, BM_SETCHECK, Bool[index] ? BST_CHECKED : BST_UNCHECKED, 0);

// zobrazení
	Disp();
}


/////////////////////////////////////////////////////////////////////////////
// přesun editačního pole

HDWP MoveEdit(HDWP hdwp)
{
	DispWidth = 150;
	if (DispWidth > EditWidth-4) DispWidth = EditWidth-4;
	if (DispWidth < 1) DispWidth = 1;

	DispHeight = 18 + 8;
	if (DispHeight > EditHeight-4) DispHeight = EditHeight-4;
	if (DispHeight < 1) DispHeight = 1;

	DispLeft = EditX + (EditWidth - DispWidth)/2;
	DispTop = EditY + (EditHeight - DispHeight)/2;

	hdwp = ::DeferWindowPos(hdwp, EditLogWnd, NULL, 
			DispLeft, DispTop, DispWidth, DispHeight,
			SWP_DRAWFRAME | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	return hdwp;
}


/////////////////////////////////////////////////////////////////////////////
// zobrazení podkladu kolem editoru

void Disp()
{
// otevření kontextu displeje
	HDC dc = ::GetDC(MainFrame);

// příprava štětce k vymazání podkladu
	HBRUSH brush = (HBRUSH)::GetStockObject(LTGRAY_BRUSH); // štětec k vymazání plochy

// vymazání plochy nahoře nad editorem
	RECT rc;
	rc.left = EditX + 2;
	rc.right = EditX + EditWidth - 2;
	rc.top = EditY + 2;
	rc.bottom = DispTop;
	if (rc.top < rc.bottom)
	{
		::FillRect(dc, &rc, brush);
	}

// vymazání plochy dole pod editorem (left a right je nastaveno)
	rc.top = DispTop + DispHeight;
	rc.bottom = EditY + EditHeight - 2;
	if (rc.top < rc.bottom)
	{
		::FillRect(dc, &rc, brush);
	}

// vymazání plochy vlevo od editoru (left je nastaveno)
	rc.right = DispLeft;
	rc.top = DispTop;
	rc.bottom = DispTop + DispHeight;
	if (rc.left < rc.right)
	{
		::FillRect(dc, &rc, brush);
	}

// vymazání plochy vpravo od editoru (top a bottom je nastaveno)
	rc.left = DispLeft + DispWidth;
	rc.right = EditX + EditWidth - 2;
	if (rc.left < rc.right)
	{
		::FillRect(dc, &rc, brush);
	}

// zrušení štětce podkladu (i když podle dokumentace rušení není nutné)
	::DeleteObject(brush);

// uvolnění kontextu displeje
	::ReleaseDC(MainFrame, dc);
}

/////////////////////////////////////////////////////////////////////////////
// přepnutí přepínače

void Switch()
{
	if (ProgMode && (FocusBuf == BufEdiID) && (EditMode == BufLogID))
	{
		Undo.AddBoolSet(Index, Bool[Index]);
		Bool[Index] = !Bool[Index];
		SetModi();
		::SendMessage(EditLogWnd, BM_SETCHECK, Bool[Index] ? BST_CHECKED : BST_UNCHECKED, 0);
		Undo.Stop();
	}
}


}
