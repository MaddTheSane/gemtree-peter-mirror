
/***************************************************************************\
*																			*
*						Kompilace programu - plocha							*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// přidání jednoho prvku do bufferu programu (vrací index prvku)

inline int CompAddItem(PROCMAP func)
	{ return CompAddItem((PROCCOM)func); }

inline int CompAddItem(PROCMAP func, int data)
	{ return CompAddItem((PROCCOM)func, data); }

inline int CompAddItem(PROCMAP func, int data, int list)
	{ return CompAddItem((PROCCOM)func, data, list); }


/////////////////////////////////////////////////////////////////////////////
// překlad výrazu s plochou (vrací true = operace OK)

bool _fastcall CompMap(int index);


/////////////////////////////////////////////////////////////////////////////
// překlad příkazu s parametrem plochy

bool CompMapPar(int index, PROCCOM func, int data);

inline bool CompMapPar(int index, PROCNUM func, int data)
	{ return CompMapPar(index, (PROCCOM)func, data); }

inline bool CompMapPar(int index, PROCTXT func, int data)
	{ return CompMapPar(index, (PROCCOM)func, data); }

inline bool CompMapPar(int index, PROCLOG func, int data)
	{ return CompMapPar(index, (PROCCOM)func, data); }

inline bool CompMapPar(int index, PROCICO func, int data)
	{ return CompMapPar(index, (PROCCOM)func, data); }

inline bool CompMapPar(int index, PROCMAP func, int data)
	{ return CompMapPar(index, (PROCCOM)func, data); }

inline bool CompMapPar(int index, PROCPIC func, int data)
	{ return CompMapPar(index, (PROCCOM)func, data); }

inline bool CompMapPar(int index, PROCSPR func, int data)
	{ return CompMapPar(index, (PROCCOM)func, data); }

inline bool CompMapPar(int index, PROCSND func, int data)
	{ return CompMapPar(index, (PROCCOM)func, data); }

inline bool CompMapPar(int index, PROCMUS func, int data)
	{ return CompMapPar(index, (PROCCOM)func, data); }


bool CompMapPar(int index, PROCCOM func, int data, int list);

inline bool CompMapPar(int index, PROCNUM func, int data, int list)
	{ return CompMapPar(index, (PROCCOM)func, data, list); }

inline bool CompMapPar(int index, PROCTXT func, int data, int list)
	{ return CompMapPar(index, (PROCCOM)func, data, list); }

inline bool CompMapPar(int index, PROCLOG func, int data, int list)
	{ return CompMapPar(index, (PROCCOM)func, data, list); }

inline bool CompMapPar(int index, PROCICO func, int data, int list)
	{ return CompMapPar(index, (PROCCOM)func, data, list); }

inline bool CompMapPar(int index, PROCMAP func, int data, int list)
	{ return CompMapPar(index, (PROCCOM)func, data, list); }

inline bool CompMapPar(int index, PROCPIC func, int data, int list)
	{ return CompMapPar(index, (PROCCOM)func, data, list); }

inline bool CompMapPar(int index, PROCSPR func, int data, int list)
	{ return CompMapPar(index, (PROCCOM)func, data, list); }

inline bool CompMapPar(int index, PROCSND func, int data, int list)
	{ return CompMapPar(index, (PROCCOM)func, data, list); }

inline bool CompMapPar(int index, PROCMUS func, int data, int list)
	{ return CompMapPar(index, (PROCCOM)func, data, list); }


bool CompMapPar(int index, PROCCOM func);

inline bool CompMapPar(int index, PROCNUM func)
	{ return CompMapPar(index, (PROCCOM)func); }

inline bool CompMapPar(int index, PROCTXT func)
	{ return CompMapPar(index, (PROCCOM)func); }

inline bool CompMapPar(int index, PROCLOG func)
	{ return CompMapPar(index, (PROCCOM)func); }

inline bool CompMapPar(int index, PROCICO func)
	{ return CompMapPar(index, (PROCCOM)func); }

inline bool CompMapPar(int index, PROCMAP func)
	{ return CompMapPar(index, (PROCCOM)func); }

inline bool CompMapPar(int index, PROCPIC func)
	{ return CompMapPar(index, (PROCCOM)func); }

inline bool CompMapPar(int index, PROCSPR func)
	{ return CompMapPar(index, (PROCCOM)func); }

inline bool CompMapPar(int index, PROCSND func)
	{ return CompMapPar(index, (PROCCOM)func); }

inline bool CompMapPar(int index, PROCMUS func)
	{ return CompMapPar(index, (PROCCOM)func); }


/////////////////////////////////////////////////////////////////////////////
// překlad příkazu se skupinou ploch (data = počet výrazů - 1)

inline bool CompMapGrp(int index, PROCLOG func, PROCLOG func1)
	{ return CompGroup(CompMap, index, (PROCCOM)func, (PROCCOM)func1); };


/////////////////////////////////////////////////////////////////////////////
// překlad příkazu s podparametrem plochy (hledaným podle identifikace)

void CompMapSubPar(int index, int idf);
