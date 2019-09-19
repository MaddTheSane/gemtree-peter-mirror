
/***************************************************************************\
*																			*
*							Provádění programu - text						*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// inline volání příkazu - text

inline void FText(CString& text)
{
	ExecItem++;
	ExecItem[-1].ExecTxt(text);
}


/////////////////////////////////////////////////////////////////////////////
// načtení jména souboru

void _fastcall FFileName(CString& text);


void _fastcall FGetStatus(CString& text);		// zjištění textu stavového řádku
void _fastcall FGetCaption(CString& text);		// zjištění textu nadpisu okna

// funkce
void _fastcall FTextFunc(CString& text);		// funkce s návratem textu

// proměnné
void _fastcall FTextCRLF(CString& text);		// text CR/LF
void _fastcall FTextEmpty(CString& text);		// prázdný text
void _fastcall FTextObj(CString& text);			// globální text (Data = index)
void _fastcall FTextObjList(CString& text);		// globalní text v seznamu (Data = index, List = seznam)
void _fastcall FTextLoc(CString& text);			// lokální text (Data = index)
void _fastcall FTextLocList(CString& text);		// lokální text v seznamu (Data = index, List = seznam)

// konverze
void _fastcall FTextFormat(CString& text);		// převod čísla na text
void _fastcall FTextFormat2(CString& text);		// převod čísla na text - 2 číslice
void _fastcall FTextFormatH(CString& text);		// převod čísla na text - 8 číslic HEX
void _fastcall FTextChar(CString& text);		// převod čísla na znak
void _fastcall FTextLower(CString& text);		// převod na malá písmena
void _fastcall FTextUpper(CString& text);		// převod na velká písmena

// obsluha času
void _fastcall FTimeDateEur(CString& text);		// převod data na evropský formát
void _fastcall FTimeDateUsa(CString& text);		// převod data na americký formát
void _fastcall FTimeDateJap(CString& text);		// převod data na japonský formát
void _fastcall FTimeTime24(CString& text);		// převod času na formát 24
void _fastcall FTimeTime12(CString& text);		// převod času na formát 12

// obsluha textů
void _fastcall FGetFileAct(CString& text);		// načtení aktivního adresáře
void _fastcall FGetDiskLabel(CString& text);	// načtení jmenovky disku
void _fastcall FGetFileRead(CString& text);		// soubor pro čtení
void _fastcall FGetFileWrite(CString& text);	// soubor pro zápis
void _fastcall FGetFileText0(CString& text);	// načtení textu zakončeného nulou
void _fastcall FGetFileTextCR(CString& text);	// načtení textu zakončeného CR
void _fastcall FGetFileTextN(CString& text);	// načtení textu s pevnou délkou

void _fastcall FFileListAll(CString& text);		// seznam souborů
void _fastcall FFileList(CString& text);		// seznam souborů

// obsluha klávesnice
void _fastcall FKeyCharWait(CString& text);		// čekání na snak z klávesnice
void _fastcall FKeyChar(CString& text);			// vstup znaku z klávesnice

// obsluha dialogů
void _fastcall FDialogText(CString& text);		// dialog box s návratem textu
void _fastcall FDialogOpen(CString& text);		// dialog otevření souboru
void _fastcall FDialogSave(CString& text);		// dialog uložení souboru
void _fastcall FDialogFind(CString& text);		// dialog vyhledání textu
void _fastcall FDialogColor(CString& text);		// dialog výběru barvy

// obsluha textů
void _fastcall FTextLeft(CString& text);		// levá část textu
void _fastcall FTextRight(CString& text);		// pravá část textu
void _fastcall FTextMid(CString& text);			// střední část textu
void _fastcall FTextlineGet(CString& text);		// řádek textu
void _fastcall FTextBlock(CString& text);		// blok textu
void _fastcall FTextAdd1(CString& text);		// součet jednoho textu - ponechá se beze změny
void _fastcall FTextAdd(CString& text);			// součet textů (Data = počet textů - 1)

// konzola
void _fastcall FConsoleIn(CString& text);		// vstup textu z konzoly

void _fastcall FGetCommandLine(CString& text);	// příkazový řádek
void _fastcall FCommandPar(CString& text);		// parametr příkazového řádku
void _fastcall FExecName(CString& text);		// jméno programu

// dialogy
void _fastcall FGetWindowText(CString& text);	// text okna
void WindowTextValidate();						// aktualizace textu okna

// DirectPlay
void _fastcall FConnectList(CString& text);		// seznam spojení
void _fastcall FGetConnectAct(CString& text);	// aktivní spojení
void _fastcall FGameList(CString& text);		// seznam her
void _fastcall FGetGameAct(CString& text);		// aktivní hra
void _fastcall FPlayerList(CString& text);		// seznam hráčů
void _fastcall FGetPlayerAct(CString& text);	// aktivní hráč
void _fastcall FGetGameDataT(CString& text);	// načtení textu z paketu
void _fastcall FGetGameIP(CString& text);		// zjištění IP počítače

// UDP
void _fastcall FGetUDPSendIP(CString& text);	// zjištění vysílací IP adresy UDP
void _fastcall FGetUDPRecIP(CString& text);		// zjištění přijaté IP adresy UDP
void _fastcall FGetUDPText(CString& text);		// příjem textu UDP

void _fastcall FGetUserFont(CString& text);		// zjištění uživatelského fontu
void _fastcall FEnumFonts(CString& text);		// zjištění seznamu fontů

void _fastcall FGetAlias(CString& text);		// načtení alias souboru

// porty
void _fastcall FGetComText(CString& text);		// načtení znaku z portu

// mixer
void _fastcall FGetMixDevice(CString& text);	// zjištění mixážních zařízení
void _fastcall FGetMixLineDst(CString& text);	// zjištění seznamu cílových signálů
void _fastcall FGetMixLineSrc(CString& text);	// zjištění seznamu zdrojových signálů
void _fastcall FGetMixControl(CString& text);	// zjištění seznamu ovládacích prvků
void _fastcall FGetMixCtrlList(CString& text);	// zjištění seznamu hodnot ovládacích prvků

// DDE
void _fastcall FGetDDEApp(CString& text);		// zjištění seznamu aplikací
void _fastcall FGetDDETopic(CString& text);		// zjištění seznamu témat

// Direct3D
void _fastcall FGet3DMode(CString& text);		// zjištění videomódu Direct3D

extern	TCHAR	Textcrlf[3];

// obsluha DLL
void _fastcall FGetMemoryText0(CString& text);	// text zakončený nulou
void _fastcall FGetMemoryText0P(CString& text);	// ukazatel na text zakončený nulou
void _fastcall FGetMemoryTextN(CString& text);	// text s pevnou délkou

// konfigurace
void _fastcall GetIniProfileString(CString& text); // načtení textu z INI (v textu je uložena implicitní hodnota)
void _fastcall FGetIniFile(CString& text);		// jméno konfiguračního souboru
void _fastcall FGetIniSection(CString& text);	// jméno konfigurační sekce
void _fastcall FGetIniValue(CString& text);		// jméno konfiguračního parametru
void _fastcall FGetIniString(CString& text);	// čtení konfiguračního textu
void _fastcall FGetIniString0(CString& text);	// čtení konfiguračního textu
void _fastcall FGetRegSubkey(CString& text);	// klíč registrů
void _fastcall FGetRegValue(CString& text);		// jméno položky registrů
void _fastcall FGetRegString(CString& text);	// čtení textu z registru
void _fastcall FGetRegString0(CString& text);	// čtení textu z registru
