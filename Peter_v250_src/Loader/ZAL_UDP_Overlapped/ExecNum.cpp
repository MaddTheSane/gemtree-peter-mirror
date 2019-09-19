
#include "Main.h"

/***************************************************************************\
*																			*
*					Provádění programu - číselný výraz						*
*																			*
\***************************************************************************/

#pragma optimize("t", on)			// optimalizace na maximální rychlost

SYSTEMTIME	SystemTime;				// pomocná proměnná pro obsluhu času

/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru převedeného na int - seříznuto směrem k nule

//int FInt() { return (int)FNum(); }


/////////////////////////////////////////////////////////////////////////////
// načtení úhlu s normalizací rozsahu 0 až 2pi

#pragma warning ( disable: 4725)	// hlášení - problémová instrukce

double FAngle()
{

// načtení úhlu
	double num = FNum();

// bude normalizace pro kladný úhel > 2pi
	if (num >= pi2)
	{

// normalizace velkých kladných úhlů
		if (num >= pi8)
		{
			double num2 = pi2;

#ifdef _M_IX86

			_asm {
				fld			num2		// načtení dělitele -> ST0
				fld			num			// načtení dělence -> ST0, dělitel -> ST1
X1:				fprem					// zbytek -> ST0, v ST1 zůstává dělitel
				wait					// synchronizace
				fnstsw		ax			// načtení stavového slova
				wait					// synchronizace
				sahf					// nastavení stavových příznaků
				jp			X1			// operace neúplná - pokračování
				fstp		num			// výsledek operace
				fstp		st(0)		// zrušení ST0 (dělitel)
			}

#else

			num = fmod(num, num2);

#endif

#define MULT 360						// tj. 5*3*3*2*2*2 (asi tak možné násobky)
#define EPS 0.01						// asi tak pro rozeznání násobku

// zarovnání nepřesností, pokud se jedná o nějaký podíl úhlu
			double num3 = num / pi * MULT;
			int m = Round(num3);		// to jsou možné násobky
			double num4 = num3 - m;		// odchylka od násobků
			if ((num4 <= EPS) && (num4 >= -EPS))
			{
				num = m * pi / MULT;	// zarovnání výsledku

				if (num >= pi2) num += pi2m;
			}
		}

// jinak normalizace malých kladných úhlů
		else
		{
			do 
			{
				num += pi2m;
			} while (num >= pi2);
		}
	}
	else
	{

// bude normalizace pro záporný úhel < 0
		if (num < 0)
		{

// normalizace velkých záporných úhlů
			if (num <= pi8m)
			{
				num = -num;
				double num2 = pi2;

#ifdef _M_IX86

				_asm {
					fld			num2		// načtení dělitele -> ST0
					fld			num			// načtení dělence -> ST0, dělitel -> ST1
X2:					fprem					// zbytek -> ST0, v ST1 zůstává dělitel
					wait					// synchronizace
					fnstsw		ax			// načtení stavového slova
					wait					// synchronizace
					sahf					// nastavení stavových příznaků
					jp			X2			// operace neúplná - pokračování
					fstp		num			// výsledek operace
					fstp		st(0)		// zrušení ST0 (dělitel)
				}

#else

				num = fmod(num, num2);

#endif

				num = pi2 - num;
				if (num >= pi2) num += pi2m;

// zarovnání nepřesností, pokud se jedná o nějaký podíl úhlu
				double num3 = num / pi * MULT;
				int m = Round(num3);		// to jsou možné násobky
				double num4 = num3 - m;		// odchylka od násobků
				if ((num4 <= EPS) && (num4 >= -EPS))
				{
					num = m * pi / MULT;	// zarovnání výsledku

					if (num >= pi2) num += pi2m;
				}
			}

// jinak normalizace malých záporných úhlů
			else
			{
				do 
				{
					num += pi2;
				} while (num < 0);
			}
		}
	}

	return num;
}

#pragma warning ( default: 4725)	// hlášení - problémová instrukce


/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru převedeného na int se zaokrouhlením

int FIntR()
{
	int result;						// výsledek operace
	double num;						// načtené číslo

	num = FNum();					// načtení čísla

#ifdef _M_IX86

	_asm {
		fld			num				// načtení čísla do koprocesoru
		fistp		result			// převod čísla na celé číslo
	}

#else

	result = Round(num);			// zaokrouhlení čísla

#endif

	return result;					// výsledek operace
}


/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru převedeného na DWORD se zaokrouhlením

DWORD FIntRN()
{
	__int64 result;					// výsledek operace
	double num;						// načtené číslo

	num = FNum();					// načtení čísla

#ifdef _M_IX86

	_asm {
		fld			num				// načtení čísla do koprocesoru
		fistp		result			// převod čísla na celé číslo
	}

#else

	if (num >= 0x80000000)
	{
		result = Round(num - 0x80000000) + 0x80000000;
	}
	else
	{
		result = Round(num);			// zaokrouhlení čísla
	}

#endif

	return (DWORD)result;				// výsledek operace
}

/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru převedeného na INT64 se zaokrouhlením

hyper FIntHN()
{
	hyper result;					// výsledek operace
	double num;						// načtené číslo

	num = FNum();					// načtení čísla

#ifdef _M_IX86

	_asm {
		fld			num				// načtení čísla do koprocesoru
		fistp		result			// převod čísla na celé číslo
	}

#else

	if (num >= 0x80000000)
	{
		result = Round(num - 0x80000000) + 0x80000000;
	}
	else
	{
		if (num < 0)
		{
			result = -Round(-num);
		}
		else
		{
			result = Round(num);			// zaokrouhlení čísla
		}
	}

#endif

	return result;				// výsledek operace
}

/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru převedeného na grafickou souřadnici X

int FIntX()
{
	int result;						// výsledek operace
	double num;						// načtené číslo

	num = FNum() * ICONWIDTH;		// načtení čísla

#ifdef _M_IX86

	_asm {
		fld			num				// načtení čísla do koprocesoru
		fistp		result			// převod čísla na celé číslo
	}

#else

	result = Round(num);			// zaokrouhlení čísla

#endif

	return result;					// výsledek operace
}


/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru vynásobeného * 1000 (čas v milisekundách)

int FInt1000()
{
	int result;						// výsledek operace
	double num;						// načtené číslo

	num = FNum() * 1000;			// načtení čísla

#ifdef _M_IX86

	_asm {
		fld			num				// načtení čísla do koprocesoru
		fistp		result			// převod čísla na celé číslo
	}

#else

	result = Round(num);			// zaokrouhlení čísla

#endif

	return result;					// výsledek operace
}


/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru vynásobeného * ICONWIDTH * 2 (průměr kruhu)

int FIntX2()
{
	int result;						// výsledek operace
	double num;						// načtené číslo

	num = FNum() * ICONWIDTH * 2;				// načtení čísla

#ifdef _M_IX86

	_asm {
		fld			num				// načtení čísla do koprocesoru
		fistp		result			// převod čísla na celé číslo
	}

#else

	result = Round(num);			// zaokrouhlení čísla

#endif

	return result;					// výsledek operace
}


/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru vynásobeného * 18.20648 (čas v časových impulsech)

int FInt18()
{
	int result;						// výsledek operace
	double num;						// načtené číslo

	num = FNum() * 18.20648;		// načtení čísla

#ifdef _M_IX86

	_asm {
		fld			num				// načtení čísla do koprocesoru
		fistp		result			// převod čísla na celé číslo
	}

#else

	result = Round(num);			// zaokrouhlení čísla

#endif

	return result;					// výsledek operace
}


/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru s vynásobením * 255 a omezením 0 až 255

BYTE FInt255()
{
	int result;						// výsledek operace
	double num;						// načtené číslo
	num = FNum() * 255;				// načtení čísla

#ifdef _M_IX86

	_asm {
		fld			num				// načtení čísla do koprocesoru
		fistp		result			// převod čísla na celé číslo
	}

#else

	result = Round(num);			// načtení čísla se zaokrouhlením

#endif

	if ((DWORD)result > 255)
	{
		if (result < 0)
		{
			result = 0;
		}
		else
		{
			result = -1;
		}
	}

	return (BYTE)result;			// výsledek operace
}


/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru převedeného na souborový čas

__int64 FNumFileTime()
{
	__int64 cas;					// pomocná proměnná k načtení času
	double num;						// načtené číslo
	num = FNum() * FILETIMEKONV;	// načtení čísla

#ifdef _M_IX86

	_asm {
		fld			num				// načtení čísla do koprocesoru
		fistp		cas				// převod čísla na celé číslo
	}

#else

	cas = (__int64)(num + 0.5);

#endif

	if (((char*)&cas)[7] < 0) return 0;

	return cas;
}


/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru převedeného na systémový čas

void FNumSysTime()
{
	__int64 cas = FNumFileTime();	// načtení souborového času
	::FileTimeToSystemTime((FILETIME*)&cas, &SystemTime);
}


/////////////////////////////////////////////////////////////////////////////
// načtení číselného parametru převedeného na světový souborový čas

__int64 FNumFileTimeUTC()
{
// načtení lokálního času
	__int64 cas = FNumFileTime();	// načtení lokálního času

// převod na světový čas
	__int64 cas2;
	::LocalFileTimeToFileTime((FILETIME*)&cas, (FILETIME*)&cas2);

	return cas2;
}


/////////////////////////////////////////////////////////////////////////////
// načtení barvy

int FNumCol()
{
/*
	DWORD barva;					// načtená barva
	double num;						// načtené číslo
	num = FNum();					// načtení čísla

#ifdef _M_IX86

	_asm {
		fld			num				// načtení čísla do koprocesoru
		fistp		barva			// převod čísla na celé číslo
	}

#else

	barva = Round(num);				// zaokrouhlení čísla

#endif
*/
	return PalImport(FIntRN());		// výsledek operace
}


/////////////////////////////////////////////////////////////////////////////
// načtení souřadnic Petra (ECX = adresa x, EDX = adresa y)

bool _fastcall PetrXY(int& x, int& y)
{
#ifdef _M_IX86

	__asm {
		mov			eax,[Sprite.m_Data]								// adresa dat sprajtu
		fld			qword ptr [eax + 0*TYPE CSprite]CSprite.m_CilX  // načtení souřadnice X
		fistp		dword ptr [ecx]									// převod X na celé číslo
		fld			qword ptr [eax + 0*TYPE CSprite]CSprite.m_CilY  // načtení souřadnice Y
		fistp		dword ptr [edx]									// převod Y na celé číslo
	}

#else

	x = Round(Sprite[0].CilX());
	y = Round(Sprite[0].CilY());

#endif

// kontrola platnosti souřadnice
	return (((DWORD)x < (DWORD)WidthN) && ((DWORD)y < (DWORD)HeightN));
}


/////////////////////////////////////////////////////////////////////////////
// načtení souřadnic Petry (ECX = adresa x, EDX = adresa y)

bool _fastcall Petr2XY(int& x, int& y)
{
#ifdef _M_IX86

	__asm {
		mov			eax,[Sprite.m_Data]								// adresa dat sprajtu
		fld			qword ptr [eax + 1*TYPE CSprite]CSprite.m_CilX  // načtení souřadnice X
		fistp		dword ptr [ecx]									// převod X na celé číslo
		fld			qword ptr [eax + 1*TYPE CSprite]CSprite.m_CilY  // načtení souřadnice Y
		fistp		dword ptr [edx]									// převod Y na celé číslo
	}

#else

	x = Round(Sprite[1].CilX());
	y = Round(Sprite[1].CilY());

#endif

// kontrola platnosti souřadnice
	return (((DWORD)x < (DWORD)WidthN) && ((DWORD)y < (DWORD)HeightN));
}


/////////////////////////////////////////////////////////////////////////////
// načtení pozice Petra (NULL=neplatná)

MAPITEM* PetrXYMap()
{
	int x;			// souřadnice X Petra
	int y;			// souřadnice Y Petra

#ifdef _M_IX86

	__asm {
		mov			eax,[Sprite.m_Data]								// adresa dat sprajtu
		fld			qword ptr [eax + 0*TYPE CSprite]CSprite.m_CilX  // načtení souřadnice X
		fistp		dword ptr x										// převod X na celé číslo
		fld			qword ptr [eax + 0*TYPE CSprite]CSprite.m_CilY  // načtení souřadnice Y
		fistp		dword ptr y										// převod Y na celé číslo
	}

#else

	x = Round(Sprite[0].CilX());
	y = Round(Sprite[0].CilY());

#endif

// kontrola platnosti souřadnice
	if (((DWORD)x < (DWORD)WidthN) && ((DWORD)y < (DWORD)HeightN))
	{
		return &Map[0].At(x, y);
	}
	else
	{
		return NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// načtení pozice Petry (NULL=neplatná)

MAPITEM* Petr2XYMap()
{
	int x;			// souřadnice X Petra
	int y;			// souřadnice Y Petra

#ifdef _M_IX86

	__asm {
		mov			eax,[Sprite.m_Data]								// adresa dat sprajtu
		fld			qword ptr [eax + 1*TYPE CSprite]CSprite.m_CilX  // načtení souřadnice X
		fistp		dword ptr x										// převod X na celé číslo
		fld			qword ptr [eax + 1*TYPE CSprite]CSprite.m_CilY  // načtení souřadnice Y
		fistp		dword ptr y										// převod Y na celé číslo
	}

#else

	x = Round(Sprite[1].CilX());
	y = Round(Sprite[1].CilY());

#endif

// kontrola platnosti souřadnice
	if (((DWORD)x < (DWORD)WidthN) && ((DWORD)y < (DWORD)HeightN))
	{
		return &Map[0].At(x, y);
	}
	else
	{
		return NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// načtení příští souřadnice Petra (ECX = adresa x, EDX = adresa y)

bool _fastcall PetrXYNext(int& x, int& y)
{
#ifdef _M_IX86

	__asm {
		mov			eax,[Sprite.m_Data]								// adresa dat sprajtu
		fld			qword ptr [eax + 0*TYPE CSprite]CSprite.m_CilX  // načtení souřadnice X
		fistp		dword ptr [ecx]									// převod X na celé číslo
		fld			qword ptr [eax + 0*TYPE CSprite]CSprite.m_CilY  // načtení souřadnice Y
		fistp		dword ptr [edx]									// převod Y na celé číslo
	}

#else

	x = Round(Sprite[0].CilX());
	y = Round(Sprite[0].CilY());

#endif

	switch (16 * Sprite[0].SmerN() / Sprite[0].Smer())
	{
	case 1:					// vpravo mírně vzhůru
	case 2:					// vpravo vzhůru
	case 3:					// mírně vpravo vzhůru
		x++;

	case 4:					// vzhůru
		y++;
		break;

	case 5:					// mírně vlevo vzhůru
	case 6:					// vlevo vzhůru
	case 7:					// vlevo mírně vzhůru
		y++;

	case 8:					// vlevo
		x--;
		break;

	case 9:					// vlevo mírně dolů
	case 10:				// vlevo dolů
	case 11:				// mírně vlevo dolů
		x--;

	case 12:				// dolů
		y--;
		break;

	case 13:				// mírně vpravo dolů
	case 14:				// vpravo dolů
	case 15:				// vpravo mírně dolů
		y--;

	case 0:					// vpravo
		x++;
		break;
	}

// kontrola platnosti souřadnice
	return (((DWORD)x < (DWORD)WidthN) && ((DWORD)y < (DWORD)HeightN));
}


/////////////////////////////////////////////////////////////////////////////
// načtení příští souřadnice Petry (ECX = adresa x, EDX = adresa y)

bool _fastcall Petr2XYNext(int& x, int& y)
{
#ifdef _M_IX86

	__asm {
		mov			eax,[Sprite.m_Data]								// adresa dat sprajtu
		fld			qword ptr [eax + 1*TYPE CSprite]CSprite.m_CilX  // načtení souřadnice X
		fistp		dword ptr [ecx]									// převod X na celé číslo
		fld			qword ptr [eax + 1*TYPE CSprite]CSprite.m_CilY  // načtení souřadnice Y
		fistp		dword ptr [edx]									// převod Y na celé číslo
	}

#else

	x = Round(Sprite[1].CilX());
	y = Round(Sprite[1].CilY());

#endif

	switch (16 * Sprite[1].SmerN() / Sprite[1].Smer())
	{
	case 1:					// vpravo mírně vzhůru
	case 2:					// vpravo vzhůru
	case 3:					// mírně vpravo vzhůru
		x++;

	case 4:					// vzhůru
		y++;
		break;

	case 5:					// mírně vlevo vzhůru
	case 6:					// vlevo vzhůru
	case 7:					// vlevo mírně vzhůru
		y++;

	case 8:					// vlevo
		x--;
		break;

	case 9:					// vlevo mírně dolů
	case 10:				// vlevo dolů
	case 11:				// mírně vlevo dolů
		x--;

	case 12:				// dolů
		y--;
		break;

	case 13:				// mírně vpravo dolů
	case 14:				// vpravo dolů
	case 15:				// vpravo mírně dolů
		y--;

	case 0:					// vpravo
		x++;
		break;
	}

// kontrola platnosti souřadnice
	return (((DWORD)x < (DWORD)WidthN) && ((DWORD)y < (DWORD)HeightN));
}


/***************************************************************************\
*																			*
*									funkce									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// funkce s návratem číselné hodnoty

double FNumFunc()
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
	double result = Real[Real.Num() - res];

// zrušení lokálních proměnných
	FCommand();

	return result;
}


/***************************************************************************\
*																			*
*								proměnné									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// číselná konstanta

double FNumConst() { return ExecItem[-1].Double; }


/////////////////////////////////////////////////////////////////////////////
// globální proměnná (Data = index)

double FNumObj()
{
// index
	int inx = ExecItem[-1].Data;

// data
	return Real[inx];
}


/////////////////////////////////////////////////////////////////////////////
// globalní proměnná v seznamu (Data = index, List = seznam)

double FNumObjList()
{
// index seznamu
	int list = ExecItem[-1].List;

// index proměnné, inkrementace indexu seznamu
	int inx = ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// data
	return Real[inx];
}


/////////////////////////////////////////////////////////////////////////////
// lokální proměnná (Data = index)

double FNumLoc()
{
// index proměnné
	int inx = Real.Num() - ExecItem[-1].Data;

// data
	return Real[inx];
}


/////////////////////////////////////////////////////////////////////////////
// lokální proměnná v seznamu (Data = index, List = seznam)

double FNumLocList()
{
// index seznamu
	int list = List.Num() - ExecItem[-1].List;

// index proměnné, inkrementace indexu seznamu
	int inx = Real.Num() - ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// data
	return Real[inx];
}


/////////////////////////////////////////////////////////////////////////////
// index globálního seznamu

double FGetList()
{
// index seznamu
	int list = ExecItem[-1].List;

// hodnota indexu
	return List[list].Index;
}


/////////////////////////////////////////////////////////////////////////////
// index lokálního seznamu

double FGetListLoc()
{
// index seznamu
	int list = List.Num() - ExecItem[-1].List;

// hodnota indexu
	return List[list].Index;
}


/////////////////////////////////////////////////////////////////////////////
// inkrementace globálního seznamu

double FGetListAuto()
{
// index seznamu
	int list = ExecItem[-1].List;

// nastavení nové hodnoty inkrementace
	return List[list].Auto;
}


/////////////////////////////////////////////////////////////////////////////
// inkrementace lokálního seznamu

double FGetListAutoLoc()
{
// index seznamu
	int list = List.Num() - ExecItem[-1].List;

// nastavení nové hodnoty inkrementace
	return List[list].Auto;
}


/////////////////////////////////////////////////////////////////////////////
// souřadnice X globálního sprajtu (Data = index)

double FGetSpriteX()
{
// index sprajtu
	int inx = ExecItem[-1].Data;

// souřadnice X sprajtu
	return Sprite[inx].AktX();
}


/////////////////////////////////////////////////////////////////////////////
// souřadnice X sprajtu v seznamu (Data = index, List = seznam)

double FGetSpriteXList()
{
// index seznamu
	int list = ExecItem[-1].List;

// index sprajtu, inkrementace indexu seznamu
	int inx = ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// souřadnice X sprajtu
	return Sprite[inx].AktX();
}


/////////////////////////////////////////////////////////////////////////////
// souřadnice X lokálního sprajtu (Data = index)

double FGetSpriteXLoc()
{
// index sprajtu
	int inx = Sprite.Num() - ExecItem[-1].Data;

// souřadnice X sprajtu
	return Sprite[inx].AktX();
}


/////////////////////////////////////////////////////////////////////////////
// souřadnice X lokálního sprajtu v seznamu (Data = index, List = seznam)

double FGetSpriteXLocList()
{
// index seznamu
	int list = List.Num() - ExecItem[-1].List;

// index sprajtu, inkrementace indexu seznamu
	int inx = Sprite.Num() - ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// souřadnice X sprajtu
	return Sprite[inx].AktX();
}


/////////////////////////////////////////////////////////////////////////////
// souřadnice Y globálního sprajtu (Data = index)

double FGetSpriteY()
{
// index sprajtu
	int inx = ExecItem[-1].Data;

// souřadnice Y sprajtu
	return Sprite[inx].AktY();
}


/////////////////////////////////////////////////////////////////////////////
// souřadnice Y sprajtu v seznamu (Data = index, List = seznam)

double FGetSpriteYList()
{
// index seznamu
	int list = ExecItem[-1].List;

// index sprajtu, inkrementace indexu seznamu
	int inx = ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// souřadnice Y sprajtu
	return Sprite[inx].AktY();
}


/////////////////////////////////////////////////////////////////////////////
// souřadnice Y lokálního sprajtu (Data = index)

double FGetSpriteYLoc()
{
// index sprajtu
	int inx = Sprite.Num() - ExecItem[-1].Data;

// souřadnice Y sprajtu
	return Sprite[inx].AktY();
}


/////////////////////////////////////////////////////////////////////////////
// souřadnice Y lokálního sprajtu v seznamu (Data = index, List = seznam)

double FGetSpriteYLocList()
{
// index seznamu
	int list = List.Num() - ExecItem[-1].List;

// index sprajtu, inkrementace indexu seznamu
	int inx = Sprite.Num() - ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// souřadnice Y sprajtu
	return Sprite[inx].AktY();
}


/////////////////////////////////////////////////////////////////////////////
// směr globálního sprajtu (Data = index)

double FGetSpriteSmer()
{
// index sprajtu
	int inx = ExecItem[-1].Data;

// směr sprajtu
	return Sprite[inx].SmerR();
}


/////////////////////////////////////////////////////////////////////////////
// směr sprajtu v seznamu (Data = index, List = seznam)

double FGetSpriteSmerList()
{
// index seznamu
	int list = ExecItem[-1].List;

// index sprajtu, inkrementace indexu seznamu
	int inx = ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// směr sprajtu
	return Sprite[inx].SmerR();
}


/////////////////////////////////////////////////////////////////////////////
// směr lokálního sprajtu (Data = index)

double FGetSpriteSmerLoc()
{
// index sprajtu
	int inx = Sprite.Num() - ExecItem[-1].Data;

// směr sprajtu
	return Sprite[inx].SmerR();
}


/////////////////////////////////////////////////////////////////////////////
// směr lokálního sprajtu v seznamu (Data = index, List = seznam)

double FGetSpriteSmerLocList()
{
// index seznamu
	int list = List.Num() - ExecItem[-1].List;

// index sprajtu, inkrementace indexu seznamu
	int inx = Sprite.Num() - ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// směr sprajtu
	return Sprite[inx].SmerR();
}


/////////////////////////////////////////////////////////////////////////////
// fáze globálního sprajtu (Data = index)

double FGetSpriteFaze()
{
// index sprajtu
	int inx = ExecItem[-1].Data;

// fáze sprajtu
	return SpriteNextFaze(inx);
}


/////////////////////////////////////////////////////////////////////////////
// fáze sprajtu v seznamu (Data = index, List = seznam)

double FGetSpriteFazeList()
{
// index seznamu
	int list = ExecItem[-1].List;

// index sprajtu, inkrementace indexu seznamu
	int inx = ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// fáze sprajtu
	return SpriteNextFaze(inx);
}


/////////////////////////////////////////////////////////////////////////////
// fáze lokálního sprajtu (Data = index)

double FGetSpriteFazeLoc()
{
// index sprajtu
	int inx = Sprite.Num() - ExecItem[-1].Data;

// fáze sprajtu
	return SpriteNextFaze(inx);
}


/////////////////////////////////////////////////////////////////////////////
// fáze lokálního sprajtu v seznamu (Data = index, List = seznam)

double FGetSpriteFazeLocList()
{
// index seznamu
	int list = List.Num() - ExecItem[-1].List;

// index sprajtu, inkrementace indexu seznamu
	int inx = Sprite.Num() - ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// fáze sprajtu
	return SpriteNextFaze(inx);
}


/////////////////////////////////////////////////////////////////////////////
// stav globálního sprajtu (Data = index)

double FGetSpriteStatus()
{
// index sprajtu
	int inx = ExecItem[-1].Data;

// stav sprajtu
	return Sprite[inx].Status();
}


/////////////////////////////////////////////////////////////////////////////
// stav sprajtu v seznamu (Data = index, List = seznam)

double FGetSpriteStatusList()
{
// index seznamu
	int list = ExecItem[-1].List;

// index sprajtu, inkrementace indexu seznamu
	int inx = ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// stav sprajtu
	return Sprite[inx].Status();
}


/////////////////////////////////////////////////////////////////////////////
// stav lokálního sprajtu (Data = index)

double FGetSpriteStatusLoc()
{
// index sprajtu
	int inx = Sprite.Num() - ExecItem[-1].Data;

// stav sprajtu
	return Sprite[inx].Status();
}


/////////////////////////////////////////////////////////////////////////////
// stav lokálního sprajtu v seznamu (Data = index, List = seznam)

double FGetSpriteStatusLocList()
{
// index seznamu
	int list = List.Num() - ExecItem[-1].List;

// index sprajtu, inkrementace indexu seznamu
	int inx = Sprite.Num() - ExecItem[-1].Data;
	inx += List.AutoIncInx(list);

// stav sprajtu
	return Sprite[inx].Status();
}


/***************************************************************************\
*																			*
*							obsluha Petra a Petry							*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// Petr - směr

double FGetSmer() { return Sprite[0].SmerR(); }


/////////////////////////////////////////////////////////////////////////////
// Petra - směr

double FGetSmer2() { return Sprite[1].SmerR(); }


/////////////////////////////////////////////////////////////////////////////
// Petr - pozice X

double FGetPozX() { return Sprite[0].AktX(); }


/////////////////////////////////////////////////////////////////////////////
// Petra - pozice X

double FGetPozX2() { return Sprite[1].AktX(); }


/////////////////////////////////////////////////////////////////////////////
// Petr - pozice Y

double FGetPozY() { return Sprite[0].AktY(); }


/////////////////////////////////////////////////////////////////////////////
// Petra - pozice Y

double FGetPozY2() { return Sprite[1].AktY(); }


/////////////////////////////////////////////////////////////////////////////
// načtení čísla - Petr

double _fastcall FGetSwcN(int mask, char rot)
{
// načtení pozice Petra
	MAPITEM* item = PetrXYMap();

// načtení čísla, je-li platná pozice
	if (item != NULL)
	{
		return (((int)item->Param & mask) >> rot);
	}
	return 0;
}

double FGetSwcN1() { return FGetSwcN(MAP_VALUE1_MASK, MAP_VALUE1_ROT); };
double FGetSwcN2() { return FGetSwcN(MAP_VALUE2_MASK, MAP_VALUE2_ROT); };
double FGetSwcN3() { return FGetSwcN(MAP_VALUE3_MASK, MAP_VALUE3_ROT); };


/////////////////////////////////////////////////////////////////////////////
// načtení čísla - Petra

double _fastcall FGet2SwcN(int mask, char rot)
{
// načtení pozice Petry
	MAPITEM* item = Petr2XYMap();

// načtení čísla, je-li platná pozice
	if (item != NULL)
	{
		return (((int)item->Param & mask) >> rot);
	}
	return 0;
}

double FGetSwcN12() { return FGet2SwcN(MAP_VALUE1_MASK, MAP_VALUE1_ROT); };
double FGetSwcN22() { return FGet2SwcN(MAP_VALUE2_MASK, MAP_VALUE2_ROT); };
double FGetSwcN32() { return FGet2SwcN(MAP_VALUE3_MASK, MAP_VALUE3_ROT); };


/***************************************************************************\
*																			*
*								výpočty										*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// inkrementace

double FInc() { return FNum() + 1; };


/////////////////////////////////////////////////////////////////////////////
// dekrementace

double FDec() { return FNum() - 1; };


/////////////////////////////////////////////////////////////////////////////
// sinus

double FSin() { return sin(FNum()); };


/////////////////////////////////////////////////////////////////////////////
// cosinus

double FCos() { return cos(FNum()); };


/////////////////////////////////////////////////////////////////////////////
// tangens

double FTg() { return tan(FNum()); };


/////////////////////////////////////////////////////////////////////////////
// cotangens

double FCoTg() { return 1/tan(FNum()); };


/////////////////////////////////////////////////////////////////////////////
// arcsinus

double FArcSin() { return asin(FNum()); }


/////////////////////////////////////////////////////////////////////////////
// arccosinus

double FArcCos() { return acos(FNum()); };


/////////////////////////////////////////////////////////////////////////////
// arctangens

double FArcTg() { return atan(FNum()); };


/////////////////////////////////////////////////////////////////////////////
// arccotangens

double FArcCoTg() { return atan(1/FNum()); };


/////////////////////////////////////////////////////////////////////////////
// celé číslo

double FNumInt() { return (int)FNum(); };


/////////////////////////////////////////////////////////////////////////////
// desetinná část čísla

double FFrac()
{
	double num = FNum();
	return num - (int)num;
}


/////////////////////////////////////////////////////////////////////////////
// absolutní hodnota

double FAbs() { return fabs(FNum()); };


/////////////////////////////////////////////////////////////////////////////
// odmocnina

double FSqrt() { return sqrt(FNum()); };


/////////////////////////////////////////////////////////////////////////////
// faktoriál

double FFact()
{
// načtení čísla pro výpočet faktoriálu
	int citac = FIntR();

// kladná hodnota v povoleném rozsahu
	if ((DWORD)citac <= MAXFACT+1) return FactTab[citac];

// přetečení kladným směrem
	if (citac > 0) return FactTab[MAXFACT+1];

// absolutní hodnota čísla
	citac = -citac;

// přetečení záporným směrem
	if (citac > MAXFACT) return -FactTab[MAXFACT+1];

// hodnota v záporném směru (lichá čísla jsou záporná)
	if (citac & 1) return -FactTab[citac];
	return FactTab[citac];
}


/////////////////////////////////////////////////////////////////////////////
// náhodné číslo s násobkem

double FRand() { return FNum() * Rand(); };


/////////////////////////////////////////////////////////////////////////////
// náhodné číslo bez násobku

double FRand0() { return Rand(); };


/////////////////////////////////////////////////////////////////////////////
// převod stupňů na radiány

double FDegRad() { return FNum() * degrad; };


/////////////////////////////////////////////////////////////////////////////
// převod radiánů na stupně

double FRadDeg() { return FNum() * raddeg; };


/////////////////////////////////////////////////////////////////////////////
// dekadický logaritmus

double FLog() { return log10(FNum()); }


/////////////////////////////////////////////////////////////////////////////
// přirozený logaritmus

double FLn() { return log(FNum()); }


/////////////////////////////////////////////////////////////////////////////
// přirozený exponent

double FExp() { return exp(FNum()); }


/////////////////////////////////////////////////////////////////////////////
// operátor ADD (Data = počet prvků - 1)

double FAdd()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení prvního čísla
	double num = FNum();

// cyklus přes další čísla
	for (; i > 0; i--)
	{

// operace s dalším číslem
		num += FNum();
	}
	return num;
}


/////////////////////////////////////////////////////////////////////////////
// operátor SUB pro 1 prvek

double FSub1() { return -FNum(); };


/////////////////////////////////////////////////////////////////////////////
// operátor SUB (Data = počet prvků - 1)

double FSub()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení prvního čísla
	double num = FNum();

// cyklus přes další čísla
	for (; i > 0; i--)
	{

// operace s dalším číslem (tento zápis je efektivnější pro překlad)
		num = -(FNum() - num);
	}
	return num;
}


/////////////////////////////////////////////////////////////////////////////
// operátor MUL (Data = počet prvků - 1)

double FMul()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení prvního čísla
	double num = FNum();

// cyklus přes další čísla
	for (; i > 0; i--)
	{

// operace s dalším číslem
		num *= FNum();
	}
	return num;
}

/////////////////////////////////////////////////////////////////////////////
// operátor DIV pro 1 prvek

double FDiv1() { return 1/FNum(); };


/////////////////////////////////////////////////////////////////////////////
// operátor DIV (Data = počet prvků - 1)

double FDiv()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení prvního čísla
	double num = FNum();

// cyklus přes další čísla
	for (; i > 0; i--)
	{

// načtení dalšího čísla
		double num2 = FNum();

// operace s dalším číslem (v případě 0 / 0 bude výsledek 1)
		if (num == num2)
		{
			num = 1;
		}
		else
		{
			num /= num2;
		}
	}
	return num;
}

/////////////////////////////////////////////////////////////////////////////
// operace MOD

#pragma warning ( disable: 4725)	// hlášení - problémová instrukce

double NumMod(double num1, double num2)
{
	if (num2 <= 0)
	{
		if (num2 == 0) return 0;
		return -NumMod(-num1, -num2);
	}

	double num;

	if (num1 < 0)
	{
		num = num2 - NumMod(-num1, num2);
		if (num == num2) num = 0;
		return num;
	}

#ifdef _M_IX86

	_asm {
		fld			num2			// načtení dělitele -> ST0
		fld			num1			// načtení dělence -> ST0, dělitel -> ST1
X1:		fprem						// zbytek -> ST0, v ST1 zůstává dělitel
		wait						// synchronizace
		fnstsw		ax				// načtení stavového slova
		wait						// synchronizace
		sahf						// nastavení stavových příznaků
		jp			X1				// operace neúplná - pokračování
		fstp		num				// výsledek operace
		fstp		st(0)			// zrušení ST0 (dělitel)
	}

#else

		num = fmod(num1, num2);

#endif

		return num;
}

#pragma warning ( default: 4725)	// hlášení - problémová instrukce

/////////////////////////////////////////////////////////////////////////////
// operátor MOD (Data = počet prvků - 1)

double FMod()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení prvního čísla
	double num = FNum();

// cyklus přes další čísla
	for (; i > 0; i--)
	{

// operace s dalším číslem
		double num2 = FNum();			// načtení dalšího čísla
		num = NumMod(num, num2);
	}
	return num;
}

/////////////////////////////////////////////////////////////////////////////
// operátor MOCNINA (Data = počet prvků - 1)

double FMocn()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení prvního čísla
	double num = FNum();

// cyklus přes další čísla
	for (; i > 0; i--)
	{

// umocnění dalším číslem
		num = pow(num, FNum());
	}
	return num;
}


/////////////////////////////////////////////////////////////////////////////
// druhá mocnina

double FNumSqr()
{
	double cislo = FNum();
	return cislo * cislo;
}


/***************************************************************************\
*																			*
*								obsluha grafiky								*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// nastavení jasu barvy

double FColorLevel()
{
// načtení barvy, převod na základní jas barvy
	int col = FNumCol();
	if ((col == BackCol) || (col == ShadCol)) col = BlackCol;
	col = (col - ResCols) / ColLev * ColLev + ResCols;

// příprava počtu odstínů pro barvu
	int cols = ColLev + 1;				// počet odstínů pro běžné barvy
	if (col >= StdColors - 2*ColLev)	// je to bílá nebo šedá?
	{
		col = StdColors - 2*ColLev;		// korekce na bílou barvu
		cols = 2*ColLev;				// počet odstínů pro bílou barvu
	}

// načtení jasu barvy
	double level = FNum()*(cols-1);

// převod na index jasu barvy
	int jas;
	
#ifdef _M_IX86

	_asm {
		fld			level				// načtení čísla do koprocesoru
		fistp		jas					// převod čísla na celé číslo
	}

#else

	jas = Round(level);					// zaokrouhlení čísla

#endif
	
// omezení indexu jasu barvy
	if ((DWORD)jas >= (DWORD)cols)
	{
		if (jas < 0)
		{
			jas = 0;
		}
		else
		{
			jas = cols-1;
		}
	}

// pro jas 0 se použije černá barva
	if (jas == 0) return 0;

// výsledná barva
	return PalExport(col + cols - 1 - jas);
}


/////////////////////////////////////////////////////////////////////////////
// syntéza barvy

double FColorSynth()
{
	DWORD color;
	((BYTE*)&color)[0] = FInt255();		// červená
	((BYTE*)&color)[1] = FInt255();		// zelená
	((BYTE*)&color)[2] = FInt255();		// modrá
	((BYTE*)&color)[3] = (BYTE)(FInt255() ^ 0xff);// alfa
	return color;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění barvy bodu

double FGetPoint()
{
// načtení souřadnic bodu
	int x = FIntX();
	int y = FIntY();

// implicitní barva - pozadí
	int col = BackCol;

// kontrola platnosty souřadnice bodu
	if (((DWORD)x < (DWORD)Width) && ((DWORD)y < (DWORD)Height))
	{

// načtení bodu
		col = BackBuf[x + y*WidthByte];
	}

// výsledná barva
	return PalExport(col);
}

/////////////////////////////////////////////////////////////////////////////
// zjištění šířky obrázku

double FGetPicWidth()
{
	CPicture pic;
	FPicture(pic);
	double n = (double)pic.Width()/ICONWIDTH;
	return n;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění výšky obrázku

double FGetPicHeight()
{
	CPicture pic;
	FPicture(pic);
	double n = (double)pic.Height()/ICONHEIGHT;
	return n;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění šířky pole k zobrazení centrovaného textu

double FGetGraphAreaWidth()
{
	double n = (double)GraphAreaWidth/ICONWIDTH;
	return n;
}


/***************************************************************************\
*																			*
*							obsluha času									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// načtení aktuálního času

double FGetTimeCurrent()
{
// načtení aktuálního času
	SYSTEMTIME systime;
	::GetLocalTime(&systime);

// převod na souborový čas
	__int64 cas;
	::SystemTimeToFileTime(&systime, (FILETIME*)&cas);

// návrat výsledku
	return (double)cas / FILETIMEKONV;
}


/////////////////////////////////////////////////////////////////////////////
// převod na zvětový čas

double FTimeUtc()
{
// načtení času s převodem na světový čas
	__int64 cas = FNumFileTimeUTC();

// návrat výsledku
	return (double)cas / FILETIMEKONV;
}


/////////////////////////////////////////////////////////////////////////////
// převod na místní čas

double FTimeLocal()
{
// načtení času k převodu
	__int64 cas = FNumFileTime();

// převod čísla
	__int64 cas2;
	::FileTimeToLocalFileTime((FILETIME*)&cas, (FILETIME*)&cas2);

// návrat výsledku
	return (double)cas2 / FILETIMEKONV;
}


/////////////////////////////////////////////////////////////////////////////
// syntéza času

double FTimeComp()
{
// načtení složek času
	SYSTEMTIME systime;

	int i = FIntR();
	if (i < 1601) i = 1601;
	if (i > 9999) i = 9999;		// system povoluje až 30000, ale kvůli zobrazení raději jen 4 číslice
	systime.wYear = (WORD)i;

	i = FIntR();
	if (i < 1) i = 1;
	if (i > 12) i = 12;
	systime.wMonth = (WORD)i;

	i = FIntR();
	if (i < 1) i = 1;
	if (i > 31) i = 31;
	systime.wDay = (WORD)i;

	i = FIntR();
	if ((DWORD)i > (DWORD)23)
	{
		if (i < 0)
		{
			i = 0;
		}
		else
		{
			i = 23;
		}
	}
	systime.wHour = (WORD)i;

	i = FIntR();
	if ((DWORD)i > (DWORD)59)
	{
		if (i < 0)
		{
			i = 0;
		}
		else
		{
			i = 59;
		}
	}
	systime.wMinute = (WORD)i;

	i = FIntR();
	if ((DWORD)i > (DWORD)59)
	{
		if (i < 0)
		{
			i = 0;
		}
		else
		{
			i = 59;
		}
	}
	systime.wSecond = (WORD)i;

	i = FIntR();
	if ((DWORD)i > (DWORD)999)
	{
		if (i < 0)
		{
			i = 0;
		}
		else
		{
			i = 999;
		}
	}
	systime.wMilliseconds = (WORD)i;

// provedení konverze
	__int64 cas;
	if (!::SystemTimeToFileTime(&systime, (FILETIME*)&cas))
	{
		if (systime.wDay != 31)
		{
			return 0;
		}

		systime.wDay = 30;

		if (!::SystemTimeToFileTime(&systime, (FILETIME*)&cas))
		{
			systime.wDay = 29;

			if (!::SystemTimeToFileTime(&systime, (FILETIME*)&cas))
			{
				systime.wDay = 28;

				if (!::SystemTimeToFileTime(&systime, (FILETIME*)&cas))
				{
					return 0;
				}
			}
		}
	}
	return (double)cas / FILETIMEKONV;
}


/////////////////////////////////////////////////////////////////////////////
// načtení roku z času

double FGetTimeYear() { FNumSysTime(); return SystemTime.wYear; }


/////////////////////////////////////////////////////////////////////////////
// načtení měsíce z času

double FGetTimeMonth() { FNumSysTime(); return SystemTime.wMonth; }


/////////////////////////////////////////////////////////////////////////////
// načtení dne z času

double FGetTimeDay() { FNumSysTime(); return SystemTime.wDay; }


/////////////////////////////////////////////////////////////////////////////
// načtení dne v týdnu z času

double FGetTimeWeek() { FNumSysTime(); return SystemTime.wDayOfWeek; }


/////////////////////////////////////////////////////////////////////////////
// načtení hodiny z času

double FGetTimeHour() { FNumSysTime(); return SystemTime.wHour; }


/////////////////////////////////////////////////////////////////////////////
// načtení minuty z času

double FGetTimeMin() { FNumSysTime(); return SystemTime.wMinute; }


/////////////////////////////////////////////////////////////////////////////
// načtení sekundy z času

double FGetTimeSec() { FNumSysTime(); return SystemTime.wSecond; }


/////////////////////////////////////////////////////////////////////////////
// načtení milisekundy z času

double FGetTimeMSec() { FNumSysTime(); return SystemTime.wMilliseconds; }


/***************************************************************************\
*																			*
*							obsluha myši									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// načtení souřadnice X myši

double FMouseX() { return MouseX; }

/////////////////////////////////////////////////////////////////////////////
// načtení souřadnice Y myši

double FMouseY() { return MouseY; }


/***************************************************************************\
*																			*
*							obsluha dialogů									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// dialog box s návratem hodnoty

double FNumDialogBox()
{
// načtení textu nadpisu okna
	CString nadpis;
	FText(nadpis);

// načtení textu hlášení
	CString text;
	FText(text);

// číslo ikony
	int icon = FIntR();

// typ tlačítek
	int but = FIntR();

// implicitní tlačítko
	int def = FIntR();

// pro nadpis se použije první řádek textu
	int crlf = nadpis.Find(13);
	if (crlf >= 0) nadpis.Delete(crlf);

// korekce povolených hodnot
	if ((DWORD)icon > 4) icon = 0;
	if ((DWORD)but > 5) but = 0;
	if (def < 1) def = 1;

	int buts;

	switch (but)
	{
	case 0: buts = 1;
			break;

	case 2:
	case 3:	buts = 3;
			break;

	default: buts = 2;
	}

	if (def > buts) def = buts;

// vypnutí celoobrazovkového módu
//	SetFullScreen(false);

#ifndef _MINI
	D3Render(true);
#endif // _MINI

// překreslení displeje před vyvoláním dialogu
	ReDisp();

// vyvolání dialogu
	IsOverlay = true;

	int result = ::MessageBox(Win[DialogParent].Wnd, text, nadpis,
		icon * 0x10 + but + (def-1)*0x100);

	IsOverlay = false;
	if (FullScreen) RePaint(&Win[0]);

	return result;
}


/***************************************************************************\
*																			*
*							obsluha souborů									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// zjištění velikosti disku

double FGetDiskSize()
{	
// načtení informací o disku
	GetDiskSpace();

// velikost disku
	return (double)DiskSize;
}


/////////////////////////////////////////////////////////////////////////////
// zjištění volného místa disku

double FGetDiskFree()
{	
// načtení informací o disku
	GetDiskSpace();

// volné místo disku
	return (double)DiskFree;
}


/////////////////////////////////////////////////////////////////////////////
// načtení sériového čísla disku

double FGetDiskSerial()
{
// proměnná k načtení čísla
	__int64 serial = 0;

// načtení sériového čísla disku
	::GetVolumeInformation(NULL, NULL, 0, (DWORD*)&serial, NULL, NULL, NULL, 0);

// načtený údaj (při chybě nebo když číslo není zůstane = 0)
	return (double)serial;
}


/////////////////////////////////////////////////////////////////////////////
// načtení informací o vstupním souboru (false = chyba)

bool FGetFileInfo(WIN32_FIND_DATA* wfd)
{
// uzavření souborů
	FileClose();

// kontrola zadání jména
	if (FileRead.IsNotEmpty())
	{

// nalezení souboru
		HANDLE find = ::FindFirstFile(FileRead, wfd);
		if (find != INVALID_HANDLE_VALUE)
		{

// operace OK
			::FindClose(find);
			return true;
		}
	}

// chyba
	FileError = true;
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// čas vytvoření souboru

double FGetFileCreate()
{
// načtení informací o souboru
	WIN32_FIND_DATA wfd;
	if (FGetFileInfo(&wfd))
	{

// převod času na lokální čas
		__int64 cas;
		if (::FileTimeToLocalFileTime(&wfd.ftCreationTime, (FILETIME*)&cas))
		{

// návrat výsledku
			return (double)cas / FILETIMEKONV;
		}
	}

// chyba
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// čas čtení ze souboru

double FGetFileOpen()
{
// načtení informací o souboru
	WIN32_FIND_DATA wfd;
	if (FGetFileInfo(&wfd))
	{

// převod času na lokální čas
		__int64 cas;
		if (::FileTimeToLocalFileTime(&wfd.ftLastAccessTime, (FILETIME*)&cas))
		{

// návrat výsledku
			return (double)cas / FILETIMEKONV;
		}
	}

// chyba
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// čas zápisu do souboru

double FGetFileModi()
{
// načtení informací o souboru
	WIN32_FIND_DATA wfd;
	if (FGetFileInfo(&wfd))
	{

// převod času na lokální čas
		__int64 cas;
		if (::FileTimeToLocalFileTime(&wfd.ftLastWriteTime, (FILETIME*)&cas))
		{

// návrat výsledku
			return (double)cas / FILETIMEKONV;
		}
	}

// chyba
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// offset pro čtení ze souboru

double FGetFileReadOff() { return FileReadOff; };


/////////////////////////////////////////////////////////////////////////////
// offset pro zápis do souboru

double FGetFileWriteOff() { return FileWriteOff; };


/////////////////////////////////////////////////////////////////////////////
// zjištění velikosti souboru

double FGetFileSize()
{
// lokální proměnné
	int size = 0;

// vyprázdění zápisového bufferu a otevření čtecího souboru
	if (FileWriteFlush() && FileReadOpen())
	{
// zjištění velikosti souboru
		int res = ::GetFileSize(FileReadHandle, NULL);

// chyba
		if (res == -1)
		{
			FileError = true;
		}
		else
		{
			size = res;
		}
	}

// zjištěná velikost
	return size;
}


/////////////////////////////////////////////////////////////////////////////
// délka textu s pevnou délkou

double FGetFileTextNLen() { return FileTextNLen; }


/////////////////////////////////////////////////////////////////////////////
// načtení typu disku

double FFileDiskType() { return ::GetDriveType(NULL); }


/////////////////////////////////////////////////////////////////////////////
// načtení čísla ze souboru bajt se znaménkem

double FGetFileByteS() { return (char)FileReadByte(); }


/////////////////////////////////////////////////////////////////////////////
// načtení čísla ze souboru bajt bez znaménka

double FGetFileByteU() { return (BYTE)FileReadByte(); }


/////////////////////////////////////////////////////////////////////////////
// načtení čísla ze souboru slovo se znaménkem

double FGetFileWordS()
{ 
	short cislo;
	((BYTE*)&cislo)[0] = FileReadByte();
	((BYTE*)&cislo)[1] = FileReadByte();
	return cislo;
}


/////////////////////////////////////////////////////////////////////////////
// načtení čísla ze souboru slovo bez znaménka

double FGetFileWordU()
{ 
	long cislo = 0;
	((BYTE*)&cislo)[0] = FileReadByte();
	((BYTE*)&cislo)[1] = FileReadByte();
	return cislo;
}


/////////////////////////////////////////////////////////////////////////////
// načtení čísla ze souboru 3 bajty se znaménkem

double FGetFile3ByteS()
{ 
	long cislo = 0;
	((BYTE*)&cislo)[0] = FileReadByte();
	((BYTE*)&cislo)[1] = FileReadByte();
	((BYTE*)&cislo)[2] = FileReadByte();

	if (((char*)&cislo)[2] < 0)
	{
		((BYTE*)&cislo)[3] = 0xff;
	}

	return cislo;
}


/////////////////////////////////////////////////////////////////////////////
// načtení čísla ze souboru 3 bajty bez znaménka

double FGetFile3ByteU()
{ 
	long cislo = 0;
	((BYTE*)&cislo)[0] = FileReadByte();
	((BYTE*)&cislo)[1] = FileReadByte();
	((BYTE*)&cislo)[2] = FileReadByte();
	return cislo;
}


/////////////////////////////////////////////////////////////////////////////
// načtení čísla ze souboru dvojslovo se znaménkem

double FGetFileDWordS()
{ 
	long cislo;
	((BYTE*)&cislo)[0] = FileReadByte();
	((BYTE*)&cislo)[1] = FileReadByte();
	((BYTE*)&cislo)[2] = FileReadByte();
	((BYTE*)&cislo)[3] = FileReadByte();
	return cislo;
}


/////////////////////////////////////////////////////////////////////////////
// načtení čísla ze souboru dvojslovo bez znaménka

double FGetFileDWordU()
{ 
	__int64 cislo;
	((BYTE*)&cislo)[0] = FileReadByte();
	((BYTE*)&cislo)[1] = FileReadByte();
	((BYTE*)&cislo)[2] = FileReadByte();
	((BYTE*)&cislo)[3] = FileReadByte();
	((long*)&cislo)[1] = 0;
	return (double)cislo;
}


/////////////////////////////////////////////////////////////////////////////
// načtení čísla ze souboru FLOAT

double FGetFileFloat()
{
	float cislo;
	((BYTE*)&cislo)[0] = FileReadByte();
	((BYTE*)&cislo)[1] = FileReadByte();
	((BYTE*)&cislo)[2] = FileReadByte();
	((BYTE*)&cislo)[3] = FileReadByte();
	return cislo;
}


/////////////////////////////////////////////////////////////////////////////
// načtení čísla ze souboru DOUBLE

double FGetFileDouble()
{
	double cislo;
	((BYTE*)&cislo)[0] = FileReadByte();
	((BYTE*)&cislo)[1] = FileReadByte();
	((BYTE*)&cislo)[2] = FileReadByte();
	((BYTE*)&cislo)[3] = FileReadByte();
	((BYTE*)&cislo)[4] = FileReadByte();
	((BYTE*)&cislo)[5] = FileReadByte();
	((BYTE*)&cislo)[6] = FileReadByte();
	((BYTE*)&cislo)[7] = FileReadByte();
	return cislo;
}


/***************************************************************************\
*																			*
*							obsluha CD disku								*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// zjištění délky CD

double FCDGetLength()
{
// aktualizace údajů o CD
	CDAkt();

// délka CD
	return (double)CDLength / 1000;
}


/////////////////////////////////////////////////////////////////////////////
// zjištění počtu stop CD

double FCDGetTracks()
{
// aktualizace údajů o CD
	CDAkt();

// počet stop CD
	return CDTracks;
}


/////////////////////////////////////////////////////////////////////////////
// zjištění pozice CD

double FCDGetPos()
{
// aktualizace údajů o CD
	CDAkt();

// pozice CD
	return (double)CDPos / 1000;
}


/////////////////////////////////////////////////////////////////////////////
// zjištění aktivní stopy CD

double FCDGetTrack()
{
// aktualizace údajů o CD
	CDAkt();

// aktivní stopa
	return CDTrack;
}


/////////////////////////////////////////////////////////////////////////////
// zjištění délky aktivní stopy

double FCDGetTrackLength()
{
// aktualizace údajů o CD
	CDAkt();

// délka stopy CD
	return (double)CDTrackLength / 1000;
}


/////////////////////////////////////////////////////////////////////////////
// zjištění pozice aktivní stopy CD

double FCDGetTrackPos()
{
// aktualizace údajů o CD
	CDAkt();

// pozice aktivní stopy CD
	return (double)CDTrackPos / 1000;
}


/***************************************************************************\
*																			*
*								obsluha hudby								*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// ďélka přehrávané hudby

double FMusicLen() { return (double)MusicDelka / 1000; }


/////////////////////////////////////////////////////////////////////////////
// pozice hudby

double FGetMusicPos() { return (double)MusicAktPos / 1000; }


/***************************************************************************\
*																			*
*								obsluha zvuku								*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// počet bitů na vzorek

double FGetSoundBit()
{
	CSound snd;
	FSound(snd);
	return (double)snd.Bits();
}


/////////////////////////////////////////////////////////////////////////////
// vzorkovací kmitočet

double FGetSoundFreq()
{
	CSound snd;
	FSound(snd);
	return (double)snd.Samples();
}

/////////////////////////////////////////////////////////////////////////////
// délka zvuku

double FGetSoundLength()
{
	CSound snd;
	FSound(snd);

	int bytespersec = snd.Samples() * snd.Channels() * (snd.Bits()/8);
	if (bytespersec == 0)
	{
		bytespersec = snd.Samples() * snd.Channels() / 2;
		if ((snd.Format() >= 0x50) && (snd.Format() < 0x60))
		{
			WAVEFORMATEX* wfx = (WAVEFORMATEX*)(snd.DataData());
			bytespersec = wfx->nAvgBytesPerSec;
		}
	}
	if (bytespersec <= 0) bytespersec = 1;

	return (double)(snd.Size() - snd.SizeHead()) / bytespersec;
}


/////////////////////////////////////////////////////////////////////////////
// pozice zvuku

double FGetSoundPos() { return GetSoundPos(); }


/////////////////////////////////////////////////////////////////////////////
// rozlišovací kód

double FGetSoundDistinguish()
{
	return (double)(WORD)SoundID;
}


/***************************************************************************\
*																			*
*								obsluha textů								*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// délka textu

double FTextLen()
{
// délka textu
	int len;

// následující úsek ponechat v závorkách, aby se vyvolal
// destruktor textu dříve než se číslo uloží do koprocesoru:
	{
// načtení textu
		CString text;
		FText(text);

// délka textu
		len = text.Length();
	}
	return len;
}


/////////////////////////////////////////////////////////////////////////////
// převod textu na číslo

double FTextScan()
{
// adresa textu
	LPCTSTR txt;

// následující úsek ponechat v závorkách, aby se vyvolal
// destruktor textu dříve než se číslo uloží do koprocesoru:
	{
// načtení textu
		CString text;
		FText(text);
		txt = text;
	}

// převod textu na číslo
	return Double(txt);
}


/////////////////////////////////////////////////////////////////////////////
// nalezení textu

double FTextFind()
{
// hledaný text
	CString subtext;
	FText(subtext);

// prohledávaný text
	CString text;
	FText(text);

// nalezení textu
	int pos = text.Find(subtext);
	return pos;
}


/////////////////////////////////////////////////////////////////////////////
// abecední pořadí znaku

double FTextOrd()
{
// pomocný registr
	int result;

// následující úsek ponechat v závorkách, aby se vyvolal
// destruktor textu dříve než se číslo uloží do koprocesoru:
	{

// načtení textu (znaku)
		CString text;
		FText(text);

// kódu znaku (není-li žádný znak, vrátí se koncový bajt 0)
#ifdef _UNICODE
		result = (WORD)text.DataData()[0];
#else
		result = (BYTE)text.DataData()[0];
#endif
	}

// návrat výsledku
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// zjištění počtu řádků textu

double FTextLineNum()
{
// načtení textu
	CString text;
	FText(text);
	return text.LineNum();
}


/***************************************************************************\
*																			*
*								obsluha klávesnice							*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// načtení klávesy bez čekání

double FVKey()
{
// příprava výsledku operace
	int result = 0;

// test, zda byla přijata klávesa
	if (KeyBufRead != KeyBufWrite)
	{

// načtení klávesy z bufferu
		result = KeyBuf[KeyBufRead];

// zvýšení čtecí adresy
		KeyBufRead++;
		if (KeyBufRead >= KEYBUFSIZE) KeyBufRead = 0;
	}

	return result;
}


/////////////////////////////////////////////////////////////////////////////
// načtení klávesy s čekáním

double FVKeyWait()
{
// příprava výsledku operace
	int result = 0;

// čekání na vstup znaku z klávesnice (nebo na přerušení)
	while ((Break == 0) && 
			(KeyBufRead == KeyBufWrite) && 
			(!ConsoleOn))
	{
#ifndef _MINI
		D3Render(true);
#endif // _MINI
		TimeWait(1);
	}

// kontrola, zda byla přijata klávesa (zda to nebylo jen přerušení)
	if (KeyBufRead != KeyBufWrite)
	{

// načtení klávesy z bufferu
		result = KeyBuf[KeyBufRead];

// zvýšení čtecí adresy
		KeyBufRead++;
		if (KeyBufRead >= KEYBUFSIZE) KeyBufRead = 0;
	}

	return result;
}


/***************************************************************************\
*																			*
*									plocha									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// šířka plochy

double FGetMapWidth()
{
// načtení plochy
	CMap map;
	FMap(map);

// šířka plochy
	return map.Width();
}

double FGetMapWidth0() { return Map[0].Width(); }


/////////////////////////////////////////////////////////////////////////////
// výška plochy

double FGetMapHeight()
{
// načtení plochy
	CMap map;
	FMap(map);

// výška plochy
	return map.Height();
}

double FGetMapHeight0() { return Map[0].Height(); }


/////////////////////////////////////////////////////////////////////////////
// načtení čísla z plochy

double _fastcall FGetMapNum(int mask, char rot)
{
// načtení plochy
	CMap map;
	FMap(map);

// načtení souřadnic
	int x = FIntR();
	int y = FIntR();

// číslo na pozici
	if (((DWORD)x < (DWORD)map.Width()) && ((DWORD)y < (DWORD)map.Height()))
	{
		return (((int)map.At(x, y).Param & mask) >> rot);
	}
	return 0;
}

double FGetMapNum1() { return FGetMapNum(MAP_VALUE1_MASK, MAP_VALUE1_ROT); };
double FGetMapNum2() { return FGetMapNum(MAP_VALUE2_MASK, MAP_VALUE2_ROT); };
double FGetMapNum3() { return FGetMapNum(MAP_VALUE3_MASK, MAP_VALUE3_ROT); };


/////////////////////////////////////////////////////////////////////////////
// načtení okna k zobrazení sprajtů

double FGetSpriteWinX1() { return (double)SpriteWinX1 / ICONWIDTH; }
double FGetSpriteWinY1() { return (double)SpriteWinY1 / ICONHEIGHT; }
double FGetSpriteWinX2() { return (double)SpriteWinX2 / ICONWIDTH; }
double FGetSpriteWinY2() { return (double)SpriteWinY2 / ICONHEIGHT; }


/////////////////////////////////////////////////////////////////////////////
// návratový kód

double FGetReturnCode() { return ReturnCode; }


/////////////////////////////////////////////////////////////////////////////
// spuštění programu

double FGetExecWait()
{
	CString text;
	FText(text);
	DWORD result = Exec(text, TRUE);
	SetConsoleText();
	return result;
}

/***************************************************************************\
*																			*
*									joystick								*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// pozice X

double FJoystickX()
{
	int inx = FIntR();
	if ((DWORD)inx >= (DWORD)MAXJOYSTICK) return 0;
	AktJoystick(inx);
	return JoystickX[inx];
}


/////////////////////////////////////////////////////////////////////////////
// pozice Y

double FJoystickY()
{
	int inx = FIntR();
	if ((DWORD)inx >= (DWORD)MAXJOYSTICK) return 0;
	AktJoystick(inx);
	return JoystickY[inx];
}


/////////////////////////////////////////////////////////////////////////////
// pozice Z

double FJoystickZ()
{
	int inx = FIntR();
	if ((DWORD)inx >= (DWORD)MAXJOYSTICK) return 0;
	AktJoystick(inx);
	return JoystickZ[inx];
}


/***************************************************************************\
*																			*
*									dialogy									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// ID aktivního prvku

double FGetWindowID() { return WindowID; }


/////////////////////////////////////////////////////////////////////////////
// vytvoření okna

double FWindowNew()
{
	WindowNew(WINITEM_WINDOW, WS_SYSMENU); 
	return WindowID;
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření okna

double FWindowRONew()
{
	WindowNew(WINITEM_WINDOWRO, 0); 
	return WindowID;
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření tlačítka

double FButtonNew()
{
	WindowItemNew(WINITEM_BUTTON, ButtonClassName, 10*8, 24, BS_PUSHBUTTON | BS_NOTIFY | WS_GROUP | WS_TABSTOP, 0);
	return WindowID;
}

double FGetButtonEsc()
{
	return ButtonEsc;
}

double FDefButtonNew()
{
	WindowItemNew(WINITEM_DEFBUTTON, ButtonClassName, 10*8, 24, BS_DEFPUSHBUTTON | BS_NOTIFY | WS_GROUP | WS_TABSTOP, 0);
	return WindowID;
}

double FButtonIconNew()
{
	WindowItemNew(WINITEM_BUTTONICON, ButtonClassName, 40, 40, BS_3STATE | BS_NOTIFY | BS_PUSHLIKE | WS_GROUP | WS_TABSTOP | BS_ICON, 0);
	return WindowID;
}

double FButtonCheckNew()
{
	WindowItemNew(WINITEM_BUTTONCHECK, ButtonClassName, 16*8, 16, BS_AUTOCHECKBOX | BS_NOTIFY | WS_GROUP | WS_TABSTOP, 0);
	return WindowID;
}

double FButtonCheckNew2()
{
	WindowItemNew(WINITEM_BUTTONCHECK2, ButtonClassName, 10*8, 24, BS_3STATE | BS_NOTIFY | BS_PUSHLIKE | WS_GROUP | WS_TABSTOP, 0);
	return WindowID;
}

double FButton3New()
{
	WindowItemNew(WINITEM_BUTTON3, ButtonClassName, 16*8, 16, BS_AUTO3STATE | BS_NOTIFY | WS_GROUP | WS_TABSTOP, 0);
	return WindowID;
}

double FButtonRadioNew()
{
	WindowItemNew(WINITEM_BUTTONRADIO, ButtonClassName, 16*8, 16, BS_AUTORADIOBUTTON | BS_NOTIFY, 0);
	return WindowID;
}

double FButtonRadioNew2()
{
	WindowItemNew(WINITEM_BUTTONRADIO2, ButtonClassName, 16*8, 16, BS_AUTORADIOBUTTON | BS_NOTIFY | WS_GROUP | WS_TABSTOP, 0);
	return WindowID;
}

double FGroupBoxNew()
{
	WindowItemNew(WINITEM_GROUP, ButtonClassName, 24*8, 12*8, BS_GROUPBOX, WS_EX_TRANSPARENT);
	return WindowID;
}

/////////////////////////////////////////////////////////////////////////////
// indikátor

CString	ProgressClass(PROGRESS_CLASS);

double FProgressNew()
{
	::InitCommonControls();

	WindowItemNew(WINITEM_PROGRESS, ProgressClass, 202, 16, 0, 0);
	return WindowID;
}

/////////////////////////////////////////////////////////////////////////////
// táhlo

CString	TrackbarClass(TRACKBAR_CLASS);

double FHTrackbarNew()
{
	::InitCommonControls();

	WindowItemNew(WINITEM_HTRACKBAR, TrackbarClass, 16*8, 34, TBS_HORZ | TBS_TOP | TBS_AUTOTICKS | WS_GROUP | WS_TABSTOP, 0);
	return WindowID;
}

double FVTrackbarNew()
{
	::InitCommonControls();

	WindowItemNew(WINITEM_VTRACKBAR, TrackbarClass, 42, 16*8, TBS_VERT | TBS_BOTH | TBS_AUTOTICKS | WS_GROUP | WS_TABSTOP, 0);
	return WindowID;
}

/////////////////////////////////////////////////////////////////////////////
// posuvník

CString	ScrollbarClass("SCROLLBAR");

double FHScrollbarNew()
{
	WindowItemNew(WINITEM_HSCROLLBAR, ScrollbarClass, 32*8, 18, SBS_HORZ | WS_GROUP | WS_TABSTOP, 0);
	return WindowID;
}

double FVScrollbarNew()
{
	WindowItemNew(WINITEM_VSCROLLBAR, ScrollbarClass, 18, 32*8, SBS_VERT | WS_GROUP | WS_TABSTOP, 0);
	return WindowID;
}

/////////////////////////////////////////////////////////////////////////////
// pozice v prvku

double FGetWindowPos()
{
	if (Win.IsValid(WindowID))
	{
		WINITEM* item = &Win[WindowID];

		switch (item->Typ)
		{
		case WINITEM_HTRACKBAR:
			return ((double)::SendMessage(item->Wnd, TBM_GETPOS, 0, 0) / 100);

		case WINITEM_VTRACKBAR:
			return ((double)(100 - ::SendMessage(item->Wnd, TBM_GETPOS, 0, 0)) / 100);

		case WINITEM_PROGRESS:
			return ((double)::SendMessage(item->Wnd, PBM_GETPOS, 0, 0) / 100);

		case WINITEM_LISTBOX:
			// není-li nic označeno, vrací LB_ERR = -1
			return (double)(int)::SendMessage(item->Wnd, LB_GETCURSEL, 0, 0);

		case WINITEM_TABS:
			// není-li nic označeno, vrací -1
			return (double)(int)::SendMessage(item->Wnd, TCM_GETCURSEL, 0, 0);

		case WINITEM_LISTEDIT:
		case WINITEM_COMBOBOX:
		case WINITEM_COMBOEDIT:
			// není-li nic označeno, vrací CB_ERR = -1
			return (double)(int)::SendMessage(item->Wnd, CB_GETCURSEL, 0, 0);

		case WINITEM_EDIT:
			{
				DWORD pos = 0;
				::SendMessage(item->Wnd, EM_GETSEL, (LPARAM)&pos, NULL);
				return (double)pos;
			}

		case WINITEM_MEMO:
			{
				int sel;
				::SendMessage(item->Wnd, EM_GETSEL, (WPARAM)&sel, 0);
				int radek = ::SendMessage(item->Wnd, EM_LINEFROMCHAR, sel, 0);
				return (double)radek;
			}

		case WINITEM_RICHEDIT:
		case WINITEM_RICHMEMO:
			{
				CHARRANGE sel;
				::SendMessage(item->Wnd, EM_GETSEL, 0, (LPARAM)&sel);
				int radek = ::SendMessage(item->Wnd, EM_LINEFROMCHAR, sel.cpMin, 0);
				return (double)radek;
			}

		case WINITEM_TABLE:
			return (double)item->TabRowF;

		case WINITEM_HSCROLLBAR:
		case WINITEM_VSCROLLBAR:
			return item->ScrollPos;
		}
		return 0;
	}
	else
	{
		return 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// pozice 2 v prvku

double FGetWindowPos2()
{
	if (Win.IsValid(WindowID))
	{
		WINITEM* item = &Win[WindowID];

		switch (item->Typ)
		{
		case WINITEM_MEMO:
			{
				int sel;
				::SendMessage(item->Wnd, EM_GETSEL, (WPARAM)&sel, 0);
				int radek = ::SendMessage(item->Wnd, EM_LINEFROMCHAR, sel, 0);
				int pozice = sel - ::SendMessage(item->Wnd, EM_LINEINDEX, radek, 0);
				return (double)pozice;
			}

		case WINITEM_RICHEDIT:
		case WINITEM_RICHMEMO:
			{
				CHARRANGE sel;
				::SendMessage(item->Wnd, EM_EXGETSEL, 0, (LPARAM)&sel);
				int radek = ::SendMessage(item->Wnd, EM_LINEFROMCHAR, sel.cpMin, 0);
				int pozice = sel.cpMin - ::SendMessage(item->Wnd, EM_LINEINDEX, radek, 0);
				return (double)pozice;
			}

		case WINITEM_TABLE:
			return (double)item->TabCol;

		case WINITEM_HSCROLLBAR:
		case WINITEM_VSCROLLBAR:
			return item->ScrollPage;
		}
		return 0;
	}
	else
	{
		return 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// editační pole

double FEditNew()
{
	WindowItemNew(WINITEM_EDIT, EditClassName, 16*8, 21, ES_AUTOHSCROLL | WS_GROUP | WS_TABSTOP, WS_EX_CLIENTEDGE);
	return WindowID;
}

double FMemoNew()
{
	WindowItemNew(WINITEM_MEMO, EditClassName, 20*8, 73, ES_AUTOVSCROLL | ES_MULTILINE | WS_GROUP | WS_TABSTOP
			| ES_WANTRETURN | WS_VSCROLL, WS_EX_CLIENTEDGE);
	return WindowID;
}


HINSTANCE RichEditLib = NULL;

double RichNew(int typ, int par)
{
// !!!!!!!!!!!!!!!!! Novější verze RichEdit při načtení textu vymaže UNDO buffer
// !!!!!!!!!!!!!!!!! u novější verze RichEdit nefunguje hledání textu
	if (RichEditLib == NULL)
	{
//		RichEditLib = ::LoadLibrary(_T("RICHED20.DLL"));
//		if (RichEditLib != NULL)
//		{
//			RichEditClassName = RICHEDIT_CLASS;
//		}
//		else
		{
			RichEditLib = ::LoadLibrary(_T("RICHED32.DLL"));
		}
	}		
	WindowItemNew(typ, RichEditClassName, 20*8, 73, par, WS_EX_CLIENTEDGE);

	return WindowID;
}

double FRicheditNew()
{
	return RichNew(WINITEM_RICHEDIT, ES_AUTOVSCROLL | ES_MULTILINE | WS_GROUP | WS_TABSTOP
			| ES_WANTRETURN | WS_VSCROLL);
}


double FRichMemoNew()
{
	return RichNew(WINITEM_RICHMEMO, ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_MULTILINE | WS_GROUP | WS_TABSTOP
			| ES_WANTRETURN | WS_VSCROLL | WS_HSCROLL);
}

/////////////////////////////////////////////////////////////////////////////
// seznam

double FListBoxNew()
{
	WindowItemNew(WINITEM_LISTBOX, ListBoxClassName, 16*8, 24*8, LBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_GROUP | WS_TABSTOP
			| LBS_USETABSTOPS | LBS_NOTIFY, WS_EX_CLIENTEDGE);
	return WindowID;
}

double FListEditNew()
{
	WindowItemNew(WINITEM_LISTEDIT, ComboBoxClassName, 16*8, 24*8, CBS_AUTOHSCROLL | CBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_GROUP | WS_TABSTOP
			| CBS_SIMPLE, 0);
	return WindowID;
}

double FComboBoxNew()
{
	WindowItemNew(WINITEM_COMBOBOX, ComboBoxClassName, 16*8, 24*8, CBS_AUTOHSCROLL | CBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_GROUP | WS_TABSTOP
			| CBS_DROPDOWNLIST, 0);
	return WindowID;
}

double FComboEditNew()
{
	WindowItemNew(WINITEM_COMBOEDIT, ComboBoxClassName, 16*8, 24*8, CBS_AUTOHSCROLL | CBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_GROUP | WS_TABSTOP
			| CBS_DROPDOWN, 0);
	return WindowID;
}

double FTabsNew()
{
	::InitCommonControls();

	WindowItemNew(WINITEM_TABS, TabsClassName, 32*8, 24*8, WS_CLIPSIBLINGS | WS_GROUP | WS_TABSTOP, 0);
	return WindowID;
}

/////////////////////////////////////////////////////////////////////////////
// tabulka

CString	ListviewClass(WC_LISTVIEW);

double FTableNew()
{
// načtení seznamu titulků
	CString text;
	FText(text);

	int cols = text.LineNum();
	if (cols <= 0) cols = 1;

// příprava jednotlivých titulků
	CBufText coltit;
	bool head = false;
	for (int i = 0; i < cols; i++)
	{
		coltit.New();
		coltit[i] = text.GetLine(i);
		if (coltit[i].IsNotEmpty()) head = true;
	}

// vytvoření tabulky
	::InitCommonControls();

	WindowItemNew(WINITEM_TABLE, ListviewClass, 32*8, 128, LVS_REPORT | LVS_SINGLESEL | LVS_OWNERDRAWFIXED | LVS_SHOWSELALWAYS
		| WS_GROUP | WS_TABSTOP | (head ? LVS_NOSORTHEADER : LVS_NOCOLUMNHEADER), WS_EX_STATICEDGE);

	WINITEM* item = &Win[WindowID];
	item->HasCaption = true;

// vytvoření sloupečků
	HWND wnd = item->Wnd;
	item->TabAlign = (BYTE*)MemGet(cols*sizeof(BYTE));

	BYTE fm;

	for (i = 0; i < cols; i++)
	{
		item->TabCols++;

		LVCOLUMN col;
		MemFill(&col, sizeof(col), 0);
		col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		fm = (BYTE)((i == 0) ? LVCFMT_LEFT : LVCFMT_CENTER);
		col.fmt = fm;
		item->TabAlign[i] = 2;
		col.cx = (16*8);
		col.pszText = (LPTSTR)(LPCTSTR)coltit[i];
		col.iSubItem = i;

		::SendMessage(wnd, LVM_INSERTCOLUMN, i, (LPARAM)&col);
	}

// vytvoření prvního řádku
	i = 32;
	while (i < cols) i *= 2;

	item->TabData = (CString*)MemGet(i*sizeof(CString));
	item->TabDataMax = i;

	for (i = 0; i < cols; i++)
	{
		item->TabData[i].Init();
	}

	item->TabRows = 1;

	LVITEM lvi;
	MemFill(&lvi, sizeof(lvi), 0);
	lvi.mask = LVIF_TEXT | LVIF_NORECOMPUTE;
	lvi.pszText = LPSTR_TEXTCALLBACK;

	::SendMessage(wnd, LVM_INSERTITEM, 0, (LPARAM)&lvi);

	return WindowID;
}

double FGetTableWidth()
{
	if (Win.IsValid(WindowID))
	{
		WINITEM* item = &Win[WindowID];
		int col = item->TabCol;

		if ((DWORD)col < (DWORD)item->TabCols)
		{
			if (item->Typ == WINITEM_TABLE)
			{
				LVCOLUMN lvc;
				lvc.mask =  LVCF_WIDTH;
				lvc.cx = 0;
				lvc.iSubItem = col;
				::SendMessage(item->Wnd, LVM_GETCOLUMN, col, (LPARAM)&lvc);

				return ((double)lvc.cx / ICONWIDTH);
			}
		}
	}

	return 0;
}

double FGetTableAlign()
{
	if (Win.IsValid(WindowID))
	{
		WINITEM* item = &Win[WindowID];
		int col = item->TabCol;

		if ((DWORD)col < (DWORD)item->TabCols)
		{
			if (item->Typ == WINITEM_TABLE)
			{
				return item->TabAlign[col];
			}
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// text

double FWinTextNew()
{
	WindowItemNew(WINITEM_WINTEXT, StaticClassName, 16*8, 16, SS_LEFT | SS_NOTIFY | SS_NOPREFIX, 0);
	return WindowID;
}

double FWinTextNewCenter()
{
	WindowItemNew(WINITEM_WINTEXT, StaticClassName, 16*8, 16, SS_CENTER | SS_NOTIFY | SS_NOPREFIX, 0);
	return WindowID;
}

double FWinTextNewRight()
{
	WindowItemNew(WINITEM_WINTEXT, StaticClassName, 16*8, 16, SS_RIGHT | SS_NOTIFY | SS_NOPREFIX, 0);
	return WindowID;
}

double FWinIconNew()
{
	WindowItemNew(WINITEM_WINICON, StaticClassName, 32, 32, SS_CENTERIMAGE | SS_ICON | SS_NOTIFY | SS_REALSIZEIMAGE, 0);
	return WindowID;
}

double FWinPicNew()
{
	WindowItemNew(WINITEM_WINPIC, StaticClassName, 32, 32, SS_NOTIFY | SS_OWNERDRAW, 0);
	return WindowID;
}


/////////////////////////////////////////////////////////////////////////////
// rozměry okna

double FGetWindowX()
{
	if (Win.IsValid(WindowID))
	{
		return (double)Win[WindowID].Left/ICONWIDTH;
	}
	else
	{
		return 0;
	}
}

double FGetWindowY()
{
	if (Win.IsValid(WindowID))
	{
		WINITEM* item = &Win[WindowID];
		int height;
		if ((item->Typ == WINITEM_WINDOW) || (item->Typ == WINITEM_WINDOWRO))
		{
			height = ScreenHeight0;
		}
		else
		{
			height = Win[item->Parent].ClientHeight;
		}
		return (double)(height - item->Top - item->Height)/ICONHEIGHT;
	}
	else
	{
		return 0;
	}
}


double FGetWindowW()
{
	if (Win.IsValid(WindowID))
	{
		return (double)Win[WindowID].Width/ICONWIDTH;
	}
	else
	{
		return (double)ScreenWidth0/ICONWIDTH;
	}
}

double FGetWindowWInt()
{
	if (Win.IsValid(WindowID))
	{
		return (double)Win[WindowID].ClientWidth/ICONWIDTH;
	}
	else
	{
		return (double)ScreenWidth/ICONWIDTH;
	}
}

double FGetWindowH()
{
	if (Win.IsValid(WindowID))
	{
		return (double)Win[WindowID].Height/ICONHEIGHT;
	}
	else
	{
		return (double)ScreenHeight0/ICONHEIGHT;
	}
}

double FGetWindowHInt()
{
	if (Win.IsValid(WindowID))
	{
		return (double)Win[WindowID].ClientHeight/ICONHEIGHT;
	}
	else
	{
		return (double)ScreenHeight/ICONHEIGHT;
	}
}


/////////////////////////////////////////////////////////////////////////////
// výška fontů

double FGetFontHeight()
{
	if (Win.IsValid(WindowID))
	{
		return Win[WindowID].FontHeight;
	}
	else
	{
		return 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// šířka fontů

double FGetFontWidth()
{
	if (Win.IsValid(WindowID))
	{
		return Win[WindowID].FontWidth;
	}
	else
	{
		return 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// úhel textu

double FGetFontAngle()
{
	if (Win.IsValid(WindowID))
	{
		return Win[WindowID].Angle;
	}
	else
	{
		return 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// barva textu

double FGetFontCol()
{
	if (Win.IsValid(WindowID))
	{
		return Win[WindowID].FontCol;
	}
	else
	{
		return StdColorBtnText;
	}
}

/////////////////////////////////////////////////////////////////////////////
// barva pozadí

double FGetFontBack()
{
	if (Win.IsValid(WindowID))
	{
		return Win[WindowID].FontBack;
	}
	else
	{
		return StdColorBtnFace;
	}
}

/////////////////////////////////////////////////////////////////////////////
// okno s fokusem

double FGetFocus() { return WindowFocus; };


/***************************************************************************\
*																			*
*									DirectPlay								*
*																			*
\***************************************************************************/

// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI

/////////////////////////////////////////////////////////////////////////////
// zjištění číselné hodnoty hry 1

double FGetGameN1()
{
	int n = (int)(((WORD*)DirectPlayGamesParam)[0] & 0x7fff) - 0x4000;
	return n;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění číselné hodnoty hry 2

double FGetGameN2()
{
	int n = (int)(((WORD*)DirectPlayGamesParam)[1] & 0x7fff) - 0x4000;
	return n;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění číselné hodnoty hry 3

double FGetGameN3()
{
	int n = (int)(((WORD*)DirectPlayGamesParam)[2] & 0x7fff) - 0x4000;
	return n;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění číselné hodnoty hry 4

double FGetGameN4()
{
	int n = (int)(((WORD*)DirectPlayGamesParam)[3] & 0x7fff) - 0x4000;
	return n;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění číselné hodnoty hry 5

double FGetGameN5()
{
	int n = (int)(((WORD*)DirectPlayGamesParam)[4] & 0x7fff) - 0x4000;
	return n;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění číselné hodnoty hry 6

double FGetGameN6()
{
	int n = (int)(((WORD*)DirectPlayGamesParam)[5] & 0x7fff) - 0x4000;
	return n;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění číselné hodnoty hry 7

double FGetGameN7()
{
	int n = (int)(((WORD*)DirectPlayGamesParam)[6] & 0x7fff) - 0x4000;
	return n;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění číselné hodnoty hry 8

double FGetGameN8()
{
	int n = (int)(((WORD*)DirectPlayGamesParam)[7] & 0x7fff) - 0x4000;
	return n;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění max. počtu hráčů

double FGetPlayerMax()
{
	return DirectPlayPlayersMax;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění indexu hráče

double FPlayerInx()
{
	return DirectPlayPlayersAktN;
}

/////////////////////////////////////////////////////////////////////////////
// příjem paketu

double FGetGamePacket()
{
	return DirectReceive();
}

/////////////////////////////////////////////////////////////////////////////
// načtení čísla z paketu

double FGetGameDataN()
{
	double num;
	DirectReceiveData((BYTE*)&num, 8);
	return num;
}

/////////////////////////////////////////////////////////////////////////////
// načtení bajtu z paketu

double FGetGameDataB()
{
	BYTE num;
	DirectReceiveData((BYTE*)&num, 1);
	return (double)(int)(BYTE)num;
}

/***************************************************************************\
*																			*
*										UDP									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// zjištění vysílacího portu UDP

double FGetUDPSendPort()
{
	int n = UDPSendAddr.sin_port;
	return ((n & 0xff) << 8) | (n >> 8);
}

/////////////////////////////////////////////////////////////////////////////
// zjištění velikosti přijímacího bufferu UDP

double FGetUDPRecSize()
{
	return UDPRecBuf.len;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění přijímacího portu UDP

double FGetUDPRecPort()
{
	int n = UDPRecAddr.sin_port;
	return ((n & 0xff) << 8) | (n >> 8);
}

/////////////////////////////////////////////////////////////////////////////
// příjem datového bloku UDP

double FGetUDPMem()
{
	int n = RecUDP();
	MemCopy(DLLMemoryWrite, UDPRecBuf.buf, n);
	DLLMemoryWrite = (BYTE*)DLLMemoryWrite + n;
#ifdef UDP_OVER
	StartRecUDP();
#endif
	return n;
}

#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------

/***************************************************************************\
*																			*
*							Bitové operace									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// operátor BIT-AND (Data = počet prvků - 1)

double FBitAnd()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení prvního čísla
	DWORD num = FIntRN();

// cyklus přes další čísla
	for (; i > 0; i--)
	{

// operace s dalším číslem
		num &= FIntRN();
	}
	return num;
}

/////////////////////////////////////////////////////////////////////////////
// operátor BIT-OR (Data = počet prvků - 1)

double FBitOr()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení prvního čísla
	DWORD num = FIntRN();

// cyklus přes další čísla
	for (; i > 0; i--)
	{

// operace s dalším číslem
		num |= FIntRN();
	}
	return num;
}

/////////////////////////////////////////////////////////////////////////////
// operátor BIT-XOR (Data = počet prvků - 1)

double FBitXor()
{
// příprava počtu prvků
	int i = ExecItem[-1].Data;

// načtení prvního čísla
	DWORD num = FIntRN();

// cyklus přes další čísla
	for (; i > 0; i--)
	{

// operace s dalším číslem
		num ^= FIntRN();
	}
	return num;
}

/////////////////////////////////////////////////////////////////////////////
// bitový doplněk

double FBitNot()
{ 
// načtení čísla
	DWORD num = FIntRN();
	num = ~num;
	return num;
};

/////////////////////////////////////////////////////////////////////////////
// rotace vlevo

double FBitLeft()
{ 
// načtení čísla
	DWORD num = FIntRN();

	num = (num << 1) | (num >> 31);

	return num;
};


/////////////////////////////////////////////////////////////////////////////
// rotace vpravo

double FBitRight()
{ 
// načtení čísla
	DWORD num = FIntRN();

	num = (num >> 1) | (num << 31);

	return num;
};



/////////////////////////////////////////////////////////////////////////////
// zjištění znakové sady

double FGetCodePage() { return CodePage; }


/////////////////////////////////////////////////////////////////////////////
// zjištění jazyku

double FGetLanguage() { return LangID; }


/***************************************************************************\
*																			*
*									Porty									*
*																			*
\***************************************************************************/

// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI

/////////////////////////////////////////////////////////////////////////////
// otevření portu

double FComOpen()
{
// načtení textu
	CString text;
	FText(text);

// korekce textu
	int id;
	text.UpperCase();
	text.TrimLeft();
	text.TrimRight();
	while ((id = text.Find(_T(' '))) >= 0)
	{
		text.Delete(id, 1);
	}

// nalezení volné položky
	for (id = 0; id < COMSMAX; id++)
	{
		if (Coms[id].File == INVALID_HANDLE_VALUE) break;
	}
	if (id == COMSMAX) return -1;

	COMITEM* item = Coms + id;

// otevření zařízení
	HANDLE file = ::CreateFile(
		text,									// jméno zařízení
		GENERIC_READ | GENERIC_WRITE,			// požadovaný přístup
		0,										// sdílení není povoleno
		NULL,									// nelze sdílet s podprocesy
		OPEN_EXISTING,							// zařízení musí existovat
		FILE_ATTRIBUTE_NORMAL,					// atributy normální
		NULL);									// není šablona
	if (file == INVALID_HANDLE_VALUE) return -1;
	item->File = file;

// inicializace bufferů (pro přijímací buffer > 1024 dělá chyby v datech nad 1024 !!!!)
	::SetupComm(file, 1010, 64);

// hlášení událostí se nebude používat
	::SetCommMask(file, 0);

// vyprázdnění bufferů
	::PurgeComm(file, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

// nastavení time-out
	COMMTIMEOUTS to;
	to.ReadIntervalTimeout = 100;
	to.ReadTotalTimeoutMultiplier = 100;
	to.ReadTotalTimeoutConstant = 0;
	to.WriteTotalTimeoutMultiplier = 100;
	to.WriteTotalTimeoutConstant = 0;
	::SetCommTimeouts(file, &to);

// nastavení jako aktivní port
	ComAkt = id;

// inicializace parametrů portu
	GetComDCB();

	DCB olddcb;
	DCB* dcb = &item->Dcb;

	MemCopy(&olddcb, dcb, sizeof(DCB));

	dcb->fBinary = TRUE;
	dcb->fOutxCtsFlow = FALSE;
	dcb->fOutxDsrFlow = FALSE;
	dcb->fDtrControl = DTR_CONTROL_DISABLE;
	dcb->fDsrSensitivity = FALSE;
	dcb->fOutX = FALSE;
	dcb->fInX = FALSE;
	dcb->fNull = FALSE;
	dcb->fRtsControl = RTS_CONTROL_DISABLE;
	dcb->fAbortOnError = FALSE;
	dcb->ErrorChar = (char)(BYTE)0xff;
	dcb->fErrorChar = TRUE;
	dcb->EofChar = 0;

// aktualizace parametrů, pokud byla změna
	if (!MemCompare(&olddcb, dcb, sizeof(DCB)))
	{
		SetComDCB();
	}

	return id;
}

/////////////////////////////////////////////////////////////////////////////
// aktivní port

double FGetComID()
{
	return ComAkt;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění přenosové rychlosti portu

double FGetComBaud()
{
	COMITEM* item = Coms + ComAkt;
	if (item->File != INVALID_HANDLE_VALUE)
	{
		return item->Dcb.BaudRate;
	}
	return 1200;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění počtu bitů portu

double FGetComBits()
{
	COMITEM* item = Coms + ComAkt;
	if (item->File != INVALID_HANDLE_VALUE)
	{
		return item->Dcb.ByteSize;
	}
	return 8;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění parity

double FGetComParit()
{
	COMITEM* item = Coms + ComAkt;
	if (item->File != INVALID_HANDLE_VALUE)
	{
		DCB* dcb = &item->Dcb;

		if (!dcb->fParity) return 0;
		return dcb->Parity;
	}
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// načtení bajtu

double FGetComNum()
{
	return (double)ComReceive();
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
// zjištění typu signálu

double FGetLineType()
{
// inicializace mixeru
	if (!MixDevInit() || ((DWORD)MixLineA >= (DWORD)MixLineN)) return -1;

// zjištění typu signálu
	return MixLine[MixLineA].Type;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění typu ovládacího prvku

double FGetCtrlType()
{
// inicializace mixeru
	if (!MixDevInit() || ((DWORD)MixCtrlA >= (DWORD)MixCtrlN)) return -1;

// zjištění typu ovládacího prvku
	return MixCtrl[MixCtrlA].Type;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění hodnoty ovládacího prvku

double FGetCtrlVal()
{
	return GetMixCtrlVal();
}

/////////////////////////////////////////////////////////////////////////////
// zjištění počtu kanálů prvku

double FGetMixChannel()
{
// inicializace mixeru
	if (!MixDevInit() || ((DWORD)MixCtrlA >= (DWORD)MixCtrlN)) return 0;

// zjištění počtu kanálů
	return MixCtrl[MixCtrlA].Channels;
}

#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------

/***************************************************************************\
*																			*
*								obsluha Direct3D							*
*																			*
\***************************************************************************/

// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI

/////////////////////////////////////////////////////////////////////////////
// zjištění módu odstraňováni ploch objektu Direct3D

double FGetD3DOCulling()
{
	if (IsValidID())
	{
		return GetID()->Culling;
	}
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření zdi (je voláno i pro vytvoření 2D objektu)

// tabulka se používá i při zobrazení textury pozadí scény v OpenGL
D3DVECTOR wall_vertices[4] = {
	-0.5,  0.5, 0,			// 0      0-----1
     0.5,  0.5, 0,			// 1	  |	  /	|
    -0.5, -0.5, 0,			// 2	  |	/	|
     0.5, -0.5, 0			// 3	  2-----3
};

int wall_faces[6] = {
	0, 1, 2,
	2, 1, 3
};

D3DVECTOR wall_normals[4] = {
	0, 0, -1,
	0, 0, -1,
	0, 0, -1,
	0, 0, -1
};

// tabulka se používá i při zobrazení textury pozadí scény v OpenGL
float wall_text[2*4] = {
	0, 0,	// 0
	1, 0,	// 1
	0, 1,	// 2
	1, 1	// 3
};

double FD3DWall()
{
	int inx = D3DF_CreateMesh(4, wall_vertices, 2, wall_faces);
	D3DF_SetNormal(inx, wall_normals);
	D3DF_SetTextUV(inx, wall_text, 0);
	D3D_ID = inx;
	D3DF_SetAddress(inx, D3DTADDRESS_WRAP, D3DTADDRESS_WRAP, 0);
	D3DF_KorRightHand();

	return D3D_ID;
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření statického 2D objektu

D3DVECTOR static2D_vertices[16] = {
// 1. stěna
	-0.5,  0.5,    0,		// 0
     0.5,  0.5,    0,		// 1
    -0.5, -0.5,    0,		// 2
     0.5, -0.5,    0,		// 3

// 2. stěna
	   0,  0.5, -0.5,		// 4
       0,  0.5,  0.5,		// 5
       0, -0.5, -0.5,		// 6
       0, -0.5,  0.5,		// 7

// 3. stěna
	 0.5,  0.5,    0,		// 8
    -0.5,  0.5,    0,		// 9
     0.5, -0.5,    0,		// 10
    -0.5, -0.5,    0,		// 11

// 4. stěna
	   0,  0.5,  0.5,		// 12
       0,  0.5, -0.5,		// 13
       0, -0.5,  0.5,		// 14
       0, -0.5, -0.5		// 15
};

int static2D_faces[24] = {
// 1. stěna
	0, 1, 2,
	2, 1, 3,

// 2. stěna
	4, 5, 6,
	6, 5, 7,

// 3. stěna
	8, 9, 10,
	10, 9, 11,

// 4. stěna
    12, 13, 14,
	14, 13, 15
};

D3DVECTOR static2D_normals[16] = {
// 1. stěna
	0, 0, -1,
	0, 0, -1,
	0, 0, -1,
	0, 0, -1,

// 2. stěna
	1, 0,  0,
	1, 0,  0,
	1, 0,  0,
	1, 0,  0,

// 3. stěna
	0, 0,  1,
	0, 0,  1,
	0, 0,  1,
	0, 0,  1,

// 4. stěna
	-1, 0, 0,
	-1, 0, 0,
	-1, 0, 0,
	-1, 0, 0
};

float static2D_text[2*16] = {
// 1. stěna
	0, 0,
	1, 0,
	0, 1,
	1, 1,

// 2. stěna
	0, 0,
	1, 0,
	0, 1,
	1, 1,

// 3. stěna
	0, 0,
	1, 0,
	0, 1,
	1, 1,

// 4. stěna
	0, 0,
	1, 0,
	0, 1,
	1, 1,
};

double FD3D2DStatic()
{
	CPicture pic;
	FPicture(pic);

	int inx = D3DF_CreateMesh(16, static2D_vertices, 8, static2D_faces);
	D3DF_SetNormal(inx, static2D_normals);
	D3DF_SetTextUV(inx, static2D_text, 0);
	D3D_ID = inx;
	D3DF_SetAddress(inx, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP, 0);
	D3DF_KorRightHand();

	D3DFITEM* item = GetID();
	item->MatSource = MATSOURCE_FRAME;
//	item->Lighton = false;

	if (pic.IsNotEmpty())
	{
		item->Texture[0] = D3DT_Add(pic);
//		item->AktMat = true;
	}

	item->AlphaRef = 0.5;
	item->AlphaRef2 = 0x80;
	item->IsAlphaRef = true;

	return D3D_ID;
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření trojúhelníku

D3DVECTOR triangle_vertices[3] = {
	-0.5, -0.5, 0,			// 0 (vlevo dole)
     0,    0.5, 0,			// 1 (uprostřed nahoře)
     0.5, -0.5, 0			// 2 (vpravo dole)
};

int triangle_faces[3] = {
	0, 1, 2
};

D3DVECTOR triangle_normals[3] = {
	0, 0, -1,
	0, 0, -1,
	0, 0, -1
};

float triangle_text[2*3] = {
	0,   1,	// 0
	0.5, 0,	// 1
	1,   1 	// 2
};

double FD3DTriangle()
{
	int inx = D3DF_CreateMesh(3, triangle_vertices, 1, triangle_faces);
	D3DF_SetNormal(inx, triangle_normals);
	D3DF_SetTextUV(inx, triangle_text, 0);
	D3D_ID = inx;
	D3DF_SetAddress(inx, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP, 0);
	D3DF_KorRightHand();
	return D3D_ID;
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření krychle

//    3----7
//    |\    \ 
//    | 2----6
//    1 |  5 |
//     \|    |
//      0----4

D3DVECTOR cube_vertices[24] = {

// horní stěna (musí být jako první stěna!)
    -0.5,  0.5,  0.5,	// ( 0) 3 horní stěny
     0.5,  0.5,  0.5,	// ( 1) 7 horní stěny
    -0.5,  0.5, -0.5,	// ( 2) 2 horní stěny
     0.5,  0.5, -0.5,	// ( 3) 6 horní stěny

// přední stěna
    -0.5,  0.5, -0.5,	// ( 4) 2 přední stěny
     0.5,  0.5, -0.5,	// ( 5) 6 přední stěny
	-0.5, -0.5, -0.5,	// ( 6) 0 přední stěny
     0.5, -0.5, -0.5,	// ( 7) 4 přední stěny

// zadní stěna
     0.5,  0.5,  0.5,	// ( 8) 7 zadní stěny
    -0.5,  0.5,  0.5,	// ( 9) 3 zadní stěny
     0.5, -0.5,  0.5,	// (10) 5 zadní stěny
    -0.5, -0.5,  0.5,	// (11) 1 zadní stěny

// levá stěna
    -0.5,  0.5,  0.5,	// (12) 3 levé stěny
    -0.5,  0.5, -0.5,	// (13) 2 levé stěny
    -0.5, -0.5,  0.5,	// (14) 1 levé stěny
	-0.5, -0.5, -0.5,	// (15) 0 levé stěny

// pravá stěna
     0.5,  0.5, -0.5,	// (16) 6 pravé stěny
     0.5,  0.5,  0.5,	// (17) 7 pravé stěny
     0.5, -0.5, -0.5,	// (18) 4 pravé stěny
     0.5, -0.5,  0.5,	// (19) 5 pravé stěny

// dolní stěna (musí být jako poslední stěna!)
	-0.5, -0.5, -0.5,	// (20) 0 dolní stěny
     0.5, -0.5, -0.5,	// (21) 4 dolní stěny
    -0.5, -0.5,  0.5,	// (22) 1 dolní stěny
     0.5, -0.5,  0.5,	// (23) 5 dolní stěny
};

//    3----7
//    |\    \ 
//    | 2----6
//    1 |  5 |
//     \|    |
//      0----4

int cube_faces[36] = {
	0,	1,	2,			// horní stěna (3-7-2 a 2-7-6)
	2,	1,	3,

	4,	5,	6,			// přední stěna (2-6-0 a 0-6-4)
	6,  5,	7,

	8,  9,  10,			// zadní stěna (7-3-5 a 5-3-1)
	10, 9,  11,

	12, 13, 14,			// levá stěna (3-2-1 a 1-2-0)
	14, 13, 15,

	16, 17, 18,			// pravá stěna (6-7-4 a 4-7-5)
	18, 17, 19,

	20, 21, 22,			// dolní stěna (0-4-1 a 1-4-5)
	22, 21, 23,
};

//    3----7
//    |\    \ 
//    | 2----6
//    1 |  5 |
//     \|    |
//      0----4

D3DVECTOR cube_normals[24] = {
	 0,  1,  0,		// 3 horní stěny
	 0,  1,  0,		// 7 horní stěny
	 0,  1,  0,		// 2 horní stěny
	 0,  1,  0,		// 6 horní stěny

	 0,  0, -1,		// 2 přední stěny
	 0,  0, -1,		// 6 přední stěny
	 0,  0, -1,		// 0 přední stěny
	 0,  0, -1,		// 4 přední stěny

	 0,  0,  1,		// 7 zadní stěny
	 0,  0,  1,		// 3 zadní stěny
	 0,  0,  1,		// 5 zadní stěny
	 0,  0,  1,		// 1 zadní stěny

	-1,  0,  0,		// 3 levé stěny
	-1,  0,  0,		// 2 levé stěny
	-1,  0,  0,		// 1 levé stěny
	-1,  0,  0,		// 0 levé stěny

	 1,  0,  0,		// 6 pravé stěny
	 1,  0,  0,		// 7 pravé stěny
	 1,  0,  0,		// 4 pravé stěny
	 1,  0,  0,		// 5 pravé stěny

	 0, -1,  0,		// 0 dolní stěny
	 0, -1,  0,		// 4 dolní stěny
	 0, -1,  0,		// 1 dolní stěny
	 0, -1,  0,		// 5 dolní stěny
};

// Mapování částí textury:
//     zadní     levá      dolní
//     přední    pravá     horní

float cube_text[2*24] = {
	(float)2/3,		0.5,			// 3 horní stěny
	1,				0.5,			// 7 horní stěny
	(float)2/3,		1,				// 2 horní stěny
	1,				1,				// 6 horní stěny

	0,				0.5,			// 2 přední stěny
	(float)1/3,		0.5,			// 6 přední stěny
	0,				1,				// 0 přední stěny
	(float)1/3,		1,				// 4 přední stěny

	0,				0,				// 7 zadní stěny
	(float)1/3,		0,				// 3 zadní stěny
	0,				0.5,			// 5 zadní stěny
	(float)1/3,		0.5,			// 1 zadní stěny

	(float)1/3,		0,				// 3 levé stěny
	(float)2/3,		0,				// 2 levé stěny
	(float)1/3,		0.5,			// 1 levé stěny
	(float)2/3,		0.5,			// 0 levé stěny

	(float)1/3,		0.5,			// 6 pravé stěny
	(float)2/3,		0.5,			// 7 pravé stěny
	(float)1/3,		1,				// 4 pravé stěny
	(float)2/3,		1,				// 5 pravé stěny

	(float)2/3,		0,				// 0 dolní stěny
	1,				0,				// 4 dolní stěny
	(float)2/3,		0.5,			// 1 dolní stěny
	1,				0.5,			// 5 dolní stěny
};


double FD3DCube()
{
	D3DVECTOR* v = cube_vertices;
	D3DVECTOR* n = cube_normals;
	float* t = cube_text;

	int vn = 24;
	int fn = 12;

	if (!D3DCreateUpper)
	{
		v += 4;
		n += 4;
		t += 8;
		vn -= 4;
		fn -= 2;
	}

	if (!D3DCreateLower)
	{
		vn -= 4;
		fn -= 2;
	}

	int inx = D3DF_CreateMesh(vn, v, fn, cube_faces);
	D3DF_SetNormal(inx, n);
	D3DF_SetTextUV(inx, t, 0);
	D3D_ID = inx;
	D3DF_SetAddress(inx, D3DTADDRESS_WRAP, D3DTADDRESS_WRAP, 0);
	D3DF_KorRightHand();
	return D3D_ID;
}

/////////////////////////////////////////////////////////////////////////////
// generátor rotačních objektů - shora dolů (závěrem zruší globální buffery)

// popisovač řady
typedef struct GENROWITEM_
{
	double	dy;					// rozdíl souřadnic Y k další řadě
	double	dr;					// rozdíl poloměrů R k další řadě
	double	length;				// délka spojnice k další řadě
	double	angle;				// úhel spojnice k další řadě
	double	norm1;				// úhel normály - první řada
	double	norm2;				// úhel normály - druhá řada
	int		vn;					// počet vrcholů v jedné řadě (bez zdvojnásobení)
	bool	r0;					// příznak nulového poloměru
	bool	smooth;				// příznak zjemnění normály - společná normála pro obě strany
	bool	close;				// objekt je uzavřený (konec a počátek jsou totožné)
	bool	prev;				// příznak, že jsou trojúhelníky zpět (nahoru)
	bool	next;				// příznak, že jsou trojúhelníky vpřed (dolů)
	bool	use2;				// příznak použití 2 řad vrcholů
} GENROWITEM;

int			genRows;			// počet řad objektu
double*		genY = NULL;		// buffer souřadnic Y (shora dolů)
double*		genR = NULL;		// buffer poloměrů řad

// parametr sm = zjemňovat vždy (=koule, toroid)

void GenRot(bool sm)
{

// pro malý počet řad se vytvoří náhradní prázdná skupina
	if (genRows <= 1)
	{
		MemFree(genY);
		MemFree(genR);

		D3D_ID = D3DF_New();
		return;
	}

// příprava proměnných
	int level = D3DLevel;			// počet dělení (polovina)
	int level2 = level*2;			// počet dělení na celý obvod
	double alfa = pi / level;		// úhel na jeden díl
	double alfa2 = alfa/2;			// poloviční úhel na jeden díl
	int i,j;

// příprava bufferu popisovače řad
	GENROWITEM* gr = (GENROWITEM*)MemGet(genRows * sizeof(GENROWITEM));
	GENROWITEM* item = gr;									// ukazatel položek popisovače řady

	double delka = 0;										// celková délka spojnic
	int vN = 0;												// počet vrcholů celkem
	int fN = 0;												// počet plošek celkem

// cyklus přes všechny řady
	bool firstrow = true;									// příznak, že to je první řada

	for (i = 0; i < genRows; i++)
	{

// příznak poslední řady
		bool lastrow = (i == (genRows - 1));

// příznak nulového poloměru
		bool r00 = (genR[i] == 0);
		item->r0 = r00;

// rozdíl souřadnic Y a poloměrů R k další řadě
		double dy0 = 0;
		double dr0 = 0;

		if (!lastrow)
		{
			dy0 = genY[i + 1] - genY[i];
			dr0 = genR[i + 1] - genR[i];
		}

		item->dy = dy0;
		item->dr = dr0;

// délka spojnice k další řadě
		double d0 = sqrt(dy0*dy0 + dr0*dr0);
		item->length = d0;
		delka += d0;
		
// úhel spojnice k další řadě
		double a0 = 0;
		if (d0 != 0)
		{
			a0 = atan2(dy0, dr0);
		}
		else
		{
			if (!firstrow)
			{
				a0 = item[-1].angle;
			}
		}
		item->angle = a0;
				 
// rozdíl úhlu s předešlou řadou (pro první a poslední řadu se počítá úhel sám se sebou)
// (první a poslední řada může splývat, je-li uzavřený objekt - např. torus)
// (pro zjednodušení předpokládáme, že uzavřený objekt má konce vždy zjemněné,
//  protože na začátku neznáme směrník konce)
		bool close = false;					// příznak uzavřeného objektu

		double da = uhel90;

		if (firstrow || lastrow)
		{
			if ((fabs(genR[0] - genR[genRows - 1]) +
				fabs(genY[0] - genY[genRows - 1])) < 0.0001)
			{
				close = true;
			}
			else
			{
				if (r00)
				{
					da = fabs(2*a0);
				}
			}
		}
		else
		{
			da = fabs(a0 - item[-1].angle);
		}

		if (da > pi) da = pi2 - da;

// porovnání úhlu, zda bude zjemnění (zvláštní korekce se provádí pro kouli se složitostí 2)
		bool smooth0;
		if (level == 2)
		{
			if ((genRows == 3) &&
				(genR[0] == 0) && 
				(genR[2] == 0))
			{
				smooth0 = true;				// zvláštní případ - koule se složitostí 2 (úhel 90 stupňů)
			}
			else
			{
				smooth0 = (da < 1.4);		// tj. úhel asi 80 stupňů
			}
		}
		else
		{
			if (level == 3)
			{
				smooth0 = (da < 1.1);		// tj. úhel asi 64 stupňů, aby vyhověla koule se složitostí 3
			}
			else
			{
				smooth0 = (da < 0.8);		// tj. úhel asi 45.8 stupňů, aby vyhověla koule se složitostí 4
			}
		}
		if (close || sm) smooth0 = true;		// pro uzavřený objekt (nebo nucené zjemnění) zjemníme vždy

		item->smooth = smooth0;
		item->close = close;

// příznak, že jsou trojúhelníky zpět (nahoru)
		bool prev0 = (!r00 && !firstrow);
		item->prev = prev0;
		if (prev0) fN += level2;

// příznak, že jsou trojúhelníky vpřed (dolů)
		bool next0 = (!r00 && !lastrow);
		item->next = next0;
		if (next0) fN += level2;

// příznak použití 2 řad vrcholů
		bool use20 = (prev0 && next0 && !smooth0);
		item->use2 = use20;

// počet vrcholů v jedné řadě
		int vn00 = level2;
		if (!r00) vn00++;
		item->vn = vn00;
		if (use20) vn00 = vn00 * 2;
		vN += vn00;

// výpočet úhlu normál
		double n2 = a0 + uhel90;
		double n1 = n2;

		if (firstrow)
		{
			if (smooth0)
			{
				n1 = uhel90;

				if (close)
				{
					if ((n2 > -uhel45) && (n2 < uhel45))
					{
						n1 = 0;
					}
					else
					{
						if ((n2 >= uhel45) && (n2 < uhel135))
						{
							n1 = uhel90;
						}
						else
						{
							if ((n2 <= -uhel45) || (n2 >= uhel225))
							{
								n1 = uhel270;
							}
							else
							{
								n1 = uhel180;
							}
						}
					}

					if (sm) n1 = uhel180;	// torus
				}
				n2 = n1;
			}
		}
		else
		{
			if (lastrow)
			{
				if (smooth0)
				{
					n1 = -uhel90;

					if (close)
					{
						if ((n2 > -uhel45) && (n2 < uhel45))
						{
							n1 = 0;
						}
						else
						{
							if ((n2 >= uhel45) && (n2 < uhel135))
							{
								n1 = uhel90;
							}
							else
							{
								if ((n2 <= -uhel45) || (n2 >= uhel225))
								{
									n1 = uhel270;
								}
								else
								{
									n1 = uhel180;
								}
							}
						}

						if (sm) n1 = uhel180;	// torus
					}
					n2 = n1;
				}
			}
			else
			{
				n1 = item[-1].angle + uhel90;
			}
		}

		if (n1 > pi) n1 -= pi2;
		if (n1 < -pi) n1 += pi2;
		if (n2 > pi) n2 -= pi2;
		if (n2 < -pi) n2 += pi2;

		if (smooth0)
		{
			n1 = (n1 + n2)/2;
			n2 = n1;
		}

		item->norm1 = n1;
		item->norm2 = n2;

// příprava pro další řadu
		firstrow = false;
		item++;
	}

// kontrola platnosti bodů - vytvoří se náhradní prázdná skupina
	if (delka == 0)
	{
		MemFree(gr);
		MemFree(genY);
		MemFree(genR);

		D3D_ID = D3DF_New();
		return;
	}

// vytvoření výstupních bufferů
	D3DVECTOR* v = (D3DVECTOR*)MemGet(vN * sizeof(D3DVECTOR)); // buffer vektorů vrcholů
	D3DVECTOR* n = (D3DVECTOR*)MemGet(vN * sizeof(D3DVECTOR)); // buffer vektorů normál
	float* t = (float*)MemGet(vN * 2 * sizeof(float));		// buffer souřadnic textur
	int* f = (int*)MemGet(fN * 3 * sizeof(int));			// buffer plošek

// proměnné ke generování
	double even = false;									// příznak sudé řady

	D3DVECTOR* vp = v;										// ukládací adresa vektorů vrcholů
	D3DVECTOR* np = n;										// ukládaví adresa vektorů normál
	float* tp = t;											// ukládací adresa souřadnic textur
	int* fp = f;											// ukládací adresa plošek

	int inx = 0;											// ukazatel indexu bodu
	double del = 0;											// ukazatel souřadnice textury
	delka = 1/delka;										// délka jako násobitel (pro zrychlení výpočtu)

// cyklus přes všechny řady bodů
	item = gr;												// ukazatel položek popisovače řady

	for (i = 0; i < genRows; i++)
	{

// příprava proměnných
		double r = genR[i];									// poloměr řady
		double y = genY[i];									// souřadnice Y řady

		int vn = item->vn;									// počet vrcholů v řadě

		bool use2 = item->use2;								// vytvoří se 2 řady vrcholů

		float nn1 = (float)sin(item->norm1);				// normála první řady vrcholů (složka y)
		float nn2 = nn1;
		double rr1 = cos(item->norm1);
		double rr2 = rr1;

		if (use2)
		{
			nn2 = (float)sin(item->norm2);					// normála druhé řady vrcholů (složka y)
			rr2 = cos(item->norm2);
		}

		double gama = 0;									// ukazatel směru otočení bodu
		if (even)
		{
			gama = alfa2;									// pro sudou řadu odsazení
		}
		else
		{
			if (item->r0) gama = alfa;						// první bod se nevytváří
		}

// vygenerování vrcholů
		for (j = vn; j > 0; j--)
		{
			double cosgama = cos(gama);
			double singama = sin(gama);

// souřadnice vrcholu
			vp->x = (float)(-r*singama);
			vp->y = (float)y;
			vp->z = (float)(r*cosgama);

// normála vrcholu
			np->x = (float)(-rr1*singama);
			np->y = nn1;
			np->z = (float)(rr1*cosgama);

// textura vrcholu
			*tp = (float)(gama/pi2);
			tp++;
			*tp = (float)del;

// vytvoření druhého vrcholu
			if (use2)
			{
				vp[vn] = vp[0];

				np[vn].x = (float)(-rr2*singama);
				np[vn].y = nn2;
				np[vn].z = (float)(rr2*cosgama);

				tp[2*vn-1] = tp[-1];
				tp[2*vn] = tp[0];
			}

// zvýšení ukazatelů
			vp++;
			np++;
			tp++;
			gama += alfa;
		}

// přeskočení druhé řady vrcholů
		if (use2)
		{
			vp += vn;
			np += vn;
			tp += 2*vn;
		}

// vygenerování horní řady trojúhelníků
		if (item->prev)
		{
			int inx2 = inx;

			for (j = 0; j < level2; j++)
			{

			// levý bod
				*fp = inx2;
				fp++;

			// horní bod
				int k = inx2 - item[-1].vn;
				if (even && !item[-1].r0) k = k + 1;
				*fp = k;
				fp++;

			// pravý bod
				*fp = inx2 + 1;
				fp++;

				inx2++;
			}
		}


// vygenerování dolní řady trojúhelníků
		if (item->next)
		{
			int inx2 = inx;
			if (use2) inx2 = inx + vn;

			for (j = 0; j < level2; j++)
			{
			// levý bod
				*fp = inx2;
				fp++;

			// pravý bod
				*fp = inx2 + 1;
				fp++;

			// dolní bod
				int k = inx2 + vn;
				if (even && !item[1].r0) k = k + 1;
				*fp = k;
				fp++;

				inx2++;
			}
		}


// zvýšení ukazatele délky (pro textury)
		del += item->length*delka;

// zvýšení ukazatele indexu vrcholu
		inx = inx + vn;							// zvýšení ukazatele indexu položek
		if (use2) inx = inx + vn;

// příprava pro další řadu
		even = !even;							// změna příznaku sudé řady
		item++;
	}

	ASSERT(inx == vN);
	ASSERT(fp == (f + 3*fN));

// vytvoření objektu
	int index = D3DF_CreateMesh(vN, v, fN, f);
	D3DF_SetNormal(index, n);
	D3DF_SetTextUV(index, t, 0);
	D3D_ID = index;
	D3DF_SetAddress(index, D3DTADDRESS_WRAP, D3DTADDRESS_CLAMP, 0);

// korekce pro pravoruký souřadný systém
	D3DF_KorRightHand();

// zrušení bufferů
	MemFree(v);
	MemFree(n);
	MemFree(f);
	MemFree(t);
	MemFree(gr);
	MemFree(genY);
	MemFree(genR);
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření kruhu

double FD3DCircle()
{
	int rows = (D3DLevel/16) + 2;		// počet pásů

	genRows = rows;

	genY = (double*)MemGet(rows * sizeof(double));
	genR = (double*)MemGet(rows * sizeof(double));

	double* geny = genY;
	double* genr = genR;

	double r = 0;
	double dr = 0.5/(rows - 1);

	for (rows--; rows > 0; rows--)
	{
		*geny = 0;
		*genr = r;
		r += dr;

		geny++;
		genr++;
	}

	*geny = 0;
	*genr = 0.5;

	GenRot(false);

// převrácení kruhu, aby směřoval "-z"
// (zde se předpokládá, že buffery ještě nebyly vytvořeny!!!)
	D3DFITEM* item = GetID();

	ASSERT(item->VertNum > 0);
	ASSERT(item->Vertex != NULL);
	ASSERT(item->Normal != NULL);
	ASSERT(item->TextUV != NULL);
	ASSERT(item->Data1 == NULL);
	ASSERT(item->Data2 == NULL);

	D3DF_SetAddress(D3D_ID, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP, 0);

	D3DVECTOR* v = item->Vertex;
	D3DVECTOR* n = item->Normal;
	float* t = item->TextUV[0];

	for (int i = item->VertNum; i > 0; i--)
	{
		float z = v->z;
		v->y = v->z;
		v->z = 0;

		*t = (float)(v->x + 0.5);
		t++;
		*t = (float)(0.5 - z);
		t++;

		v++;

		n->x = 0;
		n->y = 0;
		n->z = -1;
		n++;
	}

// aktualizovat sektory
	item->AktSector = TRUE;

	return D3D_ID;
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření koule

double FD3DSphere()
{
	int rows = D3DLevel + 1;		// počet řad bodů vnitřního pásu
	genRows = rows;

	genY = (double*)MemGet(genRows * sizeof(double));
	genR = (double*)MemGet(genRows * sizeof(double));

	double* geny = genY;
	double* genr = genR;

	*geny = 0.5;	geny++;
	*genr = 0;		genr++;

	double a = uhel90;
	double da = pi/D3DLevel;

	for (rows -= 2; rows > 0; rows--)
	{
		a -= da;
		*geny = 0.5*sin(a);		geny++;
		*genr = 0.5*cos(a);		genr++;
	}

	*geny = -0.5;
	*genr = 0;

	GenRot(true);

	return D3D_ID;
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření polokoule

double FD3DHemisphere()
{
	int rows = (D3DLevel+1)/2 + 1;		// počet řad bodů vnitřního pásu
	if (D3DCreateLower)
	{
		genRows = rows + 1;
	}
	else
	{
		genRows = rows;
	}

	genY = (double*)MemGet((rows + 1) * sizeof(double));
	genR = (double*)MemGet((rows + 1) * sizeof(double));

	double* geny = genY;
	double* genr = genR;

	*geny = 0.5;	geny++;
	*genr = 0;		genr++;

	double a = uhel90;
	rows--;
	double da = a/rows;

	for (; rows > 0; rows--)
	{
		a -= da;
		*geny = 0.5*sin(a);		geny++;
		*genr = 0.5*cos(a);		genr++;
	}

	*geny = 0;
	*genr = 0;

	GenRot(false);

	return D3D_ID;
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření torusu

double FD3DTorus()
{
	double r = FNum()/2;			// menší poloměr

	if (r < 0.0001)				// minimální poloměr
	{
		return FD3DEmpty();			// vytvoří se jen skupina
	}

	int rows = 2*D3DLevel + 1;			// počet generovaných bodů
	genRows = rows;

	genY = (double*)MemGet(genRows * sizeof(double));
	genR = (double*)MemGet(genRows * sizeof(double));

	double* geny = genY;
	double* genr = genR;

	double a = uhel180;
	double da = pi/D3DLevel;

	*geny = 0;			geny++;
	*genr = 0.5 - r;	genr++;

	for (rows -= 2; rows > 0; rows--)
	{
		a -= da;

		*geny = r*sin(a);			geny++;
		*genr = 0.5 + r*cos(a);		genr++;
	}

	*geny = 0;
	*genr = 0.5 - r;

	int oldlevel = D3DLevel;

	int ii = Round(D3DLevel/r/10);
	if (ii > (2*D3DLevel)) ii = (2*D3DLevel);
	D3DLevel += ii;
	GenRot(true);
	D3DLevel = oldlevel;

	return D3D_ID;
}


/////////////////////////////////////////////////////////////////////////////
// vytvoření válce

double FD3DCylinder()
{
	int rows = D3DLevel/8 + 2;			// počet řad bodů vnitřního pásu
	int extra = (D3DLevel/20);			// přídavné řádky nahoře a dole

	genRows = rows;

	if (D3DCreateUpper)
	{
		genRows++;
		genRows += extra;
	}

	if (D3DCreateLower)
	{
		genRows++;
		genRows += extra;
	}

	genY = (double*)MemGet((rows + 2 + 2*extra) * sizeof(double));
	genR = (double*)MemGet((rows + 2 + 2*extra) * sizeof(double));

	double* geny = genY;
	double* genr = genR;

	double dd = (double)0.5/(extra+1);

	if (D3DCreateUpper)
	{
		double d = 0;

		for (int i = extra + 1; i > 0; i--)
		{
			*geny = 0.5;	geny++;
			*genr = d;		genr++;
			d += dd;
		}
	}

	double h = 0.5;
	double dh = (double)1/(rows-1);

	for (rows--; rows > 0; rows--)
	{
		*geny = h;		geny++;
		*genr = 0.5;	genr++;
		h -= dh;
	}

	*geny = -0.5;	geny++;
	*genr = 0.5;	genr++;

	if (D3DCreateLower)
	{
		double d = 0.5;

		for (int i = extra; i > 0; i--)
		{
			d -= dd;
			*geny = -0.5;	geny++;
			*genr = d;		genr++;
		}

		*geny = -0.5;
		*genr = 0;
	}

	GenRot(false);

	return D3D_ID;
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření kužele

double FD3DCone()
{
	int rows = D3DLevel/8 + 2;			// počet řad bodů vnitřního pásu
	int extra = (D3DLevel/20);			// přídavné řádky dole

	genRows = rows;

	if (D3DCreateLower)
	{
		genRows++;
		genRows += extra;
	}

	genY = (double*)MemGet((rows + 1 + extra) * sizeof(double));
	genR = (double*)MemGet((rows + 1 + extra) * sizeof(double));

	double* geny = genY;
	double* genr = genR;

	double r = 0;
	double y = 0.5;
	double dr = 0.5/(rows-1);
	double dy = (double)1/(rows-1);

	for (rows--; rows > 0; rows--)
	{
		*geny = y;		geny++;
		*genr = r;		genr++;
		r += dr;
		y -= dy;
	}

	*geny = -0.5;	geny++;
	*genr = 0.5;	genr++;

	if (D3DCreateLower)
	{
		r = 0.5;
		dr = 0.5/(extra+1); 

		for (int i = extra; i > 0; i--)
		{
			r -= dr;
			*geny = -0.5;	geny++;
			*genr = r;		genr++;
		}

		*geny = -0.5;
		*genr = 0;
	}

	GenRot(false);

 	return D3D_ID;
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření komolého kužele

double FD3DCone2()
{
	double r = FNum()/2;			// menší poloměr

	if (r < 0.001)					// minimální poloměr
	{
		return FD3DCone();
	}

	int rows = D3DLevel/8 + 2;			// počet řad bodů vnitřního pásu
	int extra1 = (Round(2*r*D3DLevel)/20);	// přídavné řádky nahoře
	int extra2 = (D3DLevel/20);			// přídavné řádky dole

	genRows = rows;

	if (D3DCreateUpper)
	{
		genRows++;
		genRows += extra1;
	}

	if (D3DCreateLower)
	{
		genRows++;
		genRows += extra2;
	}

	genY = (double*)MemGet((rows + 2 + extra1 + extra2) * sizeof(double));
	genR = (double*)MemGet((rows + 2 + extra1 + extra2) * sizeof(double));

	double* geny = genY;
	double* genr = genR;

	double dd = r/(extra1+1);

	if (D3DCreateUpper)
	{
		double d = 0;

		for (int i = extra1 + 1; i > 0; i--)
		{
			*geny = 0.5;	geny++;
			*genr = d;		genr++;
			d += dd;
		}
	}

	double y = 0.5;
	double dr = (0.5-r)/(rows-1);
	double dy = (double)1/(rows-1);

	for (rows--; rows > 0; rows--)
	{
		*geny = y;		geny++;
		*genr = r;		genr++;
		r += dr;
		y -= dy;
	}

	*geny = -0.5;	geny++;
	*genr = 0.5;	genr++;

	if (D3DCreateLower)
	{
		r = 0.5;
		dr = 0.5/(extra2+1); 

		for (int i = extra2; i > 0; i--)
		{
			r -= dr;
			*geny = -0.5;	geny++;
			*genr = r;		genr++;
		}

		*geny = -0.5;
		*genr = 0;
	}

	GenRot(false);

 	return D3D_ID;
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření světla

double FD3DLight()
{		
	D3D_ID = D3DF_New();
	D3DFITEM* item = GetID();
	item->Type = D3DFTYPE_LIGHT;
	item->Light = D3DL_New();
	D3DL_Get(item->Light)->Frame = D3D_ID;

	return D3D_ID;
}

/////////////////////////////////////////////////////////////////////////////
// klonování

double FD3DClone()
{
	int inx = FIntR();

	D3D_ID = D3DF_Copy(inx);

	if (GetID()->IsTerrain)
	{
		TerenID = D3D_ID;
	}

	return D3D_ID;
}


/////////////////////////////////////////////////////////////////////////////
// načtení objektu ze souboru

double FD3DFile()
{
// načtení jména souboru
	CString text;
	FText(text);

// úprava jména souboru
	text.TrimLeft();
	text.TrimRight();
	text.PathName(0);

	if (text.IsEmpty())
	{
READERROR:
		FileError = true;
		return FD3DEmpty();
	}

// otevření souboru
	HANDLE file = ::CreateFile(text, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (FileReadHandle == INVALID_HANDLE_VALUE) goto READERROR;

// zjištění velikosti souboru
	int size = (int)::GetFileSize(file, NULL);

	if (size <= 10)
	{	
READERROR2:
		::CloseHandle(file);
		goto READERROR;
	}

// vytvoření datového bufferu
	BYTE* data = (BYTE*)MemGet(size);

// načtení souboru
	DWORD read = 0;

	if ((::ReadFile(file, (void*)data, size, &read, NULL) == FALSE) ||
		((int)read != size))
	{
		MemFree(data);
		goto READERROR2;
	}
	::CloseHandle(file);

// příprava cesty do adresáře souboru
	text.Delete(text.RevFind('\\'));

// vytvoření objektu
	ReadMesh(data, size, text);

// zrušení datového bufferu
	MemFree(data);

	return D3D_ID;
}

/////////////////////////////////////////////////////////////////////////////
// načtení objektu z textu

double FD3DText()
{
	CString text;
	FText(text);

	CString path;
	ReadMesh((BYTE*)(LPCSTR)text, text.Length(), path);

	return D3D_ID;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění ID aktivního objektu

double FGetD3DObjectID()
{
	return D3D_ID;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění pořadí rotací objektů

double FGetD3DOrder()
{
	if (IsValidID()) return GetID()->Order;
	return D3DFORDER_XYZ;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění měřítka ve směru X

double FGetD3DScaleX()
{
	if (IsValidID()) return GetID()->ScaleX;
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění měřítka ve směru Y

double FGetD3DScaleY()
{
	if (IsValidID()) return GetID()->ScaleY;
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění měřítka ve směru Z

double FGetD3DScaleZ()
{
	if (IsValidID()) return GetID()->ScaleZ;
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění rotace podle osy X

double FGetD3DRotateX()
{
	if (IsValidID()) return GetID()->RotateX;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění rotace podle osy Y

double FGetD3DRotateY()
{
	if (IsValidID()) return GetID()->RotateY;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění rotace podle osy Z

double FGetD3DRotateZ()
{
	if (IsValidID()) return GetID()->RotateZ;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění posunu ve směru X

double FGetD3DTransX()
{
	if (IsValidID()) return GetID()->TransX;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění posunu ve směru Y

double FGetD3DTransY()
{
	if (IsValidID()) return GetID()->TransY;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění posunu ve směru Z

double FGetD3DTransZ()
{
	if (IsValidID()) return GetID()->TransZ;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění barvy objektu (-1=vnitřní objektu, -2=z rodiče, jinak určená)

double FGetD3DColor()
{
	if (IsValidID())
	{
		D3DFITEM* item = GetID();
		if (item->MatSource == MATSOURCE_PARENT) return -2;
		if (item->MatSource == MATSOURCE_OBJECT) return -1;
		return item->Diffuse;
	}

	return -1;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění barvy svítivosti

double FGetD3DEmissive()
{
	if (IsValidID())
	{
		return GetID()->Emissive;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění odrazu rozptýleného světla

double FGetD3DAmbiRef()
{
	if (IsValidID())
	{
		D3DFITEM* item = GetID();
		if (item->Ambient == MAXDWORD) return -1;
		return item->Ambient;
	}

	return -1;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění matnosti

double FGetD3DPower()
{
	if (IsValidID())
	{
		return GetID()->Power;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění barvy odlesku

double FGetD3DSpecular()
{
	if (IsValidID())
	{
		return GetID()->Specular;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění barvy světla

double FGetD3DLightColor()
{
	if (IsValidID())
	{
		D3DFITEM* item = GetID();

		if (item->Type == D3DFTYPE_LIGHT)
		{
			return D3DL_Get(item->Light)->Diffuse;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění typu světla

double FGetD3DLightType()
{
	if (IsValidID())
	{
		D3DFITEM* item = GetID();

		if (item->Type == D3DFTYPE_LIGHT)
		{
			return (double)(int)(D3DL_Get(item->Light)->Light.Type);
		}
	}

	return (double)(int)D3DLIGHT_DIRECTIONAL;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění úhlu světelného kužele

double FGetD3DLightUmbra()
{
	if (IsValidID())
	{
		D3DFITEM* item = GetID();

		if (item->Type == D3DFTYPE_LIGHT)
		{
			return D3DL_Get(item->Light)->Theta;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění úhlu polostínu světla

double FGetD3DLightPenumbra()
{
	if (IsValidID())
	{
		D3DFITEM* item = GetID();

		if (item->Type == D3DFTYPE_LIGHT)
		{
			return D3DL_Get(item->Light)->Phi;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění dosahu světla

double FGetD3DLightRange()
{
	if (IsValidID())
	{
		D3DFITEM* item = GetID();

		if (item->Type == D3DFTYPE_LIGHT)
		{
			return D3DL_Get(item->Light)->Range;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění intenzity světla

double FGetD3DLightCatten()
{
	if (IsValidID())
	{
		D3DFITEM* item = GetID();

		if (item->Type == D3DFTYPE_LIGHT)
		{
			return D3DL_Get(item->Light)->Intens;
		}
	}

	return 1;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění lineárního útlumu světla

double FGetD3DLightLatten()
{
	if (IsValidID())
	{
		D3DFITEM* item = GetID();

		if (item->Type == D3DFTYPE_LIGHT)
		{
			return D3DL_Get(item->Light)->Attenuation1;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění kvadratického útlumu světla

double FGetD3DLightQatten()
{
	if (IsValidID())
	{
		D3DFITEM* item = GetID();

		if (item->Type == D3DFTYPE_LIGHT)
		{
			return D3DL_Get(item->Light)->Attenuation2;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění projekce

double FGetD3DProjection()
{
	return D3DProjection;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění barvy pozadí scény

double FGetD3DSceneColor()
{
	if (D3DBackCol == MAXDWORD) return -1;
	if (D3DBackCol == (MAXDWORD-1)) return -2;
	return D3DBackCol;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění ambient světla

double FGetD3DAmbient()
{	
	return D3DAmbientCol;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění barvy mlhy

double FGetD3DFogColor()
{
	if (FogColor == MAXDWORD) return -1;
	return FogColor;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění módy mlhy

double FGetD3DFogMode()
{
	int n = FogType;
	if (FogKorig) n += 4;
	return n;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění začátku mlhy

double FGetD3DFogStart()
{
	return FogStart;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění konce mlhy

double FGetD3DFogEnd()
{
	return FogEnd;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění hustotu mlhy

double FGetD3DFogDensity()
{
	return FogDens;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění projekční roviny

double FGetD3DViewFront()
{
	return D3DFrontClip;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění vzdálenosti dohledu

double FGetD3DViewBack()
{
	return D3DBackClip;
}

/////////////////////////////////////////////////////////////////////////////
// vyhledání objektu

//double FD3DPick()
//{		
	/*
	int x = FIntX();
	if (x == (0x1000000*ICONWIDTH))
	{
		x = D3DW/2;
	}
	else
	{
		x -= D3DX;
	}

	int y = FIntY();
	if (y == (0x1000000*ICONHEIGHT))
	{
		y = D3DH/2;
	}
	else
	{
		y = Height - y - D3DY;
	}

	if (D3View != NULL)
	{
		LPDIRECT3DRMPICKEDARRAY list = NULL;

		D3View->Pick(x, y, &list);

		if (list != NULL)
		{
			int size = list->GetSize();
			int inx = 0;

			for (; size > 0; size--)
			{
				LPDIRECT3DRMVISUAL visual = NULL;
				LPDIRECT3DRMFRAMEARRAY frames = NULL;
				D3DRMPICKDESC desc;

				list->GetPick(inx, &visual, &frames, &desc);

				if (frames != NULL)
				{
					int size2 = frames->GetSize();
					int inx2 = size2 - 1;

					for (; size2 > 0; size2--)
					{
						LPDIRECT3DRMFRAME frame = NULL;
  
						frames->GetElement(inx2, &frame);

						if (frame != NULL)
						{
							int i = frame->GetAppData();
							if ((i != 1) && D3DF.IsValid(i) && (D3DF[i].Light == NULL))
							{
								frame->Release();
								frames->Release();
								list->Release();
								return i;
							}

							frame->Release();
						}

						inx2--;
					}

					frames->Release();
				}

				inx++;
			}

			list->Release();
		}
	}
	*/

//	return -1;
//}

/////////////////////////////////////////////////////////////////////////////
// vytvoření 2D objektu

double FD3D2DObject()
{
	CPicture pic;
	FPicture(pic);

	int inx = D3DF_CreateMesh(4, wall_vertices, 2, wall_faces);
	D3DF_SetNormal(inx, wall_normals);
	D3DF_SetTextUV(inx, wall_text, 0);
	D3D_ID = inx;

	D3DFITEM* item = GetID();
	item->Type = D3DFTYPE_DECAL;
	item->MatSource = MATSOURCE_FRAME;
	D3DF_SetAddress(inx, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP, 0);
	D3DF_KorRightHand();

	if (pic.IsNotEmpty())
	{
		item->Texture[0] = D3DT_Add(pic);
//		item->AktMat = true;
	}

	item->AlphaRef = 0.5;
	item->AlphaRef2 = 0x80;
	item->IsAlphaRef = true;

	return D3D_ID;
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření prázdného objektu

double FD3DEmpty()
{
	D3D_ID = D3DF_New();
	return D3D_ID;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění složitosti objektů

double FGetD3DSplits()
{
	return D3DLevel;
}


/////////////////////////////////////////////////////////////////////////////
// zjištění ovladače

double FGetD3DDriver()
{
	return D3DDevAkt;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění rozhraní

double FGetD3DInterface()
{
	return D3DIntAkt;
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření rotačního objektu

double FD3DRotary()
{
// načtení textu definice
	CString text;
	FText(text);

#define MAX_ROWS 10000

// načtení definice bodů
	int lin = 0;					// ukazatel řádku textu
	CString line;					// jeden řádek textu
	genRows = 0;					// čítač řádků s body
	genY = (double*)MemGet(MAX_ROWS * sizeof(double));	// buffer souřadnice Y
	genR = (double*)MemGet(MAX_ROWS * sizeof(double));	// buffer poloměru bodu

	for (int i = text.LineNum(); i > 0; i--)
	{

// načtení jednoho řádku textu
		line = text.GetLine(lin);		// jeden řádek z textu
		lin++;

		line.TrimLeft();
		line.TrimRight();
		if (line.IsNotEmpty())
		{

// načtení souřadnice Y
			genY[genRows] = Double(line);

// načtení poloměru bodu
			int pos = line.Find(' ');
			if (pos < 0) pos = line.Find(';');
			if (pos < 0) pos = line.Find(9);

			genR[genRows] = 0;

			if (pos >= 0)
			{
				line.Delete(0, pos + 1);
				line.TrimLeft();
				line.TrimRight();

				if (line.IsNotEmpty())
				{
					genR[genRows] = Double(line);
				}
			}

// zvýšení čítače řádků bodů
			genRows++;
			if (genRows == MAX_ROWS) break;
		}
	}

// vytvoření objektu
	GenRot(false);

	return D3D_ID;
}

/////////////////////////////////////////////////////////////////////////////
// generátor terénu (zruší globální buffery)

//#define MAXVERT 0xf000 // MaxVertexNum

int		terWidth;				// šířka terénu (políček)
int		terHeight;				// výška terénu (políček)
float*	terMap;					// mapa terénu (width*height hodnot 0 až 1),
								// pořadí zleva doprava a zdola nahoru,
								// (levý sloupec a poslední řada se zduplikují)

void GenTerrain()
{
	int i,j;

// omezení velikosti terénu
	i = terWidth * terHeight * 4;
	int korwidth = terWidth;
	int korheight = terHeight;

//	if (i > MAXVERT)
//	{
//		korwidth = RoundM(sqrt(MAXVERT/4));
//		if (korwidth > terWidth) korwidth = terWidth;
//		korheight = MAXVERT/4/korwidth;
//		ASSERT((korwidth * korheight * 4) <= MAXVERT);
//	}

// vytvoření bufferu výškové mapy (s duplikací okrajů)
	float* termap = (float*)MemGet((korwidth+1)*(korheight+1)*sizeof(float));
	float* ter = termap + korwidth + 1;
	float* tersrc = terMap;

	for (i = korheight; i > 0; i--)
	{
		MemCopy(ter, tersrc, korwidth*sizeof(float));
		ter += (korwidth+1);
		ter[-1] = tersrc[0];
		tersrc += terWidth;
	}
	MemCopy(termap, ter - korwidth - 1, (korwidth+1)*sizeof(float));

	terWidth = korwidth;
	terHeight = korheight;
	MemFree(terMap);

// vytvoření bufferu vektorů vrcholů (od dolní řady nahoru, zleva doprava, každé políčko má 4 vrcholy)
	int vertN = terWidth*terHeight*4;
	D3DVECTOR* vert = (D3DVECTOR*)MemGet(vertN*sizeof(D3DVECTOR));
	D3DVECTOR* v = vert;
	ter = termap;

	double z = -0.5;
	double dz = 1/(double)terHeight;
	double dx = 1/(double)terWidth;

	for (i = terHeight; i > 0; i--)
	{
		double x = -0.5;
		double z2 = z + dz;

		for (j = terWidth; j > 0; j--)
		{

		// levý dolní roh
			v->x = (float)x;
			v->y = ter[0];
			v->z = (float)z;
			v++;

		// levý horní roh
			v->x = (float)x;
			v->y = ter[terWidth+1];
			v->z = (float)z2;
			v++;

			x += dx;

		// pravý horní roh
			v->x = (float)x;
			v->y = ter[terWidth+2];
			v->z = (float)z2;
			v++;

		// pravý dolní roh
			v->x = (float)x;
			v->y = ter[1];
			v->z = (float)z;
			v++;

			ter++;
		}

		ter++;
		z = z2;
	}

// vytvoření bufferu normál (stejná organizace jako vektor vrcholů)
	D3DVECTOR* norm = (D3DVECTOR*)MemGet(vertN * sizeof(D3DVECTOR));
	D3DVECTOR* n = norm;
	v = vert;

// Normála je násobek vektorů: A x B = {Ay*Bz-Az*By,Az*Bx-Ax*Bz,Ax*By-Ay*Bx)
// pro A={dx,dxn,0} a B={0,dzn,dz} je N={dz*dxn,-dz*dx,dzn*dx}
// Vektory nejsou normalizované - provede se při jejich uložení.

	float dy = (float)(dz * dx);

	for (i = terHeight; i > 0; i--)
	{
		for (j = terWidth; j > 0; j--)
		{
			double dx1 = (v[0].y - v[3].y);		// rozdíl dole
			double dx2 = (v[1].y - v[2].y);		// rozdíl nahoře

			double dz1 = (v[0].y - v[1].y);		// rozdíl vlevo
			double dz2 = (v[3].y - v[2].y);		// rozdíl vpravo

		// levý dolní roh
			n->x = (float)(dz*dx1);
			n->y = dy;
			n->z = (float)(dz1*dx);
			n++;

		// levý horní roh
			n->x = (float)(dz*dx2);
			n->y = dy;
			n->z = (float)(dz1*dx);
			n++;

		// pravý horní roh
			n->x = (float)(dz*dx2);
			n->y = dy;
			n->z = (float)(dz2*dx);
			n++;

		// pravý dolní roh
			n->x = (float)(dz*dx1);
			n->y = dy;
			n->z = (float)(dz2*dx);
			n++;

			v += 4;
		}
	}

// vyhlazení normál
	n = norm;

	int down = (terHeight-1)*terWidth*4;

	for (i = terHeight; i > 0; i--)
	{
		int left = 4*(terWidth-1);

		for (j = terWidth; j > 0; j--)
		{
			float x = (float)((n[0].x + n[left+3].x)/2);
			n[0].x = x;
			n[left+3].x = x;
			n[down+1].x = x;
			n[left+down+2].x = x;

			float z = (float)((n[0].z + n[down+1].z)/2);
			n[0].z = z;
			n[left+3].z = z;
			n[down+1].z = z;
			n[left+down+2].z = z;

			n += 4;

			left = -4;
		}

		down = -4*terWidth;
	}	 

// vytvoření bufferu implicitního mapování textury
	float* text = (float*)MemGet(vertN * 2 * sizeof(float));
	float* t = text;

	float v1 = 1;
	int vi = 4;

	for (i = terHeight; i > 0; i--)
	{
		float v2 = (float)(v1 - 0.25);
		if (vi == 1) v2 = 0;

		float u = 0;
		int ui = 4;

		for (j = terWidth; j > 0; j--)
		{
		// levý dolní roh
			*t = u;
			t++;
			*t = v1;
			t++;

		// levý horní roh
			*t = u;
			t++;
			*t = v2;
			t++;

			u += 0.25;

		// pravý horní roh
			*t = u;
			t++;
			*t = v2;
			t++;

		// pravý dolní roh
			*t = u;
			t++;
			*t = v1;
			t++;

			ui--;
			if (ui == 0)
			{
				u = 0;
				ui = 4;
			}
		}

		v1 = v2;
		vi--;
		if (vi == 0)
		{
			v1 = 1;
			vi = 4;
		}
	}

// vytvoření plošek (zleva doprava, zdola nahoru)
// orientace rohů políčka:
//		1    2
//      0    3

	int faceN = terHeight*terWidth*2;
	int* face = (int*)MemGet(faceN * 3 *sizeof(int));

	int* f = face;
	int inx = 0;

	for (i = terHeight*terWidth; i > 0; i--)
	{
	// 1. trojúhleník (vrcholy 0-1-2)
		*f = inx;
		f++;
		*f = inx + 1;
		f++;
		*f = inx + 2;
		f++;

	// 2. trojúhelník (vrcholy 0-2-3)
		*f = inx;
		f++;
		*f = inx + 2;
		f++;
		*f = inx + 3;
		f++;

		inx += 4;
	}

// vytvoření objektu
	int index = D3DF_CreateMesh(vertN, vert, faceN, face);
	D3DF_SetNormal(index, norm);
	D3DF_SetTextUV(index, text, 0);
	D3D_ID = index;
	TerenID = index;
	D3DFITEM* item = GetID();
	item->IsTerrain = true;
	item->TerWidth = terWidth;
	item->TerHeight = terHeight;
	item->TerMap = termap;
	D3DF_SetAddress(D3D_ID, D3DTADDRESS_WRAP, D3DTADDRESS_WRAP, 0);
	D3DF_KorRightHand();

// zrušení bufferů
	MemFree(vert);
	MemFree(face);
	MemFree(norm);
	MemFree(text);
}

/////////////////////////////////////////////////////////////////////////////
// popisovač souseda ikony

typedef struct _ICONNEIGH
{
	int		icon;			// index ikony
	int		smer;			// offset souseda (0 až 7)
	int		neigh;			// index souseda
	int		counter;		// čítač výskytu
} ICONNEIGH;

// pomocná tabulka směrů X
const int smerX[8] =
{
	1,						// 0: doprava
	1,						// 1: doprava o řádek zpět (dolů)
	0,						// 2: o řádek zpět
	-1,						// 3: doleva o řádek zpět (dolů)
	-1,						// 4: doleva
	-1,						// 5: doleva o řádek vpřed (nahoru)
	0,						// 6: o řádek vpřed
	1						// 7: doprava o řádek vpřed (nahoru)
};

// pomocná tabulka směrů Y
const int smerY[8] =
{
	0,						// 0: doprava
	-1,						// 1: doprava o řádek zpět (dolů)
	-1,						// 2: o řádek zpět
	-1,						// 3: doleva o řádek zpět (dolů)
	0,						// 4: doleva
	1,						// 5: doleva o řádek vpřed (nahoru)
	1,						// 6: o řádek vpřed
	1						// 7: doprava o řádek vpřed (nahoru)
};

#define ICONBORD	16							// šířka přídavného pásu kolem ikon (v bodech)
#define ICONSIZEB	(ICONWIDTH + 2*ICONBORD)	// šířka a výška ikony s přídavným pásem

/////////////////////////////////////////////////////////////////////////////
// vytvoření terénu z plochy

double FD3DTerrainMap()
{

// načtení plochy
	CMap map;
	FMap(map);

// příprava parametrů plochy
	terWidth = map.Width();
	terHeight = map.Height();
	int size = terWidth * terHeight;
	int i,j;

// příprava bufferu pro zmapování ikon a výšek
	CIcon* ico = (CIcon*)MemGet(size * sizeof(CIcon));
	int* icomap = (int*)MemGet(size * sizeof(int));
	int icons = 0;

// zmapování ikon
	CIcon ico0;

	for (i = 0; i < size; i++)
	{
		ico0 = map[i].Icon;

		for (j = 0; j < icons; j++)
		{
			if (ico0 == ico[j])
			{
				break;
			}
		}

		if (j == icons)
		{
			ico[j].Init(ico0.Data());
			ico[j].DeComp();
			icons++;
		}
		icomap[i] = j;
	}

// vyhledání četností sousedů ikon
	ICONNEIGH* iconneigh = (ICONNEIGH*)MemGet(8 * size * sizeof(ICONNEIGH));
	int neighN = 0;

	int x = 0;
	int y = 0;

	for (i = 0; i < size; i++)
	{
		int icoinx = icomap[i];			// středová ikona
		
		for (j = 0; j < 8; j++)			// cyklus přes všechny sousedy
		{
			int x2 = x + smerX[j];		// souřadnice X souseda
			if (x2 < 0) x2 = terWidth - 1;
			if (x2 >= terWidth) x2 = 0;

			int y2 = y + smerY[j];		// souřadnice Y souseda
			if (y2 < 0) y2 = terHeight - 1;
			if (y2 >= terHeight) y2 = 0;

			int neighinx = icomap[x2 + y2*terWidth]; // soused

			ICONNEIGH* item = iconneigh;

			for (int k = 0; k < neighN; k++)
			{
				if ((item->icon == icoinx) &&
					(item->smer == j) &&
					(item->neigh == neighinx))
				{
					item->counter++;
					break;
				}		  
				item++;
			}

			if (k == neighN)
			{
				item->icon = icoinx;
				item->smer = j;
				item->neigh = neighinx;
				item->counter = 1;
				neighN++;
			}
		}

		x++;
		if (x == terWidth)
		{
			x = 0;
			y++;
		}
	}

// vyhledání nejčastějších sousedů (tabulka je po 8 položkách indexů sousedů)
	int* neighTab = (int*)MemGet(icons * 8 * sizeof(int));
	int* neighP = neighTab;

	for (i = 0; i < icons; i++)
	{
		for (j = 0; j < 8; j++)
		{
			int count = 0;

			ICONNEIGH* item = iconneigh;

			for (int k = 0; k < neighN; k++)
			{
				if ((item->icon == i) &&
					(item->smer == j) &&
					(item->counter > count))
				{
					count = item->counter;
					*neighP = item->neigh;
				}	
				item++;
			}

			ASSERT(count > 0);		// (to by byla chyba v této proceduře)
			neighP++;
		}
	}

// příprava textury
	int icopicWH = 32;
	int icoWH;

	do
	{
		icopicWH = (icopicWH << 1);
		icoWH = icopicWH/ICONSIZEB;

	} while (icoWH*icoWH < icons);

	CPicture pic(icopicWH, icopicWH);
	BYTE* picdata = pic.DataData();
	MemFill(picdata, icopicWH * icopicWH, BlackCol);

// dekódování ikon textury (od horní řady dolů)
	int radek = 1;
	int pozice = 0;

	for (i = 0; i < icons; i++)
	{
		BYTE* dst0 = picdata + (icopicWH - radek*ICONSIZEB + ICONBORD)*icopicWH + pozice*ICONSIZEB + ICONBORD;

		ico[i].DeComp();
		BYTE* src = ico[i].DataData();

	// přenesení ikony
		BYTE* dst = dst0; 
		for (j = ICONHEIGHT; j > 0; j--)
		{
			MemCopy(dst, src, ICONWIDTH);
			src += ICONWIDTH;
			dst += icopicWH;
		}

	// přenesení sousedů
		for (j = 0; j < 8; j++)
		{
			src = ico[neighTab[8*i + j]].DataData();
			int width = ICONWIDTH;
			int height = ICONHEIGHT;
			dst = dst0;

		// vlevo
			if (smerX[j] < 0)
			{
				src += (ICONWIDTH-ICONBORD);
				dst -= ICONBORD;
				width = ICONBORD;
			}

		// vpravo
			if (smerX[j] > 0)
			{
				dst += ICONWIDTH;
				width = ICONBORD;
			}

		// zpět (dolů)
			if (smerY[j] < 0)
			{
				src += ((ICONHEIGHT-ICONBORD)*ICONWIDTH);
				dst -= (ICONBORD*icopicWH);
				height = ICONBORD;
			}

		// vpřed (nahoru)
			if (smerY[j] > 0)
			{
				height = ICONBORD;
				dst += (ICONHEIGHT*icopicWH);
			}

			for (int k = height; k > 0; k--)
			{
				MemCopy(dst, src, width);
				src += ICONWIDTH;
				dst += icopicWH;
			}
		}

	// zvýšení ukazatele pozice
		pozice++;
		if (pozice == icoWH)
		{
			pozice = 0;
			radek++;
		}
	}

// vytvoření bufferu výškové mapy (od dolní řady nahoru)
	float* ter = (float*)MemGet(size*sizeof(float));
	terMap = ter;

	for (i = 0; i < size; i++)
	{
		*ter = (float)((double)((map[i].Param & MAP_VALUE1_MASK) >> MAP_VALUE1_ROT)/1000);
		ter++;
	}

// zrušení bufferů
	for (i = 0; i < icons; i++)
	{
		ico[i].Term();
	}

	MemFree(ico);
	MemFree(neighTab);
	MemFree(iconneigh);

// vytvoření objektu
	int oldwidth = terWidth;

	GenTerrain();

	int newwidth = terWidth;
	int newheight = terHeight;
	int newsize = newwidth * newheight;
	size = oldwidth * newheight;
	oldwidth -= newwidth;

// vytvoření bufferu mapování textur
	float* t = (float*)MemGet(newsize*2*4*sizeof(float));
	float* tp = t;

	int xx = newwidth;

	for (i = 0; i < size; i++)
	{
		j = icomap[i];

		float u = (float)((double)((j % icoWH)*ICONSIZEB + ICONBORD)/icopicWH);
		float u2 = (float)((double)((j % icoWH + 1)*ICONSIZEB - ICONBORD)/icopicWH);

		float v = (float)((double)((j / icoWH)*ICONSIZEB + ICONBORD)/icopicWH);
		float v2 = (float)((double)((j / icoWH + 1)*ICONSIZEB - ICONBORD)/icopicWH);

		// vlevo dole
		*tp = u;
		tp++;
		*tp = v2;
		tp++;

		// vlevo nahoře
		*tp = u;
		tp++;
		*tp = v;
		tp++;

		// vpravo nahoře
		*tp = u2;
		tp++;
		*tp = v;
		tp++;

		// vpravo dole
		*tp = u2;
		tp++;
		*tp = v2;
		tp++;

		xx--;
		if (xx == 0)
		{
			xx = newwidth;
			i += oldwidth;
		}
	}

// nastavení mapování textur
	D3DF_SetTextUV(D3D_ID, t, 0);

// zrušení bufferů
	MemFree(t);
	MemFree(icomap);

// nastavení opakování textury
	D3DF_SetAddress(D3D_ID, D3DTADDRESS_WRAP, D3DTADDRESS_WRAP, 0);

// nastavení textury
	D3DFITEM* item = GetID();
	item->MatSource = MATSOURCE_FRAME;

	if (pic.IsNotEmpty())
	{
		item->Texture[0] = D3DT_Add(pic);
		D3DT_Get(item->Texture[0])->MipMaps = 4;
//		item->AktMat = true;
	}

	return D3D_ID;
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření terénu z obrázku

double FD3D2DTerrain()
{

// načtení obrázku s terénem
	CPicture pic;
	FPicture(pic);

// příprava parametrů obrázku
	pic.DeComp();
	int width = pic.Width();
	int height = pic.Height();

#define OKRAJ	4			// přídavný okraj

// pracovní obrázek s většími rozměry
	int korig = OKRAJ; //Round(OKRAJ*D3DSmooth);		 	// přidané body
	int widthnew = width + 2*korig;
	int heightnew = height + 2*korig;
	CPicture pic2(widthnew, heightnew);

// kopie původního obrázku (s duplikací okrajů)
	BYTE* dst = pic2.DataData() + korig*widthnew;
	BYTE* src = pic.DataData();
	int i;

	for (i = height; i > 0; i--)
	{
		MemCopy(dst, src + width - korig, korig);
		dst += korig;
		MemCopy(dst, src, width);
		dst += width;
		MemCopy(dst, src, korig);
		dst += korig;
		src += width;
	}

	dst = pic2.DataData();
	MemCopy(dst, dst + height*widthnew, korig*widthnew);
	MemCopy(dst + (korig+height)*widthnew, dst + korig*widthnew, korig*widthnew);

// konverze dat na TrueColor
	BYTE* buf = pic2.ExportTrueAlphaBeta();

// konverze obrázku na zmenšenou velikost
	int widthnew2 = Round(widthnew / D3DSmooth); // / D3DSmooth);
	if (widthnew2 < 2) widthnew = 2;
//	if (widthnew2 < ((2*OKRAJ+2)/2)) widthnew2 = ((2*OKRAJ+2)/2);
	int heightnew2 = Round(heightnew / D3DSmooth); // / D3DSmooth);
	if (heightnew2 < 2) heightnew2 = 2;
//	if (heightnew2 < ((2*OKRAJ+2)/2)) heightnew2 = ((2*OKRAJ+2)/2);

	buf = ZoomTrueColor(buf, widthnew, heightnew, widthnew2, heightnew2);

// konverze obrázku na správnou velikost
//	widthnew = Round(widthnew / D3DSmooth);
//	if (widthnew < (2*OKRAJ+2)) widthnew = (2*OKRAJ+2);
//	heightnew = Round(heightnew / D3DSmooth);
//	if (heightnew < (2*OKRAJ+2)) heightnew = (2*OKRAJ+2);

	buf = ZoomTrueColor(buf, widthnew2, heightnew2, widthnew, heightnew);

//	width = widthnew - 2*OKRAJ;
//	height = heightnew - 2*OKRAJ;

// konverze obrázku na výškové body
	float* ter = (float*)MemGet(width*height*sizeof(float));
	terMap = ter;

	src = buf + 5*OKRAJ*(widthnew+1);

	for (i = height; i > 0; i--)
	{
		for (int j = width; j > 0; j--)
		{
			*ter = (float)((double)(src[0] + src[1] + src[2])/(3*255));
			ter++;
			src += 5;
		}

		src += 5*2*OKRAJ;
	}

	terWidth = width;
	terHeight = height;

// zrušení bufferu
	MemFree(buf);

// vytvoření objektu
	GenTerrain();

// nastavení opakování textury
	D3DF_SetAddress(D3D_ID, D3DTADDRESS_WRAP, D3DTADDRESS_WRAP, 0);

	return D3D_ID;
}


/////////////////////////////////////////////////////////////////////////////
// zjištění úrovně terénu

double FD3DLevel()
{

// načtení horizontální souřadnice
	double x = FNum();
	if (x == 0x1000000)
	{
		x = D3DF_Get(1)->TransX;
	}

// načtení vertikální souřadnice
	double y = FNum();
	if (y == 0x1000000)
	{
		y = D3DF_Get(1)->TransZ;
	}

// aktuální terén
	double h = 0;

	if (D3DF_IsValid(TerenID))
	{
		D3DFITEM* frame = D3DF_Get(TerenID);
		
// test, zda to je terén
		if (frame->IsTerrain &&
			(frame->TerMap != NULL))
		{

// odečtení souřadnice X a Z
			x -= frame->TransX;
			y -= frame->TransZ;

// korekce měřítka X a Z
			x /= frame->ScaleX;
			y /= frame->ScaleZ;

// modulování souřadnic
			x = x + 0.5;
			y = y + 0.5;

			if (x < 0)
			{
				x = 1 + (x + Round0(-x));
			}
			else
			{
				if (x > 1) x -= RoundM(x);
			}
			ASSERT(x >= 0);
			ASSERT(x <= 1);
			if (x < 0) x = 0;

			if (y < 0)
			{
				y = 1 + (y + Round0(-y));
			}
			else
			{
				if (y > 1) y -= RoundM(y);
			}
			ASSERT(y >= 0);
			ASSERT(y <= 1);
			if (y < 0) y = 0;

// přepočet na bodové souřadnice
			x = x * frame->TerWidth;
			y = y * frame->TerHeight;

// souřadnice počátečního bodu
			int x0 = Round0(x);
			double dx = x - x0;
			if (x0 >= frame->TerWidth)
			{
				x0 = frame->TerWidth - 1;
				dx = 1;
			}

			int y0 = Round0(y);
			double dy = y - y0;
			if (y0 >= frame->TerHeight)
			{
				y0 = frame->TerHeight - 1;
				dy = 1;
			}

// zjištění 4 bodů
			int ww = frame->TerWidth + 1;

			float* t = frame->TerMap + y0*ww + x0;
			float n1 = t[0];	// 1: vlevo dole
			float n2 = t[1];	// 2: vpravo dole
			float n3 = t[ww];	// 3: vlevo nahoře
			float n4 = t[ww+1];	// 4: vpravo nahoře

// výpočet výšky (trojúhelník 1-4-2 a 1-3-4)
			if (dx > dy)
			{
				h = n1 + (n2 - n1)*dx + (n4 - n2)*dy;
			}
			else
			{
				h = n1 + (n3 - n1)*dy + (n4 - n3)*dx;
			}

// vynásobení měřítkem Y
			h *= frame->ScaleY;

// přičtení souřadnice Y
			h += frame->TransY;
		}
	}
	return h;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění vyhlazení textur

double FGetD3DSmooth()
{
	return D3DSmooth;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění průhlednosti objektu Direct3D

double FGetD3DTransparent()
{
	if (IsValidID())
	{
		return (double)(GetID()->SrcDstBlend);
	}
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění alfa úrovně Direct3D

double FGetD3DAlphaRef()
{
	if (IsValidID())
	{
		return (GetID()->AlphaRef);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// uplynulý přesný čas 

double FD3DElapsed0()
{
	return D3DElapsedTime;
}

/////////////////////////////////////////////////////////////////////////////
// uplynulý přesný čas 

double FD3DElapsed()
{	  
	double n = FNum();
	return (D3DElapsedTime*n);
}

/////////////////////////////////////////////////////////////////////////////
// rychlost renderování

double FD3DSpeed()
{
	return D3DAverageFreqI;
}

/////////////////////////////////////////////////////////////////////////////
// volná videopaměť

double FD3DFree()
{
	FPUFloat();
	double n = pD3Free();
	FPUDouble();

	return n;
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření 2D obrázku

double FD3DPicture()
{
	CPicture pic;
	FPicture(pic);

	int inx = D3DF_CreateMesh(4, wall_vertices, 2, wall_faces);
	D3DF_SetNormal(inx, wall_normals);
	D3DF_SetTextUV(inx, wall_text, 0);
	D3D_ID = inx;

	D3DFITEM* item = GetID();
	item->Type = D3DFTYPE_PICTURE;
	item->MatSource = MATSOURCE_FRAME;
	D3DF_SetAddress(inx, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP, 0);
	D3DF_KorRightHand();

	if (pic.IsNotEmpty())
	{
		item->Texture[0] = D3DT_Add(pic);
	}

	if ((D3DWidth != 0) && (D3DHeight != 0))
	{
		item->ScaleX = (double)pic.Width()/D3DWidth/D3DSmooth;
		item->ScaleY = (double)pic.Height()/D3DHeight/D3DSmooth;
	}

	item->Lighton = false;
	item->Shades = false;
	item->MagFilter[0] = false;
	item->MinFilter[0] = false;
	item->MipFilter[0] = false;

	item->AlphaRef = 0.5;
	item->AlphaRef2 = 0x80;
	item->IsAlphaRef = true;

	item->RendGroup = RENDPIC;
	item->ZTest = D3DCMP_ALWAYS;
	item->ZWrite = FALSE;

// připojení ke kameře
	D3DF_Link(1, D3D_ID);

	return D3D_ID;
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření čočkového odlesku

double FD3DLensFlare()
{
	CPicture pic;
	FPicture(pic);

	int inx = D3DF_CreateMesh(4, wall_vertices, 2, wall_faces);
	D3DF_SetNormal(inx, wall_normals);
	D3DF_SetTextUV(inx, wall_text, 0);
	D3D_ID = inx;

	D3DFITEM* item = GetID();
	item->Type = D3DFTYPE_LENSFLARE;
	item->MatSource = MATSOURCE_FRAME;
	D3DF_SetAddress(inx, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP, 0);
	D3DF_KorRightHand();

	if (pic.IsNotEmpty())
	{
		item->Texture[0] = D3DT_Add(pic);
	}

	if ((D3DWidth != 0) && (D3DHeight != 0))
	{
		item->ScaleX = (double)pic.Width()/D3DWidth/D3DSmooth;
		item->ScaleY = (double)pic.Height()/D3DHeight/D3DSmooth;
	}

	item->Lighton = false;
	item->Shades = false;
	item->MagFilter[0] = true;
	item->MinFilter[0] = true;
	item->MipFilter[0] = true;

	item->AlphaRef = 0;
	item->AlphaRef2 = 0;
	item->IsAlphaRef = false;

	item->IsBlend = true;
	item->BlendRend = true;
	item->SrcBlend = BLEND_ONE;
	item->DstBlend = BLEND_ONE;
	item->SrcDstBlend = 11;

	item->RendGroup = RENDPIC;
	item->ZTest = D3DCMP_ALWAYS;
	item->ZWrite = FALSE;

	return D3D_ID;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění počtu stupňů textur

double FGetD3DStage()
{
	return (double)MaxTextureSimult;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění operace barev

double FGetD3DColorOp()
{
	if (IsValidID())
	{
		return GetID()->ColorOp[D3DAktStage];
	}
	return D3DTOP_DISABLE;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění argumentu 1 barev

double FGetD3DColorArg1()
{
	if (IsValidID())
	{
		return GetID()->ColorArg1[D3DAktStage];
	}
	return D3DTA_TEXTURE;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění argumentu 2 barev

double FGetD3DColorArg2()
{
	if (IsValidID())
	{
		return GetID()->ColorArg2[D3DAktStage];
	}
	return D3DTA_CURRENT;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění operace alfa

double FGetD3DAlphaOp()
{
	if (IsValidID())
	{
		return GetID()->AlphaOp[D3DAktStage];
	}
	return D3DTOP_DISABLE;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění argumentu 1 alfa

double FGetD3DAlphaArg1()
{
	if (IsValidID())
	{
		return GetID()->AlphaArg1[D3DAktStage];
	}
	return D3DTA_TEXTURE;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění argumentu 2 alfa

double FGetD3DAlphaArg2()
{
	if (IsValidID())
	{
		return GetID()->AlphaArg2[D3DAktStage];
	}
	return D3DTA_CURRENT;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění horizontálního adresování

double FGetD3DAddressU()
{
	if (IsValidID())
	{
		return GetID()->AddressU[D3DAktStage];
	}
	return D3DTADDRESS_WRAP;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění vertikálního adresování

double FGetD3DAddressV()
{
	if (IsValidID())
	{
		return GetID()->AddressV[D3DAktStage];
	}
	return D3DTADDRESS_WRAP;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění barvy okolí textury
/*
double FGetD3DBorder()
{
	if (IsValidID())
	{
		return GetID()->Border[D3DAktStage];
	}
	return 0;
}
*/
/////////////////////////////////////////////////////////////////////////////
// zjištění zjemnění vzdálených textur

double FGetD3DBias()
{
	if (IsValidID())
	{
		return GetID()->Bias[D3DAktStage];
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění barvy faktoru textur

double FGetD3DTFactor()
{
	if (IsValidID())
	{
		return GetID()->TFactor;
	}
	return 0xffffff;
}

/////////////////////////////////////////////////////////////////////////////
// počet úrovní mipmaps

double FGetD3DMipMaps()
{
	if (IsValidID())
	{
		D3DFITEM* item = GetID();

		int text = item->Texture[D3DAktStage];

		if (D3DT_IsValid(text))
		{
			return D3DT_Get(text)->MipMaps;
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění renderovací skupiny

double FGetD3DZGroup()
{
	if (IsValidID())
	{
		return (double)(GetID()->RendGroup);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění hloubkového testu

double FGetD3DZTest()
{
	if (IsValidID())
	{
		return (double)(GetID()->ZTest);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění dosahu stínu

double FGetD3DShadowRange()
{
	if (IsValidID())
	{
		return (double)(GetID()->ShadowRange);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění objektu pro LOD

double FGetD3DLODObject()
{
	if (IsValidID())
	{
		return (double)(GetID()->LODNext);
	}
	return -1;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění vzdálenosti pro LOD

double FGetD3DLODDistance()
{
	if (IsValidID())
	{
		return (GetID()->LODMax);
	}
	return 1e15;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění max. stupně morfování

double FGetD3DMorphModel()
{
	if (IsValidID())
	{
		return (GetID()->MorphMax);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// zjištění stupně morfování

double FGetD3DMorphLevel()
{
	if (IsValidID())
	{
		return (GetID()->MorphStage);
	}
	return 0;
}


#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------


/////////////////////////////////////////////////////////////////////////////
// výpočet vzdálenosti bodu

double FPolarRadius()
{
	double x = FNum();
	double y = FNum();

	double n = sqrt(x*x + y*y);

	return n;
}

/////////////////////////////////////////////////////////////////////////////
// výpočet úhlu bodu

double FPolarAngle()
{
	double x = FNum();
	double y = FNum();

	double n = atan2(y, x);
	if (n < 0) n = pi2 + n;

	return n;
}

/////////////////////////////////////////////////////////////////////////////
// přesný čítač času

bool PrecTimerInit = false;			// proběhla inicializace přesného času

// přesný čítač času
bool IsPerformanceTimer = false;	// je přesný čítač času
double PerfFrequency = 0;			// frekvence přesného čítače času
__int64 PerfCounter = 0;			// načtená hodnota přesného čítače času

// multimediální čítač
double OldPrecTimer = 0;			// stará uschovaná hodnota čítače
double AddPrecTimer = 0;			// přidaná hodnota k čítači (přetečení po 49 dnech)

double FPreciseTimer()
{
// zjištění, zda je přesný měřič času
	if (!PrecTimerInit)
	{
		PrecTimerInit = true;

		if (::QueryPerformanceFrequency((LARGE_INTEGER*)&PerfCounter) &&
			(PerfCounter > (__int64)1000))
		{
			PerfFrequency = (double)PerfCounter;

			IsPerformanceTimer = true;
		}
	}

// obsluha pomocí čítače přesného času
	if (IsPerformanceTimer)
	{
		if (::QueryPerformanceCounter((LARGE_INTEGER*)&PerfCounter))
		{
			return (double)PerfCounter / PerfFrequency;
		}
	}

// načtení multimediálního čítače
	double time = (double)(DWORD)::timeGetTime() * 0.001;
	double newtime;

// korekce při přetečení měřící periody (49.71 dnů)
	for (;;)
	{
		newtime = time + AddPrecTimer;
		if (newtime >= OldPrecTimer) break;
		AddPrecTimer = AddPrecTimer + (double)4294967296;
	}
	OldPrecTimer = newtime;

	return newtime;
}

/***************************************************************************\
*																			*
*								obsluha DLL									*
*																			*
\***************************************************************************/

// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI

/////////////////////////////////////////////////////////////////////////////
// načtení DLL knihovny

double FDLLLoad()
{
// načtení jména knihovny
	CString jmeno;
	FText(jmeno);

// načtení knihovny
	HINSTANCE inst = ::LoadLibrary(jmeno);
	
	return (double)(int)inst;
}


/////////////////////////////////////////////////////////////////////////////
// importování DLL funkce

double FDLLImport()
{
// načtení handle knihovny
	int module = FIntR();

// načtení jména funkce
	CString jmeno;
	FText(jmeno);

// načtení počtu parametrů
	int param = FIntR();

// načtení příznaku volací konvence C
	bool conv = FBool();

// kontrola zadání parametrů
	if ((module == 0) || jmeno.IsEmpty() || (param < 0) || (param > 0xffff)) return 0;

// importování funkce
	FARPROC proc = ::GetProcAddress((HMODULE)module, (LPCSTR)jmeno);

// při neúspěchu doplnění o písmeno "A"
	if (proc == NULL)
	{
		jmeno += 'A';
		proc = ::GetProcAddress((HMODULE)module, (LPCSTR)jmeno);
	}

// při neúspěchu druhý pokus - vyhledání číslem
/*
	if (proc == NULL)
	{
		if ((jmeno.Length() > 1) && (jmeno[0] == '#') &&
			((jmeno[1] == '$') ||
			((jmeno[1] >= '0') && (jmeno[1] <= '9'))))
		{
			int n = Int((LPCTSTR)(CString)jmeno.Mid(1));

			proc = ::GetProcAddress((HMODULE)module, (LPCSTR)n);
		}
	}
*/
// kontrola platnosti funkce
	if (proc == NULL) return 0;

// vytvoření bloku parametrů
	int* blok = (int*)MemGet(param*4 + 8);

// inicializace bloku parametrů
	blok[0] = (int)proc;
	blok[1] = param;
	if (conv) blok[1] |= 0x10000;
	MemFill(blok + 2, param * 4, 0);

	return (double)(int)blok;
}

/////////////////////////////////////////////////////////////////////////////
// provedení DLL funkce

double FDLLExecNum()
{
// načtení bloku parametrů funkce
	int* blok = (int*)FIntR();

	int	retcode = 0;

#ifndef _M_IX86
	nepodporovaný překlad!!!!	
#endif

_asm {
		push	ecx

		mov		esi,[blok]			// bloku parametrů
		or		esi,esi				// jsou platná data?
		jz		pasconv				// není platný blok dat
		lods	dword ptr [esi]		// adresa funkce

		xor		ecx,ecx
		mov		cx,[esi]			// počet parametrů
		shl		ecx,2				// délka dat v bajtech
		sub		esp,ecx				// vytvoření místa pro parametry
		mov		edi,esp
		mov		cx,[esi]			// počet parametrů
		add		esi,4				// začátek parametrů v bloku
		rep		movs dword ptr [edi], dword ptr [esi]

		call	eax					// volání funkce
		mov		[retcode],eax		// návratový kód

		mov		esi,[blok]			// blok parametrů
		test	byte ptr [esi+6],1	// je volací konvence C ?
		jz		pasconv				// není C, je Pascal

		xor		eax,eax
		mov		ax,[esi+4]			// EAX <- počet parametrů
		shl		eax,2				// počet bajtů
		add		esp,eax				// návrat ukazatele zásobníku

pasconv:
		pop		ecx
	}

	return retcode;
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření bloku paměti

double FMemoryNew()
{
// načtení velikosti bloku
	int size = FIntR();

// vytvoření paměťového bloku
	void* blok = MemGet(size);

	return (double)(int)blok;
}

/////////////////////////////////////////////////////////////////////////////
// změna velikosti bloku paměti

double FMemoryResize()
{
// načtení staré adresy bloku
	void* blok = (void*)FIntR();

// načtení požadované velikosti bloku
	int size = FIntR();

// změna velikosti bloku
	blok = MemSize(blok, size);

	return (double)(int)blok;
}

/////////////////////////////////////////////////////////////////////////////
// ukazatel čtení z paměti

double FGetMemoryRead()
{
	return (double)(int)DLLMemoryRead;
}

/////////////////////////////////////////////////////////////////////////////
// ukazatel zápisu do paměti

double FGetMemoryWrite()
{
	return (double)(int)DLLMemoryWrite;
}

/////////////////////////////////////////////////////////////////////////////
// porovnání dat

double FMemoryCompare()
{
// načtení délky dat
	int len = FIntR();

// adresa dat
	char* buf1 = (char*)DLLMemoryRead;
	char* buf2 = (char*)DLLMemoryWrite;

// porovnání dat
	for (; ((len > 0) && (*buf1 == *buf2)); len--)
	{
		buf1++;
		buf2++;
	}

	DLLMemoryRead = buf1;
	DLLMemoryWrite = buf2;

	return (double)len;
}

/////////////////////////////////////////////////////////////////////////////
// bajt se znaménkem

double FGetMemoryByteS()
{
	char* buf = (char*)DLLMemoryRead;
	long data = (long)*buf;
	buf++;
	DLLMemoryRead = buf;
	return data;
}

/////////////////////////////////////////////////////////////////////////////
// bajt bez znaménka

double FGetMemoryByteU()
{
	BYTE* buf = (BYTE*)DLLMemoryRead;
	long data = (long)*buf;
	buf++;
	DLLMemoryRead = buf;
	return data;
}

/////////////////////////////////////////////////////////////////////////////
// slovo se znaménkem

double FGetMemoryWordS()
{
	short* buf = (short*)DLLMemoryRead;
	long data = (long)*buf;
	buf++;
	DLLMemoryRead = buf;
	return data;
}

/////////////////////////////////////////////////////////////////////////////
// slovo bez znaménka

double FGetMemoryWordU()
{
	WORD* buf = (WORD*)DLLMemoryRead;
	long data = (long)*buf;
	buf++;
	DLLMemoryRead = buf;
	return data;
}

/////////////////////////////////////////////////////////////////////////////
// 3-bajt se znaménkem

double FGetMemory3ByteS()
{
	char* buf = (char*)DLLMemoryRead;
	long data;
	*(WORD*)&data = *(WORD*)buf;
	((short*)&data)[1] = (short)buf[2];
	buf += 3;
	DLLMemoryRead = buf;
	return data;
}

/////////////////////////////////////////////////////////////////////////////
// 3-bajt bez znaménka

double FGetMemory3ByteU()
{
	BYTE* buf = (BYTE*)DLLMemoryRead;
	long data;
	*(WORD*)&data = *(WORD*)buf;
	((WORD*)&data)[1] = (WORD)buf[2];
	buf += 3;
	DLLMemoryRead = buf;
	return data;
}

/////////////////////////////////////////////////////////////////////////////
// dvouslovo se znaménkem

double FGetMemoryDWordS()
{
	long* buf = (long*)DLLMemoryRead;
	long data = *buf;
	buf++;
	DLLMemoryRead = buf;
	return data;
}

/////////////////////////////////////////////////////////////////////////////
// dvouslovo bez znaménka

double FGetMemoryDWordU()
{
	DWORD* buf = (DWORD*)DLLMemoryRead;
	DWORD data = *buf;
	buf++;
	DLLMemoryRead = buf;
	return data;
}

/////////////////////////////////////////////////////////////////////////////
// reálné číslo 4 bajty

double FGetMemoryFloat()
{
	float* buf = (float*)DLLMemoryRead;
	float data = *buf;
	buf++;
	DLLMemoryRead = buf;
	return data;
}

/////////////////////////////////////////////////////////////////////////////
// reálné číslo 8 bajtů

double FGetMemoryDouble()
{
	double* buf = (double*)DLLMemoryRead;
	double data = *buf;
	buf++;
	DLLMemoryRead = buf;
	return data;
}

/////////////////////////////////////////////////////////////////////////////
// délka textu s pevnou délkou

double FGetMemoryTextNLen()
{
	return DLLMemoryTextNLen;
}


/////////////////////////////////////////////////////////////////////////////
// handle okenního prvku

double FWindowHandle()
{
	int prvek = FIntR();

	if (Win.IsValid(prvek))
	{
		return (int)(Win[prvek].Wnd);
	}
	else
	{
		return 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření 3D objektu z paměti

double FMemory3D()
{
// načtení parametrů
	int vN = FIntR();						// počet vrcholů
	D3DVECTOR* vert = (D3DVECTOR*)FIntR();	// buffer vrcholů
	D3DVECTOR* norm = (D3DVECTOR*)FIntR();	// buffer normál
	float* text = (float*)FIntR();			// buffer textur
	D3DCOLORVALUE* col = (D3DCOLORVALUE*)FIntR(); // buffer barev
	int fN = FIntR();						// počet plošek
	int* face = (int*)FIntR();				// buffer plošek

// omezení indexů plošek
	for (int i = fN*3 - 1; i >= 0; i--)
	{
		if (face[i] >= vN) face[i] = 0;
	}

// vytvoření objektu
	int inx = D3DF_CreateMesh(vN, vert, fN, face);

// nastavení normál
	if (norm != NULL)
	{
		D3DF_SetNormal(inx, norm);
	}
	else
	{
		D3DF_AutoNormals(inx, 0.5);
	}

// nastavení mapování textur
	if (text != NULL)
	{
		D3DF_SetTextUV(inx, text, 0);
	}

// nastavení barev
	if (col != NULL)
	{
		D3DF_SetVertCol(inx, col);
	}

	D3D_ID = inx;
	D3DF_SetAddress(inx, D3DTADDRESS_WRAP, D3DTADDRESS_WRAP, 0);
	D3DF_KorRightHand();

	return D3D_ID;
}

/////////////////////////////////////////////////////////////////////////////
// vytvoření paměťového bloku z 3D objektu

double FD3DMemory()
{
	int inx = FIntR();

	if (D3DF_IsValid(inx))
	{
		D3DFITEM* item = D3DF_Get(inx);

		int* mem = (int*)MemGet(8*sizeof(int));

		mem[0] = 0;

		int vN = item->VertNum;
		mem[1] = vN;

		D3DVECTOR* v = (D3DVECTOR*)MemGet(vN * sizeof(D3DVECTOR));
		MemCopy(v, item->Vertex, vN * sizeof(D3DVECTOR));
		mem[2] = (int)v;

		D3DVECTOR* n = (D3DVECTOR*)MemGet(vN * sizeof(D3DVECTOR));
		MemCopy(n, item->Normal, vN * sizeof(D3DVECTOR));
		mem[3] = (int)n;

		float* tex = NULL;
		if (item->TextUV[0] != NULL)
		{
			tex = (float*)MemGet(vN * 2 * sizeof(float));
			MemCopy(tex, item->TextUV[0], vN * 2 * sizeof(float));
		}
		mem[4] = (int)tex;

		D3DCOLORVALUE* col = NULL;
		if (item->VertColF != NULL)
		{
			col = (D3DCOLORVALUE*)MemGet(vN * sizeof(D3DCOLORVALUE));
			MemCopy(col, item->VertColF, vN * sizeof(D3DCOLORVALUE));
		}
		mem[5] = (int)col;

		int fN = item->FaceNum;
		mem[6] = fN;

		int* face = (int*)MemGet(fN * 3 * sizeof(int));
		MemCopy(face, item->Face, fN * 3 * sizeof(int));
		mem[7] = (int)face;

		return (int)mem;
	}
	else
	{
		return 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// poskytnutí potomka 3D objektu

double FD3DChild0()
{
	if (IsValidID())
	{
		return (double)(GetID()->Child);
	}
	return -1;
}

double FD3DChild()
{
	int inx = FIntR();

// nalezení prvního potomka
	if (inx == -1)
	{
		return FD3DChild0();
	}

// nalezení rodiče
	if (inx == -2)
	{
		if (IsValidID())
		{
			return (double)(GetID()->Parent);
		}
		return -1;
	}

// nalezení dalšího potomka
	if (D3DF_IsValid(inx))
	{
		D3DFITEM* item = D3DF_Get(inx);
		return item->Next;
	}
	return -1;
}


/***************************************************************************\
*																			*
*								konfigurace									*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// čtení konfiguračního čísla

double FGetIniNum()
{
	int cislo = FIntR();
	CString text;
	text.Int(cislo);
	GetIniProfileString(text);
	return (double)Int((LPCTSTR)text);
}

double FGetIniNum0()
{
	CString text('0');
	GetIniProfileString(text);
	return (double)Int((LPCTSTR)text);
}

/////////////////////////////////////////////////////////////////////////////
// skupina klíčů registru

double FGetRegKey()
{
	return REGKey;
}

/////////////////////////////////////////////////////////////////////////////
// čtení čísla z registru

int GetRegNum(int cislo)
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
		DWORD bufsize = 4;

		::RegQueryValueEx(
			hkey,
			REGValue,
			0,
			NULL,
			(BYTE*)&cislo,
			&bufsize);

// uzavření klíče
		::RegCloseKey(hkey);
	}

	return cislo;
}

double FGetRegNum()
{
	int cislo = FIntR();
	return GetRegNum(cislo);
}

double FGetRegNum0()
{
	return GetRegNum(0);
}

#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------
