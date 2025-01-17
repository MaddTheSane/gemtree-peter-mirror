
#include "Main.h"

/***************************************************************************\
*																			*
*									Editor zvuků							*
*																			*
\***************************************************************************/

namespace EditSound
{

/////////////////////////////////////////////////////////////////////////////
// parametry

// editovaný zvuk
int		Index = 0;						// editovaná položka
SOUNDDATA* Data = NULL;					// editovaný zvuk
int		BytesPerSec;					// počet bajtů za sekundu

// zobrazení
int		DLeft;							// souřadnice X displeje
int		DTop;							// souřadnice Y displeje
int		DWidth;							// šířka displeje
int		DHeight;						// výška displeje

// zobrazení času
HWND	DispWnd = NULL;					// okno zobrazení času
int		LastTime = 0;					// naposledy zobrazený čas (sekund)

// ovládání
BOOL	Play = FALSE;					// probíhá přehrávání
BOOL	Record = FALSE;					// probíhá nahrávání
BOOL	Loop = FALSE;					// zapnuta smyčka
BOOL	Pause = FALSE;					// zapauzování přehrávání

// přehrávání a narávání
HWAVEOUT	WaveOut;					// handle zařízení pro přehrávání
HWAVEIN		WaveIn;						// handle zařízení pro nahrávání
WAVEHDR		WaveHdr[2];					// popisovač bufferu dat
DWORD		Timer = 0;						// časovač pro zobrazení času
char*		DataData = NULL;			// ukazatel dat při přehrávání
int			DataSize = 0;				// čítač zbylých dat při přehrávání
int			MaxBufSize = 0x20000;		// max. velikost bufferu

#define SoundTimerID 15123				// ID časovače
#define MAXLENGTH 60					// maximální délka nahrávání (sekund)
#define SOUNDDEFSAMPLES		22050
#define SOUNDDEFCHANNELS	1
#define SOUNDDEFBITS		16
#define SOUNDDEFRATE		(SOUNDDEFSAMPLES * SOUNDDEFCHANNELS * (SOUNDDEFBITS/8))
#define SOUNDDEFALIGN		(SOUNDDEFCHANNELS * (SOUNDDEFBITS/8))

/////////////////////////////////////////////////////////////////////////////
// inicializace při startu programu

void StartInit()
{
// vytvoření okna displeje
	DispWnd = ::CreateWindowEx(
		WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME, // rozšířený styl
		_T("STATIC"),					// třída
		_T("00:00"),					// text
		WS_CHILD | SS_CENTER,			// styl
		300,							// X
		200,							// Y
		200,							// šířka
		100,							// výška
		MainFrame,						// rodič
		NULL,							// ID
		hInstance,						// instance
		NULL);							// data

// nastavení fontu okna displeje
	HFONT font = ::CreateFont(80, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, _T("Courier New"));
	::SendMessage(DispWnd, WM_SETFONT, (WPARAM)font, FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// zahájení editace (musí být platný index!)

void BegEdit(int index)
{
// index prvku
	Index = index;
	ASSERT(Sound.IsValid(index));

// vypnutí případného přehrávání
	OnStop();

// kopie před editací
	Sound[index].CopyWrite();

// adresa zvuku
	Data = Sound[index].Data();

// počet bajtů za sekundu
	BytesPerSec = Data->Samples * Data->Channels * (Data->Bits/8);
	if (BytesPerSec == 0)
	{
		BytesPerSec = Data->Samples * Data->Channels / 2;
		if ((Data->Format >= 0x50) && (Data->Format < 0x60))
		{
			WAVEFORMATEX* wfx = (WAVEFORMATEX*)(Data->Data);
			BytesPerSec = wfx->nAvgBytesPerSec;
		}
	}
	if (BytesPerSec <= 0) BytesPerSec = 1;

// zobrazení délky zvuku
	DispLength();

// zobrazení okna
	::ShowWindow(DispWnd, SW_SHOW);

// zobrazení
	Disp();

// aktualizace voleb
	UpdateMenu();
}


/////////////////////////////////////////////////////////////////////////////
// přesun okna displeje

HDWP MoveDisp(HDWP hdwp)
{
	int oldwidth = DWidth;
	int oldheight = DHeight;

	DWidth = 235;
	if (DWidth > EditWidth-4) DWidth = EditWidth-4;
	if (DWidth < 1) DWidth = 1;

	DHeight = 85;
	if (DHeight > EditHeight-4) DHeight = EditHeight-4;
	if (DHeight < 1) DHeight = 1;

	DLeft = EditX + (EditWidth - DWidth)/2;
	DTop = EditY + (EditHeight - DHeight)/2;

	hdwp = ::DeferWindowPos(hdwp, DispWnd, NULL, 
			DLeft, DTop, DWidth, DHeight, 
			(((oldwidth != DWidth) || (oldheight != DHeight)) ? SWP_NOCOPYBITS : 0) |
			SWP_DRAWFRAME | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	return hdwp;
}


/////////////////////////////////////////////////////////////////////////////
// nastavení časového údaje (v sekundách)

void DispSet(int time)
{
	if (time != LastTime)
	{
		LastTime = time;

		CText txt;

		txt = (TCHAR)((time / 600) % 10 + '0');
		time %= 600;

		txt.Add((TCHAR)(time / 60 + '0'));
		time %= 60;

		txt.Add(_T(':'));

		txt.Add((TCHAR)(time / 10 + '0'));
		time %= 10;

		txt.Add((TCHAR)(time + '0'));

		txt.SetWindowText(DispWnd);
	}
}


/////////////////////////////////////////////////////////////////////////////
// zobrazení délky zvuku

void DispLength()
{
	DispSet((Data->Size + BytesPerSec/2)/ BytesPerSec);
}


/////////////////////////////////////////////////////////////////////////////
// zobrazení okna (vymazání okolí kolem displeje)

void Disp()
{
// otevření kontextu displeje
	HDC dc = ::GetDC(MainFrame);

// příprava štětce k vymazání podkladu
	HBRUSH brush = (HBRUSH)::GetStockObject(LTGRAY_BRUSH); // štětec k vymazání plochy

// vymazání plochy nahoře nad displejem
	RECT rc;
	rc.left = EditX + 2;
	rc.right = EditX + EditWidth - 2;
	rc.top = EditY + 2;
	rc.bottom = DTop;
	if (rc.top < rc.bottom)
	{
		::FillRect(dc, &rc, brush);
	}

// vymazání plochy dole pod displejem (left a right je nastaveno)
	rc.top = DTop + DHeight;
	rc.bottom = EditY + EditHeight - 2;
	if (rc.top < rc.bottom)
	{
		::FillRect(dc, &rc, brush);
	}

// vymazání plochy vlevo od displeje (left je nastaveno)
	rc.right = DLeft;
	rc.top = DTop;
	rc.bottom = DTop + DHeight;
	if (rc.left < rc.right)
	{
		::FillRect(dc, &rc, brush);
	}

// vymazání plochy vpravo od displeje (top a bottom je nastaveno)
	rc.left = DLeft + DWidth;
	rc.right = EditX + EditWidth - 2;
	if (rc.left < rc.right)
	{
		::FillRect(dc, &rc, brush);
	}

// zrušení štětce podkladu (i když podle dokumentace rušení není nutné)
	::DeleteObject(brush);

// uvolnění kontextu displeje
	::ReleaseDC(MainFrame, dc);
}


/////////////////////////////////////////////////////////////////////////////
// start přehrávání

void OnPlay()
{
// v pauze pokračování pauzou
	if (Pause)
	{
		OnPause();
		return;
	}

// v módu přehrávání vypnutí přehrávání
	if (Play)
	{
		OnStop();
		return;
	}

// kontrola povolení operace
	if (Record || (Data->Size <= Data->SizeHead))
	{
		UpdateMenu();
		return;
	}

// příprava popisovače dat
	WaveHdr[0].dwFlags = WHDR_DONE;
	WaveHdr[1].dwFlags = WHDR_DONE;

// příprava popisovače formátu
	WAVEFORMATEX	wf0;
	WAVEFORMATEX* wf = &wf0;
	
	if (Data->Format == WAVE_FORMAT_PCM)
	{
		MaxBufSize = 0x400000;							// pro PCM je veliký buffer (nedekomprimuje se - zvolíme 4 MB)
		wf->wFormatTag = (WORD)Data->Format;			// formát dat
		wf->nChannels = (WORD)Data->Channels;			// počet kanálů
		wf->nSamplesPerSec = Data->Samples;				// vzorkovací kmitočet
		wf->nAvgBytesPerSec = BytesPerSec;				// přenosová rychlost dat
		wf->nBlockAlign = (WORD)(Data->Channels*(Data->Bits/8));	// zarovnávání dat
		wf->wBitsPerSample = (WORD)Data->Bits;			// počet bitů na vzorek
		wf->cbSize = 0;									// doplňková data ve struktuře
	}
	else
	{
		wf = (WAVEFORMATEX*)Data->Data;
		MaxBufSize = 0x100000;							// pro ostatní formáty omezíme na 1 MB
		if ((Data->Format >= 0x50) && (Data->Format < 0x60))
		{
			MaxBufSize = 0x20000;						// pro MPEG formáty omezíme na 128 KB - velká dekomprese
		}
	}

// otevření přehrávacího zařízení
	MMRESULT res = ::waveOutOpen(
		&WaveOut,									// handle zařízení
		WAVE_MAPPER,								// vybrat nejvhodnější zařízení
		wf,											// popisovač formátu
		NULL,										// není CALLBACK funkce
		NULL,										// data CALLBACK
		CALLBACK_NULL);								// není CALLBACK

	if (res != MMSYSERR_NOERROR)
	{
		UpdateMenu();
		return;
	}

// příprava ukazatele dat
	DataData = (char*)Data->Data + Data->SizeHead;		// adresa dat
	DataSize = Data->Size - Data->SizeHead;				// velikost dat
	if (DataSize > MaxBufSize) Loop = false;			// pro více bufferů není zajištěno opakování

// spuštění časovače
	Timer = ::SetTimer(MainFrame, SoundTimerID, TimerConst, NULL);

// zapnutí příznaku přehrávání
	Play = TRUE;

// aktualizace voleb
	UpdateMenu();

// zahájení přehrávání
	OnTimer(SoundTimerID);
}


/////////////////////////////////////////////////////////////////////////////
// obsluha časovače (TRUE=obslouženo)

BOOL OnTimer(UINT timerID)
{
// obsluha zobrazení času
	if (timerID == SoundTimerID)
	{
		if (Play)
		{

// zastavení přehrávání
			if ((DataSize <= 0) && (WaveHdr[0].dwFlags & WHDR_DONE) && (WaveHdr[1].dwFlags & WHDR_DONE))
			{
				OnStop();
			}
			else
			{

// obsluha bufferů
				for (int i = 0; ((i < 2) && (DataSize > 0)); i++)
				{
					WAVEHDR* wh = &WaveHdr[i];
					if (wh->dwFlags & WHDR_DONE)
					{

// uvolnění bufferu pro zápis
						if (wh->dwFlags & WHDR_PREPARED)
						{
							::waveOutUnprepareHeader(WaveOut, wh, sizeof(WAVEHDR));
						}

// příprava popisovače dat
						wh->lpData = DataData;	// adresa dat
						int size = DataSize;								// velikost dat
						if (size > MaxBufSize) size = MaxBufSize;			// omezení velikosti bufferu
						DataSize -= size;									// snížení čítače velikosti dat
						DataData += size;									// posun ukazatele dat
						wh->dwBufferLength = size;						// velikost dat
						wh->dwBytesRecorded = size;						// velikost dat nahraných v bufferu
						wh->dwUser = 0;										// uživatelská data
						wh->dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP; // parametry
						wh->dwLoops = (Loop) ? 200000000 : 1;		// počet opakování
						wh->lpNext = NULL;							// není další buffer
						wh->reserved = 0;

// příprava bufferu k přehrávání
						MMRESULT res = ::waveOutPrepareHeader(
						WaveOut,									// handle zařízení
						wh,											// popisovač dat
						sizeof(WAVEHDR));							// velikost popisovače dat

						if (res != MMSYSERR_NOERROR)
						{
							OnStop();
							return TRUE;
						}

// předání bufferu přehrávacímu zařízení
						res = ::waveOutWrite(
						WaveOut,									// handle zařízení
						wh,											// popisovač dat
						sizeof(WAVEHDR));							// velikost popisovače dat

						if (res != MMSYSERR_NOERROR)
						{
							OnStop();
							return TRUE;
						}
					}
				}
			}
		}

		if (Play || Record)
		{

// zastavení nahrávání
			if (Record && (WaveHdr[0].dwFlags & WHDR_DONE))
			{
				OnStop();
			}

// připrava bufferu k načtení pozice
			MMTIME tm;
			tm.wType = TIME_SAMPLES;
			tm.u.sample = 0;

// načtení aktuální pozice
			if (Play)
			{
				::waveOutGetPosition(WaveOut, &tm, sizeof(MMTIME));
			}
			else
			{
				::waveInGetPosition(WaveIn, &tm, sizeof(MMTIME));
			}

// zobrazení aktuální pozice, zastavení operace
			DispSet(tm.u.sample / Data->Samples );
		}
		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// start nahrávání

void OnRecord()
{
// v módu nahrávání vypnuti nahrávání
	if (Record)
	{
		OnStop();
		return;
	}

// kontrola povolení operace
	if (Play)
	{
		UpdateMenu();
		return;
	}

// příprava popisovače dat
	WaveHdr[0].dwFlags = WHDR_DONE;
	WaveHdr[1].dwFlags = WHDR_DONE;

// příprava popisovače formátu
	WAVEFORMATEX	wf;
	wf.wFormatTag = WAVE_FORMAT_PCM;				// formát dat
	wf.nChannels = SOUNDDEFCHANNELS;				// počet kanálů
	wf.nSamplesPerSec = SOUNDDEFSAMPLES;			// vzorkovací kmitočet
	wf.nAvgBytesPerSec = SOUNDDEFRATE;				// přenosová rychlost dat
	wf.nBlockAlign = SOUNDDEFALIGN;					// zarovnávání dat
	wf.wBitsPerSample = SOUNDDEFBITS;				// počet bitů na vzorek
	wf.cbSize = 0;									// doplňková data ve struktuře

// otevření nahrávacího zařízení
	MMRESULT res = ::waveInOpen(
		&WaveIn,									// handle zařízení
		WAVE_MAPPER,								// vybrat nejvhodnější zařízení
		&wf,										// popisovač formátu
		NULL,										// není CALLBACK funkce
		NULL,										// data CALLBACK
		CALLBACK_NULL);								// není CALLBACK

	if (res != MMSYSERR_NOERROR)
	{
		UpdateMenu();
		return;
	}

// úschova pro UNDO
	Undo.AddSndSet(Index, Sound[Index]);
	Undo.Stop();

// příprava bufferu
	Sound[Index].New(MAXLENGTH*SOUNDDEFRATE);		// vytvoření nového bufferu
	Sound[Index].Samples(SOUNDDEFSAMPLES);
	Sound[Index].Format(WAVE_FORMAT_PCM);
	Sound[Index].Channels(SOUNDDEFCHANNELS);
	Sound[Index].Bits(SOUNDDEFBITS);
	SetModi();										// příznak modifikace programu
	Data = Sound[Index].Data();						// adresa dat
	BytesPerSec = SOUNDDEFRATE;						// počet bajtů za sekundu

// příprava popisovače dat
	WaveHdr[0].lpData = (char*)Data->Data;				// adresa dat
	WaveHdr[0].dwBufferLength = MAXLENGTH*SOUNDDEFRATE;// velikost dat
	WaveHdr[0].dwBytesRecorded = 0;					// velikost dat nahraných v bufferu
	WaveHdr[0].dwUser = 0;								// uživatelská data
	WaveHdr[0].dwFlags = 0;							// parametry
	WaveHdr[0].dwLoops = 1;							// počet opakování
	WaveHdr[0].lpNext = NULL;							// není další buffer
	WaveHdr[0].reserved = 0;

// příprava bufferu k nahrávání
	res = ::waveInPrepareHeader(
		WaveIn,										// handle zařízení
		&WaveHdr[0],									// popisovač dat
		sizeof(WAVEHDR));							// velikost popisovače dat

	if (res != MMSYSERR_NOERROR)
	{
		::waveInClose(WaveIn);						// uzavření zařizení
		WaveIn = NULL;								// zrušení handle zařízení

		UpdateMenu();
		return;
	}

// předání bufferu nahrávacímu zařízení
	res = ::waveInAddBuffer(
		WaveIn,										// handle zařízení
		&WaveHdr[0],									// popisovač dat
		sizeof(WAVEHDR));							// velikost popisovače dat

	if (res != MMSYSERR_NOERROR)
	{
		::waveInUnprepareHeader(
			WaveIn,									// handle zařízení
			&WaveHdr[0],								// popisovač dat
			sizeof(WAVEHDR));						// velikost popisovače dat

		::waveInClose(WaveIn);						// uzavření zařizení
		WaveIn = NULL;								// zrušení handle zařízení

		UpdateMenu();
		return;
	}

// spuštění časovače
	Timer = ::SetTimer(MainFrame, SoundTimerID, TimerConst, NULL);

// zahájení nahrávání
	res = ::waveInStart(WaveIn);

// zapnutí příznaku nahrávání
	Record = TRUE;

// aktualizace voleb
	UpdateMenu();
}


/////////////////////////////////////////////////////////////////////////////
// zastavení nahrávání i přehrávání (může být voláno i zvenku při neaktivním editoru)

void OnStop()
{
// zastavení časovače (musí být před zobrazením délky)
	if (Timer)
	{
		::KillTimer(MainFrame, Timer);
		Timer = 0;
	}

// vypnutí přehrávání
	if (Play)
	{

// příznak ukončení přehrávání
		Play = FALSE;

// přerušení probíhajícího nahrávání
		VERIFY(::waveOutReset(WaveOut) == MMSYSERR_NOERROR);

// uvolnění bufferu
		if (WaveHdr[1].dwFlags & WHDR_PREPARED)
		{
			VERIFY(::waveOutUnprepareHeader(
				WaveOut,								// handle zařízení
				&WaveHdr[1],							// popisovač dat
				sizeof(WAVEHDR)) == MMSYSERR_NOERROR);	// velikost popisovače dat
		}

		if (WaveHdr[0].dwFlags & WHDR_PREPARED)
		{
			VERIFY(::waveOutUnprepareHeader(
				WaveOut,								// handle zařízení
				&WaveHdr[0],							// popisovač dat
				sizeof(WAVEHDR)) == MMSYSERR_NOERROR);	// velikost popisovače dat
		}

// uzavření přehrávacího zařízení
		VERIFY(::waveOutClose(WaveOut) == MMSYSERR_NOERROR);	// uzavření zařizení
		WaveOut = NULL;								// zrušení handle zařízení

// zobrazení délky zvuku
		DispLength();
	}

// vypnutí nahrávání
	if (Record)
	{

// příznak ukončení nahrávání
		Record = FALSE;

// přerušení probíhajícího nahrávání
		::waveInStop(WaveIn);
		::waveInReset(WaveIn);

// uvolnění bufferu
		::waveInUnprepareHeader(
			WaveIn,									// handle zařízení
			&WaveHdr[0],							// popisovač dat
			sizeof(WAVEHDR));						// velikost popisovače dat

// úschova velikosti dat
		int num = WaveHdr[0].dwBytesRecorded;
		if (num > MAXLENGTH*SOUNDDEFRATE) num = MAXLENGTH*SOUNDDEFRATE;
		if (num < 0) num = 0;

// uzavření nahrávacího zařízení
		::waveInClose(WaveIn);						// uzavření zařizení
		WaveIn = NULL;								// zrušení handle zařízení

// nastavení velikosti bufferu
		Sound[Index].ReSize(num);					// nastavení

// zobrazení délky zvuku
		DispLength();
	}

// aktualizace voleb
	Pause = FALSE;
	UpdateMenu();
}


/////////////////////////////////////////////////////////////////////////////
// přepnutí příznaku opakování

void OnLoop()
{
// kontrola povolení operace
	if (Play || Record) return;

// přepnutí příznaku opakování
	Loop = !Loop;

// aktualizace voleb
	UpdateMenu();
}


/////////////////////////////////////////////////////////////////////////////
// pozastavení přehrávání

void OnPause()
{
// kontrola povolení operace
	if (!Play) return;

// přepnutí příznaku opakování
	Pause = !Pause;

// zastavení přehrávání
	if (Pause)
	{
		::waveOutPause(WaveOut);
	}
	else
	{
		::waveOutRestart(WaveOut);
	}

// aktualizace voleb
	UpdateMenu();
}


/////////////////////////////////////////////////////////////////////////////
// aktualizace voleb

void UpdateMenu()
{
	if (ProgMode && (FocusBuf == BufEdiID) && (EditMode == BufSndID))
	{
		CheckCommand(IDN_PLAY, Play && !Pause);
		CheckCommand(IDN_LOOP, Loop);
		CheckCommand(IDN_RECORD, Record);
		CheckCommand(IDN_PAUSE, Pause);
		EnableCommand(IDN_PLAY, !Record);
		EnableCommand(IDN_STOP, Play || Record);
		EnableCommand(IDN_LOOP, !Play && !Record);
		EnableCommand(IDN_RECORD, !Play && (VerzeOS <= 0));   // JAKJAK 070611 dle MIN rady
		EnableCommand(IDN_PAUSE, Play);
	}
}


/////////////////////////////////////////////////////////////////////////////
// kopie do bufferu

void Copy()
{
// otevření schránky
	if (!::OpenClipboard(MainFrame))
		return;

// zapnutí čekacího kurzoru
	BeginWaitCursor();

// vyprázdnění schránky
	if (!::EmptyClipboard())
	{
		EndWaitCursor();
		::CloseClipboard();
		return;
	}

// vytvoření globálního bufferu pro data
	HGLOBAL global = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, Data->Size + sizeof(WAVHEAD));
	if (global == NULL)
	{
		EndWaitCursor();
		::CloseClipboard();
		return;
	}

// uzamknutí bufferu
	BYTE* data = (BYTE*) ::GlobalLock(global);

// inicializace záhlaví souboru
	((WAVHEAD*)data)->tWavIdent[0] = 'R';
	((WAVHEAD*)data)->tWavIdent[1] = 'I';
	((WAVHEAD*)data)->tWavIdent[2] = 'F';
	((WAVHEAD*)data)->tWavIdent[3] = 'F';
	((WAVHEAD*)data)->nFileSize = Data->Size + sizeof(WAVFORMAT) + sizeof(WAVDATA);

// inicializace formátu
	WAVFORMAT* wf = &(((WAVHEAD*)data)->WavFormat);
	wf->tFormatIdent[0] = 'W';
	wf->tFormatIdent[1] = 'A';
	wf->tFormatIdent[2] = 'V';
	wf->tFormatIdent[3] = 'E';
	wf->tFormatIdent[4] = 'f';
	wf->tFormatIdent[5] = 'm';
	wf->tFormatIdent[6] = 't';
	wf->tFormatIdent[7] = ' ';
	wf->nFormatSize = 16;

	wf->wFormatTag = WAVE_FORMAT_PCM;
	wf->nChannels = (WORD)Data->Channels;
	wf->nSamplesPerSec = Data->Samples;
	wf->nAvgBytesPerSec = BytesPerSec;
	wf->nBlockAlign = (WORD)((Data->Bits/8) * Data->Channels);
	wf->wBitsPerSample = (WORD)Data->Bits;
	
// inicializace dat
	WAVDATA* wd = &(((WAVHEAD*)data)->WavData);
	wd->tDataIdent[0] = 'd';
	wd->tDataIdent[1] = 'a';
	wd->tDataIdent[2] = 't';
	wd->tDataIdent[3] = 'a';
	wd->nDataSize = Data->Size;

// přenesení dat
	MemCopy(data + sizeof(WAVHEAD), Data->Data, Data->Size);

// odemknutí bufferu
	::GlobalUnlock(global);

// uložení dat do schránky
	::SetClipboardData(CF_WAVE, global);

// uzavření schránky
	::CloseClipboard();

// vypnutí čekacího kurzoru
	EndWaitCursor();

// aktualizace voleb bloku
	UpdateClipboard();
}


/////////////////////////////////////////////////////////////////////////////
// navrácení z bufferu

void Paste()
{
// lokální proměnné
	HGLOBAL		global;			// globální buffer s daty
	BYTE*		data;			// ukazatel na data
	int			size;			// velikost dat souboru
	int			channels;		// počet kanálů
	int			samples;		// vzorkovací kmitočet
	int			bits;			// počet bitů na vzorek

// otevření schránky
	if (!::OpenClipboard(MainFrame))
		return;

// zapnutí čekacího kurzoru
	BeginWaitCursor();

// načtení dat schránky
	global = ::GetClipboardData(CF_WAVE);
	if (global == NULL)
	{
		EndWaitCursor();
		::CloseClipboard();
		return;
	}
	
// uzamknutí bufferu
	data = (BYTE*) ::GlobalLock(global);

// kontrola platnosti formátu souboru
	if ((data[0] == 'R') &&
		(data[1] == 'I') &&
		(data[2] == 'F') &&
		(data[3] == 'F'))
	{
// velikost dat
		size = ((WAVHEAD*)data)->nFileSize;
		if (size < 0) size = 0;
		data += 8;

// kontrola popisovače formátu
		if ((data[0] == 'W') &&
			(data[1] == 'A') &&
			(data[2] == 'V') &&
			(data[3] == 'E') &&
			(data[4] == 'f') &&
			(data[5] == 'm') &&
			(data[6] == 't') &&
			(data[7] == ' ') &&
			(((WAVFORMAT*)data)->wFormatTag == WAVE_FORMAT_PCM))
		{
			channels = ((WAVFORMAT*)data)->nChannels;
			if (channels != 2) channels = 1;

			samples = ((WAVFORMAT*)data)->nSamplesPerSec;
			if (samples < 0) samples = SOUNDDEFSAMPLES;

			bits = ((WAVFORMAT*)data)->wBitsPerSample;
			if (bits != 16) bits = 8;

			size -= ((WAVFORMAT*)data)->nFormatSize + 12;
			data += ((WAVFORMAT*)data)->nFormatSize + 12;

// nalezení bloku dat
			for (;;)
			{
				if (size < 10) break;

				if ((data[0] == 'd') &&
					(data[1] == 'a') &&
					(data[2] == 't') &&
					(data[3] == 'a') &&
					(size < 700000000))
				{
					if (size > (int)((WAVDATA*)data)->nDataSize)
					{
						size = ((WAVDATA*)data)->nDataSize;
					}
					if (size < 0) size = 0;
					data += 8;

// kopie dat do bufferu
					Sound[Index].New(size);			// vytvoření nového bufferu
					SetModi();						// příznak modifikace programu
					Data = Sound[Index].Data();		// adresa dat
					BytesPerSec = samples * channels * (bits/8);
					Data->Samples = samples;
					Data->Format = WAVE_FORMAT_PCM;
					Data->Channels = (BYTE)channels;
					Data->Bits = (BYTE)bits;
					Data->SizeHead = 0;
					MemCopy(Data->Data, data, size);
					DispLength();
					break;
				}
				
				size -= ((WAVDATA*)data)->nDataSize + 8;
				data += ((WAVDATA*)data)->nDataSize + 8;
			}
		}
	}

// odemknutí bufferu
	::GlobalUnlock(global);

// uzavření schránky
	::CloseClipboard();

// vypnutí čekacího kurzoru
	EndWaitCursor();

// aktualizace voleb bloku
	UpdateClipboard();
}

/////////////////////////////////////////////////////////////////////////////
// aktualizace voleb bloku

void UpdateClipboard()
{
	if ((FocusBuf == BufEdiID) && (EditMode == BufSndID))
	{
		EnableCommand(IDN_CUT, FALSE);
		EnableCommand(IDN_COPY, Data->Size > 1);
		EnableCommand(IDN_PASTE, ::IsClipboardFormatAvailable(CF_WAVE));
		EnableCommand(IDN_CLEAR, FALSE);
		EnableCommand(IDN_ALL, FALSE);
	}
}


}
