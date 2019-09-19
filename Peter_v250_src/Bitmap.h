
/***************************************************************************\
*																			*
*							Obsluha ikon a bitmap							*
*																			*
\***************************************************************************/


////////////////////////////////////////////////////////////////////
// definice záhlaví souboru ICO

#pragma pack( push )
#pragma pack( 2 )

typedef struct ICONDIRENTRY_
{
	BYTE	Width;					// šířka
	BYTE	Height;					// výška
	BYTE	Colors;					// počet barev (0 = 256 barev)
	BYTE	Reserved;				// ... rezervováno, musí být vždy 0
	WORD	Planes;					// počet barevných rovin (nastaveno na 0)
	WORD	Bits;					// počet bitů na bod (nastaveno na 0)
	DWORD	Size;					// velikost obrázku v bajtech
	DWORD	Offset;					// offset obrázku v souboru
} ICONDIRENTRY;

#define SIZEOFICONDIRENTRY (4*sizeof(BYTE) + 2*sizeof(WORD) + 2*sizeof(DWORD)) // 16 bajtů

typedef struct ICONDIR_
{
	WORD			Reserved;		// ... rezervováno, musí být vždy 0
	WORD			Type;			// typ souboru (ikona = 1, kurzor = 2)
	WORD			Count;			// počet položek v adresáři
	ICONDIRENTRY	Data[1];		// položky ikon
} ICONDIR;

#define SIZEOFICONDIR (3*sizeof(WORD))	// 6 bajtů

// Obrázek ikony:
//	BITMAPINFOHEADER	- záhlaví BMP
//			biSize až biBitCount a biSizeImage = platné položky, ostatní = 0
//	RGBQUAD				- palety
//	BYTE Xor[]			- data barev
//	BYTE And[]			- maska obrázku

#pragma pack( pop )


////////////////////////////////////////////////////////////////////
// test platnosti záhlaví BMP (TRUE=je OK)

BOOL TestBMP(const BITMAPINFO* bitmap);


////////////////////////////////////////////////////////////////////
// dekomprese dat bitmapy BI_RLE8

void DekompRLE8(BYTE* dst, int dstN, BYTE* src, int srcN);


////////////////////////////////////////////////////////////////////
// dekomprese dat bitmapy BI_RLE4

void DekompRLE4(BYTE* dst, int dstN, BYTE* src, int srcN);


////////////////////////////////////////////////////////////////////
// komprese dat bitmapy BI_RLE8, vrací velikost dat

int KompRLE8(BYTE* dstbuf, BYTE* srcbuf, int width, int height);
