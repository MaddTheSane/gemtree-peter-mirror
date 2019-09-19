
#include "Main.h"

/***************************************************************************\
*																			*
*						Provádění programu - logický výraz					*
*																			*
\***************************************************************************/

#pragma optimize("t", on)			// optimalizace na maximální rychlost


/***************************************************************************\
*																			*
*									funkce									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// funkce s návratem logické hodnoty

bool FBoolFunc()
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
	bool result = Bool[Bool.Num() - res];

// zrušení lokálních proměnných
	FCommand();

	return result;
}


/***************************************************************************\
*																			*
*								proměnné									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// konstanta TRUE a FALSE

bool FTrue() { return true; };
bool FFalse() { return false; };


/////////////////////////////////////////////////////////////////////////////
// globální proměnná (Data = index)

bool FBoolObj()
{
// index
	int inx = ExecItem[-1].Data;

// data
	return Bool[inx];
}


/////////////////////////////////////////////////////////////////////////////
// globalní proměnná v seznamu (Data = index, List = seznam)

bool FBoolObjList()
{
// index seznamu
	int list = ExecItem[-1].List;

// index proměnné, inkrementace indexu seznamu
	int inx = ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// data
	return Bool[inx];
}


/////////////////////////////////////////////////////////////////////////////
// lokální proměnná (Data = index)

bool FBoolLoc()
{
// index proměnné
	int inx = Bool.Num() - ExecItem[-1].Data;

// data
	return Bool[inx];
}


/////////////////////////////////////////////////////////////////////////////
// lokální proměnná v seznamu (Data = index, List = seznam)

bool FBoolLocList()
{
// index seznamu
	int list = List.Num() - ExecItem[-1].List;

// index proměnné, inkrementace indexu seznamu
	int inx = Bool.Num() - ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// data
	return Bool[inx];
}


/////////////////////////////////////////////////////////////////////////////
// pohyb globálního sprajtu (Data = index)

bool FGetSpriteMove()
{
// index sprajtu
	int inx = ExecItem[-1].Data;

// pohyb sprajtu
	return (Sprite[inx].KrokCit() > 0);
}


/////////////////////////////////////////////////////////////////////////////
// pohyb sprajtu v seznamu (Data = index, List = seznam)

bool FGetSpriteMoveList()
{
// index seznamu
	int list = ExecItem[-1].List;

// index sprajtu, inkrementace indexu seznamu
	int inx = ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// pohyb sprajtu
	return (Sprite[inx].KrokCit() > 0);
}


/////////////////////////////////////////////////////////////////////////////
// pohyb lokálního sprajtu (Data = index)

bool FGetSpriteMoveLoc()
{
// index sprajtu
	int inx = Sprite.Num() - ExecItem[-1].Data;

// pohyb sprajtu
	return (Sprite[inx].KrokCit() > 0);
}


/////////////////////////////////////////////////////////////////////////////
// pohyb lokálního sprajtu v seznamu (Data = index, List = seznam)

bool FGetSpriteMoveLocList()
{
// index seznamu
	int list = List.Num() - ExecItem[-1].List;

// index sprajtu, inkrementace indexu seznamu
	int inx = Sprite.Num() - ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// pohyb sprajtu
	return (Sprite[inx].KrokCit() > 0);
}


/////////////////////////////////////////////////////////////////////////////
// viditelnost globálního sprajtu (Data = index)

bool FGetSpriteVis()
{
// index sprajtu
	int inx = ExecItem[-1].Data;

// viditelnost sprajtu
	return Sprite[inx].Visible();
}


/////////////////////////////////////////////////////////////////////////////
// viditelnost sprajtu v seznamu (Data = index, List = seznam)

bool FGetSpriteVisList()
{
// index seznamu
	int list = ExecItem[-1].List;

// index sprajtu, inkrementace indexu seznamu
	int inx = ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// viditelnost sprajtu
	return Sprite[inx].Visible();
}


/////////////////////////////////////////////////////////////////////////////
// viditelnost lokálního sprajtu (Data = index)

bool FGetSpriteVisLoc()
{
// index sprajtu
	int inx = Sprite.Num() - ExecItem[-1].Data;

// viditelnost sprajtu
	return Sprite[inx].Visible();
}


/////////////////////////////////////////////////////////////////////////////
// viditelnost lokálního sprajtu v seznamu (Data = index, List = seznam)

bool FGetSpriteVisLocList()
{
// index seznamu
	int list = List.Num() - ExecItem[-1].List;

// index sprajtu, inkrementace indexu seznamu
	int inx = Sprite.Num() - ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// viditelnost sprajtu
	return Sprite[inx].Visible();
}


/***************************************************************************\
*																			*
*							matematické operátory							*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// operátor EQU - pro 1 prvek (porovná číslo s nulou)

bool FEqu1() { return FNum() == 0; };


/////////////////////////////////////////////////////////////////////////////
// operátor EQU (Data = počet prvků - 1)

bool FEqu()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení prvního čísla
	double num = FNum();

// cyklus přes další čísla
	for (; i > 0; i--)
	{

// porovnání dalšího čísla
		if (FNum() != num)
		{

// nevyhovuje - zrychlené ukončení operace
			if (i > 1) ExecItem += ExecItem->Jump;
			return false;
		}
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// operátor NEQU - pro 1 prvek (porovná číslo s nulou)

bool FNEqu1() { return FNum() != 0; };


/////////////////////////////////////////////////////////////////////////////
// operátor NEQU (Data = počet prvků - 1)

bool FNEqu()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení prvního čísla
	double num1 = FNum();
	double num2;

// cyklus přes další čísla
	for (; i > 0; i--)
	{

// načtení dalšího čísla
		num2 = FNum();

// porovnání dalšího čísla
		if (num2 == num1)
		{

// nevyhovuje - zrychlené ukončení operace
			if (i > 1) ExecItem += ExecItem->Jump;
			return false;
		}

// úschova čísla pro příští operaci
		num1 = num2;
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// operátor EQGR - pro 1 prvek (porovná číslo s nulou)

bool FEqGr1() { return FNum() >= 0; };


/////////////////////////////////////////////////////////////////////////////
// operátor EQGR (Data = počet prvků - 1)

bool FEqGr()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení prvního čísla
	double num1 = FNum();
	double num2;

// cyklus přes další čísla
	for (; i > 0; i--)
	{

// načtení dalšího čísla
		num2 = FNum();

// porovnání dalšího čísla
		if (num1 < num2)
		{

// nevyhovuje - zrychlené ukončení operace
			if (i > 1) ExecItem += ExecItem->Jump;
			return false;
		}

// úschova čísla pro příští operaci
		num1 = num2;
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// operátor EQLT - pro 1 prvek (porovná číslo s nulou)

bool FEqLt1() { return FNum() <= 0; };


/////////////////////////////////////////////////////////////////////////////
// operátor EQLT (Data = počet prvků - 1)

bool FEqLt()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení prvního čísla
	double num1 = FNum();
	double num2;

// cyklus přes další čísla
	for (; i > 0; i--)
	{

// načtení dalšího čísla
		num2 = FNum();

// porovnání dalšího čísla
		if (num1 > num2)
		{

// nevyhovuje - zrychlené ukončení operace
			if (i > 1) ExecItem += ExecItem->Jump;
			return false;
		}

// úschova čísla pro příští operaci
		num1 = num2;
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// operátor GR - pro 1 prvek (porovná číslo s nulou)

bool FGr1() { return FNum() > 0; };


/////////////////////////////////////////////////////////////////////////////
// operátor GR (Data = počet prvků - 1)

bool FGr()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení prvního čísla
	double num1 = FNum();
	double num2;

// cyklus přes další čísla
	for (; i > 0; i--)
	{

// načtení dalšího čísla
		num2 = FNum();

// porovnání dalšího čísla
		if (num1 <= num2)
		{

// nevyhovuje - zrychlené ukončení operace
			if (i > 1) ExecItem += ExecItem->Jump;
			return false;
		}

// úschova čísla pro příští operaci
		num1 = num2;
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// operátor LT - pro 1 prvek (porovná číslo s nulou)

bool FLt1() { return FNum() < 0; };


/////////////////////////////////////////////////////////////////////////////
// operátor LT (Data = počet prvků - 1)

bool FLt()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení prvního čísla
	double num1 = FNum();
	double num2;

// cyklus přes další čísla
	for (; i > 0; i--)
	{

// načtení dalšího čísla
		num2 = FNum();

// porovnání dalšího čísla
		if (num1 >= num2)
		{

// nevyhovuje - zrychlené ukončení operace
			if (i > 1) ExecItem += ExecItem->Jump;
			return false;
		}

// úschova čísla pro příští operaci
		num1 = num2;
	}
	return true;
}


/***************************************************************************\
*																			*
*								logické operátory							*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// operátor AND - pro 1 prvek (jenom vrátí hodnotu beze změny)

bool FAnd1() { return FBool(); }


/////////////////////////////////////////////////////////////////////////////
// operátor AND (Data = počet prvků - 1)

bool FAnd()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// cyklus přes všechny prvky
	for (; i >= 0; i--)
	{

// kontrola prvku
		if (!FBool())
		{

// nevyhovuje - zrychlené ukončení operace
			if (i > 0) ExecItem += ExecItem->Jump;
			return false;
		}
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// operátor OR (Data = počet prvků - 1)

bool FOr()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// cyklus přes všechny prvky
	for (; i >= 0; i--)
	{

// kontrola prvku
		if (FBool())
		{

// vyhovuje - zrychlené ukončení operace
			if (i > 0) ExecItem += ExecItem->Jump;
			return true;
		}
	}
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// operátor XOR (Data = počet prvků - 1)

bool FXor()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// střadač operace
	bool result = false;

// cyklus přes všechny prvky
	for (; i >= 0; i--)
	{

// načtení prvku
		result ^= FBool();
	}
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// operátor NOT

bool FNot() 
{
	bool result = FBool();
	result ^= true ^ false;
	return result;
}


/***************************************************************************\
*																			*
*								obsluha ikon								*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// porovnání jedné ikony, zda je prázdná

bool FCompEqIco1()
{
	CIcon icon;
	FIcon(icon);
	return (icon.IsEmpty() != FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// porovnání shodnosti ikon (Data = počet prvků - 1)

bool FCompEqIco()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení první ikony
	CIcon icon1;
	FIcon(icon1);
	CIcon icon2;

// cyklus přes další ikony
	for (; i > 0; i--)
	{

// načtení další ikony
		FIcon(icon2);

// porovnání ikon
		if (icon1 != icon2)
		{

// nevyhovuje - zrychlené ukončení operace
			if (i > 1) ExecItem += ExecItem->Jump;
			return false;
		}

// ikona 2 se uschová pro příští použití
//		icon1 = icon2;
	}
	return true;
}







/////////////////////////////////////////////////////////////////////////////
// porovnání jedné plochy, zda je prázdná

bool FCompEqMap1()
{
	CMap map;
	FMap(map);
	return (map.IsEmpty() != FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// porovnání shodnosti ploch (Data = počet prvků - 1)

bool FCompEqMap()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení první plochy
	CMap map1;
	FMap(map1);
	CMap map2;

// cyklus přes další plochy
	for (; i > 0; i--)
	{

// načtení další plochy
		FMap(map2);

// porovnání ploch
		if (map1 != map2)
		{

// nevyhovuje - zrychlené ukončení operace
			if (i > 1) ExecItem += ExecItem->Jump;
			return false;
		}

// plocha 2 se uschová pro příští použití
//		map1 = map2;
	}
	return true;
}



/////////////////////////////////////////////////////////////////////////////
// porovnání jednoho obrázku, zda je prázdný

bool FCompEqPic1()
{
	CPicture pic;
	FPicture(pic);
	return (pic.IsEmpty() != FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// porovnání shodnosti obrázků (Data = počet prvků - 1)

bool FCompEqPic()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení prvního obrázku
	CPicture pic1;
	FPicture(pic1);
	CPicture pic2;

// cyklus přes další obrázky
	for (; i > 0; i--)
	{

// načtení dalšího obrázku
		FPicture(pic2);

// porovnání obrázků
		if (pic1 != pic2)
		{

// nevyhovuje - zrychlené ukončení operace
			if (i > 1) ExecItem += ExecItem->Jump;
			return false;
		}

// obrázek 2 se uschová pro příští použití
//		pic1 = pic2;
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// porovnání jednoho sprajtu, zda je prázdný

bool FCompEqSpr1()
{
	CSprite spr;
	FSprite(spr);
	return (spr.IsEmpty() != FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// porovnání shodnosti sprajtů (Data = počet prvků - 1)

bool FCompEqSpr()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení prvního sprajtu
	CSprite spr1;
	FSprite(spr1);
	CSprite spr2;

// cyklus přes další sprajty
	for (; i > 0; i--)
	{

// načtení dalšího sprajtu
		FSprite(spr2);

// porovnání sprajtů
		if (spr1 != spr2)
		{

// nevyhovuje - zrychlené ukončení operace
			if (i > 1) ExecItem += ExecItem->Jump;
			return false;
		}

// sprajt 2 se uschová pro příští použití
//		spr1 = spr2;
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// porovnání jednoho zvuku, zda je prázdný

bool FCompEqSnd1()
{
	CSound snd;
	FSound(snd);
	return (snd.IsEmpty() != FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// porovnání shodnosti zvuků (Data = počet prvků - 1)

bool FCompEqSnd()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení prvního zvuku
	CSound snd1;
	FSound(snd1);
	CSound snd2;

// cyklus přes další zvuky
	for (; i > 0; i--)
	{

// načtení dalšího zvuku
		FSound(snd2);

// porovnání zvuků
		if (snd1 != snd2)
		{

// nevyhovuje - zrychlené ukončení operace
			if (i > 1) ExecItem += ExecItem->Jump;
			return false;
		}

// zvuk 2 se uschová pro příští použití
//		snd1 = snd2;
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// porovnání jedné hudby, zda je prázdná

bool FCompEqMus1()
{
	CMusic mus;
	FMusic(mus);
	return (mus.IsEmpty() != FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// porovnání shodnosti hudeb (Data = počet prvků - 1)

bool FCompEqMus()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení první hudby
	CMusic mus1;
	FMusic(mus1);
	CMusic mus2;

// cyklus přes další hudby
	for (; i > 0; i--)
	{

// načtení další hudby
		FMusic(mus2);

// porovnání hudeb
		if (mus1 != mus2)
		{

// nevyhovuje - zrychlené ukončení operace
			if (i > 1) ExecItem += ExecItem->Jump;
			return false;
		}

// hudba 2 se uschová pro příští použití
//		mus1 = mus2;
	}
	return true;
}


/***************************************************************************\
*																			*
*								obsluha textů								*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// porovnání jednoho textu, zda je prázdný

bool FCompEqTxt1()
{
	CString txt;
	FText(txt);
	return (txt.IsEmpty() != FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// porovnání textů na shodnost (Data = počet textů - 1)

bool FCompEqTxt()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení prvního textu
	CString txt1;
	FText(txt1);
	CString txt2;

// cyklus přes další texty
	for (; i > 0; i--)
	{

// načtení dalšího textu
		FText(txt2);

// porovnání textů
		if (txt1 != txt2)
		{

// nevyhovuje - zrychlené ukončení operace
			if (i > 1) ExecItem += ExecItem->Jump;
			return false;
		}

// text 2 se uschová pro příští použití
//		txt1 = txt2;
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// porovnání jednoho textu, zda je neprázdný

bool FCompGtTxt1()
{
	CString txt;
	FText(txt);
	return (txt.IsNotEmpty() != FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// porovnání textů na neshodnost (Data = počet textů - 1)

bool FCompGtTxt()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení prvního textu
	CString txt1;
	FText(txt1);
	CString txt2;

// cyklus přes další texty
	for (; i > 0; i--)
	{

// načtení dalšího textu
		FText(txt2);

// porovnání textů
		if (txt1 <= txt2)
		{

// nevyhovuje - zrychlené ukončení operace
			if (i > 1) ExecItem += ExecItem->Jump;
			return false;
		}

// text 2 se uschová pro příští použití
		txt1 = txt2;
	}
	return true;
}


/***************************************************************************\
*																			*
*							obsluha Petra a Petry							*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// načtení viditelnosti Petra a Petry

bool FGetVisible() { return Sprite[0].Visible(); }
bool FGetVisible2() { return Sprite[1].Visible(); }


/////////////////////////////////////////////////////////////////////////////
// načtení přepínače Petra

bool _fastcall FGetSwc(DWORD mask)
{
// načtení pozice Petra
	MAPITEM* item = PetrXYMap();

// zjištění přepínače, je-li platná pozice
	if (item != NULL)
	{
		return ((item->Param & mask) != 0);
	}
	return false;
}

bool FGetSwc1() { return FGetSwc(MAP_FLAG1); };		// přepínač 1 - Petr
bool FGetSwc2() { return FGetSwc(MAP_FLAG2); };		// přepínač 2 - Petr
bool FGetSwc3() { return FGetSwc(MAP_FLAG3); };		// přepínač 3 - Petr
bool FGetSwc4() { return FGetSwc(MAP_FLAG4); };		// přepínač 4 - Petr
bool FGetSwc5() { return FGetSwc(MAP_FLAG5); };		// přepínač 5 - Petr

/////////////////////////////////////////////////////////////////////////////
// načtení přepínače Petry

bool _fastcall FGet2Swc(DWORD mask)
{
// načtení pozice Petry
	MAPITEM* item = Petr2XYMap();

// zjištění přepínače, je-li platná pozice
	if (item != NULL)
	{
		return ((item->Param & mask) != 0);
	}
	return false;
}

bool FGetSwc12() { return FGet2Swc(MAP_FLAG1); };	// přepínač 1 - Petra
bool FGetSwc22() { return FGet2Swc(MAP_FLAG2); };	// přepínač 2 - Petra
bool FGetSwc32() { return FGet2Swc(MAP_FLAG3); };	// přepínač 3 - Petra
bool FGetSwc42() { return FGet2Swc(MAP_FLAG4); };	// přepínač 4 - Petra
bool FGetSwc52() { return FGet2Swc(MAP_FLAG5); };	// přepínač 5 - Petra


/***************************************************************************\
*																			*
*							obsluha klávesnice								*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// test stisku klávesy

bool FKeyTest()
{
// načtení kódu klávesy
	int klav = FIntR();

// kód klávesy Pause
	if (klav == VK_PAUSE) return Pause;

// kontrola platnosti kódu klávesy
	if ((DWORD)klav >= (DWORD)KEYMAPSIZE) return false;

// test žádné klávesy
	if (klav == 0) return ((KeyMapNum == 0) && !Pause);

// test klávesy
	return KeyMap[klav];
}


/////////////////////////////////////////////////////////////////////////////
// test stavu Num Lock

bool FStatusNumLock() { return NumLock; }


/////////////////////////////////////////////////////////////////////////////
// test stavu Caps Lock

bool FStatusCapsLock() { return CapsLock; }


/////////////////////////////////////////////////////////////////////////////
// test stavu Scroll Lock

bool FStatusScrollLock() { return ScrollLock; }


/////////////////////////////////////////////////////////////////////////////
// test stavu Insert

bool FStatusInsert() { return InsertLock; }


/***************************************************************************\
*																			*
*							obsluha myši									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// test stisku levého tlačítka myši

bool FMouseL() { return LMouseDown; }


/////////////////////////////////////////////////////////////////////////////
// test stisku pravého tlačítka myši

bool FMouseR() { return RMouseDown; }


/////////////////////////////////////////////////////////////////////////////
// test kliku levým tlačítkem myši

bool FMouseLClick()
{
	bool result = LMouseClick;
	LMouseClick = false;
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// test kliku pravým tlačítkem myši

bool FMouseRClick()
{
	bool result = RMouseClick;
	RMouseClick = false;
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// test dvojkliku levým tlačítkem myši

bool FMouseLDClick()
{
	bool result = LMouseDClick;
	LMouseDClick = false;
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// test dvojkliku pravým tlačítkem myši

bool FMouseRDClick()
{
	bool result = RMouseDClick;
	RMouseDClick = false;
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// test myši v oblasti

bool FMouseRegion()
{
// načtení rozměrů regionu
	double x1 = FNum();				// počátek X
	double y1 = FNum();				// počátek Y
	double x2 = FNum() + x1;		// konec X
	double y2 = FNum() + y1;		// konec Y

// porovnání oblasti
	return ((MouseX >= x1) && (MouseX < x2) && (MouseY >= y1) && (MouseY < y2));
}

/////////////////////////////////////////////////////////////////////////////
// test myši v dialogovém prvku

bool FMouseItem()
{
// načtení čísla prvku
	int id = FIntR();

// test, zda je platný prvek okna
	if (Win.IsValid(id))
	{
		WINITEM* item = &Win[id];

// příprava výšky rodiče
		int height;
		if ((item->Typ == WINITEM_WINDOW) || (item->Typ == WINITEM_WINDOWRO))
		{
			height = ScreenHeight0;
		}
		else
		{
			height = Win[item->Parent].ClientHeight;
		}

// příprava rozměrů regionu
		double x1 = (double)item->Left/ICONWIDTH;							// počátek X
		double y1 = (double)(height - item->Top - item->Height)/ICONHEIGHT;	// počátek Y
		double x2 = (double)item->Width/ICONWIDTH + x1;						// konec X
		double y2 = (double)item->Height/ICONHEIGHT + y1;					// konec Y

// porovnání oblasti
		return ((MouseX >= x1) && (MouseX < x2) && (MouseY >= y1) && (MouseY < y2));
	}
	else
	{
		return (id < 0);
	}
}

/***************************************************************************\
*																			*
*							obsluha zvuku									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// test přehrávání jakéhokoliv zvuku

bool FSoundTest()
{
	return ((WaveOut != NULL) || (SoundKanalu > 0));
}


/////////////////////////////////////////////////////////////////////////////
// test přehrávání konkrétního zvuku

bool FSoundTest1()
{
// načtení testovaného zvuku
	CSound sound;
	FSound(sound);

// nestandardní formát
	if (!WaveFormatPCM)
	{
		return ((WaveOut != NULL) && (sound == Sound2));
	}

// nalezení zvuku v bufferu
	for (int i = 0; i < MAXSOUNDKANALU; i++)
	{
		if ((SoundChan[i].Num > 0) && (SoundChan[i].Sound == sound) && (SoundChan[i].SoundID == SoundID))
		{
			return true;
		}
	}
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// příznak stereo

bool FGetSoundStereo()
{
	CSound snd;
	FSound(snd);
	return (snd.Channels() == 2);
}


/////////////////////////////////////////////////////////////////////////////
// test zapauzování zvuku

bool FSoundTestPause()
{
	return SoundPausing;
}


/***************************************************************************\
*																			*
*							obsluha hudby									*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// test přehrávání hudby

bool FMusicTest()
{
	return MusicPlaing;
}


/////////////////////////////////////////////////////////////////////////////
// test zapauzování hudby

bool FMusicTestPause()
{
	return MusicPausing;
}


/***************************************************************************\
*																			*
*							obsluha CD disku								*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// test pauzy CD

bool FCDGetPause()
{
// aktualizace údajů o CD
	CDAkt();

// příznak pauzy CD
	return CDPausing;
}


/////////////////////////////////////////////////////////////////////////////
// test přehrávání CD

bool FCDTest()
{
// aktualizace údajů o CD
	CDAkt();

// příznak přehrávání CD
	return CDPlaing;
}


/////////////////////////////////////////////////////////////////////////////
// test vysunutí CD

bool FCDGetEject()
{
// aktualizace údajů o CD
	CDAkt();

// příznak vysunutí CD
	return !CDDiskValid;
}


/***************************************************************************\
*																			*
*							obsluha souborů									*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// vytvoření souboru

bool FBoolFileNew()
{
// uzavření souborů
	FileClose();

// načtení jména adresáře
	CString jmeno;
	FFileName(jmeno);

// jméno musí být zadáno a nesmí končit znakem "\"
	if (jmeno.IsNotEmpty() && (jmeno[jmeno.Length()-1] != '\\'))
	{

// první pokus o vytvoření souboru
		HANDLE file = ::CreateFile(jmeno, GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL);

		if (file != INVALID_HANDLE_VALUE)
		{
			::CloseHandle(file);
			return true;
		}

// kontrola, zda soubor již existuje
		DWORD atr = ::GetFileAttributes(jmeno);
		if ((int)atr != -1)
		{
			if ((atr & FILE_ATTRIBUTE_DIRECTORY) == 0) return true;
		}
		else
		{

// vytvoření cesty k souboru
			int prev = jmeno.RevFind('\\');
			if (prev > 0)
			{
				if (CreatePath(jmeno.Left(prev)))
				{

// druhý pokus o vytvoření souboru
					file = ::CreateFile(jmeno, GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
						FILE_ATTRIBUTE_NORMAL, NULL);

					if (file != INVALID_HANDLE_VALUE)
					{
						::CloseHandle(file);
						return true;
					}
				}
			}
		}
	}

// chyba operace
	FileError = true;
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření adresáře

bool FBoolDirNew()
{
// uzavření souborů
	FileClose();

// načtení jména adresáře
	CString jmeno;
	FFileName(jmeno);

// vytvoření adresáře
	if (jmeno.IsEmpty() || !CreatePath(jmeno))
	{
		FileError = true;
		return false;
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// zrušení souboru/adresáře

bool FBoolFileDelete()
{
// uzavření souborů
	FileClose();

// načtení jména souboru/adresáře
	CString jmeno;
	FFileName(jmeno);

// zrušení souboru nebo adresáře
	if (jmeno.IsEmpty() ||
		(!::DeleteFile(jmeno) &&
		!::RemoveDirectory(jmeno)))
	{
		
		FileError = true;
		return false;
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// test existence disku/souboru/adresáře

bool FFileTest()
{
// načtení jména disku/adresáře/souboru
	CString jmeno;
	FFileName(jmeno);

// délka jména
	int delka = jmeno.Length();
	if (delka <= 0) return false;

// kontroluje se jenom disk, příp. root
	if (((delka == 2) && (jmeno[1] == ':')) ||
		((delka == 3) && (jmeno[1] == ':') && (jmeno[2] == '\\')))
	{
		TCHAR disk = jmeno[0];
		if ((disk >= 'a') && (disk < 'a'+32)) disk = (TCHAR)(disk + 'A' - 'a');
		if ((disk < 'A') || (disk >= 'A'+32)) return false;

		disk -= 'A';
		if ((::GetLogicalDrives() & (1 << disk)) == 0) return false;

		if (delka < 3) return true;

		DWORD comp;
		DWORD syst;
		TCHAR name[20];

		return (::GetVolumeInformation(jmeno, NULL, 0, NULL,
			&comp, &syst, name, 20) != FALSE);
	}

// kontroluje se adresář/soubor
	WIN32_FIND_DATA wfd;
	HANDLE find = ::FindFirstFile(jmeno, &wfd);
	if (find == INVALID_HANDLE_VALUE) return false;
	::FindClose(find);
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// kopie souborů

bool FBoolFileCopy()
{
// uzavření otevřených souborů
	FileClose();

// jména obou souborů musí být platná
	if (FileRead.IsNotEmpty() && FileWrite.IsNotEmpty())
	{

// nulování ukazatelů v souborech (kdyby se změnily)
		FileReadOff = 0;
		FileWriteOff = 0;

// provedení kopie souborů
		if (::CopyFile(FileRead, FileWrite, TRUE)) return true;
	}

// chyba operace
	FileError = true;
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// přesun souborů

bool FBoolFileMove()
{
// uzavření otevřených souborů
	FileClose();

// jména obou souborů musí být platná
	if (FileRead.IsNotEmpty() && FileWrite.IsNotEmpty())
	{

// nulování ukazatelů v souborech (kdyby se změnily)
		FileReadOff = 0;
		FileWriteOff = 0;

// provedení přesunu souborů
		if (::MoveFile(FileRead, FileWrite)) return true;
	}

// chyba operace
	FileError = true;
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// načtení atributu souboru

bool _fastcall FGetFileAttrib(DWORD attrib)
{
// uzavření otevřených souborů
	FileClose();

// musí být nějaké jméno souboru
	if (FileRead.IsNotEmpty())
	{

// načtení atributů souboru
		DWORD atr = ::GetFileAttributes(FileRead);
		if ((int)atr != -1) return ((atr & attrib) != 0);
	}

// chyba operace
	FileError = true;
	return false;
}

bool FGetFileRO()	{ return FGetFileAttrib(FILE_ATTRIBUTE_READONLY); }
bool FGetFileHID()	{ return FGetFileAttrib(FILE_ATTRIBUTE_HIDDEN); }
bool FGetFileSYS()	{ return FGetFileAttrib(FILE_ATTRIBUTE_SYSTEM); }
bool FGetFileDIR()	{ return FGetFileAttrib(FILE_ATTRIBUTE_DIRECTORY); }
bool FGetFileARC()	{ return FGetFileAttrib(FILE_ATTRIBUTE_ARCHIVE); }
bool FGetFileTMP()	{ return FGetFileAttrib(FILE_ATTRIBUTE_TEMPORARY); }
bool FGetFileCMP()	{ return FGetFileAttrib(FILE_ATTRIBUTE_COMPRESSED); }
bool FGetFileOFF()	{ return FGetFileAttrib(FILE_ATTRIBUTE_OFFLINE); }


/////////////////////////////////////////////////////////////////////////////
// načtení logické hodnoty

bool FGetFileLogic() { return (FileReadByte() != 0); }


/////////////////////////////////////////////////////////////////////////////
// poskytnutí příznaku chyby souborů

bool FGetFileError() { return FileError; }


/////////////////////////////////////////////////////////////////////////////
// poskytnutí příznaku textu UNICODE

bool FGetFileTextUnicode() { return FileTextUnicode; }


/***************************************************************************\
*																			*
*								obsluha plochy								*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// načtení přepínače z plochy

bool _fastcall FGetMapSwc(DWORD mask)
{
// načtení plochy
	CMap map;
	FMap(map);

// načtení souřadnic
	int x = FIntR();
	int y = FIntR();

// přepínač na pozici
	if (((DWORD)x < (DWORD)map.Width()) && ((DWORD)y < (DWORD)map.Height()))
	{
		return ((map.At(x, y).Param & mask) != 0);
	}
	return false;
}

bool FGetMapSwc1() { return FGetMapSwc(MAP_FLAG1); };		// přepínač 1
bool FGetMapSwc2() { return FGetMapSwc(MAP_FLAG2); };		// přepínač 2
bool FGetMapSwc3() { return FGetMapSwc(MAP_FLAG3); };		// přepínač 3
bool FGetMapSwc4() { return FGetMapSwc(MAP_FLAG4); };		// přepínač 4
bool FGetMapSwc5() { return FGetMapSwc(MAP_FLAG5); };		// přepínač 5


/***************************************************************************\
*																			*
*									konzola									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// režim konzoly

bool FGetConsoleOn() { return ConsoleOn; }


/***************************************************************************\
*																			*
*									joystick								*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// tlačítko 1

bool FJoystick1()
{
	int inx = FIntR();
	if ((DWORD)inx >= (DWORD)MAXJOYSTICK) return false;
	AktJoystick(inx);
	return Joystick1[inx];
}

/////////////////////////////////////////////////////////////////////////////
// tlačítko 2

bool FJoystick2()
{
	int inx = FIntR();
	if ((DWORD)inx >= (DWORD)MAXJOYSTICK) return false;
	AktJoystick(inx);
	return Joystick2[inx];
}

/////////////////////////////////////////////////////////////////////////////
// tlačítko 3

bool FJoystick3()
{
	int inx = FIntR();
	if ((DWORD)inx >= (DWORD)MAXJOYSTICK) return false;
	AktJoystick(inx);
	return Joystick3[inx];
}

/////////////////////////////////////////////////////////////////////////////
// tlačítko 4

bool FJoystick4()
{
	int inx = FIntR();
	if ((DWORD)inx >= (DWORD)MAXJOYSTICK) return false;
	AktJoystick(inx);
	return Joystick4[inx];
}


/***************************************************************************\
*																			*
*									dialogy									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// viditelnost okna

bool FGetWindowVisible()
{
	if (Win.IsValid(WindowID))
	{
		return Win[WindowID].Show;
	}
	else
	{
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////
// zákaz okna

bool FGetWindowDisable()
{
	if (Win.IsValid(WindowID))
	{
		return Win[WindowID].Disable;
	}
	else
	{
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////
// kliknutí na tlačítko

bool FButtonClick()
{
	if (Win.IsValid(WindowID))
	{
		bool click = Win[WindowID].Click;
		Win[WindowID].Click = false;
		return click;
	}
	else
	{
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////
// stav přepínače

bool FGetCheckBoxOn()
{
	if (Win.IsValid(WindowID))
	{
		return Win[WindowID].Check;
	}
	else
	{
		return false;
	}
}

bool FGetButton3On()
{
	if (Win.IsValid(WindowID))
	{
		return Win[WindowID].Check3;
	}
	else
	{
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////
// zvýrazněné písmo

bool FGetFontBold()
{
	if (Win.IsValid(WindowID))
	{
		return Win[WindowID].Bold;
	}
	else
	{
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////
// písmo s pevnou roztečí

bool FGetFontFixed()
{
	if (Win.IsValid(WindowID))
	{
		return Win[WindowID].Fixed;
	}
	else
	{
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////
// šikmé písmo

bool FGetFontItalic()
{
	if (Win.IsValid(WindowID))
	{
		return Win[WindowID].Italic;
	}
	else
	{
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////
// podtržené písmo

bool FGetFontUnder()
{
	if (Win.IsValid(WindowID))
	{
		return Win[WindowID].Underline;
	}
	else
	{
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////
// přeškrtnuté písmo

bool FGetFontStrike()
{
	if (Win.IsValid(WindowID))
	{
		return Win[WindowID].Strikeout;
	}
	else
	{
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////
// patičkové písmo

bool FGetFontSerif()
{
	if (Win.IsValid(WindowID))
	{
		return Win[WindowID].Serif;
	}
	else
	{
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////
// celoobrazovkový režim

bool FGetFullScreen()
{
	return FullScreen;
}

/////////////////////////////////////////////////////////////////////////////
// načtení přepínače rámečku okna

bool FGetWindowFrame()
{
	if (Win.IsValid(WindowID))
	{
		return Win[WindowID].HasBorder;
	}
	else
	{
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////
// načtení přepínače nadpisu okna

bool FGetWindowCaption()
{
	if (Win.IsValid(WindowID))
	{
		return Win[WindowID].HasCaption;
	}
	else
	{
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////
// načtení přepínače okna vždy nahoře

bool FGetWindowTop()
{
	if (Win.IsValid(WindowID))
	{
		return Win[WindowID].AlwaysTop;
	}
	else
	{
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////
// načtení přepínače okna měnitelné velikosti

bool FGetWindowSize()
{
	if (Win.IsValid(WindowID))
	{
		return Win[WindowID].HasSize;
	}
	else
	{
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////
// požadavek o uzavření okna

bool FGetWindowMayClose()
{
	bool reqclose = ReqClose;
	ReqClose = false;
	return reqclose;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění maximalizace okna

bool FGetWindowMaxim()
{
	return (::IsZoomed(Win[DialogParent].Wnd) != 0);
}

/////////////////////////////////////////////////////////////////////////////
// zjištění minimalizace okna

bool FGetWindowMinim()
{
	return (::IsIconic(Win[DialogParent].Wnd) != 0);
}

/***************************************************************************\
*																			*
*									DirectPlay								*
*																			*
\***************************************************************************/

// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI

/////////////////////////////////////////////////////////////////////////////
// zjištění přepínače hry 1

bool FGetGameL1()
{
	return (((WORD*)DirectPlayGamesParam)[0] >= 0x8000);
}

/////////////////////////////////////////////////////////////////////////////
// zjištění přepínače hry 2

bool FGetGameL2()
{
	return (((WORD*)DirectPlayGamesParam)[1] >= 0x8000);
}

/////////////////////////////////////////////////////////////////////////////
// zjištění přepínače hry 3

bool FGetGameL3()
{
	return (((WORD*)DirectPlayGamesParam)[2] >= 0x8000);
}

/////////////////////////////////////////////////////////////////////////////
// zjištění přepínače hry 4

bool FGetGameL4()
{
	return (((WORD*)DirectPlayGamesParam)[3] >= 0x8000);
}

/////////////////////////////////////////////////////////////////////////////
// zjištění přepínače hry 5

bool FGetGameL5()
{
	return (((WORD*)DirectPlayGamesParam)[4] >= 0x8000);
}

/////////////////////////////////////////////////////////////////////////////
// zjištění přepínače hry 6

bool FGetGameL6()
{
	return (((WORD*)DirectPlayGamesParam)[5] >= 0x8000);
}

/////////////////////////////////////////////////////////////////////////////
// zjištění přepínače hry 7

bool FGetGameL7()
{
	return (((WORD*)DirectPlayGamesParam)[6] >= 0x8000);
}

/////////////////////////////////////////////////////////////////////////////
// zjištění přepínače hry 8

bool FGetGameL8()
{
	return (((WORD*)DirectPlayGamesParam)[7] >= 0x8000);
}

/////////////////////////////////////////////////////////////////////////////
// zjištění příznaku hostitele

bool FGameHost()
{
	return DirectPlayGamesHost;
}

/////////////////////////////////////////////////////////////////////////////
// načtení přepínače z paketu

bool FGetGameDataL()
{
	BYTE log;
	DirectReceiveData(&log, 1);
	return (log != 0);
}

/***************************************************************************\
*																			*
*									Porty									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// zjištění stopbitů

bool FGetComStop()
{
	COMITEM* item = Coms + ComAkt;
	if (item->File != INVALID_HANDLE_VALUE)
	{
		return (item->Dcb.StopBits != ONESTOPBIT);
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// test přijatého znaku

bool FComIn()
{
	COMITEM* item = Coms + ComAkt;
	HANDLE file = item->File;

	if (file != INVALID_HANDLE_VALUE)
	{
		COMSTAT cs;
		DWORD flags;
		if (::ClearCommError(file, &flags, &cs))
		{
			if (cs.cbInQue > 0) return true;
		}
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// test povolení vysílání, protějšek je připraven přijímat data (signál CTS)

bool FGetComSend()
{
	COMITEM* item = Coms + ComAkt;
	HANDLE file = item->File;

	if (file != INVALID_HANDLE_VALUE)
	{
		DWORD flags;
		if (::GetCommModemStatus(file, &flags))
		{
			if (flags & MS_CTS_ON) return true;
		}
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// test povolení příjmu, data vysílaná od protějšku jsou platná (signál DSR)

bool FGetComReceive()
{
	COMITEM* item = Coms + ComAkt;
	HANDLE file = item->File;

	if (file != INVALID_HANDLE_VALUE)
	{
		DWORD flags;
		if (::GetCommModemStatus(file, &flags))
		{
			if (flags & MS_DSR_ON) return true;
		}
	}
	return false;
}

#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------

/////////////////////////////////////////////////////////////////////////////
// zjištění módu grafického pozadí

bool FGetDialogGraph()
{
	return DialogGraph;
}

/***************************************************************************\
*																			*
*									Direct3D								*
*																			*
\***************************************************************************/

// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI

/////////////////////////////////////////////////////////////////////////////
// zjištění nastavení vyplňování ploch

bool FGetD3DWireframe()
{
	return D3DWireframeGlobal;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění nastavení osvětlení

bool FGetD3DLighton()
{
	return D3DLightOnGlobal;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění nastavení plynulého stínování

bool FGetD3DShades()
{
	return D3DShadesGlobal;
}
		   
/////////////////////////////////////////////////////////////////////////////
// zjištění filtrace zmenšených textur

bool FGetD3DMinFilter()
{
	return D3DMinFilterGlobal;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění filtrace zvětšených textur

bool FGetD3DMagFilter()
{
	return D3DMagFilterGlobal;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění filtrace vzdálených textur

bool FGetD3DMipFilter()
{
	return D3DMipFilterGlobal;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění nastavení vyplňování ploch objektu

bool FGetD3DOWireframe()
{
	if (IsValidID())
	{
		return GetID()->Wireframe;
	}
	return FGetD3DWireframe();
}

/////////////////////////////////////////////////////////////////////////////
// zjištění nastavení osvětlení objektu

bool FGetD3DOLighton()
{
	if (IsValidID())
	{
		return GetID()->Lighton;
	}
	return FGetD3DLighton();
}

/////////////////////////////////////////////////////////////////////////////
// zjištění nastavení plynulého stínování objektu

bool FGetD3DOShades()
{
	if (IsValidID())
	{
		return GetID()->Shades;
	}
	return FGetD3DShades();
}
		   
/////////////////////////////////////////////////////////////////////////////
// zjištění filtrace zmenšených textur objektu

bool FGetD3DOMinFilter()
{
	if (IsValidID())
	{
		return GetID()->MinFilter[D3DAktStage];
	}
	return FGetD3DMinFilter();
}

/////////////////////////////////////////////////////////////////////////////
// zjištění filtrace zvětšených textur objektu

bool FGetD3DOMagFilter()
{
	if (IsValidID())
	{
		return GetID()->MagFilter[D3DAktStage];
	}
	return FGetD3DMagFilter();
}

/////////////////////////////////////////////////////////////////////////////
// zjištění filtrace vzdálených textur objektu

bool FGetD3DOMipFilter()
{
	if (IsValidID())
	{
		return GetID()->MipFilter[D3DAktStage];
	}
	return FGetD3DMipFilter();
}

/////////////////////////////////////////////////////////////////////////////
// tvořit horní podstavu

bool FGetD3DUpper()
{
	return D3DCreateUpper;
}

/////////////////////////////////////////////////////////////////////////////
// tvořit dolní podstavu

bool FGetD3DLower()
{
	return D3DCreateLower;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění viditelnosti objektu Direct3D

bool FGetD3DVisible()
{
	if (IsValidID())
	{
		return GetID()->Visible;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění hloubkového zápisu

bool FGetD3DZWrite()
{
	if (IsValidID())
	{
		return (GetID()->ZWrite != FALSE);
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění hloubkového třídění

bool FGetD3DZSort()
{
	if (IsValidID())
	{
		return (RendSort[GetID()->RendGroup] != FALSE);
	}
	return false;
}

#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------


/////////////////////////////////////////////////////////////////////////////
// test, zda to je aktivní aplikace

bool FGetActive()
{
	return AppActive;
}

/////////////////////////////////////////////////////////////////////////////
// tisk obrázku

bool FPicPrint() 
{ 
// načtení obrázku k tisku
	CPicture pic;
	FPicture(pic);
	if ((pic.Width() == 0) || (pic.Height() == 0)) return false;

// nastavení tiskárny
	PRINTDLG pd;
	MemFill(&pd, sizeof(PRINTDLG), 0);
	pd.lStructSize = sizeof(PRINTDLG);
	pd.Flags = PD_RETURNDC | PD_PRINTSETUP;
	pd.hwndOwner = Win[DialogParent].Wnd;

	BOOL res = ::PrintDlg(&pd);
	if (pd.hDevMode) ::GlobalFree (pd.hDevMode);
	if (pd.hDevNames) :: GlobalFree (pd.hDevNames);
	if (!res || (pd.hDC == NULL)) return false;

// zahájení tisku
	DOCINFO di;
	MemFill(&di, sizeof(DOCINFO), 0);
	di.cbSize = sizeof(DOCINFO);
	di.lpszDocName = Win[DialogParent].Text;
	::StartDoc(pd.hDC, &di);
	::StartPage(pd.hDC);

// tisk obrázku
	pic.DeComp();

	StdBitmapInfo->bmiHeader.biWidth = pic.Width();
	StdBitmapInfo->bmiHeader.biHeight = pic.Height();

	int horzres = ::GetDeviceCaps(pd.hDC, HORZRES);	// šířka papíru v tiskových bodech
	int horzres0 = Round(horzres * 0.95);			// užitečná šířka papíru v bodech
	int vertres = ::GetDeviceCaps(pd.hDC, VERTRES); // výška papíru v tiskových bodech
	int vertres0 = Round(vertres * 0.95);			// užitečná výška papíru v bodech

	int scale = 1;									// měřítko * 4

	while ((((pic.Width() * (scale+1) + 2)/4) <= horzres0) &&
		(((pic.Height() * (scale+1) + 2)/4) <= vertres0))
	{
		scale++;
	}

	int width2 = (pic.Width() * scale + 2) / 4;
	int height2 = (pic.Height() * scale + 2) / 4;

	int left = (horzres - width2)/2;
	if (left < 0) left = 0;

	int top = 0;
	if (vertres < horzres)
	{
		top = (vertres - height2)/2;
		if (top < 0) top = 0;
	}

	::StretchDIBits(pd.hDC,
		left, top, width2, height2,					// cílové parametry
		0, 0, pic.Width(), pic.Height(),			// zdrojové parametry
		pic.DataData(),								// data obrázku
		StdBitmapInfo, DIB_RGB_COLORS, SRCCOPY);

// ukončení tisku
	::EndPage(pd.hDC);
	::EndDoc(pd.hDC);
	::DeleteDC(pd.hDC);

	return true;
}


/***************************************************************************\
*																			*
*								obsluha DLL									*
*																			*
\***************************************************************************/

// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI

/////////////////////////////////////////////////////////////////////////////
// logická hodnota

bool FGetMemoryBool()
{
	BOOL* buf = (BOOL*)DLLMemoryRead;
	bool data = (*buf != FALSE);
	buf++;
	DLLMemoryRead = buf;
	return data;
}


#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------
