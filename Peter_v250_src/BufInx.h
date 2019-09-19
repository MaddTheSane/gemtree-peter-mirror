
/***************************************************************************\
*																			*
*							Buffer přemapování indexů						*
*																			*
\***************************************************************************/

class CBufIndex
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné
	int*	m_Data;			// ukazatel na data
	int		m_Num;			// počet položek v bufferu
	int		m_Max;			// velikost bufferu (položek)

// ---------------------------- veřejné funkce ------------------------------

public:

// konstruktor a destruktor
	CBufIndex();
	~CBufIndex();

// statický konstruktor a destruktor
	void Init();
	void Term();

// zrušení všech položek v bufferu
	void DelAll();

// poskytnutí adresy dat
	inline int* Data() const { return m_Data; };

// poskytnutí/nastavení počtu položek v bufferu
// (nové položky neinicializované, vrací FALSE=chyba paměti)
	inline int Num() const { return m_Num; };
	bool _fastcall Num(const int num);

// nastavení počtu položek s vymazáním "-1", vrací FALSE=chyba paměti
	bool _fastcall NumClear(const int num);

// vymazání všech položek v bufferu zadanou hodnotou
	void _fastcall Clear(const int clear);

// kontrola platnosti položky (kontroluje se pouze rozsah indexu)
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

// poskytnutí položky (s kontrolou platnosti indexu, pro neplatnou vrátí -1)
	int _fastcall Get(const int index) const;

// nastavení položky (s kontrolou platnosti indexu)
	void _fastcall Set(const int index, const int data);

// přidání položky na konec bufferu (vrací index položky, při chybě paměti vrací <0)
	int _fastcall Add(const int data);

// operátor přiřazení (při chybě paměti počet položek nesouhlasí)
	const CBufIndex& _fastcall operator= (const CBufIndex& srcbuf);
};
