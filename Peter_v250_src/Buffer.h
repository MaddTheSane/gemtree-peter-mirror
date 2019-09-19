
/***************************************************************************\
*																			*
*								Šablona bufferů								*
*																			*
\***************************************************************************/

// Minimální velikost objektu musí být 4 Bajty!!!!!

template <class OBJECT> class CBuffer
{

// ------------------------- interní proměnné a funkce ----------------------

protected:

// proměnné
	OBJECT*		m_Data;			// buffer dat
	bool*		m_Valid;		// buffer platnosti položek
	int			m_Num;			// počet platných položek v bufferu
	int			m_Max;			// velikost bufferu (položek)
	int			m_Next;			// příští volná položka (-1=není)
	bool		m_Undo;			// požadavek registrace změn pro UNDO
	OBJECT		m_EmptyItem;	// prázdný objekt

// vytvoření nových dat (vrací TRUE=operace OK)
	bool NewData();

// vytvoření nové položky (vrací index položky, při chybě vrací < 0)
	int NewItem();

// zrušení položky - zařazení do volných položek (nekontroluje index)
	void _fastcall DelItem(const int index);
		
// přidání záznamu o vložení položky do UNDO bufferu
	bool UndoAddIns(int index);

// ---------------------------- veřejné funkce ------------------------------

public:

// konstruktor a destruktor
	CBuffer();
	~CBuffer();

// statický konstruktor a destruktor
	void Init();
	void Term();

// zrušení všech položek v bufferu (ukládání začne opět po řadě od začátku)
// provádí záznam do UNDO bufferu, při chybě paměti vynuluje UNDO
	void DelAll();

// poskytnutí prázdného objektu
	inline OBJECT& EmptyItem() { return m_EmptyItem; };
	inline const OBJECT& EmptyItem() const { return m_EmptyItem; };

// poskytnutí bufferu dat
	inline OBJECT* Data() const { return m_Data; };

// poskytnutí bufferu platnosti položek
	inline bool* Valid() const { return m_Valid; };

// poskytnutí počtu platných položek v bufferu
	inline int Num() const { return m_Num; };

// poskytnutí velikosti bufferu (včetně zrušených položek)
	inline int Max() const { return m_Max; };

// nastavení požadavku registrace změn pro UNDO
	inline void UndoOn() { m_Undo = true; };
	inline void UndoOff() { m_Undo = false; };
	inline void UndoSet(const bool undo) { m_Undo = undo; };

// kontrola platnosti položky
	inline BOOL IsValid(const int index) const
		{ return (((DWORD)index < (DWORD)m_Max) && m_Valid[index]); };

	inline BOOL IsNotValid(const int index) const
		{ return (((DWORD)index >= (DWORD)m_Max) || !m_Valid[index]); };

// poskytnutí přístupu k položce (bez kontroly indexu)
	inline OBJECT& operator[] (const int index)
		{ ASSERT(IsValid(index)); return m_Data[index]; };

	inline const OBJECT& operator[] (const int index) const
		{ ASSERT(IsValid(index)); return m_Data[index]; };

	inline OBJECT& At(const int index)
		{ ASSERT(IsValid(index)); return m_Data[index]; };

	inline const OBJECT& At(const int index) const
		{ ASSERT(IsValid(index)); return m_Data[index]; };

// poskytnutí položky (s kontrolou platnosti indexu - pro neplatnou vrátí prázdnou položku)
	const OBJECT& _fastcall Get(const int index) const;

// nastavení položky (s kontrolou platnosti indexu)
	void _fastcall Set(const int index, const OBJECT& data);

// zrušení položky (s kontrolou platnosti indexu) (vrací FALSE=chyba paměti)
// provádí záznam do UNDO bufferu
	bool _fastcall Del(const int index);

// navrácení položky v UNDO operaci (vrací TRUE=operace OK)
// provádí záznam do UNDO bufferu
	bool UndoIns(const OBJECT& data, const int index);

// operátor přiřazení (neobsluhuje chybu paměti!)
	const CBuffer& _fastcall operator= (const CBuffer& src);

// vytvoření nové prázdné položky (vrací index položky, <0 = chyba)
// provádí záznam do UNDO bufferu
	int New();

// přidání položky (vrací index položky, <0 = chyba)
// provádí záznam do UNDO bufferu
	int _fastcall Add(const OBJECT& data);

// duplikace položky (neplatná položka se duplikuje na prázdnou, vrací index položky, <0 = chyba paměti)
// provádí záznam do UNDO bufferu
	int _fastcall Dup(const int index);
};


////////////////////////////////////////////////////////////////////
// vytvoření nových dat (vrací TRUE=operace OK)

#define NEWDATANUM 256				// počet nově vytvořených položek (typicky 1 KB)

template <class OBJECT> bool CBuffer<OBJECT>::NewData()
{
// nový počet položek
	int next = m_Max;				// příští položka - 1
	int max = next + NEWDATANUM;	// nový počet položek

// zvětšení bufferu dat
	OBJECT* newdata = (OBJECT*)MemSize(m_Data, max*sizeof(OBJECT));
	if (newdata == NULL) return false;
	m_Data = newdata;

// zvětšení bufferu platnosti
	bool* newvalid = (bool*)MemSize(m_Valid, max*sizeof(bool));
	if (newvalid == NULL) return false;
	m_Valid = newvalid;

// nový maximální počet položek v bufferu
	m_Max = max;

// vymazání příznaků platnosti položek (nastavení na příznak neplatnosti)
	int i;
	newvalid += next;
	for (i = NEWDATANUM; i > 0; i--) { *newvalid = false; newvalid++; }

// začlenění do řetězce volných položek
	newdata += next - 1;			// ukazatel dat - 1
	for (i = NEWDATANUM; i > 0; i--)
	{
		newdata++;					// zvýšení ukazatele položek
		next++;						// zvýšení indexu příští položky
		*(int*)newdata = next;		// odkaz na příští položku
	}
	*(int*)newdata = m_Next;		// navázání na další položku
	m_Next = m_Max-NEWDATANUM;		// odkaz na první novou položku

// příznak operace OK
	return true;
};

////////////////////////////////////////////////////////////////////
// vytvoření nové položky (vrací index položky, při chybě vrací < 0)

template <class OBJECT> int CBuffer<OBJECT>::NewItem()
{
// vytvoření nových dat, není-li volná další položka
	if (m_Next < 0)				// není další položka?
	{
		if (!NewData()) return -1;	// vytvoření nových dat
	}

// vyjmutí položky z řetězce volných položek
	int i = m_Next;				// příští volná položka
	m_Next = *(int*)(m_Data + i); // další položka
	m_Valid[i] = true;			// nastavení příznaku platnosti položky
	m_Num++;					// zvýšení čítače platných položek

// nově vytvořená položka
	return i;
};

////////////////////////////////////////////////////////////////////
// zrušení položky - zařazení do volných položek (nekontroluje index a neruší objekt)

template <class OBJECT> void _fastcall CBuffer<OBJECT>::DelItem(const int index)
{
	*(int*)(m_Data + index) = m_Next;	// příští volná položka
	m_Valid[index] = false;				// zrušení příznaku platnosti
	m_Num--;							// snížení čítače platných položek
	m_Next = index;						// odkaz na tuto položku
}

////////////////////////////////////////////////////////////////////
// konstruktor a destruktor

template <class OBJECT> CBuffer<OBJECT>::CBuffer()
{
	m_Data = NULL;						// není buffer dat
	m_Valid = NULL;						// není buffer platnosti
	m_Num = 0;							// není žádná platná položka
	m_Max = 0;							// není buffer položek
	m_Next = -1;						// přiští volná položka (-1=není)
	m_Undo = false;						// neregistrovat změny pro UNDO
	m_EmptyItem.Empty();
}

template <class OBJECT> CBuffer<OBJECT>::~CBuffer()
{
	DelAll();							// zrušení všech položek
}

/////////////////////////////////////////////////////////////////////////////
// statický konstruktor a destruktor

template <class OBJECT> void CBuffer<OBJECT>::Init()
{
	m_Data = NULL;						// není buffer dat
	m_Valid = NULL;						// není buffer platnosti
	m_Num = 0;							// není žádná platná položka
	m_Max = 0;							// není buffer položek
	m_Next = -1;						// přiští volná položka (-1=není)
	m_Undo = false;						// neregistrovat změny pro UNDO
	m_EmptyItem.Init();
}

template <class OBJECT> void CBuffer<OBJECT>::Term()
{
	DelAll();							// zrušení všech položek
	m_EmptyItem.Term();
}

////////////////////////////////////////////////////////////////////
// zrušení všech položek v bufferu (ukládání začne opět po řadě od začátku)
// provádí záznam do UNDO bufferu, při chybě paměti vynuluje UNDO

template <class OBJECT> void CBuffer<OBJECT>::DelAll()
{
// zrušení všech položek
	if (m_Num > 0)
	{
		for (int i = m_Max-1; i >= 0; i--)
		{
			if (!Del(i))
			{
				if (m_Undo) Undo.DelAll();
			}
		}
	}

// zrušení bufferů
	MemFree(m_Data);			// zrušení bufferu dat
	m_Data = NULL;				// není buffer dat
	MemFree(m_Valid);			// zrušení bufferu platnosti
	m_Valid = NULL;				// není bufferu platnosti
	m_Num = 0;					// není žádná platná položka
	m_Max = 0;					// není žádná položka v bufferu
	m_Next = -1;				// není příští položka
}

////////////////////////////////////////////////////////////////////
// poskytnutí položky (s kontrolou platnosti indexu - pro neplatnou vrátí prázdnou položku)

template <class OBJECT> const OBJECT& _fastcall CBuffer<OBJECT>::Get(const int index) const
{
	if (IsValid(index))			// je index platný?
	{
		return m_Data[index];	// položka na daném indexu
	}
	return m_EmptyItem;			// pro neplatný index vrátí prázdný objekt
}

////////////////////////////////////////////////////////////////////
// nastavení položky (s kontrolou platnosti indexu)

template <class OBJECT> void _fastcall CBuffer<OBJECT>::Set(const int index, const OBJECT& data)
{
	if (IsValid(index))					// je index platný?
	{
		m_Data[index] = data;
	}
}

////////////////////////////////////////////////////////////////////
// zrušení položky (s kontrolou platnosti indexu, vrací FALSE=chyba paměti)
// provádí záznam do UNDO bufferu

template <class OBJECT> bool _fastcall CBuffer<OBJECT>::Del(const int index)
{
// kontrola platnosti položky
	if (IsValid(index))						// je index platný?
	{

// záznam o zrušení do UNDO bufferu
		if (m_Undo)
		{
			if (!Undo.AddDel(index, m_Data[index]))
			{
				return false;
			}
		}

// zrušení položky
		m_Data[index].Term();				// ukončení objektu
		DelItem(index);						// zrušení položky
	}
	return true;
}

////////////////////////////////////////////////////////////////////
// navrácení položky v UNDO operaci (vrací TRUE=operace OK)
// provádí záznam do UNDO bufferu

template <class OBJECT> bool CBuffer<OBJECT>::UndoIns(const OBJECT& data, const int index)
{
// vytvoření nových dat (zajištění existence položky)
	ASSERT(index >= 0);
	while (index >= m_Max)
	{
		if (!NewData()) return false;
	}

// adresa vkládané položky
	ASSERT(IsNotValid(index));					// položka musí být neplatná
	OBJECT* item = m_Data + index;				// adresa nové položky

// záznam operace pro UNDO
	if (m_Undo)
	{
		if (!UndoAddIns(index)) return false;
	}

// vyjmutí položky z volných položek
	int i = m_Next;								// ukazatel řetězce volných položek

// navrácena hned první položka z řetězce volných položek
	if (i == index)								// je to hned první položka?
	{
		m_Next = *(int*)item;					// odkaz na další položku
	}
	else
	{

// nalezení položky v řetězci volných položek
		while (*(int*)(m_Data + i) != index)
		{
			i = *(int*)(m_Data + i);

// tento případ nesmí nikdy nastat - položka nebyla nalezena!!!
			ASSERT(i >= 0);
			if (i < 0)								// to je chyba!!!!!!!
			{
				Undo.DelAll();
				return false;
			}
		}

// přeskočení odkazu na další položku
		*(int*)(m_Data + i) = *(int*)item;
	}

// nastavení příznaku platnosti položky
	m_Num++;									// zvýšení čítače položek
	m_Valid[index] = true;						// příznak platnosti položky

// inicializace položky
	item->Init(data);

// příznak operace OK
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// operátor přiřazení (neobsluhuje chybu paměti!)

template <class OBJECT> const CBuffer<OBJECT>& _fastcall CBuffer<OBJECT>::operator= (const CBuffer<OBJECT>& src)
{
// zrušení všech starých dat
	DelAll();					// zrušení starých dat

// cyklus přes všechny položky
	int index = 0;				// index načítané položky
	for (int i = src.Max(); i > 0; i--) // pro všechny položky v bufferu
	{

// přidání položky do bufferu
		if (src.m_Valid[index])	// je to platná položka?
		{
			Add(src[index]);	// kopie položky
		}
		index++;				// inkrementace čtecího indexu
	}

// souhlasí počet položek?
	ASSERT(m_Num == src.Num());
	return *this;
}

////////////////////////////////////////////////////////////////////
// vytvoření nové prázdné položky (vrací index položky, <0 = chyba)
// provádí záznam do UNDO bufferu

template <class OBJECT> int CBuffer<OBJECT>::New()
{
// vytvoření nové položky
	int result = NewItem();				// vytvoření nové položky
	if (result >= 0)
	{

// záznam UNDO
		if (m_Undo)
		{
			if (!UndoAddIns(result))
			{
				DelItem(result);
				return -1;
			}
		}

// inicializace objektu
		m_Data[result].Init(m_EmptyItem);
	}

// index nové položky (nebo <0 = chyba)
	return result;
}


////////////////////////////////////////////////////////////////////
// přidání položky (vrací index položky, <0 = chyba paměti)
// provádí záznam do UNDO bufferu

template <class OBJECT> int _fastcall CBuffer<OBJECT>::Add(const OBJECT& data)
{
// vytvoření nové položky
	int result = NewItem();		// vytvoření nové položky
	if (result >= 0)
	{

// záznam UNDO
		if (m_Undo)
		{
			if (!UndoAddIns(result))
			{
				DelItem(result);
				return -1;
			}
		}

// inicializace objektu
		m_Data[result].Init(data);	// inicializace položky
	}

// index nové položky (nebo <0 = chyba)
	return result;
}

////////////////////////////////////////////////////////////////////
// duplikace položky (neplatná položka se duplikuje na prázdnou, vrací index položky, <0 = chyba paměti)
// provádí záznam do UNDO bufferu

template <class OBJECT> int _fastcall CBuffer<OBJECT>::Dup(const int index)
{
// vytvoření nové položky
	int result = NewItem();		// vytvoření nové položky
	if (result >= 0)
	{

// záznam UNDO
		if (m_Undo)
		{
			if (!UndoAddIns(result))
			{
				DelItem(result);
				return -1;
			}
		}

// kopie položky nebo vytvoření nové
		if (IsValid(index))					// je index platný?
		{
			m_Data[result].Init(m_Data[index]);	// kopie položky
		}
		else
		{
			m_Data[result].Init(m_EmptyItem); 	// inicializace neplatné položky
		}
	}

// index nové položky (nebo <0 = chyba)
	return result;
}
