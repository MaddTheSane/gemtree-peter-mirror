
/***************************************************************************\
*																			*
*								Editor čísel								*
*																			*
\***************************************************************************/

namespace EditNum
{
// inicializace při startu
	void StartInit();

// zahájení editace
	void BegEdit(int index);

// přesun editačního pole
	HDWP MoveEdit(HDWP hdwp);

// zobrazení
	void Disp();

// vstup znaku z klávesnice (TRUE=zrušit)
	BOOL OnChar(TCHAR znak);

// výběr všeho
	void SelectAll();

// zrušení bloku
	void Delete();

// kopie bloku
	void Copy();

// vystřížení bloku
	void Cut();

// navrácení bloku
	void Paste();

// aktualizace voleb bloku
	void UpdateClipboard();

// změna textu
	void OnChange();

// znovunačtení čísla
	void ReLoad();
}
