
/***************************************************************************\
*																			*
*							Buffer reálných čísel							*
*																			*
\***************************************************************************/


class CBufReal
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné
	double*		m_Data;		// ukazatel na data
	int			m_Num;		// počet platných položek v bufferu
	int			m_Max;		// velikost bufferu (položek)

// vytvoření nové položky
	inline int NewItem()
	{
		int i = m_Num;
		if (i >= m_Max)				// není další položka?
		{
			NewData();				// vytvoření nových dat
		}
		m_Num = i + 1;
		return i;
	};

// vytvoření nových dat (odděleno kvůli lepší optimalizaci)
	void NewData();


// ---------------------------- veřejné funkce ------------------------------

public:

// konstruktor a destruktor
	CBufReal();
	~CBufReal();

// statický konstruktor a destruktor
	void Init();			// statický konstruktor
	void Term();			// statický destruktor

// zrušení všech položek v bufferu
	void DelAll();

// zjištění počtu platných položek v bufferu
	inline int Num() const { return m_Num; };

// nastavení počtu položek v bufferu (nové položky neinicializované)
	void _fastcall Num(int num);

// zjištění velikosti bufferu (včetně zrušených položek)
	inline int Max() const { return m_Max; };

// data bufferu
	inline double* Data() const { return m_Data; };

// kontrola platnosti položky
	inline BOOL IsValid(const int index) const
		{ return ((DWORD)index < (DWORD)m_Num); };

	inline BOOL IsNotValid(const int index) const
		{ return ((DWORD)index >= (DWORD)m_Num); };

// poskytnutí přístupu k položce (bez kontroly indexu)
	inline double& operator[] (const int index)
		{ ASSERT(IsValid(index)); return m_Data[index]; }

// poskytnutí přístupu k položce (bez kontroly indexu)
	inline const double& operator[] (const int index) const
		{ ASSERT(IsValid(index)); return m_Data[index]; }

// poskytnutí položky (s kontrolou platnosti indexu)
	double _fastcall Get(const int index) const;

// nastavení položky (s kontrolou platnosti indexu)
	void _fastcall Set(const int index, const double data);

// vložení položky do bufferu na požadovanou pozici (omezí index)
	void _fastcall Insert(int index, const double data);

// zrušení položek z konce bufferu
	void _fastcall Del(int num);

// zrušení položky z bufferu (ostatní položky se přisunou)
	void _fastcall DelItem(const int index);

// vytvoření prázdné položky (vrací index položky)
	int New();

// přidání položky (vrací index položky)
	int _fastcall Add(const double data);

// duplikace položky (s kontrolou platnosti indexu, vrací index první položky)
	int _fastcall Dup(const int index);
	int _fastcall Dup(const int index, int num);

// načtení čísla ze souboru (načítá do proměnné, true=operace OK)
//	bool LoadFile(double& data, CString jmeno);

// uložení čísla do souboru formátu NUM (false=chyba)
//	bool SaveFile(double data, CString jmeno) const;

// operátor přiřazení
	const CBufReal& operator= (const CBufReal& src);
};
