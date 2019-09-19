
/***************************************************************************\
*																			*
*									Soubor									*
*																			*
\***************************************************************************/

class CFile
{
private:
	HANDLE	m_File;				// handle souboru (INVALID_HANDLE_VALUE=neotevřen)
	CString	m_Name;				// jméno souboru

public:
// konstruktor a destruktor
	CFile();
	~CFile();

// statický konstruktor a destruktor
//	void Init();
//	void Term();

// odpojení souboru bez uzavření (např. konzola)
	inline void Detach() { m_File = INVALID_HANDLE_VALUE; };

// poskytnutí/nastavení jména souboru
	inline void Name(const CString& name) { m_Name = name; };
	inline CString& Name() { return m_Name; };
	inline const CString& Name() const { return m_Name; };

// poskytnutí/nastavení handle souboru (INVALID_HANDLE_VALUE = není otevřen)
	inline void File(const HANDLE file) { m_File = file; };
	inline HANDLE File() const { return m_File; };
	inline operator HANDLE() const { return m_File; }

// otevření souboru pro čtení i zápis
	BOOL Open();

// otevření souboru pro čtení
	BOOL OpenRead();

// otevření souboru pro zápis
	BOOL OpenWrite();

// otevření souboru bez přístupu (nastavení vlastností)
	BOOL OpenNone();

// otevření adresáře (pro nastavení vlastností)
	BOOL OpenDirectory();

// vytvoření souboru
	BOOL Create();

// uzavření souboru
	BOOL Close();

// vytvoření/zrušení konzoly
	inline BOOL AllocConsole() { return ::AllocConsole(); };
	inline BOOL FreeConsole() { return ::FreeConsole(); };

// otevření pro standardní vstup (neuzavírat!)
	BOOL OpenInput();

// otevření pro standardní výstup (neuzavírat!)
	BOOL OpenOutput();

// otevření pro chybový výstup (neuzavírat!)
	BOOL OpenError();

// čtení ze souboru
	BOOL Read(void* buf, int num);

// zápis do souboru
	BOOL Write(void* buf, int num);

// zrušení souboru (musí být uzavřen)
	BOOL Delete();

// poskytnutí velikosti souboru (musí být otevřen, -1 = chyba)
	int Size();

// poskytnutí typu souboru (musí být otevřen)
//		FILE_TYPE_UNKNOWN ... neznámý
//		FILE_TYPE_DISK ...... diskový soubor
//		FILE_TYPE_CHAR ...... znakové zařízení
//		FILE_TYPE_PIPE ...... kanál
	int Type();

// poskytnutí/nastavení času vytvoření (musí být otevřen)
	BOOL GetCreationTime(FILETIME* time);
	BOOL SetCreationTime(FILETIME* time);

// poskytnutí/nastavení času posledního čtení (musí být otevřen)
	BOOL GetLastRead(FILETIME* time);
	BOOL SetLastRead(FILETIME* time);

// poskytnutí/nastavení času posledního zápisu (musí být otevřen)
	BOOL GetLastWrite(FILETIME* time);
	BOOL SetLastWrite(FILETIME* time);

// poskytnutí atributů souboru (nemusí být otevřen, -1=chyba)
	int Attribute();

// nastavení atributů souboru (nemusí být otevřen)
	BOOL Attribute(int atr);

// resetování ukazatele v souboru
	BOOL Reset();

// poskytnutí ukazatele v souboru (-1 = chyba)
	int Seek();

// nastavení ukazatele v souboru
	BOOL Seek(int pos);

// relativní nastavení ukazatele v souboru
	BOOL SeekRel(int pos);

// nastavení ukazatele v souboru na konec
	BOOL End();

// operátor přiřazení
	const CFile& _fastcall operator= (const CFile& file);
};


/***************************************************************************\
*																			*
*								Datový buffer								*
*																			*
\***************************************************************************/

class CBuf
{
private:
	BYTE*	m_Adr;			// adresa bufferu v paměti
	int		m_Size;			// velikost bufferu
	BYTE*	m_End;			// adresa za koncem bufferu

public:
// konstruktor
	CBuf();

// statický konstruktor
//	void Init();

// nastavení adresy a velikosti bufferu
	inline void AdrSize(BYTE* adr, int size)
		{ m_Adr = adr; m_Size = size; m_End = adr + size; };

// poskytnutí/nastavení adresy bufferu
	inline BYTE* Adr() { return m_Adr; };
	inline void Adr(BYTE* adr) { m_Adr = adr; m_End = adr + m_Size; };

// poskytnutí/nastavení velikosti bufferu
	inline int Size() { return m_Size; };
	inline void Size(int size) { m_Size = size; m_End = m_Adr + size; };

// poskytnutí adresy za koncem bufferu
	inline BYTE* End() { return m_End; };

// kontrola platnosti adresy
	inline BOOL IsValid(void* adr) { return ((adr >= m_Adr) && (adr < m_End)); };
	inline BOOL IsNotValid(void* adr) { return ((adr < m_Adr) || (adr >= m_End)); };

// kontrola platnosti bloku
	BOOL IsValid(void* adr, int size);
	BOOL IsNotValid(void* adr, int size);

// adresa NT záhlaví souboru EXE (NULL=není)
	IMAGE_NT_HEADERS* NTHeader();

// nalezení NT sekce souboru EXE (NULL=není)
	IMAGE_SECTION_HEADER* NTSection(IMAGE_NT_HEADERS* hdr, char* name);

// nalezení resource dat souboru EXE (NULL=není)
//	BYTE* NTResource();
};


/***************************************************************************\
*																			*
*					Paměťově mapovaný soubor (jen pro čtení)				*
*																			*
\***************************************************************************/

class CFileMap : public CBuf, private CFile
{
private:
	HANDLE		m_Map;		// handle mapování (NULL=neotevřeno)

// nastavení velikosti bufferu
	inline void Size(int size) { CBuf::Size(size); };

// nastavení adresy bufferu
	inline void Adr(BYTE* adr) { CBuf::Adr(adr); };

public:
// konstruktor a destruktor
	CFileMap();
	~CFileMap();

// statický konstruktor a destruktor
//	void Init();
//	void Term();

// poskytnutí velikosti souboru
	inline int Size() { return CBuf::Size(); };

// poskytnutí adresy v paměti (NULL = není otevřeno)
	inline BYTE* Adr() { return CBuf::Adr(); };

// poskytnutí adresy za paměťovým oknem (NULL = není otevřeno)
	inline BYTE* End() { return CBuf::End(); };

// kontrola platnosti adresy
	inline BOOL IsValid(void* adr) { return CBuf::IsValid(adr); };
	inline BOOL IsNotValid(void* adr) { return CBuf::IsNotValid(adr); };

// kontrola platnosti bloku
	inline BOOL IsValid(void* adr, int size) { return CBuf::IsValid(adr, size); };
	inline BOOL IsNotValid(void* adr, int size) { return CBuf::IsNotValid(adr, size); };

// otevření
	BOOL Open(const CString& name);

// uzavření (při destrukci uzavírá automaticky)
	void Close();
};


/***************************************************************************\
*																			*
*								Resource data								*
*																			*
\***************************************************************************/

class CResource : public CBuf
{
private:
	HINSTANCE	m_Instance;	// instance s daty (NULL=implicitní)

// nastavení velikosti bufferu
	inline void Size(int size) { CBuf::Size(size); };

// nastavení adresy bufferu
	inline void Adr(BYTE* adr) { CBuf::Adr(adr); };

public:
// konstruktor a destruktor
	CResource();
	~CResource();

// statický konstruktor a destruktor
//	void Init();
//	void Term();

// poskytnutí velikosti souboru
	inline int Size() { return CBuf::Size(); };

// poskytnutí adresy v paměti (NULL = není otevřeno)
	inline BYTE* Adr() { return CBuf::Adr(); };

// poskytnutí adresy za paměťovým oknem (NULL = není otevřeno)
	inline BYTE* End() { return CBuf::End(); };

// kontrola platnosti adresy
	inline BOOL IsValid(void* adr) { return CBuf::IsValid(adr); };
	inline BOOL IsNotValid(void* adr) { return CBuf::IsNotValid(adr); };

// kontrola platnosti bloku
	inline BOOL IsValid(void* adr, int size) { return CBuf::IsValid(adr, size); };
	inline BOOL IsNotValid(void* adr, int size) { return CBuf::IsNotValid(adr, size); };

// otevření souboru s resource
	BOOL OpenFile(LPCTSTR name);

// uzavření souboru s resource (při destrukci se uzavírá automaticky)
	void CloseFile();

// otevření resource
	BOOL Open(int id, LPCTSTR typ);

// otevření kopie dat (pro možnost zápisu, buffer je nutno po použití zrušit)
	BOOL OpenCopy(int id, LPCTSTR typ);

// zrušení bufferu s kopií resource dat
	void CloseCopy();

// uzavření resource (při destrukci uzavírá automaticky)
	void Close();
};
