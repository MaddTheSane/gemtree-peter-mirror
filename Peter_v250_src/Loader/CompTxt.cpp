
#include "Main.h"

/***************************************************************************\
*																			*
*						Kompilace programu - text							*
*																			*
\***************************************************************************/

#pragma optimize("s", on)			// optimalizace na minimální velikost


/////////////////////////////////////////////////////////////////////////////
// překlad textového výrazu (vrací true = operace OK)

bool _fastcall CompTxt(int index)
{
// adresa zdrojového prvku
	if ((DWORD)index >= (DWORD)BufEdiN) return false;
	PETPROG*	item = BufEdi + index;
	PETPROG2*	item2 = BufEdi2 + index;
	int refinx = item->RefIndex;

// kontrola, zda je položka vypnuta
	if ((item->Param & (PETPROG_OFF | PETPROG_OFF_DEP)) != 0) return false;

// větvení podle funkce
	switch (item->Func + IDF)
	{
	case IDF_CAPTION:
		CompAddItem(FGetCaption);		// načtení nadpisu okna
		return true;

	case IDF_STATUS_LINE:
		CompAddItem(FGetStatus);		// načtení textu stavového řádku
		return true;

	case IDF_TEXT_CONST:
		CompAddItem(FTextObj, Text.Dup(item->Name)); // textová konstanta
		return true;

//	case IDF_TEXT_LINE:
//		{
//			int i = Text.Dup(item->Name);
//			Text[i].Add(Textcrlf, 2);
//			CompAddItem(FTextObj, i);	 // textová konstanta
//		}
//		return true;
		
	case IDF_TEXT_CRLF:
		CompAddItem(FTextCRLF);				// text CR/LF
		return true;

	case IDF_FNC:
		return CompFunc(index, IDF_TEXT);	// funkce s návratem textu

	case IDF_TEXT:
		if (item->RefBlok == BufObjID)
		{
			if ((DWORD)refinx >= (DWORD)BufObjN) return false;
			item = BufObj + refinx;
			item2 = BufObj2 + refinx;
			if (Text.IsNotValid(item2->Data)) return false;

			if (item2->List < 0)
			{
				CompAddItem(FTextObj, item2->Data);
			}
			else
			{
				CompAddItem(FTextObjList, item2->Data, item2->List);
			}
			return true;
		}

		if (item->RefBlok == BufLocID)
		{
			if ((DWORD)refinx >= (DWORD)BufLocN) return false;
			item = BufLoc + refinx;
			item2 = BufLoc2 + refinx;
			if (Text.IsNotValid(item2->Data)) return false;

			if (item2->List < 0)
			{
				CompAddItem(FTextLoc, item2->Local + KorigTxt);
			}
			else
			{
				CompAddItem(FTextLocList, item2->Local + KorigTxt, item2->LocalList + KorigLst);
			}
			return true;
		}
		return false;

	case IDF_TEXT_LEFT:
		CompAddItem(FTextLeft);				// levá část textu
		CompNumSubPar(index, IDF_TEXT_LEFT_LEN, 1);
		CompTxtSubPar(index, IDF_TEXT_INPUT);
		return true;

	case IDF_TEXT_RIGHT:
		CompAddItem(FTextRight);				// pravá část textu
		CompNumSubPar(index, IDF_TEXT_RIGHT_POS, 0);
		CompTxtSubPar(index, IDF_TEXT_INPUT);
		return true;

	case IDF_TEXT_MID:
		CompAddItem(FTextMid);				// střední část textu
		CompNumSubPar(index, IDF_TEXT_MID_POS, 0);
		CompNumSubPar(index, IDF_TEXT_MID_LEN, 1);
		CompTxtSubPar(index, IDF_TEXT_INPUT);
		return true;

	case IDF_TEXTLINE_GET:
		CompAddItem(FTextlineGet);			// řádek textu
		CompNumSubPar(index, IDF_TEXTLINE_INDEX, 0);
		CompTxtSubPar(index, IDF_TEXT_INPUT);
		return true;

	case IDF_TEXT_ADD:
		return CompTxtGrp(index, FTextAdd, FTextAdd1);	// součet textů

	case IDF_DIALOG_TEXT:
		CompAddItem(FDialogText);				// textový dialog
		CompTxtSubPar(index, IDF_DIALOG_BOX_CAPTION);
		CompTxtSubPar(index, IDF_DIALOG_TEXT1);
		CompTxtSubPar(index, IDF_DIALOG_TEXT2);
		CompTxtSubPar(index, IDF_DIALOG_TEXT3);
		CompTxtSubPar(index, IDF_DIALOG_TEXT_PAR);
		return true;

	case IDF_DIALOG_OPEN:
		CompAddItem(FDialogOpen);				// dialog otevření souboru
		CompTxtSubPar(index, IDF_DIALOG_BOX_CAPTION);
		CompTxtSubPar(index, IDF_DIALOG_FILES);
		CompTxtSubPar(index, IDF_DIALOG_EXT);
		CompTxtSubPar(index, IDF_DIALOG_NAME);
		return true;

	case IDF_DIALOG_SAVE:
		CompAddItem(FDialogSave);				// dialog uložení souboru
		CompTxtSubPar(index, IDF_DIALOG_BOX_CAPTION);
		CompTxtSubPar(index, IDF_DIALOG_FILES);
		CompTxtSubPar(index, IDF_DIALOG_EXT);
		CompTxtSubPar(index, IDF_DIALOG_NAME);
		return true;

	case IDF_DIALOG_FIND:
		CompAddItem(FDialogFind);				// dialog nalezení textu
		return true;

	case IDF_DIALOG_COLOR:
		CompAddItem(FDialogColor);				// dialog výběru barvy
		return true;

	case IDF_TEXT_FORMAT:
		return CompNumPar(index, FTextFormat); // převod čísla na text

	case IDF_TEXT_FORMAT2:
		return CompNumPar(index, FTextFormat2); // převod čísla na text 2 číslice

	case IDF_TEXT_FORMATH:
		return CompNumPar(index, FTextFormatH); // převod čísla na HEX text

	case IDF_TEXT_LOWER:
		return CompTxtPar(index, FTextLower); // převod na malá písmena

	case IDF_TEXT_UPPER:
		return CompTxtPar(index, FTextUpper); // převod na velká písmena

	case IDF_CHAR:
		return CompNumPar(index, FTextChar); // převod čísla na znak

	case IDF_KLAV_CHAR_WAIT:
		CompAddItem(FKeyCharWait);		// načtení znaku z klávesnice
		return true;

	case IDF_KLAV_CHAR:
		CompAddItem(FKeyChar);			// vstup znaku z klávesnice
		return true;

	case IDF_FILE_ACT:
		CompAddItem(FGetFileAct);		// aktivní adresář
		return true;

	case IDF_FILE_DISKLABEL:
		CompAddItem(FGetDiskLabel);		// jméno disku
		return true;

	case IDF_FILE_READ:
		CompAddItem(FGetFileRead);		// jméno souboru pro čtení
		return true;

	case IDF_FILE_WRITE:
		CompAddItem(FGetFileWrite);		// jméno souboru pro zápisu
		return true;

	case IDF_FILE_TEXT0:
		CompAddItem(FGetFileText0);		// načtení textu zakončeného nulou
		return true;

	case IDF_FILE_TEXTCR:
		CompAddItem(FGetFileTextCR);	// načtení textu zakončeného s CR
		return true;

	case IDF_FILE_TEXTN:
		CompAddItem(FGetFileTextN);		// načtení textu s pevnou délkou
		return true;

	case IDF_DATETIME_DATEEUR:
		return CompNumPar(index, FTimeDateEur);	// převod data na evropský formát

	case IDF_DATETIME_DATEUSA:
		return CompNumPar(index, FTimeDateUsa);	// převod data na americký formát

	case IDF_DATETIME_DATEJAP:
		return CompNumPar(index, FTimeDateJap);	// převod data na japonský formát

	case IDF_DATETIME_TIME24:
		return CompNumPar(index, FTimeTime24);	// převod času na formát 24

	case IDF_DATETIME_TIME12:
		return CompNumPar(index, FTimeTime12);	// převod času na formát 12

	case IDF_CONSOLE_IN:
		CompAddItem(FConsoleIn);				// vstup textu z konzoly
		return true;

	case IDF_COMMAND_LINE:
		CompAddItem(FGetCommandLine);			// příkazový řádek
		return true;

	case IDF_COMMAND_PAR:
		return CompNumPar(index, FCommandPar);	// parametr programu

	case IDF_EXEC_NAME:
		CompAddItem(FExecName);					// jméno programu
		return true;

	case IDF_WINDOW_TEXT:
		CompAddItem(FGetWindowText);			// text okna
		return true;

	case IDF_FILE_LIST:
		if (!CompTxtPar(index, FFileList))		// seznam souborů
		{
			CompAddItem(FFileListAll);
		}
		return true;


// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI

	case IDF_CONNECT_LIST:
		CompAddItem(FConnectList);				// seznam spojení DirectPlay
		return true;

	case IDF_CONNECT_ACT:
		CompAddItem(FGetConnectAct);			// aktivní spojení DirectPlay
		return true;

	case IDF_GAME_LIST:
		CompAddItem(FGameList);					// seznam her DirectPlay
		return true;

	case IDF_GAME_ACT:
		CompAddItem(FGetGameAct);				// aktivní hra DirectPlay
		return true;

	case IDF_PLAYER_LIST:
		CompAddItem(FPlayerList);				// seznam hráčů DirectPlay
		return true;

	case IDF_PLAYER_ACT:
		CompAddItem(FGetPlayerAct);				// aktivní hráč DirectPlay
		return true;

	case IDF_GAME_DATAT:
		CompAddItem(FGetGameDataT);				// načtení textu z paketu
		return true;

	case IDF_GAME_IP:
		CompAddItem(FGetGameIP);				// zjištění IP počítače
		return true;

	case IDF_DDE_APP:
		CompAddItem(FGetDDEApp);				// zjištění seznamu aplikací
		return true;

	case IDF_DDE_TOPIC:
		CompAddItem(FGetDDETopic);				// zjištění seznamu témat
		return true;

	case IDF_UDP_SEND_IP:
		CompAddItem(FGetUDPSendIP);				// zjištění vysílací IP adresy UDP
		return true;

	case IDF_UDP_REC_IP:
		CompAddItem(FGetUDPRecIP);				// zjištění přijaté IP adresy UDP
		return true;

	case IDF_UDP_TEXT:
		CompAddItem(FGetUDPText);				// příjem textu UDP
		return true;

#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------

	case IDF_USERFONT:
		CompAddItem(FGetUserFont);				// zjištění uživatelského fontu
		return true;

	case IDF_ENUMFONTS:
		CompAddItem(FEnumFonts);				// zjištění seznamu fontů
		return true;

	case IDF_ALIAS:
		CompAddItem(FGetAlias);					// načtení alias souboru
		return true;

// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI

	case IDF_MEMORY_TEXT0:							// text zakončený nulou
		CompAddItem(FGetMemoryText0);
		return true;

	case IDF_MEMORY_TEXT0P:							// ukazatel na text zakončený nulou
		CompAddItem(FGetMemoryText0P);
		return true;

	case IDF_MEMORY_TEXTN:							// text s pevnou délkou
		CompAddItem(FGetMemoryTextN);
		return true;

	case IDF_INI_FILE:								// jméno konfiguračního souboru
		CompAddItem(FGetIniFile);
		return true;

	case IDF_INI_SECTION:							// jméno konfigurační sekce
		CompAddItem(FGetIniSection);
		return true;

	case IDF_INI_VALUE:								// jméno konfiguračního parametru
		CompAddItem(FGetIniValue);
		return true;

	case IDF_INI_STRING:							// čtení konfiguračního textu
		if (!CompTxtPar(index, FGetIniString))
		{
			CompAddItem(FGetIniString0);
		}
		return true;

	case IDF_REG_SUBKEY:							// klíč registrů
		CompAddItem(FGetRegSubkey);
		return true;

	case IDF_REG_VALUE:								// jméno položky registru
		CompAddItem(FGetRegValue);
		return true;

	case IDF_REG_STRING:							// čtení textu z registru
		if (!CompTxtPar(index, FGetRegString))
		{
			CompAddItem(FGetRegString0);
		}
		return true;

#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------


// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI

	case IDF_COM_TEXT:
		CompAddItem(FGetComText);				// načtení znaku z portu
		return true;

#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------

// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI

	case IDF_MIX_DEVICE:
		CompAddItem(FGetMixDevice);				// zjištění seznamu mixážních zařízení
		return true;

	case IDF_MIX_LINE_DST:
		CompAddItem(FGetMixLineDst);			// zjištění seznamu cílových signálů
		return true;

	case IDF_MIX_LINE_SRC:
		CompAddItem(FGetMixLineSrc);			// zjištění seznamu zdrojových signálů
		return true;

	case IDF_MIX_CONTROL:
		CompAddItem(FGetMixControl);			// zjištění seznamu ovládacích prvků
		return true;

	case IDF_MIX_CONTROL_LIST:
		CompAddItem(FGetMixCtrlList);			// zjištění seznamu hodnot ovládacích prvků
		return true;
		
#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------

	case IDF_DIRECT3D_MODE_LIST:
		CompAddItem(FGet3DMode);				// zjištění seznamu videomódů
		return true;

	case IDF_TEXT_BLOCK:						// blok textu
		CompAddItem(FTextBlock);
		CompNumSubPar(index, IDF_TEXT_BLOCK_BEGLINE, 0);
		CompNumSubPar(index, IDF_TEXT_BLOCK_BEGPOS, 0);
		CompNumSubPar(index, IDF_TEXT_BLOCK_ENDLINE, -1);
		CompNumSubPar(index, IDF_TEXT_BLOCK_ENDPOS, -1);
		CompTxtSubPar(index, IDF_TEXT_INPUT);
		return true;

	default:
		return false;
	}
}


/////////////////////////////////////////////////////////////////////////////
// překlad příkazu s textovým parametrem

bool CompTxtPar(int index, PROCCOM func)
{
	return CompTxtPar(index, func, 0, -1);
}

bool CompTxtPar(int index, PROCCOM func, int data)
{
	return CompTxtPar(index, func, data, -1);
}

bool CompTxtPar(int index, PROCCOM func, int data, int list)
{
	int zahlavi = CompAddItem(func, data, list);

	if ((BufEdi2[index].Items == 1) || !CompTxt(index+1))
	{
		ProgNum = zahlavi;
		return false;
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// překlad příkazu s textovým podparametrem (hledaným podle identifikace)

void CompTxtSubPar(int index, int idf)
{
// korekce identifikace funkce
	idf -= IDF;

// ukazatel výchozího prvku
	PETPROG*	item = BufEdi + index;
	PETPROG2*	item2 = BufEdi2 + index;

// kontrola, zda má položka nějaké potomky
	if (item->Param & PETPROG_CHILDS)
	{
		int posun = 1;

// cyklus přes všechny potomky
		do {

// adresa dalšího potomka
			index += posun;
			item += posun;
			item2 += posun;

// test, zda to je hledaný prvek - načtení prvku
			if ((item->Func == idf) &&
				(item->Param & PETPROG_CHILDS) && 
				CompTxt(index + 1))
			{
				return;
			}

// posun pro příští prvek
			posun = item2->Items;

// dokud je další potomek
		} while (item->Param & PETPROG_NEXT);
	}

// použije se implicitní hodnota - prázdný text
	CompAddItem(FTextEmpty);
}
