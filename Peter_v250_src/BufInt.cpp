
#include "Main.h"

/***************************************************************************\
*																			*
*								Buffer celých čísel							*
*																			*
\***************************************************************************/

////////////////////////////////////////////////////////////////////
// přidání záznamu o vložení položky do UNDO bufferu

template <> bool CBuffer<CInt>::UndoAddIns(int index)
{
	return Undo.AddBoolIns(index);
}


/////////////////////////////////////////////////////////////////////////////
// načtení logické hodnoty ze souboru (načítá do proměnné, vrací TRUE=operace OK)

bool CBufBool::LoadFile(BOOL& data, CText jmeno)
{
// načtení textového souboru
	CText txt;
	if (!txt.LoadFile(jmeno)) return false;

// odstranění mezer a jiných oddělovačů
	if (!txt.TrimLeft() || !txt.TrimRight()) return false;

// konverze na velká písmena
	if (!txt.UpperCase()) return false;

// prázdný text je jako FALSE
	if (txt.IsEmpty())
	{
		data = FALSE;
		return true;
	}

// nastavení dat podle prvního písmene
	TCHAR znak = txt[0];
	data = ((znak == 'T') ||
			(znak == 'Y') ||
			(znak == 'J') ||
			(znak == 'A') ||
			(znak ==  1 ) ||
			(znak == '1'));

// příznak operace OK
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// uložení logické hodnoty do souboru formátu LOG (vrací TRUE=operace OK)

bool CBufBool::SaveFile(BOOL data, CText jmeno) const
{
// příprava textu
	CText txt;
	if (data)
	{
		txt = _T("TRUE");
	}
	else
	{
		txt = _T("FALSE");
	}

// uložení textu do souboru
	return txt.SaveFile(jmeno);
}
