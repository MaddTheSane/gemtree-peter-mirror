
/***************************************************************************\
*																			*
*									Zvuk									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// struktura položky dat zvuku (32 bajtů)
// pro jiný formát než PCM je na začátku dat popisovač formátu WAVEFORMATEX s udanou
// velikostí doplňujících dat "cbSize", za popisovačem bezprostředně následují data

typedef struct SOUNDDATA_
{
	long	Refer;					// (4) čítač referencí
	long	Size;					// (4) velikost dat (bajtů)	- včetně případného popisovače WAVEFORMATEX
	long	Samples;				// (4) vzorkovací kmitočet - vzorků za sekundu
									//		(pro PCM jsou jen hodnoty 11025/22050/44100)
	long	Format;					// (4) formát dat (1 = PCM, pro jiný formát je na začátku dat záhlaví WAVEFORMATEX)
	long	Channels;				// (4) počet kanálů (pro PCM jsou jen hodnoty 1 = mono, 2 = stereo)
	long	Bits;					// (4) počet bitů na vzorek kanálu (pro PCM jsou jen hodnoty 8 nebo 16)
	long	SizeHead;				// (4) velikost záhlaví WAVEFORMATEX na začátku dat (pro PCM je = 0)
	BYTE*	Data;					// (4) ukazatel na data (s případným popisovačem WAVEFORMATEX na začátku)

} SOUNDDATA;

// velikost položky dat zvuku (bez dat)
#define SIZEOFSOUNDDATA	(7*sizeof(long) + sizeof(BYTE*))

extern SOUNDDATA EmptySoundData;			// data prázdného zvuku
extern	const CSound		EmptySound;		// prázdný zvuk


/////////////////////////////////////////////////////////////////////////////
// standardní popisovač pro export/import

#pragma pack( push )
#pragma pack( 2 )

// popisovač dat souboru WAV (8 B)
typedef struct WAVDATA_ {
	char			tDataIdent[4];		// (4) identifikace dat "data"
	DWORD			nDataSize;			// (4) velikost následujících dat zvuku
} WAVDATA;

// popisovač formátu souboru WAV (28 B)
typedef struct WAVFORMAT_ {
	char			tFormatIdent[8];	// (8) identifikace záhlaví formátu (= "WAVEfmt ")
	DWORD			nFormatSize;		// (4) velikost následujících dat formátu

	WORD			wFormatTag;			// (2) formát dat (1 = PCM)
	WORD			nChannels;			// (2) počet kanálů (1 = mono, 2 = stereo)
	DWORD			nSamplesPerSec;		// (4) vzorkovací kmitočet (vzorků za sekundu)
	DWORD			nAvgBytesPerSec;	// (4) přenosová rychlost (bajtů za sekundu)
	WORD			nBlockAlign;		// (2) zarovnávání dat (bity*kanály/8)

	WORD			wBitsPerSample;		// (2) počet bitů na jeden vzorek
} WAVFORMAT;

// struktura souboru WAV (44 B)
typedef struct WAVHEAD_ {
	char		tWavIdent[4];		// (4) identifikace souboru (= "RIFF")
	DWORD		nFileSize;			// (4) velikost následujících dat souboru

	WAVFORMAT	WavFormat;			// blok popisovače formátu
	WAVDATA		WavData;			// blok dat zvuku
} WAVHEAD;

#pragma pack( pop )

/////////////////////////////////////////////////////////////////////////////
// statické funkce

// vytvoření dat zvuku (při chybě paměti vrací NULL)
SOUNDDATA* _fastcall NewSoundData(int size);

// zrušení dat zvuku (odděleno kvůli lepší optimalizaci)
void _fastcall DelSoundData(SOUNDDATA* data);


/////////////////////////////////////////////////////////////////////////////
// zvuk

class CSound
{
// ------------------------- interní proměnné a funkce ----------------------

private:

// proměnné - pouze ukazatel na data
	SOUNDDATA*		pData;					// ukazatel na záhlaví zvuku

// připojení dat zvuku
	inline void attach(SOUNDDATA* data)
	{
		ASSERT(data != NULL);

		pData = data;
		LongIncrement(&(data->Refer));
	}

// odpojení (a zrušení) dat zvuku
	inline void detach(SOUNDDATA* data)
	{
		ASSERT(data);
		ASSERT(data->Refer > 0);

		if (LongDecrement(&(data->Refer)))
		{
			DelSoundData(data);
		}
	}


// ---------------------------- veřejné funkce ------------------------------

public:

// konstruktor a destruktor
	CSound();					// prázdný zvuk o délce 0
	CSound(const CSound& src);
	CSound(SOUNDDATA* data);
	~CSound();

// statický konstruktor a destruktor
	void Init();				// prázdný zvuk o délce 0
	void _fastcall Init(const CSound& src);
	void _fastcall Init(SOUNDDATA* data);
	bool _fastcall Init(int size);	// při chybě paměti vrací FALSE, zvuk není vytvořen

	void Term();

// poskytnutí ukazatele na data
	inline SOUNDDATA* Data() const { return pData; };
	inline BYTE* DataData() const { return pData->Data; };

// poskytnutí velikosti dat zvuku (bez záhlaví)
	inline int Size() const { return pData->Size; };

// poskytnutí/nastavení vzorkovacího kmitočtu
	inline int Samples() const { return pData->Samples; };
	inline void Samples(int samples) { pData->Samples = samples; };

// poskytnutí/nastavení formátu dat
	inline int Format() const { return pData->Format; };
	inline void Format(int format) { pData->Format = format; };

// poskytnutí/nastavení počtu kanálů
	inline int Channels() const { return pData->Channels; };
	inline void Channels(int channels) { pData->Channels = channels; };

// poskytnutí/nastavení počtu bitů na vzorek
	inline int Bits() const { return pData->Bits; };
	inline void Bits(int bits) { pData->Bits = bits; };

// vyprázdnění (uvolnění dat) - připojí se standardní prázdný zvuk
	void Empty();

// kopie do vlastního bufferu před modifikací
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool CopyWrite();

// vytvoření nového zvuku (připraveno k zápisu, data i parametry jsou náhodné)
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool _fastcall New(int size);

// nastavení velikosti zvuku
// při chybě paměti vrátí FALSE, obsah bude nezměněn
	bool ReSize(int size);

// načtení zvuku ze souboru (TRUE=operace OK)
	bool LoadFile(CText jmeno);

// uložení do souboru formátu WAV (FALSE=chyba)
	bool SaveFile(CText jmeno) const;

// operátor přiřazení
	const CSound& operator= (const CSound& src);
	const CSound& operator= (SOUNDDATA* src);
};

/***************************************************************************\
*																			*
*								Buffer zvuku								*
*																			*
\***************************************************************************/

class CBufSound : public CBuffer<CSound>
{

// ---------------------------- veřejné funkce ------------------------------

public:

// vytvoření nové položky - obsah náhodný (vrací index položky, <0 = chyba)
// provádí záznam do UNDO bufferu
	inline int New() { return CBuffer<CSound>::New(); }
	int New(int size);

// přidání položky (vrací index položky, <0 = chyba)
// provádí záznam do UNDO bufferu
	inline int _fastcall Add(const CSound& data) { return CBuffer<CSound>::Add(data); }
	int _fastcall Add(SOUNDDATA* data);
};

