
/***************************************************************************\
*																			*
*								Buffer seznamů								*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// struktura prvku seznamu (16 Bajtů)

typedef struct LISTDATA_
{
	long	Index;					// (4) aktuální index seznamu
	long	Size;					// (4) velikost seznamu (položek)
	long	Auto;					// (4) automatická inkrementace indexu
	long	Res;					// (4) ... rezerva
} LISTDATA;


class CBufList
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné
	LISTDATA*	m_Data;		// ukazatel na data
	int			m_Num;		// počet platných položek v bufferu
	int			m_Max;		// velikost bufferu (položek)

// vytvoření nové položky
	inline int NewItem()
	{
		int i = m_Num;				// počet položek
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
	CBufList();
	~CBufList();

// statický konstruktor a destruktor
	void Init();			// statický konstruktor
	void Term();			// statický destruktor

// zrušení všech položek v bufferu
	void DelAll();

// poskytnutí bufferu dat
	inline LISTDATA* Data() const { return m_Data; };

// poskytnutí počtu platných položek v bufferu
	inline int Num() const { return m_Num; };

// poskytnutí velikosti bufferu
	inline int Max() const { return m_Max; };

// kontrola platnosti položky
	inline BOOL IsValid(const int index) const
		{ return ((DWORD)index < (DWORD)m_Num); };

	inline BOOL IsNotValid(const int index) const
		{ return ((DWORD)index >= (DWORD)m_Num); };

// poskytnutí přístupu k položce (bez kontroly indexu)
	inline LISTDATA& operator[] (const int index)
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline const LISTDATA& operator[] (const int index) const
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline LISTDATA& At(const int index)
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline const LISTDATA& At(const int index) const
		{ ASSERT(IsValid(index)); return m_Data[index]; }

// zjištění aktuálního indexu seznamu
	inline const int _fastcall Inx(int index)
	{
		return m_Data[index].Index;
	}

// automatická inkrementace indexu
	inline void _fastcall AutoInc(int index)
	{
	// adresa položky seznamu
		LISTDATA* list = m_Data + index;

	// test, zda bude inkrementace
		if (list->Auto != 0)
		{

	// nový index
			int newindex = list->Index + list->Auto;

	// kontrola přetečení indexu
			int size = list->Size;

			if ((DWORD)newindex >= (DWORD)size)
			{
				while (newindex < 0) newindex += size;
				while (newindex >= size) newindex -= size;
			}

	// nastavení nového indexu
			list->Index = newindex;
		}
	}

// automatická inkrementace indexu, vrací původní index
	inline int _fastcall AutoIncInx(int index)
	{
	// adresa položky seznamu
		LISTDATA* list = m_Data + index;

	// úschova původního indexu
		int result = list->Index;

	// test, zda bude inkrementace
		if (list->Auto != 0)
		{

	// nový index
			int newindex = result + list->Auto;

	// kontrola přetečení indexu
			int size = list->Size;

			if ((DWORD)newindex >= (DWORD)size)
			{
				while (newindex < 0) newindex += size;
				while (newindex >= size) newindex -= size;
			}

	// nastavení nového indexu
			list->Index = newindex;
		}

	// návrat původního indexu
		return result;
	}

// poskytnutí položky (s kontrolou platnosti indexu)
	const LISTDATA& _fastcall Get(const int index) const;

// nastavení položky (s kontrolou platnosti indexu)
	void _fastcall Set(const int index, const LISTDATA& data);

// zrušení položek z konce bufferu
	void _fastcall Del(int num);

// vytvoření prázdné položky (vrací index položky)
	int New();

// přidání položky (vrací index položky)
	int _fastcall Add(const LISTDATA& data);

// duplikace položky (s kontrolou platnosti indexu, vrací index první položky)
	int _fastcall Dup(const int index);
	int _fastcall Dup(const int index, int num);

// operátor přiřazení
	const CBufList& operator= (const CBufList& src);
};

