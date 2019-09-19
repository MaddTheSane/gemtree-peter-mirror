
#include "Main.h"

/***************************************************************************\
*																			*
*								Editor čísel								*
*																			*
\***************************************************************************/

namespace EditNum
{

/////////////////////////////////////////////////////////////////////////////
// parametry

int		Index = -1;						// editovaná položka
int		DispLeft;						// souřadnice X editačního pole
int		DispTop;						// souřadnice Y editačního pole
int		DispWidth;						// šířka editačního pole
int		DispHeight;						// výška editačního pole

/////////////////////////////////////////////////////////////////////////////
// inicializace při startu programu

void StartInit()
{
// vytvoření editačního okna
	EditNumWnd = ::CreateWindowEx(
		WS_EX_CLIENTEDGE,				// rozšířený styl
		_T("EDIT"),						// třída
		NULL,							// text
		WS_CHILD | ES_AUTOHSCROLL,		// styl
		300,							// X
		200,							// Y
		100,							// šířka
		30,								// výška
		MainFrame,						// rodič
		NULL,							// ID
		hInstance,						// instance
		NULL);							// data
}


/////////////////////////////////////////////////////////////////////////////
// zahájení editace (musí být platný index!)

void BegEdit(int index)
{
// index prvku
	Index = index;
	ASSERT(Real.IsValid(index));

// zobrazení okna
	::ShowWindow(EditNumWnd, SW_SHOW);

// příprava textu
	ReLoad();

// zobrazení
	Disp();

// aktualizace voleb bloku
	UpdateClipboard();
}


/////////////////////////////////////////////////////////////////////////////
// přesun editačního pole

HDWP MoveEdit(HDWP hdwp)
{
	DispWidth = 200;
	if (DispWidth > EditWidth-4) DispWidth = EditWidth-4;
	if (DispWidth < 1) DispWidth = 1;

	DispHeight = 25;
	if (DispHeight > EditHeight-4) DispHeight = EditHeight-4;
	if (DispHeight < 1) DispHeight = 1;

	DispLeft = EditX + (EditWidth - DispWidth)/2;
	DispTop = EditY + (EditHeight - DispHeight)/2;

	hdwp = ::DeferWindowPos(hdwp, EditNumWnd, NULL, 
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
// vstup znaku z klávesnice (TRUE=zrušit)

BOOL OnChar(TCHAR znak)
{
	switch (znak)
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '.':
	case ',':
	case 'E':
	case 'e':
	case '-':
	case '+':
		return FALSE;
	case 13:
	case 27:
		return TRUE;
	}

	if (znak < 32) return FALSE;

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// výběr všeho

void SelectAll()
{
// výběr všeho
	::SendMessage(EditNumWnd, EM_SETSEL, 0, (LPARAM)-1);

// aktualizace voleb bloku
	UpdateClipboard();
}

/////////////////////////////////////////////////////////////////////////////
// zrušení bloku nebo znaku

void Delete()
{
// zjištění označeného úseku textu
	long int start, end;
	::SendMessage(EditNumWnd, EM_GETSEL, (WPARAM)&start, (WPARAM)&end);

// není-li nic označeno, ruší se znak za kurzorem (podhození klávesy DELETE)
	if (start == end)
	{
		::SendMessage(EditNumWnd, WM_KEYDOWN,	VK_DELETE, 0x01530001);
		::SendMessage(EditNumWnd, WM_KEYUP,		VK_DELETE, 0xC1530001);
	}
	else

// jinak zrušení označeného úseku
	{
		::SendMessage(EditNumWnd, WM_CLEAR, 0, 0);
	}

// aktualizace voleb bloku
	UpdateClipboard();
}

/////////////////////////////////////////////////////////////////////////////
// kopie bloku

void Copy()
{
// operace COPY
	::SendMessage(EditNumWnd, WM_COPY, 0, 0);

// aktualizace voleb bloku
	UpdateClipboard();
}

/////////////////////////////////////////////////////////////////////////////
// vystřížení bloku

void Cut()
{
// operaceCUT
	::SendMessage(EditNumWnd, WM_CUT, 0, 0);

// aktualizace voleb bloku
	UpdateClipboard();
}

/////////////////////////////////////////////////////////////////////////////
// navrácení bloku

void Paste()
{
// operace PASTE
	::SendMessage(EditNumWnd, WM_PASTE, 0, 0);

// aktualizace voleb bloku
	UpdateClipboard();
}

/////////////////////////////////////////////////////////////////////////////
// aktualizace voleb bloku

void UpdateClipboard()
{
// kontrola, zda je režim editace
	if (!ProgMode) return;

	if ((FocusBuf == BufEdiID) && (EditMode == BufNumID))
	{
		long int start, end;
		::SendMessage(EditNumWnd, EM_GETSEL, (WPARAM)&start, (WPARAM)&end);
		BOOL enable = (end != start);
		int len = ::GetWindowTextLength(EditNumWnd);

		EnableCommand(IDN_CUT, enable);
		EnableCommand(IDN_COPY, enable);
		EnableCommand(IDN_PASTE, ::IsClipboardFormatAvailable(CF_TEXT));
		EnableCommand(IDN_CLEAR, enable || (start < len));
		EnableCommand(IDN_ALL, len > 0);

		SetStatusText2(EmptyText);
	}
}


/////////////////////////////////////////////////////////////////////////////
// změna textu

void OnChange()
{
	if ((FocusBuf == BufEdiID) && (EditMode == BufNumID) && Real.IsValid(Index))
	{
		CText txt;
		txt.GetWindowText(EditNumWnd);
		double num = Double(txt);
		txt.Double(num);
		num = Double(txt);				// opakování pro případ zadání vyšší přesnosti
		if (Real[Index] != num)
		{
			Undo.AddRealSet(Index, Real[Index]);

			Real[Index] = num;
			SetModi();

			Undo.Stop();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// znovunačtení čísla

void ReLoad()
{
	CText txt;
	txt.Double(Real[Index]);
	txt.SetWindowText(EditNumWnd);
	SelectAll();
	Undo.Fixup();	
}

}
