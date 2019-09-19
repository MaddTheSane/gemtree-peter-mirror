
#pragma warning ( disable: 4201)		// hlášení - nepojmenovaná struktura
#include <windows.h>					// základní definice WINDOWS
#pragma warning ( default: 4201)		// hlášení - nepojmenovaná struktura

#pragma warning ( disable: 4100)		// hlášení - nepoužitý formální parametr
#pragma warning ( disable: 4514)		// hlášení - odstraněna inline funkce

#include "Resource.h"

//////////////////////////////////////////////////////////////////////////////
// Data v datovém bloku jsou nutná, aby bylo dost sekcí pro vytvoření sekce Petra
char DATAsize[] =									// buffer importů od překladače
{
	"DATAsize"										// sem bude uložena adresa a velikost dat, současně je to identifikátor
};

LPCTSTR ProductKey0 = "Software\\Gemtree Software\\Peter";

void WinMainCRTStartup()
{
	DWORD num = 0;
	HKEY hkey;

// zjištění jazyku spuštěného Petra
	if (::RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		ProductKey0,
		0,
		KEY_QUERY_VALUE,
		&hkey) == ERROR_SUCCESS)
	{
		DWORD type = REG_DWORD;
		DWORD bufsize = 4;

		::RegQueryValueEx(
			hkey,
			"Language",
			0,
			&type,
			(BYTE*)&num,
			&bufsize);

		::RegCloseKey(hkey);
	}

	if (num == 0)
	{
		num = (::GetUserDefaultLangID() & 0xff);
	}

	LPCTSTR nadp;
	LPCTSTR text;

	switch (num)
	{
// česky
	case 0x05:
		nadp = "Nespustitelný program";
		text = "Tento program byl vytvořen programovacím nástrojem Petr\na je nyní uložen v nespustitelné formě.";
		break;

// německy
	case 0x07:
		nadp = "Speicherplatzmangel";
		text = "Dieses Programm wurde mit dem Programmiermittel Peter hergestellt.\nJetzt ist es in einer nicht startbaren Form gespeichert.";
		break;

// slovensky
	case 0x1b:
		nadp = "Nespustiteľný program";
		text = "Tento program bol vytvorený programovacím nástrojom Peter\na je teraz uložený v nespustiteľnej forme.";
		break;

// anglicky
	default:
		nadp = "Nonexecutable program";
		text = "This program was created by a programmable tool Peter\nand is now stored in a nonexecutable form.";
		break;
	}

	::MessageBox(NULL, text, nadp, MB_OK | MB_ICONWARNING);
	::ExitProcess(255);
}
