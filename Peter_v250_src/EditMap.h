
/***************************************************************************\
*																			*
*								Editor ploch								*
*																			*
\***************************************************************************/

namespace EditMap
{
extern	BOOL	MDraging;				// příznak tažení ikony editoru plochy

// inicializace při startu
	void StartInit();

// zahájení editace
	void BegEdit(int index);

// nastavení rozměrů plochy
	void OnDimen();

// ----------- obsluha zobrazení ------------

// přepočet rozměrů editoru ikon a obrázků
	void ReCalc();

// zobrazení celého okna editoru
	void Disp();

// aktualizační překreslení plochy (s přepočtem rozměrů)
	void ReDisp();

// aktualizační překreslení plochy
	void DispAkt(HDC dc);

// zapnutí/vypnutí rastru
	void OnRastr();

// obsluha zobrazení kurzoru myši (TRUE=obslouženo)
	BOOL OnSetCursor(int x, int y);

// ---------------- obsluha posuvníků ---------

// přesun posuvníků při změně rozměrů okna
	HDWP MoveScroll(HDWP hdwp);

// aktualizace zobrazení posuvníků
	void SetScroll();

// aktualizace pozice editačního pole
	void SetEditMapNum();

// horizontální posuvník
	void OnHScroll(int code, int pos);

// vertikální posuvník
	void OnVScroll(int code, int pos);

// -------------- obsluha editace -----------

// zahájení editace číselného parametru
	void BegEditSwcNum(int x, int y);

// změna editovaného čísla
	void OnChangeEditSwcNum();

// ukončení editace čísla
	void EndEditSwcNum();

// ukončení tažení
	void EndDrag();

// stisk tlačítka myši (TRUE = obslouženo)
	BOOL OnButtonDown(UINT flags, int x, int y, BOOL right, BOOL dclick);

// posun myši
	void OnMouseMove(UINT flags, int x, int y);

// uvolnění tlačítka myši
	void OnButtonUp(UINT keys, BOOL right);

// položení ikony zvnějšku
	void DragDrop(int icon);

// vstup klávesy z klávesnice (TRUE=obslouženo)
	BOOL OnKeyDown(int key);

// nastavení editačního módu
	void SetMode(int mode);

// aktualizace voleb výběru módu
	void UpdateMenu();

// -------------- obsluha bloku -----------

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

// mód výplně
	void OnFillMap();

// editační mód
	void OnEditMap();

// -------------- obsluha UNDO -------------

// úschova plochy pro UNDO
	void PushUndo();
}
