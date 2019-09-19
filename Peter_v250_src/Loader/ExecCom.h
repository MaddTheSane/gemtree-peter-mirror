
/***************************************************************************\
*																			*
*							Provádění programu - příkaz						*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// inline volání příkazu

inline void FCommand()
{
	ExecItem++;
	ExecItem[-1].ExecCom();
}

// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI

// nastavení transformací objektu
//void SetTransform(D3DFITEM* frame);

/////////////////////////////////////////////////////////////////////////////
// automatické mapování textury

void D3DTextMap(D3DFITEM* item);


#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------


// větvení CASE
void FCaseReal();				// CASE s číslem
void FCaseBool();				// CASE s logickým výrazem
void FCaseIcon();				// CASE s ikonou
void FCaseSprite();				// CASE se spritem
void FCaseMap();				// CASE s plochou
void FCaseMusic();				// CASE s hudbou
void FCaseText();				// CASE s textem
void FCaseSound();				// CASE se zvukem
void FCasePicture();			// CASE s obrázkem

// funkce
void FFunc();					// funkce s lokálními proměnnými
void FFunc0();					// funkce bez lokálních proměnných
void FDupList();				// vytvoření lokálního seznamu duplikací (Data = index vzoru)
void FDelList();				// zrušení lokálního seznamu (Data = počet)
void FDupReal();				// vytvoření lokální číselné proměnné duplikací (Data = index vzoru)
void FDupRealList();			// vytvoření lokální číselné proměnné seznamu (Data = index vzoru, List = počet)
void FNewReal();				// vytvoření lokální číselné proměnné s inicializací
void FDelReal();				// zrušení lokálních číselných proměnných (Data = počet)
void FDupBool();				// vytvoření lokální logické proměnné duplikací (Data = index vzoru)
void FDupBoolList();			// vytvoření lokální logické proměnné seznamu (Data = index vzoru, List = počet)
void FNewBool();				// vytvoření lokální logické proměnné s inicializací
void FDelBool();				// zrušení lokálních logických proměnných (Data = počet)
void FDupIcon();				// vytvoření lokální proměnné ikony duplikací (Data = index vzoru)
void FDupIconList();			// vytvoření lokální proměnné ikony seznamu (Data = index vzoru, List = počet)
void FNewIcon();				// vytvoření lokální proměnné ikony s inicializací
void FDelIcon();				// zrušení lokálních proměnných ikon (Data = počet)
void FDupSprite();				// vytvoření lokální proměnné sprajtu duplikací (Data = index vzoru)
void FDupSpriteList();			// vytvoření lokální proměnné sprajtu seznamu (Data = index vzoru, List = počet)
void FNewSprite();				// vytvoření lokální proměnné sprajtu s inicializací
void FDelSprite();				// zrušení lokálních proměnných sprajtů (Data = počet)
void FDupMap();					// vytvoření lokální proměnné plochy duplikací (Data = index vzoru)
void FDupMapList();				// vytvoření lokální proměnné plochy seznamu (Data = index vzoru, List = počet)
void FNewMap();					// vytvoření lokální proměnné plochy s inicializací
void FDelMap();					// zrušení lokálních proměnných ploch (Data = počet)
void FDupText();				// vytvoření lokální proměnné textu duplikací (Data = index vzoru)
void FDupTextList();			// vytvoření lokální proměnné textu seznamu (Data = index vzoru, List = počet)
void FNewText();				// vytvoření lokální proměnné textu s inicializací
void FDelText();				// zrušení lokálních proměnných textů (Data = počet)
void FDupSound();				// vytvoření lokální proměnné zvuku duplikací (Data = index vzoru)
void FDupSoundList();			// vytvoření lokální proměnné zvuku seznamu (Data = index vzoru, List = počet)
void FNewSound();				// vytvoření lokální proměnné zvuku s inicializací
void FDelSound();				// zrušení lokálních proměnných zvuků (Data = počet)
void FDupPicture();				// vytvoření lokální proměnné obrázku duplikací (Data = index vzoru)
void FDupPictureList();			// vytvoření lokální proměnné obrázku seznamu (Data = index vzoru, List = počet)
void FNewPicture();				// vytvoření lokální proměnné obrázku s inicializací
void FDelPicture();				// zrušení lokálních proměnných obrázků (Data = počet)
void FDupMusic();				// vytvoření lokální proměnné hudby duplikací (Data = index vzoru)
void FDupMusicList();			// vytvoření lokální proměnné hudby seznamu (Data = index vzoru, List = počet)
void FNewMusic();				// vytvoření lokální proměnné hudby s inicializací
void FDelMusic();				// zrušení lokálních proměnných hudby (Data = počet)

// nastavení proměnných
void FSetList();				// nastavení indexu globálního seznamu
void FSetListLoc();				// nastavení indexu lokálního seznamu
void FSetListAuto();			// nastavení inkrementace globálního seznamu
void FSetListAutoLoc();			// nastavení inkrementace lokálního seznamu
void FSetIcon();				// nastavení globální ikony (Data = index)
void FSetIconList();			// nastavení ikony v seznamu (Data = index, List = seznam)
void FSetIconLoc();				// nastavení lokální ikony (Data = index)
void FSetIconLocList();			// nastavení lokální ikony v seznamu (Data = index, List = seznam)
void FSetLog();					// nastavení globální logické proměnné (Data = index)
void FSetLogList();				// nastavení logické proměnné v seznamu (Data = index, List = seznam)
void FSetLogLoc();				// nastavení lokální logické proměnné (Data = index)
void FSetLogLocList();			// nastavení lokální logické proměnné v seznamu (Data = index, List = seznam)
void FSetMap();					// nastavení globální plochy (Data = index)
void FSetMapList();				// nastavení plochy v seznamu (Data = index, List = seznam)
void FSetMapLoc();				// nastavení lokální plochy (Data = index)
void FSetMapLocList();			// nastavení lokální plochy v seznamu (Data = index, List = seznam)
void FSetNum();					// nastavení globálního čísla (Data = index)
void FSetNumList();				// nastavení čísla v seznamu (Data = index, List = seznam)
void FSetNumLoc();				// nastavení lokálního čísla (Data = index)
void FSetNumLocList();			// nastavení lokálního čísla v seznamu (Data = index, List = seznam)
void FSetPic();					// nastavení globálního obrázku (Data = index)
void FSetPicList();				// nastavení obrázku v seznamu (Data = index, List = seznam)
void FSetPicLoc();				// nastavení lokálního obrázku (Data = index)
void FSetPicLocList();			// nastavení lokálního obrázku v seznamu (Data = index, List = seznam)
void FSetSound();				// nastavení globálního zvuku (Data = index)
void FSetSoundList();			// nastavení zvuku v seznamu (Data = index, List = seznam)
void FSetSoundLoc();			// nastavení lokálního zvuku (Data = index)
void FSetSoundLocList();		// nastavení lokálního zvuku v seznamu (Data = index, List = seznam)
void FSetMusic();				// nastavení globální hudby (Data = index)
void FSetMusicList();			// nastavení hudby v seznamu (Data = index, List = seznam)
void FSetMusicLoc();			// nastavení lokální hudby (Data = index)
void FSetMusicLocList();		// nastavení lokální hudby v seznamu (Data = index, List = seznam)
void FSetText();				// nastavení globálního textu (Data = index)
void FSetTextList();			// nastavení textu v seznamu (Data = index, List = seznam)
void FSetTextLoc();				// nastavení lokálního textu (Data = index)
void FSetTextLocList();			// nastavení lokálního textu v seznamu (Data = index, List = seznam)
void FSetSprite();				// nastavení globálního sprajtu (Data = index)
void FSetSpriteList();			// nastavení sprajtu v seznamu (Data = index, List = seznam)
void FSetSpriteLoc();			// nastavení lokálního sprajtu (Data = index)
void FSetSpriteLocList();		// nastavení lokálního sprajtu v seznamu (Data = index, List = seznam)
void FSpriteX();				// nastavení souřadnice X globálního sprajtu (Data = index)
void FSpriteXList();			// nastavení souřadnice X sprajtu v seznamu (Data = index, List = seznam)
void FSpriteXLoc();				// nastavení souřadnice X lokálního sprajtu (Data = index)
void FSpriteXLocList();			// nastavení souřadnice X lokálního sprajtu v seznamu (Data = index, List = seznam)
void FSpriteY();				// nastavení souřadnice Y globálního sprajtu (Data = index)
void FSpriteYList();			// nastavení souřadnice Y sprajtu v seznamu (Data = index, List = seznam)
void FSpriteYLoc();				// nastavení souřadnice Y lokálního sprajtu (Data = index)
void FSpriteYLocList();			// nastavení souřadnice Y lokálního sprajtu v seznamu (Data = index, List = seznam)
void FSpriteSmer();				// nastavení směru globálního sprajtu (Data = index)
void FSpriteSmerList();			// nastavení směru sprajtu v seznamu (Data = index, List = seznam)
void FSpriteSmerLoc();			// nastavení směru lokálního sprajtu (Data = index)
void FSpriteSmerLocList();		// nastavení směru lokálního sprajtu v seznamu (Data = index, List = seznam)
void FSpriteFaze();				// nastavení fáze globálního sprajtu (Data = index)
void FSpriteFazeList();			// nastavení fáze sprajtu v seznamu (Data = index, List = seznam)
void FSpriteFazeLoc();			// nastavení fáze lokálního sprajtu (Data = index)
void FSpriteFazeLocList();		// nastavení fáze lokálního sprajtu v seznamu (Data = index, List = seznam)
void FSpriteStatus();			// nastavení fáze globálního sprajtu (Data = index)
void FSpriteStatusList();		// nastavení fáze sprajtu v seznamu (Data = index, List = seznam)
void FSpriteStatusLoc();		// nastavení fáze lokálního sprajtu (Data = index)
void FSpriteStatusLocList();	// nastavení fáze lokálního sprajtu v seznamu (Data = index, List = seznam)
void FSpriteVis();				// nastavení viditelnosti globálního sprajtu (Data = index)
void FSpriteVisList();			// nastavení viditelnosti sprajtu v seznamu (Data = index, List = seznam)
void FSpriteVisLoc();			// nastavení viditelnosti lokálního sprajtu (Data = index)
void FSpriteVisLocList();		// nastavení viditelnosti lokálního sprajtu v seznamu (Data = index, List = seznam)

// řídicí příkazy
void FIf();						// podmínka IF
void FWhile();					// cyklus WHILE
void FRepeat();					// cyklus REPEAT
void FBreak();					// přerušení cyklu
void FReturn();					// přerušení funkce
void FCommandExec0();			// prázdný příkaz
void FCommandExec1();			// 1 příkaz
void FCommandExec();			// posloupnost příkazů (Data = počet příkazů - 1)
void FWait1();					// čekání - 1 impuls
void FWait();					// čekání
void FSetStatus();				// nastavení textu stavového řádku
void FSetCaption();				// nastavení textu nadpisu okna

// obsluha dialogů
void FDialogBox();				// dialog box

// obsluha souborů
void FFileNew();				// nový soubor
void FDirNew();					// nový adresář
void FFileDelete();				// zrušení souboru/adresáře
void FFileAct();				// nastavení aktivního adresáře
void FDiskLabel();				// nastavení jmenovky disku
void FFileRead();				// soubor pro čtení
void FFileWrite();				// soubor pro zápis
void FFileCopy();				// kopie souborů
void FFileMove();				// přesun souborů
void FFileRO();					// nastavení atributu R/O
void FFileHID();				// nastavení atributu HID
void FFileSYS();				// nastavení atributu SYS
void FFileARC();				// nastavení atributu ARC
void FFileCreate();				// nastavení času vytvoření souboru
void FFileOpen();				// nastavení času čtení ze souboru
void FFileModi();				// nastavení času zápisu do souboru
void FFileSize();				// nastavení velikosti souboru
void FFileReadOff();			// obsluha souborů
void FFileWriteOff();			// nastavení offsetu pro zápis
void FFileError();				// nastavení příznaku chyby
void FFileTextNLen();			// nastavení délky textu s pevnou délkou
void FFileByteS();				// zápis bajtu se znaménkem
void FFileByteU();				// zápis bajtu bez znaménka
void FFileWordS();				// zápis slova se znaménkem
void FFileWordU();				// zápis slova bez znaménka
void FFile3ByteS();				// zápis 3 bajtů se znaménkem
void FFile3ByteU();				// zápis 3 bajtů bez znaménka
void FFileDWordS();				// zápis dvojslova se znaménkem
void FFileDWordU();				// zápis dvojslova bez znaménka
void FFileFloat();				// zápis čísla FLOAT
void FFileDouble();				// zápis čísla DOUBLE
void FFileLogic();				// zápis logické hodnoty
void FFileText0();				// zápis textu zakončeného nulou
void FFileTextCR();				// zápis textu zakončeného CR/LF
void FFileTextN();				// zápis textu s pevnou délkou
void FFileTextUnicode();		// nastavení příznaku textu UNICODE
void FFileIcon();				// zápis ikony
void FFilePicture();			// zápis obrázku
void FFileSound();				// zápis zvuku
void FFileMusic();				// zápis hudby
void FFileSprite();				// zápis sprajtu

// obsluha času
void FTimeCurrent();			// nastavení aktuálního času

// obsluha CD disku
void FCDPlay();					// přehrátí CD 
void FCDStop();					// zastavení přehrávání CD
void FCDPause();				// pauza CD
void FCDPauseSet();				// pauza CD
void FCDSetPos();				// nastavení pozice CD
void FCDSetTrack();				// nastavení stopy CD
void FCDSetTrackPos();			// nastavení pozice stopy CD
void FCDEject();				// vysunutí CD
void FCDEjectSet();				// vysunutí CD

// obsluha hudby
void FMusicPlay();				// přehrátí hudby
void FMusicLoop();				// opakované přehrávání hudby
void FMusicStop();				// zastavení přehrávání hudby
void FMusicPause();				// pauza hudby se změnou
void FMusicPauseSet();			// pauza hudby s nastavením
void FSetMusicPos();			// nastavení pozice hudby

// obsluha Petra a Petry
//void PeterWait();				// Petr - čekání na dokončení pohybu
//void Peter2Wait();				// Petra - čekání na dokončení pohybu
void FKrok();					// Petr - krok vpřed
void FKrok2();					// Petra - krok vpřed
void FVlevo();					// Petr - vlevo
void FVlevo2();					// Petra - vlevo
void FVpravo();					// Petr - vpravo
void FVpravo2();				// Petra - vpravo
void FVzad();					// Petr - vzad
void FVzad2();					// Petra - vzad
void FVisible();				// Petr - viditelnost
void FVisible2();				// Petra - viditelnost
void FSetPredmet();				// nastavení předmětu před Petrem
void FSetPredmet2();			// nastavení předmětu před Petrou
void FSetPredmetPoz();			// nastavení předmětu pod Petrem
void FSetPredmet2Poz();			// nastavení předmětu pod Petrou
void FSetSmer();				// Petr - nastavení směru
void FSetSmer2();				// Petra - nastavení směru
void FSetPozX();				// Petr - nastavení souřadnice X
void FSetPozX2();				// Petra - nastavení souřadnice X
void FSetPozY();				// Petr - nastavení souřadnice Y
void FSetPozY2();				// Petra - nastavení souřadnice Y
void FSetSwcN1();				// Petr - nastavení čísla 1
void FSetSwcN2();				// Petr - nastavení čísla 2
void FSetSwcN3();				// Petr - nastavení čísla 3
void FSetSwcN12();				// Petr2 - nastavení čísla 1
void FSetSwcN22();				// Petr2 - nastavení čísla 2
void FSetSwcN32();				// Petr2 - nastavení čísla 3
void FSetSwc1();				// Petr - nastavení přepínače 1
void FSetSwc2();				// Petr - nastavení přepínače 2
void FSetSwc3();				// Petr - nastavení přepínače 3
void FSetSwc4();				// Petr - nastavení přepínače 4
void FSetSwc5();				// Petr - nastavení přepínače 5
void FSetSwc12();				// Petra - nastavení přepínače 1
void FSetSwc22();				// Petra - nastavení přepínače 2
void FSetSwc32();				// Petra - nastavení přepínače 3
void FSetSwc42();				// Petra - nastavení přepínače 4
void FSetSwc52();				// Petra - nastavení přepínače 5

// obsluha zvuku
void FSoundPlay();				// přehrátí zvuku
void FSoundStop();				// zastavení všech zvuků
void FSoundStop1();				// zastavení konkrétního zvuku
void FSetSoundPos();			// nastavení pozice zvuku
void FSoundPause();				// pauza zvuku
void FSoundPauseSet();			// pauza zvuku
void FSetSoundDistinguish();	// nastavení rozlišovacího kódu

// obsluha grafiky
extern int PointWidth;			// nastavená šířka grafického bodu
extern BYTE PointCol;			// nastavená barva grafického bodu
void _fastcall CirclePoint(int x, int y); // zobrazení bodu nebo kruhu
void FPoint();					// zobrazení bodu
void FLine();					// zobrazení čáry
void FBox();					// zobrazení obdélníku
void FBoxFill();				// zobrazení vyplněného obdélníku
void FFill();					// výplň
void FCircle();					// kružnice
void FCircleFill();				// kruh
void FSphere();					// koule
void FTriangle();				// vyplněný trojůhelník
void FGraphText();				// zobrazení textu
void FSetGraphAreaWidth();		// nastavení šířky pole k zobrazení centrovaného textu
void FPicDisp();				// zobrazení obrázku
void FTransparent();			// zprůhlednění výřezu

// obsluha klávesnice
void FComVKeyWait();			// čekání na klávesu
void FComCharWait();			// čekání na snak z klávesnice
void FKlavFlush();				// vyprázdnění bufferu kláves
void FSetNumLock();				// nastavení přepínače Num Lock
void FSetCapsLock();			// nastavení přepínače Caps Lock
void FSetScrollLock();			// nastavení přepínače Scroll Lock
void FSetInsertLock();			// nastavení přepínače Insert

void FSpriteShiftX();			// hromadný posun sprajtů ve směru X
void FSpriteShiftY();			// hromadný posun sprajtů ve směru Y

// okno k zobrazení sprajtů
void FSpriteWinX1();
void FSpriteWinY1();
void FSpriteWinX2();
void FSpriteWinY2();

// konzola
void FConsoleOn();				// režim konzoly
void FConsoleOut();				// výstup textu na konzolu
void FConsoleErr();				// výstup chybového textu na konzolu

void FSetReturnCode();			// návratový kód

void FExec();					// spuštění programu
void FExecWait();				// spuštění programu

// dialog
//void FDialogOn();				// režim dialogu
//void FDialogOnSet();
void FWindowID();				// číslo prvku
void FWindowDel();				// zrušení okna
void FWindowText();				// text okna
void FWindowX();				// pozice okna X
void FWindowY();				// pozice okna Y
void FWindowW();				// šířka okna
void FWindowH();				// výška okna
void FWindowWInt();				// vnitřní šířka okna
void FWindowHInt();				// vnitřní výška okna
void FWindowVisible();			// viditelnost okna
void FWindowVisibleSet();		// viditelnost okna
void FWindowDisable();			// zákaz okna
void FWindowDisableSet();		// zákaz okna
void FCheckBoxOn();				// přepínač
void FCheckBoxOnSet();			// přepínač
void FButton3On();
void FButton3OnSet();
void FFocus();					// okno s fokusem
void FFontBold();
void FFontBoldSet();
void FFontFixed();
void FFontFixedSet();
void FFontHeightSet();
void FFontItalic();
void FFontItalicSet();
void FFontUnder();
void FFontUnderSet();
void FFontStrike();
void FFontStrikeSet();
void FFontSerif();
void FFontSerifSet();
void FFontWidthSet();
void FFontAngleSet();
void FFontColSet();
void FFontBackSet();
void FListBoxAdd();				// přidání položky do seznamu
void FListBoxClear();			// vyprázdnění seznamu
void FWindowIcon();				// ikona okna
void FWindowPic();				// obrázek okna
void FSetButtonEsc();			// tlačítko pro klávesu Esc
void FSetWindowPos();			// nastavení pozice v prvku okna
void FSetWindowPos2();			// nastavení pozice 2 v prvku okna
void FSetTableWidth();			// šířka sloupce tabulky
void FSetTableAlign();			// zarovnání sloupce tabulky

void FSetWindowFrame();			// nastavení rámečku okna
void FSetWindowCaption();		// nastavení nadpisu okna
void FSetWindowTop();			// nastavení okna vždy nahoře
void FSetWindowSize();			// nastavení okna s měnitelnou velikostí
void FSetWindowMayClose();		// nastavení povolení uzavření okna
void FSetWindowMaxim();			// maximalizace okna
void FSetWindowMinim();			// minimalizace okna

void FFullScreen();				// celoobrazokový mód
void FFullScreenSet();			// celoobrazokový mód

void FMouseCursor();			// kurzor myši

void FSetMouseX();				// nastavení souřadnice X myši
void FSetMouseY();				// nastavení souřadnice Y myši

/////////////////////////////////////////////////////////////////////////////
// obsluha kurzoru myši (vrací TRUE = kurzor myši nastaven)

BOOL OnSetCursor(int x, int y);

// DirectPlay
void FSetConnectAct();			// nastavení aktivního spojení
void FSetGameAct();				// nastavení aktivní hry
void FSetPlayerAct();			// nastavení aktivního hráče

void FSetGameN1();				// nastavení číselné hodnoty hry 1
void FSetGameN2();				// nastavení číselné hodnoty hry 2
void FSetGameN3();				// nastavení číselné hodnoty hry 3
void FSetGameN4();				// nastavení číselné hodnoty hry 4
void FSetGameN5();				// nastavení číselné hodnoty hry 5
void FSetGameN6();				// nastavení číselné hodnoty hry 6
void FSetGameN7();				// nastavení číselné hodnoty hry 7
void FSetGameN8();				// nastavení číselné hodnoty hry 8

void FSetGameL1();				// nastavení přepínače hry 1
void FSetGameL2();				// nastavení přepínače hry 2
void FSetGameL3();				// nastavení přepínače hry 3
void FSetGameL4();				// nastavení přepínače hry 4
void FSetGameL5();				// nastavení přepínače hry 5
void FSetGameL6();				// nastavení přepínače hry 6
void FSetGameL7();				// nastavení přepínače hry 7
void FSetGameL8();				// nastavení přepínače hry 8

void FSetPlayerMax();			// nastavení max. počtu hráčů
void FSetGamePacket();			// vyslání datového paketu
void FSetGamePacket0();			// vyslání datového paketu (všem)
void FSetGameDataN();			// vyslání čísla paketem
void FSetGameDataT();			// vyslání textu paketem (není pro UNICODE !!!)
void FSetGameDataL();			// vyslání přepínače paketem
void FSetGameDataB();			// vyslání bajtu paketem

// UDP
void FSetUDPSendIP();			// nastavení vysílací IP adresy UDP
void FSetUDPSendPort();			// nastavení vysílacího portu UDP
void FSetUDPRecSize();			// nastavení velikosti přijímacího bufferu UDP (0=zavření příjmu)
void FSetUDPRecIP();			// nastavení přijímací IP adresy UDP
void FSetUDPRecPort();			// nastavení přijímacího portu UDP
void FSetUDPMem();				// odeslání binárního paketu UDP
void FSetUDPText();				// odeslání textového paketu UDP

void FSetCodePage();			// nastavení znakové sady
void FSetUserFont();			// nastavení uživatelského fontu
void FSetLanguage();			// nastavení jazyku

void FSetAlias();				// nastavení alias

// porty
void FSetComID();				// nastavení aktivního portu
void FComClose();				// uzavření portu
void FSetComBaud();				// nastavení přenosové rychlosti portu
void FSetComStop();				// nastavení stopbitů
void FSetComBits();				// nastavení počtu bitů
void FSetComParit();			// nastavení parity
void FSetComText();				// vyslání znaku
void FSetComNum();				// vyslání čísla
void FSetComSend();				// povolení vysílání
void FSetComReceive();			// povolení příjmu

// mixer
void FSetMixDevice();			// nastavení aktivního mixeru
void FSetMixLineDst();			// nastavení cílového signálu
void FSetMixLineSrc();			// nastavení zdrojového signálu
void FSetLineType();			// nastavení typu signálu
void FSetMixControl();			// nastavení ovládacího prvku
void FSetCtrlType();			// nastavení typu ovládacího prvku
void FSetCtrlVal();				// nastavení hodnoty ovládacího prvku
void FSetMixChannel();			// nastavení kanálu ovládacího prvku
void FSetMixCtrlList();			// výběr hodnoty ovládacího prvku

// Direct3D
void FSet3DMode();				// nastavení videomódu Direct3D
void FDirect3DView();			// nastavení okna Direct3D

void FSetD3DWireframe();		// nastavení módu výplně Direct3D
void FSetD3DLighton();			// zapnutí osvětlení Direct3D
void FSetD3DShades();			// nastavení módu stínování Direct3D
void FD3DInvert();				// inverze objektu
void FSetD3DMinFilter();		// zapnutí filtrace při zmenšení
void FSetD3DMagFilter();		// zapnutí filtrace při zvětšení
void FSetD3DMipFilter();		// zapnutí filtrace při vzdálení
void FSetD3DOWireframe();		// nastavení módu výplně objektu Direct3D
void FSetD3DOLighton();			// zapnutí osvětlení objektu Direct3D
void FSetD3DOShades();			// nastavení módu stínování objektu Direct3D
void FSetD3DOMinFilter();		// zapnutí filtrace při zmenšení textury objektu
void FSetD3DOMagFilter();		// zapnutí filtrace při zvětšení textury objektu
void FSetD3DOMipFilter();		// zapnutí filtrace při vzdálení textury objektu
void FSetD3DOCulling();			// nastavení odstraňování ploch objektu

void FSetD3DVisible();			// nastavení viditelnosti objektu Direct3D
void FSetD3DObjectID();			// nastavení aktivního objektu Direct3D
void FD3DObjectDelete();		// zrušení objektu Direct3D

void FSetD3DScaleX();			// nastavení měřítka ve směru X
void FSetD3DScaleY();			// nastavení měřítka ve směru Y
void FSetD3DScaleZ();			// nastavení měřítka ve směru Z
void FSetD3DRotateX();			// nastavení rotace podle osy X
void FSetD3DRotateY();			// nastavení rotace podle osy Y
void FSetD3DRotateZ();			// nastavení rotace podle osy Z
void FSetD3DTransX();			// nastavení posunu ve směru X
void FSetD3DTransY();			// nastavení posunu ve směru Y
void FSetD3DTransZ();			// nastavení posunu ve směru Z
void FSetD3DColor();			// nastavení barvy objektu
void FSetD3DAmbiRef();			// nastavení odrazu rozptýleného světla
void FSetD3DEmissive();			// nastavení barvy svítivosti
void FSetD3DPower();			// nastavení matnosti
void FSetD3DSpecular();			// nastavení barvy odlesku
void FSetD3DTexture();			// nastavení textury objektu Direct3D
void FD3DTextureFile();			// načtení textury objektu Direct3D ze souboru
void FD3DTextMap();				// mapování textury
void FSetD3DLightColor();		// nastavení barvy světla
void FSetD3DLightType();		// nastavení typu světla
void FSetD3DLightUmbra();		// nastavení úhlu stínu světla
void FSetD3DLightPenumbra();	// nastavení úhlu polostínu světla
void FSetD3DLightRange();		// nastavení dosahu světla
void FSetD3DLightCatten();		// nastavení konstantního útlumu světla
void FSetD3DLightLatten();		// nastavení lineárního útlumu světla
void FSetD3DLightQatten();		// nastavení kvadratického útlumu světla
void FSetD3DProjection();		// nastavení projekce
void FD3DLink();				// připojení jiného objektu
//void FD3DSave();				// uložení objektu do souboru
void FSetD3DSceneColor();		// nastavení barvy pozadí scény
void FSetD3DAmbient();			// nastavení ambient světla
void FSetD3DSceneTexture();		// nastavení textury pozadí scény
void FSetD3DFogColor();			// nastavení barvy mlhy
void FSetD3DFogMode();			// nastavení módu mlhy
void FSetD3DFogStart();			// nastavení začátku mlhy
void FSetD3DFogEnd();			// nastavení konce mlhy
void FSetD3DFogDensity();		// nastavení hustoty mlhy
void FSetD3DViewFront();		// nastavení promítací roviny
void FSetD3DViewBack();			// nastavení vzdálenosti dohledu
void FSetD3DUpper();			// vytvářet horní podstavu
void FSetD3DLower();			// vytvářet dolní podstavu
void FSetD3DSplits();			// nastavení složitosti objektů
void FSetD3DOrder();			// nastavení pořadí rotací objektu
void FSetD3DDriver();			// nastavení ovladače
void FSetD3DInterface();		// nastavení rozhraní
void FSetD3DMinFilter();		// zapnutí filtrace textur při zmenšení
//void FSetD3DPersp();			// nastavení korekce perspektivy
void FSetD3DSmooth();			// nastavení vyhlazení ditheringu textur
void FD3DNorm();				// normalizace objektu
void FSetD3DTransparent();		// nastavení průhlednosti
void FSetD3DAlphaRef();			// nastavení alpfa referenční úrovně
void FSetD3DStage();			// nastavení aktivního stupně textur
void FSetD3DColorOp();			// nastavení operace barev
void FSetD3DColorArg1();		// nastavení argumentu 1 barev
void FSetD3DColorArg2();		// nastavení argumentu 2 barev
void FSetD3DAlphaOp();			// nastavení operace alfa
void FSetD3DAlphaArg1();		// nastavení argumentu 1 barev
void FSetD3DAlphaArg2();		// nastavení argumentu 2 barev
void FSetD3DAddressU();			// nastavení horizontálního adresování
void FSetD3DAddressV();			// nastavení vertikálního adresování
//void FSetD3DBorder();			// nastavení barvy okolí textury
void FSetD3DBias();				// nastavení zjemnění vzdálených textur
void FSetD3DTFactor();			// nastavení barvy faktoru textur
void FSetD3DMipMaps();			// počet úrovní mipmaps
void FSetD3DZGroup();			// nastavení renderovací skupiny
void FSetD3DZSort();			// nastavení třídění
void FSetD3DZWrite();			// nastavení hloubkového zápisu
void FSetD3DZTest();			// nastavení hloubkového testu
void FSetD3DShadowRange();		// nastavení dosahu stínu
void FD3DNormals();				// generování normál
void FD3DNormals0();			// generování normál (implicitní parametr)
void FD3DRender();				// renderování bez zobrazení
void FSetD3DLODObject();		// nastavení objektu pro LOD
void FSetD3DLODDistance();		// nastavení vzdálenosti pro LOD
void FSetD3DMorphModel();		// nastavení vzoru morfování
void FSetD3DMorphLevel();		// nastavení stupně morfování

// obsluha DDE
void FSetDDEApp();				// nastavení jména aplikace DDE

void FSetActive();				// nastavení aktivní aplikace
void FSetDialogGraph();			// nastavení grafického pozadí

void FPicPrint2();				// tisk obrázku

// obsluha DLL
void FDLLFree();				// uvolnění DLL knihovny
void FDLLExecCom();				// provedení DLL funkce
void FMemoryFree();				// zrušení bloku paměti
void FSetMemoryRead();			// ukazatel čtení z paměti
void FSetMemoryWrite();			// ukazatel zápisu do paměti
void FMemoryCopy();				// kopie dat
void FSetMemoryByteS();			// bajt se znaménkem
inline void FSetMemoryByteU() { FSetMemoryByteS(); } // bajt bez znaménka
void FSetMemoryWordS();			// slovo se znaménkem
inline void FSetMemoryWordU() { FSetMemoryWordS(); }	// slovo bez znaménka
void FSetMemory3ByteS();		// 3-bajt se znaménkem
inline void FSetMemory3ByteU() { FSetMemory3ByteS(); } // 3-bajt bez znaménka
void FSetMemoryDWordS();		// dvouslovo se znaménkem
void FSetMemoryDWordU();		// dvouslovo bez znaménka
void FSetMemoryFloat();			// reálné číslo 4 bajty
void FSetMemoryDouble();		// reálné číslo 8 bajtů
void FSetMemoryBool();			// logická hodnota
void FSetMemoryText0();			// text zakončený nulou
void FSetMemoryText0P();		// ukazatel na text zakončený nulou
void FSetMemoryTextN();			// text s pevnou délkou
void FSetMemoryTextNLen();		// délka textu s pevnou délkou
void FMemoryLoad();				// načtení bloku dat
void FMemorySave();				// zápis bloku dat

// konfigurace
void FSetIniFile();				// jméno konfiguračního souboru
void FSetIniSection();			// jméno konfigurační sekce
void FSetIniValue();			// jméno konfiguračního parametru
void FSetIniDel();				// zrušení konfiguračního parametru
void FSetIniString();			// zápis konfiguračního textu
void FSetIniNum();				// zápis konfiguračního čísla
void FSetRegKey();				// skupina klíčů registrů
void FSetRegSubkey();			// klíč registrů
void FSetRegValue();			// jméno položky registru
void FSetRegDel();				// zrušení registru
void FSetRegString();			// zápis textu do registru
void FSetRegNum();				// zápis čísla do registru
