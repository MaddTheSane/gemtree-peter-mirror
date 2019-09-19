
/***************************************************************************\
*																			*
*							Kompilace programu								*
*																			*
\***************************************************************************/

// korekce lokálních proměnných (při volání ve vstupních parametrech)

extern int KorigNum;
extern int KorigLog;
extern int KorigIco;
extern int KorigMap;
extern int KorigMus;
extern int KorigPic;
extern int KorigSnd;
extern int KorigSpr;
extern int KorigTxt;
extern int KorigLst;


typedef bool (_fastcall *COMPCOMP) (int index);

/////////////////////////////////////////////////////////////////////////////
// překlad příkazu se skupinou (data = počet parametrů)

bool CompGroup(COMPCOMP comp, int index, PROCCOM func, PROCCOM func1);


/////////////////////////////////////////////////////////////////////////////
// překlad volání funkce (typ = ID proměnné výsledku, IDF_FNC = není)

bool CompFunc(int index, int typ);


/////////////////////////////////////////////////////////////////////////////
// kompilace programu

void Comp();
