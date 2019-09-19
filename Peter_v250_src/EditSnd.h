
/***************************************************************************\
*																			*
*								Editor zvuků								*
*																			*
\***************************************************************************/

namespace EditSound
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

// zobrazení délky zvuku
	void DispLength();

// zobrazení
	void Disp();

// obsluha časovače (TRUE=obslouženo)
	BOOL OnTimer(UINT timerID);

// start přehrávání
	void OnPlay();

// pozastavení nahrávání, přehrávání
	void OnPause();

// zastavení nahrávání i přehrávání
	void OnStop();

// přepnutí příznaku opakování
	void OnLoop();

// start nahrávání
	void OnRecord();

// aktualizace voleb
	void UpdateMenu();

// kopie bloku
	void Copy();

// navrácení bloku
	void Paste();

// aktualizace voleb bloku
	void UpdateClipboard();
}
