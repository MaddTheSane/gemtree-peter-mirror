
#include "Main.h"

/***************************************************************************\
*																			*
*							Buffer reálných čísel							*
*																			*
\***************************************************************************/


////////////////////////////////////////////////////////////////////
// přidání záznamu o vložení položky do UNDO bufferu

template <> bool CBuffer<CReal>::UndoAddIns(int index)
{
	return Undo.AddRealIns(index);
}

/////////////////////////////////////////////////////////////////////////////
// načtení čísla ze souboru (načítá do proměnné, TRUE=operace OK)

bool CBufReal::LoadFile(double& data, CText jmeno)
{
// načtení textového souboru
	CText txt;
	if (!txt.LoadFile(jmeno)) return false;

// odstranění mezer a jiných oddělovačů
	if (!txt.TrimLeft() || !txt.TrimRight()) return false;

// načtení čísla
	data = Double(txt);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// uložení čísla do souboru formátu NUM (FALSE=chyba)

bool CBufReal::SaveFile(double data, CText jmeno) const
{
// příprava čísla
	CText txt;
	if (!txt.Double(data)) return false;

// uložení textu do souboru
	return txt.SaveFile(jmeno);
}
