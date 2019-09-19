
/***************************************************************************\
*																			*
*								Editor textu								*
*																			*
\***************************************************************************/

namespace EditText
{
	extern	int		Index;						// editovaná položka

// opakovaná inicializace po změně jazyku
	void StartReInit();

// inicializace při startu
	void StartInit();

// zahájení editace
	void BegEdit(int index);

// přesun editačního pole
	HDWP MoveEdit(HDWP hdwp);

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
}
