
/***************************************************************************\
*																			*
*								Editor sprajtů								*
*																			*
\***************************************************************************/

namespace EditSprite
{
extern	BOOL	MDraging;				// příznak tažení ikony editoru sprajtů
//extern	CIcon*	EditIcon;				// editovaná ikona
extern	int		Index;					// editovaný sprajt
extern	int		IndexPic;				// index editovaného obrázku

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

// ---------------- obsluha posuvníků ---------

// přesun posuvníků při změně rozměrů okna
	HDWP MoveScroll(HDWP hdwp);

// aktualizace zobrazení posuvníků
	void SetScroll();

// horizontální posuvník
	void OnHScroll(int code, int pos);

// vertikální posuvník
	void OnVScroll(int code, int pos);

// -------------- obsluha editace -----------

// ukončení tažení
	void EndDrag();

// stisk tlačítka myši (TRUE = obslouženo)
	BOOL OnButtonDown(UINT flags, int x, int y, BOOL right, BOOL dclick);

// posun myši
	void OnMouseMove(UINT flags, int x, int y);

// uvolnění tlačítka myši
	void OnButtonUp(UINT keys, BOOL right);

// položení obrázku zvnějšku
	void DragDrop(int pic);

// test
	void OnTest();

// -------------- obsluha UNDO -------------

// úschova pro UNDO
	void PushUndo();
}
