
/***************************************************************************\
*																			*
*						Provádění programu - logický výraz					*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// inline volání příkazu - logický výraz

inline bool FBool()
{
	ExecItem++;
	return ExecItem[-1].ExecLog();
}


// funkce
bool FBoolFunc();				// funkce s návratem logické hodnoty

// proměnné
bool FTrue();					// konstanta TRUE
bool FFalse();					// konstanta FALSE
bool FBoolObj();				// globální proměnná (Data = index)
bool FBoolObjList();			// globalní proměnná v seznamu (Data = index, List = seznam)
bool FBoolLoc();				// lokální proměnná (Data = index)
bool FBoolLocList();			// lokální proměnná v seznamu (Data = index, List = seznam)
bool FGetSpriteMove();			// pohyb globálního sprajtu (Data = index)
bool FGetSpriteMoveList();		// pohyb sprajtu v seznamu (Data = index, List = seznam)
bool FGetSpriteMoveLoc();		// pohyb lokálního sprajtu (Data = index)
bool FGetSpriteMoveLocList();	// pohyb lokálního sprajtu v seznamu (Data = index, List = seznam)
bool FGetSpriteVis();			// viditelnost globálního sprajtu (Data = index)
bool FGetSpriteVisList();		// viditelnost sprajtu v seznamu (Data = index, List = seznam)
bool FGetSpriteVisLoc();		// viditelnost lokálního sprajtu (Data = index)
bool FGetSpriteVisLocList();	// viditelnost lokálního sprajtu v seznamu (Data = index, List = seznam)

// matematické operátory
bool FEqu1();					// operátor EQU - pro 1 prvek (porovná číslo s nulou)
bool FEqu();					// operátor EQU (Data = počet prvků - 1)
bool FNEqu1();					// operátor NEQU - pro 1 prvek (porovná číslo s nulou)
bool FNEqu();					// operátor NEQU (Data = počet prvků - 1)
bool FEqGr1();					// operátor EQGR - pro 1 prvek (porovná číslo s nulou)
bool FEqGr();					// operátor EQGR (Data = počet prvků - 1)
bool FEqLt1();					// operátor EQLT - pro 1 prvek (porovná číslo s nulou)
bool FEqLt();					// operátor EQLT (Data = počet prvků - 1)
bool FGr1();					// operátor GR - pro 1 prvek (porovná číslo s nulou)
bool FGr();						// operátor GR (Data = počet prvků - 1)
bool FLt1();					// operátor LT - pro 1 prvek (porovná číslo s nulou)
bool FLt();						// operátor LT (Data = počet prvků - 1)

// logické operátory
bool FAnd1();					// operátor AND - pro 1 prvek (jenom vrátí hodnotu beze změny)
bool FAnd();					// operátor AND (Data = počet prvků - 1)
inline bool FOr1() { return FAnd1(); }	// operátor OR - pro 1 prvek (jenom vrátí hodnotu beze změny)
bool FOr();						// operátor OR (Data = počet prvků - 1)
inline bool FXor1() { return FAnd1(); }	// operátor XOR - pro 1 prvek (jenom vrátí hodnotu beze změny)
bool FXor();					// operátor XOR (Data = počet prvků - 1)
bool FNot();					// operátor NOT

// obsluha ikon
bool FCompEqIco1();				// porovnání jedné ikony, zda je prázdná
bool FCompEqIco();				// porovnání shodnosti ikon

bool FCompEqMap1();				// porovnání jedné plochy, zda je prázdná
bool FCompEqMap();				// porovnání shodnosti ploch
bool FCompEqPic1();				// porovnání jednoho obrázku, zda je prázdný
bool FCompEqPic();				// porovnání shodnosti obrázků
bool FCompEqSpr1();				// porovnání jednoho sprajtu, zda je prázdný
bool FCompEqSpr();				// porovnání shodnosti sprajtů
bool FCompEqSnd1();				// porovnání jednoho zvuku, zda je prázdný
bool FCompEqSnd();				// porovnání shodnosti zvuků
bool FCompEqMus1();				// porovnání jedné hudby, zda je prázdná
bool FCompEqMus();				// porovnání shodnosti hudeb

// obsluha textů
bool FCompEqTxt1();				// porovnání jednoho textu, zda je prázdný
bool FCompEqTxt();				// porovnání textů na shodnost (Data = počet textů - 1)
bool FCompGtTxt1();				// porovnání jednoho textu, zda je neprázdný
bool FCompGtTxt();				// porovnání textů na neshodnost (Data = počet textů - 1)

// obsluha Petra a Petry
bool FGetVisible();				// načtení viditelnosti Petra
bool FGetVisible2();			// načtení viditelnosti Petry

bool FGetSwc1();				// přepínač 1 - Petr
bool FGetSwc2();				// přepínač 2 - Petr
bool FGetSwc3();				// přepínač 3 - Petr
bool FGetSwc4();				// přepínač 4 - Petr
bool FGetSwc5();				// přepínač 5 - Petr

bool FGetSwc12();				// přepínač 1 - Petra
bool FGetSwc22();				// přepínač 2 - Petra
bool FGetSwc32();				// přepínač 3 - Petra
bool FGetSwc42();				// přepínač 4 - Petra
bool FGetSwc52();				// přepínač 5 - Petra

// obsluha klávesnice
bool FKeyTest();				// test stisku klávesy
bool FStatusNumLock();			// test stavu Num Lock
bool FStatusCapsLock();			// test stavu Caps Lock
bool FStatusScrollLock();		// test stavu Scroll Lock
bool FStatusInsert();			// test stavu Insert

// obsluha myši
bool FMouseL();					// test stisku levého tlačítka myši
bool FMouseR();					// test stisku pravého tlačítka myši
bool FMouseLClick();			// test kliku levým tlačítkem myši
bool FMouseRClick();			// test kliku pravým tlačítkem myši
bool FMouseLDClick();			// test dvojkliku levým tlačítkem myši
bool FMouseRDClick();			// test dvojkliku pravým tlačítkem myši
bool FMouseRegion();			// test myši v oblasti
bool FMouseItem();				// test myši v dialogovém prvku

// obsluha zvuku
bool FSoundTest();				// test přehrávání jakéhokoliv zvuku
bool FSoundTest1();				// test přehrávání konkrétního zvuku
bool FGetSoundStereo();			// příznak stereo
bool FSoundTestPause();			// test zapauzování zvuku

// obsluha hudby
bool FMusicTest();				// test přehrávání hudby
bool FMusicTestPause();			// test zapauzování hudby

// obsluha CD disků
bool FCDGetPause();				// test pauzy CD
bool FCDTest();					// test přehrávání CD
bool FCDGetEject();				// test vysunutí CD

// obsluha souborů
bool FBoolFileNew();			// vytvoření souboru
bool FBoolDirNew();				// vytvoření adresáře
bool FBoolFileDelete();			// zrušení souboru/adresáře
bool FFileTest();				// test existence disku/souboru/adresáře
bool FBoolFileCopy();			// kopie souborů
bool FBoolFileMove();			// přesun souborů
bool FGetFileRO();				// načtení atributu R/O souboru
bool FGetFileHID();				// načtení atributu HID souboru
bool FGetFileSYS();				// načtení atributu SYS souboru
bool FGetFileDIR();				// načtení atributu DIR souboru
bool FGetFileARC();				// načtení atributu ARC souboru
bool FGetFileTMP();				// načtení atributu TMP souboru
bool FGetFileCMP();				// načtení atributu CMP souboru
bool FGetFileOFF();				// načtení atributu OFF souboru
bool FGetFileError();			// poskytnutí příznaku chyby souborů
bool FGetFileTextUnicode();		// poskytnutí příznaku textu UNICODE
bool FGetFileLogic();			// načtení logické hodnoty

// obsluha plochy
bool FGetMapSwc1();				// načtení přepínače 1 z plochy
bool FGetMapSwc2();				// načtení přepínače 2 z plochy
bool FGetMapSwc3();				// načtení přepínače 3 z plochy
bool FGetMapSwc4();				// načtení přepínače 4 z plochy
bool FGetMapSwc5();				// načtení přepínače 5 z plochy

// konzola
bool FGetConsoleOn();			// režim konzoly

// joystick
bool FJoystick1();				// tlačítko 1
bool FJoystick2();				// tlačítko 2
bool FJoystick3();				// tlačítko 3
bool FJoystick4();				// tlačítko 4

// dialogy
//bool FGetDialogOn();			// režim dialogu
bool FGetWindowVisible();		// viditelnost okna
bool FGetWindowDisable();		// zákaz okna
bool FButtonClick();			// kliknutí na tlačítko
bool FGetCheckBoxOn();			// stav přepínače
bool FGetButton3On();
bool FGetFontBold();			// zvýrazněné písmo
bool FGetFontFixed();			// písmo s pevnou roztečí
bool FGetFontItalic();			// šikmé písmo
bool FGetFontUnder();			// podtržené písmo
bool FGetFontStrike();			// přeškrtnuté písmo
bool FGetFontSerif();			// patičkové písmo
bool FGetFullScreen();			// celoobrazovkový režim
bool FGetWindowFrame();			// načtení přepínače rámečku okna
bool FGetWindowCaption();		// načtení přepínače nadpisu okna
bool FGetWindowTop();			// načtení přepínače okna vždy nahoře
bool FGetWindowSize();			// načtení přepínače okna měnitelné velikosti
bool FGetWindowMayClose();		// požadavek o uzavření okna
bool FGetWindowMaxim();			// zjištění maximalizace okna
bool FGetWindowMinim();			// zjištění minimalizace okna

// DirectPlay
bool FGetGameL1();				// zjištění přepínače hry 1
bool FGetGameL2();				// zjištění přepínače hry 2
bool FGetGameL3();				// zjištění přepínače hry 3
bool FGetGameL4();				// zjištění přepínače hry 4
bool FGetGameL5();				// zjištění přepínače hry 5
bool FGetGameL6();				// zjištění přepínače hry 6
bool FGetGameL7();				// zjištění přepínače hry 7
bool FGetGameL8();				// zjištění přepínače hry 8
bool FGameHost();				// zjištění příznaku hostitele
bool FGetGameDataL();			// načtení přepínače z paketu

// porty
bool FGetComStop();				// zjištění stopbitů
bool FComIn();					// test přijatého znaku
bool FGetComSend();				// test povolení vysílání
bool FGetComReceive();			// test povolení příjmu

bool FGetDialogGraph();			// zjištění módu grafického pozadí

// Direct3D
bool FGetD3DWireframe();		// zjištění nastavení vyplňování ploch
bool FGetD3DLighton();			// zjištění nastavení osvětlení
bool FGetD3DShades();			// zjištění nastavení plynulého stínování
bool FGetD3DMinFilter();		// zjištění filtrace zmenšených textur
bool FGetD3DMagFilter();		// zjištění filtrace zvětšených textur
bool FGetD3DMipFilter();		// zjištění filtrace vzdálených textur
bool FGetD3DOWireframe();		// zjištění nastavení vyplňování ploch objektu
bool FGetD3DOLighton();			// zjištění nastavení osvětlení objektu
bool FGetD3DOShades();			// zjištění nastavení plynulého stínování objektu
bool FGetD3DOMinFilter();		// zjištění filtrace zmenšených textur objektu
bool FGetD3DOMagFilter();		// zjištění filtrace zvětšených textur objektu
bool FGetD3DOMipFilter();		// zjištění filtrace vzdálených textur objektu
bool FGetD3DVisible();			// zjištění viditelnosti objektu Direct3D
bool FGetD3DUpper();			// tvořit horní podstavu
bool FGetD3DLower();			// tvořit dolní podstavu
bool FGetD3DZWrite();			// zjištění hloubkového zápisu
bool FGetD3DZSort();			// zjištění hloubkového třídění

bool FGetActive();				// test, zda to je aktivní aplikace

bool FPicPrint();				// tisk obrázku

// obsluha DLL
bool FGetMemoryBool();			// logická hodnota
