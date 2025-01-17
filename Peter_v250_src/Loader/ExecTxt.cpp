
#include "Main.h"

/***************************************************************************\
*																			*
*						Provádění programu - text							*
*																			*
\***************************************************************************/

#pragma optimize("t", on)			// optimalizace na maximální rychlost


/////////////////////////////////////////////////////////////////////////////
// načtení jména souboru

void _fastcall FFileName(CString& text)
{
	FText(text);
	text.TrimLeft();
	text.TrimRight();
}


/////////////////////////////////////////////////////////////////////////////
// zjištění textu stavového řádku

void _fastcall FGetStatus(CString& text) { text = StatusText; }


/////////////////////////////////////////////////////////////////////////////
// zjištění textu nadpisu okna

void _fastcall FGetCaption(CString& text) { text = CaptionText; }


/***************************************************************************\
*																			*
*									funkce									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// funkce s návratem textu

void _fastcall FTextFunc(CString& text)
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
	text = Text[Text.Num() - res];

// zrušení lokálních proměnných
	FCommand();
}


/***************************************************************************\
*																			*
*								proměnné									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// prázdný text

void _fastcall FTextEmpty(CString& text) { text.Empty(); };


/////////////////////////////////////////////////////////////////////////////
// text CR/LF

TCHAR	Textcrlf[3] = {13,10,0};
CString TextCRLF(Textcrlf, 2);

void _fastcall FTextCRLF(CString& text) { text = TextCRLF; };


/////////////////////////////////////////////////////////////////////////////
// globální text (Data = index)

void _fastcall FTextObj(CString& text)
{
// index textu
	int inx = ExecItem[-1].Data;

// nastavení textu
	text = Text[inx];
}


/////////////////////////////////////////////////////////////////////////////
// globalní text v seznamu (Data = index, List = seznam)

void _fastcall FTextObjList(CString& text)
{
// index seznamu
	int list = ExecItem[-1].List;

// index textu, inkrementace indexu seznamu
	int inx = ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// nastavení textu
	text = Text[inx];
}


/////////////////////////////////////////////////////////////////////////////
// lokální text (Data = index)

void _fastcall FTextLoc(CString& text)
{
// index textu
	int inx = Text.Num() - ExecItem[-1].Data;

// nastavení textu
	text = Text[inx];
}


/////////////////////////////////////////////////////////////////////////////
// lokální text v seznamu (Data = index, List = seznam)

void _fastcall FTextLocList(CString& text)
{
// index seznamu
	int list = List.Num() - ExecItem[-1].List;

// index textu, inkrementace indexu seznamu
	int inx = Text.Num() - ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// nastavení textu
	text = Text[inx];
}


/***************************************************************************\
*																			*
*								konverze									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// převod čísla na text

void _fastcall FTextFormat(CString& text) { text.Double(FNum()); };


/////////////////////////////////////////////////////////////////////////////
// převod čísla na text - 2 číslice

void _fastcall FTextFormat2(CString& text)
{
	text.Empty();
	int num = FIntR();
	if (num < 0) num = 0;
	if (num > 99) num = 99;
	text.Add2Dig((char)num);
}


/////////////////////////////////////////////////////////////////////////////
// převod čísla na text - 8 číslic HEX

void _fastcall FTextFormatH(CString& text) { text.Hex(FIntRN()); };


/////////////////////////////////////////////////////////////////////////////
// převod čísla na znak

void _fastcall FTextChar(CString& text) { text = (TCHAR)FIntR(); }


/////////////////////////////////////////////////////////////////////////////
// převod na malá písmena

void _fastcall FTextLower(CString& text)
{
	FText(text);
	text.LowerCase();
}


/////////////////////////////////////////////////////////////////////////////
// převod na velká písmena

void _fastcall FTextUpper(CString& text)
{
	FText(text);
	text.UpperCase();
}


/***************************************************************************\
*																			*
*							obsluha času									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// převod data na evropský formát

void _fastcall FTimeDateEur(CString& text)
{
// načtení času
	FNumSysTime();

// dekódování data
	text.Empty();
	text.Add2DigSpc((char)SystemTime.wDay);
	text += _T('.');
	text.Add2Dig((char)SystemTime.wMonth);
	text += _T('.');
	text.Add4Dig(SystemTime.wYear);
}


/////////////////////////////////////////////////////////////////////////////
// převod data na americký formát

void _fastcall FTimeDateUsa(CString& text)
{
// načtení času
	FNumSysTime();

// dekódování data
	text.Empty();
	text.Add2DigSpc((char)SystemTime.wMonth);
	text += _T('/');
	text.Add2Dig((char)SystemTime.wDay);
	text += _T('/');
	text.Add4Dig(SystemTime.wYear);
}


/////////////////////////////////////////////////////////////////////////////
// převod data na japonský formát

void _fastcall FTimeDateJap(CString& text)
{
// načtení času
	FNumSysTime();

// dekódování data
	text.Empty();
	text.Add4Dig(SystemTime.wYear);
	text += _T('-');
	text.Add2Dig((char)SystemTime.wMonth);
	text += _T('-');
	text.Add2Dig((char)SystemTime.wDay);
}


/////////////////////////////////////////////////////////////////////////////
// převod času na formát 24

void _fastcall FTimeTime24(CString& text)
{
// načtení času
	FNumSysTime();

// dekódování času
	text.Empty();
	text.Add2DigSpc((char)SystemTime.wHour);
	text += _T(':');
	text.Add2Dig((char)SystemTime.wMinute);
	text += _T(':');
	text.Add2Dig((char)SystemTime.wSecond);
}


/////////////////////////////////////////////////////////////////////////////
// převod času na formát 12

void _fastcall FTimeTime12(CString& text)
{
// načtení času
	FNumSysTime();

// příprava hodiny
	char h = (char)SystemTime.wHour;
	if (h >= 12) h -= 12;
	if (h == 0) h = 12;

// dekódování času
	text.Empty();
	text.Add2DigSpc(h);
	text += _T(':');
	text.Add2Dig((char)SystemTime.wMinute);
	text += _T(':');
	text.Add2Dig((char)SystemTime.wSecond);

// označení a/p
	if (SystemTime.wHour >= 12)
	{
		text += _T('p');
	}
	else
	{
		text += _T('a');
	}
}


/***************************************************************************\
*																			*
*							obsluha souborů									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// načtení aktivního adresáře

void _fastcall FGetFileAct(CString& text) { text.GetAktDir(); }


/////////////////////////////////////////////////////////////////////////////
// načtení jmenovky disku

void _fastcall FGetDiskLabel(CString& text)
{
// příprava bufferu
	TCHAR buf[261];
	buf[0] = 0;

// načtení jména disku
	::GetVolumeInformation(NULL, buf, 260, NULL, NULL, NULL, NULL, 0);

// nastavení do proměnné
	text = buf;
}


/////////////////////////////////////////////////////////////////////////////
// soubor pro čtení

void _fastcall FGetFileRead(CString& text) { text = FileRead; }


/////////////////////////////////////////////////////////////////////////////
// soubor pro zápis

void _fastcall FGetFileWrite(CString& text) { text = FileWrite; }


/////////////////////////////////////////////////////////////////////////////
// načtení textu zakončeného nulou

void _fastcall FGetFileText0(CString& text)
{
// vyprázdnění textu
	text.Empty();

// načtení textu v kódu UNICODE
	if (FileTextUnicode)
	{

// buffer k načtení části textu
		WCHAR buf[256];
		int index = 0;

// cyklus až po bajt 0
		WCHAR znak = FileReadWord();
		while (znak != 0)
		{
			buf[index] = znak;
			index++;
			if (index == 256)
			{
				text.Add(buf, index);
				index = 0;
			}
			znak = FileReadWord();
		}

// zápis zbytku textu
		text.Add(buf, index);
	}

// načtení textu v kódu Windows
	else
	{

// buffer k načtení části textu
		char buf[256];
		int index = 0;

// cyklus až po bajt 0
		BYTE znak = FileReadByte();
		while (znak != 0)
		{
			buf[index] = (char)znak;
			index++;
			if (index == 256)
			{
				text.Add(buf, index);
				index = 0;
			}
			znak = FileReadByte();
		}

// zápis zbytku textu
		text.Add(buf, index);
	}
}


/////////////////////////////////////////////////////////////////////////////
// načtení textu zakončeného CR

void _fastcall FGetFileTextCR(CString& text)
{
// vyprázdnění textu
	text.Empty();

// načtení textu v kódu UNICODE
	if (FileTextUnicode)
	{

// buffer k načtení části textu
		WCHAR buf[256];
		int index = 0;

// cyklus až po bajt 0, CR nebo LF
		WCHAR znak = FileReadWord();
		while ((znak != 0) && (znak != 13) && (znak != 10))
		{
			buf[index] = znak;
			index++;
			if (index == 256)
			{
				text.Add(buf, 256);
				index = 0;
			}
			znak = FileReadWord();
		}

// zápis zbytku textu
		text.Add(buf, index);

// pro znak CR nebo LF se ruší párový LF nebo CR
		if (znak == 13)
		{
			znak = FileReadWord();
			if (znak != 10) FileReadOff -= 2;
		}
		else
		{
			if (znak == 10)
			{
				znak = FileReadWord();
				if (znak != 13) FileReadOff -= 2;
			}
		}
	}

// načtení textu v kódu Windows
	else
	{

// buffer k načtení části textu
		char buf[256];
		int index = 0;

// cyklus až po bajt 0, CR nebo LF
		BYTE znak = FileReadByte();
		while ((znak != 0) && (znak != 13) && (znak != 10))
		{
			buf[index] = (char)znak;
			index++;
			if (index == 256)
			{
				text.Add(buf, 256);
				index = 0;
			}
			znak = FileReadByte();
		}

// zápis zbytku textu
		text.Add(buf, index);

// pro znak CR nebo LF se ruší párový LF nebo CR
		if (znak == 13)
		{
			znak = FileReadByte();
			if (znak != 10) FileReadOff--;
		}
		else
		{
			if (znak == 10)
			{
				znak = FileReadByte();
				if (znak != 13) FileReadOff--;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// načtení textu s pevnou délkou

void _fastcall FGetFileTextN(CString& text)
{
// ukazatel indexu znaku
	int index = 0;

// načtení textu v kódu UNICODE
	if (FileTextUnicode)
	{

#ifdef _UNICODE

// nastavení délky textu
		text.Length(FileTextNLen);

// načtení textu
		for (int i = FileTextNLen; i > 0; i--)
		{
			WCHAR znak = FileReadWord();
			if (znak == 0) znak = ' ';
			text[index] = znak;
			index++;
		}

#else

		WCHAR* buf = (WCHAR*)MemGet(2*FileTextNLen);

// načtení textu
		for (int i = FileTextNLen; i > 0; i--)
		{
			WCHAR znak = FileReadWord();
			if (znak == 0) znak = ' ';
			buf[index] = znak;
			index++;
		}
		text.Term();
		text.Init(buf, index);

		MemFree(buf);

#endif

	}

// načtení textu v kódu Windows
	else
	{

#ifdef _UNICODE

		char* buf = (char*)MemGet(FileTextNLen);

// načtení textu
		for (int i = FileTextNLen; i > 0; i--)
		{
			BYTE znak = FileReadByte();
			if (znak == 0) znak = ' ';
			buf[index] = znak;
			index++;
		}
		text.Term();
		text.Init(buf, index);

		MemFree(buf);

#else

// nastavení délky textu
		text.Length(FileTextNLen);

// načtení textu
		for (int i = FileTextNLen; i > 0; i--)
		{
			BYTE znak = FileReadByte();
			if (znak == 0) znak = ' ';
			text[index] = znak;
			index++;
		}

#endif

	}

// odstranění mezer z konce textu
	while ((index > 0) && (text[index-1] == ' '))
	{
		index--;
	}
	text.Length(index);
}


/////////////////////////////////////////////////////////////////////////////
// seznam souborů

void FileList(CString& text)
{
// příprava masky souborů
	CString maska = text;
	maska.TrimLeft();
	maska.TrimRight();
	if (maska.IsEmpty()) maska = _T("*.*");		// implicitní maska
	text.Empty();								// vyprázdnění seznamu souborů
	CString maska0;								// jedna maska

// cyklus přes všechny masky
	do
	{

// příprava jedné masky
		int pos = maska.Find(';');
		if (pos < 0) pos = maska.Length();
		maska0 = maska.Left(pos);
		maska0.TrimLeft();
		maska0.TrimRight();
		if (maska0.IsNotEmpty())
		{

// zahájení vyhledávání souborů
			WIN32_FIND_DATA wfd;			// struktura pro hledání souborů
			HANDLE handle = ::FindFirstFile(maska0, &wfd);

// vyhledání souborů
			if (handle != INVALID_HANDLE_VALUE)
			{
				do
				{
					text += wfd.cFileName;
					text += 13;					// CR
					text += 10;					// LF

				} while (::FindNextFile(handle, &wfd));
				::FindClose(handle);
			}
		}

// vypuštění zpracované masky
		maska = maska.Mid(pos+1);

// dokud je nějaká další maska
	} while (maska.IsNotEmpty());
}

void _fastcall FFileListAll(CString& text)
{
	text.Empty();
	FileList(text);
}

void _fastcall FFileList(CString& text)
{
	FText(text);
	FileList(text);
}

/////////////////////////////////////////////////////////////////////////////
// načtení alias pro jeden jazyk (vrací FALSE=není alias, text se nezmění)

bool _fastcall ReadAlias(CString& text, int lan)
{
// příprava jména skupiny
	InitAliasGroup(lan);

// načtení alias
	TCHAR buf[1024];
	buf[0] = 0;
	int len = ::GetPrivateProfileString(AliasGroup, AliasKey, EmptyString, buf, 1023, AliasName);

// inicializace textu
	if (len > 0)
	{
		text.Term();
		text.Init(buf, len);
		return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// načtení alias souboru

void _fastcall FGetAlias(CString& text)
{
// text implicitně prázdný
	text.Empty();

// test, zda je soubor zadaný
	if (FileRead.IsEmpty())
	{
		FileError = true;
		return;
	}

// příprava aliases
	InitAlias(FileRead);

// implicitní aliases
	text = AliasKey;
	int pos = text.RevFind(_T('.'));
	if (pos >= 0) text.Delete(pos);

// test souboru ALIASES.INI
	if ((int)::GetFileAttributes(AliasName) != -1)
	{
		if (!ReadAlias(text, Jazyk) &&
			!ReadAlias(text, JazykDef) &&
			!ReadAlias(text, JAZYKENG))
		{
			for (int i = 0; i < JAZYKNUM; i++)
			{
				if ((i != Jazyk) &&
					(i != JazykDef) &&
					(i != JAZYKENG))
				{
					if (ReadAlias(text, i)) break;
				}
			}
		}
	}
}


/***************************************************************************\
*																			*
*								obsluha klávesnice							*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// čekání na znak z klávesnice

void _fastcall FKeyCharWait(CString& text)
{
// vyprázdnění textu
	text.Empty();

// čekání na vstup znaku z klávesnice (nebo na přerušení)
	while ((Break == 0) && 
			(CharBufRead == CharBufWrite) && 
			(!ConsoleOn))
	{
#ifndef _MINI
		D3Render(true);
#endif // _MINI
		TimeWait(1);
	}

// kontrola, zda byla přijata klávesa (zda to nebylo jen přerušení)
	if (CharBufRead != CharBufWrite)
	{

// načtení klávesy
		text = (TCHAR)CharBuf[CharBufRead];

// zvýšení čtecí adresy
		CharBufRead++;
		if (CharBufRead >= KEYBUFSIZE) CharBufRead = 0;
	}
}


/////////////////////////////////////////////////////////////////////////////
// vstup znaku z klávesnice

void _fastcall FKeyChar(CString& text)
{
// test, zda byla přijata klávesa
	if (CharBufRead != CharBufWrite)
	{

// načtení klávesy
		text = (TCHAR)CharBuf[CharBufRead];

// zvýšení čtecí adresy
		CharBufRead++;
		if (CharBufRead >= KEYBUFSIZE) CharBufRead = 0;
	}
	else
	{

// jinak vyprázdnění textu
		text.Empty();
	}
}


/***************************************************************************\
*																			*
*							obsluha dialogů									*
*																			*
\***************************************************************************/

CString DialogTextNadpis;
CString DialogText1;
CString DialogText2;
CString DialogText3;
CString DialogTextPar;

/////////////////////////////////////////////////////////////////////////////
// obsluha dialogu zadání textu

BOOL CALLBACK DialogTextProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_INITDIALOG:
		{
			CenterWindow(hWnd, Win[DialogParent].Wnd);
			DialogTextNadpis.SetWindowText(hWnd);
			DialogText1.SetDialogText(hWnd, IDC_TEXT1);
			DialogText2.SetDialogText(hWnd, IDC_TEXT2);
			DialogText3.SetDialogText(hWnd, IDC_TEXT3);
			DialogTextPar.SetDialogText(hWnd, IDC_EDIT);
			CString txt;
			txt = JazykInfo[Jazyk].CancelTxt;
			txt.SetDialogText(hWnd, IDCANCEL);
		}
		return TRUE;

	case WM_COMMAND:
		switch (HIWORD(wParam))
		{
		case 0:
		case 1:
			switch (LOWORD(wParam))
			{
			case IDOK:
				DialogTextPar.GetDialogText(hWnd, IDC_EDIT);
				::EndDialog(hWnd, LOWORD(wParam));
				return TRUE;

			case IDCANCEL:
				DialogTextPar.Empty();
				::EndDialog(hWnd, LOWORD(wParam));
				return TRUE;
			}
			break;
		}
		break;

	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// dialog box s návratem textu

void _fastcall FDialogText(CString& text)
{
// načtení textů
	FText(DialogTextNadpis);
	FText(DialogText1);
	FText(DialogText2);
	FText(DialogText3);
	FText(DialogTextPar);

// vypnutí celoobrazovkového módu
//	SetFullScreen(false);

#ifndef _MINI
	D3Render(true);
#endif // _MINI

// překreslení displeje před vyvoláním dialogu
	ReDisp();

// vyvolání dialogu
	IsOverlay = true;

	::DialogBoxParam(
			hInstance,
			MAKEINTRESOURCE(IDD_TEXT_PARAM),
			Win[DialogParent].Wnd,
			(DLGPROC)DialogTextProc,
			(LPARAM)0);

	IsOverlay = false;
	if (FullScreen) RePaint(&Win[0]);

// navrácená hodnota
	text = DialogTextPar;
}

/////////////////////////////////////////////////////////////////////////////
// dialog otevření/uložení souboru

void _fastcall FDialogOpenSave(CString& text, bool save)
{
// načtení titulku okna
	CString title;
	FText(title);

// načtení jmen souborů
	CString jmena;
	FText(jmena);

// načtení masky souborů
	CString masky;
	FText(masky);

// načtení přednastaveného jména souboru
	CString name;
	FText(name);
	name.TrimLeft();
	name.TrimRight();
	name.PathName(0);

// vypnutí celoobrazovkového módu
//	SetFullScreen(false);

// překreslení displeje před vyvoláním dialogu
	ReDisp();

// příprava struktury k zadání jména souboru
	OPENFILENAME ofn;
	MemFill(&ofn, sizeof(ofn), 0);
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = Win[DialogParent].Wnd;

// výchozí adresář
	CString aktdir;
	int namepos = name.RevFind(_T('\\'));					// nalezení konce cesty
	if (namepos < 0)
	{
		namepos = name.Find(_T(':'));						// nalezení alespoň disku
		if (namepos < 0)
		{
			aktdir.GetAktDir();								// použije se aktuální adresář
		}
		else
		{
			aktdir = name.Left(namepos + 1);				// označení disku
		}
	}
	else
	{
		if ((namepos == 0) || 
			((namepos == 2) && (name.Get(1) == _T(':'))))	// je to ROOT disku?
		{
			aktdir = name.Left(namepos + 1);				// adresář bude s koncovým lomítkem
		}
		else
		{
			aktdir = name.Left(namepos);					// adresář po koncové lomítko
		}
	}

	ofn.lpstrInitialDir = aktdir;							// výchozí adresář

// odstranění cesty ze jména souboru
	name.Delete(0, namepos + 1);

// rozdělení jména souboru na jméno a příponu
	CString defext;
	int extpos = name.Find(_T('.'));
	if (extpos >= 0)
	{
		defext = name;
		name.Delete(extpos);
		defext.Delete(0, extpos + 1);
		if (defext.Find(_T('*')) >= 0) defext.Empty();
	}
	CString defext2;			// alternativní implicitní přípona
	int filterindex2 = 0;		// alternativní index filtru

// příprava přípony pro hledání v seznamu masek
	CString defextup;
	if (defext.IsNotEmpty())
	{
		ofn.lpstrDefExt = defext;

		defextup = _T('.') + defext + _T(';');
		defextup.UpperCase();
	}

// buffer pro masku souborů
	TCHAR* maskbuf = NULL;
	int masksize = 0;

// počet řádků zadaného seznamu masek
	int l = masky.LineNum();
	int l2 = jmena.LineNum();
	if (l < l2) l = l2;
	if (l <= 0) l = 1;

// načtení seznamu masek
	for (int n = 0; n < l;)
	{

// název masky
		CString radek = jmena.GetLine(n);

// ořezání oddělovačů ze začátku a konce názvu masky
		radek.TrimLeft();
		radek.TrimRight();

// implicitní název masky
		if (radek.IsEmpty())
		{
			radek = JazykInfo[Jazyk].AllFilesTxt;
			radek += _T(" (*.*)");
		}

// maska souborů
		CString maska = masky.GetLine(n);
		n++;

// ořezání oddělovačů ze začátku a konce masky
		maska.TrimLeft();
		maska.TrimRight();

// ořezání středníku ze začátku a konce masky
		while (maska.Get(0) == _T(';')) maska.Delete(0,1);
		while (maska.LastChar() == _T(';')) maska.Delete(maska.Length()-1, 1);

// vypuštění mezer z masky
		for (;;)
		{
			int p = maska.Find(_T(' '));
			if (p < 0) break;
			maska.Delete(p, 1);
		}

// implicitní maska
		if (maska.IsEmpty()) maska = _T("*.*");

// implicitní přípona
		if (defext2.IsEmpty())
		{
			defext2 = maska;
			defext2.Delete(0, defext2.Find(_T('.')) + 1);	// zrušení části před a s tečkou
			int pos = defext2.Find(_T(';'));				// začátek další masky
			if (pos >= 0) defext2.Delete(pos);				// zrušení další masky

			if (defext2.Find(_T('*')) >= 0)
			{
				defext2.Empty();
			}
			else
			{
				filterindex2 = n;

				if (ofn.lpstrDefExt == NULL)				// přípona nebyla zadána
				{
					defext = defext2;
					ofn.lpstrDefExt = defext;

					defextup = _T('.') + defext + _T(';');
					defextup.UpperCase();
				}
			}
		}

// vyhledání přípony
		if (defextup.IsNotEmpty())
		{
			CString maska2 = maska + _T(';');
			maska2.UpperCase();
			if (maska2.Find(defextup) >= 0)
			{
				ofn.nFilterIndex = n;
			}
		}

// zvýšení velikosti bufferu masek
		int newsize = masksize + (radek.Length() + maska.Length() + 2)*sizeof(TCHAR);
		maskbuf = (TCHAR*)MemSize(maskbuf, newsize + sizeof(TCHAR));

// přidání názvu masky
		radek.Write(maskbuf + masksize/sizeof(TCHAR));
		masksize += (radek.Length() + 1)*sizeof(TCHAR);
		maskbuf[(masksize-1)/sizeof(TCHAR)] = 0;

// přidání masky souborů
		maska.Write(maskbuf + masksize/sizeof(TCHAR));
		masksize = newsize;
		maskbuf[(masksize-1)/sizeof(TCHAR)] = 0;
	}

// koncová nula označující konec seznamu masek
	maskbuf[masksize/sizeof(TCHAR)] = 0;

// buffer seznamu masek
	ofn.lpstrFilter = maskbuf;

// není-li nalezena odpovídající maska, nebude ani implicitní přípona
	if (ofn.nFilterIndex == 0)
	{
		if (defext2.IsNotEmpty())
		{
			ofn.lpstrDefExt = defext2;
			ofn.nFilterIndex = filterindex2;
		}
		else
		{
			ofn.lpstrDefExt = NULL;
		}
	}

// přednastavené jméno souboru
	n = name.Length() + 1;
	if (n < 1024) n = 1024;
	TCHAR* file = (TCHAR*)MemGet(n*sizeof(TCHAR));
	MemFill(file, n*sizeof(TCHAR), 0);
	name.Write(file);
	ofn.lpstrFile = file;
	ofn.nMaxFile = n;

// titulek okna
	if (title.IsEmpty())
	{
		ofn.lpstrTitle = NULL;
	}
	else
	{
		ofn.lpstrTitle = title;
	}

// příznaky
	ofn.Flags = OFN_ENABLESIZING | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_LONGNAMES |
			OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_SHAREAWARE;

// zadání jména souboru
	BOOL res;
	IsOverlay = true;
	if (save)
	{
		res = ::GetSaveFileName(&ofn);
	}
	else
	{
		res = ::GetOpenFileName(&ofn);
	}
	IsOverlay = false;
	if (FullScreen) RePaint(&Win[0]);

// načtení jména zadaného souboru (při chybě nebo přerušení bude prázdný text)
	if (res)
	{
		text = file;
	}
	else
	{
		text.Empty();
	}

// zrušení bufferů
	MemFree(file);
	MemFree(maskbuf);
}

/////////////////////////////////////////////////////////////////////////////
// dialog otevření souboru

void _fastcall FDialogOpen(CString& text)
{
	FDialogOpenSave(text, false);
}

/////////////////////////////////////////////////////////////////////////////
// dialog uložení souboru

void _fastcall FDialogSave(CString& text)
{
	FDialogOpenSave(text, true);
}

/////////////////////////////////////////////////////////////////////////////
// dialog vyhledání textu

void _fastcall FDialogFind(CString& text)
{
	text.Empty();
}

/////////////////////////////////////////////////////////////////////////////
// dialog výběru barvy

void _fastcall FDialogColor(CString& text)
{
	text.Empty();
}


/***************************************************************************\
*																			*
*								obsluha textů								*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// levá část textu

void _fastcall FTextLeft(CString& text)
{
	int znaku = FIntR();
	FText(text);
	text.Delete(znaku);
}


/////////////////////////////////////////////////////////////////////////////
// pravá část textu

void _fastcall FTextRight(CString& text)
{
	int pozice = FIntR();
	CString txt;
	FText(txt);
	text = txt.Right(txt.Length() - pozice);
}


/////////////////////////////////////////////////////////////////////////////
// střední část textu

void _fastcall FTextMid(CString& text)
{
	int pozice = FIntR();
	int znaku = FIntR();
	CString txt;
	FText(txt);
	text = txt.Mid(pozice, znaku);
}


/////////////////////////////////////////////////////////////////////////////
// řádek textu

void _fastcall FTextlineGet(CString& text)
{
	int radek = FIntR();

	CString txt;
	FText(txt);

	text = txt.GetLine(radek);
}

/////////////////////////////////////////////////////////////////////////////
// blok textu

void _fastcall FTextBlock(CString& text)
{
	int begline = FIntR();
	int begpos = FIntR();
	int endline = FIntR();
	int endpos = FIntR();

	CString txt;
	FText(txt);

	text = txt.GetBlock(begline, begpos, endline, endpos);
}

/////////////////////////////////////////////////////////////////////////////
// součet jednoho textu - ponechá se beze změny

void _fastcall FTextAdd1(CString& text) { FText(text); }


/////////////////////////////////////////////////////////////////////////////
// součet textů (Data = počet textů - 1)

void _fastcall FTextAdd(CString& text)
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// pomocný buffer textu
	text.Empty();
	CString text2;

// cyklus přes všechny texty
	for (; i >= 0; i--)
	{

// načtení dalšího textu
		FText(text2);
		text += text2;
	}
}


/***************************************************************************\
*																			*
*									konzola									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// vstup textu z konzoly

void _fastcall FConsoleIn(CString& text) { ConsoleIn(text); }


/////////////////////////////////////////////////////////////////////////////
// příkazový řádek

void _fastcall FGetCommandLine(CString& text) { text = CommandLine; }

void _fastcall FCommandPar(CString& text)
{
	text.Empty();

// index parametru
	int i = FIntR();
	int pos = 0;
	int beg = 0;
	int len = CommandLine.Length();

// nalezení parametru
	for (; i >= 0; i--)
	{

// nalezení začátku parametru
		while (	(pos < len) && 
				(CommandLine[pos] <= _T(' ')) && 
				(CommandLine[pos] > 0)) 
		{
			pos++;
		}
		beg = pos;

// nalezení konce parametru
		bool uvoz = false;
		while (	(pos < len) &&
				(	(CommandLine[pos] < 0) ||
					(CommandLine[pos] > _T(' ')) ||
					uvoz
				) )
		{
			if (CommandLine[pos] == _T('"')) uvoz = !uvoz;
			pos++;
		}
	}

// přenesení parametru
	text = CommandLine.Mid(beg, pos-beg);

// zrušení uvozovek
	pos = 0;
	len = text.Length();
	while (pos < len)
	{
		if (text[pos] == _T('"'))
		{
			text.Delete(pos, 1);
			len--;
			if ((pos < len) && (text[pos] != _T('"'))) pos--;
		}
		pos++;
	}
}


/////////////////////////////////////////////////////////////////////////////
// jméno programu

void _fastcall FExecName(CString& text) { text = ExeFileName; }


/***************************************************************************\
*																			*
*									dialogy									*
*																			*
\***************************************************************************/

CString InRichEditText;

DWORD CALLBACK InEditStreamCallback(DWORD cookie, BYTE* buf, long num, long* bytes)
{
	*bytes = num;

	InRichEditText.Add((TCHAR*)buf, num/sizeof(TCHAR));

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// text okna

void WindowTextValidate()
{
	if (Win.IsValid(WindowID))
	{
		WINITEM* item = &Win[WindowID];

		if (!item->TextValid)
		{
			switch (item->Typ)
			{
			case WINITEM_LISTBOX:
				{
					item->Text.Empty();
					int i = ::SendMessage(item->Wnd, LB_GETCURSEL, 0, 0);
					if (i >= 0)
					{
						int len = ::SendMessage(item->Wnd, LB_GETTEXTLEN, i, 0);

						if (len > 0)
						{
							TCHAR* buf = (TCHAR*)MemGet((len+1)*sizeof(TCHAR));
							::SendMessage(item->Wnd, LB_GETTEXT, i, (LPARAM)buf);
							item->Text.Term();
							item->Text.Init(buf, len);
							MemFree(buf);
						}
					}
				}
				break;

			case WINITEM_COMBOBOX:
				{
					item->Text.Empty();
					int i = ::SendMessage(item->Wnd, CB_GETCURSEL, 0, 0);
					if (i >= 0)
					{
						int len = ::SendMessage(item->Wnd, CB_GETLBTEXTLEN, i, 0);

						if (len > 0)
						{
							TCHAR* buf = (TCHAR*)MemGet((len+1)*sizeof(TCHAR));
							::SendMessage(item->Wnd, CB_GETLBTEXT, i, (LPARAM)buf);
							item->Text.Term();
							item->Text.Init(buf, len);
							MemFree(buf);
						}
					}
				}
				break;

			case WINITEM_RICHEDIT:
			case WINITEM_RICHMEMO:
				{
					InRichEditText.Empty();
					EDITSTREAM stream;
					stream.pfnCallback = InEditStreamCallback;
					::SendMessage(item->Wnd, EM_STREAMOUT, (item->Typ == WINITEM_RICHEDIT) ? SF_RTF : SF_TEXT, (LPARAM)&stream);
					item->Text = InRichEditText;

				}
				break;

			case WINITEM_TABLE:
				{
					item->Text.Empty();

					int col = item->TabCol;
					int cols = item->TabCols;

					if ((DWORD)col < (DWORD)cols)
					{
						int row = item->TabRow;

						if ((DWORD)row < (DWORD)item->TabRows)
						{
							item->Text = item->TabData[row * cols + col];
						}
					}
				}
				break;

			default:
				item->Text.GetWindowText(item->Wnd);
			}

			item->TextValid = true;
		}
	}
}


void _fastcall FGetWindowText(CString& text)
{
	if (Win.IsValid(WindowID))
	{
		WindowTextValidate();
		text = Win[WindowID].Text;
	}
	else
	{
		text.Empty();
	}
}

/***************************************************************************\
*																			*
*									DirectPlay								*
*																			*
\***************************************************************************/

// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI

/////////////////////////////////////////////////////////////////////////////
// seznam spojení

void _fastcall FConnectList(CString& text)
{
	DirectPlayConInit();
	text = DirectPlayConnects;
}

/////////////////////////////////////////////////////////////////////////////
// aktivní spojení

void _fastcall FGetConnectAct(CString& text)
{
	text = DirectPlayConnectsAkt;
}

/////////////////////////////////////////////////////////////////////////////
// seznam her

void _fastcall FGameList(CString& text)
{
	DirectPlayEnumGames();
	text = DirectPlayGames;
}


/////////////////////////////////////////////////////////////////////////////
// aktivní hra

void _fastcall FGetGameAct(CString& text)
{
	text = DirectPlayGamesAkt;
}

/////////////////////////////////////////////////////////////////////////////
// seznam hráčů

void _fastcall FPlayerList(CString& text)
{
	DirectPlayEnumPlayers();
	text = DirectPlayPlayers;
}


/////////////////////////////////////////////////////////////////////////////
// aktivní hráč

void _fastcall FGetPlayerAct(CString& text)
{
	text = DirectPlayPlayersAkt;
}

/////////////////////////////////////////////////////////////////////////////
// načtení textu z paketu

void _fastcall FGetGameDataT(CString& text)
{
// není nic v bufferu
	if (DirectPlayDataInO >= DirectPlayDataInN)
	{
		text.Empty();
		return;
	}

// načtení textu z bufferu
	text = (char*)(DirectPlayDataIn + DirectPlayDataInO);

// posun ukazatelů
	DirectPlayDataInO += text.Length() + 1;
	if (DirectPlayDataInO > DirectPlayDataInN)
	{
		DirectPlayDataInO = DirectPlayDataInN;
	}
}

/////////////////////////////////////////////////////////////////////////////
// zjištění IP adresy počítače

void _fastcall FGetGameIP(CString& text)
{
// test platnosti IP adresy
	if (HostIPValid <= 0)
	{
		HostIPValid = 3*18;

		// inicializace WSA knihovny
		if (WSAInitialize1())
		{

// příprava bufferu k načtení lokálního jména
			char* buf = (char*)MemGet(512);

// načtení lokálního jména
			if (pWSAGetHostName(buf, 511) == 0)
			{
				struct hostent* hst = pWSAGetHostByName(buf);
				if (hst != NULL)
				{

// test, zda je platná IP adresa
					char** list = hst->h_addr_list;
					if ((list != NULL) && (*list != NULL))
					{

// dekódování IP adresy
						char FAR* adr = pWSAInetNtoa(**(struct in_addr**)list);
						if (adr != NULL)
						{
							HostIP = adr;
						}
					}	
				}
			}

// uvolnění bufferu lokálního jména
			MemFree(buf);
		}
	}

// IP adresa počítače
	text = HostIP;
}

/***************************************************************************\
*																			*
*										UDP									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// zjištění vysílací IP adresy UDP

void _fastcall FGetUDPSendIP(CString& text)
{
	text = "127.0.0.1";

	if (WSAInitialize1())
	{
		char FAR* adr = pWSAInetNtoa(UDPSendAddr.sin_addr);
		if (adr != NULL) text = adr;
	}
}

/////////////////////////////////////////////////////////////////////////////
// zjištění přijaté IP adresy UDP

void _fastcall FGetUDPRecIP(CString& text)
{
	text = "255.255.255.255";

	if (WSAInitialize1())
	{
		char FAR* adr = pWSAInetNtoa(UDPRecRecAddr.sin_addr);
		if (adr != NULL) text = adr;
	}
}

/////////////////////////////////////////////////////////////////////////////
// příjem textu UDP

void _fastcall FGetUDPText(CString& text)
{
	int n = RecUDP();
	text.Term();
	text.Init((LPCSTR)UDPRecBuf, n);
	text.KorigNul();
}

#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------



/////////////////////////////////////////////////////////////////////////////
// zjištění uživatelského fontu

void _fastcall FGetUserFont(CString& text)
{
	text = UserFont;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění seznamu fontů

int CALLBACK EnumFontFamProc(const LOGFONT* lpelf, const TEXTMETRIC* lpntm, DWORD FontType, LPARAM lParam)
{
	CString txt = lpelf->lfFaceName;
	txt += 13;
	txt += 10;
	FontList += txt;
	return TRUE;
}

void _fastcall FEnumFonts(CString& text)
{
	if (FontList.IsEmpty())
	{
		HDC dc = ::GetDC(MainFrame);
		::EnumFontFamilies(dc, NULL, EnumFontFamProc, 0);
		::ReleaseDC(MainFrame, dc);
	}
	text = FontList;
}


// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI

/////////////////////////////////////////////////////////////////////////////
// načtení znaku z portu

void _fastcall FGetComText(CString& text)
{
	BYTE n = ComReceive();

//	if (n == 0)
//	{
//		text.Empty();
//	}
//	else
//	{
		switch (Coms[ComAkt].Dcb.ByteSize)
		{
		case 5:
		case 6:
			n |= 0x40;
		}

		text = (char)n;
//	}
}

#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------



/***************************************************************************\
*																			*
*								obsluha mixeru								*
*																			*
\***************************************************************************/

// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI

/////////////////////////////////////////////////////////////////////////////
// zjištění mixážních zařízení

void _fastcall FGetMixDevice(CString& text)
{
	MixDevInit();
	text = MixDevT;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění seznamu cílových signálů

void _fastcall FGetMixLineDst(CString& text)
{
	MixDevInit();
	text = MixLineDT;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění seznamu zdrojových signálů

void _fastcall FGetMixLineSrc(CString& text)
{
	MixDevInit();
	text = MixLineST;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění seznamu ovládacích prvků

void _fastcall FGetMixControl(CString& text)
{
	MixDevInit();
	text = MixCtrlT;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění seznamu hodnot ovládacích prvků

void _fastcall FGetMixCtrlList(CString& text)
{
	MixDevInit();
	text = MixValT;
}

/***************************************************************************\
*																			*
*									DDE										*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// zjištění seznamu serverů

void _fastcall FGetDDEApp(CString& text)
{
// načtení seznamu aplikací DDE
	DDEAppListLoad();

// sestavení seznamu serverů DDE
	text.Empty();

	for (int i = 0; i < DDEServNum; i++)
	{
		if (DDEServList[i].ServHandle != MainFrame)
		{
			text += DDEServList[i].ServName;
			text += TextCRLF;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// zjištění seznamu témat

void _fastcall FGetDDETopic(CString& text)
{
// příprava položky serveru
	int i = DDEServAkt;
	if (i < 0)
	{
		i = DDEAppAkt;
		if (i >= 0)
		{
			i = DDEAppList[i].ServFirst;
		}
	}

// není platný server
	text.Empty();
	if (i < 0) return;

// sestavení seznamu témat
	i = DDEServList[i].TopicFirst;

	while (i >= 0)
	{
		DDETOPITEM* topitem = DDETopList + i;
		text += topitem->TopicName;
		text += TextCRLF;

		i = topitem->TopicNext;
	}
}

/***************************************************************************\
*																			*
*									Direct3D								*
*																			*
\***************************************************************************/

#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------

/////////////////////////////////////////////////////////////////////////////
// zjištění seznamu videomódů

void _fastcall FGet3DMode(CString& text)
{
// inicializace seznamu videomódů
	InitVideoModes();

	text.Empty();

	CString txt2;
	CString txt3;

	for (int i = 0; i < NumVideoModes; i++)
	{
		int width = TabVideoModes[i].Width;
		int height = TabVideoModes[i].Height;
		int bits = TabVideoModes[i].Bits;

		txt2.Int(width);
		txt2 += _T('*');
		txt3.Int(height);
		txt2 += txt3;
		txt2 += _T('/');
		txt3.Int(bits);
		txt2 += txt3;
		if ((width == ScreenWidth0) &&
			(height == ScreenHeight0) &&
			(bits == ScreenBits))
		{
			txt2 += _T('<');
		}

		txt2 += _T(13);
		txt2 += _T(10);
		text += txt2;
	}
}

/***************************************************************************\
*																			*
*								obsluha DLL									*
*																			*
\***************************************************************************/

// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI

/////////////////////////////////////////////////////////////////////////////
// text zakončený nulou

void _fastcall FGetMemoryText0(CString& text)
{
	char* buf = (char*)DLLMemoryRead;
	text = buf;
	buf += text.Length() + 1;
	DLLMemoryRead = buf;
}

/////////////////////////////////////////////////////////////////////////////
// ukazatel na text zakončený nulou

void _fastcall FGetMemoryText0P(CString& text)
{
	char** buf = (char**)DLLMemoryRead;
	text = *buf;
	buf++;
	DLLMemoryRead = buf;
}

/////////////////////////////////////////////////////////////////////////////
// text s pevnou délkou

void _fastcall FGetMemoryTextN(CString& text)
{
	text.Term();
	char* buf = (char*)DLLMemoryRead;
	text.Init(buf, DLLMemoryTextNLen);
	buf += DLLMemoryTextNLen;
	DLLMemoryRead = buf;
}

/***************************************************************************\
*																			*
*								konfigurace									*
*																			*
\***************************************************************************/

#define MAXPROFILELEN 0x0FFF

/////////////////////////////////////////////////////////////////////////////
// načtení textu z INI (v textu je uložena implicitní hodnota)

void _fastcall GetIniProfileString(CString& text)
{
// jméno sekce
	LPCTSTR section = NULL;
	if (INISection.IsNotEmpty()) section = INISection;

// jméno hodnoty
	LPCTSTR key = NULL;
	if (INIKey.IsNotEmpty()) key = INIKey;

// příprava implicitního textu (varování - ve Win95 může být buffer modifikován)
	text.TrimLeft();
	text.TrimRight();
	if (text.Length() > MAXPROFILELEN) text.Delete(MAXPROFILELEN);
	LPTSTR vzor = (LPTSTR)MemGet((text.Length() + 1) * sizeof(TCHAR));
	text.Write(vzor);
	vzor[text.Length()] = 0;

// příprava cílového bufferu
	LPTSTR buf = (LPTSTR)MemGet((MAXPROFILELEN + 1) * sizeof(TCHAR));

// načtení položky
	int len = ::GetPrivateProfileString(section, key, vzor, buf, MAXPROFILELEN, INIFile);

// uložení seznamu
	if ((section == NULL) || (key == NULL))
	{
		text.Empty();
		LPCTSTR src = buf;

		while (*src != 0)
		{
			CString text2(src);
			text += text2;
			text += TextCRLF;
			src += text2.Length() + 1;
		}
	}

// uložení položky, není-li seznam
	else
	{
		text.Term();
		text.Init(buf, len);
		text.KorigNul();
	}

	MemFree(buf);
}

/////////////////////////////////////////////////////////////////////////////
// jméno konfiguračního souboru

void _fastcall FGetIniFile(CString& text)
{
	text = INIFile;
}

/////////////////////////////////////////////////////////////////////////////
// jméno konfigurační sekce

void _fastcall FGetIniSection(CString& text)
{
	text = INISection;
}

/////////////////////////////////////////////////////////////////////////////
// jméno konfiguračního parametru

void _fastcall FGetIniValue(CString& text)
{
	text = INIKey;
}

/////////////////////////////////////////////////////////////////////////////
// čtení konfiguračního textu

void _fastcall FGetIniString(CString& text)
{
	FText(text);
	GetIniProfileString(text);
}

void _fastcall FGetIniString0(CString& text)
{
	text.Empty();
	GetIniProfileString(text);
}

/////////////////////////////////////////////////////////////////////////////
// klíč registrů

void _fastcall FGetRegSubkey(CString& text)
{
	text = REGSubkey;
}

/////////////////////////////////////////////////////////////////////////////
// jméno položky registrů

void _fastcall FGetRegValue(CString& text)
{
	text = REGValue;
}

/////////////////////////////////////////////////////////////////////////////
// čtení textu z registru

void _fastcall GetRegString(CString& text)
{
// otevření klíče
	HKEY hkey;

	if (::RegOpenKeyEx(
		(HKEY) (REGKey | 0x80000000),
		REGSubkey,
		0,
		KEY_QUERY_VALUE,
		&hkey) == ERROR_SUCCESS)
	{

// načtení hodnoty klíče
		int bufsize = (MAXPROFILELEN + 1) * sizeof(TCHAR);

		LPTSTR buf = (LPTSTR)MemGet((MAXPROFILELEN + 1) * sizeof(TCHAR));

		if (::RegQueryValueEx(
			hkey,
			REGValue,
			0,
			NULL,
			(BYTE*)buf,
			(DWORD*)&bufsize) == ERROR_SUCCESS)
		{
			bufsize /= sizeof(TCHAR);

			if (bufsize > 0)
			{
				text.Term();
				text.Init(buf, bufsize-1);
				text.KorigNul();
			}
		}

		MemFree(buf);

// uzavření klíče
		::RegCloseKey(hkey);
	}
}

void _fastcall FGetRegString(CString& text)
{
	FText(text);
	GetRegString(text);
}

void _fastcall FGetRegString0(CString& text)
{
	text.Empty();
	GetRegString(text);
}

#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------
