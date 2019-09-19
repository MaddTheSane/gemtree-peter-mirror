
#include "Main.h"

/***************************************************************************\
*																			*
*									Zvuk									*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// inicializační prázdný zvuk (modifikuje se počet referencí!)

BYTE	EmptySoundData0 = 0;

SOUNDDATA EmptySoundData = { 
	1,								// čítač referencí
	0,								// velikost dat
	22050,							// vzorkovací kmitočet
	WAVE_FORMAT_PCM,				// formát
	1,								// počet kanálů
	16,								// počet bitů
	0,								// velikost záhlaví WAVEFORMATEX
	&EmptySoundData0				// adresa dat
};

const CSound		EmptySound;		// prázdný zvuk

/////////////////////////////////////////////////////////////////////////////
// vytvoření dat zvuku (při chybě paměti vrací NULL)

SOUNDDATA* _fastcall NewSoundData(int size)
{
	ASSERT(size >= 0);

// vytvoření záhlaví hudby
	SOUNDDATA* data = (SOUNDDATA*)MemGet(SIZEOFSOUNDDATA);
	if (data != NULL)
	{

// nastavení dat zvuku
		data->Refer = 1;				// počet referencí
		data->Size = size;				// velikost dat
		data->Samples = 22050;			// implicitní vzorkovací kmitočet
		data->Format = WAVE_FORMAT_PCM;	// implicitní formát
		data->Channels = 1;				// implicitně mono
		data->Bits = 16;				// implicitní počet bitů
		data->SizeHead = 0;				// velikost záhlaví WAVEFORMATEX

// vytvoření bufferu dat zvuku
		BYTE* datadata = (BYTE*)MemGet(size);
		data->Data = datadata;

// při chybě paměti zrušení záhlaví zvuku
		if (datadata == NULL)
		{
			MemFree(data);
			data = NULL;
		}
	}
	return data;
}

/////////////////////////////////////////////////////////////////////////////
// zrušení dat zvuku (odděleno kvůli lepší optimalizaci)

void _fastcall DelSoundData(SOUNDDATA* data)
{
	ASSERT(data != NULL);
	ASSERT(data->Data != NULL);

	MemFree(data->Data);		// zrušení dat zvuku
	MemFree(data);				// zrušení záhlaví zvuku
}

/////////////////////////////////////////////////////////////////////////////
// konstruktor a destruktor

CSound::CSound() 
{ 
	attach(&EmptySoundData); 
};

CSound::CSound(const CSound& src) 
{ 
	attach(src.pData); 
};

CSound::CSound(SOUNDDATA* data)
{ 
	attach(data); 
};

CSound::~CSound() 
{ 
	detach(pData);
};


/////////////////////////////////////////////////////////////////////////////
// statický konstruktor a destruktor

void CSound::Init()
{ 
	attach(&EmptySoundData); 
};

void _fastcall CSound::Init(const CSound& src) 
{ 
	attach(src.pData); 
};

void _fastcall CSound::Init(SOUNDDATA* data)
{ 
	attach(data); 
};

bool _fastcall CSound::Init(int size)			// při chybě paměti vrací FALSE, zvuk není vytvořen
{
	pData = NewSoundData(size);
	return (pData != NULL);
};

void CSound::Term()
{ 
	detach(pData);
};


/////////////////////////////////////////////////////////////////////////////
// vyprázdnění zvuku (uvolnění dat)

void CSound::Empty()
{ 
	Term(); 
	Init();
}


/////////////////////////////////////////////////////////////////////////////
// kopie do vlastního bufferu před modifikací
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool CSound::CopyWrite()
{
// úschova ukazatelů
	SOUNDDATA* olddata = pData;			// adresa starých dat
	long* refer = &(olddata->Refer);	// počet referencí

// test, zda je nutné přivlastnění
	if (*refer > 1)						// je nějaký jiný majitel?
	{

// vytvoření nového bufferu
		SOUNDDATA* newdata = NewSoundData(olddata->Size);
		if (newdata == NULL) return false;

// přenesení dat
		MemCopy(newdata->Data, olddata->Data, olddata->Size);

// přenesení parametrů
		newdata->Samples = olddata->Samples;
		newdata->Format = olddata->Format;
		newdata->Channels = olddata->Channels;
		newdata->Bits = olddata->Bits;
		newdata->SizeHead = olddata->SizeHead;

// odpojení starých dat
		detach(olddata);

// připojení nových dat
		pData = newdata;
	}

// operace OK
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření nového zvuku (připraveno k zápisu, data jsou náhodná)
// při chybě paměti vrátí FALSE, obsah bude nezměněn
// parametry (kromě Size) jsou nedefinované!

bool _fastcall CSound::New(int size)
{
// ukazatel na stará data
	SOUNDDATA* olddata = pData;

// test, zda je nutné vytvoření nového bufferu
	if ((olddata->Refer > 1) ||		// na buffer je více referencí
		(olddata->Size != size))	// nesouhlasí velikost
	{

// vytvoření nového bufferu
		SOUNDDATA* newdata = NewSoundData(size);
		if (newdata == NULL) return false;

// odpojení starých dat
		detach(olddata);

// připojení nových dat
		pData = newdata;
	}

// operace OK
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// nastavení velikosti dat
// při chybě paměti vrátí FALSE, obsah bude nezměněn

bool CSound::ReSize(int size)
{
	ASSERT(size >= 0);

// test, zda se velikost dat mění
	if (size == pData->Size) return true;

// kopie před zápisem
	if (!CopyWrite()) return false;

// nastavení nové velikosti bufferu
	BYTE* newdata = (BYTE*)MemSize(pData->Data, size);
	if (newdata == NULL) return false;

	pData->Data = newdata;
	pData->Size = size;				// velikost dat
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// načtení zvuku ze souboru (TRUE=operace OK)

int MP3freq[4] = { 44100, 4800, 3200, 0 };
BYTE MP3chan[4] = { 2, 2, 2, 1 };
BYTE MP3rate[2*4*16] = 
{
// verze 0
	0x00,0x04,0x06,0x07,0x08,0x0a,0x0c,0x0e,0x10,0x12,0x14,0x16,0x18,0x1c,0x20,0x00,	// layer 1
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x0a,0x0c,0x0e,0x10,0x12,0x14,0x00,	// layer 2
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x0a,0x0c,0x0e,0x10,0x12,0x14,0x00,	// layer 3
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// layer 4

// verze 1
	0x00,0x04,0x08,0x0c,0x10,0x14,0x18,0x1c,0x20,0x24,0x28,0x2c,0x30,0x34,0x38,0x00,	// layer 1
	0x00,0x04,0x06,0x07,0x08,0x0a,0x0c,0x0e,0x10,0x14,0x18,0x1c,0x20,0x28,0x30,0x00,	// layer 2
	0x00,0x04,0x05,0x06,0x07,0x08,0x0a,0x0c,0x0e,0x10,0x14,0x18,0x1c,0x20,0x28,0x00,	// layer 3
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00		// layer 4
};

bool CSound::LoadFile(CText jmeno)
{
// otevření souboru mapovaného do paměti (uzavřen při destrukci!)
	CFileMap file;
	if (!file.Open(jmeno)) return false;
	int size = file.Size();

// záhlaví souboru s popisovačem formátu
	WAVHEAD* wav = (WAVHEAD*) file.Adr();
	
// kontrola záhlaví souboru s popisovačem formátu
	if ((size < 40) ||
		(wav->tWavIdent[0] != 'R') ||
		(wav->tWavIdent[1] != 'I') ||
		(wav->tWavIdent[2] != 'F') ||
		(wav->tWavIdent[3] != 'F') ||
		(wav->WavFormat.tFormatIdent[0] != 'W') ||
		(wav->WavFormat.tFormatIdent[1] != 'A') ||
		(wav->WavFormat.tFormatIdent[2] != 'V') ||
		(wav->WavFormat.tFormatIdent[3] != 'E') ||
		(wav->WavFormat.tFormatIdent[4] != 'f') ||
		(wav->WavFormat.tFormatIdent[5] != 'm') ||
		(wav->WavFormat.tFormatIdent[6] != 't') ||
		(wav->WavFormat.tFormatIdent[7] != ' '))
	{

// interpretace jako soubor MP3
		DWORD hdr = *(DWORD*)file.Adr();
		int ver = ((hdr >> 11) & 0x1);
		int layer = 4 - ((hdr >> 9) & 0x3);

		int freq = MP3freq[(hdr >> 18) & 0x3];
		if (ver == 0) freq /= 2;

		int chan = MP3chan[(hdr >> 30 ) & 0x3];
		int rate = MP3rate[(ver*4 + (layer - 1))*16 + ((hdr >> 20) & 0xf)] * 1000;

		if (((hdr & 0xf0ff) == 0xf0ff) &&
			(layer == 3) &&
			(freq > 0) &&
			(rate > 0))
		{

// vytvoření bufferu souboru
#define MPEG3FORMATSIZE 30
			New(size + MPEG3FORMATSIZE + 16);	// přehrávač MPEG potřebuje rezervu za daty
			pData->Size -= 16;
			pData->Samples = freq;				// vzorkovací kmitočet
			pData->Channels = chan;				// počet kanálů
			pData->Bits = 0;					// počet bitů na vzorek
			pData->Format = WAVE_FORMAT_MPEGLAYER3;	// formát
			pData->SizeHead = MPEG3FORMATSIZE;	// velikost popisovače formátu

// inicializace záhlaví MP3
			MPEGLAYER3WAVEFORMAT* mp3 = (MPEGLAYER3WAVEFORMAT*)pData->Data;
			mp3->wfx.wFormatTag = WAVE_FORMAT_MPEGLAYER3;
			mp3->wfx.nChannels = (WORD)chan;
			mp3->wfx.nSamplesPerSec = freq;
			mp3->wfx.nAvgBytesPerSec = rate;
			mp3->wfx.nBlockAlign = (WORD)((layer == 1) ? 4 : 1);
			mp3->wfx.wBitsPerSample = 0;
			mp3->wfx.cbSize = MPEGLAYER3_WFX_EXTRA_BYTES;
			mp3->wID = MPEGLAYER3_ID_MPEG;
			mp3->fdwFlags = MPEGLAYER3_FLAG_PADDING_OFF;
			mp3->nBlockSize = (WORD)((288 << ver) * chan * rate / freq);
			mp3->nFramesPerBlock = 1;
			mp3->nCodecDelay = 1393;

// načtení dat
			MemCopy(pData->Data + MPEG3FORMATSIZE, file.Adr(), size);

// zkrácení o komentář na konci souboru
			if ((pData->Size >= (128 + MPEG3FORMATSIZE)) &&
				(pData->Data[pData->Size - 128 + 0] == 'T') &&
				(pData->Data[pData->Size - 128 + 1] == 'A') &&
				(pData->Data[pData->Size - 128 + 2] == 'G'))
			{
				pData->Size -= 128;
			}
			return true;
		}

		return false;
	}

// formát dat souboru
	int formatsize = wav->WavFormat.nFormatSize;	// velikost popisovače formátu
	int format = wav->WavFormat.wFormatTag;			// formát dat
	if (formatsize < SIZEOFWAVEFORMATEX) format = WAVE_FORMAT_PCM;	// nejsou doplňující data formátu

// první popisovač dat
	WAVDATA* dat = (WAVDATA*)((BYTE*)wav + 8 + 12 + formatsize);
	size -= 8 + 12 + formatsize + sizeof(WAVDATA);
	if ((formatsize < 14) || (formatsize > 0x7f00) || (size < 0)) return false;

// cyklus nalezení bloku s daty
	while (	(dat->tDataIdent[0] != 'd') ||
			(dat->tDataIdent[1] != 'a') ||
			(dat->tDataIdent[2] != 't') ||
			(dat->tDataIdent[3] != 'a'))
	{

// posun ukazatele na další záhlaví
		if ((DWORD)dat->nDataSize > 0x7FFF0000) return false;
		size -= dat->nDataSize + sizeof(WAVDATA);
		if (size < 0) return false;
		dat = (WAVDATA*)((BYTE*)dat + sizeof(WAVDATA) + dat->nDataSize);
	}

// příprava velikosti dat
	if (size > (long)dat->nDataSize) size = dat->nDataSize;
	if (size < 0) size = 0;

// parametry formátu dat souboru
	int channels = wav->WavFormat.nChannels;		// počet kanálů
	int samples = wav->WavFormat.nSamplesPerSec;	// vzorkovací kmitočet
	int bits = wav->WavFormat.wBitsPerSample;		// počet bitů na vzorek
	if (formatsize < 16) bits = 8;

// korekce formátu dat
	if (format == WAVE_FORMAT_PCM)
	{
		formatsize = 0;
		if (samples < ((11025 + 22050)/2))
		{
			samples = 11025;
		}
		else
		{
			if (samples > ((22050 + 44100)/2))
			{
				samples = 44100;
			}
			else
			{
				samples = 22050;
			}
		}
		if (channels != 2) channels = 1;
		if (bits != 16) bits = 8;
	}

// vytvoření bufferu
	if (!New(size + formatsize)) return false;

// kopie formátu
	if (formatsize > 0)
	{
		MemCopy(pData->Data, &(wav->WavFormat.wFormatTag), formatsize);
		((WAVEFORMATEX*)(pData->Data))->cbSize = (WORD)(formatsize - SIZEOFWAVEFORMATEX);
	}

// kopie dat
	MemCopy(pData->Data + formatsize, (BYTE*)dat + sizeof(WAVDATA), size);

// nastavení parametrů zvuku
	pData->Samples = samples;			// vzorkovací kmitočet
	pData->Format = format;				// formát
	pData->Channels = channels;			// počet kanálů
	pData->Bits = bits;					// počet bitů na vzorek
	pData->SizeHead = formatsize;		// velikost popisovače formátu

// příznak - načteno OK
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// uložení do souboru formátu WAV (FALSE=chyba)

bool CSound::SaveFile(CText jmeno) const
{
// vytvoření souboru
	CFile file;
	file.Name(jmeno);
	if (!file.Create()) return false;

// příprava záhlaví souboru
	WAVHEAD head;
	head.tWavIdent[0] = 'R';
	head.tWavIdent[1] = 'I';
	head.tWavIdent[2] = 'F';
	head.tWavIdent[3] = 'F';

	head.WavFormat.tFormatIdent[0] = 'W';
	head.WavFormat.tFormatIdent[1] = 'A';
	head.WavFormat.tFormatIdent[2] = 'V';
	head.WavFormat.tFormatIdent[3] = 'E';
	head.WavFormat.tFormatIdent[4] = 'f';
	head.WavFormat.tFormatIdent[5] = 'm';
	head.WavFormat.tFormatIdent[6] = 't';
	head.WavFormat.tFormatIdent[7] = ' ';

	head.WavData.tDataIdent[0] = 'd';
	head.WavData.tDataIdent[1] = 'a';
	head.WavData.tDataIdent[2] = 't';
	head.WavData.tDataIdent[3] = 'a';

// uložení záhlaví a dat souboru pro formát PCM
	BOOL result;

	if (pData->Format == WAVE_FORMAT_PCM)
	{
		head.nFileSize = pData->Size + sizeof(WAVHEAD) - 8;

		head.WavFormat.nFormatSize = 16;
		head.WavFormat.wFormatTag = WAVE_FORMAT_PCM;
		head.WavFormat.nChannels = (WORD)pData->Channels;
		head.WavFormat.nSamplesPerSec = pData->Samples;
		head.WavFormat.nAvgBytesPerSec = pData->Samples * pData->Channels * (pData->Bits/8);
		head.WavFormat.nBlockAlign = (WORD)(pData->Channels * (pData->Bits/8));
		head.WavFormat.wBitsPerSample = (WORD)pData->Bits;

		head.WavData.nDataSize = pData->Size;

		result = file.Write(&head, sizeof(WAVHEAD)) &&
					file.Write(pData->Data, pData->Size);
	}

// uložení záhlaví a dat pro jiný formát
	else
	{
		head.nFileSize = pData->Size + 12 + sizeof(WAVDATA);

		head.WavFormat.nFormatSize = pData->SizeHead;

		head.WavData.nDataSize = pData->Size - pData->SizeHead;

		result = file.Write(&head, 8 + 12) &&
					file.Write(pData->Data, pData->SizeHead) &&
					file.Write(&(head.WavData), sizeof(WAVDATA)) &&
					file.Write(pData->Data + pData->SizeHead, pData->Size - pData->SizeHead);
	}

// uzavření souboru
	file.Close();

// při chybě zrušení souboru
	if (!result)
	{
		file.Delete();
		return false;
	}

// příznak - uloženo OK
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// operátor přiřazení

const CSound& CSound::operator= (const CSound& src)
{
	detach(pData);			// zrušení starých dat
	attach(src.pData);		// přiřazení nových dat
	return *this;
}

const CSound& CSound::operator= (SOUNDDATA* src)
{
	detach(pData);			// zrušení starých dat
	attach(src);			// přiřazení nových dat
	return *this;
}


/***************************************************************************\
*																			*
*								Buffer zvuku								*
*																			*
\***************************************************************************/

////////////////////////////////////////////////////////////////////
// přidání záznamu o vložení položky do UNDO bufferu

template <> bool CBuffer<CSound>::UndoAddIns(int index)
{
	return Undo.AddSndIns(index);
}

////////////////////////////////////////////////////////////////////
// vytvoření nové položky - obsah náhodný (vrací index položky, <0 = chyba)
// provádí záznam do UNDO bufferu

int CBufSound::New(int size)
{
// vytvoření nové položky
	int result = New();				// vytvoření nové položky
	if (result >= 0)
	{

// inicializace objektu
		if (!m_Data[result].New(size))
		{
			Del(result);
			if (m_Undo) Undo.DelAll();
			result = -1;
		}
	}

// index nové položky (nebo <0 = chyba)
	return result;
}

////////////////////////////////////////////////////////////////////
// přidání položky (vrací index položky, <0 = chyba)
// provádí záznam do UNDO bufferu

int _fastcall CBufSound::Add(SOUNDDATA* data)
{
	CSound snd(data);
	int result = Add(snd);		// result musí být oddělen, pro inline by se špatně volal destruktor
	return result;
}
