
/***************************************************************************\
*																			*
*								Obsluha paměti								*
*																			*
\***************************************************************************/

//////////////////////////////////////////////////////////////////////////////
// přepínače preprocesoru:
//	_DEBUG ............. je debugger
//	_OPTIM ............. je optimalizace
//	_MT ................ vícetokový režim
//	_UNICODE ........... kódování znaků UNICODE
//
//	_M_IX86 ............ procesor Intel 86
//	_M_ALPHA ........... procesor DEC Alpha
//	_M_MPPC ............ procesor Power Macintosh
//	_M_MRX000 .......... procesor MIPS
//	_M_PPC ............. procesor PowerPC


extern BOOL MemoryError;		// příznak chyby paměti

extern BOOL MemoryOK;			// příznak platného správce paměti

/////////////////////////////////////////////////////////////////////////////
// inicializace správce paměti (TRUE=inicializace OK)

BOOL MemInit();


/////////////////////////////////////////////////////////////////////////////
// ukončení správce paměti

void MemTerm();


/////////////////////////////////////////////////////////////////////////////
// přidělení bloku paměti (povolena 0 - vrátí malý blok) (vrací NULL=chyba)

void* _fastcall MemGet(int size);


/////////////////////////////////////////////////////////////////////////////
// uvolnění bloku paměti (povolena adresa NULL - ignoruje se)

void _fastcall MemFree(void* adr);


/////////////////////////////////////////////////////////////////////////////
// změna velikosti bloku paměti (adresa NULL = vytvoření, velikost 0 = zrušení)
// vrací NULL=příznak chyby (nebo zrušen), starý blok při chybě nebude změněn

void* _fastcall MemSize(void* adr, int size);


/////////////////////////////////////////////////////////////////////////////
// vícetoková inkrementace čísla LONG INT

#ifdef _MT
#ifdef _M_IX86

void _fastcall LongIncrement(long* num);

#else	// _M_IX86

inline void LongIncrement(long* num)
{
	::InterlockedIncrement(num);
}

#endif	// _M_IX86
#else	// _MT

inline void LongIncrement(long* num)
{
	(*num)++;
}

#endif	// _MT


/////////////////////////////////////////////////////////////////////////////
// vícetoková dekrementace čísla LONG INT (vrací TRUE = výsledek je 0)

#ifdef _MT
#ifdef _M_IX86

BOOL _fastcall LongDecrement(long* num);

#else	// _M_IX86

inline BOOL LongDecrement(long* num)
{
	return (::InterlockedDecrement(num) == 0);
}

#endif	// _M_IX86
#else	// _MT

inline BOOL LongDecrement(long* num)
{
	return (--(*num) == 0);
}

#endif	// _MT


/////////////////////////////////////////////////////////////////////////////
// naplnění bloku paměti konstantou (při optimalizaci je nahrazeno instrukcí REP STOS)

inline void _fastcall MemFill(void* dst, int count, char val)
{
	char* dst1 = (char*)dst;

	for (; count > 0; count--)
	{
		*dst1 = val;
		dst1++;
	}
}

inline void _fastcall MemFillShort(void* dst, int count, short val)
{
	short* dst2 = (short*)dst;

	for (; count > 0; count--)
	{
		*dst2 = val;
		dst2++;
	}
}

inline void _fastcall MemFillLong(void* dst, int count, long val)
{
	long* dst4 = (long*)dst;

	for (; count > 0; count--)
	{
		*dst4 = val;
		dst4++;
	}
}


/////////////////////////////////////////////////////////////////////////////
// kopie bloku dat 
// 1) Pozor - delka nesmí být záporná (délka je číslo bez znaménka)!!!!!
// 2) kopie probíhá vždy od nižších adres k vyšším
// 3) Při optimalizaci je nahrazeno instrukcí REP MOVS. Data se přenáší
//    po dvojslovech a proto se mírně liší od kopie bloku dat po bajtech
//	  v případě, že zdrojová adresa je vyšší než cílová o 1 až 3 (překryv)

inline void _fastcall MemCopy(void* dst, const void* src, DWORD count)
{
#ifdef _OPTIM
	memcpy(dst, src, count);
#else
	long* dst4 = (long*)dst;
	const long* src4 = (const long*)src;

	DWORD count4 = count/4;

	for (; count4 != 0; count4--)
	{
		*dst4 = *src4;
		dst4++;
		src4++;
	}

	char* dst1 = (char*)dst4;
	const char* src1 = (const char*)src4;

	DWORD count1 = count & 3;

	for (; count1 != 0; count1--)
	{
		*dst1 = *src1;
		dst1++;
		src1++;
	}
#endif
}	

// Pozn.: MemCopyShort by byl přeložen stejně jako MemCopy, nemá proto význam

//inline void _fastcall MemCopyLong(void* dst, const void* src, DWORD count)
//{
//#ifdef _OPTIM
//	memcpy(dst, src, 4*count);
//#else
//	long* dst4 = (long*)dst;
//	const long* src4 = (const long*)src;
//
//	for (; count != 0; count--)
//	{
//		*dst4 = *src4;
//		dst4++;
//		src4++;
//	}
//#endif
//}	

//#define MemCopyInt MemCopyLong

/////////////////////////////////////////////////////////////////////////////
// posun bloku dat bez překryvu (nahoru nebo dolů)
// (funkce se používá málo, není proto příliš optimalizována)

inline void MemMove(void* dst, const void* src, int count)
{
	if ((dst <= src) || ((char*)dst >= ((char*)src + count)))
	{
		for (; count > 0; count--)
		{
			*(char*)dst = *(char*)src;
			dst = (char*)dst + 1;
			src = (char*)src + 1;
		}
	}
	else
	{
		dst = (char*)dst + count - 1;
		src = (char*)src + count - 1;

		for (; count > 0; count--)
		{
			*(char*)dst = *(char*)src;
			dst = (char*)dst - 1;
			src = (char*)src - 1;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// porovnání bloku dat (při shodě nebo pro délku <= 0 vrací TRUE)

inline BOOL _fastcall MemCompare(const void* buf1, const void* buf2, int count)
{
#ifdef _OPTIM
	return ((count <= 0) || (memcmp(buf1, buf2, count) == 0));
#else
	for (; ((count > 0) && (*(char*)buf1 == *(char*)buf2)); count--)
	{
		buf1 = (char*)buf1 + 1;
		buf2 = (char*)buf2 + 1;
	}
	return (count <= 0);
#endif
}
