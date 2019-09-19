
/***************************************************************************\
*																			*
*							Hlavní okno aplikace							*
*																			*
\***************************************************************************/

//#define OPENGL11								// povoleno používat rozhraní OpenGl 1.1


// obsluha DirectDraw (inicializace pouze jednou, při prvním použití)
//extern bool					IsInitDirectDraw;	// proběhla inicializace DirectDraw
//extern LPDIRECTDRAW			DirectDraw;			// objekt DirectDraw (NULL=není)
//extern LPDIRECTDRAWSURFACE	DirectSurface;		// povrch pro DirectDraw (NULL=není)
//extern LPDIRECTDRAWPALETTE	DirectPalette;		// palety pro DirectDraw (NULL=nejsou)
extern	bool				Resizing;			// probíhá programová změna okna

extern	bool				AppActive;			// aplikace je aktivní
extern	bool				AppMinim;			// aplikace minimalizovaná

#define MAX_STAGES	8				// max. počet stupňů operací textur
#define MAX_RENDGROUP	16			// počet renderovacích skupin
#define	RENDNORM	4				// skupina normálních objektů k renderování
#define	RENDTRANS	10				// skupina průhledných objektů k renderování
#define	RENDPIC		14				// skupina obrázků k renderování

//#define MAX_ZBIAS	16				// maximální počet BIAS odsazení (je to o 1 méně, než je povoleno)
//#define MID_ZBIAS	8				// střední (implicitní) odsazení (bude -8 až +7)

// funkce DirectDrawCreate
//typedef	HRESULT (WINAPI *DIRECTDRAWCREATE) (GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter);
//extern DIRECTDRAWCREATE	pDirectDrawCreate; // funkce DirectDrawCreate (NULL=není)

#if (DIRECT3D_VERSION < 0x0800)
typedef enum _D3DFORMAT
{
    D3DFMT_UNKNOWN              =  0,

    D3DFMT_R8G8B8               = 20,
    D3DFMT_A8R8G8B8             = 21,
    D3DFMT_X8R8G8B8             = 22,
    D3DFMT_R5G6B5               = 23,
    D3DFMT_X1R5G5B5             = 24,
    D3DFMT_A1R5G5B5             = 25,
    D3DFMT_A4R4G4B4             = 26,
    D3DFMT_R3G3B2               = 27,
    D3DFMT_A8                   = 28,
    D3DFMT_A8R3G3B2             = 29,
    D3DFMT_X4R4G4B4             = 30,

    D3DFMT_A8P8                 = 40,
    D3DFMT_P8                   = 41,

    D3DFMT_L8                   = 50,
    D3DFMT_A8L8                 = 51,
    D3DFMT_A4L4                 = 52,

    D3DFMT_V8U8                 = 60,
    D3DFMT_L6V5U5               = 61,
    D3DFMT_X8L8V8U8             = 62,
    D3DFMT_Q8W8V8U8             = 63,
    D3DFMT_V16U16               = 64,
    D3DFMT_W11V11U10            = 65,

    D3DFMT_UYVY                 = MAKEFOURCC('U', 'Y', 'V', 'Y'),
    D3DFMT_YUY2                 = MAKEFOURCC('Y', 'U', 'Y', '2'),
    D3DFMT_DXT1                 = MAKEFOURCC('D', 'X', 'T', '1'),
    D3DFMT_DXT2                 = MAKEFOURCC('D', 'X', 'T', '2'),
    D3DFMT_DXT3                 = MAKEFOURCC('D', 'X', 'T', '3'),
    D3DFMT_DXT4                 = MAKEFOURCC('D', 'X', 'T', '4'),
    D3DFMT_DXT5                 = MAKEFOURCC('D', 'X', 'T', '5'),

    D3DFMT_D16_LOCKABLE         = 70,
    D3DFMT_D32                  = 71,
    D3DFMT_D15S1                = 73,
    D3DFMT_D24S8                = 75,
    D3DFMT_D16                  = 80,
    D3DFMT_D24X8                = 77,
    D3DFMT_D24X4S4              = 79,


    D3DFMT_VERTEXDATA           =100,
    D3DFMT_INDEX16              =101,
    D3DFMT_INDEX32              =102,

    D3DFMT_FORCE_DWORD          =0x7fffffff
} D3DFORMAT;
#endif

#if (DIRECT3D_VERSION < 0x0800)
typedef struct _D3DMATERIAL8 {
    D3DCOLORVALUE   Diffuse;        /* Diffuse color RGBA */
    D3DCOLORVALUE   Ambient;        /* Ambient color RGB */
    D3DCOLORVALUE   Specular;       /* Specular 'shininess' */
    D3DCOLORVALUE   Emissive;       /* Emissive color RGB */
    float           Power;          /* Sharpness if specular highlight */
} D3DMATERIAL8;

typedef struct _D3DLIGHT8 {
    D3DLIGHTTYPE    Type;            /* Type of light source */
    D3DCOLORVALUE   Diffuse;         /* Diffuse color of light */
    D3DCOLORVALUE   Specular;        /* Specular color of light */
    D3DCOLORVALUE   Ambient;         /* Ambient color of light */
    D3DVECTOR       Position;         /* Position in world space */
    D3DVECTOR       Direction;        /* Direction in world space */
    float           Range;            /* Cutoff range */
    float           Falloff;          /* Falloff */
    float           Attenuation0;     /* Constant attenuation */
    float           Attenuation1;     /* Linear attenuation */
    float           Attenuation2;     /* Quadratic attenuation */
    float           Theta;            /* Inner angle of spotlight cone */
    float           Phi;              /* Outer angle of spotlight cone */
} D3DLIGHT8;



#endif

// definice roviny
typedef struct PLANE_
{
	float		A;								// složka X normály
	float		B;								// složka Y normály
	float		C;								// složka Z normály
	float		D;								// posun od 0 proti směru normály
} PLANE;

// videomód DirectDraw
typedef struct VIDEOMODEITEM_
{
	int			Width;							// šířka
	int			Height;							// výška
	int			Bits;							// počet bitů na bod (8, 16, 24 nebo 32)
	int			res;							// rezervováno
//	D3DFORMAT	Format;							// format videomódu Direct3D
} VIDEOMODEITEM;

#define MAXVIDEOMODES 64

extern	bool				FullScreen;			// je celoobrazovkový režim s DirectDraw
extern	BOOL				OldMaximized;		// uschovaný příznak maximalizace
extern	int					NumVideoModes;		// počet videomódů v tabulce
extern	VIDEOMODEITEM*		TabVideoModes;		// tabulka videomódů

// požadovaný uživatelský videomód
extern	int					UserScreenWidth;	// uživatelská šířka videomódu (0=implicitní)
extern	int					UserScreenHeight;	// uživatelská výška videomódu (0=implicitní)
//extern	bool				UserFullScreen;		// uživatelem požadován celoobrazovkový mód 3D
extern	int					UserScreenBits;		// uživatelský počet bitů (0=implicitní)


// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI
	  
// zvolené rozhraní a ovladač
extern	int					D3DIntUser;			// uživatelem zadané rozhraní 0 až D3INTERFACES-1 (0=automatický)
extern	int					D3DIntAkt;			// typ aktivního rozhraní (0=není,1=GL0,2=GL1,3=GL2,4=DX3,5=DX5,6=DX6,7=DX7,8=DX8)
extern	int					D3DDevUser;			// uživatelem zadaný typ ovladače 0 až D3DEVICES-1 (0=automatický)
extern	int					D3DDevAkt;			// typ aktivního ovladače (0=není,1=HAL,2=TnLHal,3=REF,4=RGB,5=MMX,6=Ramp)



// ovladače a rozhraní 3D
enum D3DEVICE {
	NDefDev = 0,			// implicitní ovladač
	NHALDev = 1,			// HAL
	NTnLHalDev = 2,			// TnLHal
	NRefDev = 3,			// REF
	NRGBDev = 4,			// RGB
	NMMXDev = 5,			// MMX
	NRampDev = 6,			// Ramp
	D3DEVICES = 7,			// počet typů ovladačů
};

enum D3INTERFACE {
	NDefInt = 0,			// implicitní rozhraní
	NGL0Int = 1,			// OpenGL 1.0
	NGL1Int = 2,			// OpenGL 1.1
	NGL2Int = 3,			// OpenGL 2.0
	NDX3Int = 4,			// DirectX 3
	NDX5Int = 5,			// DirectX 5
	NDX6Int = 6,			// DirectX 6
	NDX7Int = 7,			// DirectX 7
	NDX8Int = 8,			// DirectX 8
	D3INTERFACES = 9,		// počet typů rozhraní
};

// tabulka ID kódů ovladačů Direct3D 
extern	const GUID* Dev3DTab[D3DEVICES];

// videomód Direct3D
extern	bool				D3DFullScreen;		// je celoobrazovkový režim s Direct3D
extern	int					D3DNumVideoModes;	// počet videomódů v tabulce
extern	VIDEOMODEITEM*		D3DTabVideoModes;	// tabulka videomódů
//extern	CString				D3DListVideoModes;	// textový seznam videomódů

// vlastnosti zařízení
//extern	D3DCAPS8*			D3DCaps8;			// buffer vlastností zařízení Direct3D
extern	int					MaxTextureWidth;	// maximální šířka textury
extern	int					MaxTextureHeight;	// maximální výška textury
extern	int					MaxTextureRepeat;	// maximální počet opakování textury
extern	bool				SquareTexture;		// textury musí být čtvercové
extern	bool				TexturePow2;		// rozměr textur musí být mocnina 2
extern	bool				IsHWRaster;			// je HW rasterizace
extern	bool				IsHWTransLight;		// je HW transformace a osvětlení
extern	bool				IsPureDevice;		// pouze HW obsluha
extern	bool				IsBlending;			// jsou blending operace
extern	bool				IsMagFilter;		// podporuje filtr při zvětšení
extern	bool				IsMinFilter;		// podporuje filtr při zmenšení
extern	bool				IsMipMap;			// podporuje mipmap textury
extern	bool				AlphaGreaterEqual;	// lze porovnávat Alpha	>=
extern	bool				AlphaGreater;		// lze porovnávat Alpha >
extern	int					MaxVertexNum;		// maximální počet vrcholů skutečný
extern	int					MaxVertexNum2;		// maximální počet vrcholů pro renderování
extern	int					MaxTextureStages;	// maximální počet stupňů textur
extern	int					MaxTextureSimult;	// maximální počet textur pro jeden průchod
extern	bool				IsTableFog;			// podporována tabulková mlha
extern	bool				IsVertexFog;		// podporována vrcholová mlha
extern	bool				IsRangeFog;			// podporována vzdálenostní mlha

// přepínače nastavení
extern	bool				D3DWireframeAkt;	// vyplňování ploch aktuálně zapnut
extern	bool				D3DWireframeGlobal;	// vyplňování ploch globálně povoleno
extern	bool				D3DLightOnAkt;		// osvětlení aktuálně zapnuto
extern	bool				D3DLightOnGlobal;	// osvětlení globálně povoleno
extern	bool				D3DShadesAkt;		// plynulé stínování aktuálně zapnuto
extern	bool				D3DShadesGlobal;	// plynulé stínování globálně povoleno
extern	bool				D3DMinFilterAkt[MAX_STAGES];	// filtrace zmenšených textur zapnuta
extern	bool				D3DMinFilterGlobal;	// filtrace zmenšených textur globálně povolena
extern	bool				D3DMagFilterAkt[MAX_STAGES];	// filtrace zvětšených textur zapnuta
extern	bool				D3DMagFilterGlobal;	// filtrace zvětšených textur globálně povolena
extern	bool				D3DMipFilterAkt[MAX_STAGES];	// filtrace vzdálených textur zapnuta
extern	bool				D3DMipFilterGlobal;	// filtrace vzdálených textur globálně povolena


extern	int					D3DAktStage;		// aktivní stupeň textur

extern	bool				D3DRamp;			// je ovladač Ramp

extern	bool				D3DVSync;			// synchronizovat s VSYN

extern	bool				D3DVertFog;			// pouze vektorová mlha (SW driver)


extern	int ReSet3DCount;
extern	int ReSet3DCountN;


extern	bool				D3DCullingUnknown;	// stav přepínače odstraňování ploch neznámý
extern	short				D3DCullingAkt;		// volba zobrazených ploch
extern	bool				D3DCameraInvert;	// je inverzní kamera, invertovat plošky
//extern	bool				D3DCullingGlobal;	// odstraňování ploch globálně povoleno

// funkce Direct3DCreate8
//typedef	IDirect3D8* (WINAPI *DIRECT3DCREATE8) (UINT SDKVersion);
//extern DIRECT3DCREATE8	pDirect3DCreate8;		// funkce Direct3DCreate8 (NULL=není)

// ovladače Direct3D8
extern	bool				IsDirect3D;			// proběhla inicializace Direct3D
//extern	IDirect3D8*			Direct3D;			// objekt Direct3D verze 8 (NULL=není)
//extern  IDirect3DDevice8*	Direct3DDev;		// zařízení Direct3D (NULL=není)
//extern	int					D3DDevUser;			// uživatelem zadaný typ zařízení (0=automatický)
//extern	int					D3DDevAkt;			// typ aktivního zařízení (0=není,1=HAL,2=TnLHal,3=REF,4=RGB,5=MMX,6=Ramp)
//extern	int					D3DIntUser;			// uživatelem zadaný interface (0=automatický)
//extern	int					D3DIntAkt;			// typ aktivního rozhraní (0=není,1=GL0,2=GL1,3=GL2,4=DX4,5=DX5,6=DX6,7=DX7,8=DX8)

// okno Direct3D
extern	bool				D3D;				// je okno Direct3D
//extern	bool				D3DDX;				// je okno 3D - ovladač DirectX
//extern	bool				D3DGL;				// je okno 3D - ovladač OpenGL
extern	bool				D3D2D;				// je kombinace 3D s 2D grafikou
extern	bool				D3DFull;			// okno 3D využívá celou plochu hlavního okna

extern	bool				D3DReturn;			// blokování Alt+Enter při D3D módu

// formáty textur (dwRGBBitCount=0 pro neplatný formát)
extern	DDPIXELFORMAT		D3DTextFormatA8R8G8B8;			// formát textury D3DFMT_A8R8G8B8
extern	DDPIXELFORMAT		D3DTextFormatA4R4G4B4;			// formát textury D3DFMT_A4R4G4B4
extern	DDPIXELFORMAT		D3DTextFormatA1R5G5B5;			// formát textury D3DFMT_A1R5G5B5
extern	DDPIXELFORMAT		D3DTextFormatR8G8B8;			// formát textury D3DFMT_R8G8B8
extern	DDPIXELFORMAT		D3DTextFormatR5G6B5;			// formát textury D3DFMT_R5G6B5
extern	DDPIXELFORMAT		D3DTextFormatX1R5G5B5;			// formát textury D3DFMT_X1R5G5B5
extern	DDPIXELFORMAT		D3DTextFormatX8R8G8B8;			// formát textury D3DFMT_X8R8G8B8
extern	DDPIXELFORMAT		D3DTextFormatX4R4G4B4;			// formát textury D3DFMT_X4R4G4B4
extern	bool				D3DLowText;			// používat nižší kvalitu textur
extern	bool				D3DTextFormatOK;	// nalezen platný formát textur

// formáty Z-bufferů (dwZBufferBitDepth=0 pro neplatný formát) - jen pro hledání formátu Z bufferu
extern	DDPIXELFORMAT		D3DZBufferFormat24S8;			// formát bufferu 24 bitů, stencil 8 bitů
extern	DDPIXELFORMAT		D3DZBufferFormat24S4;			// formát bufferu 24 bitů, stencil 4 bity
extern	DDPIXELFORMAT		D3DZBufferFormat15S1;			// formát bufferu 15 bitů, stencil 1 bit
extern	DDPIXELFORMAT		D3DZBufferFormat32S0;			// formát bufferu 32 bitů, stencil 0 bitů
extern	DDPIXELFORMAT		D3DZBufferFormat24S0;			// formát bufferu 24 bitů, stencil 0 bitů
extern	DDPIXELFORMAT		D3DZBufferFormat16S0;			// formát bufferu 16 bitů, stencil 0 bitů
extern	DDPIXELFORMAT		D3DZBufferFormat8S0;			// formát bufferu 8 bitů, stencil 0 bitů
extern	bool				D3DZBufferFormatOK;				// nalezen platný formát Z-bufferu

// stencil buffer
extern	bool				D3DSBufferFormatOK;				// nalezen Stencil buffer
extern	int					D3DSBufferDepth;				// hloubka Stencil bufferu (bitů)
extern	int					D3DSBufferMax;					// maximální hodnota Stencil bufferu
extern	bool				D3DSBufferClear;				// požadavek k vymazání Stencil bufferu
extern	bool				D3DShadows;						// povoleno používání stínů

// projekční matice (pozor, člen _34 matice nesmí být < 0 !!!)
extern	bool				D3DProjAkt;			// požadavek aktualizace projekční matice
//extern	double				D3DHalfHeight;		// výška horizontu
//extern	double				D3DAspectRatio;		// poměr výška/šířka
extern	double				D3DFrontClip;		// vzdálenost přední omezující roviny
extern	double				D3DBackClip;		// vzdálenost zadní omezující roviny
//extern	double				D3DViewAngle;		// úhel šířky pohledu
extern	D3DMATRIX			D3DProjMatrix;		// projekční matice (nepoužité členy jsou = 0)
extern	int					D3DProjection;		// typ projekce (0=persp, 1=orto, 2=r-persp, 3=r-orto)
extern	bool				D3DRightHand;		// pravoruká projekce (typ projekce 2 a 3)

// pohledová matice (odvozená z kamery)
extern	bool				D3DViewAkt;			// požadavek aktualizace pohledové matice
extern	D3DMATRIX			D3DViewMatrix;		// pohledová matice (vytvořená z objektu kamery)

// kombinovaná matice projekční*pohledová (pro přepočet 2D objektů)
//extern	bool				D3DViewProjAkt;		// aktualizovat výpočet kombinované matice
//extern	D3DMATRIX			D3DViewProjMatrix;	// kombinovaná matice

// rozhraní OpenGL
//extern	bool				IsOpenGL;			// proběhla inicializace OpenGL

//extern	bool				OKOpenGL;			// inicializace OpenGL je úspěšná

//#ifdef OPENGL11
//extern	bool				OKOpenGL11;			// inicializace OpenGL 1.1 je úspěšná
//#endif // OPENGL11

//extern	HINSTANCE			OpenGLLib;			// knihovna OPENGL32.DLL
//extern	HGLRC				GLContext;			// renderovací plocha OpenGL
//extern	HPALETTE			GLPalette;			// palety pro OpenGL

// pohledová matice (odvozená z kamery)
extern	bool				D3DViewAkt;			// požadavek aktualizace pohledové matice
extern	D3DMATRIX			D3DViewMatrix;		// pohledová matice (vytvořená z objektu kamery)

// frustum (odvozeno z pohledové a projekční matice)
extern	bool				D3DFrustumAkt;		// aktualizovat frustum
extern	PLANE				D3DFrustum[6];		// 6 rovin projekčního jehlanu

// rozměry okna Direct3D zadané uživatelem, bez omezení rozsahu
extern	int					D3DX0;				// počátek X okna Direct3D (zleva) zadaný uživatelem
extern	int					D3DY0;				// počátek Y okna Direct3D (zdola) zadaný uživatelem
extern	int					D3DW0;				// šířka okna Direct3D zadaná uživatelem
extern	int					D3DH0;				// výška okna Direct3D zadaná uživatelem

// rozměry okna Direct3D zadané uživatelem, s omezením na plochu (pro zobrazení grafiky)
extern	int					D3DX;				// počátek X okna Direct3D (zleva)
extern	int					D3DY;				// počátek Y okna Direct3D (zdola)
extern	int					D3DW;				// šířka okna Direct3D
extern	int					D3DH;				// výška okna Direct3D

// skutečné rozměry okna Direct3D na obrazovce (omezení na klientské souřadnice okna)
extern	int					D3DLeft;			// skutečný počátek X okna Direct3D
extern	int					D3DTop;				// skutečný počátek Y okna Direct3D (shora)
extern	int					D3DWidth;			// skutečná šířka okna Direct3D
extern	int					D3DHeight;			// skutečná výška okna Direct3D
extern	double				D3DWidthHeight2;	// odmocnina s poměru D3DWidth/D3DHeight

// uživatelské nastavení
//extern	bool				D3DDither;			// použít dithering Direct3D
//extern	bool				D3DRendReq;			// požadavek přerenderování Direct3D
//extern	int					D3DQuality;			// kvalita renderování

// objekty Direct3D
//extern	LPDIRECTDRAWSURFACE D3Surface;			// povrch pro Direct 3D (NULL=není)
//extern	LPDIRECTDRAWSURFACE D3ZBuffer;			// povrch pro Direct 3D Z-buffer (NULL=není)
//extern	LPDIRECT3DVIEWPORT	D3DViewport;		// viewport pro Direct 3D
//extern	LPDIRECT3DMATERIAL	D3DBackMaterial;	// materiál pozadí scény
//extern	D3DMATERIALHANDLE	hD3DBackMaterial;	// handle materiálu pozadí scény

// objekty Direct3DRM
//extern	LPDIRECT3DRM		Direct3DRM;			// objekt Direct3D RM
//extern	LPDIRECT3DRM2		Direct3DRM2;		// objekt Direct3D RM 2
//extern	LPDIRECT3DRMDEVICE	Direct3DRMDev;		// zařízení Direct3D RM
//extern	LPDIRECT3DRMDEVICE2	Direct3DRMDev2;		// zařízení Direct3D RM 2
//extern	LPDIRECT3DRMFRAME2	D3Scene;			// rám scény
//extern	LPDIRECT3DRMFRAME2	D3Camera;			// rám kamery
//extern	LPDIRECT3DRMVIEWPORT D3View;			// okno pohledu


// pozadí scény
extern	DWORD				D3DBackCol;			// barva pozadí scény (MAXDWORD = nemazat)
extern	DWORD				D3DBackColRGB;		// barva pozadí scény ve formátu RGB
extern	D3DCOLORVALUE		D3DBackColF;		// barva pozadí scény ve formátu float
extern	bool				D3DBackColAkt;		// barva pozadí změněna
extern	int					D3DBackText;		// textura pozadí scény (-1 = není)
extern	bool				D3DBackTextAkt;		// textura pozadí změněna
extern	int					D3DBackMat;			// materiál pozadí scény
extern	D3DMATERIAL8		D3DBackMatMat;		// buffer materiálu pozadí scény
//extern	IDirect3DVertexBuffer8*	D3DBackBuf;		// buffer textury Direct3D

// nastavení mlhy
extern	BOOL				D3DFogOn;			// mlha je aktuálně zapnuta (-1 = neznámé)
extern	bool				FogAkt;				// mlhu je nutno aktualizovat
extern	bool				FogKorig;			// korigovaná mlha (přičítá se +4 k typu mlhy)
extern	bool				FogOn;				// mlha je zapnuta
extern	DWORD				FogColor;			// barva mlhy ve formátu Petra BGR (MAXDWORD=vypnuto)
extern	DWORD				FogColorRGB;		// barva mlhy ve formáty RGB
extern	D3DCOLORVALUE		FogColorF;			// barva mlhy ve formátu float
extern	int					FogType;			// typ mlhy (0=lineární, 1=exponenciální, 2=kvadratická)
extern	double				FogStart;			// začátek lineární mlhy
extern	double				FogEnd;				// konec lineární mlhy
extern	double				FogDens;			// hustota exponenciální a kvadratické mlhy (0 až 1)

// ambient osvětlení
extern	DWORD				D3DAmbientCol;		// ambient osvětlení ve formátu Petra BGR
extern	DWORD				D3DAmbientColRGB;	// ambient osvětlení ve formátu RGB
extern	D3DCOLORVALUE		D3DAmbientColF;		// ambient osvětlení ve formátu float
extern	bool				D3DAmbientColAkt;	// ambient osvětlení změněno



//extern	CPicture			D3ScenePic;			// textura pozadí scény
//extern	bool				D3SceneBack;		// je pozadí scény

extern	int					D3DLevel;			// složitost objektů (2 a více)
extern	bool				D3DCreateUpper;		// vytvářet horní podstavu objektů
extern	bool				D3DCreateLower;		// vytvářet dolní podstavu objektů
extern	int					D3D_ID;				// aktivní objekt (rám) Direct3D
extern	int					TerenID;			// naposledy zvolený objekt terénu (-1=není)

//extern	int					D3DFilter0;			// filtrace textur zadaná uživatelem (-1=implicitní)
//extern	int					D3DFilter;			// aktuální filtrace textur
//extern	bool				D3DMipmaps;			// používají se mipmap
extern	double				D3DSmooth;			// vyhlazení ditheringu textur (1=ne)

// zrychlené renderování
//extern	bool				OpenGLQMat;			// používat zkrácené výpočty matic
													// - pohledová matice se násobí
													//   s projekční maticí, pro objekty
													//   se nastaví pouze modelová matice
													// - v jiném případě se pohledová
													//   matice násobí s každou modelovou
													//   maticí
													//   (musí být vypnuto v případě mlhy)

// nastavení kamery a mlhy
//extern	D3DVALUE			FogStart;			// začátek mlhy
//extern	D3DVALUE			FogEnd;				// konec mlhy
//extern	D3DVALUE			FogDens;			// hustota mlhy
//extern	D3DRMPROJECTIONTYPE D3DProj;			// typ projekce


// měření uplynulého času
extern	double D3DLastTime;							// poslední čas renderování
extern	double D3DElapsedTime;						// uplynulý čas od posledního renderování
extern	double D3DAverageTime;						// střadač středního času
extern	double D3DAverageFreq;						// střední frekvence
extern	int D3DAverageFreqI;						// frekvence Integer


#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------

/////////////////////////////////////////////////////////////////////////////
// globální proměnné

extern	HWND		MainFrame;				// hlavní okno aplikace
extern	TCHAR		MainFrameClass[];		// název třídy hlavního okna

extern	int			MainFrameStyleEx;		// rozšířený styl
extern	int			MainFrameStyle;			// styl okna

//extern	TCHAR		MainFrameSubClass[];	// název třídy podokna
extern	bool		MainFrameVisible;		// hlavní okno zobrazeno
extern	HWND		StatusBar;				// stavová lišta

extern	int			MainFrameX;				// souřadnice X středu hlavního okna
extern	int			MainFrameY;				// souřadnice Y středu hlavního okna

extern	bool		ReqClose;				// požadavek uzavření okna od uživatele

extern	bool		StatusVisible;			// příznak viditelnosti stavového řádku

extern	RECT		ClientRect;				// klientská oblast hlavního okna
extern	int			ClientWidth;			// šířka klientské oblasti
extern	int			ClientHeight;			// výška klientské oblasti

//extern	HCURSOR		CurAkt;					// aktivní kurzor
extern	HCURSOR		CurArrow;				// (standardní) šipka
extern	HCURSOR		CurArrow2;				// (standardní) šipka pro DirectX
//extern	HCURSOR		CurWait;				// čekání

extern	CString		AktCaptionText;			// text titulku hlavního okna
extern	CString		AktStatusText;			// zobrazený text stavové lišty

// stav myši (aktualizováno v PreTranslateMessage)
extern	bool		LMouseDown;				// levé tlačítko stisknuto
extern	bool		LMouseClick;			// klik levého tlačítka
extern	bool		LMouseDClick;			// dvojklik levého tlačítka

extern	bool		RMouseDown;				// pravé tlačítko stisknuto
extern	bool		RMouseClick;			// klik pravého tlačítka
extern	bool		RMouseDClick;			// dvojklik pravého tlačítka

extern	POINT		MouseScreen;			// souřadnice myši na displeji
extern	POINT		MouseMain;				// souřadnice myši v hlavním okně
extern	bool		MouseValid;				// příznak aktualizace souřadnice myši

extern	bool		ProgramFill;			// plnění prvku okna programem

// buffer hledaného textu v okně RichEdit
extern	LPTSTR		FindBuff;				// buffer hledaného textu (v okně RichEdit)
extern	bool		FindMatchCase;			// při hledání rozlišena velká a malá písmena
extern	bool		FindWholeWord;			// hledat jen celá slova
extern	HWND		FindDialogBox;			// handle na dialog hledání (NULL=není)
extern	UINT		FindMsgString;			// identifikátor zprávy hledání

/////////////////////////////////////////////////////////////////////////////
// příprava požadovaných rozměrů okna

void AdjustMainFrame(RECT* wrc);


/////////////////////////////////////////////////////////////////////////////
// první zobrazení hlavního okna

void MainFrameShow();


/////////////////////////////////////////////////////////////////////////////
// vytvoření hlavního okna aplikace

bool MainFrameCreate();

/////////////////////////////////////////////////////////////////////////////
// přepočet rozměrů okna Direct3D (podle údajů zadaných uživatelem)
// vrací TRUE=má být okno 3D

bool RecalcD3D();

/////////////////////////////////////////////////////////////////////////////
// inicializace obsluhy DirectDraw (vrací TRUE=je DirectDraw)

bool InitDirectDraw();

/////////////////////////////////////////////////////////////////////////////
// inicializace seznamu videomódů (volá se při prvním použití)

void InitVideoModes();

/////////////////////////////////////////////////////////////////////////////
// setřídění videomódů displeje (na začátku je nejmenší rozlišení a nejnižší počet bitů)

void SortDisplayModes(VIDEOMODEITEM* tab, int num);

/////////////////////////////////////////////////////////////////////////////
// start videomódu (vrací TRUE=OK)

bool StartVideoMode();

/////////////////////////////////////////////////////////////////////////////
// stop aktivního videomódu

void StopVideoMode();

/////////////////////////////////////////////////////////////////////////////
// konverze barvy z formátu Petra na formát D3DCOLORVALUE (float)

void BGRFloat(DWORD src, D3DCOLORVALUE* dst);

/////////////////////////////////////////////////////////////////////////////
// inicializace objektů scény (volá se ještě před inicializací 3D!)

void Init3D();

/////////////////////////////////////////////////////////////////////////////
// nalezení nejbližšího videomódu v tabulce (vrací -1=nenalezeno)

int FindVideoMode(VIDEOMODEITEM* tab, int tabn, int width, int height, int bits);


/////////////////////////////////////////////////////////////////////////////
// aktivace videomódu DirectX (vrací TRUE=OK)

//bool InitDXVideoMode(int width, int height, int bits);


/////////////////////////////////////////////////////////////////////////////
// ukončení obsluhy DirectDraw

//void TermDirectDraw();

/////////////////////////////////////////////////////////////////////////////
// inicializace obsluhy Direct3D (vrací TRUE=je Direct3D)

//bool InitDirect3D();

/////////////////////////////////////////////////////////////////////////////
// přidání formátu textur

void D3DEnumPixelFormat(DDPIXELFORMAT* pf);

/////////////////////////////////////////////////////////////////////////////
// přidání formátu Z-bufferu

void D3DEnumZBufferFormat(DDPIXELFORMAT* pf);

/////////////////////////////////////////////////////////////////////////////
// výběr uživatelského ovladače Direct3D

bool UserD3DDevice();

// přenastavení filtrace textur
//void ReSetFilter();

// přenastavení kvality renderování
//void ReSetD3DQuality();

// přenastavení přepínače Dithering
//void ReSetD3DDither();
	
// záměna modré a červené barvy v RGB
DWORD XRGB(DWORD col);
	
// zjištění parametrů mlhy
//void GetFogPar();

// nastavení parametrů mlhy
//void SetFogPar();
	
// zobrazení animace
//void DispAnimate();
		  
// vyrenderování scény
void D3Render(bool disp);

// animace
//void Animate();

/////////////////////////////////////////////////////////////////////////////
// nastavení stavu Direct3D (nekontroluje rozsahy a platnosti!)

void _fastcall SetD3DRenderState(D3DRENDERSTATETYPE state, DWORD value);

/////////////////////////////////////////////////////////////////////////////
// nastavení módu vyplňování ploch

void SetD3DWireframe(bool on);

/////////////////////////////////////////////////////////////////////////////
// nastavení módu zapnutí osvětlení

void SetD3DLightOn(bool on);

/////////////////////////////////////////////////////////////////////////////
// nastavení módu plynulého stínování

void SetD3DShades(bool on);

/////////////////////////////////////////////////////////////////////////////
// nastavení módu filtrace textur při zvětšení

void SetD3DMagFilter(bool on, int stage);

/////////////////////////////////////////////////////////////////////////////
// nastavení módu filtrace zmenšených a vzdálených textur

void SetD3DMinMipFilter(bool min, bool mip, int stage);

/////////////////////////////////////////////////////////////////////////////
// nastavení odstraňování ploch

void SetD3DCullingKorig(short val);
void SetD3DCulling(short val);

/////////////////////////////////////////////////////////////////////////////
// zapnutí mlhy

void D3DCheckFogOn();

void SetD3DFogOn(BOOL on);

/////////////////////////////////////////////////////////////////////////////
// resetování přepínačů nastavení 

void ResetD3DSwitch();

/////////////////////////////////////////////////////////////////////////////
// uvolnění ovladače Direct3D

void DeSelectD3DDev();

/////////////////////////////////////////////////////////////////////////////
// aktualizace viewportu (např. po změně velikosti okna)

bool UpdateViewport();


/////////////////////////////////////////////////////////////////////////////
// zapnutí/vypnutí celoobrazovkového módu

void SetFullScreen(bool full);


/////////////////////////////////////////////////////////////////////////////
// znovunastavení videomódu v celoobrazovkovém módu

void ReSetDisplayMode();


/////////////////////////////////////////////////////////////////////////////
// aktualizace klientských souřadnic

void InitClientRect(WINITEM* item);


/////////////////////////////////////////////////////////////////////////////
// zobrazení kurzoru čekání (zahájení a ukončení musí být do páru!)

//void BeginWaitCursor();


/////////////////////////////////////////////////////////////////////////////
// vypnutí kurzoru čekání (zahájení a ukončení musí být do páru!)

//void EndWaitCursor();


/////////////////////////////////////////////////////////////////////////////
// nastavení textu stavové lišty

void SetStatusText(CString& text);


/////////////////////////////////////////////////////////////////////////////
// zobrazení stavového řádku

void SetStatusVisible(bool show);


/////////////////////////////////////////////////////////////////////////////
// nastavení titulku pro konzolu

void SetConsoleText();


/////////////////////////////////////////////////////////////////////////////
// zobrazení jména v titulku okna

void SetCaptionText(CString& text);

/////////////////////////////////////////////////////////////////////////////
// obsluha zprávy před rozesláním do oken

BOOL PreTranslateMessage(MSG* msg);


/////////////////////////////////////////////////////////////////////////////
// aktualizace předešlého okna

void AktPrevWindow();
