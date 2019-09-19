
/***************************************************************************\
*																			*
*						Provádění programu - číselný výraz					*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// inline volání příkazu - číslo

inline double FNum()
{
	ExecItem++;
	return ExecItem[-1].ExecNum();
}


/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru převedeného na int

//int FInt();

/////////////////////////////////////////////////////////////////////////////
// načtení úhlu s normalizací rozsahu 0 až 2pi

double FAngle();


/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru převedeného na int se zaokrouhlením

int FIntR();


/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru převedeného na DWORD se zaokrouhlením

DWORD FIntRN();

/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru převedeného na INT64 se zaokrouhlením

hyper FIntHN();

/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru převedeného na grafickou souřadnici X

int FIntX();


/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru převedeného na grafickou souřadnici Y

inline int FIntY() { return FIntX(); };


/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru vynásobeného * 1000 (čas v milisekundách)

int FInt1000();


/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru vynásobeného * 2 (průměr kruhu)

int FIntX2();


/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru vynásobeného * 18.20648 (čas v časových impulsech)

int FInt18();


/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru s vynásobením * 255

BYTE FInt255();


/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru převedeného na souborový čas

__int64 FNumFileTime();


/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru převedeného na systémový čas

#define FILETIMEKONV 10000000		// převod na souborový čas

extern	SYSTEMTIME	SystemTime;			// pomocná proměnná pro obsluhu času
void FNumSysTime();


/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru převedeného na světový souborový čas

__int64 FNumFileTimeUTC();


/////////////////////////////////////////////////////////////////////////////
// načtení barvy

int FNumCol();


/////////////////////////////////////////////////////////////////////////////
// načtení souřadnic Petra/Petry (ECX = adresa x, EDX = adresa y)

bool _fastcall PetrXY(int& x, int& y);
bool _fastcall Petr2XY(int& x, int& y);


/////////////////////////////////////////////////////////////////////////////
// načtení pozice Petra/Petry (NULL=neplatná)

MAPITEM* PetrXYMap();
MAPITEM* Petr2XYMap();


/////////////////////////////////////////////////////////////////////////////
// načtení příští souřadnice Petra/Petry (ECX = adresa x, EDX = adresa y)

bool _fastcall PetrXYNext(int& x, int& y);
bool _fastcall Petr2XYNext(int& x, int& y);


// funkce
double FNumFunc();					// funkce s návratem číselné hodnoty

// proměnné
double FNumConst();					// číselná konstanta
double FNumObj();					// globální proměnná (Data = index)
double FNumObjList();				// globalní proměnná v seznamu (Data = index, List = seznam)
double FNumLoc();					// lokální proměnná (Data = index)
double FNumLocList();				// lokální proměnná v seznamu (Data = index, List = seznam)
double FGetList();					// index globálního seznamu
double FGetListLoc();				// index lokálního seznamu
double FGetListAuto();				// inkrementace globálního seznamu
double FGetListAutoLoc();			// inkrementace lokálního seznamu
double FGetSpriteX();				// souřadnice X globálního sprajtu (Data = index)
double FGetSpriteXList();			// souřadnice X sprajtu v seznamu (Data = index, List = seznam)
double FGetSpriteXLoc();			// souřadnice X lokálního sprajtu (Data = index)
double FGetSpriteXLocList();		// souřadnice X lokálního sprajtu v seznamu (Data = index, List = seznam)
double FGetSpriteY();				// souřadnice Y globálního sprajtu (Data = index)
double FGetSpriteYList();			// souřadnice Y sprajtu v seznamu (Data = index, List = seznam)
double FGetSpriteYLoc();			// souřadnice Y lokálního sprajtu (Data = index)
double FGetSpriteYLocList();		// souřadnice Y lokálního sprajtu v seznamu (Data = index, List = seznam)
double FGetSpriteSmer();			// směr globálního sprajtu (Data = index)
double FGetSpriteSmerList();		// směr sprajtu v seznamu (Data = index, List = seznam)
double FGetSpriteSmerLoc();			// směr lokálního sprajtu (Data = index)
double FGetSpriteSmerLocList();		// směr lokálního sprajtu v seznamu (Data = index, List = seznam)
double FGetSpriteFaze();			// fáze globálního sprajtu (Data = index)
double FGetSpriteFazeList();		// fáze sprajtu v seznamu (Data = index, List = seznam)
double FGetSpriteFazeLoc();			// fáze lokálního sprajtu (Data = index)
double FGetSpriteFazeLocList();		// fáze lokálního sprajtu v seznamu (Data = index, List = seznam)
double FGetSpriteStatus();			// stav globálního sprajtu (Data = index)
double FGetSpriteStatusList();		// stav sprajtu v seznamu (Data = index, List = seznam)
double FGetSpriteStatusLoc();		// stav lokálního sprajtu (Data = index)
double FGetSpriteStatusLocList();	// stav lokálního sprajtu v seznamu (Data = index, List = seznam)

// obsluha Petra a Petry
double FGetSmer();					// Petr - směr
double FGetSmer2();					// Petra - směr
double FGetPozX();					// Petr - pozice X
double FGetPozX2();					// Petra - pozice X
double FGetPozY();					// Petr - pozice Y
double FGetPozY2();					// Petra - pozice Y
double FGetSwcN1();					// načtení čísla 1 - Petr
double FGetSwcN12();				// načtení čísla 1 - Petra
double FGetSwcN2();					// načtení čísla 2 - Petr
double FGetSwcN22();				// načtení čísla 2 - Petra
double FGetSwcN3();					// načtení čísla 3 - Petr
double FGetSwcN32();				// načtení čísla 3 - Petra

// výpočty
double FInc();						// inkrementace
double FDec();						// dekrementace
double FSin();						// sinus
double FCos();						// cosinus
double FTg();						// tangens
double FCoTg();						// cotangens
double FArcSin();					// arcsinus
double FArcCos();					// arccosinus
double FArcTg();					// arctangens
double FArcCoTg();					// arccotangens
double FNumInt();					// celé číslo
double FFrac();						// desetinná část čísla
double FAbs();						// absolutní hodnota
double FSqrt();						// odmocnina
double FFact();						// faktoriál
double FRand();						// náhodné číslo s násobkem
double FRand0();					// náhodné číslo bez násobku
double FDegRad();					// převod stupňů na radiány
double FRadDeg();					// převod radiánů na stupně
double FLog();						// dekadický logaritmus
double FLn();						// přirozený logaritmus
double FExp();						// přirozený exponent
double FAdd();						// operátor ADD
double FSub1();						// operátor SUB pro 1 prvek
double FSub();						// operátor SUB
double FMul();						// operátor MUL
double FDiv1();						// operátor DIV pro 1 prvek
double FDiv();						// operátor DIV
double FMod();						// operátor MOD
double FMocn();						// operátor MOCNINA
double FNumSqr();					// druhá mocnina

// obsluha grafiky
double FColorLevel();				// nastavení jasu barvy
double FColorSynth();				// syntéza barvy
double FGetPoint();					// zjištění barvy bodu
double FGetPicWidth();				// zjištění šířky obrázku
double FGetPicHeight();				// zjištění výšky obrázku
double FGetGraphAreaWidth();		// zjištění šířky pole k zobrazení centrovaného textu

// obsluha času
double FGetTimeCurrent();			// načtení aktuálního času
double FTimeUtc();					// převod na světový čas
double FTimeLocal();				// převod na místní čas
double FTimeComp();					// syntéza času
double FGetTimeYear();				// načtení roku z času
double FGetTimeMonth();				// načtení měsíce z času
double FGetTimeDay();				// načtení dne z času
double FGetTimeWeek();				// načtení dne v týdnu z času
double FGetTimeHour();				// načtení hodiny z času
double FGetTimeMin();				// načtení minuty z času
double FGetTimeSec();				// načtení sekundy z času
double FGetTimeMSec();				// načtení milisekundy z času

// obsluha myši
double FMouseX();					// načtení souřadnice X myši
double FMouseY();					// načtení souřadnice Y myši

// obsluha dialogů
double FNumDialogBox();				// dialog box s návratem hodnoty

// obsluha souboru
double FGetDiskSize();				// zjištění velikosti disku
double FGetDiskFree();				// zjištění volného místa disku
double FGetDiskSerial();			// načtení sériového čísla disku
double FGetFileCreate();			// čas vytvoření souboru
double FGetFileOpen();				// čas čtení ze souboru
double FGetFileModi();				// čas zápisu do souboru
double FGetFileReadOff();			// offset pro čtení ze souboru
double FGetFileWriteOff();			// offset pro zápis do souboru
double FGetFileSize();				// zjištění velikosti souboru
double FGetFileTextNLen();			// délka textu s pevnou délkou
double FFileDiskType();				// načtení typu disku
double FGetFileByteS();				// načtení čísla ze souboru bajt se znaménkem
double FGetFileByteU();				// načtení čísla ze souboru bajt bez znaménka
double FGetFileWordS();				// načtení čísla ze souboru slovo se znaménkem
double FGetFileWordU();				// načtení čísla ze souboru slovo bez znaménka
double FGetFile3ByteS();			// načtení čísla ze souboru 3 bajty se znaménkem
double FGetFile3ByteU();			// načtení čísla ze souboru 3 bajty bez znaménka
double FGetFileDWordS();			// načtení čísla ze souboru dvojslovo se znaménkem
double FGetFileDWordU();			// načtení čísla ze souboru dvojslovo bez znaménka
double FGetFileFloat();				// načtení čísla ze souboru FLOAT
double FGetFileDouble();			// načtení čísla ze souboru DOUBLE

// obsluha CD disku
double FCDGetLength();				// zjištění délky CD
double FCDGetTracks();				// zjištění počtu stop CD
double FCDGetPos();					// zjištění pozice CD
double FCDGetTrack();				// zjištění aktivní stopy CD
double FCDGetTrackLength();			// zjištění délky aktivní stopy
double FCDGetTrackPos();			// zjištění pozice aktivní stopy CD

// obsluha hudby
double FMusicLen();					// délka přehrávané hudby
double FGetMusicPos();				// pozice hudby

// obsluha zvuku
double FGetSoundBit();				// počet bitů na vzorek
double FGetSoundFreq();				// vzorkovací kmitočet
double FGetSoundLength();			// délka zvuku
double FGetSoundPos();				// pozice zvuku
double FGetSoundDistinguish();		// rozlišovací kód

// obsluha textů
double FTextLen();					// délka textu
double FTextScan();					// převod textu na číslo
double FTextFind();					// nalezení textu
double FTextOrd();					// abecední pořadí znaku
double FTextLineNum();				// zjištění počtu řádků textu

// obsluha klávesnice
double FVKey();						// načtení klávesy bez čekání
double FVKeyWait();					// načtení klávesy s čekáním

// plocha
double FGetMapWidth();				// šířka plochy
double FGetMapWidth0();				// šířka hlavní plochy
double FGetMapHeight();				// výška plochy
double FGetMapHeight0();			// výška hlavní plochy
double FGetMapNum1();				// načtení čísla 1 z plochy
double FGetMapNum2();				// načtení čísla 2 z plochy
double FGetMapNum3();				// načtení čísla 3 z plochy

// načtení okna k zobrazení sprajtů
double FGetSpriteWinX1();
double FGetSpriteWinY1();
double FGetSpriteWinX2();
double FGetSpriteWinY2();

double FGetReturnCode();			// návratový kód

double FGetExecWait();				// spuštění programu

// joystick
double FJoystickX();				// pozice X
double FJoystickY();				// pozice Y
double FJoystickZ();				// pozice Z

// dialogy
double FGetWindowID();				// ID aktivního prvku
double FWindowNew();				// vytvoření okna
double FWindowRONew();				// vytvoření okna
double FButtonNew();				// vytvoření tlačítka
double FGetButtonEsc();				// tlačítko Esc
double FDefButtonNew();				// vytvoření tlačítka
double FButtonIconNew();			// vytvoření ikonového tlačítka
double FButton3New();				// vytvoření tlačítka
double FButtonCheckNew();			// vytvoření tlačítka
double FButtonCheckNew2();			// vytvoření přepínacího tlačítka
double FEditNew();					// vytvoření editačního řádku
double FButtonRadioNew();			// vytvoření tlačítka
double FButtonRadioNew2();			// vytvoření tlačítka
double FMemoNew();					// vytvoření editačního pole
double FRicheditNew();				// vytvoření editoru Richedit
double FRichMemoNew();				// vytvoření editačního pole se zalamováním
double FGroupBoxNew();				// vytvoření skupiny
double FListBoxNew();				// vytvoření seznamu
double FTabsNew();					// vytvoření záložek
double FWinTextNew();				// vytvoření textu
double FWinTextNewCenter();			// vytvoření textu vystředěného
double FWinTextNewRight();			// vytvoření textu zarovnaného vpravo
double FListEditNew();				// vytvoření seznamu
double FComboBoxNew();				// vytvoření seznamu
double FComboEditNew();				// vytvoření seznamu
double FTableNew();					// vytvoření tabulky
double FGetTableWidth();			// zjištění šířky sloupce tabulky
double FGetTableAlign();			// zjištění zarovnání sloupce tabulky
double FGetWindowX();				// počátek okna vlevo
double FGetWindowY();				// počátek okna dole
double FGetWindowW();				// šířka okna
double FGetWindowH();				// výška okna
double FGetWindowWInt();			// vnitřní šířka okna
double FGetWindowHInt();			// vnitřní výška okna
double FGetFontHeight();			// výška fontů
double FGetFontWidth();				// šířka fontů
double FGetFontAngle();				// úhel textu
double FGetFontCol();				// barva textu
double FGetFontBack();				// barva pozadí
double FGetFocus();					// okno s fokusem
double FWinIconNew();				// vytvoření ikony
double FWinPicNew();				// vytvoření obrázku
double FProgressNew();				// indikátor
double FHTrackbarNew();				// táhlo horizontálně
double FVTrackbarNew();				// táhlo vertikálně
double FHScrollbarNew();			// horizontální posuvník
double FVScrollbarNew();			// vertikální posuvník
double FGetWindowPos();				// pozice v prvku
double FGetWindowPos2();			// pozice 2 v prvku

// DirectPlay
double FGetGameN1();				// zjištění číselné hodnoty hry 1
double FGetGameN2();				// zjištění číselné hodnoty hry 2
double FGetGameN3();				// zjištění číselné hodnoty hry 3
double FGetGameN4();				// zjištění číselné hodnoty hry 4
double FGetGameN5();				// zjištění číselné hodnoty hry 5
double FGetGameN6();				// zjištění číselné hodnoty hry 6
double FGetGameN7();				// zjištění číselné hodnoty hry 7
double FGetGameN8();				// zjištění číselné hodnoty hry 8
double FGetPlayerMax();				// zjištění max. počtu hráčů
double FPlayerInx();				// zjištění indexu hráče
double FGetGamePacket();			// příjem paketu
double FGetGameDataN();				// načtení čísla z paketu
double FGetGameDataB();				// načtení bajtu z paketu

// UDP
double FGetUDPSendPort();			// zjištění vysílacího portu UDP
double FGetUDPRecSize();			// zjištění velikosti přijímacího bufferu UDP
double FGetUDPRecPort();			// zjištění přijímacího portu UDP
double FGetUDPMem();				// příjem datového bloku UDP

// bitové operace
double FBitAnd();					// operátor BIT-AND (Data = počet prvků - 1)
double FBitOr();					// operátor BIT-OR (Data = počet prvků - 1)
double FBitXor();					// operátor BIT-XOR (Data = počet prvků - 1)
double FBitNot();					// bitový doplněk
double FBitLeft();					// rotace vlevo
double FBitRight();					// rotace vpravo


double FGetCodePage();				// zjištění znakové sady
double FGetLanguage();				// zjištění jazyku

// porty
double FComOpen();					// otevření portu
double FGetComID();					// aktivní port
double FGetComBaud();				// zjištění přenosové rychlosti portu
double FGetComBits();				// zjištění počtu bitů portu
double FGetComParit();				// zjištění parity
double FGetComNum();				// načtení bajtu

// mixer
double FGetLineType();				// zjištění typu signálu
double FGetCtrlType();				// zjištění typu ovládacího prvku
double FGetCtrlVal();				// zjištění hodnoty ovládacího prvku
double FGetMixChannel();			// zjištění počtu kanálů prvku

// Direct3D
double FGetD3DOCulling();			// zjištění módu odstraňováni ploch objektu Direct3D
double FD3DTriangle();				// vytvoření trojúhleníku
double FD3DWall();					// vytvoření zdi
double FD3DCircle();				// vytvoření kruhu
double FD3DCube();					// vytvoření krychle
double FD3DSphere();				// vytvoření koule
double FD3DHemisphere();			// vytvoření polokoule
double FD3DTorus();					// vytvoření torusu
double FD3DCylinder();				// vytvoření válce
double FD3DCone();					// vytvoření kužele
double FD3DCone2();					// vytvoření komolého kužele
double FD3DClone();					// klonování
double FD3DText();					// načtení objektu z textu
double FD3DFile();					// načtení objektu ze souboru
double FGetD3DObjectID();			// zjištění ID aktivního objektu
double FD3DLight();					// vytvoření světla

double FGetD3DScaleX();				// zjištění měřítka ve směru X
double FGetD3DScaleY();				// zjištění měřítka ve směru Y
double FGetD3DScaleZ();				// zjištění měřítka ve směru Z
double FGetD3DRotateX();			// zjištění rotace podle osy X
double FGetD3DRotateY();			// zjištění rotace podle osy Y
double FGetD3DRotateZ();			// zjištění rotace podle osy Z
double FGetD3DTransX();				// zjištění posunu ve směru X
double FGetD3DTransY();				// zjištění posunu ve směru Y
double FGetD3DTransZ();				// zjištění posunu ve směru Z
double FGetD3DColor();				// zjištění barvy objektu
double FGetD3DEmissive();			// zjištění barvy svítivosti
double FGetD3DAmbiRef();			// zjištění odrazu rozptýleného světla
double FGetD3DPower();				// zjištění matnosti
double FGetD3DSpecular();			// zjištění barvy odlesku
double FGetD3DLightColor();			// zjištění barvy světla
double FGetD3DLightType();			// zjištění typu světla
double FGetD3DLightUmbra();			// zjištění úhlu stínu světla
double FGetD3DLightPenumbra();		// zjištění úhlu polostínu světla
double FGetD3DLightRange();			// zjištění dosahu světla
double FGetD3DLightCatten();		// zjištění konstantího útlumu světla
double FGetD3DLightLatten();		// zjištění lineárního útlumu světla
double FGetD3DLightQatten();		// zjištění lineárního útlumu světla
double FGetD3DProjection();			// zjištění projekce
double FGetD3DSceneColor();			// zjištění barvy pozadí scény
double FGetD3DAmbient();			// zjištění ambient světla
double FGetD3DFogColor();			// zjištění barvy mlhy
double FGetD3DFogMode();			// zjištění módy mlhy
double FGetD3DFogStart();			// zjištění začátku mlhy
double FGetD3DFogEnd();				// zjištění konce mlhy
double FGetD3DFogDensity();			// zjištění hustotu mlhy
double FGetD3DViewFront();			// zjištění projekční roviny
double FGetD3DViewBack();			// zjištění vzdálenosti dohledu
//double FD3DPick();					// vyhledání objektu
double FD3D2DObject();				// vytvoření 2D objektu
double FD3D2DTerrain();				// vytvoření terénu
double FD3DLevel();					// zjištění úrovně terénu
double FD3DEmpty();					// vytvoření prázdného objektu
double FGetD3DSplits();				// zjištění složitosti objektů
double FGetD3DOrder();				// zjištění pořadí rotací objektů
double FGetD3DDriver();				// zjištění ovladače
double FGetD3DInterface();			// zjištění rozhraní
double FD3DRotary();				// vytvoření rotačního objektu
double FD3DTerrainMap();			// vytvoření terénu z plochy
double FGetD3DSmooth();				// zjištění vyhlazení textur
double FD3D2DStatic();				// vytvoření statického 2D objektu
double FGetD3DTransparent();		// zjištění průhlednosti textury objektu Direct3D
double FGetD3DAlphaRef();			// zjištění alfa úrovně Direct3D
double FD3DElapsed0();				// uplynulý přesný čas 
double FD3DElapsed();				// uplynulý přesný čas 
double FD3DSpeed();					// rychlost renderování
double FD3DFree();					// volná videopaměť
double FD3DPicture();				// vytvoření 2D obrázku
double FD3DLensFlare();				// vytvoření čočkového odlesku
double FGetD3DStage();				// zjištění počtu stupňů textur
double FGetD3DColorOp();			// zjištění operace barev
double FGetD3DColorArg1();			// zjištění argumentu 1 barev
double FGetD3DColorArg2();			// zjištění argumentu 2 barev
double FGetD3DAlphaOp();			// zjištění operace alfa
double FGetD3DAlphaArg1();			// zjištění argumentu 1 alfa
double FGetD3DAlphaArg2();			// zjištění argumentu 2 alfa
double FGetD3DAddressU();			// zjištění horizontálního adresování
double FGetD3DAddressV();			// zjištění vertikálního adresování
//double FGetD3DBorder();				// zjištění barvy okolí textury
double FGetD3DBias();				// zjištění zjemnění vzdálených textur
double FGetD3DMipMaps();			// počet úrovní mipmaps

double FPolarRadius();				// výpočet vzdálenosti bodu
double FPolarAngle();				// výpočet úhlu bodu
double FPreciseTimer();				// přesný čítač času
double FGetD3DTFactor();			// zjištění barvy faktoru textur
double FGetD3DZGroup();				// zjištění renderovací skupiny
double FGetD3DZTest();				// zjištění hloubkového testu
double FGetD3DShadowRange();		// zjištění dosahu stínu
double FGetD3DLODObject();			// zjištění objektu pro LOD
double FGetD3DLODDistance();		// zjištění vzdálenosti pro LOD
double FGetD3DMorphModel();			// zjištění max. stupně morfování
double FGetD3DMorphLevel();			// zjištění stupně morfování
double FD3DChild0();				// poskytnutí potomka 3D objektu
double FD3DChild();					// poskytnutí dalšího potomka 3D objektu

// tabulky pro tvorbu zdi 3D, používají se i při zobrazení 2D objektu v OpenGL
extern D3DVECTOR wall_vertices[4];
extern float wall_text[2*4];

// obsluha DLL
double FDLLLoad();					// načtení DLL knihovny
double FDLLImport();				// importování DLL funkce
double FDLLExecNum();				// provedení DLL funkce
double FMemoryNew();				// vytvoření bloku paměti
double FMemoryResize();				// změna velikosti bloku paměti
double FGetMemoryRead();			// ukazatel čtení z paměti
double FGetMemoryWrite();			// ukazatel zápisu do paměti
double FMemoryCompare();			// porovnání dat
double FGetMemoryByteS();			// bajt se znaménkem
double FGetMemoryByteU();			// bajt bez znaménka
double FGetMemoryWordS();			// slovo se znaménkem
double FGetMemoryWordU();			// slovo bez znaménka
double FGetMemory3ByteS();			// 3-bajt se znaménkem
double FGetMemory3ByteU();			// 3-bajt bez znaménka
double FGetMemoryDWordS();			// dvouslovo se znaménkem
double FGetMemoryDWordU();			// dvouslovo bez znaménka
double FGetMemoryFloat();			// reálné číslo 4 bajty
double FGetMemoryDouble();			// reálné číslo 8 bajtů
double FGetMemoryTextNLen();		// délka textu s pevnou délkou
double FWindowHandle();				// handle okenního prvku
double FMemory3D();					// vytvoření 3D objektu z paměti
double FD3DMemory();				// vytvoření paměťového bloku z 3D objektu

// konfigurace
double FGetIniNum();				// čtení konfiguračního čísla
double FGetIniNum0();				// čtení konfiguračního čísla
double FGetRegKey();				// skupina klíčů registru
double FGetRegNum();				// čtení čísla z registru
double FGetRegNum0();				// čtení čísla z registru
