
/***************************************************************************\
*																			*
*								Editor hudby								*
*																			*
\***************************************************************************/

namespace EditMusic
{
	extern HWND DispWnd;				// okno displeje

// inicializace při startu
	void StartInit();

// zahájení editace
	void BegEdit(int index);

// přesun okna displeje
	HDWP MoveDisp(HDWP hdwp);

// nastavení časového údaje (v sekundách)
	void DispSet(int time);

// zobrazení délky hudby
	void DispLength();

// zobrazení
	void Disp();

// obsluha časovače (TRUE=obslouženo)
	BOOL OnTimer(UINT timerID);

// ukončení přehrávání
	void OnMMNotify(DWORD flags, int devID);

// start přehrávání (init = jen zjištění délky a přerušení)
	void OnPlay(BOOL init = FALSE);

// pozastavení nahrávání, přehrávání
	void OnPause();

// zastavení nahrávání i přehrávání
	void OnStop();

// přepnutí příznaku opakování
	void OnLoop();

// převinutí zpět
	void OnRew();

// převinutí vpřed
	void OnFrw();

// aktualizace voleb
	void UpdateMenu();

// kopie bloku
	void Copy();

// navrácení bloku
	void Paste();

// aktualizace voleb bloku
	void UpdateClipboard();
}
