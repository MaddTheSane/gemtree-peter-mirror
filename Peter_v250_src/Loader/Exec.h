
/***************************************************************************\
*																			*
*							Provádění programu								*
*																			*
\***************************************************************************/

// ------- konstanty

// řízení programu
#define WHILEOUT	50				// čítač time-out cyklu
#define REDISPOUT	4				// čítač time-out překreslení displeje
#define MODULMERITKA 32				// modul rozlišení měřítka (musí být dělitelem 32 !!!!)
#define MODULMERITKA2 4				// modul rozlišení měřítka pro celoobrazovkový mód
#define DEFCOLOR 0x00ffffff			// implicitní barva pro grafiku
#define MAXHLOUBKA	200				// maximální hloubka vnoření volání funkcí

// obsluha souborů
#define FILECLOSETIME	10;			// čas pro uzavření souborů
#define FILEBUFFER		0xf000		// velikost souborového bufferu

// bity pro přerušení (Break)
#define BREAKPROG	0x1				// úplné přerušení programu
#define BREAKFUNC	0x2				// přerušení funkce
#define BREAKWHILE	0x4				// přerušení cyklu

// obsluha klávesnice
#define KEYBUFSIZE		20			// velikost bufferu kláves
#define KEYMAPSIZE		256			// velikost mapy stisknutých kláves
#define VK_RETURN_NUM	0xad		// ENTER v numerické klávesnici

// ------ proměnné

// buffer prováděného programu
extern	int			ProgStart;		// index volání hlavní funkce
extern	EXECITEM*	ProgBuf;		// buffer programu
extern	int			ProgNum;		// počet položek programu
extern	int			ProgMax;		// velikost bufferu programu
extern	EXECITEM*	ExecItem;		// ukazatel prováděného programu
extern	int			Hloubka;		// hloubka vnoření do funkcí

// buffer obrázků
extern BYTE*	BackBuf;			// buffer obrázku pozadí
extern BYTE*	PicBuf;				// buffer obrázku sestavy (s ikonami a sprajty)
extern BYTE*	TextBuf;			// buffer pro výstup textu

// aktualizace překreslení ikon
extern bool*	AktBuf;				// buffer požadavků k překreslení ikon

extern int		AktLeft;			// levý okraj k překreslení - včetně (v bodech)
extern int		AktTop;				// horní okraj k překreslení - včetně (v bodech)
extern int		AktRight;			// pravý okraj k překreslení - bez (v bodech)
extern int		AktBottom;			// dolní okraj k překreslení - bez (v bodech)

extern	bool	IsOverlay;			// zobrazeno překryvné okno Windows (otevření souborů atd.)

// aktivní zobrazená plocha
extern int		WidthN;				// šířka plochy v ikonách
extern int		HeightN;			// výška plochy v ikonách
extern int		Width;				// šířka plochy v bodech
extern int		Height;				// výška plochy v bodech
extern int		WidthByte;			// délka linky v bajtech
extern int		RowByte;			// délka řádku ikon v bajtech
extern int		DispLeft;			// levý okraj k zobrazení v okně
extern int		DispTop;			// horní okraj k zobrazení v okně
extern int		DispWidth;			// zobrazená šiřka v bodech
extern int		DispHeight;			// zobrazená výška v bodech

extern	double	Meritko;			// měřítko zobrazení
extern	double	Meritko0;			// uschované měřítko zobrazení pro normální okno

// zobrazení textu
extern	int		GraphAreaWidth;		// šířka pole k zobrazení textu centrovaně (v bodech)

// nadpis a stavový řádek
extern	bool	CaptionAkt;			// požadavek k překreslení nadpisu okna
extern	CString	CaptionText;		// nový text titulku k nastavení
extern	bool	StatusAkt;			// požadavek k překreslení stavového řádku
extern	CString	StatusText;			// nový stavový text k nastavení

// buffer sprajtů
extern	CBufInt	SpriteVisible;		// buffer indexů viditelných sprajtů
extern	CBufInt	SpriteMoving;		// buffer indexů pohybujících se sprajtů
extern	CBufInt	SpriteDispReq;		// buffer sprajtů vyžadujících překreslení
extern	CBufInt	SpriteDispLevel;	// hladiny sprajtů vyžadujících překreslení

extern	int		SpriteWinX1;		// levý okraj okna sprajtů
extern	int		SpriteWinY1;		// dolní okraj okna sprajtů
extern	int		SpriteWinX2;		// pravý okraj okna sprajtů
extern	int		SpriteWinY2;		// horní okraj okna sprajtů

extern	int		WhileOut;			// čítač time-out cyklu
extern	int		ReDispOut;			// čítač time-out překreslení displeje

// obsluha myši
extern	bool	MouseValid;			// příznak platnosti údajů o myši

// obsluha časovače
extern	UINT	TimerID;			// ID časovače
extern	bool	TimerReq;			// přišel signál od časovače

extern	bool	Pause;				// příznak zapaouzování programu
extern	volatile int Break;			// příznaky přerušení (0 = není přerušení)

extern	double		MouseX;			// souřadnice myši X (v ikonových souřadnicích)
extern	double		MouseY;			// souřadnice myši Y (v ikonových souřadnicích)

// obsluha CD
#define CDAKTTIME		18			// čas pro aktualizace údajů o CD
#define CDERRORTIME		(3*18)		// čas pro nový pokus při chybě
#define CDCLOSETIME		30			// čas pro uzavření přístupu k CD (a zneplatnění informací o disku)
#define CDDEFLENGTH		(60*60*1000) // implicitní délka CD
#define CDMAXLENGTH		(24*60*60*1000) // maximální délka CD
#define CDMAXTRACKS		999			// maximální počet stop CD

extern	UINT	CDDevice;			// ID zařízení MCI pro přehrávání CD (0=není otevřeno)
extern	int		CDError;			// čítač pro nový pokus při chybě (musí být < 0)
extern	int		CDValidTime;		// čítač platnosti údajů o CD (musí být > 0)
extern	bool	CDDiskValid;		// údaje o disku jsou platné (nebyl vyměněn disk)
extern	int		CDStart;			// startovací pozice CD v ms
extern	int		CDLength;			// délka CD v ms
extern	int		CDTracks;			// počet stop CD
extern	int		CDPos;				// přehrávaná pozice CD v ms
extern	int		CDTrack;			// přehrávaná stopa
extern	int		CDTrackLength;		// délka přehrávané stopy v ms
extern	int		CDTrackPos;			// pozice v přehrávané stopě v ms
extern	bool	CDPlaing;			// příznak přehrávání CD
extern	bool	CDPausing;			// příznak pauzy CD

// obsluha klávesnice
extern	int		KeyBufWrite;		// ukládací index do bufferu klávesnice
extern	int		KeyBufRead;			// čtecí index z bufferu klávesnice
extern	int		KeyBuf[KEYBUFSIZE];	// buffer klávesnice

extern	int		CharBufWrite;		// ukládací index do bufferu znaků
extern	int		CharBufRead;		// čtecí index z bufferu znaků
extern	int		CharBuf[KEYBUFSIZE];// buffer znaků

extern	bool	CtrlAgain;			// příznak opakovaného stisku Ctrl

extern	int		KeyMapNum;			// počet stisknutých kláves
extern	bool	KeyMap[KEYMAPSIZE];	// mapa stisknutých kláves

extern	bool	NumLock;			// stav Num Lock
extern	bool	CapsLock;			// stav Caps Lock
extern	bool	ScrollLock;			// stav Scroll Lock
extern	bool	InsertLock;			// stav Insert

extern	bool	AktNumLock;			// požadavek nastavení Num Lock
extern	bool	AktCapsLock;		// požadavek nastavení Caps Lock
extern	bool	AktScrollLock;		// požadavek nastavení Scroll Lock
extern	bool	AktInsertLock;		// požadavek nastavení Insert

// obsluha souborů
extern	bool	FileError;			// příznak chyby souborů
extern	int		FileCloseTime;		// čítač času pro uzavření souborů
extern	int		FileTextNLen;		// délka textu s pevnou délkou
extern	bool	FileTextUnicode;	// text je v kódu UNICODE

extern	CString	FileRead;			// jméno souboru pro čtení
extern	HANDLE	FileReadHandle;		// handle souboru pro čtení
extern	int		FileReadOff;		// offset v souboru pro čtení
extern	BYTE*	FileReadBuf;		// buffer pro čtení
extern	int		FileReadBufOff;		// offset bufferu pro čtení
extern	int		FileReadBufN;		// počet bajtů v bufferu pro čtení

extern	CString	FileWrite;			// jméno souboru pro zápis
extern	HANDLE	FileWriteHandle;	// handle souboru pro zápis
extern	int		FileWriteOff;		// offset v souboru pro zápis
extern	BYTE*	FileWriteBuf;		// buffer pro zápis
extern	int		FileWriteBufOff;	// offset bufferu pro zápis
extern	int		FileWriteBufN;		// počet bajtů v bufferu pro zápis

extern	CString	AliasName;			// jméno souboru ALIASES
extern	CString	AliasKey;			// klíč ALIASES
extern	CString	AliasGroup;			// skupina ALIASES

extern	__int64	DiskSize;			// velikost disku (z funkce GetDiskSpace)
extern	__int64	DiskFree;			// volné místo disku (z funkce GetDiskSpace)
extern	__int64	DiskFreeUser;		// volné místo uživatele (z funkce GetDiskSpace)

// konzola
extern	bool	ConsoleOn;			// příznak režimu konzoly

// joystick
#define MAXJOYSTICK 4						// max. počet joysticků
#define JOYSTICKTIME (5*18)					// doba do dalšího testu joysticku
extern	int		JoystickNext[MAXJOYSTICK];	// čítač do dalšího testu (musí být < 0)
extern	bool	JoystickValid[MAXJOYSTICK];	// příznak platnosti údajů joysticku
extern	double	JoystickX[MAXJOYSTICK];		// souřadnice X
extern	double	JoystickY[MAXJOYSTICK];		// souřadnice Y
extern	double	JoystickZ[MAXJOYSTICK];		// souřadnice Z
extern	bool	Joystick1[MAXJOYSTICK];		// tlačítko 1
extern	bool	Joystick2[MAXJOYSTICK];		// tlačítko 2
extern	bool	Joystick3[MAXJOYSTICK];		// tlačítko 3
extern	bool	Joystick4[MAXJOYSTICK];		// tlačítko 4

// prvky oken
extern	CString ButtonClassName;
extern	CString EditClassName;
extern	CString RichEditClassName;
extern	CString ListBoxClassName;
extern	CString ComboBoxClassName;
extern	CString StaticClassName;
extern	CString TabsClassName;

extern	bool	DialogGraph;			// je grafické pozadí hlavního dialogového okna

extern	bool	DialogMode;				// dialogový mód
extern	int		DialogParent;			// aktivní rodičovské okno
extern	int		WindowID;				// aktivní prvek (-1 = neplatný)
extern	int		WindowFocus;			// prvek s fokusem (-1 = není nebo neznámý)
extern	int		ButtonEsc;				// tlačítko pro Esc

extern	int		StdColorBtnFace;		// systémová barva podkladu tlačítek
extern	int		StdColorBtnText;		// systémová barva textu tlačítek
extern	HBRUSH	StdBrushBtn;			// štětec pozadí tlačítek

extern	int		StdColorWindow;			// systémová barva podkladu editoru
extern	int		StdColorWindowText;		// systémová barva textu editoru
extern	HBRUSH	StdBrushWindow;			// štětec pozadí editoru

extern	int		StdColorHighlightText;	// systémová barva textu kurzoru
extern	int		StdColorHighlight;		// systémová barva pozadí kurzoru
extern	HBRUSH	StdBrushHighlight;		// štětec pozadí kurzoru

// správce fontů
typedef struct FONTITEM_
{
	bool	Bold;					// je zvýraznění
	bool	Italic;					// je kurzíva
	bool	Underline;				// je podtržení
	bool	Strikeout;				// je přeškrtnutí
	bool	Serif;					// jsou patičky
	bool	Fixed;					// je pevná rozteč
	int		Height;					// bodová výška znaků (0=běžná)
	double	Width;					// relativní šířka znaků (0=běžná, 1=základní)
	double	Angle;					// úhel sklonu v radiánech (0=běžný)
	HFONT	Font;					// vytvořený font (NULL=volná položka)
	DWORD	CharSet;				// znaková sada fontu
	CString	UserFont;				// jméno uživatelského fontu
	int		Used;					// čítač použití fontu (blokování proti zrušení)
} FONTITEM;

#define FONTTABSIZE	64				// velikost tabulky fontů
#define	FONTTABSTD	4				// počet standardních fontů na začátku tabulky
extern	int			FontNext;		// index příštího volného fontu k uložení
extern	FONTITEM*	FontTab;		// tabulka fontů
extern	HFONT		FontDefault;	// implicitní systémový font (při chybě fontů)
extern	CString		UserFont;		// uživatelský font
extern	CString		FontList;		// seznam systémových fontů

// oblasti kurzoru myši
typedef struct MOUSEREG_
{
	int			x1;						// (4) počátek oblasti vlevo (včetně)
	int			y1;						// (4) počátek oblasti dole (včetně)
	int			x2;						// (4) konec oblasti vpravo (bez)
	int			y2;						// (4) konec oblasti nahoře (bez)
	HCURSOR		Cursor;					// (4) handle kurzoru myši
	int			res;					// (4) ... rezerva pro zarovnání
} MOUSEREG;

extern	MOUSEREG*	MouseReg;			// regiony kurzoru myši
extern	int			MouseRegNum;		// počet regionů kurzoru myši
extern	int			MouseRegMax;		// velikost bufferu regionů kurzoru myši

// DirectPlay
// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI

extern	bool		Coinit;				// proběhla inicializace COM knihovny

extern	CString	DirectPlayConnects;		// seznam spojení DirectPlay
extern	CString	DirectPlayConnectsAkt;	// aktivní spojení (prázdné = nevybráno)
extern	CString	DirectPlayGames;		// seznam her DirectPlay
extern	CString	DirectPlayGamesAkt;		// aktivní hra (prázdné = nevybráno)
extern	DWORD	DirectPlayGamesParam[4];// parametry hry (15 bitů číslo + 0x4000, 16. bit = příznak)
extern	bool	DirectPlayGamesHost;	// počítač je hostitelem
extern	int		DirectPlayPlayersMax;	// maximální počet hráčů (0 = neomezeno)
extern	CString	DirectPlayPlayers;		// seznam hráčů DirectPlay
extern	CString	DirectPlayPlayersAkt;	// aktivní hráč (prázdné = nevybrán)
extern	int		DirectPlayPlayersAktN;	// aktivní hráč (-1 = není)
extern	BYTE*	DirectPlayDataIn;		// datový blok vstupních dat (první 2 bajty = odesílatel)
extern	int		DirectPlayDataInN;		// velikost vstupních dat
extern	int		DirectPlayDataInO;		// offset čtení ze vstupních dat

extern	GUID DirectPlayGuidData;		// identifikátor spojení DirectPlay
extern	CString	DirectPlayGuidText;		// identifikační text pro DirectPlay

// WSA 1.1 rozhraní pro DirectPlay a UDP
typedef	int (PASCAL FAR *WSASTARTUP) (WORD wVersionRequired, LPWSADATA lpWSAData);
typedef	int (PASCAL FAR *WSACLEANUP) ();
typedef	int (PASCAL FAR *WSAGETHOSTNAME) (char FAR* name, int namelen);
typedef	struct hostent FAR* (PASCAL FAR *WSAGETHOSTBYNAME) (const char FAR* name);
typedef	char FAR* (PASCAL FAR *WSAINET_NTOA) (struct in_addr in);

extern	WSASTARTUP	pWSAStartup;		// funkce "WSAStartup"
extern	WSACLEANUP	pWSACleanup;		// funkce "WSACleanup"
extern	WSAGETHOSTNAME	pWSAGetHostName;// funkce "gethostname"
extern	WSAGETHOSTBYNAME pWSAGetHostByName;// funkce "gethostbyname"
extern	WSAINET_NTOA pWSAInetNtoa;		// funkce "inet_ntoa"
// PC formát IP adresy: první číslo je nejvýše, tj. např. 127.0.0.1 je 0x7f000001
// WSA formát IP adresy je v MAC formátu: např. 127.0.0.1 je 0x0100007f

// rozšíření pro UDP
typedef SOCKET (PASCAL FAR *WSASOCKET) (int af, int type, int protocol);
typedef WORD (PASCAL FAR *WSAHTONS) (WORD hostshort);
typedef int (PASCAL FAR *WSASETSOCKOPT) (SOCKET s, int level, int optname,
								const char FAR * optval, int optlen);
typedef int (PASCAL FAR *WSABIND) (SOCKET s, const struct sockaddr FAR * name, int namelen);
typedef int (PASCAL FAR *WSACLOSESOCKET) (SOCKET s);
typedef int (PASCAL FAR *WSASENDTO) (SOCKET s, const char FAR * buf, int len,
				int flags, const struct sockaddr FAR * to, int tolen);
typedef int (PASCAL FAR *WSARECVFROM) (SOCKET s, char FAR * buf, int len,
				int flags, struct sockaddr FAR * from, OUT int FAR * fromlen);
typedef DWORD (PASCAL FAR *WSAINETADDR) (const char FAR * cp);
typedef int (PASCAL FAR *WSASELECT) (int nfds, fd_set FAR * readfds, fd_set FAR * writefds,
				fd_set FAR *exceptfds, const struct timeval FAR * timeout);

extern	WSASOCKET pWSASocket;			// funkce "socket"
extern	WSAHTONS pWSAhtons;				// funkce "htons" (převede WORD číslo portu - na PC prohodí bajty)
extern	WSASETSOCKOPT pWSASetSockOpt;	// funkce "setsockopt"
extern	WSABIND pWSABind;				// funkce "bind"
extern	WSACLOSESOCKET pWSACloseSocket;	// funkce "closesocket"
extern	WSASENDTO pWSASendTo;			// funkce "sendto"
extern	WSARECVFROM pWSARecvFrom;		// funkce "recvfrom"
extern	WSAINETADDR pWSAInetAddr;		// funkce "inet_addr"
extern	WSASELECT pWSASelect;			// funkce "select"

// knihovna WSA
extern	HINSTANCE WSALibrary;			// WSA knihovna
extern	bool	Wsaload;				// knihovna WSA byla načítána
extern	bool	Wsainit;				// knihovna WSA 1.1 úspěšně inicializována
extern	bool	Wsainit2;				// rozšíření pro UDP úspěšně inicializováno
extern	WSADATA	Wsadata;				// data knihovny WSA
extern	CString	HostIP;					// IP adresa počítače
extern	int		HostIPValid;			// čítač platnosti IP adresy počítače (je-li > 0)

// porty (není u MINI verze)
typedef struct COMITEM_
{
	HANDLE		File;					// identifikátor (INVALID_HANDLE_VALUE = nepoužitý)
	DCB			Dcb;					// informace o portu
} COMITEM;

#define	COMSMAX		64					// max. počet portů
extern	COMITEM*	Coms;				// tabulky portů
extern	int			ComAkt;				// aktivní port

// mixer (není u MINI verze)
typedef struct MIXDEV_
{
	CString	Name;						// jméno mixážního zařízení
	int		Dest;						// počet cílových signálů
} MIXDEV;

typedef struct MIXLINE_
{
	CString Name;						// jméno signálu
	int		Dest;						// index cílového signálu
	int		Source;						// index zdrojového signálu (-1=jen cílový)
	int		SourceI;					// index počátečního zdrojového signálu
	DWORD	LineID;						// identifikátor signálu
	DWORD	Type;						// typ signálu
	int		Channels;					// počet samostatných kanálů
	int		Sources;					// počet připojených zdrojových signálů
	int		Controls;					// počet ovládacích prvků signálu
} MIXLINE;

typedef struct MIXCTRL_
{
	CString	Name;						// jméno ovládacího prvku
	DWORD	ControlID;					// identifikátor ovládacího prvku
	DWORD	Type;						// typ ovládacího prvku
	int		Steps;						// počet kroků nastavení hodnoty bez počáteční 0 (0=maximálně)
	int		Channels;					// počet samostatných kanálů
	int		List;						// počet prvků pro seznam (1=není seznam)
	double	Min;						// minimální hodnota prvku
	double	Max;						// maximální hodnota prvku
	double	Rozsah;						// rozsah hodnot prvku
	double	Inc;						// přírustek kroku prvku
} MIXCTRL;

extern	int		MixDevN;				// počet mixážních zařízení
extern	int		MixDevA;				// aktivní mixážní zařízení (-1=není)
extern	CString	MixDevT;				// víceřádkový seznam mixážních zařízení
extern	MIXDEV*	MixDev;					// seznam mixážních zařízení
extern	HMIXER	MixDevH;				// handle mixeru (NULL=není)

extern	int		MixDest;				// počet cílových signálů mixeru
extern	int		MixLineN;				// počet signálů mixeru
extern	MIXLINE* MixLine;				// seznam signálů (na začátku jsou cílové)
extern	CString	MixLineDT;				// víceřádkový seznam cílových signálů
extern	CString	MixLineST;				// víceřádkový seznam zdrojových signálů

extern	int		MixLineDA;				// aktivní cílový signál
extern	int		MixLineSA;				// aktivní zdrojový signál (-1=jen cílový)
extern	int		MixLineA;				// aktivní signál absolutně (v tabulce)

extern	int		MixCtrlN;				// počet ovládacích prvků signálu
extern	MIXCTRL* MixCtrl;				// seznam ovládacích prvků
extern	CString	MixCtrlT;				// víceřádkový seznam ovládacích prvků
extern	int		MixCtrlA;				// aktivní ovládací prvek (-1=není)
extern	int		MixChannel;				// nastavovaný kanál (-1=všechny)

extern	int		MixValN;				// počet hodnot prvku (1=není seznam hodnot)
extern	CString* MixVal;				// buffer jmen ovládacích prvků
extern	CString	MixValT;				// seznam hodnot prvku seznamu
extern	int		MixValA;				// aktivní prvek seznamu

// obsluha DDE
extern	CString		DDEDefApp;			// implicitní jméno aplikace pro DDE

typedef struct DDEAPPITEM_				// položka aplikace
{
	CString		AppName;				// jméno aplikace
	int			ServFirst;				// index prvního serveru s tímto jménem aplikace
} DDEAPPITEM;

extern	DDEAPPITEM*	DDEAppList;			// seznam aplikací
extern	int			DDEAppNum;			// počet aplikací
extern	int			DDEAppMax;			// velikost bufferu aplikací
extern	bool		DDEAppLoad;			// načítá se seznam aplikací

extern	int			DDEAppAkt;			// aktivní aplikace (-1=všechny)
extern	int			DDEServAkt;			// aktivní server (-1=všechny)
extern	CString		DDEAppAktName;		// jméno zadané aktivní aplikace/serveru

typedef	struct DDESERVITEM_				// položka serveru
{
	int			AppIndex;				// index aplikace
	CString		ServName;				// plné jméno serveru s handle okna ([12345678] Name)
	int			ServNext;				// index dalšího serveru s tímto jménem aplikace (-1=není)
	HWND		ServHandle;				// handle okna serveru
	int			ServServ;				// počet témat, pro které je to aktivní server
	CString		Execute;				// příkaz k provedení	
	int			TopicFirst;				// index prvního tématu tohoto serveru
} DDESERVITEM;

extern	DDESERVITEM* DDEServList;		// seznam severů
extern	int			DDEServNum;			// počet serverů
extern	int			DDEServMax;			// velikost bufferu serverů

typedef struct DDETOPITEM_				// položka tématu
{
	int			ServIndex;				// index serveru
	CString		TopicName;				// jméno tématu
	int			TopicNext;				// index dalšího tématu serveru (-1=není)
	int			TopicServ;				// počet položek, pro které je téma serverem
	int			DataFirst;				// index první položky tohoto tématu (-1=není)
} DDETOPITEM;

extern	DDETOPITEM*	DDETopList;			// seznam témat
extern	int			DDETopNum;			// počet témat
extern	int			DDETopMax;			// velikost bufferu témat
extern	int			DDETopAkt;			// aktivní téma (-1=všechny)

typedef struct DDEDATAITEM_				// položka tabulky prvků DDE
{
	int			TopicIndex;				// index tématu
	CString		DataName;				// jméno prvku
	int			DataNext;				// index dalšího prvku tématu (-1=není)
	int			DataType;				// typ prvku (0 až 7, -1=neplatný)
	double		DataNumber;				// 0: číslo
	bool		DataBool;				// 1: příznak
	bool		Server;					// prvek je serverem
	bool		res2;
	bool		res3;
	CString		DataText;				// 2: text
	CIcon		DataIcon;				// 3: ikona
	CPicture	DataPicture;			// 4: obrázek
	CSound		DataSound;				// 5: zvuk
	CMusic		DataMusic;				// 6: hudba
	CSprite		DataSprite;				// 7: sprajt

} DDEDATAITEM;

extern	DDEDATAITEM* DDEDataList;		// seznam datových položek
extern	int			DDEDataNum;			// počet datových položek
extern	int			DDEDataMax;			// velikost bufferu datových položek
extern	int			DDEDataAkt;			// aktivní položka (-1=všechny)

// obsluha DLL
extern	void*		DLLMemoryRead;		// ukazatel čtení z paměti
extern	void*		DLLMemoryWrite;		// ukazatel zápisu do paměti
extern	int			DLLMemoryTextNLen;	// délka textu s pevnou délkou

// konfigurace
extern CString		INIFile;			// jméno konfiguračního souboru
extern CString		INISection;			// jméno konfigurační sekce
extern CString		INIKey;				// jméno konfiguračního parametru
extern int			REGKey;				// skupina registrů (1=CURRENT_USER, 2=LOCAL_MACHINE)
extern CString		REGSubkey;			// jméno klíče
extern CString		REGValue;			// jméno položky

// UDP
extern SOCKET		UDPSocket;			// UDP soket (INVALID_SOCKET = není)
extern SOCKADDR_IN	UDPSendAddr;		// vysílací adresa UDP
extern SOCKADDR_IN	UDPRecAddr;			// přijímací adresa UDP
extern SOCKADDR_IN	UDPRecRecAddr;		// přijatá adresa UDP
extern char*		UDPRecBuf;			// přijímací buffer UDP
extern int			UDPRecBufSize;		// velikost přijímacího bufferu UDP

/////////////////////////////////////////////////////////////////////////////
// obsluha DDE

void DDEAppListLoad();					// načtení seznamu aplikací

/////////////////////////////////////////////////////////////////////////////
// obsluha mixeru (není u MINI verze)

bool MixDevInit();						// inicializace mixážních zařízení (vrací TRUE=OK)
bool MixDevSet(int mix);				// nastavení aktivního mixeru (vrací TRUE=OK)
void MixDstSet(int dst);				// nastavení cílového signálu (-1=žádný)
void MixSrcSet(int src);				// nastavení zdrojového signálu (-1=jen cílový)
void MixCtrlSet(int ctrl);				// nastavení ovládacího prvku (-1=žádný)
double GetMixCtrlVal();					// zjištění hodnoty ovládacího prvku
void SetMixCtrlVal(double val);			// nastavení hodnoty ovládacího prvku

/////////////////////////////////////////////////////////////////////////////
// obsluha portů (není u MINI verze)

void ComReinit();						// inicializace příjmu dat
void ComClose(int i);					// uzavření portu (i=platný index)
void GetComDCB();						// načtení DCB aktivního portu
void SetComDCB();						// nastavení DCB aktivního portu
void ComSend(BYTE data);				// vyslání bajtu na komunikační port
BYTE ComReceive();						// příjem bajtu z komunikačního portu (0=nejsou data)

/////////////////////////////////////////////////////////////////////////////
// DirectPlay (není u MINI verze)

void DirectSendData(BYTE* data, int n);	// zápis dat do výstupního bufferu
void DirectSend(int id);				// vyslání bloku dat (-1 = všem)
void DirectReceiveData(BYTE* data, int n); // čtení dat ze vstupního bufferu (při nedostatku dat doplní nulami)
int DirectReceive();					// příjem bloku dat (-1 = není)
void SetPlayerAct(CString txt);			// nastavení aktivního hráče
bool DirectPlayEnumPlayers();			// seznam hráčů (FALSE=chyba)
void SetPlayerMax(int n);				// nastavení max. hráčů (0=neomezeno)
void GetGameDesc();						// načtení parametrů hry
void SetGameN(int n, int i);			// nastavení číselné hodnoty hry (n = rozsah -16384 až +16383, i = index 0 až 7)
void SetGameL(bool n, int i);			// nastavení přepínače hry (i = index 0 až 7)
void SetGameAct(CString txt);			// nastavení aktivní hry
bool DirectPlayEnumGames();				// seznam her (FALSE=chyba)
void SetConnectAct(CString txt);		// nastavení aktivního spojení
bool DirectPlayConInit();				// inicializace seznamu spojení (FALSE=chyba)
void DirectPlayConUninit();				// ukončení seznamu spojení
bool DirectPlayInit();					// inicializace objektu DirectPlay (FALSE=chyba)
void DirectPlayUninit();				// uvolnění objektu DirectPlay
bool COMInit();							// inicializace knihovny COM (FALSE=chyba)
void COMUninit();						// uvolnění knihovny COM

/////////////////////////////////////////////////////////////////////////////
// UDP

void WSAInitialize();					// inicializace WSA knihovny
bool WSAInitialize1();					// inicializace WSA 1.1 knihovny (vrací TRUE je-li OK)
bool WSAInitialize2();					// inicializace WSA 2.2 knihovny (vrací TRUE je-li OK)
void WSADeinitialize();					// deinicializace WSA knihovny

void InitUDP();							// inicializace obsluhy UDP (inicializace bufferů)
bool OpenUDP();							// otevření soketu UDP (při chybě vrací FALSE)
void CloseUDP();						// zavření soketu UDP
void SendUDP(const void* data, int len);// odeslání UDP paketu
int RecUDP();							// příjem UDP paketu (vrací velikost dat v přijímacím bufferu)

#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------

/////////////////////////////////////////////////////////////////////////////
// dialogy

void DialogOn(bool on);					// režim dialogu
void WindowNew(int typ, int styl);		// vytvoření okna
HFONT GetFont(bool bold, bool italic, bool underline, bool strikeout,
			  bool serif, bool fixed, int height, double width, double angle); // vytvoření fontu (nutné uvolnění pomocí FreeFont)
void FreeFont(HFONT font);				// uvolnění fontu přiděleného s GetFont
void WindowSetFont(WINITEM* item);	// aktualizace fontu
void WindowItemNew(int typ, CString& classname, int width, int height, int style, int exstyle); // vytvoření prvku okna
void WindowDel(int inx);				// zrušení prvku
void FocusNext();						// posun fokusu na další prvek
void FocusPrev();						// posun fokusu na předchozí prvek
void RadioNext();						// posun fokusu na další přepínač
void RadioPrev();						// posun fokusu na předchozí přepínač
void RadioSet(int inx);					// nastavení přepínače


/////////////////////////////////////////////////////////////////////////////
// joystick

void AktJoystick(int inx);				// aktualizace údajů o joysticku


/////////////////////////////////////////////////////////////////////////////
// konzola

void ConsoleOnSet(bool on);			// zapnutí/vypnutí konzoly
void ConsoleOut(const CString& text);	// výstup na konzolu
void ConsoleErr(const CString& text);	// chybový výstup na konzolu
void ConsoleIn(CString& text);		// vstup textu z konzoly


/////////////////////////////////////////////////////////////////////////////
// obsluha CD disků

void CDPlay(int stopa);				// zahájení přehrávání od zadané stopy
void CDSetTrack(int stopa);			// nastavení stopy
void CDStop();						// zastavení přehrávání CD
void CDEject(bool eject);			// vysunutí CD
void CDPause();						// pauza CD
void CDSetPos(int pozice);			// nastavení pozice CD v ms + přehrávání
void CDSetTrackPos(int pozice);		// nastavení pozice v aktivní stopě CD v ms + přehrávání
void CDClose();						// uzavření ovladače CD
bool CDOpen();						// otevření ovladače CD
bool CDAkt();						// aktualizace údajů o CD disku


/////////////////////////////////////////////////////////////////////////////
// zjištění informací o velikosti a volném místu aktivního disku

void GetDiskSpace();


/////////////////////////////////////////////////////////////////////////////
// obsluha souborů

bool FileReadOpen();				// otevření souboru pro čtení
bool FileWriteOpen();				// otevření souboru pro zápis
bool FileWriteFlush();				// vyprázdnění zápisového bufferu
void _fastcall FileWriteBlok(BYTE* buf, int size); // zápis bloku do souboru
void FileWriteByte(BYTE data);		// zápis bajtu do výstupního souboru
void FileWriteWord(WORD data);		// zápis slova do výstupního souboru
void FileWriteDWord(DWORD data);	// zápis dvojslova do výstupního souboru
WORD FileReadWord();				// čtení slova ze vstupního souboru
BYTE FileReadByte();				// čtení bajtu ze vstupního souboru
void FileClose();					// uzavření souborů
void FileReadBlok(BYTE* buf, int size); //načtení bloku ze souboru
void _fastcall InitAlias(CString name);		// příprava jména ALIAS
void _fastcall InitAliasGroup(int lan);		// inicializace skupiny ALIAS

/////////////////////////////////////////////////////////////////////////////
// provádění programu

void Exec();


/////////////////////////////////////////////////////////////////////////////
// načtení souřadnic myši

void GetAktMousePos();


/////////////////////////////////////////////////////////////////////////////
// inicializace informací o ploše

void ExecInitMap();


/////////////////////////////////////////////////////////////////////////////
// nastavování rozměrů okna

void OnSizing(RECT* rc, int side);


/////////////////////////////////////////////////////////////////////////////
// aktualizace rozměrů okna

void _fastcall OnSize(WINITEM* item);


/////////////////////////////////////////////////////////////////////////////
// překreslení okna

void _fastcall OnPaint(WINITEM* item, int inx);


/////////////////////////////////////////////////////////////////////////////
// překreslení celého okna

void _fastcall RePaint(WINITEM* item);


/////////////////////////////////////////////////////////////////////////////
// aktualizační překreslení okna

void ReDisp();


/////////////////////////////////////////////////////////////////////////////
// čekání po zadanou dobu

void _fastcall TimeWait(int count);


/////////////////////////////////////////////////////////////////////////////
// obsluha časovače

void BackTimer();


/////////////////////////////////////////////////////////////////////////////
// test časovače

BOOL TestTimer();


/////////////////////////////////////////////////////////////////////////////
// čekání na signál od časovače s obsluhou zpráv

void WaitTimer(bool wait);


/////////////////////////////////////////////////////////////////////////////
// přesun sprajtu

void SpriteMove(int index, double cilx, double cily);


/////////////////////////////////////////////////////////////////////////////
// nastavení směru sprajtu

void SetSpriteSmer(int index, double smer);


/////////////////////////////////////////////////////////////////////////////
// nastavení viditelnosti sprajtu

void SetSpriteVisible(int index, bool visible);


/////////////////////////////////////////////////////////////////////////////
// nastavení sprajtu

void SetSprite(int inx, const CSprite& src);


/////////////////////////////////////////////////////////////////////////////
// zjištění příští fáze sprajtu (pro proměnnou fáze sprajtu)

int SpriteNextFaze(int index);


/////////////////////////////////////////////////////////////////////////////
// zajištění vymazání sprajtu (požadavek překreslení ikon)

void _fastcall SpriteClear(int index);


/////////////////////////////////////////////////////////////////////////////
// nastavení požadavku překreslení ikony (bez kontroly souřadnic)

inline void SetAktIcon(int x, int y, bool b)
{
	AktBuf[x + WidthN*y] = b;
}


/////////////////////////////////////////////////////////////////////////////
// nastavení požadavku překreslení všech ikon

void AktAllIcon();


/////////////////////////////////////////////////////////////////////////////
// nastavení požadavku překreslení ikony v intervalu (včetně koncových bodů)

void _fastcall AktIconBoxG(int x1, int y1, int x2, int y2);
void _fastcall AktIconBox(int x1, int y1, int x2, int y2);
