
/***************************************************************************\
*																			*
*									D3D buffery								*
*																			*
\***************************************************************************/

// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI

/***************************************************************************\
*																			*
*								D3D buffer - textury						*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// struktura položky textury

// Do referencí se započítává i reference z obrázku. Položka je proto zrušena
// z bufferu až ve chvíli zrušení obrázku, potom až je ref=0

typedef struct D3DTITEM_
{
	int					Ref;		// počet referencí na texturu (0=položka je volná)
	CPicture			Pic;		// obrázek textury
	BOOL				Mips;		// aktuální textura vytvořena s mipmaps
	int					MipMaps;	// max. počet úrovní mipmaps (0=neomezeno)
	void*				Data1;		// data textury pro rozhraní 1 (NULL=není)
	void*				Data2;		// data textury pro rozhraní 2 (NULL=není)
} D3DTITEM;

////////////////////////////////////////////////////////////////////
// data bufferu textur

extern	int				D3DT_Num;	// počet položek v bufferu textur
extern	int				D3DT_Max;	// velikost bufferu textur (položek)
extern	D3DTITEM*		D3DT_Data;	// buffer textur

extern	int				D3DT_Active[MAX_STAGES];	// aktivně nastavená textura (i je-li vypnutá)
extern	bool			D3DT_On[MAX_STAGES];	// textura je zapnuta

struct D3DFITEM_;

//extern	D3DFITEM_*		D3D_Rend[3*MAX_ZBIAS]; // položky k renderování (běžné/blending/2D)

////////////////////////////////////////////////////////////////////
// přidání textury do seznamu textur, vrací index textury

int D3DT_Add(CPicture pic);

////////////////////////////////////////////////////////////////////
// uvolnění textury (kontroluje platnost indexu)

void D3DT_Del(int index);

////////////////////////////////////////////////////////////////////
// kontrola platnosti položky

inline BOOL D3DT_IsValid(const int index)
{ 
	return (((DWORD)index < (DWORD)D3DT_Num) &&
				(D3DT_Data[index].Ref > 0)); 
};

////////////////////////////////////////////////////////////////////
// poskytnutí adresy položky (bez kontroly indexu)

inline D3DTITEM* D3DT_Get(const int index)
{ 
	ASSERT(D3DT_IsValid(index));
	return &(D3DT_Data[index]);
}

////////////////////////////////////////////////////////////////////
// resetování cachování bufferu (před ukončením ovladače)

void D3DT_Reset();


////////////////////////////////////////////////////////////////////
// měkké resetování cachování bufferu (před resetováním ovladače při změně viewportu)
// - v současnosti není potřeba žádná obsluha

inline void D3DT_SoftReset() {}

////////////////////////////////////////////////////////////////////
// nastavení aktivní textury (-1 = vypnuta)

void D3DT_Akt(int index, int stage);


/***************************************************************************\
*																			*
*								D3D buffer - materiál						*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// struktura položky materiálu

typedef struct D3DMITEM_
{
	D3DMATERIAL8		Material;	// materiál objektu
	void*				Data1;		// data objektu 1 (NULL=není)
	void*				Data2;		// data objektu 2 (NULL=není)
} D3DMITEM;

/////////////////////////////////////////////////////////////////////////////
// data bufferu materiálů (položka 0 = implicitní materiál)

extern	int				D3DM_Num;		// počet položek v bufferu materiálů
extern	int				D3DM_Max;		// velikost bufferu materiálů (položek)	
extern	int				D3DM_Akt;		// aktivně nastavený materiál
extern	D3DMITEM*		D3DM_Data;		// buffer materiálů

////////////////////////////////////////////////////////////////////
// přidání materiálu do bufferu materiálů (vrací index materiálu)
// - pokud materiál již existuje, vrací index existujícího materiálu

int D3DM_Add(D3DMATERIAL8* mat);

////////////////////////////////////////////////////////////////////
// nastavení materiálu jako aktivní (bez kontroly indexu)

void D3DM_Set(const int index);

////////////////////////////////////////////////////////////////////
// resetování cachování bufferu (před ukončením ovladače)

void D3DM_Reset();


////////////////////////////////////////////////////////////////////
// měkké resetování cachování bufferu (před resetováním ovladače při změně viewportu)

inline void D3DM_SoftReset() { D3DM_Reset(); }


////////////////////////////////////////////////////////////////////
// kontrola platnosti položky

inline BOOL D3DM_IsValid(const int index)
{ 
	return ((DWORD)index < (DWORD)D3DM_Num);
};


////////////////////////////////////////////////////////////////////
// poskytnutí adresy položky (bez kontroly indexu)

inline D3DMITEM* D3DM_Get(const int index)
{ 
	ASSERT(D3DM_IsValid(index));
	return &(D3DM_Data[index]);
}

/***************************************************************************\
*																			*
*								D3D buffer - světla							*
*																			*
\***************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// struktura položky světla

typedef struct D3DLITEM_
{
	bool				IsUsed;		// světlo je použité
	bool				Akt;		// je nutné aktualizovat světlo
									// (platí i pro zrušené světlo)
	bool				Enable;		// světlo je zapnuto
	D3DLIGHT8			Light;		// popisovač světla
	int					Frame;		// index rámu s odkazem na světlo
	DWORD				Diffuse;	// difusní (a specular) barva zadaná uživatelem
	double				Range;		// dosah zadaný uživatelem
	double				Intens;		// intenzita světla zadaná uživatelem (1=normal)
	double				Attenuation1; // lineární zeslabení zadané uživatelem
	double				Attenuation2; // kvadratické zeslabení zadané uživatelem
	double				Theta;		// úhel světelného kužele (vnitřní kužel)
	double				Phi;		// úhel polostínu (vnější kužel)
	void*				Data1;		// data objektu 1 (NULL=není)
	void*				Data2;		// data objektu 2 (NULL=není)
} D3DLITEM;

////////////////////////////////////////////////////////////////////
// data bufferu světel

extern	int				D3DL_Num;	// počet položek v bufferu světel (včetně volných)
extern	int				D3DL_Max;	// velikost bufferu světel (položek)	
extern	D3DLITEM*		D3DL_Data;	// buffer světel

////////////////////////////////////////////////////////////////////
// vytvoření nového světla (vrací index světla)

int D3DL_New();

////////////////////////////////////////////////////////////////////
// aktualizace světel

void D3DL_AktLight();

////////////////////////////////////////////////////////////////////
// zrušení světla (kontroluje platnost indexu)

void D3DL_Del(int index);

////////////////////////////////////////////////////////////////////
// resetování bufferu světel

void D3DL_Reset();

////////////////////////////////////////////////////////////////////
// měkké resetování cachování bufferu (před resetováním ovladače při změně viewportu)

inline void D3DL_SoftReset() { D3DL_Reset(); }

////////////////////////////////////////////////////////////////////
// kontrola platnosti položky

inline BOOL D3DL_IsValid(const int index)
{ 
	return (((DWORD)index < (DWORD)D3DL_Num) && 
		(D3DL_Data[index].IsUsed));
};

////////////////////////////////////////////////////////////////////
// poskytnutí adresy položky (bez kontroly indexu)

inline D3DLITEM* D3DL_Get(const int index)
{ 
	ASSERT(D3DL_IsValid(index));
	return &(D3DL_Data[index]);
}


/***************************************************************************\
*																			*
*								D3D buffer - rámy							*
*																			*
\***************************************************************************/

// omezení počtu vrcholů
#define MAXVERTICES	1024	// maximální počet vrcholů pro jeden objekt

// vzor jednotkové matice
extern	D3DMATRIX	Matrix1Vzor;

// typy rámů
typedef enum _D3DFTYPE
{
	D3DFTYPE_GROUP = 0,		// prázdný rám (nebo scéna, index 0)
	D3DFTYPE_CAMERA,		// kamera (index 1)
	D3DFTYPE_OBJECT,		// 3D objekt (mesh, terén)
	D3DFTYPE_DECAL,			// 2D objekt
	D3DFTYPE_PICTURE,		// 2D obrázek (připojen ke kameře, index 1)
	D3DFTYPE_LIGHT,			// světlo
	D3DFTYPE_SECTOR,		// sektor objektu rozděleného na části
	D3DFTYPE_LENSFLARE,		// čočkový odlesk (připojen ke kameře, index 1)
} D3DFTYPE;

// typy pořadí rotací
typedef enum _D3DORDER
{
	D3DFORDER_XYZ = 0,		// XYZ
	D3DFORDER_XZY = 1,		// XZY
	D3DFORDER_YXZ = 2,		// YXZ
	D3DFORDER_YZX = 3,		// YZX
	D3DFORDER_ZXY = 4,		// ZXY
	D3DFORDER_ZYX = 5		// ZYX
} D3DORDER;

// zdroj materiálu
typedef enum _MATSOURCE
{
	MATSOURCE_OBJECT = 0,	// vnitřní materiál objektu (=implicitně)
	MATSOURCE_PARENT,		// materiál zděděn od rodiče
	MATSOURCE_FRAME			// implicitní materiál tohoto rámu
} MATSOURCE;

// typy průhlednosti (nastaveno pro zdroj nebo cíl blending operace)
typedef enum _BLENDTYPE
{
	BLEND_ZERO			= 0,	// (0, 0, 0, 0)
	BLEND_ONE			= 1,	// (1, 1, 1, 1)
	BLEND_SRCCOLOR		= 2,	// (Rs, Gs, Bs, As)
	BLEND_INVSRCCOLOR	= 3,	// (1-Rs, 1-Gs, 1-Bs, 1-As)
	BLEND_SRCALPHA		= 4,	// (As, As, As, As)
	BLEND_INVSRCALPHA	= 5,	// (1-As, 1-As, 1-As, 1-As)
	BLEND_DSTCOLOR		= 6,	// (Rd, Gd, Bd, Ad)
	BLEND_INVDSTCOLOR	= 7,	// (1-Rd, 1-Gd, 1-Bd, 1-Ad)
	BLEND_DSTALPHA		= 8,	// (Ad, Ad, Ad, Ad)
	BLEND_INVDSTALPHA	= 9,	// (1-Ad, 1-Ad, 1-Ad, 1-Ad)
	BLEND_SATURATE		= 10,	// menší ze složek SRC/DST
	BLEND_INVALID		= -1,	// neplatný typ operace
} BLENDTYPE;

extern	int	BlendTabD3D[11];	// tabulka blending operací pro Direct3D
extern	int	BlendTabOGL[11];	// tabulka blending operací pro OpenGL

extern	BLENDTYPE	D3DSrcBlend; // aktuální nastavení blending operace zdroje
extern	BLENDTYPE	D3DDstBlend; // aktuální nastavení blending operace cíle
extern	int			D3DAlphaRef;// aktuální nastavení alfa úrovně

extern	int			D3DColorOp[MAX_STAGES];		// aktuální nastavení operací barev
extern	int			D3DColorArg1[MAX_STAGES];	// aktuální nastavení argumentu 1 operací barev
extern	int			D3DColorArg2[MAX_STAGES];	// aktuální nastavení argumentu 2 operací barev
extern	int			D3DAlphaOp[MAX_STAGES];		// aktuální nastavení operací barev
extern	int			D3DAlphaArg1[MAX_STAGES];	// aktuální nastavení argumentu 1 operací barev
extern	int			D3DAlphaArg2[MAX_STAGES];	// aktuální nastavení argumentu 2 operací barev
extern	int			D3DAddressU[MAX_STAGES];	// aktuální adresování textur U
extern	int			D3DAddressV[MAX_STAGES];	// aktuální adresování textur V
//extern	int			D3DBorder[MAX_STAGES];		// barva okolí textur
extern	float		D3DBias[MAX_STAGES];		// zjemnění vzdálených textur
extern	DWORD		D3DTFactor;					// aktuální nastavení faktoru textur
extern	BOOL		D3DZWrite;					// aktuální hodnota hloubkového zápisu
extern	int			D3DZTest;					// aktuální hodnota hloubkového testu

extern	int			D3DAktMat;					// první položka k aktualizaci matic (-1=není)

/////////////////////////////////////////////////////////////////////////////
// popisovač automatického mapování textur

typedef struct AUTOMAP_
{
	int				method;			// metoda mapování (0=adresy, 1=vrcholy, 2=plošky)
	int				type;			// typ mapování (0=plocha, 1=válec, 2=koule, 3=chrom, 4=čočka)
	double			ox;				// souřadnice X počátku mapování
	double			oy;				// souřadnice Y počátku mapování
	double			oz;				// souřadnice Z počátku mapování
	double			rz;				// rotace podle osy Z
	double			rx;				// rotace podle osy X
	double			ry;				// rotace podle osy Y
	double			ou;				// počátek textury horizontálně
	double			ov;				// počátek textury vertikálně
	double			su;				// měřítko textury horizontálně
	double			sv;				// měřítko textury vertikálně

	D3DMATRIX		m;				// použitá transformační matice
									//   testují se jen členy 1 až 3,
									//	 členy _41 až _43 uchovávají
									//	 vektor k pozorovateli
									// Pro neplatné mapování (např. změna
									// parametrů) je _41 až _43 = 0

} AUTOMAP;

/////////////////////////////////////////////////////////////////////////////
// struktura položky rámu D3D (výpočty se provádí v přesnosti float)

typedef struct D3DFITEM_
{
	D3DFTYPE			Type;		// typ rámu

	int					Index;		// index této položky

// hiearchie (na začátku je scéna s indexem 0, nemá sousedy ani rodiče)
	int					Parent;		// index rodiče (-1 = není, 0 = scéna)
	int					Child;		// první potomek (-1 = není)
	int					Prev;		// předcházející soused stejné úrovně (-1 = není)
	int					Next;		// další soused stejné úrovně (-1 = není)

// informace k renderování
	int					Dist;		// druhá mocnina vzdálenosti od kamery k položce
	int					RendGroup;	// renderovací skupina (0 až 15)
	BOOL				ZWrite;		// zapisovat hloubkovou informaci
	int					ZTest;		// hloubkový test (1 až 8)

// rozdělení objektu na části
	BOOL				AktSector;	// aktualizovat generování sektorů
	BOOL				UseSector;	// používají se sektory namísto objektu
	BOOL				ParSector;	// meziúschova parametru od rodiče
	BOOL				res2;

// viditelnost rámu
	bool				Visible;	// zapnuta viditelnost

// nastavení objektu
	bool				Wireframe;	// vyplňování ploch
	bool				Lighton;	// osvětlení
	bool				Shades;		// plynulé stínování
	bool				MagFilter[MAX_STAGES];	// filtrace zvětšených textur
	bool				MinFilter[MAX_STAGES];	// filtrace zmenšených textur
	bool				MipFilter[MAX_STAGES];	// filtrace vzdálených textur
	short				Culling;	// zobrazené plochy (0=obě, 1=přední, 2=zadní)
	short				res3;

// snížená úroveň detailů LOD
	int					LODPrev;	// předcházející objekt pro LOD (-1=není)
	int					LODNext;	// další objekt pro LOD (-1=není)
	double				LODMax;		// maximální vzdálenost pro LOD
	float				LODMin2;	// minimální vzdálenost pro LOD ^2 (včetně této hodnoty)
	float				LODMax2;	// maximální vzdálenost pro LOD ^2 (bez této hodnoty)

// multitexturové operace
	int					Stages;		// aktivní počet stupňů, pro které jsou vygenerovány buffery
	int					ColorOp[MAX_STAGES];	// operace barvy
	int					ColorArg1[MAX_STAGES];	// první argument barvy
	int					ColorArg2[MAX_STAGES];	// druhý argument barvy
	int					AlphaOp[MAX_STAGES];	// operace alfa
	int					AlphaArg1[MAX_STAGES];	// první argument alfa
	int					AlphaArg2[MAX_STAGES];	// druhý argument alfa
	int					AddressU[MAX_STAGES];	// mód adresování U
	int					AddressV[MAX_STAGES];	// mód adresování V
//	int					Border[MAX_STAGES];		// barva okolí textury v barvách Petra (BGR)
//	DWORD				BorderRGB[MAX_STAGES];	// barva okolí v módu RGB
//	D3DCOLORVALUE		BorderF[MAX_STAGES];	// barva okolí v módu float
	float				Bias[MAX_STAGES];		// zjemnění vzdálených textur (0=běžné)
	bool				UseText[MAX_STAGES];	// stupeň používá příkazy využívající texturu
	DWORD				TFactor;				// faktor textur v barvách Petra (BGR)
	DWORD				TFactorRGB;				// faktor textur v barvách RGB
	D3DCOLORVALUE		TFactorF;				// faktor textur v barvách float

// hranice (koule)
	double				BoundR0;	// vlastní netransformovaný poloměr hranice
	double				BoundR;		// netransformovaný poloměr s potomky
	double				ParScale;	// přechodně uschované měřítko od rodiče
	bool				ParClip;	// uschovaný příznak od rodiče - testovat viditelnost
	bool				AktClip;	// položku je nutné testovat během renderování
	bool				AktBound;	// aktualizovat poloměr hranice s potomky
	bool				res4;

// světlo
	int					Light;		// index světla (-1 = není)

// transformace rámu
	D3DORDER			Order;		// pořadí rotací
	double				ScaleX;		// změna měřítka ve směru X
	double				ScaleY;		// změna měřítka ve směru Y
	double				ScaleZ;		// změna měřítka ve směru Z
	double				ScaleB;		// měřítko pro hranici (větší z měřítek)
	double				RotateX;	// rotace podle osy X (normalizováno 0 až 2pi)
	double				SinX;		// sinus úhlu rotace podle X
	double				CosX;		// kosinus úhlu rotace podle X
	double				RotateY;	// rotace podle osy Y (normalizováno 0 až 2pi)
	double				RotateY2D;	// přídavné natočení 2D objektu na kameru
	double				RotateX2D;	// uschovaná souřadnice X relativně ke kameře
	double				RotateZ2D;	// uschovaná souřadnice Z relativně ke kameře
	int					res8;

	double				SinY;		// sinus úhlu rotace podle Y
	double				CosY;		// kosinus úhlu rotace podle Y
	double				RotateZ;	// rotace podle osy Z (normalizováno 0 až 2pi)
	double				SinZ;		// sinus úhlu rotace podle Z
	double				CosZ;		// kosinus úhlu rotace podle Z
	double				TransX;		// posun ve směru X
	double				TransY;		// posun ve směru Y
	double				TransZ;		// posun ve směru Z

	bool				IsRotX;		// je rotace podle osy X (tj. úhel != 0)
	bool				IsRotY;		// je rotace podle osy Y (tj. úhel != 0)
	bool				IsRotZ;		// je rotace podle osy Z (tj. úhel != 0)

	bool				AktOwn;		// aktualizovat vlastní matici
	bool				AktWorld;	// aktualizovat výslednou matici

	bool				AktMat;		// zahrnut do řetězce aktualizace matic
	short				res5;

	int					AktMatPrev;	// předchozí položka pro aktualizaci matic (-1=je první)
	int					AktMatNext;	// následující položka pro aktualizaci matic (-1=je poslední)

	D3DMATRIX			MatrixOwn;	// vlastní transformační matice
	D3DMATRIX			MatrixWorld;// výsledná transformační matice (+matice rodiče)

// automatické mapování textur
	AUTOMAP*			AutoMap;	// automatické mapování textur (NULL=není)

// parametry terénu
	bool				IsTerrain;	// objekt je terén
	bool				res6;
	short				res7;
	int					TerWidth;	// šířka terénu (políček)
	int					TerHeight;	// výška terénu (políček)
	float*				TerMap;		// mapa terénu ((width+1)*(height+1) hodnot 0 až 1)

// klonování (pro 1 objekt je ClonePrev = CloneNext = Index)
// u klonů jsou sdílené tyto buffery:

//		TerMap

//		Vertex
//		Normal
//		VertCol
//		VertColF
//		TextUV
//		Face
//		FaceW

//		ShadeVert
//		ShadeFace
//		ShadeNorm
//		ShadeEdgeV
//		ShadeEdgeN
//		ShadeNext
//		ShadeLeft

//		Data1
//		Data2

	int					ClonePrev;	// předcházející klon (není klon: ClonePrev == Index)
	int					CloneNext;	// další klon (není klon: CloneNext == Index)

// vzor objektu
	int					VertNum;	// počet vrcholů v seznamu vrcholů, normál, barev a souřadnic textur
	D3DVECTOR*			Vertex;		// seznam vektorů vrcholů
	D3DVECTOR*			Normal;		// seznam vektorů normál
	DWORD				AutoCol;	// aktuální automatická barva vrcholů (MAXDWORD = není)
	DWORD*				VertCol;	// seznam barev vrcholů RGBA (NULL=není)
	D3DCOLORVALUE*		VertColF;	// seznam barev vrcholů FLOAT (NULL=není)
	float*				TextUV[MAX_STAGES];	// souřadnice U a V textur (NULL=nejsou)
	int					FaceNum;	// počet plošek v seznamu plošek
	int*				Face;		// seznam plošek (každá 3 vrcholy)
	WORD*				FaceW;		// seznam plošek ve formátu WORD (jen pokud to vyžaduje driver)

// implicitní povrch (barvy (int) jsou ve formátru Petra, tj. BGR)
	MATSOURCE			MatSource;	// použitý zdroj materiálu
	DWORD				Diffuse;	// difusní barva materiálu zadaná uživatelem
	DWORD				Ambient;	// ambient barva zadaná uživatelem (MAXDWORD=jako difusní)
	DWORD				Emissive;	// emisivní barva materiálu zadaná uživatelem
	DWORD				Specular;	// barva odlesku materiálu zadaná uživatelem
	double				Power;		// mocnina barvy odlesku zadaná uživatelem (0 až 128)
	int					Material;	// index materiálu (0 = implicitní)
	int					ParMat;		// index materiálu rodiče
									// (používá se pouze k přechodné úschově
									// materiálu rodiče během renderování)
	DWORD				ParDif;		// difusní barva materiálu rodiče (pouze k úschově)

// morfování
	int					MorphMax;	// počet stupňů morfování (0=není)
	double				MorphStage;	// aktuální stupeň morfování (0 až MorphMax)
	D3DVECTOR**			MorphVert;	// buffery vrcholů
	D3DVECTOR**			MorphNorm;	// buffery normál
	float**				MorphUV;	// buffery souřadnic textur (pro každý stupeň MAX_STAGES bufferů)

// textura
	int					Texture[MAX_STAGES]; // index textury (-1 = není)
	int					ParText;	// index textury 0 rodiče
									// (používá se pouze k přechodné úschově
									// textury rodiče během renderování)

// průhlednost
	bool				IsBlend;	// je zapnut alpha blending (SRC<>1 nebo DST<>0)
	bool				IsAlphaRef;	// je zapnuto alfa porovnávání
	bool				BlendRend;	// vyžaduje dodatečný průchod pro alfa blending
	BLENDTYPE			SrcBlend;	// blending operace pro zdroj
	BLENDTYPE			DstBlend;	// blending operace pro cíl
	int					SrcDstBlend; // kód blending operace v kódu uživatele = SRC + 10*DST
									// (1=normal, 11=oheň, 20=sklo, 54=překryv)
	double				AlphaRef;	// referenční úroveň alfa kanálu (0=normal)
	int					AlphaRef2;	// referenční úroveň ve tvaru BYTE (0=normal)

// stín (pro přegenerování bufferů stačí zrušit jen "ShadeVert")
	double				ShadowRange; // dosah stínu
	bool				IsShadow;	// je zapnut stín
	bool				ShadowParent; // odvozovat vektor dopadu stínu od rodiče
	D3DVECTOR			ShadowPoint; // poslední vektor dopadu stínu ([0,0,0]=neplatný)

	int					ShadeVertN;	// počet vrcholů
	D3DVECTOR*			ShadeVert;	// buffer vrcholů (vypuštěné zdvojené vrcholy)
	int*				ShadeFace;	// buffer plošek (každá ploška 3 indexy vrcholů)
	D3DVECTOR*			ShadeNorm;	// buffer normál plošek
	// hrany jsou kolem plošky orientovány proti směru hodinových ručiček (=kladný směr pro vějíř)
	// vrcholy hran: 0: v1-v3, 1: v3-v2, 2: v2-v1
	int*				ShadeEdgeV;	// vrcholy hran plošek (každá ploška 3 hrany po 2 vrcholech)
	int*				ShadeEdgeN;	// protichůdná hrana sousední plošky (každá ploška 3 indexy hran, -1=není)
	int*				ShadeNext;	// navazující hrana první vpravo z 2. vrcholu (každá ploška 3 indexy)
	int*				ShadeLeft;	// další hrana z vrcholu 1 směrem doleva (každá ploška 3 indexy)

	char*				ShadeEdgeS;	// stav hrany (každá ploška 3 hrany)
									//   bit 0: ploška vlevo osvícená
									//   bit 1: ploška vpravo osvícená

	int					ShadowFanN;	// počet obrysů v bufferu (počet "vějířů")
	int					ShadowFan0;	// délka bufferu stínu celkem (=počet vrcholů)
	int*				ShadowFan;	// počty vrcholů pro jednotlivé obrysy ("vějíře")
	D3DVECTOR*			ShadowVolume; // buffer stínu (vějíře, 1 vrchol počátek, zbytek vrcholy vějíře)

// datové buffery ovladačů
	void*				Data1;		// datový buffer 1 (vrcholy), NULL=není
	void*				Data2;		// datový buffer 2 (indexy), NULL=není
	void*				Data3;		// datový buffer pro stíny, NULL=není

} D3DFITEM;


////////////////////////////////////////////////////////////////////
// popisovače bufferu rámů

extern	D3DFITEM*	D3DF_Data;		// ukazatel na data
extern	bool*		D3DF_Valid;		// příznaky platnosti položek
extern	int			D3DF_Max;		// velikost bufferu (položek)
extern	int			D3DF_Next;		// příští volná položka (-1 = není)

////////////////////////////////////////////////////////////////////
// seznam skupin renderování

extern	BOOL		RendSort[MAX_RENDGROUP];	// příznaky třídění skupin
extern	int			RendNum[MAX_RENDGROUP];		// počty položek v bufferech
extern	D3DFITEM**	RendItems[MAX_RENDGROUP];	// seznam položek k renderování
extern	int			RendMax;					// velikost bufferů položek

////////////////////////////////////////////////////////////////////
// vynulování matice

inline void Matrix0(D3DMATRIX* m)
{
	MemFill(m, sizeof(D3DMATRIX), 0);
}

////////////////////////////////////////////////////////////////////
// vytvoření jednotkové matice

inline void Matrix1(D3DMATRIX* m)
{
	MemCopy(m, &Matrix1Vzor, sizeof(D3DMATRIX));
}

////////////////////////////////////////////////////////////////////
// aktualizace transformačních matic všech objektů
// (provádí se při snížené přesnosti FPU!)

void D3DF_AktMatrix();

////////////////////////////////////////////////////////////////////
// transpozice matice (zdrojová a cílová matice může být stejná)

void _fastcall MatrixTrans(D3DMATRIX* dst, D3DMATRIX* src);

////////////////////////////////////////////////////////////////////
// vynásobení dvou matic (m = m1 * m2), výsledná matice nesmí být vstupní!

void _fastcall MatrixMul(D3DMATRIX* m, const D3DMATRIX* m1, const D3DMATRIX* m2);

////////////////////////////////////////////////////////////////////
// inverze matice (zdrojová a cílová matice může být stejná)

void MatrixInv(D3DMATRIX* dst, D3DMATRIX* src);

////////////////////////////////////////////////////////////////////
// vynásobení vektoru maticí (provádí se při snížené přesnosti FPU)
// (zdrojový i cílový vektor mohou být shodné)

void VecXMat(D3DVECTOR* dst, D3DVECTOR* src, D3DMATRIX* mat);


////////////////////////////////////////////////////////////////////
// nastavení transformací podle transformační matice

void D3DF_SetTransMatrix(int index, D3DMATRIX* m);

////////////////////////////////////////////////////////////////////
// připojení do řetězce aktualizace matic (kontroluje, zda je připojen)

void _fastcall D3DF_AktMatLink(D3DFITEM* item);

////////////////////////////////////////////////////////////////////
// odpojení z řetězce aktualizace matic (kontroluje, zda je připojen)

void _fastcall D3DF_AktMatUnlink(D3DFITEM* item);

////////////////////////////////////////////////////////////////////
// vytvoření nové položky bufferu (vrací index položky)

int D3DF_New();

////////////////////////////////////////////////////////////////////
// příprava položky k modifikaci (odpojení od klonů)

void D3DF_Modi(int index);

////////////////////////////////////////////////////////////////////
// kopie položky s potomky (vrací index nové položky)

int D3DF_Copy(int index);

////////////////////////////////////////////////////////////////////
// zrušení položky (kontroluje platnost indexu)

void _fastcall D3DF_Del(const int index);

////////////////////////////////////////////////////////////////////
// resetování cachování bufferu (před ukončením ovladače)

void D3DF_Reset();

////////////////////////////////////////////////////////////////////
// měkké resetování cachování bufferu (před resetováním ovladače při změně viewportu)
// - v současnosti není potřeba žádná obsluha

inline void D3DF_SoftReset() {}

////////////////////////////////////////////////////////////////////
// zrušení všech položek v bufferu (ukládání začne opět po řadě)

void D3DF_DelAll();

////////////////////////////////////////////////////////////////////
// zrušení bufferů morfování

void D3DF_MorphReset(D3DFITEM* item);

/////////////////////////////////////////////////////////////////////////////
// aktualizace měřítka pro hranici

void AktScaleB(D3DFITEM* item);

////////////////////////////////////////////////////////////////////
// inicializace manažeru vrcholů (index položky musí být platný!)

void D3DF_UsedInit(int inx);

////////////////////////////////////////////////////////////////////
// duplikace vrcholu (parametr = index vrcholu plošky), vrací nový index

int D3DF_UsedDuplik(int faceinx);

////////////////////////////////////////////////////////////////////
// nastavení normály vrcholu (pro daný index vrcholu plošky)

void D3DF_UsedNormal(int faceinx, float nx, float ny, float nz);

////////////////////////////////////////////////////////////////////
// nastavení barvy vrcholu (pro daný index vrcholu plošky)

void D3DF_UsedColor(int faceinx, DWORD col, float r, float g, float b, float a);

void D3DF_UsedColorB(int faceinx, BYTE r, BYTE g, BYTE b, BYTE a);

////////////////////////////////////////////////////////////////////
// nastavení adresy textury vrcholu (pro daný index vrcholu plošky)

void D3DF_UsedTextUV(int faceinx, float u, float v, int stage);

////////////////////////////////////////////////////////////////////
// napojení položky na rodiče (s odpojením od předcházejícího rodiče)
// kontroluje platnost obou indexů

void _fastcall D3DF_Link(const int parent, const int child);

////////////////////////////////////////////////////////////////////
// zmapování vlastní hranice (index musí být platný)

void D3DF_MapBound(int index);

////////////////////////////////////////////////////////////////////
// vytvoření objektu (vrací index objektu)
// Po vytvoření objektu je nutné nastavit normály!!!

int D3DF_CreateMesh(int vertN, D3DVECTOR* vert, int faceN, int* face);

////////////////////////////////////////////////////////////////////
// nastavení normál objektu (počet normál musí souhlasit s počtem vrcholů!)

void D3DF_SetNormal(int index, D3DVECTOR* norm);

////////////////////////////////////////////////////////////////////
// korekce nově vytvořeného aktuálního objektu pro pravoruký souřadný systém

void D3DF_KorRightHand();

////////////////////////////////////////////////////////////////////
// nastavení barev vrcholů objektu (počet musí souhlasit s počtem vrcholů!)

void D3DF_SetVertCol(int index, D3DCOLORVALUE* col);

////////////////////////////////////////////////////////////////////
// nastavení mapování textur (počet musí souhlasit s počtem vrcholů!)

void D3DF_SetTextUV(int index, float* textuv, int stage);

////////////////////////////////////////////////////////////////////
// nastavení adresování a zalamování textury

void D3DF_SetAddress(int index, int adru, int adrv, int stage);

////////////////////////////////////////////////////////////////////
// nastavení blending módu (1=normal, 11=oheň, 20=sklo, 54=překryv, 100=saturace)

void D3DF_SetBlend(int index, int blendmode);

////////////////////////////////////////////////////////////////////
// vygenerování normál (kontroluje index)

void D3DF_AutoNormals(int index, double angle);

////////////////////////////////////////////////////////////////////
// renderování objektů (pro Direct3D musí být BeginScene a EndScene!)

void D3DF_Render();

////////////////////////////////////////////////////////////////////
// renderování objektů s blending (pro Direct3D musí být BeginScene a EndScene!)

void D3DF_RenderBlend();

////////////////////////////////////////////////////////////////////
// renderování 2D obrázků (pro Direct3D musí být BeginScene a EndScene!)

void D3DF_Render2D();

////////////////////////////////////////////////////////////////////
// kontrola platnosti položky

inline BOOL D3DF_IsValid(const int index)
{ 
	return (((DWORD)index < (DWORD)D3DF_Max) &&
				D3DF_Valid[index]); 
};

inline BOOL IsValidID() { return D3DF_IsValid(D3D_ID); }

////////////////////////////////////////////////////////////////////
// poskytnutí adresy položky (bez kontroly indexu)

inline D3DFITEM* D3DF_Get(const int index)
{ 
	ASSERT(D3DF_IsValid(index));
	return &(D3DF_Data[index]);
}

inline D3DFITEM* GetID() { return D3DF_Get(D3D_ID); }


// funkce aktivního rozhraní - definice
typedef	void (*D3STOP) ();						// ukončení ŕunkce zařízení
typedef	void (*D3SIZEVIEW) ();					// aktualizace viewportu po změně velikosti
typedef	void (*D3MOVEVIEW) ();					// aktualizace viewportu po přesunu
typedef	void (*D3DISP) ();						// zobrazení renderovacího bufferu
typedef	void (*D3CLEAR) ();						// vymazání renderovací plochy
typedef	void (*D3AKTPROJ) ();					// aktualizace projekční matice
typedef	void (*D3AKTVIEW) ();					// aktualizace pohledové matice
typedef	void (*D3BEGIN) ();						// zahájení renderování scény
typedef	void (*D3END) ();						// ukončení renderování scény
typedef void (*D3RENDER) (D3DFITEM* item);		// vyrenderování jednoho objektu
typedef void (*D3RESET) (D3DFITEM* item);		// resetování objektu před ukončením ovladače
typedef void (*D3SRESET) (D3DFITEM* item);		// resetování stínu objektu před ukončením ovladače
typedef void (*D3AKTWIREFRAME) ();				// aktualizovat přepínač vyplňování ploch
typedef void (*D3AKTLIGHTON) ();				// aktualizovat přepínač osvětlení
typedef void (*D3AKTSHADES) ();					// aktualizovat přepínač plynulého stínování
typedef void (*D3AKTCULLING) ();				// aktualizovat přepínač odstraňování ploch
typedef void (*D3AKTMAGFILTER) (int stage);		// aktualizovat přepínač filtrace zvětšených textur
typedef void (*D3AKTMINMIPFILTER) (int stage);	// aktualizovat přepínač filtrace zmenšených a vzdálených textur
typedef void (*D3AKTLIGHT) (int inx, D3DLITEM* item);	// aktualizace položky světla
typedef void (*D3SETMATERIAL) (D3DMITEM* item);	// nastavení materiálu
typedef void (*D3TEXTURERES) (D3DTITEM* item);	// resetování položky textury před ukončením ovladače
typedef void (*D3TEXTURESET) (D3DTITEM* item, int stage);	// nastavení a zapnutí textury
typedef void (*D3TEXTUREON) (D3DTITEM* item, int stage);	// zapnutí původní textury
typedef void (*D3TEXTUREOFF) (int stage);				// vypnutí textury
typedef void (*D3MATRESET) (D3DMITEM* item);	// resetování materiálu
typedef void (*D3LIGHTRESET) (int inx, D3DLITEM* item);	// resetování světla
typedef void (*D3AKTAMBIENT) ();				// aktualizace ambient osvětlení
typedef int (*D3FREE) ();						// volná videopaměť
typedef void (*D3AKTFOG) ();					// aktualizace mlhy
typedef void (*D3FOGON) (BOOL on);				// zapnutí mlhy
typedef void (*D3AKTSTATE) ();					// aktualizace stavu
typedef void (*D3AKTBLEND) (D3DFITEM* item);	// aktualizace blending operací objektu
typedef void (*D3AKTALPHA) (D3DFITEM* item);	// aktualizace referenční úrovně alfa
typedef void (*D3AKTPALETTE) (HWND wnd);		// aktualizace palet
typedef void (*D3SETCOLOROP) (int op, int arg1, int arg2, int stage); // nastavení operace barev
typedef void (*D3SETALPHAOP) (int op, int arg1, int arg2, int stage); // nastavení operace alfa
typedef void (*D3SETADDRESS) (int adru, int adrv, int stage); // nastavení adresování textur
//typedef void (*D3SETBORDER) (D3DFITEM* item, int stage); // nastavení barvy okolí textur
typedef void (*D3SETBIAS) (float bias, int stage);	// nastavení zjemnění vzdálených textur
typedef void (*D3SETTFACTOR) (D3DFITEM* item);	// nastavení barvy faktoru textur
typedef void (*D3SETZFUNC)(int func);			// nastavení Z funkce (1 až 8, v kódu DirectX, 0=vypnuto)
typedef void (*D3SETZWRITE)(BOOL write);		// nastavení Z zápisu
typedef void (*D3SENABLE) (BOOL enable);		// povolení stencil operací
typedef void (*D3SETSOP) (int fail, int zfail, int zpass); // nastavení stencil operací
typedef void (*D3SETSFUNC) (int func, int ref, DWORD mask); // nastavení stencil testu
typedef void (*D3SETSMASK) (DWORD mask);		// nastavení zápisové masky do stencil bufferu
typedef void (*D3SHADOW) (D3DFITEM* item);		// vyrenderování stínu jednoho objektu
typedef void (*D3SHADOWS) ();					// vykreslení stínů

// funkce aktivního rozhraní
extern	D3STOP			pD3Stop;				// ukončení funkce zařízení
extern	D3SIZEVIEW		pD3SizeView;			// aktualizace viewportu po změně velikosti
extern	D3MOVEVIEW		pD3MoveView;			// aktualizace viewportu po přesunu
extern	D3DISP			pD3Disp;				// zobrazení renderovacího bufferu
extern	D3DISP			pD3Disp;				// zobrazení renderovacího bufferu
extern	D3CLEAR			pD3Clear;				// vymazání renderovací plochy
extern	D3AKTPROJ		pD3AktProj;				// aktualizace projekční matice
extern	D3AKTVIEW		pD3AktView;				// aktualizace pohledové matice
extern	D3BEGIN			pD3Begin;				// zahájení renderování scény
extern	D3END			pD3End;					// ukončení renderování scény
extern	D3RENDER		pD3Render;				// vyrenderování jednoho objektu
extern	D3RESET			pD3Reset;				// resetování objektu před ukončením ovladače
extern	D3SRESET		pD3SReset;				// resetování stínu objektu před ukončením ovladače
extern	D3AKTWIREFRAME	pD3AktWireframe;		// aktualizovat přepínač vyplňování ploch
extern	D3AKTLIGHTON	pD3AktLightOn;			// aktualizovat přepínač osvětlení
extern	D3AKTSHADES		pD3AktShades;			// aktualizovat přepínač plynulého stínování
extern	D3AKTCULLING	pD3AktCulling;			// aktualizovat přepínač odstraňování ploch
extern	D3AKTMAGFILTER	pD3AktMagFilter;		// aktualizovat přepínač filtrace zvětšených textur
extern	D3AKTMINMIPFILTER pD3AktMinMipFilter;	// aktualizovat přepínač filtrace zmenšených a vzdalených textur
extern	D3AKTLIGHT		pD3AktLight;			// aktualizace položky světla
extern	D3SETMATERIAL	pD3SetMaterial;			// nastavení materiálu
extern	D3TEXTURERES	pD3TextureRes;			// resetování položky textury před ukončením ovladače
extern	D3TEXTURESET	pD3TextureSet;			// nastavení a zapnutí textury
extern	D3TEXTUREON		pD3TextureOn;			// zapnutí původní textury
extern	D3TEXTUREOFF	pD3TextureOff;			// vypnutí textury
extern	D3MATRESET		pD3MatReset;			// resetování materiálu
extern	D3LIGHTRESET	pD3LightReset;			// resetování světla
extern	D3AKTAMBIENT	pD3AktAmbient;			// aktualizace ambient osvětlení
extern	D3FREE			pD3Free;				// volná videopaměť
extern	D3AKTFOG		pD3AktFog;				// aktualizace mlhy
extern	D3FOGON			pD3FogOn;				// zapnutí mlhy
extern	D3AKTSTATE		pD3AktState;			// aktualizace stavu
extern	D3AKTBLEND		pD3AktBlend;			// aktualizace blending operací objektu
extern	D3AKTALPHA		pD3AktAlfa;				// aktualizace referenční úrovně alfa
extern	D3AKTPALETTE	pD3AktPalette;			// aktualizace palet
extern	D3SETCOLOROP	pD3SetColorOp;			// nastavení operace barev
extern	D3SETALPHAOP	pD3SetAlphaOp;			// nastavení operace alfa
extern	D3SETADDRESS	pD3SetAddress;			// nastavení adresování textur
//extern	D3SETBORDER		pD3SetBorder;			// nastavení barvy okolí textur
extern	D3SETBIAS		pD3SetBias;				// nastavení zjemnění vzdálených textur
extern	D3SETTFACTOR	pD3SetTFactor;			// nastavení barvy faktoru textur
extern	D3SETZFUNC		pD3SetZFunc;			// nastavení Z funkce (1 až 8, v kódu DirectX)
extern	D3SETZWRITE		pD3SetZWrite;			// nastavení hloubkového zápisu
extern	D3SENABLE		pD3SEnable;				// povolení stencil operací
extern	D3SETSOP		pD3SetSOp;				// nastavení stencil operací
extern	D3SETSFUNC		pD3SetSFunc;			// nastavení stencil testu
extern	D3SETSMASK		pD3SetSMask;			// nastavení zápisové masky do stencil bufferu
extern	D3SHADOW		pD3Shadow;				// vyrenderování stínu jednoho objektu
extern	D3SHADOWS		pD3Shadows;				// vykreslení stínů


#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------
