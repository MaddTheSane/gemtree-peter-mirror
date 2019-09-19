
/***************************************************************************\
*																			*
*									Obrázky									*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// struktura položky dat obrázku - 32 bajtů
// Při komprimaci udává první dvojslovo dat délku komprimovaných dat
// Dekomprimace i komprimace se provádí s daty společnými pro všechny obrázky
// je proto nutno v případě potřeby zajistit vlastnictví obrázku!

typedef struct PICTUREDATA_
{
	long	Refer;					// (4) čítač referencí na obrázek
	long	Width;					// (4) šířka obrázku v bodech
	long	Height;					// (4) výška obrázku v linkách
	long	Param;					// (4) parametry (průhlednost, komprese)
	long	Res1;					// (4) ... pro zarovnání
	long	Res2;					// (4) ... pro zarovnání
	long	Res3;					// (4) ... pro zarovnání
	BYTE*	Data;					// (4) ukazatel na data (velikost Width*Height)
} PICTUREDATA;

//////////////////////////////////////////////////////////////////////////////
// příznaky typu ikony a obrázku
//enum PICPARAM {
//	PicParamPic,			// pouze obrázek bez pozadí
//	PicParamMix,			// obrázek mixovaný s pozadím
//	PicParamBack,			// pouze pozadí (prázdný obrázek)
//	PicParamNone,			// neznámý obsah
//	PicParamComp,			// komprimovaná data
//};

#define SIZEOFPICTUREDATA	(7*sizeof(long) + sizeof(BYTE*))

extern PICTUREDATA EmptyPictureData;	// data prázdného obrázku (velikost ICONSIZE)
extern	const CPicture	EmptyPicture;	// prázdný obrázek

/////////////////////////////////////////////////////////////////////////////
// statické funkce

// statická inicializace obrázků (při chybě paměti vrací FALSE)
bool InitPicture();

// vytvoření dat obrázku (při chybě paměti vrací NULL)
PICTUREDATA* _fastcall NewPictureData(int width, int height);

// zrušení dat obrázku (odděleno kvůli lepší optimalizaci)
void _fastcall DelPictureData(PICTUREDATA* data);


/////////////////////////////////////////////////////////////////////////////
// obrázek

class CPicture
{

// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné - pouze ukazatel na data
	PICTUREDATA*		pData;			// ukazatel na záhlaví obrázku

// připojení dat obrázku
	inline void attach(PICTUREDATA* data)
	{
		ASSERT(data != NULL);

		pData = data;
		LongIncrement(&(data->Refer));
	}

// odpojení (a zrušení) dat obrázku
	inline void detach(PICTUREDATA* data)
	{
		ASSERT(data != NULL);
		ASSERT(data->Refer > 0);

		if (LongDecrement(&(data->Refer)))
		{
			DelPictureData(data);
		}
	}

// ---------------------------- veřejné funkce ------------------------------

public:

// konstruktor a destruktor
	CPicture();						// prázdný obrázek o velikosti ICONSIZE
	CPicture(const CPicture& src);
	CPicture(PICTUREDATA* data);
	~CPicture();

// statický konstruktor a destruktor
	void Init();					// prázdný obrázek o velikosti ICONSIZE
	void _fastcall Init(const CPicture& src);
	void _fastcall Init(PICTUREDATA* data);
	bool _fastcall Init(int width, int height); // při chybě paměti vrací FALSE, obrázek není vytvořen
	void Term();

// poskytnutí ukazatele na data
	inline PICTUREDATA* Data() const { return pData; }; // poskytnutí adresy dat
	inline BYTE* DataData() const { return pData->Data; }; // poskytnutí adresy dat

// poskytnutí šířky obrázky
	inline int Width() const { return pData->Width; };

// poskytnutí výšky obřázku
	inline int Height() const { return pData->Height; };

// poskytnutí velikosti nezkomprimovaných dat obrázku (bez záhlaví)
	inline int Size() const { return pData->Width * pData->Height; };

// vyprázdnění (uvolnění dat)
	void Empty();

// test, zda je obrázek prázdný (zda ukazuje na standardní prázdný obrázek ICONSIZE)
	inline BOOL IsEmpty() { return ((DWORD)pData == (DWORD)&EmptyPictureData); };
	inline BOOL IsNotEmpty() { return ((DWORD)pData != (DWORD)&EmptyPictureData); };

// vymazání obsahu obrázku (naplnění průhlednou barvou), zajistí přivlastnění (a dekomprimaci) bufferu,
// při chybě paměti vrací FALSE, původní obsah nezměněn
	bool Clear();

// vymazání obsahu obrázku s nastavením velikosti (naplnění průhlednou barvou), zajistí přivlastnění (a dekomprimaci) bufferu,
// při chybě paměti vrací FALSE, původní obsah nezměněn
	bool _fastcall Clear(int width, int height);

// kopie obrázku do vlastního bufferu před modifikací (komprimovaná data zůstanou komprimovaná)
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool CopyWrite();

// vytvoření nového obrázku se stejnou velikostí (připraveno k zápisu, data jsou náhodná)
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool New();

// vytvoření nového obrázku (připraveno k zápisu, data jsou náhodná)
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool _fastcall New(int width, int height);

// nastavení nových rozměrů obrázku (nová data jsou vymazána)
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool Resize(int width, int height);

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

// poskytnutí přístupu k bodu (bez kontroly offsetu/indexu) - obrázek nesmí být komprimovaný!
// v případě zápisu musí být zajištěno přivlastnění bufferu!
	inline BYTE& operator[] (const int off) 
		{ ASSERT(IsValid(off)); return pData->Data[off]; }

	inline const BYTE& operator[] (const int off) const
		{ ASSERT(IsValid(off)); return pData->Data[off]; }

	inline BYTE& At(const int off) 
		{ ASSERT(IsValid(off)); return pData->Data[off]; }

	inline const BYTE& At(const int off) const
		{ ASSERT(IsValid(off)); return pData->Data[off]; }

	inline BYTE& At(const int x, const int y) 
		{ ASSERT(IsValid(x, y)); return pData->Data[x + y*pData->Width]; }

	inline const BYTE& At(const int x, const int y) const
		{ ASSERT(IsValid(x, y)); return pData->Data[x + y*pData->Width]; }

// poskytnutí bodu (s kontrolou platnosti offsetu/indexu), obrázek nesmí být komprimovaný!
// pro neplatný bod vrátí barvu pozadí)
	BYTE _fastcall Get(const int off) const;
	BYTE _fastcall Get(const int x, const int y) const;

// nastavení bodu (s kontrolou platnosti offsetu/indexu), obrázek nesmí být komprimovaný!
// před zápisem je nutno zajistit přivlastnění bufferu!
	void _fastcall Set(const int off, const BYTE data);
	void _fastcall Set(const int x, const int y, const BYTE data);

// komprese dat obrázku, vrací velikost dat (při chybě paměti vrací <0, data jsou nezměněna)
// komprese se provádí ve společném bufferu pro všechny proměnné!
// (v případě potřeby je nutno zajistit přivlastnění bufferu)
// původní buffer s daty je zrušen
// (velikost komprimovaných dat je udávána bez dvojslova s velikostí, buffer s daty
// je tedy o 4 větší, data začínají na offsetu 4 v bufferu)
	int Comp() const;

// dekomprimace dat obrázku (jsou-li komprimována), při chybě vrací FALSE
// dekomprese se provádí ve společném bufferu pro všechny proměnné
// (v případě potřeby je nutno zajistit přivlastnění bufferu)
// původní buffer s komprimovanými daty je zrušen
	bool DeComp() const;

// kopie nových dat obrázku (rozměry zůstanou nezměněny) - zajistí odpojení dat
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool CopyData(BYTE* src);

// kopie komprimovaných nových dat obrázku (rozměry zůstanou nezměněny) - zajistí odpojení dat
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool CompCopy(BYTE* src);

// kopie výřezu do obrázku (se zprůhledněním)
	void AddCopy(const CPicture& src, int x, int y, int w, int h, int xs, int ys);
	void AddCopy(const CPicture& src, int x, int y);
	void AddCopy(int id, int x, int y);

// kopie nových dat obrázku s konverzí (rozměry zůstanou nezměněny) - zajistí odpojení dat
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool CopyKonvData(BYTE* src);

// načtení obrázku ze souboru (FALSE=chyba, obsah nezměněn)
//	bool LoadFile(CText jmeno);

// uložení obrázku do souboru formátu BMP (komprimovaný obrázek dekomprimuje)(FALSE=chyba)
//	bool SaveFile(CText jmeno) const;

// vygenerování obrázku pro tažení (při chybě vrací NULL)
//	HIMAGELIST GenerDrag(const CText& text);

// načtení obrázku z resource
	bool CPicture::Load(const int nID);

// operátor přiřazení
	const CPicture& operator= (const CPicture& src);
	const CPicture& operator= (PICTUREDATA* src);
};

