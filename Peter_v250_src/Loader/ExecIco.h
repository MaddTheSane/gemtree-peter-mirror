
/***************************************************************************\
*																			*
*							Provádění programu - ikona						*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// inline volání příkazu - ikona

inline void FIcon(CIcon& icon)
{
	ExecItem++;
	ExecItem[-1].ExecIco(icon);
}


void _fastcall FIconGet(CIcon& icon);		// načtení ikony z plochy

// funkce
void _fastcall FIconFunc(CIcon& icon);		// funkce s návratem ikony

// proměnné
void _fastcall FIconEmpty(CIcon& icon);		// prázdná ikona
void _fastcall FIconObj(CIcon& icon);		// globální ikona (Data = index)
void _fastcall FIconObjList(CIcon& icon);	// globalní ikona v seznamu (Data = index, List = seznam)
void _fastcall FIconLoc(CIcon& icon);		// lokální ikona (Data = index)
void _fastcall FIconLocList(CIcon& icon);	// lokální ikona v seznamu (Data = index, List = seznam)

// obsluha Petra a Petry
void _fastcall FPredmet(CIcon& icon);		// načtení předmětu před Petrem
void _fastcall FPredmet2(CIcon& icon);		// načtení předmětu před Petrou
void _fastcall FPredmetPoz(CIcon& icon);	// načtení předmětu pod Petrem
void _fastcall FPredmet2Poz(CIcon& icon);	// načtení předmětu pod Petrou

void _fastcall FGetMapIcon(CIcon& icon);	// načtení předmětu z plochy

void _fastcall FGetWindowIcon(CIcon& icon);	// načtení ikony okna

void _fastcall FGetFileIcon(CIcon& icon);	// načtení ikony ze souboru
