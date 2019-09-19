
/***************************************************************************\
*																			*
*							Editor ikon a obrázků							*
*																			*
\***************************************************************************/

namespace EditIcon
{
extern	BOOL	NewDimPoint;			// rozměry zadány v bodech

// ----------- obsluha volby editace -------

// inicializace editoru ikon při startu
	void StartInit();

// zahájení editace
	void BegEdit(int bufID, int index);

// nastavení rozměrů obrázku
	void OnDimen();


// ----------- obsluha zobrazení ------------

// přepočet rozměrů editoru ikon a obrázků
	void ReCalc();

// zobrazení celého okna editoru
	void Disp();

// aktualizační překreslení obrázku (s přepočtem rozměrů)
	void ReDisp();

// překreslení okrajů kolem obrázku
	void DispRam(HDC dc);

// aktualizační překreslení obrázku
	void DispAkt(HDC dc);

// vykreslení políčka výběru barvy
	void DispCol(int col);
	void DispCol(HDC dc, int col);

// zvětšení měřítka
	void ZoomIn();

// zmenšení měřítka
	void ZoomOut();

// aktualizace tlačítek pro zvětšení/zmenšení měřítka
	void UpdateZoomInOut();

// překreslení ikon ve stromech
	void UpdateTree();

// zapnutí/vypnutí rastru
	void OnRastr();

// ---------------- obsluha posuvníků ---------

// přesun posuvníků při změně rozměrů okna
	HDWP MoveScroll(HDWP hdwp);

// aktualizace zobrazení posuvníků
	void SetScroll();

// horizontální posuvník
	void OnHScroll(int code, int pos);

// vertikální posuvník
	void OnVScroll(int code, int pos);

// ------------ obsluha editace -------------

// úschova obrázku do pomocného bufferu
	void Push();

// návrat obrázku z pomocného bufferu
	void Pop();

// načtení bodu z bufferu
	inline BYTE _fastcall GetPush(int x, int y);

// nastavení bodu v bufferu
	inline void _fastcall SetPush(int x, int y, BYTE col);

// nastavení indikátoru barvy
	void SetColInd(BYTE col);

// načtení bodu obrázku
	inline BYTE _fastcall GetPoint(int x, int y);

// obsluha zobrazení kurzoru myši (TRUE=obslouženo)
	BOOL OnSetCursor(int x, int y);

// stisk tlačítka myši (TRUE = obslouženo)
	BOOL OnButtonDown(UINT flags, int x, int y, BOOL right, BOOL dclick);

// posun myši
	void OnMouseMove(UINT flags, int x, int y);

// uvolnění tlačítka myši
	void OnButtonUp(UINT keys, BOOL right);

// vstup znaku z klávesnice (TRUE=obslouženo)
	BOOL OnChar(TCHAR znak);

// vstup klávesy z klávesnice (TRUE=obslouženo)
	BOOL OnKeyDown(int key);

// nastavení editačního módu
	void SetMode(int mode);

// provedení editace bloku (-1 = zatím neurčeno)
	void EditBlok(int mode);

// aktualizace voleb výběru módu
	void UpdateMenu();

// -------------- obsluha kreslení ---------

// vykreslení bodu
	void _fastcall SetPoint(int x, int y, BYTE col);

// vykreslení bodu štětce
	void SetPaint(int x, int y, BYTE col);

// nastavení bodu spreje
	void SetSpray(int x, int y, BYTE col);

// nastavení čáry štětcem
	void SetLinePaint(int x1, int y1, int x2, int y2, BYTE col);

// vykreslení obdélníku
	void SetRect(int x1, int y1, int x2, int y2, BYTE col);

// vykreslení obdélníku s výplní
	void SetRectFill(int x1, int y1, int x2, int y2, BYTE col);

// vykreslení kružnice
	void SetRound(int x1, int y1, int x2, int y2, BYTE col);

// kreslení kruhu
	void SetRoundFill(int x1, int y1, int x2, int y2, BYTE col);

// kreslení elipsy
	void SetElip(int x1, int y1, int x2, int y2, BYTE col);

// kreslení oválu
	void SetElipFill(int x1, int y1, int x2, int y2, BYTE col);

// kreslení koule
	void SetKoule(int x1, int y1, int x2, int y2, BYTE col0);

// výplň (col = nová barva, fil = podklad)
	void SetFill(int x, int y, BYTE col, BYTE fil);

// zobrazení textu
	void ReDispText();

// -------------- obsluha bloku -----------

// načtení bodu z bloku
	inline BYTE _fastcall GetBlok(int x, int y);

// nastavení bodu v bloku
	inline void _fastcall SetBlok(int x, int y, BYTE col);

// zapnutí/vypnutí výběru bloku
	void DispSelect();

// zobrazení bloku při přesunu
	void MoveSelect();

// test bodu, zda je uvnitř vybraného bloku
	BOOL TestBlok(int x, int y);

// zrušení bloku
	void Delete();

// kopie bloku (TRUE=operace OK)
	BOOL Copy();

// vystřížení bloku
	void Cut();

// vložení bloku
	void Paste();

// výběr všeho
	void SelectAll();

// aktualizace voleb bloku
	void UpdateClipboard();

// otisk bloku
	void OnOtisk();

// -------------- obsluha UNDO -------------

// omezení velikosti UNDO bufferu
//	void LimitUndoSize();

// nastavení nové velikosti UNDO bufferu
//	void SetUndoNum(int num);

// uložení obrázku do undo bufferu
	void PushUndo();

// navrácení ikony z undo bufferu
//	void PopUndo();

// UNDO
//	void Undo();

// REDO
//	void Redo();

// aktualizace voleb UNDO a REDO
//	void UpdateUndoRedo();
}
