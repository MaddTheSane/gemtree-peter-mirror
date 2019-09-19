
/***************************************************************************\
*																			*
*							Hlavní okno aplikace							*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// globální proměnné

extern	HWND		MainFrame;				// hlavní okno aplikace
extern	HMENU		Menu;					// aktivní menu v hlavním okně
extern	HWND		ToolBar;				// nástrojová lišta

extern	HBITMAP		ToolBarBmp;				// bitmapa panelu nástrojů

extern	RECT		ClientRect;				// klientská oblast hlavního okna
extern	int			ClientWidth;			// šířka klientské oblasti
extern	int			ClientHeight;			// výška klientské oblasti

extern	CText		MainFrameName;			// titulek hlavního okna - jméno aplikace
extern	CText		MainFrameText;			// úplný text titulku hlavního okna

// kurzory myši
extern	HCURSOR		CurAkt;					// aktivní kurzor

extern	HCURSOR		CurArrow;				// (standardní) šipka
extern	HCURSOR		CurCil;					// cíl operace
extern	HCURSOR		CurCopy;				// kopie
extern	HCURSOR		CurDelete;				// zrušení
extern	HCURSOR		CurElip;				// elipsa
extern	HCURSOR		CurFill;				// výplň
extern	HCURSOR		CurFillElip;			// ovál
extern	HCURSOR		CurFillRect;			// vyplněný obdélník
extern	HCURSOR		CurFillRound;			// kruh
extern	HCURSOR		CurKapatko;				// kapátko
extern	HCURSOR		CurKoule;				// koule
extern	HCURSOR		CurLine;				// čára
extern	HCURSOR		CurMove;				// přesun
extern	HCURSOR		CurNoDrag;				// zákaz tažení
extern	HCURSOR		CurPaint;				// štětec
extern	HCURSOR		CurPen;					// pero
extern	HCURSOR		CurRect;				// obdélník
extern	HCURSOR		CurRound;				// kružnice
extern	HCURSOR		CurRuka;				// ruka
extern	HCURSOR		CurSelect;				// výběr bloku
extern	HCURSOR		CurSelectMove;			// přesun výběru
extern	HCURSOR		CurSplitH;				// rozdělení horizontálně
extern	HCURSOR		CurSplitV;				// rozdělení vertikálně
extern	HCURSOR		CurSpray;				// sprej
extern	HCURSOR		CurWait;				// čekání

// stav myši (aktualizováno v PreTranslateMessage)
extern	BOOL		LMouseDown;				// levé tlačítko stisknuto
extern	BOOL		LMouseDClick;			// dvojklik levého tlačítka
extern	BOOL		RMouseDown;				// pravé tlačítko stisknuto
extern	BOOL		RMouseDClick;			// dvojklik pravého tlačítka
extern	HWND		MouseWnd;				// okno pod kurzorem myši
extern	POINT		MouseClient;			// souřadnice myši v okně pod kurzorem
extern	POINT		MouseScreen;			// souřadnice myši na displeji
extern	POINT		MouseMain;				// souřadnice myši v hlavním okně

extern	HWND		StatusBar;				// stavová lišta
extern	CText		StatusText;				// zobrazený text stavové lišty
extern	CText		StatusText2;			// zobrazený text stavové lišty 2
extern	int			StatusWidth;			// šířka panelu 2 stavové lišty
extern	BOOL		StatusBig;				// možnost použít velké fonty stavové lišty

/////////////////////////////////////////////////////////////////////////////
// menu

// definice položky menu
typedef struct MENUITEM_ 
{
	int		id0;							// identifikace jména položky (0=separátor nebo konec)
	int		id;								// identifikace příkazu a nápovědy položky (0=následuje podmenu nebo konec)
	int		bitmap;							// číslo ikony s obrázkem (-1 = není)
	int		typ;							// typ položky - kombinace příznaků (0=nic)
} MENUITEM;

#define MSWC	1							// je spínač
#define MRAD	2							// je přepínač

// informace o položce spuštěného menu
typedef struct MENUAKTITEM_
{
	int		id0;							// identifikace jména položky (0=separátor nebo konec)
	int		id;								// identifikace příkazu a nápovědy položky (0=následuje podmenu nebo konec)
	int		bitmap;							// číslo ikony s obrázkem (-1 = není)
	int		typ;							// typ položky - kombinace příznaků (0=nic)
	HMENU	menu;							// menu s položkou
	BOOL	main;							// je to položka hlavního menu
	int		inx;							// index položky v menu
	int		width;							// šířka položky (s přihlédnutím k ostatním položkám)
	int		height;							// výška položky
	CText	text;							// text položky v menu (bez zkratkové klávesy)
	CText	textshort;						// text zkratkové klávesy
	int		key;							// horká klávesa - velké písmeno (0=není)
	CText	help;							// dlouhá nápověda
	CText	help2;							// krátká nápověda (max. 79 znaků)
} MENUAKTITEM;


extern MENUITEM** MenuAkt;					// aktuální menu (kvůli cachování, NULL = aktualizovat vždy)

extern MENUITEM MenuSoubor1[];				// soubor 1
extern MENUITEM MenuSoubor2[];				// soubor 2
extern MENUITEM MenuEditace[];				// editace
extern MENUITEM MenuZobraz[];				// zobrazení
extern MENUITEM MenuProgram[];				// program
extern MENUITEM MenuObrazek[];				// obrázek
extern MENUITEM MenuIkona[];				// ikona
extern MENUITEM MenuNastroje[];				// nástroje
extern MENUITEM MenuTloustka[];				// tloušťka pera
extern MENUITEM MenuPlocha[];				// plocha
extern MENUITEM MenuSprajt[];				// sprajt
extern MENUITEM MenuSound[];				// zvuk
extern MENUITEM MenuMusic[];				// hudba
extern MENUITEM MenuHlp[];					// nápověda

/////////////////////////////////////////////////////////////////////////////
// nastavení nového menu

void SetNewMenu(MENUITEM** item);


/////////////////////////////////////////////////////////////////////////////
// příprava jména hlavního okna

void MainFrameNameInit();


/////////////////////////////////////////////////////////////////////////////
// vytvoření hlavního okna aplikace

BOOL MainFrameCreate();


/////////////////////////////////////////////////////////////////////////////
// nastavení fontu stavového řádku

void SetStatusFont();


/////////////////////////////////////////////////////////////////////////////
// zobrazení jména souboru v titulku okna

void ZobrazJmeno();


/////////////////////////////////////////////////////////////////////////////
// povolení volby v menu a v panelu nástrojů

void EnableCommand(int id, BOOL enable);

/////////////////////////////////////////////////////////////////////////////
// zapnutí volby v menu a v panelu nástrojů

void CheckCommand(int id, BOOL check);

/////////////////////////////////////////////////////////////////////////////
// nastavení/aktualizace šířky pole 2 stavové lišty

void SetStatusWidth(int width);


/////////////////////////////////////////////////////////////////////////////
// nastavení textu stavové lišty

void SetStatusText(const CText& text);


/////////////////////////////////////////////////////////////////////////////
// nastavení textu stavové lišty 2

void SetStatusText2(const CText& text);


/////////////////////////////////////////////////////////////////////////////
// zrušení všech tlačítek panelu nástrojů (až po zadané minimální množství)

void ToolBarClear(int min);


/////////////////////////////////////////////////////////////////////////////
// přidání tlačítek do panelu nástrojů

void ToolBarAdd(TBBUTTON* button, int num);


/////////////////////////////////////////////////////////////////////////////
// nastavení obrázku tlačítka v panelu nástrojů

void ToolBarBitmap(int id, int bitmap);


/////////////////////////////////////////////////////////////////////////////
// aktualizace zobrazení panelu nástrojů

void ToolBarResize();

/////////////////////////////////////////////////////////////////////////////
// aktualizace klientských souřadnic

void InitClientRect();

/////////////////////////////////////////////////////////////////////////////
// obsluha informačního okna About

BOOL CALLBACK AboutBoxProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

void OnAboutBox();

/////////////////////////////////////////////////////////////////////////////
// vytvoření kombinovaného pole v panelu nástrojů

HWND CreateToolCombo(int id, int* items, int index);

/////////////////////////////////////////////////////////////////////////////
// výběr prvku v COMBO boxu

void SelectCombo(HWND combo, int id, int* items);

/////////////////////////////////////////////////////////////////////////////
// zobrazení kurzoru čekání (zahájení a ukončení musí být do páru!)

void BeginWaitCursor();

/////////////////////////////////////////////////////////////////////////////
// vypnutí kurzoru čekání (zahájení a ukončení musí být do páru!)

void EndWaitCursor();

/////////////////////////////////////////////////////////////////////////////
// obsluha zprávy před rozesláním do oken

BOOL PreTranslateMessage(MSG* msg);


/////////////////////////////////////////////////////////////////////////////
// časové překreslení okna (zadaného bufferu, -1=vše)

void TimeRepaint(int bufID);


/////////////////////////////////////////////////////////////////////////////
// aktualizace předešlého okna

void AktPrevWindow();


/////////////////////////////////////////////////////////////////////////////
// nastavení fontu a stylu pro vlastní výpis textu

void DialogCustomText(HWND hWnd, DWORD id);


/////////////////////////////////////////////////////////////////////////////
// nastavení vlastního výpisu textu

void DialogCustom0(HWND hWnd, DWORD id);


/////////////////////////////////////////////////////////////////////////////
// nastavení vlastního výpisu textu

void DialogCustom(HWND hWnd, DWORD id, BOOL load, BOOL font);


/////////////////////////////////////////////////////////////////////////////
// vlastní vykreslení prvků v dialogových oknech (vrací TRUE=obslouženo)

BOOL DialogDraw(HWND hWnd, LPARAM lParam);


/////////////////////////////////////////////////////////////////////////////
// nastavení vlastního přepínače RADIO

void DialogCustomRadio(HWND hWnd, DWORD id);


/////////////////////////////////////////////////////////////////////////////
// nastavení vlastního přepínače CHECK

void DialogCustomCheck(HWND hWnd, DWORD id);


/////////////////////////////////////////////////////////////////////////////
// nastavení vlastního rámečku

void DialogCustomBox(HWND hWnd, DWORD id);


/////////////////////////////////////////////////////////////////////////////
// nastavení hodnoty přepínače

void DialogSetCheck(HWND hWnd, DWORD id, BOOL check);


/////////////////////////////////////////////////////////////////////////////
// nastavení směru textu zprava doleva   JAKJAK

void SetRightToLeft(BOOL righttoleft);


/////////////////////////////////////////////////////////////////////////////
// indexy obrázků tlačítek pro ToolBar

enum ToolButton {
	ButtonStart,			// start
	ButtonNew,				// nový soubor
	ButtonOpen,				// otevření souboru
	ButtonClose,			// uzavření souboru
	ButtonSave,				// uložení souboru
	ButtonSaveModi,			// uložení modifikovaného souboru
	ButtonUndo,				// undo
	ButtonRedo,				// redo
	ButtonCut,				// cut
	ButtonCopy,				// copy
	ButtonPaste,			// paste
	ButtonClear,			// clear
	ButtonAll,				// select all
	ButtonPrint,			// tisk
	ButtonHelpCont,			// help
	ButtonHelp,				// help k obsahu
	ButtonIn,				// zoom in
	ButtonOut,				// zoom out
	ButtonPen,				// pero
	ButtonPaint,			// štětec
	ButtonLine,				// čára
	ButtonBox,				// obdélník
	ButtonFillBox,			// vyplnění obdélník
	ButtonCircle,			// kružnice
	ButtonFillCircle,		// kruh
	ButtonEllipse,			// elipsa
	ButtonFillEllipse,		// ovál
	ButtonWidth1,			// šířka 1
	ButtonWidth2,			// šířka 2
	ButtonWidth3,			// šířka 3
	ButtonWidth4,			// šířka 4
	ButtonWidth5,			// šířka 5
	ButtonPipe,				// kapátko
	ButtonFill,				// výplň
	ButtonSpray,			// sprej
	ButtonText,				// text
	ButtonSelect,			// výběr
	ButtonYFlip,			// převrácení podle vertikální osy
	ButtonXFlip,			// převrácení podle horizontální osy
	ButtonRRot,				// otočení doprava
	ButtonLRot,				// otočení doleva
	ButtonXCol,				// záměna barev
	ButtonSCol,				// nastavení barvy
	ButtonMul2,				// zvětšení 2x
	ButtonDiv2,				// zmenšení 2x
	ButtonLRot45,			// otočení o 45 stupňů
	ButtonLRot30,			// otočení o 30 stupňů
	ButtonLRot22,			// otočení o 22.5 stupňů
	ButtonArrow,			// mód šipky
	ButtonMap,				// mód obrázků mapy
	ButtonSwc1,				// přepínač 1
	ButtonSwc2,				// přepínač 2
	ButtonSwc3,				// přepínač 3
	ButtonSwc4,				// přepínač 4
	ButtonSwc5,				// přepínač 5
	ButtonSwcN1,			// číselný přepínač 1
	ButtonSwcN2,			// číselný přepínač 2
	ButtonSwcN3,			// číselný přepínač 3
	ButtonPlay,				// přehrávání
	ButtonPause,			// pozastavení přehrávání
	ButtonStop,				// zastavení přehrávání
	ButtonRew,				// rychlé převíjení zpět
	ButtonFrw,				// rychlé převíjení vpřed
	ButtonLoop,				// opakované přehrávání
	ButtonRecord,			// nahrávání
	ButtonTest,				// test sprajtu
	ButtonHistPrev,			// návrat v historii zpět
	ButtonHistNext,			// návrat v historii vpřed
	ButtonFind,				// hledání
	ButtonFindPrev,			// hledání zpět
	ButtonFindNext,			// hledání vpřed
	ButtonRefDef,			// nalezení deklarace
	ButtonRefPrev,			// předchozí reference
	ButtonRefNext,			// následující reference
	ButtonLock,				// zámek
	ButtonOff,				// vypnutí funkce
	ButtonDefName,			// implicitní jméno
	ButtonDefIcon,			// implicitní ikona
	ButtonSphere,			// koule
	ButtonDimen,			// rozměry
	ButtonSpritePar,		// parametry sprajtu
	ButtonEditName,			// editace jména
	ButtonEditIcon,			// editace ikony
	ButtonLRot67,			// otočení o 67.5 stupňů
	ButtonLRot60,			// otočení o 60 stupňů
	ButtonRRot67,			// otočení o 67.5 stupňů vpravo
	ButtonRRot60,			// otočení o 60 stupňů vpravo
	ButtonRRot45,			// otočení o 45 stupňů vpravo
	ButtonRRot30,			// otočení o 30 stupňů vpravo
	ButtonRRot22,			// otočení o 22.5 stupňů vpravo
	ButtonEditBlok,			// úpravy obrázku
	ButtonRastr,			// rastr
	ButtonOtisk,			// otisk bloku
	ButtonFillMap,			// výplň plochy políčkem
	ButtonEditMap,			// editace políček plochy
	ButtonDirNew,			// nová složka programů
	ButtonMulDiv,			// volitelné zvětšení
	ButtonCheck,			// zapnut přepínač menu
	ButtonCheckOff,			// vypnut přepínač menu
	ButtonRadioOn,			// přepínač zapnut
	ButtonRadioOff,			// přepínač vypnut
	ButtonBul,				// bulharská vlajka
	ButtonCz,				// česká vlajka
	ButtonDan,				// dánská vlajka
	ButtonGer,				// německá vlajka
	ButtonRec,				// řecká vlajka
	ButtonEng,				// anglická vlajka
	ButtonSpa,				// španělská vlajka
	ButtonFin,				// finská vlajka
	ButtonFra,				// francouzská vlajka
	ButtonMad,				// maďarská vlajka
	ButtonIsl,				// islandská vlajka
	ButtonIta,				// italská vlajka
	ButtonHol,				// holandská vlajka
	ButtonNor,				// norská vlajka
	ButtonPol,				// polská vlajka
	ButtonPor,				// portugalská vlajka
	ButtonRum,				// rumunská vlajka
	ButtonRus,				// ruská vlajka
	ButtonSrb,				// srbochorvatská vlajka
	ButtonSlo,				// slovenská vlajka
	ButtonAlb,				// albánská vlajka
	ButtonSwe,				// šwédská vlajka
	ButtonTur,				// turecká vlajka
	ButtonVie,				// vietnamská vlajka
	ButtonAra,				// arabská vlajka - asi egyptská, JAKJAK
	ButtonHeb,				// hebrejská vlajka - asi izraelská

	ToolButtonNum,			// počet tlačítek celkem
};
