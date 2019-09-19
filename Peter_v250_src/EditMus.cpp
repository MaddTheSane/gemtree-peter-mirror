
#include "Main.h"

/***************************************************************************\
*																			*
*									Editor hudby							*
*																			*
\***************************************************************************/

namespace EditMusic
{

/////////////////////////////////////////////////////////////////////////////
// parametry

// editovaná hudba
int		Index = 0;						// editovaná položka
MUSICDATA*	Data = NULL;				// data hudby
int		Delka = 0;						// délka skladby v ms

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
BOOL	Loop = FALSE;					// zapnuta smyčka
BOOL	Pause = FALSE;					// zapauzování přehrávání

// přehrávání a narávání
TCHAR		FileName[MAX_PATH + 101];	// jméno přechodného souboru
MCIDEVICEID	DeviceID = 0;				// ID zařízení
DWORD		Timer = 0;					// časovač pro zobrazení času

#define MusicTimerID 13263				// ID časovače

/////////////////////////////////////////////////////////////////////////////
// inicializace při startu programu

void StartInit()
{
// není soubor
	FileName[0] = 0;

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
	ASSERT(Music.IsValid(index));

// vypnutí případného přehrávání
	OnStop();

// kopie před editací
	Music[index].CopyWrite();

// adresa dat hudby
	Data = Music[index].Data();

// zobrazení délky hudby
	OnPlay(TRUE);
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
// zobrazení délky hudby

void DispLength()
{
	DispSet(Delka / 1000);
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
// start přehrávání (init = jen zjištění délky a přerušení)

void OnPlay(BOOL init)
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
	if (Data->Size <= 1)
	{
		UpdateMenu();
		return;
	}

// příprava jména přechodného souboru
	TCHAR tmppath[101];
	tmppath[0] = 0;
	::GetTempPath(100, tmppath);
	FileName[0] = 0;
	::GetTempFileName(tmppath, _T("PET"), 0, FileName);

// vytvoření přechodného souboru
	HANDLE file = ::CreateFile(FileName, GENERIC_READ | GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);

	if (file == INVALID_HANDLE_VALUE)
	{
		FileName[0] = 0;
		UpdateMenu();
		return;
	}

// zápis dat do přechodného souboru
	DWORD writen;
	BOOL res = ::WriteFile(file, Data->Data, Data->Size, &writen, NULL);

// uzavření souboru
	::CloseHandle(file);

// kontrola operace zápisu	
	if (!res)
	{
		::DeleteFile(FileName);
		FileName[0] = 0;
		UpdateMenu();
		return;
	}

// otevření výstupního zařízení
    MCI_OPEN_PARMS mop;

    mop.lpstrDeviceType = _T("sequencer");
    mop.lpstrElementName = FileName;

	res = ::mciSendCommand(NULL, MCI_OPEN,
        MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD) &mop);
	
	if (res)
	{
		::DeleteFile(FileName);
		FileName[0] = 0;
		UpdateMenu();
		return;
	}
    DeviceID = mop.wDeviceID;

// nastavení časového formátu na milisekundy
	MCI_SET_PARMS msp;
	msp.dwTimeFormat = MCI_FORMAT_MILLISECONDS;

	res = ::mciSendCommand(DeviceID, MCI_SET, 
		MCI_SET_TIME_FORMAT, (DWORD) &msp);

// načtení délky skladby
	MCI_STATUS_PARMS mtp;
	mtp.dwItem = MCI_STATUS_LENGTH;

	res = ::mciSendCommand(DeviceID, MCI_STATUS, 
		MCI_STATUS_ITEM, (DWORD) &mtp);
	Delka = mtp.dwReturn;
	if (Delka < 0) Delka = 0;
	if (Delka > 1000000000) Delka = 1000000000;

// přehrání souboru
    MCI_PLAY_PARMS mpp;

	if (!init)
	{
	    mpp.dwCallback = (DWORD)MainFrame;
		mpp.dwFrom = 0;
		if ((int)mpp.dwFrom > Delka) mpp.dwFrom = Delka;
		res = ::mciSendCommand(DeviceID, MCI_PLAY, 
			MCI_NOTIFY | MCI_FROM, (DWORD) &mpp);
	}

	if (res || init)
	{
        ::mciSendCommand(DeviceID, MCI_CLOSE, 0, NULL);
		DeviceID = 0;
		::DeleteFile(FileName);
		FileName[0] = 0;
		UpdateMenu();
		return;
	}

// spuštění časovače
	Timer = ::SetTimer(MainFrame, MusicTimerID, TimerConst, NULL);

// zapnutí příznaku přehrávání
	Play = TRUE;

// aktualizace voleb
	UpdateMenu();
}


/////////////////////////////////////////////////////////////////////////////
// obsluha časovače (TRUE=obslouženo)

BOOL OnTimer(UINT timerID)
{
// obsluha zobrazení času
	if (timerID == MusicTimerID)
	{
		if (Play)
		{
			MCI_STATUS_PARMS msp;
			msp.dwItem = MCI_STATUS_POSITION;

			::mciSendCommand(DeviceID, MCI_STATUS, 
				MCI_STATUS_ITEM, (DWORD) &msp);

			DispSet(msp.dwReturn/1000);
		}
		return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// ukončení přehrávání

void OnMMNotify(DWORD flags, int devID)
{
	if (flags == MCI_NOTIFY_SUCCESSFUL)
	{
		OnStop();
		if (Loop)
		{
			OnPlay();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// zastavení přehrávání (může být voláno i zvenku při neaktivním editoru)

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

// uzavření přehrávacího zařízení
		if (DeviceID)
		{
			::mciSendCommand(DeviceID, MCI_STOP, 0, NULL);
	        ::mciSendCommand(DeviceID, MCI_CLOSE, 0, NULL);
			DeviceID = 0;
		}

// zrušení souboru
		if (FileName[0])
		{
			::DeleteFile(FileName);
			FileName[0] = 0;
		}

// zobrazení délky skladby
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
	if (Play) return;

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
		::mciSendCommand(DeviceID, MCI_PAUSE, 0, NULL);
	}
	else
	{
	    MCI_PLAY_PARMS mpp;
		mpp.dwCallback = (DWORD)MainFrame;
		::mciSendCommand(DeviceID, MCI_PLAY, MCI_NOTIFY, (DWORD) &mpp);
	}

// aktualizace voleb
	UpdateMenu();
}


/////////////////////////////////////////////////////////////////////////////
// převinutí zpět

void OnRew()
{
// kontrola, zda je přehrávání
	if (!Play) return;

// úschova příznaku pauzy
	BOOL pause = Pause;

// odpauzování, je-li zapauzováno
	if (pause) OnPause();

// příprava nové pozice
	MCI_PLAY_PARMS mpp;
	mpp.dwCallback = (DWORD)MainFrame;
	mpp.dwFrom = LastTime * 1000 - 10000;

// přetečení hranice
	while ((int)mpp.dwFrom < 0) 
	{
		mpp.dwFrom += Delka;
	}

// nastavení nové pozice přehrávání
	::mciSendCommand(DeviceID, MCI_PLAY, MCI_NOTIFY | MCI_FROM, 
    (DWORD) &mpp);

// navrácení pauzy
	if (pause) OnPause();
}

/////////////////////////////////////////////////////////////////////////////
// převinutí vpřed

void OnFrw()
{
// kontrola, zda je přehrávání
	if (!Play) return;

// úschova příznaku pauzy
	BOOL pause = Pause;

// odpauzování, je-li zapauzováno
	if (pause) OnPause();

// příprava nové pozice
	MCI_PLAY_PARMS mpp;
	mpp.dwCallback = (DWORD)MainFrame;
	mpp.dwFrom = LastTime * 1000 + 10000;

// přetečení hranice
	while ((int)mpp.dwFrom >= Delka) 
	{
		mpp.dwFrom -= Delka;
	}

// nastavení nové pozice přehrávání
	::mciSendCommand(DeviceID, MCI_PLAY, MCI_NOTIFY | MCI_FROM, 
    (DWORD) &mpp);

// navrácení pauzy
	if (pause) OnPause();
}


/////////////////////////////////////////////////////////////////////////////
// aktualizace voleb

void UpdateMenu()
{
	if (ProgMode && (FocusBuf == BufEdiID) && (EditMode == BufMusID))
	{
		CheckCommand(IDN_PLAY, Play && !Pause);
		CheckCommand(IDN_LOOP, Loop);
		CheckCommand(IDN_PAUSE, Pause);
		EnableCommand(IDN_PLAY, TRUE);
		EnableCommand(IDN_STOP, Play);
		EnableCommand(IDN_LOOP, !Play);
		EnableCommand(IDN_PAUSE, Play);
		EnableCommand(IDN_REW, Play);
		EnableCommand(IDN_FRW, Play);
	}
}


/////////////////////////////////////////////////////////////////////////////
// kopie do bufferu

void Copy()
{
	/*
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
	HGLOBAL global = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, Data->Size);
	if (global == NULL)
	{
		EndWaitCursor();
		::CloseClipboard();
		return;
	}

// uzamknutí bufferu
	BYTE* data = (BYTE*) ::GlobalLock(global);

// přenesení dat
	MemCopy(data, Data->Data, Data->Size);

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
	*/
}


/////////////////////////////////////////////////////////////////////////////
// navrácení z bufferu

void Paste()
{
	/*
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
			if (samples < 0) samples = 22050;

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
					Data->Channels = channels;
					Data->Bits = bits;
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
	*/
}

/////////////////////////////////////////////////////////////////////////////
// aktualizace voleb bloku

void UpdateClipboard()
{
	if ((FocusBuf == BufEdiID) && (EditMode == BufMusID))
	{
		EnableCommand(IDN_CUT, FALSE);
		EnableCommand(IDN_COPY, FALSE); //Data->Size > 1);
		EnableCommand(IDN_PASTE, FALSE); //::IsClipboardFormatAvailable(CF_WAVE));
		EnableCommand(IDN_CLEAR, FALSE);
		EnableCommand(IDN_ALL, FALSE);
	}
}


}
