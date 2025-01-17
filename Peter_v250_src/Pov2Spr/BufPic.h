
/***************************************************************************\
*																			*
*									Obrázky									*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// struktura položky dat obrázku - 16 bajtů + data

typedef struct PICTUREDATA_
{
	long	Refer;					// (4) čítač referencí na obrázek
	long	Width;					// (4) šířka obrázku v bodech
	long	Height;					// (4) výška obrázku v linkách
	BYTE*	Data;					// (4) ukazatel na data (velikost Width*Height)
} PICTUREDATA;

#define SIZEOFPICTUREDATA	(3*sizeof(long) + sizeof(BYTE*))

extern PICTUREDATA EmptyPictureData;	// data prázdného obrázku (velikost ICONSIZE)


/////////////////////////////////////////////////////////////////////////////
// obrázek

class CPicture
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné - pouze ukazatel na data
	PICTUREDATA*		pData;			// ukazatel na záhlaví obrázku

// připojení dat
	inline void Attach(PICTUREDATA* data)
	{
		ASSERT(data);
		pData = data;
		LongIncrement(&(data->Refer));
	}

// odpojení (a zrušení) dat
	inline void Detach()
	{
		ASSERT(pData);
		ASSERT(pData->Refer > 0);

		if (LongDecrement(&(pData->Refer)))
		{
			ASSERT(pData->Data);
			MemFree(pData->Data);
			MemFree(pData);
#ifdef _DEBUG
			pData = NULL;
#endif
		}
	}

// vytvoření nového bufferu - starý buffer musí být odpojen!
	inline void NewBuffer(int width, int height)
	{
		ASSERT((width > 0) && (height > 0));
		if (width <= 0) width = 1;
		if (height <= 0) height = 1;
		PICTUREDATA* data = (PICTUREDATA*)MemGet(SIZEOFPICTUREDATA);
		data->Refer = 1;				// počet referencí
		data->Width = width;			// šířka
		data->Height = height;			// výška
		data->Data = (BYTE*)MemGet(width*height);
		pData = data;					// adresa dat
	}

// ---------------------------- veřejné funkce ------------------------------

public:

// konstruktor a destruktor
	CPicture();							// standardní konstruktor
	CPicture(const CPicture& src);		// kopírovací konstruktor
	CPicture(int width, int height);	// konstruktor s vytvořením obrázku
	~CPicture();						// standardní destruktor

// statický konstruktor a destruktor
	void Init();						// statický konstruktor
	void Init(PICTUREDATA* data);		// statický konstruktor se zadáním dat
	void Init(int width, int height);	// statický konstruktor s vytvořením obrázku
	void Term();						// statický destruktor

// poskytnutí ukazatele na data
	inline PICTUREDATA* Data() const { return pData; }; // poskytnutí adresy dat
	inline BYTE* DataData() const { return pData->Data; }; // poskytnutí adresy dat

// poskytnutí šířky obrázky
	inline int Width() const { return pData->Width; };

// poskytnutí výšky obřázku
	inline int Height() const { return pData->Height; };

// poskytnutí velikost dat obrázku (bez záhlaví)
	inline int Size() const { return (Width()*Height()); };

// vymazání obsahu obrázku (naplnění průhlednou barvou)
	inline void Clear() { MemFill(pData->Data, Size(), BackCol); };

// kopie nových dat obrázku (rozměry zůstanou nezměněny) - zajistí odpojení dat
	void CopyData(BYTE* src);

// kopie nových dat obrázku s konverzí (rozměry zůstanou nezměněny) - zajistí odpojení dat
	void CopyKonvData(BYTE* src);

// kopie do vlastního bufferu před modifikací
	void CopyWrite();

// vytvoření nového obrázku (připraveno k zápisu, data jsou náhodná)
	void New(int width, int height);

// nastavení nových rozměrů obrázku (nová data jsou vymazána)
	void Resize(int width, int height);
	void HalfSize();

// kontrola platnosti offsetu bodu
	inline BOOL IsValid(const int off) const 
		{ return ((DWORD)off < (DWORD)Size()); };

	inline BOOL IsNotValid(const int off) const 
		{ return ((DWORD)off >= (DWORD)Size()); };

// kontrola platnosti indexu 
	inline BOOL IsValid(const int x, const int y) const
		{ return (((DWORD)x < (DWORD)pData->Width) && ((DWORD)y < (DWORD)pData->Height)); };

	inline BOOL IsNotValid(const int x, const int y) const
		{ return (((DWORD)x >= (DWORD)pData->Width) || ((DWORD)y >= (DWORD)pData->Height)); };

// poskytnutí přístupu k bodu (bez kontroly offsetu/indexu)
	inline BYTE& operator[] (const int off) 
		{ ASSERT(IsValid(off)); return pData->Data[off]; }

	inline const BYTE& operator[] (const int off) const
		{ ASSERT(IsValid(off)); return pData->Data[off]; }

	inline BYTE& At(const int off) 
		{ ASSERT(IsValid(off)); return pData->Data[off]; }

	inline const BYTE& At(const int off) const
		{ ASSERT(IsValid(off)); return pData->Data[off]; }

	inline BYTE& At(const int x, const int y) 
		{ ASSERT(IsValid(x, y)); return pData->Data[x + y*Width()]; }

	inline const BYTE& At(const int x, const int y) const
		{ ASSERT(IsValid(x, y)); return pData->Data[x + y*Width()]; }

// poskytnutí bodu (s kontrolou platnosti offsetu/indexu)
	BYTE _fastcall Get(const int off) const;
	BYTE _fastcall Get(const int x, const int y) const;

// nastavení bodu (s kontrolou platnosti offsetu/indexu)
	void _fastcall Set(const int off, const BYTE data);
	void _fastcall Set(const int x, const int y, const BYTE data);

// načtení obrázku ze souboru (TRUE=operace OK)
//	BOOL LoadFile(CString jmeno);

// načtení obrázku ze souboru s poloviční velikostí (TRUE=operace OK)
	BOOL LoadFile2(CString jmeno);

// uložení obrázku do souboru formátu BMP (FALSE=chyba)
	BOOL SaveFile(CString jmeno) const;

// operátor přiřazení
	const CPicture& operator= (const CPicture& src);
};

/***************************************************************************\
*																			*
*								Buffer obrázků								*
*																			*
\***************************************************************************/

class CBufPic
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné
	CPicture*	m_Data;		// ukazatel na data
	BYTE*		m_Valid;	// příznaky platnosti položek
	int			m_Num;		// počet platných položek v bufferu
	int			m_Max;		// velikost bufferu (položek)
	int			m_Next;		// příští volná položka (-1=není)

// vytvoření nové položky
	inline int NewItem()
	{
		if (m_Next < 0)				// není další položka?
		{
			NewData();				// vytvoření nových dat
		}

		int i = m_Next;				// příští volna položka
		m_Next = *(int*)(m_Data + i); // další položka
		m_Valid[i] = TRUE;			// nastavení příznaku platnosti položky
		m_Num++;					// zvýšení čítače platných položek
		return i;
	};

// vytvoření nových dat (odděleno kvůli lepší optimalizaci)
	void NewData();

// ---------------------------- veřejné funkce ------------------------------

public:

// konstruktor a destruktor
	CBufPic();
	~CBufPic();

// statický konstruktor a destruktor
	void Init();
	void Term();

// zrušení všech položek v bufferu (ukládání začne opět po řadě od začátku)
	void DelAll();

// poskytnutí bufferu dat
	inline CPicture* Data() const { return m_Data; };

// poskytnutí pole platností
	inline BYTE* Valid() const { return m_Valid; };

// poskytnutí počtu platných položek v bufferu
	inline int Num() const { return m_Num; };

// poskytnutí velikosti bufferu (včetně zrušených položek)
	inline int Max() const { return m_Max; };

// kontrola platnosti položky
	inline BOOL IsValid(const int index) const
		{ return (((DWORD)index < (DWORD)m_Max) && m_Valid[index]); };

	inline BOOL IsNotValid(const int index) const
		{ return (((DWORD)index >= (DWORD)m_Max) || !m_Valid[index]); };

// poskytnutí přístupu k položce (bez kontroly indexu)
	inline CPicture& operator[] (const int index)
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline const CPicture& operator[] (const int index) const
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline CPicture& At(const int index)
		{ ASSERT(IsValid(index)); return m_Data[index]; }

	inline const CPicture& At(const int index) const
		{ ASSERT(IsValid(index)); return m_Data[index]; }

// poskytnutí položky (s kontrolou platnosti indexu)
	const CPicture& _fastcall Get(const int index) const;

// nastavení položky (s kontrolou platnosti indexu)
	void _fastcall Set(const int index, const CPicture& data);

// zrušení položky (s kontrolou platnosti indexu)
	void _fastcall Del(const int index);

// vytvoření prázdné položky (vrací index položky)
	int New();
	int New(int width, int height);			// obrázek vymaže průhlednou barvou

// přidání položky (vrací index položky)
	int _fastcall Add(const CPicture& data);
	int _fastcall Add(PICTUREDATA* data);

// duplikace položky (s kontrolou platnosti indexu, vrací index položky)
	int _fastcall Dup(const int index);

// operátor přiřazení
	const CBufPic& operator= (const CBufPic& src);
};

