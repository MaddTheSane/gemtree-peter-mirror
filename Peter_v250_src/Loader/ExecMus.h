
/***************************************************************************\
*																			*
*							Provádění programu - hudba						*
*																			*
\***************************************************************************/

#define MUSICERRORWAIT	(5*18)		// čekání na nový pokus o otevření zvuku
#define MUSICSTOPWAIT	(4*18)		// čítač pro zastavení hudby
#define MUSICMAXDELKA	(24*60*60*1000) // max. délka hudby

// obsluha hudby
extern	UINT	MusicDevice;		// ID zařízení MCI pro přehrávání hudby (0=není otevřeno)
extern	int		MusicDelka;			// délka hudby v ms
extern	bool	MusicPlaing;		// příznak přehrávání hudby
extern	bool	MusicPausing;		// příznak pauzy hudby
extern	bool	MusicEnd;			// příznak ukončení přehrávání
extern	int		MusicError;			// povolení opakovaného otevření hudby (při < 0)
extern	int		MusicAktPos;		// aktuální pozice hudby

/////////////////////////////////////////////////////////////////////////////
// obsluha hudby

void MusicPlay(CMusic mus, bool loop);	// zahájení přehrávání hudby
void SetMusicPos(int pos);			// nastavení pozice hudby v ms
void MusicPause();					// pauza přehrávání
void MusicStop();					// ukončení přehrávání hudby
void OnMusicEnd();					// ukončení přehrávání (obsluha hlášení)
void PlayMusicBack();				// obsluha hudby na pozadí


/////////////////////////////////////////////////////////////////////////////
// inline volání příkazu - hudba

inline void FMusic(CMusic& music)
{
	ExecItem++;
	ExecItem[-1].ExecMus(music);
}


// funkce
void _fastcall FMusicFunc(CMusic& music);		// funkce s návratem hudby

// proměnné
void _fastcall FMusicEmpty(CMusic& music);		// prázdná hudba
void _fastcall FMusicObj(CMusic& music);		// globální hudba (Data = index)
void _fastcall FMusicObjList(CMusic& music);	// globalní hudba v seznamu (Data = index, List = seznam)
void _fastcall FMusicLoc(CMusic& music);		// lokální hudba (Data = index)
void _fastcall FMusicLocList(CMusic& music);	// lokální hudba v seznamu (Data = index, List = seznam)

void _fastcall FGetFileMusic(CMusic& music);	// načtení hudby ze souboru
