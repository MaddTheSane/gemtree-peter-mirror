
/***************************************************************************\
*																			*
*							Hlavní okno aplikace							*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// globální promìnné

extern	HWND		MainFrame;				// hlavní okno aplikace
extern	HMENU		Menu;					// aktivní menu v hlavním oknì
extern	HWND		ToolBar;				// nástrojová lišta

extern	HBITMAP		ToolBarBmp;				// bitmapa panelu nástrojù

extern	RECT		ClientRect;				// klientská oblast hlavního okna
extern	int			ClientWidth;			// šíøka klientské oblasti
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
extern	HCURSOR		CurFill;				// výplò
extern	HCURSOR		CurFillElip;			// ovál
extern	HCURSOR		CurFillRect;			// vyplnìný obdélník
extern	HCURSOR		CurFillRound;			// kruh
extern	HCURSOR		CurKapatko;				// kapátko
extern	HCURSOR		CurKoule;				// koule
extern	HCURSOR		CurLine;				// èára
extern	HCURSOR		CurMove;				// pøesun
extern	HCURSOR		CurNoDrag;				// zákaz tažení
extern	HCURSOR		CurPaint;				// štìtec
extern	HCURSOR		CurPen;					// pero
extern	HCURSOR		CurRect;				// obdélník
extern	HCURSOR		CurRound;				// kružnice
extern	HCURSOR		CurRuka;				// ruka
extern	HCURSOR		CurSelect;				// výbìr bloku
extern	HCURSOR		CurSelectMove;			// pøesun výbìru
extern	HCURSOR		CurSplitH;				// rozdìlení horizontálnì
extern	HCURSOR		CurSplitV;				// rozdìlení vertikálnì
extern	HCURSOR		CurSpray;				// sprej
extern	HCURSOR		CurWait;				// èekání

// stav myši (aktualizováno v PreTranslateMessage)
extern	BOOL		LMouseDown;				// levé tlaèítko stisknuto
extern	BOOL		LMouseDClick;			// dvojklik levého tlaèítka
extern	BOOL		RMouseDown;				// pravé tlaèítko stisknuto
extern	BOOL		RMouseDClick;			// dvojklik pravého tlaèítka
extern	HWND		MouseWnd;				// okno pod kurzorem myši
extern	POINT		MouseClient;			// souøadnice myši v oknì pod kurzorem
extern	POINT		MouseScreen;			// souøadnice myši na displeji
extern	POINT		MouseMain;				// souøadnice myši v hlavním oknì

extern	HWND		StatusBar;				// stavová lišta
extern	CText		StatusText;				// zobrazený text stavové lišty
extern	CText		StatusText2;			// zobrazený text stavové lišty 2
extern	int			StatusWidth;			// šíøka panelu 2 stavové lišty
extern	BOOL		StatusBig;				// možnost použít velké fonty stavové lišty

/////////////////////////////////////////////////////////////////////////////
// menu

// definice položky menu
typedef struct MENUITEM_ 
{
	int		id0;							// identifikace jména položky (0=separátor nebo konec)
	int		id;								// identifikace pøíkazu a nápovìdy položky (0=následuje podmenu nebo konec)
	int		bitmap;							// èíslo ikony s obrázkem (-1 = není)
	int		typ;							// typ položky - kombinace pøíznakù (0=nic)
} MENUITEM;

#define MSWC	1							// je spínaè
#define MRAD	2							// je pøepínaè

// informace o položce spuštìného menu
typedef struct MENUAKTITEM_
{
	int		id0;							// identifikace jména položky (0=separátor nebo konec)
	int		id;								// identifikace pøíkazu a nápovìdy položky (0=následuje podmenu nebo konec)
	int		bitmap;							// èíslo ikony s obrázkem (-1 = není)
	int		typ;							// typ položky - kombinace pøíznakù (0=nic)
	HMENU	menu;							// menu s položkou
	BOOL	main;							// je to položka hlavního menu
	int		inx;							// index položky v menu
	int		width;							// šíøka položky (s pøihlédnutím k ostatním položkám)
	int		height;							// výška položky
	CText	text;							// text položky v menu (bez zkratkové klávesy)
	CText	textshort;						// text zkratkové klávesy
	int		key;							// horká klávesa - velké písmeno (0=není)
	CText	help;							// dlouhá nápovìda
	CText	help2;							// krátká nápovìda (max. 79 znakù)
} MENUAKTITEM;


extern MENUITEM** MenuAkt;					// aktuální menu (kvùli cachování, NULL = aktualizovat vždy)

extern MENUITEM MenuSoubor1[];				// soubor 1
extern MENUITEM MenuSoubor2[];				// soubor 2
extern MENUITEM MenuEditace[];				// editace
extern MENUITEM MenuZobraz[];				// zobrazení
extern MENUITEM MenuProgram[];				// program
extern MENUITEM MenuObrazek[];				// obrázek
extern MENUITEM MenuIkona[];				// ikona
extern MENUITEM MenuNastroje[];				// nástroje
extern MENUITEM MenuTloustka[];				// tlouška pera
extern MENUITEM MenuPlocha[];				// plocha
extern MENUITEM MenuSprajt[];				// sprajt
extern MENUITEM MenuSound[];				// zvuk
extern MENUITEM MenuMusic[];				// hudba
extern MENUITEM MenuHlp[];					// nápovìda

/////////////////////////////////////////////////////////////////////////////
// nastavení nového menu

void SetNewMenu(MENUITEM** item);


/////////////////////////////////////////////////////////////////////////////
// pøíprava jména hlavního okna

void MainFrameNameInit();


/////////////////////////////////////////////////////////////////////////////
// vytvoøení hlavního okna aplikace

BOOL MainFrameCreate();


/////////////////////////////////////////////////////////////////////////////
// nastavení fontu stavového øádku

void SetStatusFont();


/////////////////////////////////////////////////////////////////////////////
// zobrazení jména souboru v titulku okna

void ZobrazJmeno();


/////////////////////////////////////////////////////////////////////////////
// povolení volby v menu a v panelu nástrojù

void EnableCommand(int id, BOOL enable);

/////////////////////////////////////////////////////////////////////////////
// zapnutí volby v menu a v panelu nástrojù

void CheckCommand(int id, BOOL check);

/////////////////////////////////////////////////////////////////////////////
// nastavení/aktualizace šíøky pole 2 stavové lišty

void SetStatusWidth(int width);


/////////////////////////////////////////////////////////////////////////////
// nastavení textu stavové lišty

void SetStatusText(const CText& text);


/////////////////////////////////////////////////////////////////////////////
// nastavení textu stavové lišty 2

void SetStatusText2(const CText& text);


/////////////////////////////////////////////////////////////////////////////
// zrušení všech tlaèítek panelu nástrojù (až po zadané minimální množství)

void ToolBarClear(int min);


/////////////////////////////////////////////////////////////////////////////
// pøidání tlaèítek do panelu nástrojù

void ToolBarAdd(TBBUTTON* button, int num);


/////////////////////////////////////////////////////////////////////////////
// nastavení obrázku tlaèítka v panelu nástrojù

void ToolBarBitmap(int id, int bitmap);


/////////////////////////////////////////////////////////////////////////////
// aktualizace zobrazení panelu nástrojù

void ToolBarResize();

/////////////////////////////////////////////////////////////////////////////
// aktualizace klientských souøadnic

void InitClientRect();

/////////////////////////////////////////////////////////////////////////////
// obsluha informaèního okna About

BOOL CALLBACK AboutBoxProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

void OnAboutBox();

/////////////////////////////////////////////////////////////////////////////
// vytvoøení kombinovaného pole v panelu nástrojù

HWND CreateToolCombo(int id, int* items, int index);

/////////////////////////////////////////////////////////////////////////////
// výbìr prvku v COMBO boxu

void SelectCombo(HWND combo, int id, int* items);

/////////////////////////////////////////////////////////////////////////////
// zobrazení kurzoru èekání (zahájení a ukonèení musí být do páru!)

void BeginWaitCursor();

/////////////////////////////////////////////////////////////////////////////
// vypnutí kurzoru èekání (zahájení a ukonèení musí být do páru!)

void EndWaitCursor();

/////////////////////////////////////////////////////////////////////////////
// obsluha zprávy pøed rozesláním do oken

BOOL PreTranslateMessage(MSG* msg);


/////////////////////////////////////////////////////////////////////////////
// èasové pøekreslení okna (zadaného bufferu, -1=vše)

void TimeRepaint(int bufID);


/////////////////////////////////////////////////////////////////////////////
// aktualizace pøedešlého okna

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
// vlastní vykreslení prvkù v dialogových oknech (vrací TRUE=obslouženo)

BOOL DialogDraw(HWND hWnd, LPARAM lParam);


/////////////////////////////////////////////////////////////////////////////
// nastavení vlastního pøepínaèe RADIO

void DialogCustomRadio(HWND hWnd, DWORD id);


/////////////////////////////////////////////////////////////////////////////
// nastavení vlastního pøepínaèe CHECK

void DialogCustomCheck(HWND hWnd, DWORD id);


/////////////////////////////////////////////////////////////////////////////
// nastavení vlastního rámeèku

void DialogCustomBox(HWND hWnd, DWORD id);


/////////////////////////////////////////////////////////////////////////////
// nastavení hodnoty pøepínaèe

void DialogSetCheck(HWND hWnd, DWORD id, BOOL check);


/////////////////////////////////////////////////////////////////////////////
// nastavení smìru textu zprava doleva   JAKJAK

void SetRightToLeft(BOOL righttoleft);


/////////////////////////////////////////////////////////////////////////////
// indexy obrázkù tlaèítek pro ToolBar

enum ToolButton {
	ButtonStart,			// start
	ButtonNew,				// nový soubor
	ButtonOpen,				// otevøení souboru
	ButtonClose,			// uzavøení souboru
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
	ButtonPaint,			// štìtec
	ButtonLine,				// èára
	ButtonBox,				// obdélník
	ButtonFillBox,			// vyplnìní obdélník
	ButtonCircle,			// kružnice
	ButtonFillCircle,		// kruh
	ButtonEllipse,			// elipsa
	ButtonFillEllipse,		// ovál
	ButtonWidth1,			// šíøka 1
	ButtonWidth2,			// šíøka 2
	ButtonWidth3,			// šíøka 3
	ButtonWidth4,			// šíøka 4
	ButtonWidth5,			// šíøka 5
	ButtonPipe,				// kapátko
	ButtonFill,				// výplò
	ButtonSpray,			// sprej
	ButtonText,				// text
	ButtonSelect,			// výbìr
	ButtonYFlip,			// pøevrácení podle vertikální osy
	ButtonXFlip,			// pøevrácení podle horizontální osy
	ButtonRRot,				// otoèení doprava
	ButtonLRot,				// otoèení doleva
	ButtonXCol,				// zámìna barev
	ButtonSCol,				// nastavení barvy
	ButtonMul2,				// zvìtšení 2x
	ButtonDiv2,				// zmenšení 2x
	ButtonLRot45,			// otoèení o 45 stupòù
	ButtonLRot30,			// otoèení o 30 stupòù
	ButtonLRot22,			// otoèení o 22.5 stupòù
	ButtonArrow,			// mód šipky
	ButtonMap,				// mód obrázkù mapy
	ButtonSwc1,				// pøepínaè 1
	ButtonSwc2,				// pøepínaè 2
	ButtonSwc3,				// pøepínaè 3
	ButtonSwc4,				// pøepínaè 4
	ButtonSwc5,				// pøepínaè 5
	ButtonSwcN1,			// èíselný pøepínaè 1
	ButtonSwcN2,			// èíselný pøepínaè 2
	ButtonSwcN3,			// èíselný pøepínaè 3
	ButtonPlay,				// pøehrávání
	ButtonPause,			// pozastavení pøehrávání
	ButtonStop,				// zastavení pøehrávání
	ButtonRew,				// rychlé pøevíjení zpìt
	ButtonFrw,				// rychlé pøevíjení vpøed
	ButtonLoop,				// opakované pøehrávání
	ButtonRecord,			// nahrávání
	ButtonTest,				// test sprajtu
	ButtonHistPrev,			// návrat v historii zpìt
	ButtonHistNext,			// návrat v historii vpøed
	ButtonFind,				// hledání
	ButtonFindPrev,			// hledání zpìt
	ButtonFindNext,			// hledání vpøed
	ButtonRefDef,			// nalezení deklarace
	ButtonRefPrev,			// pøedchozí reference
	ButtonRefNext,			// následující reference
	ButtonLock,				// zámek
	ButtonOff,				// vypnutí funkce
	ButtonDefName,			// implicitní jméno
	ButtonDefIcon,			// implicitní ikona
	ButtonSphere,			// koule
	ButtonDimen,			// rozmìry
	ButtonSpritePar,		// parametry sprajtu
	ButtonEditName,			// editace jména
	ButtonEditIcon,			// editace ikony
	ButtonLRot67,			// otoèení o 67.5 stupòù
	ButtonLRot60,			// otoèení o 60 stupòù
	ButtonRRot67,			// otoèení o 67.5 stupòù vpravo
	ButtonRRot60,			// otoèení o 60 stupòù vpravo
	ButtonRRot45,			// otoèení o 45 stupòù vpravo
	ButtonRRot30,			// otoèení o 30 stupòù vpravo
	ButtonRRot22,			// otoèení o 22.5 stupòù vpravo
	ButtonEditBlok,			// úpravy obrázku
	ButtonRastr,			// rastr
	ButtonOtisk,			// otisk bloku
	ButtonFillMap,			// výplò plochy políèkem
	ButtonEditMap,			// editace políèek plochy
	ButtonDirNew,			// nová složka programù
	ButtonMulDiv,			// volitelné zvìtšení
	ButtonCheck,			// zapnut pøepínaè menu
	ButtonCheckOff,			// vypnut pøepínaè menu
	ButtonRadioOn,			// pøepínaè zapnut
	ButtonRadioOff,			// pøepínaè vypnut
	ButtonBul,				// bulharská vlajka
	ButtonCz,				// èeská vlajka
	ButtonDan,				// dánská vlajka
	ButtonGer,				// nìmecká vlajka
	ButtonRec,				// øecká vlajka
	ButtonEng,				// anglická vlajka
	ButtonSpa,				// španìlská vlajka
	ButtonFin,				// finská vlajka
	ButtonFra,				// francouzská vlajka
	ButtonMad,				// maïarská vlajka
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

	ToolButtonNum,			// poèet tlaèítek celkem
};
