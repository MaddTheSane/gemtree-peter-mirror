
#include "Main.h"


/***************************************************************************\
*																			*
*							Komprese/dekomprese dat							*
*					(pozor - nepodporuje vícetokový režim!)					*
*																			*
\***************************************************************************/


//////////////////////////////////////////////////////////////////////////////
// kódovací diagram:
/*
						  |
	+----------0--------(1 bit)-----1-----------+
 přenos										opakování
následujícího									|
  bajtu						+-----------0----(1 bit)-----------1----------------+
						opakování											opakování
						předešlého											předešlé
						  bajtu												  linky
						    |													|
			+-------0-----(1 bit)---1---+							+-----0--(1 bit)------1-----+
		počet=1							|						 délka=1						|
						+-----000----(3 bity)---jinak---+					 +-----000-----(3 bity)----jinak---+
						|				|				|					 |					|			   |
						|			   001				|					 |				   001			   |
					 počet=15			|		 počet=2 až 7			délka=15				|		   délka=2 až 7
									1x opakování										  1x opakování
								  před-předešlého										 dalšího bajtu
									  bajtu												 předešlé linky
*/

#define SUBSTLEN 22		// náhradní délka (pro kód = 0)

//////////////////////////////////////////////////////////////////////////////
// lokální proměnné pro kompresi

// výstupní buffer
BYTE*	DstBuf;					// adresa výstupního bufferu
BYTE*	DstAdr;					// ukládací adresa do výstupního bufferu
int		DstNum;					// velikost zbylých výstupních dat (při dekompresi)

// vstupní buffer
BYTE*	SrcBuf;					// adresa vstupního bufferu
BYTE*	SrcAdr;					// čtecí adresa ze vstupního bufferu
int		SrcNum;					// velikost zbylých vstupních dat (při kompresi)

// kompresní buffer (první 2 bajty je stavové slovo střadače bitů)
int		BitBit;					// počet bitů ve stavovém slově
int		BitNum;					// počet bajtů ve vyrovnávacím bufferu
BYTE	BitBuf[100];			// vyrovnávací výstupní buffer

// dekompresní buffer
WORD	StatBit;				// stavové slovo

//////////////////////////////////////////////////////////////////////////////
// zápis vyrovnávacího bufferu řetězce

void WriteBuf()
{
	int n = BitNum;				// počet bajtů ve vyrovnávacím bufferu
	MemCopy(DstAdr, BitBuf, n);	// zápis dat do bufferu
	DstAdr += n;				// posun ukládací adresy do bufferu
	BitNum = 2;					// nový počet bajtů v bufferu (= stavové slovo)
}


//////////////////////////////////////////////////////////////////////////////
// zápis skupiny bitů do stavového slova

void _fastcall WriteBit(int data, int num)
{
// přidání bitů ke střadači bitů
	int i = BitBit;				// starý počet bitů ve stavovém slově
	data <<= i;					// rotace nových dat na pozici
	*(WORD*)BitBuf |= (WORD)data; // přidání nových dat

// při přetečení počtu bitů zápis řetězce
	i += num;					// nový počet bitů
	if (i > 16)
	{
		WriteBuf();				// zápis vyrovnávacího bufferu
		i -= 16;				// snížený počet bitů ve střadači
		data >>= 16;			// neuložená data
		*(WORD*)BitBuf = (WORD)data; // zbylé neuložené bity
	}

// nový počet bitů střadače
	BitBit = i;
}


//////////////////////////////////////////////////////////////////////////////
// přenos bajtu beze změny

void WriteByte()
{
	WriteBit(0,1);
	BitBuf[BitNum] = *SrcAdr;
	SrcAdr++;
	SrcNum--;
	BitNum++;
}


//////////////////////////////////////////////////////////////////////////////
// komprese dat
//		dst = výstupní buffer (měl by být minimálně o 1/8 větší než vstupní buffer)
//		src = vstupní buffer s daty
//		size = velikost dat ke kompresi
//		width = délka linky bitmapy (pro offset předešlé linky)
// výstup: velikost dat ve výstupním bufferu

int Compress(BYTE* dst, BYTE* src, int size, int width)
{
// výstupní buffer
	DstBuf = dst;
	DstAdr = dst;

// vstupní buffer
	SrcBuf = src;
	SrcAdr = src;
	SrcNum = size;
	if (size <= 0) return 0;
	if ((width > size) || (width <= 1)) width = size;

// vyrovnávací buffer řetězce
	BitBit = 0;					// počet bitů ve stavovém slově
	BitNum = 2;					// počet bajtů ve vyrovnávacím bufferu
	*(WORD*)BitBuf = 0;			// stavové slovo (střadač bitů)

// první bajt se přenese beze změny
	WriteByte();

// cyklus komprese pro první linku
	for (int i = width; (i > 0) && (SrcNum > 0); i--)
	{

// opakování předešlého bajtu
		BYTE d = SrcAdr[-1];

		for (int j = 0; (j < SrcNum) && (d == SrcAdr[j]); j++) {};
		if (j > 0)
		{
			SrcAdr += j;
			SrcNum -= j;

			while (j > 0)
			{
				if (j == 1)
				{
					WriteBit(1, 3);
					j -= 1;
				}
				else
				{
					if (j >= SUBSTLEN)
					{
						WriteBit(5, 6);
						j -= SUBSTLEN;
					}
					else
					{
						int n = j;
						if (n > 7) n = 7;
						WriteBit((n << 3) + 5, 6);
						j -= n;
					}
				}
			}
		}

		else
		{

// opakování před-předešlého bajtu
			if ((SrcAdr > SrcBuf + 1) && (SrcAdr[0] == SrcAdr[-2]))
			{
				WriteBit(0xd, 6);
				SrcAdr++;
				SrcNum--;
			}
			else
			{

// přenos bajtu beze změny
				WriteByte();
			}
		}
	}

// cyklus komprese dat
	for (; SrcNum > 0;)
	{

// zjištění délky opakování předešlého bajtu
		BYTE d = SrcAdr[-1];
		int j;
		for (j = 0; (j < SrcNum) && (d == SrcAdr[j]); j++) {};

// zjištění délky opakování předešlé linky
		int  k;
		for (k = 0; (k < SrcNum) && (SrcAdr[k-width] == SrcAdr[k]); k++) {};

// test, zda bude opakování
		if ((j > 0) || (k > 0))
		{

// bude opakování předešlého bajtu
			if (j >= k)
			{
				SrcAdr += j;
				SrcNum -= j;

				while (j > 0)
				{
					if (j == 1)
					{
						WriteBit(1, 3);
						j -= 1;
					}
					else
					{
						if (j >= SUBSTLEN)
						{
							WriteBit(5, 6);
							j -= SUBSTLEN;
						}
						else
						{
							int n = j;
							if (n > 7) n = 7;
							WriteBit((n << 3) + 5, 6);
							j -= n;
						}
					}
				}
			}

// bude opakování předešlé linky
			else
			{
				SrcAdr += k;
				SrcNum -= k;

				while (k > 0)
				{
					if (k == 1)
					{
						WriteBit(3, 3);
						break;
					}
					else
					{
						if (k >= SUBSTLEN)
						{
							WriteBit(7, 6);
							k -= SUBSTLEN;
						}
						else
						{
							int n = k;
							if (n > 7) n = 7;
							WriteBit((n << 3) + 7, 6);
							k -= n;
						}
					}
				}
			}
		}
		else
		{

// opakování před-předešlého bajtu
			if (SrcAdr[0] == SrcAdr[-2])
			{
				WriteBit(0xd, 6);
				SrcAdr++;
				SrcNum--;
			}
			else
			{

// opakování dalšího bajtu předešlé linky
				if (SrcAdr[0] == SrcAdr[1-width])
				{
					WriteBit(0xf, 6);
					SrcAdr++;
					SrcNum--;
				}
				else
				{

// přenos bajtu beze změny
					WriteByte();
				}
			}
		}
	}

// vyprázdnění zbytku dat
	WriteBuf();

// počet bajtů ve výstupním bufferu
	return (DstAdr-DstBuf);
}


//////////////////////////////////////////////////////////////////////////////
// načtení bitu při dekompresi

BOOL ReadBit()
{
	WORD statbit = StatBit;
	BOOL b = statbit & 1;
	statbit >>= 1;
	if (statbit == 0)
	{
		statbit = *(WORD*)SrcAdr;
		SrcAdr += 2;
		b = statbit & 1;
		statbit >>= 1;
		statbit |= 0x8000;
	}
	StatBit = statbit;
	return b;
}


//////////////////////////////////////////////////////////////////////////////
// dekomprese dat
//		dst = výstupní buffer
//		src = vstupní buffer s daty
//		size = velikost výstupních dat (nezkomprimovaných)
//		width = délka linky bitmapy (pro offset předešlé linky)

void DeCompress(BYTE* dst, BYTE* src, int size, int width)
{
// výstupní buffer
	DstBuf = dst;
	DstAdr = dst;

// vstupní buffer
	SrcBuf = src;
	SrcAdr = src;
	SrcNum = size;
	if ((width > size) || (width <= 1)) width = size;
	width = -width;

	StatBit = 0;

// cyklus přes požadovaná data
	for (; size > 0;)
	{

// přenos bajtu beze změny
		if (!ReadBit())
		{
			*DstAdr = *SrcAdr;
			DstAdr++;
			SrcAdr++;
			size--;
		}

// bude opakování
		else
		{

// bude opakování předešlého bajtu
			if (!ReadBit())
			{

// opakování jednoho bajtu
				if (!ReadBit())
				{
					*DstAdr = DstAdr[-1];
					DstAdr++;
					size--;
				}

// načtení délky dat
				else
				{
					int n = 0;
					if (ReadBit()) n |= 1;
					if (ReadBit()) n |= 2;
					if (ReadBit()) n |= 4;

// opakování bajtu
					switch (n)
					{
					case 1:
						*DstAdr = DstAdr[-2];
						DstAdr++;
						size--;
						break;

					case 0:
						n = SUBSTLEN;

					default:
						{
							size -= n;
							BYTE d = DstAdr[-1];
							do 
							{
								*DstAdr = d;
								DstAdr++;
								n--;
							} while (n > 0);
						}
					}
				}
			}

// bude opakování předešlé linky
			else
			{

// opakování jednoho bajtu
				if (!ReadBit())
				{
					*DstAdr = DstAdr[width];
					DstAdr++;
					size--;
				}

// načtení délky dat
				else
				{
					int n = 0;
					if (ReadBit()) n |= 1;
					if (ReadBit()) n |= 2;
					if (ReadBit()) n |= 4;

// opakování bajtu
					switch (n)
					{
					case 1:
						*DstAdr = DstAdr[1+width];
						DstAdr++;
						size--;
						break;

					case 0:
						n = SUBSTLEN;

					default:
						{
							size -= n;
							do 
							{
								*DstAdr = DstAdr[width];
								DstAdr++;
								n--;
							} while (n > 0);
						}
					}
				}
			}
		}
	}
}
