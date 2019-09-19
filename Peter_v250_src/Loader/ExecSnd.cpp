
#include "Main.h"

/***************************************************************************\
*																			*
*						Provádění programu - zvuk							*
*																			*
\***************************************************************************/

#pragma optimize("t", on)			// optimalizace na maximální rychlost

// obsluha přehrávání zvuku
HWAVEOUT	WaveOut = NULL;					// handle výstupního zařízení (NULL=není otevřeno)
WAVEHDR		WaveHeader[SOUNDBUFFERU];		// popisovače dat
BYTE*		SoundBuf[SOUNDBUFFERU];			// zvukové buffery
int			SoundKanalu = 0;				// počet aktivních zvukových kanálů
SOUNDCHAN*	SoundChan;						// buffer zvukových kanálů
int			SoundError = -1;				// povolení opakovaného otevření zvuku (při < 0)
bool		WaveFormatPCM = true;			// je přehráván formát PCM (vlastní obsluha)
char*		WaveData2 = NULL;				// ukazatel dat pro nestandardní formát
int			WaveSize2 = 0;					// čítač dat pro nestandardní formát
int			WaveLoop = 1;					// čítač opakování pro nestandardní formát
CSound		Sound2;							// přehrávaný nestandardní zvuk, naposledy zadaný zvuk
double		WaveBeg2 = 0;					// posunutý offset pro nestandardní formát (sekund)
bool		SoundPausing = false;			// příznak pauzy zvuku
int			SoundBlokSize = 1024;			// zarovnávání velikosti bloku nestandardního formátu
int			BytesPerSec = 22050;			// přenosová rychlost pro nestandardní formát
int			SoundBufSize = SOUNDBUFSIZE;	// velikost bufferu nestandardního formátu
WORD		SoundID = 0;					// rozlišovací kód zvuku

// obsluha DirectSound
#define		DSSOUNDOUTRATE 22050			// výstupní vzorkovací kmitočet pro DirectSound

// ----- při testování bez použití DirectSound nastavit InitDSound na TRUE ----

BOOL		InitDSound = FALSE;				// probéhla inicializace DirectSound
BOOL		DSoundOK = FALSE;				// DirectSound úspěšné inicializováno

#define VOLUMESTEPS 1001					// počet kroků tabulky hlasitosti
short*		VolumeTab;						// tabulka hlasitosti (konverze na decibely)

DIRECTSOUNDCREATE	pDirectSoundCreate = NULL;	// funkce DirectSoundCreate (NULL=není)
HINSTANCE	DSoundDLL = NULL;				// knihovna DSOUND.DLL
IDirectSound* DirectSound = NULL;			// DirectSound interface
int			DSoundFlags = DSBCAPS_STICKYFOCUS |
							DSBCAPS_GLOBALFOCUS |
							DSBCAPS_GETCURRENTPOSITION2 |
							DSBCAPS_CTRLFREQUENCY |
							DSBCAPS_CTRLPAN |
							DSBCAPS_CTRLVOLUME;

/***************************************************************************\
*																			*
*								obsluha zvuku								*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// deinicializace DirectSound

void TermDirectSound()
{

// uvolnění rozhraní DirectSound
	if (DirectSound != NULL)
	{
		DirectSound->Release();
		DirectSound = NULL;
	}

// uvolnění knihovny DSOUND.DLL
	if (DSoundDLL != NULL)
	{
		::FreeLibrary(DSoundDLL);
		DSoundDLL = NULL;
	}

// zrušení tabulky hlasitosti
	if (VolumeTab != NULL)
	{
		MemFree(VolumeTab);
		VolumeTab = NULL;
	}

// příznak neplatnosti DirectSound
	DSoundOK = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// inicializace DirectSound (vrací TRUE = DirectSound je OK)

BOOL InitDirectSound()
{
// test, zda již probíhala inicializce
	if (!InitDSound)
	{
		InitDSound = TRUE;

// načtení knihovny DSOUND.DLL
		DSoundDLL = ::LoadLibrary("DSOUND.DLL");
		if (DSoundDLL != NULL)
		{

// importování funkce DirectSoundCreate
			pDirectSoundCreate = (DIRECTSOUNDCREATE)::GetProcAddress(DSoundDLL, "DirectSoundCreate");
			if (pDirectSoundCreate != NULL)
			{

// vytvoření rozhraní DirectSound
				if ((pDirectSoundCreate(NULL, &DirectSound, NULL) == DS_OK) && (DirectSound != NULL))
				{

// nastavení módu spolupráce DirectSound
					if (DirectSound->SetCooperativeLevel(MainFrame, DSSCL_PRIORITY) == DS_OK)
					{

// popisovač primárního bufferu
						DSBUFFERDESC dsbd;
						MemFill(&dsbd, sizeof(DSBUFFERDESC), 0);
						dsbd.dwSize = sizeof(DSBUFFERDESC);
						dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_STICKYFOCUS;

// vytvoření primárního bufferu
						IDirectSoundBuffer* prim = NULL;

						HRESULT res = DirectSound->CreateSoundBuffer(&dsbd, &prim, NULL);

						if ((res != DS_OK) || (prim == NULL))
						{
							dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
							res = DirectSound->CreateSoundBuffer(&dsbd, &prim, NULL);
						}

						if ((res == DS_OK) && (prim != NULL))
						{

// formát primárního bufferu
							WAVEFORMATEX wfx;
							wfx.wFormatTag = WAVE_FORMAT_PCM;
							wfx.nChannels = 2;
							wfx.nSamplesPerSec = DSSOUNDOUTRATE;
							wfx.nAvgBytesPerSec = 4 * DSSOUNDOUTRATE;
							wfx.nBlockAlign = 4;
							wfx.wBitsPerSample = 16;
							wfx.cbSize = 0;

// nastavení formátu primárního bufferu
							res = prim->SetFormat(&wfx);
							prim->Release();

							if (res == DS_OK)
							{

// pokus o vytvoření sekundárního bufferu
								wfx.wFormatTag = WAVE_FORMAT_PCM;
								wfx.nChannels = 2;
								wfx.nSamplesPerSec = DSSOUNDOUTRATE;
								wfx.nBlockAlign = 4;
								wfx.nAvgBytesPerSec = DSSOUNDOUTRATE*4;
								wfx.wBitsPerSample = 16;
								wfx.cbSize = 0;

								DSBUFFERDESC dsbd;
								dsbd.dwSize = sizeof(DSBUFFERDESC);
								// je to streaming buffer aby zůstal v paměti kvůli možnosti duplikování
								dsbd.dwFlags = DSoundFlags;
								dsbd.dwBufferBytes = 65000;
								dsbd.dwReserved = 0;
								dsbd.lpwfxFormat = &wfx;

								IDirectSoundBuffer* buf = NULL;
								if (DirectSound->CreateSoundBuffer(&dsbd, &(buf), NULL) != DS_OK) buf = NULL;

								if (buf == NULL)
								{
									DSoundFlags = DSBCAPS_GETCURRENTPOSITION2 |
													DSBCAPS_CTRLFREQUENCY |
													DSBCAPS_CTRLPAN |
													DSBCAPS_CTRLVOLUME;
									dsbd.dwFlags = DSoundFlags;
									if (DirectSound->CreateSoundBuffer(&dsbd, &(buf), NULL) != DS_OK) buf = NULL;

									if (buf == NULL)
									{
										DSoundFlags = DSBCAPS_CTRLFREQUENCY |
														DSBCAPS_CTRLVOLUME;
										dsbd.dwFlags = DSoundFlags;
										if (DirectSound->CreateSoundBuffer(&dsbd, &(buf), NULL) != DS_OK) buf = NULL;
									}
								}

								if (buf != NULL)
								{
									void* dst = NULL;
									DWORD n = 0;
									res = buf->Lock(0, 65000, &dst, &n, NULL, NULL, 0);
									if (res == DS_OK) buf->Unlock(dst, n, NULL, 0);

									buf->Release();

// vygenerování tabulky hlasitosti
									if (res == DS_OK)
									{
										VolumeTab = (short*)MemGet(VOLUMESTEPS * sizeof(short));

										VolumeTab[0] = -10000;

										for (int i = 1; i < VOLUMESTEPS; i++)
										{
											VolumeTab[i] = (short)Round(2000 * log10((double)i/(VOLUMESTEPS-1)));
										}

										DSoundOK = TRUE;
										return TRUE;
									}
								}
							}
						}
					}
				}				
			}

// při chybě uvolnění DirectSound
			TermDirectSound();
		}
	}

	return DSoundOK;
}


/////////////////////////////////////////////////////////////////////////////
// otevření zvukového kanálu (balance: -1=vlevo, 0=střed, +1=vpravo)

void PlaySoundOpen(CSound sound, int loop, bool stop, double volume, double balance, double speed)
{
// inicializace DirectSound
	InitDirectSound();

// kontrola velikosti dat
	if (sound.Size() <= 0) return;

// korekce počtu opakování
	if ((loop < 1) || (loop > 100000)) loop = BigInt;

// zastavení přehrávání, bude-li nebo byl-li jiný formát než PCM
	if (!WaveFormatPCM || (sound.Format() != WAVE_FORMAT_PCM))
	{
		PlaySoundStop();
	}

// obsluha nestandardního formátu
	if (sound.Format() != WAVE_FORMAT_PCM)
	{

// povolení otevření zvuku
		if (SoundError > 0) return;

// příznak přehrávání zvuku
		WaveFormatPCM = false;
		Sound2 = sound;

// příprava zarovnávání dat a velikosti bufferu
		BytesPerSec = sound.Samples() * sound.Channels() / 2;				// bajtů za sekundu pro ADPCM
		SoundBlokSize = 1024;											// velikost bloku pro ADPCM (asi tak nějak)

		if ((sound.Format() >= 0x50) && (sound.Format() < 0x60))				// je MPEG ?
		{
			WAVEFORMATEX* wfx = (WAVEFORMATEX*)(sound.DataData());
			BytesPerSec = wfx->nAvgBytesPerSec;									// bajtů za sekundu pro MPEG
			SoundBlokSize = ((MPEGLAYER3WAVEFORMAT*)(sound.DataData()))->nBlockSize;	// velikost bloku pro MPEG
		}
		if (BytesPerSec <= 2) BytesPerSec = 2;
		if (SoundBlokSize < 2) SoundBlokSize = 2;
		SoundBufSize = (SOUNDBUFSIZE + SoundBlokSize-1) / SoundBlokSize * SoundBlokSize;

// otevření přehrávacího zařízení
		MMRESULT res = ::waveOutOpen(
			&WaveOut,								// handle zařízení
			WAVE_MAPPER,							// vybrat nejvhodnější zařízení
			(WAVEFORMATEX*)(sound.DataData()),		// popisovač formátu
			NULL,									// není funkce CALLBACK
			NULL,									// data CALLBACK
			CALLBACK_NULL);							// není CALLBACK

		if (res != MMSYSERR_NOERROR)
		{
			WaveOut = NULL;
			SoundError = SOUNDERRORWAIT;
			return;
		}

// příprava ukazatelů dat
		WaveBeg2 = 0;
		WaveLoop = loop;
		WaveData2 = (char*)sound.DataData() + sound.SizeHead();	// adresa dat
		WaveSize2 = sound.Size() - sound.SizeHead();
		if (WaveSize2 < 0)
		{
			WaveSize2 = 0;
			WaveLoop = 0;
		}
		return;
	}

// bude formát PCM
	WaveFormatPCM = true;
	Sound2 = sound;

// odpauzování
	SoundPause(false);

// příprava balance
	if (balance < -1) balance = -1;
	if (balance > 1) balance = 1;
	
	double left = 1;
	double right = 1;
	if (balance > 0)
	{
		left = 1 - balance;
	}
	else
	{
		right = 1 + balance;
	}

// příprava hlasitosti
	volume = fabs(volume);
	if (volume > 1) volume = 1;
	left *= volume;
	right *= volume;

// rychlost přehrávání v bajtech na vzorek
	speed = fabs(speed);
	double speedB = speed * sound.DataRate() / SOUNDOUTRATE;
	if (speedB >= sound.Size()) speedB = sound.Size();

// parametry pro DirectSound
	int dsfreq = Round(speed * DSSOUNDOUTRATE);
	if (dsfreq < DSBFREQUENCY_MIN) dsfreq = DSBFREQUENCY_MIN;
	if (dsfreq > DSBFREQUENCY_MAX) dsfreq = DSBFREQUENCY_MAX;

	short dsvolume = 0;
	short dsbalance = 0;

	if (DSoundOK)
	{
		dsvolume = VolumeTab[Round(volume * (VOLUMESTEPS-1))];

		if (balance > 0)
		{
			dsbalance = (short)-VolumeTab[Round((1-balance) * (VOLUMESTEPS-1))];
		}
		else
		{
			dsbalance = VolumeTab[Round((1+balance) * (VOLUMESTEPS-1))];
		}
	}

// příprava proměnných k prohledání tabulky kanálů
	int i = MAXSOUNDKANALU;			// čítač testovaných kanálů
	int cit = SoundKanalu;			// čítač zbylých aktivních kanálů
	int min = BigInt;				// velikost dat volného kanálu
	int index = 0;					// index nalezeného volného kanálu
	SOUNDCHAN* sndchan = SoundChan;	// ukazatel kanálů

// nalezení zvuku, zda je již přehráván, nalezení nejstaršího zvuku
	for (; (i > 0) && (cit > 0); i--)
	{
		if (sndchan->Num > 0)
		{
			cit--;

			if ((sndchan->Sound == sound) && (sndchan->SoundID == SoundID))
			{

				IDirectSoundBuffer* buf = sndchan->DSBuffer;

// při zastavení přehrávání zvuku inicializace od začátku zvuku
				if (stop)
				{
					sndchan->Offset = 0;
					if (buf != NULL) buf->SetCurrentPosition(0);
				}

// nastavení parametrů pro DirectSound
				if (buf != NULL)
				{
					if (dsfreq != sndchan->DSFreq)
					{
						buf->SetFrequency(dsfreq);
					}

					if (dsvolume != sndchan->DSVolume)
					{
						buf->SetVolume(dsvolume);
					}

					if (dsbalance != sndchan->DSBalance)
					{
						buf->SetPan(dsbalance);
					}

// zahájení přehrávání, pokud neprobíhá
					DWORD stat = 0;
					buf->GetStatus(&stat);

					if ((stat & DSBSTATUS_PLAYING) == 0)
					{
						buf->Play(0, 0, (loop == BigInt) ? DSBPLAY_LOOPING : 0);
					}
				}

// nastavení nových parametrů zvuku
				sndchan->Left = left;
				sndchan->Right = right;
				sndchan->SpeedB = speedB;
				sndchan->Loop = loop;
				sndchan->DSFreq = dsfreq;
				sndchan->DSVolume = dsvolume;
				sndchan->DSBalance = dsbalance;

				return;
			}
		}

// nalezení volného kanálu (s nejmenšími zbylými daty)
		if (sndchan->Num == 0)
		{
			if (min > 0)
			{
				index = MAXSOUNDKANALU-i;
				min = 0;
			}
		}
		else
		{
			int nn = sndchan->Num - Round(sndchan->Offset);

			if (nn < min)
			{
				index = MAXSOUNDKANALU-i;
				min = nn;
			}
		}

// zvýšení ukazatele v bufferu
		sndchan++;
	}

// korekce volného kanálu, je-li k dispozici nějaký nepoužitý kanál
	if ((i > 0) && (SoundChan[index].Num > 0)) index = MAXSOUNDKANALU-i;

// zvýšení čítače kanálů, pokud byl kanál volný
	sndchan = SoundChan + index;
	if (sndchan->Num == 0) SoundKanalu++;

// zrušení bufferu DirectSound
	if (sndchan->DSBuffer != NULL)
	{
		sndchan->DSBuffer->Release();
		sndchan->DSBuffer = NULL;
	}

// nastavení ukazatelů pro nový kanál
	sndchan->Num = sound.Size();
	sndchan->Data = sound.DataData();
	sndchan->Left = left;
	sndchan->Right = right;
	sndchan->Offset = 0;
	sndchan->SpeedB = speedB;
	sndchan->Loop = loop;
	sndchan->Sound = sound;
	sndchan->DSFreq = dsfreq;
	sndchan->DSVolume = dsvolume;
	sndchan->DSBalance = dsbalance;
	sndchan->SoundID = SoundID;

// výstup pomocí DirectSound
	if (DSoundOK)
	{

// zrušení vzoru bufferu je-li ztracen
		SOUNDDATA* sd = sound.Data();
		if (sd->DSBuffer != NULL)
		{
			DWORD stat = 0;
			sd->DSBuffer->GetStatus(&stat);

			if ((stat & DSBSTATUS_BUFFERLOST) != 0)
			{
				sd->DSBuffer->Release();
				sd->DSBuffer = NULL;
			}
		}

// test, zda existuje vzor zvuku
		if (sd->DSBuffer == NULL)
		{

// příprava standardního formátu zvuku
			CSound snd = sound;
			snd.DeComp();
			snd.Conv16Bit();
			snd.ConvStereo();
			snd.ConvRate(DSSOUNDOUTRATE);

// vytvoření bufferu DirectSound
			WAVEFORMATEX wfx;
			wfx.wFormatTag = WAVE_FORMAT_PCM;
			wfx.nChannels = 2;
			wfx.nSamplesPerSec = DSSOUNDOUTRATE;
			wfx.nBlockAlign = 4;
			wfx.nAvgBytesPerSec = DSSOUNDOUTRATE*4;
			wfx.wBitsPerSample = 16;
			wfx.cbSize = 0;

			DSBUFFERDESC dsbd;
			dsbd.dwSize = sizeof(DSBUFFERDESC);
			// je to streaming buffer aby zůstal v paměti kvůli možnosti duplikování
			dsbd.dwFlags = DSoundFlags;
			dsbd.dwBufferBytes = snd.Size();
			dsbd.dwReserved = 0;
			dsbd.lpwfxFormat = &wfx;

			IDirectSoundBuffer* buf = NULL;
			if (DirectSound->CreateSoundBuffer(&dsbd, &(buf), NULL) != DS_OK) buf = NULL;

// přenesení dat do bufferu
			if (buf != NULL)
			{
				void* dst = NULL;
				DWORD n = 0;

				if (buf->Lock(0, snd.Size(), &dst, &n, NULL, NULL, 0) == DS_OK)
				{
					if ((int)n < 0) n = 0;
					if ((int)n > snd.Size()) n = snd.Size();

					MemCopy(dst, snd.DataData(), n);

					buf->Unlock(dst, n, NULL, 0);
				}
				else
				{
					buf->Release();
					buf = NULL;
				}
			}

			sd->DSBuffer = buf;
		}

// vytvoření kopie bufferu (při neúspěchu se použije originál, např. je-li HW buffer)
		if (sd->DSBuffer != NULL)
		{
			if (DirectSound->DuplicateSoundBuffer(sd->DSBuffer, &(sndchan->DSBuffer)) != DS_OK)
			{
				sndchan->DSBuffer = sd->DSBuffer;
				sd->DSBuffer = NULL;
			}

// nastavení parametrů zvuku
			IDirectSoundBuffer* buf = sndchan->DSBuffer;

			buf->SetFrequency(dsfreq);
			buf->SetVolume(dsvolume);
			buf->SetPan(dsbalance);

// zahájení přehrávání zvuku
			buf->SetCurrentPosition(0);
			buf->Play(0, 0, (loop == BigInt) ? DSBPLAY_LOOPING : 0);
			return;
		}
	}

// otevření výstupního zařízení
	if (WaveOut == NULL)
	{

// povolení otevření zvuku
		if (SoundError > 0) return;

// vynulování bufferů
		for (i = SOUNDBUFFERU-1; i >= 0; i--)
		{
			WaveHeader[i].dwFlags = WHDR_DONE;
		}

// příprava struktury popisovače formátu
		WAVEFORMATEX wf;							// popisovač formátu
		wf.wFormatTag = WAVE_FORMAT_PCM;			// formát dat
		wf.nChannels = 2;							// počet kanálů
		wf.nSamplesPerSec = SOUNDOUTRATE;			// vzorkovací kmitočet
		wf.nAvgBytesPerSec = SOUNDOUTRATE*4;		// přenosová rychlost dat
		wf.nBlockAlign = 4;							// zarovnávání dat
		wf.wBitsPerSample = 16;						// počet bitů na vzorek
		wf.cbSize = 0;								// doplňková data ve struktuře

// otevření přehrávacího zařízení
		MMRESULT res = ::waveOutOpen(
			&WaveOut,								// handle zařízení
			WAVE_MAPPER,							// vybrat nejvhodnější zařízení
			&wf,									// popisovač formátu
			NULL,									// není funkce CALLBACK
			NULL,									// data CALLBACK
			CALLBACK_NULL);							// není CALLBACK

		if (res != MMSYSERR_NOERROR)
		{
			sndchan->Num = 0;
			WaveOut = NULL;
			SoundKanalu = 0;
			SoundError = SOUNDERRORWAIT;
			return;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// uzavření zvukového kanálu

void PlaySoundClose(CSound sound)
{
// nestandardní zvuk
	if (!WaveFormatPCM)
	{
		if (Sound2 == sound)
		{
			PlaySoundStop();
		}
		return;
	}

// příprava proměnných k prohledání tabulky kanálů
	int i = MAXSOUNDKANALU;			// čítač testovaných kanálů
	int cit = SoundKanalu;			// čítač zbylých aktivních kanálů
	SOUNDCHAN* sndchan = SoundChan;	// ukazatel kanálů

// nalezení zvuku
	for (; (i > 0) && (cit > 0); i--)
	{
		if (sndchan->Num > 0)
		{
			cit--;

			if ((sndchan->Sound == sound) && (sndchan->SoundID == SoundID))
			{

// zrušení zvuku v kanálu
				sndchan->Num = 0;
				sndchan->Sound.Empty();
				SoundKanalu--;

				IDirectSoundBuffer* buf = sndchan->DSBuffer;
				if (buf != NULL)
				{
					buf->Stop();
					buf->Release();
					sndchan->DSBuffer = NULL;
				}
				return;
			}
		}

// zvýšení ukazatele v bufferu
		sndchan++;
	}
}


/////////////////////////////////////////////////////////////////////////////
// nastavení pozice přehrávání zvuku (v sekundách)

void SetSoundPos(double pos)
{
// v pauze se pozice nenastavuje
	if (SoundPausing) return;

// korekce pozice
	if (pos < 0) pos = 0;

// pozice zvuku v nestandardním formátu
	if (!WaveFormatPCM)
	{

// zvuk není přehráván
		if (WaveOut == NULL) return;

// obsluhovány jen některé formáty
		if ((Sound2.Format() == WAVE_FORMAT_ADPCM) ||
			(Sound2.Format() == WAVE_FORMAT_MPEGLAYER3))
		{
			double len = (double)(Sound2.Size() - Sound2.SizeHead()) / BytesPerSec;		// délka zvuku celkem (v sekundách)
			if (pos > len) pos = len;												// omezení maximální pozice

			int n = Round(BytesPerSec * pos + SoundBlokSize/2) / SoundBlokSize * SoundBlokSize;		// nový offset zarovnaný na blok
			if (n < 0) n = 0;
			if (n > (Sound2.Size() - Sound2.SizeHead())) n = Sound2.Size() - Sound2.SizeHead();	// omezení offsetu
	
			WaveData2 = (char*)Sound2.DataData() + Sound2.SizeHead() + n;			// nová adresa dat
			WaveSize2 = Sound2.Size() - Sound2.SizeHead() - n;						// nová velikost dat
			WaveBeg2 = (double)n / BytesPerSec;										// fiktivní offset začátku skladby

			if (WaveSize2 < 0)
			{
				WaveSize2 = 0;
				WaveLoop = 0;
			}

			::waveOutReset(WaveOut);												// zastavení přehrávání

			for (int i = SOUNDBUFFERU-1; i >= 0; i--)
			{
				if (WaveHeader[i].dwFlags & WHDR_PREPARED)
				{
					waveOutUnprepareHeader(
						WaveOut,
						WaveHeader + i,
						sizeof(WAVEHDR));
				}
				WaveHeader[i].dwFlags = WHDR_DONE;
			}

			::waveOutClose(WaveOut);										// uzavření výstupního zařízení

			if (::waveOutOpen(
					&WaveOut,								// handle zařízení
					WAVE_MAPPER,							// vybrat nejvhodnější zařízení
					(WAVEFORMATEX*)(Sound2.DataData()),		// popisovač formátu
					NULL,									// není funkce CALLBACK
					NULL,									// data CALLBACK
					CALLBACK_NULL) != MMSYSERR_NOERROR)		// není CALLBACK
			{
				WaveOut = NULL;
			}
		}
		return;
	}

// pozice zvuku ve standardním formátu
	int i = MAXSOUNDKANALU;			// čítač testovaných kanálů
	int cit = SoundKanalu;			// čítač zbylých aktivních kanálů
	SOUNDCHAN* sndchan = SoundChan;	// ukazatel kanálů

	for (; (i > 0) && (cit > 0); i--)
	{
		if (sndchan->Num > 0)
		{
			cit--;
			if ((sndchan->Sound == Sound2) && (sndchan->SoundID == SoundID))
			{
				double len = (double)(Sound2.Size()) / Sound2.DataRate();
				if (pos > len) pos = len;
				int n = Round(Sound2.DataRate() * pos);
				if (n > Sound2.Size()) n = Sound2.Size();
				sndchan->Offset = (n & ~(Sound2.Align()-1));

				if (sndchan->DSBuffer != NULL)
				{
					n = Round(4 * DSSOUNDOUTRATE * pos) & ~3;
					sndchan->DSBuffer->SetCurrentPosition(n);
				}
				return;
			}
		}
		sndchan++;
	}
	return;
}


/////////////////////////////////////////////////////////////////////////////
// zjištění pozice přehrávání zvuku

double GetSoundPos()
{
// pozice zvuku v nestandardním formátu
	if (!WaveFormatPCM)
	{
		if (WaveOut == NULL) return 0;

		MMTIME tm;
		tm.wType = TIME_SAMPLES;
		tm.u.sample = 0;
		::waveOutGetPosition(WaveOut, &tm, sizeof(MMTIME));
		return ((double)tm.u.sample / Sound2.Samples() ) + WaveBeg2;
	}

// obsluha naposledy zadaného zvuku
	int i = MAXSOUNDKANALU;			// čítač testovaných kanálů
	int cit = SoundKanalu;			// čítač zbylých aktivních kanálů
	SOUNDCHAN* sndchan = SoundChan;	// ukazatel kanálů

	for (; (i > 0) && (cit > 0); i--)
	{
		if (sndchan->Num > 0)
		{
			cit--;
			if ((sndchan->Sound == Sound2) && (sndchan->SoundID == SoundID))
			{
				if (sndchan->DSBuffer != NULL)
				{
					DWORD n = 0;
					sndchan->DSBuffer->GetCurrentPosition(&n, NULL);
					return ((double)(int)n / (DSSOUNDOUTRATE * 4));
				}

				int n = Round(sndchan->Offset);
				if (n < 0) n = 0;
				if (n > Sound2.Size()) n = Sound2.Size();
				return ((double)n / Sound2.DataRate());
			}
		}
		sndchan++;
	}
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// zastavení přehrávání zvuků

void PlaySoundStop()
{
// je výstupní zařizení otevřeno?
	if (WaveOut != NULL)
	{

// přerušení probíhajícího přehrávání
		::waveOutReset(WaveOut);

// uvolnění bufferů
		for (int i = SOUNDBUFFERU-1; i >= 0; i--)
		{
			if (WaveHeader[i].dwFlags & WHDR_PREPARED)
			{
				waveOutUnprepareHeader(
					WaveOut,
					WaveHeader + i,
					sizeof(WAVEHDR));
			}
			WaveHeader[i].dwFlags = WHDR_DONE;
		}

// uzavření přehrávacího zařizení
		::waveOutClose(WaveOut);
		WaveOut = NULL;
	}

// uvolnění všech kanálů
	SOUNDCHAN* sndchan = SoundChan;	// ukazatel kanálů
	for (int i = MAXSOUNDKANALU; i > 0; i--)
	{
		sndchan->Num = 0;
		sndchan->Sound.Empty();

		if (sndchan->DSBuffer != NULL)
		{
			sndchan->DSBuffer->Stop();
			sndchan->DSBuffer->Release();
			sndchan->DSBuffer = NULL;
		}

		sndchan++;
	}
	Sound2.Empty();
	SoundKanalu = 0;
	WaveSize2 = 0;
	WaveLoop = 0;
	SoundPausing = false;
}


/////////////////////////////////////////////////////////////////////////////
// pauza přehrávání zvuku

void SoundPause(bool pause)
{
	if ((WaveOut != NULL) && (pause != SoundPausing))
	{
		SoundPausing = pause;

		if (pause)
		{
			::waveOutPause(WaveOut);
		}
		else
		{
			::waveOutRestart(WaveOut);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// zápis zvuku do bufferu (vrací počet zapsaných vzorků, tj. v násobcích 4 bajtů)

int _fastcall SoundWriteBuf(SOUNDCHAN* snd, short* dst, int num)
{
#define MAXDATA 0x7fff

// čítač uložených vzorků
	int result = 0;

// velikost bufferu
	int n = snd->Num;

// adresa bufferu
	BYTE* src = snd->Data;

// hlasitost kanálů
	double left = snd->Left;
	double right = snd->Right;

// offset dat
	double offset = snd->Offset;

// rychlost dat
	double speedB = snd->SpeedB;

// příznak typu zvuku
	int typ = 0;

#define SOUND8BIT	0
#define SOUND16BIT	1
	if (snd->Sound.Bits() == 16) typ |= SOUND16BIT;
	
#define SOUNDMONO	0
#define SOUNDSTEREO	2
	if (snd->Sound.Stereo()) typ |= SOUNDSTEREO;

// rozlišení typu zvuku
	switch (typ)
	{
	case SOUND8BIT | SOUNDMONO:
		for (; num > 0; num--)
		{
			int off = Round(offset);

			if ((DWORD)off < (DWORD)n)
			{
				int data0 = (short)(char)(src[off] - 128) * 256;

				int data = Round(data0 * left) + dst[0];
				if (data > MAXDATA) data = MAXDATA;
				if (data < -MAXDATA) data = -MAXDATA;
				dst[0] = (short)data;

				data = Round(data0 * right) + dst[1];
				if (data > MAXDATA) data = MAXDATA;
				if (data < -MAXDATA) data = -MAXDATA;
				dst[1] = (short)data;

			}
			else
			{
				break;
			}

			dst++;
			dst++;

			result++;
			offset += speedB;
		}
		break;

	case SOUND8BIT | SOUNDSTEREO:
		for (; num > 0; num--)
		{
			int leftoff = Round(offset) & ~1;
			if ((DWORD)leftoff < (DWORD)n)
			{
				int data = Round(((short)(char)(src[leftoff] - 128)
							* 256) * left) + *dst;
				if (data > MAXDATA) data = MAXDATA;
				if (data < -MAXDATA) data = -MAXDATA;
				*dst = (short)data;
			}
			dst++;

			int rightoff = leftoff | 1;
			if ((DWORD)rightoff < (DWORD)n)
			{
				int data = Round(((short)(char)(src[rightoff] - 128)
							* 256) * right) + *dst;
				if (data > MAXDATA) data = MAXDATA;
				if (data < -MAXDATA) data = -MAXDATA;
				*dst = (short)data;
			}
			else
			{
				if ((DWORD)leftoff >= (DWORD)n) break;
			}
			dst++;

			result++;
			offset += speedB;
		}
		break;

	case SOUND16BIT | SOUNDMONO:
		n--; if (n <= 0) break;

		for (; num > 0; num--)
		{
			int off = Round(offset) & ~1;
			if ((DWORD)off < (DWORD)n)
			{
				int data = Round(*(short*)(src + off) * left) + dst[0];
				if (data > MAXDATA) data = MAXDATA;
				if (data < -MAXDATA) data = -MAXDATA;
				dst[0] = (short)data;

				data = Round(*(short*)(src + off) * right) + dst[1];
				if (data > MAXDATA) data = MAXDATA;
				if (data < -MAXDATA) data = -MAXDATA;
				dst[1] = (short)data;
			}
			else
			{
				break;
			}
			dst++;
			dst++;

			result++;
			offset += speedB;
		}
		break;

	case SOUND16BIT | SOUNDSTEREO:
		n--; if (n <= 0) break;

		for (; num > 0; num--)
		{
			int leftoff = Round(offset) & ~3;
			if ((DWORD)leftoff < (DWORD)n)
			{
				int data = Round(*(short*)(src + leftoff) * left) + *dst;
				if (data > MAXDATA) data = MAXDATA;
				if (data < -MAXDATA) data = -MAXDATA;
				*dst = (short)data;
			}
			dst++;

			int rightoff = leftoff + 2;
			if ((DWORD)rightoff < (DWORD)n)
			{
				int data = Round(*(short*)(src + rightoff) * right) + *dst;
				if (data > MAXDATA) data = MAXDATA;
				if (data < -MAXDATA) data = -MAXDATA;
				*dst = (short)data;
			}
			else
			{
				if ((DWORD)leftoff >= (DWORD)n) break;
			}
			dst++;

			result++;
			offset += speedB;
		}
		break;

	default:
		ASSERTERROR;
	}

	snd->Offset = offset;
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// obsluha přehrávání zvuku na pozadí

void PlaySoundBack()
{
// čítač povolení otevření zvuku
	SoundError--;

	if (SoundPausing) return;

// obsluha nestandardního formátu
	if (!WaveFormatPCM)
	{

// test, zda probíhá zvukový výstup
		if (WaveOut != NULL)
		{

// test, zda je ukončeno přehrávání
			BOOL stop = ((WaveSize2 <= 0) && (WaveLoop <= 0));
			int i;
			for (i = SOUNDBUFFERU-1; i >= 0; i--)
			{
				if ((WaveHeader[i].dwFlags & WHDR_DONE) == 0)
				{
					stop = FALSE;
					break;
				}
			}

// ukončení přehrávání
			if (stop)
			{
				PlaySoundStop();
				return;
			}
		
// obsluha bufferů
			for (i = SOUNDBUFFERU-1; ((i >= 0) && ((WaveSize2 > 0) || (WaveLoop > 0))); i--)
			{
				WAVEHDR* wh = &WaveHeader[i];
				if (wh->dwFlags & WHDR_DONE)
				{

// uvolnění bufferu pro zápis
					if (wh->dwFlags & WHDR_PREPARED)
					{
						::waveOutUnprepareHeader(WaveOut, wh, sizeof(WAVEHDR));
					}

// příprava popisovače dat
					wh->lpData = WaveData2;						// adresa dat
					int size = WaveSize2;
					if (size > SoundBufSize) size = SoundBufSize;
					WaveSize2 -= size;
					WaveData2 += size;
					wh->dwBufferLength = size;				// velikost dat
					wh->dwBytesRecorded = size;				// velikost dat nahraných v bufferu
					wh->dwUser = 0;							// uživatelská data
					wh->dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP; // parametry
					wh->dwLoops = 1;						// počet opakování
					wh->lpNext = NULL;						// není další buffer
					wh->reserved = 0;

// příprava bufferu k přehrávání
					MMRESULT res = ::waveOutPrepareHeader(
						WaveOut,									// handle zařízení
						wh,										// popisovač dat
						sizeof(WAVEHDR));							// velikost popisovače dat

					if (res != MMSYSERR_NOERROR)
					{
						wh->dwFlags |= WHDR_DONE;			// při velikosti zbytku dat < 512 B to neprojde!!!!
					}
					else
					{

// předání bufferu přehrávacímu zařízení
						res = ::waveOutWrite(
							WaveOut,									// handle zařízení
							wh,								// popisovač dat
							sizeof(WAVEHDR));							// velikost popisovače dat

						if (res != MMSYSERR_NOERROR)
						{
							wh->dwFlags |= WHDR_DONE;
						}
					}

// obsluha opakování zvuku
					if (WaveSize2 <= 0)
					{
						WaveLoop--;
						if (WaveLoop > 0)
						{
							WaveData2 = (char*)Sound2.DataData() + Sound2.SizeHead();			// nová adresa dat
							WaveSize2 = Sound2.Size() - Sound2.SizeHead();						// nová velikost dat
							WaveBeg2 = 0;										// fiktivní offset začátku skladby
						}
					}
				}
			}
		}
		return;
	}

// obsluha přehrávání DirectSound
	if (DSoundOK)
	{
		int i = MAXSOUNDKANALU;			// čítač testovaných kanálů
		int cit = SoundKanalu;			// čítač zbylých aktivních kanálů
		SOUNDCHAN* sndchan = SoundChan;	// ukazatel kanálů

		for (; (i > 0) && (cit > 0); i--)
		{
			if (sndchan->Num > 0)
			{
				cit--;

				IDirectSoundBuffer* buf = sndchan->DSBuffer;

				if (buf != NULL)
				{
					DWORD stat = 0;

					buf->GetStatus(&stat);
					if ((stat & DSBSTATUS_PLAYING) == 0)
					{
						if (((stat & DSBSTATUS_BUFFERLOST) == 0) && (sndchan->Loop > 1))
						{
							if (sndchan->Loop != BigInt)
							{
								sndchan->Loop--;
								buf->SetCurrentPosition(0);
								buf->Play(0, 0, 0);
							}
						}
						else
						{
							buf->Stop();

							SoundKanalu--;
							sndchan->Num = 0;
							buf->Release();
							sndchan->DSBuffer = NULL;
						}
					}
				}
			}

			sndchan++;
		}

		if (WaveOut == NULL) return;
	}

// test, zda jsou nějaké aktivní kanály (a příp. uzavření výstupu)
	if (SoundKanalu <= 0)
	{
		if (WaveOut != NULL)
		{
			for (int i = SOUNDBUFFERU-1; i >= 0; i--)
			{
				if ((WaveHeader[i].dwFlags & WHDR_DONE) == 0) return;
			}
			PlaySoundStop();
		}
		return;
	}

// nalezení volného bufferu
	WAVEHDR* wh = WaveHeader;
	for (int b = SOUNDBUFFERU; (b > 0) && (SoundKanalu > 0); b--)
	{
		if (wh->dwFlags & WHDR_DONE)
		{

// uvolnění bufferu pro zápis
			if (wh->dwFlags & WHDR_PREPARED)
			{
				::waveOutUnprepareHeader(WaveOut, wh, sizeof(WAVEHDR));
			}

// adresa dat bufferu
			BYTE* buf = SoundBuf[SOUNDBUFFERU-b];

// příprava struktury popisovače dat
			wh->lpData = (char*)buf;			// adresa bufferu s daty
			wh->dwBufferLength = SOUNDBUFSIZE;	// velikost dat v bufferu
			wh->dwBytesRecorded = SOUNDBUFSIZE;	// počet dat v bufferu
			wh->dwUser = 0;						// uživatelská data
			wh->dwFlags = 0;					// parametry
			wh->dwLoops = 0;					// počet opakování
			wh->lpNext = NULL;					// není další buffer
			wh->reserved = 0;

// vymazání bufferu
			MemFill(buf, SOUNDBUFSIZE, 0);

// příprava proměnných k prohledání tabulky kanálů
			int cit = SoundKanalu;			// čítač zbylých aktivních kanálů
			int i = MAXSOUNDKANALU;			// čítač testovaných kanálů
			SOUNDCHAN* sndchan = SoundChan;	// ukazatel kanálů

// nalezení aktivního zvukového kanálu
			for (; (i > 0) && (cit > 0) && !Pause; i--)
			{
				if (sndchan->Num > 0)
				{
					cit--;

// zápis dat do bufferu
					int num = SOUNDBUFSIZE/4;
					short* buf2 = (short*)buf;

					int n = SoundWriteBuf(sndchan, buf2, num);

					num -= n;
					buf2 += n*2;

// ukončení zvuku
					while ((DWORD)Round(sndchan->Offset) >= (DWORD)sndchan->Num)
					{
						sndchan->Loop--;

						if (sndchan->Loop > 0)
						{
							sndchan->Offset = 0;

							if (num > 0)
							{
								n = SoundWriteBuf(sndchan, buf2, num);
								num -= n;
								buf2 += n*2;
							}
							else
							{
								break;
							}
						}
						else
						{
							SoundKanalu--;
							sndchan->Sound.Empty();
							sndchan->Num = 0;

							IDirectSoundBuffer* buf = sndchan->DSBuffer;

							if (buf != NULL)
							{
								buf->Stop();
								buf->Release();
								sndchan->DSBuffer = NULL;
							}
							break;
						}
					}
				}

// zvýšení ukazatele v bufferu kanálů
				sndchan++;
			}

// příprava bufferu k přehrávání
			waveOutPrepareHeader(WaveOut, wh, sizeof(WAVEHDR));

// předání bufferu přehrávacímu zařízení
			waveOutWrite(WaveOut, wh, sizeof(WAVEHDR));
		}

// adresa dalšího bufferu
		wh++;
	}
}
	

/***************************************************************************\
*																			*
*									funkce									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// funkce s návratem zvuku

void _fastcall FSoundFunc(CSound& sound)
{
// úschova indexu volané funkce
	int data = ExecItem[-1].Data;

// úschova indexu proměnné s návratovou hodnotou
	int res = ExecItem[-1].List;

// inicializace lokálních proměnných
	FCommand();

// úschova ukazatele programu
	EXECITEM* oldexe = ExecItem;

// nová adresa programu
	ExecItem = ProgBuf + data;

// kontrola hloubky vnoření
	Hloubka--;
	if (Hloubka >= 0)
	{

// vyvolání funkce
		FCommand();
	}
	Hloubka++;

// návrat adresy programu
	ExecItem = oldexe;

// zrušení požadavku o přerušení
	Break &= ~(BREAKFUNC | BREAKWHILE);

// návrat výsledku operace
	sound = Sound[Sound.Num() - res];

// zrušení lokálních proměnných
	FCommand();
}


/***************************************************************************\
*																			*
*								proměnné									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// prázdný zvuk

void _fastcall FSoundEmpty(CSound& sound) { sound.Empty(); };


/////////////////////////////////////////////////////////////////////////////
// globální zvuk (Data = index)

void _fastcall FSoundObj(CSound& sound)
{
// index zvuku
	int inx = ExecItem[-1].Data;

// nastavení zvuku
	sound = Sound[inx];
}


/////////////////////////////////////////////////////////////////////////////
// globalní zvuk v seznamu (Data = index, List = seznam)

void _fastcall FSoundObjList(CSound& sound)
{
// index seznamu
	int list = ExecItem[-1].List;

// index zvuku, inkrementace indexu seznamu
	int inx = ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// nastavení zvuku
	sound = Sound[inx];
}


/////////////////////////////////////////////////////////////////////////////
// lokální zvuk (Data = index)

void _fastcall FSoundLoc(CSound& sound)
{
// index zvuku
	int inx = Sound.Num() - ExecItem[-1].Data;

// nastavení zvuku
	sound = Sound[inx];
}


/////////////////////////////////////////////////////////////////////////////
// lokální zvuk v seznamu (Data = index, List = seznam)

void _fastcall FSoundLocList(CSound& sound)
{
// index seznamu
	int list = List.Num() - ExecItem[-1].List;

// index zvuku, inkrementace indexu seznamu
	int inx = Sound.Num() - ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// nastavení zvuku
	sound = Sound[inx];
}


/***************************************************************************\
*																			*
*								konverze									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// konverze na 8 bitů

void _fastcall FSoundConv8Bit(CSound& sound)
{
// načtení zvuku ke konverzi
	FSound(sound);

// konverze na 8 bitů
	sound.Conv8Bit();
}


/////////////////////////////////////////////////////////////////////////////
// konverze na 16 bitů

void _fastcall FSoundConv16Bit(CSound& sound)
{
// načtení zvuku ke konverzi
	FSound(sound);

// konverze na 16 bitů
	sound.Conv16Bit();
}


/////////////////////////////////////////////////////////////////////////////
// konverze na stereo

void _fastcall FSoundConvStereo(CSound& sound)
{
// načtení zvuku ke konverzi
	FSound(sound);

// konverze na stereo
	sound.ConvStereo();
}


/////////////////////////////////////////////////////////////////////////////
// konverze na mono

void _fastcall FSoundConvMono(CSound& sound)
{
// načtení zvuku ke konverzi
	FSound(sound);

// konverze na mono
	sound.ConvMono();
}


/////////////////////////////////////////////////////////////////////////////
// konverze na frekvenci 11025

void _fastcall FSoundConv11025(CSound& sound)
{
// načtení zvuku ke konverzi
	FSound(sound);

// konverze na frekvenci 11025
	sound.ConvRate(11025);
}


/////////////////////////////////////////////////////////////////////////////
// konverze na frekvenci 22050

void _fastcall FSoundConv22050(CSound& sound)
{
// načtení zvuku ke konverzi
	FSound(sound);

// konverze na frekvenci 22050
	sound.ConvRate(22050);
}


/////////////////////////////////////////////////////////////////////////////
// konverze na frekvenci 44100

void _fastcall FSoundConv44100(CSound& sound)
{
// načtení zvuku ke konverzi
	FSound(sound);

// konverze na frekvenci 44100
	sound.ConvRate(44100);
}

/////////////////////////////////////////////////////////////////////////////
// součet jednoho zvuku - ponechá se beze změny

void _fastcall FSoundAdd1(CSound& sound) { FSound(sound); }


/////////////////////////////////////////////////////////////////////////////
// součet zvuků (Data = počet zvuků - 1)

void _fastcall FSoundAdd(CSound& sound)
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení prvního zvuku
	if (i >= 0) FSound(sound);

// pomocný buffer zvuku
	CSound sound2;

// cyklus přes všechny zvuky
	for (; i > 0; i--)
	{

// načtení dalšího zvuku
		FSound(sound2);
		sound.Add(sound2);
	}
}


/////////////////////////////////////////////////////////////////////////////
// konverze rychlosti zvuku

void _fastcall FSoundSpeed(CSound& sound)
{
// načtení vstupního zvuku
	FSound(sound);

// načtení koeficientu převodu
	double koef = FNum();

// konverze rychlosti zvuku
	sound.ConvSpeed(koef);
}


/////////////////////////////////////////////////////////////////////////////
// tónový generátor

void _fastcall FTonGen(CSound& sound)
{
// načtení frekvence
	double freq = FNum();

// načtení délky
	double len = FNum();

// vygenerování tónu
	sound.TonGen(freq, len);
}

/////////////////////////////////////////////////////////////////////////////
// načtení zvuku ze souboru

void _fastcall FGetFileSound(CSound& sound)
{
	sound.Empty();
	sound.LoadFile();
}

/////////////////////////////////////////////////////////////////////////////
// dekomprimace zvuku

void _fastcall FSoundConvPCM(CSound& sound)
{
// načtení vstupního zvuku
	FSound(sound);

// dekomprese zvuku
	sound.DeComp();
}
