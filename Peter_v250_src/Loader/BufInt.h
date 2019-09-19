
/***************************************************************************\
*																			*
*								Buffer celých čísel							*
*																			*
\***************************************************************************/

class CBufInt
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné
	int*	m_Data;			// ukazatel na data
	int		m_Num;			// počet platných položek v bufferu
	int		m_Max;			// velikost bufferu (položek)

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
	CBufInt();
	CBufInt(int num);		// vytvoření bufferu + vymazání hodnotou "-1"
	~CBufInt();

// statický konstruktor a destruktor
	void Init();
	void Init(int num);		// vytvoření bufferu + vymazání hodnotou "-1"
	void Term();

// zrušení všech položek v bufferu
	void DelAll();

// poskytnutí bufferu dat
	inline int* Data() const { return m_Data; };

// poskytnutí počtu platných položek v bufferu
	inline int Num() const { return m_Num; };

// nastavení počtu položek v bufferu (nové položky neinicializované)
	void _fastcall Num(int num);

// poskytnutí velikosti bufferu
	inline int Max() const { return m_Max; };

// vymazání všech položek v bufferu zadanou hodnotou
	void _fastcall Clear(const int clear = -1);

// kontrola platnosti položky
	inline BOOL IsValid(const int index) const
		{ return ((DWORD)index < (DWORD)m_Num); };

	inline BOOL IsNotValid(const int index) const
		{ return ((DWORD)index >= (DWORD)m_Num); };

// poskytnutí přístupu k položce (bez kontroly indexu)
	inline int& operator[] (const int index) 
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline const int& operator[] (const int index) const
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline int& At(const int index) 
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline const int& At(const int index) const
		{ ASSERT(IsValid(index)); return m_Data[index]; }

// poskytnutí položky (s kontrolou platnosti indexu)
	int _fastcall Get(const int index) const;

// nastavení položky (s kontrolou platnosti indexu)
	void _fastcall Set(const int index, const int data);

// vložení položky do bufferu na požadovanou pozici (omezí index)
	void _fastcall Insert(int index, const int data);

// zrušení položek z konce bufferu
	void _fastcall Del(int num);

// zrušení položky z bufferu (ostatní položky se přisunou)
	void _fastcall DelItem(const int index);

// vytvoření prázdné položky (vrací index položky)
	int New();

// přidání položky (vrací index položky)
	int _fastcall Add(const int data);

// duplikace položky (s kontrolou platnosti indexu, vrací index první položky)
	int _fastcall Dup(const int index);
	int _fastcall Dup(const int index, int num);

// operátor přiřazení
	const CBufInt& operator= (const CBufInt& src);
};


/***************************************************************************\
*																			*
*							Buffer logických proměnných						*
*																			*
\***************************************************************************/

class CBufBool
{ 

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné
	bool*	m_Data;			// ukazatel na data
	int		m_Num;			// počet platných položek v bufferu
	int		m_Max;			// velikost bufferu (položek)

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
	CBufBool();
	CBufBool(int num);		// vytvoření bufferu + vymazání hodnotou false
	~CBufBool();

// statický konstruktor a destruktor
	void Init();
	void Term();

// zrušení všech položek v bufferu
	void DelAll();

// poskytnutí bufferu dat
	inline bool* Data() const { return m_Data; };

// poskytnutí počtu platných položek v bufferu
	inline int Num() const { return m_Num; };

// poskytnutí velikosti bufferu
	inline int Max() const { return m_Max; };

// nastavení počtu položek v bufferu (nové položky neinicializované)
	void _fastcall Num(int num);

// vymazání všech položek v bufferu zadanou hodnotou
	void _fastcall Clear(const bool clear);

// kontrola platnosti položky
	inline BOOL IsValid(const int index) const
		{ return ((DWORD)index < (DWORD)m_Num); };

	inline BOOL IsNotValid(const int index) const
		{ return ((DWORD)index >= (DWORD)m_Num); };

// poskytnutí přístupu k položce (bez kontroly indexu)
	inline bool& operator[] (const int index) 
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline const bool& operator[] (const int index) const
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline bool& At(const int index) 
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline const bool& At(const int index) const
		{ ASSERT(IsValid(index)); return m_Data[index]; }

// poskytnutí položky (s kontrolou platnosti indexu)
	bool _fastcall Get(const int index) const;

// nastavení položky (s kontrolou platnosti indexu)
	void _fastcall Set(const int index, const bool data);

// zrušení položek z konce bufferu
	void _fastcall Del(int num);

// vytvoření prázdné položky (vrací index položky)
	int New();

// přidání položky (vrací index položky)
	int _fastcall Add(const bool data);

// duplikace položky (s kontrolou platnosti indexu, vrací index první položky)
	int _fastcall Dup(const int index);
	int _fastcall Dup(const int index, int num);

// operátor přiřazení
	const CBufBool& operator= (const CBufBool& src);

// načtení logické hodnoty ze souboru (načítá do proměnné, TRUE=operace OK)
//	bool LoadFile(bool& data, CString jmeno);

// uložení logické hodnoty do souboru formátu LOG (FALSE=chyba)
//	bool SaveFile(bool data, CString jmeno) const;
};
