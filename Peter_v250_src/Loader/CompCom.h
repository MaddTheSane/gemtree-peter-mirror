
/***************************************************************************\
*																			*
*						Kompilace programu - příkaz							*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// přidání jednoho prvku do bufferu programu (vrací index prvku)

int CompAddItem(PROCCOM func);
int CompAddItem(PROCCOM func, int data);
int CompAddItem(PROCCOM func, int data, int list);


/////////////////////////////////////////////////////////////////////////////
// překlad příkazu (vrací true = operace OK)

bool _fastcall CompCom(int index);


/////////////////////////////////////////////////////////////////////////////
// překlad příkazu se skupinou příkazů (data = počet příkazů - 1)

inline bool CompComGrp(int index, PROCCOM func, PROCCOM func1)
	{ return CompGroup(CompCom, index, (PROCCOM)func, (PROCCOM)func1); };


/////////////////////////////////////////////////////////////////////////////
// překlad příkazu s podparametrem příkazů (hledaným podle identifikace), Jump = přeskočení skupiny

void CompComSubPar(int index, int idf);
