
/***************************************************************************\
*																			*
*							3D rozhraní - OpenGL 1.0						*
*																			*
\***************************************************************************/

// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI

/////////////////////////////////////////////////////////////////////////////
// definice funkcí OpenGL 1.0

typedef	WINGDIAPI HGLRC (WINAPI *WGLCREATECONTEXT) (HDC);
typedef	WINGDIAPI BOOL (WINAPI *WGLDELETECONTEXT) (HGLRC);
typedef WINGDIAPI HDC (WINAPI *WGLGETCURRENTDC) (void);
typedef	WINGDIAPI BOOL (WINAPI *WGLMAKECURRENT) (HDC, HGLRC);

typedef WINGDIAPI void (APIENTRY *GLALPHAFUNC) (GLenum func, GLclampf ref);
typedef	WINGDIAPI void (APIENTRY *GLBEGIN) (GLenum mode);
typedef WINGDIAPI void (APIENTRY *GLBLENDFUNC) (GLenum sfactor, GLenum dfactor);
typedef	WINGDIAPI void (APIENTRY *GLCLEAR) (GLbitfield mask);
typedef	WINGDIAPI void (APIENTRY *GLCLEARCOLOR) (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
typedef	WINGDIAPI void (APIENTRY *GLCLEARSTENCIL) (GLint s);
typedef	WINGDIAPI void (APIENTRY *GLCOLORMASK) (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
typedef	WINGDIAPI void (APIENTRY *GLCOLOR3F) (GLfloat red, GLfloat green, GLfloat blue);
typedef	WINGDIAPI void (APIENTRY *GLCOLOR3FV) (const GLfloat* v);
typedef	WINGDIAPI void (APIENTRY *GLCOLOR4F) (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef	WINGDIAPI void (APIENTRY *GLCOLOR4FV) (const GLfloat* v);
typedef	WINGDIAPI void (APIENTRY *GLCULLFACE) (GLenum mode);
typedef	WINGDIAPI void (APIENTRY *GLDEPTHFUNC) (GLenum func);
typedef	WINGDIAPI void (APIENTRY *GLDEPTHMASK) (GLboolean flag);
typedef WINGDIAPI void (APIENTRY *GLDEPTHRANGE) (GLclampd znear, GLclampd zfar);
typedef	WINGDIAPI void (APIENTRY *GLDISABLE) (GLenum cap);
typedef	WINGDIAPI void (APIENTRY *GLENABLE) (GLenum cap);
typedef	WINGDIAPI void (APIENTRY *GLEND) (void);
typedef WINGDIAPI void (APIENTRY *GLFOGFV) (GLenum pname, GLfloat* params);
typedef	WINGDIAPI void (APIENTRY *GLFRONTFACE) (GLenum mode);
typedef WINGDIAPI void (APIENTRY *GLGETINTEGERV) (GLenum pname, GLint* params);
typedef WINGDIAPI void (APIENTRY *GLHINT) (GLenum target, GLenum mode);
typedef WINGDIAPI void (APIENTRY *GLLIGHTFV) (GLenum light, GLenum pname, const GLfloat* params);
typedef WINGDIAPI void (APIENTRY *GLLIGHTMODELFV) (GLenum pname, const GLfloat* params);
typedef WINGDIAPI void (APIENTRY *GLLOADIDENTITY) (void);
typedef WINGDIAPI void (APIENTRY *GLLOADMATRIXF) (const GLfloat* m);
typedef WINGDIAPI void (APIENTRY *GLMATERIALFV) (GLenum face, GLenum pname, const GLfloat* params);
typedef WINGDIAPI void (APIENTRY *GLMATRIXMODE) (GLenum mode);
typedef WINGDIAPI void (APIENTRY *GLMULTMATRIXF) (const GLfloat* m);
typedef	WINGDIAPI void (APIENTRY *GLNORMAL3FV) (const GLfloat* v);
typedef WINGDIAPI void (APIENTRY *GLPIXELSTOREI) (const GLenum pname, GLint param);
typedef WINGDIAPI void (APIENTRY *GLPOLYGONMODE) (GLenum face, GLenum mode);
typedef WINGDIAPI void (APIENTRY *GLPOPMATRIX) (void);
typedef WINGDIAPI void (APIENTRY *GLPUSHMATRIX) (void);
typedef WINGDIAPI void (APIENTRY *GLSHADEMODEL) (GLenum mode);
typedef WINGDIAPI void (APIENTRY *GLTEXCOORD2FV) (const GLfloat* v);
typedef WINGDIAPI void (APIENTRY *GLTEXENVI) (GLenum target, GLenum pname, GLint param);
typedef WINGDIAPI void (APIENTRY *GLTEXGENI) (GLenum coord, GLenum pname, GLint param);
typedef WINGDIAPI void (APIENTRY *GLTEXIMAGE2D) (GLenum target, GLint level, GLint components,
							GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels);
typedef WINGDIAPI void (APIENTRY *GLTEXPARAMETERI) (GLenum target, GLenum pname, GLint param);
typedef	WINGDIAPI void (APIENTRY *GLVERTEX2I) (GLint x, GLint y);
typedef	WINGDIAPI void (APIENTRY *GLVERTEX2F) (GLfloat x, GLfloat y);
typedef	WINGDIAPI void (APIENTRY *GLVERTEX3F) (GLfloat x, GLfloat y, GLfloat z);
typedef	WINGDIAPI void (APIENTRY *GLVERTEX3FV) (const GLfloat* v);
typedef WINGDIAPI void (APIENTRY *GLVIEWPORT) (GLint x, GLint y, GLsizei width, GLsizei height);
typedef WINGDIAPI void (APIENTRY *GLSTENCILOP) (GLenum fail, GLenum zfail, GLenum zpass);
typedef WINGDIAPI void (APIENTRY *GLSTENCILFUNC) (GLenum func, GLint ref, GLuint mask);
typedef WINGDIAPI void (APIENTRY *GLSTENCILMASK) (GLuint mask);

/////////////////////////////////////////////////////////////////////////////
// funkce OpenGL 1.0

extern	WGLCREATECONTEXT	pwglCreateContext;
extern	WGLDELETECONTEXT	pwglDeleteContext;
extern	WGLGETCURRENTDC		pwglGetCurrentDC;
extern	WGLMAKECURRENT		pwglMakeCurrent;

extern	GLALPHAFUNC			pglAlphaFunc;
extern	GLBEGIN				pglBegin;
extern	GLBLENDFUNC			pglBlendFunc;
extern	GLCLEAR				pglClear;
extern	GLCLEARCOLOR		pglClearColor;
extern	GLCLEARSTENCIL		pglClearStencil;
extern	GLCOLORMASK			pglColorMask;
extern	GLCOLOR3F			pglColor3f;
extern	GLCOLOR3FV			pglColor3fv;
extern	GLCOLOR4F			pglColor4f;
extern	GLCOLOR4FV			pglColor4fv;
extern	GLCULLFACE			pglCullFace;
extern	GLDEPTHFUNC			pglDepthFunc;
extern	GLDEPTHMASK			pglDepthMask;
extern	GLDEPTHRANGE		pglDepthRange;
extern	GLDISABLE			pglDisable;
extern	GLENABLE			pglEnable;
extern	GLEND				pglEnd;
extern	GLFOGFV				pglFogfv;
extern	GLFRONTFACE			pglFrontFace;
extern  GLGETINTEGERV		pglGetIntegerv;
extern	GLHINT				pglHint;
extern	GLLIGHTFV			pglLightfv;
extern	GLLIGHTMODELFV		pglLightModelfv;
extern	GLLOADIDENTITY		pglLoadIdentity;
extern	GLLOADMATRIXF		pglLoadMatrixf;
extern	GLMATERIALFV		pglMaterialfv;
extern	GLMATRIXMODE		pglMatrixMode;
extern	GLMULTMATRIXF		pglMultMatrixf;
extern	GLNORMAL3FV			pglNormal3fv;
extern	GLPIXELSTOREI		pglPixelStorei;
extern	GLPOLYGONMODE		pglPolygonMode;
extern	GLPOPMATRIX			pglPopMatrix;
extern	GLPUSHMATRIX		pglPushMatrix;
extern	GLSHADEMODEL		pglShadeModel;
extern  GLTEXCOORD2FV		pglTexCoord2fv;
extern	GLTEXENVI			pglTexEnvi;
extern	GLTEXGENI			pglTexGeni;
extern  GLTEXIMAGE2D		pglTexImage2D;
extern  GLTEXPARAMETERI		pglTexParameteri;
extern	GLVERTEX2I			pglVertex2i;
extern	GLVERTEX3F			pglVertex3f;
extern	GLVERTEX3FV			pglVertex3fv;
extern	GLVIEWPORT			pglViewport;
extern	GLSTENCILOP			pglStencilOp;
extern	GLSTENCILFUNC		pglStencilFunc;
extern	GLSTENCILMASK		pglStencilMask;

/////////////////////////////////////////////////////////////////////////////
// data

// OpenGL 1.0
extern	HINSTANCE			D3GL0Lib;		// handle modulu OPENGL32.DLL

// renderovací povrch
extern	HGLRC				D3GL0Cont;		// renderovací povrch
extern	HPALETTE			D3GL0Pal;		// palety OpenGL

/////////////////////////////////////////////////////////////////////////////
// inicializace rozhraní OpenGL 1.0 (vrací TRUE=rozhraní je vytvořeno)

bool D3GL0InitD3();
void D3GL0TermD3();

/////////////////////////////////////////////////////////////////////////////
// nastavení viewportu (vrací TRUE=operace OK)

void D3GL0SetView();
void D3GL0SizeView();
void D3GL0MoveView();
	   
/////////////////////////////////////////////////////////////////////////////
// inicializace palet pro OpenGL

void D3GL0InitPal(HDC dc);
void D3GL0TermPal();

/////////////////////////////////////////////////////////////////////////////
// vytvoření kontextu zařízení (vrací TRUE=OK)

bool D3GL0InitCont();
void D3GL0TermCont();

/////////////////////////////////////////////////////////////////////////////
// zahájení funkce zařízení (vrací TRUE=inicializace OK)
// typ zařízení: 1=HAL,2=TnLHal,3=REF,4=RGB,5=MMX,6=Ramp,7=Null

bool D3GL0Start(int type);
void D3GL0Stop();

/////////////////////////////////////////////////////////////////////////////
// vymazání renderovací plochy

void D3GL0Clear();

/////////////////////////////////////////////////////////////////////////////
// zobrazení renderovacího bufferu

void D3GL0Disp();

/////////////////////////////////////////////////////////////////////////////
// aktualizace projekční matice

void D3GL0AktProj();

/////////////////////////////////////////////////////////////////////////////
// aktualizace pohledové matice

void D3GL0AktView();

/////////////////////////////////////////////////////////////////////////////
// zahájení renderování scény

void D3GL0Begin();

/////////////////////////////////////////////////////////////////////////////
// ukončení renderování scény

void D3GL0End();

/////////////////////////////////////////////////////////////////////////////
// vyrenderování jednoho objektu

void D3GL0Render(D3DFITEM* item);

/////////////////////////////////////////////////////////////////////////////
// resetování objektu před ukončením ovladače

void D3GL0Reset(D3DFITEM* item);

/////////////////////////////////////////////////////////////////////////////
// aktualizovat přepínač vyplňování ploch

void D3GL0AktWireframe();

/////////////////////////////////////////////////////////////////////////////
// aktualizovat přepínač osvětlení

void D3GL0AktLightOn();

/////////////////////////////////////////////////////////////////////////////
// aktualizovat přepínač plynulého stinování

void D3GL0AktShades();

/////////////////////////////////////////////////////////////////////////////
// aktualizovat přepínač odstraňování ploch

void D3GL0AktCulling();

/////////////////////////////////////////////////////////////////////////////
// aktualizovat přepínač filtrace zvětšených textur

void D3GL0AktMagFilter(int stage);

/////////////////////////////////////////////////////////////////////////////
// aktualizovat přepínač filtrace zmenšených a vzdálených textur

void D3GL0AktMinMipFilter(int stage);

/////////////////////////////////////////////////////////////////////////////
// aktualizace položky světla

void D3GL0AktLight(int inx, D3DLITEM* item);

/////////////////////////////////////////////////////////////////////////////
// nastavení materiálu

void D3GL0SetMaterial(D3DMITEM* item);

/////////////////////////////////////////////////////////////////////////////
// resetování položky textury před ukončením ovladače

void D3GL0TextureRes(D3DTITEM* item);

/////////////////////////////////////////////////////////////////////////////
// nastavení a zapnutí textury

void D3GL0TextureSet(D3DTITEM* item, int stage);

/////////////////////////////////////////////////////////////////////////////
// zapnutí původní textury

void D3GL0TextureOn(D3DTITEM* item, int stage);

/////////////////////////////////////////////////////////////////////////////
// vypnutí textury

void D3GL0TextureOff(int stage);

/////////////////////////////////////////////////////////////////////////////
// resetování položky materiálu

void D3GL0MatReset(D3DMITEM* item);

/////////////////////////////////////////////////////////////////////////////
// resetování položky světla

void D3GL0LightReset(int inx, D3DLITEM* item);

////////////////////////////////////////////////////////////////////
// zobrazení textury OpenGL

void GLTexture(PICTUREDATA* picdata, BOOL mips);

/////////////////////////////////////////////////////////////////////////////
// aktualizace ambient osvětlení

void D3GL0AktAmbient();

/////////////////////////////////////////////////////////////////////////////
// volná videopaměť

int D3GL0Free();

/////////////////////////////////////////////////////////////////////////////
// aktualizace mlhy

void D3GL0AktFog();

/////////////////////////////////////////////////////////////////////////////
// zapnutí mlhy

void D3GL0FogOn(BOOL on);

/////////////////////////////////////////////////////////////////////////////
// aktualizace stavu

void D3GL0AktState();

/////////////////////////////////////////////////////////////////////////////
// aktualizace blending operací objektu

void D3GL0AktBlend(D3DFITEM* item);

/////////////////////////////////////////////////////////////////////////////
// aktualizace referenční úrovně alfa

void D3GL0AktAlfa(D3DFITEM* item);

/////////////////////////////////////////////////////////////////////////////
// nastavení operace barev

void D3GL0SetColorOp(int op, int arg1, int arg2, int stage);

/////////////////////////////////////////////////////////////////////////////
// nastavení adresování textur

void D3GL0SetAddress(int adru, int adrv, int stage);

/////////////////////////////////////////////////////////////////////////////
// nastavení barvy okolí textur

//void D3GL0SetBorder(D3DFITEM* item, int stage);

/////////////////////////////////////////////////////////////////////////////
// aktualizace palet

void D3GL0AktPalette(HWND wnd);

/////////////////////////////////////////////////////////////////////////////
// nastavení Z funkce (1 až 8, v kódu DirectX, 0=vypnuto)

void D3GL0SetZFunc(int func);

/////////////////////////////////////////////////////////////////////////////
// nastavení Z zápisu

void D3GL0SetZWrite(BOOL write);

/////////////////////////////////////////////////////////////////////////////
// povolení stencil operací

void D3GL0SEnable(BOOL enable);

/////////////////////////////////////////////////////////////////////////////
// nastavení stencil operací

void D3GL0SetSOp(int fail, int zfail, int zpass);

/////////////////////////////////////////////////////////////////////////////
// nastavení stencil testu

void D3GL0SetSFunc(int func, int ref, DWORD mask);

/////////////////////////////////////////////////////////////////////////////
// nastavení zápisové masky do stencil bufferu

void D3GL0SetSMask(DWORD mask);

/////////////////////////////////////////////////////////////////////////////
// vyrenderování stínu jednoho objektu

void D3GL0Shadow(D3DFITEM* item);

/////////////////////////////////////////////////////////////////////////////
// vykreslení stínů

void D3GL0Shadows();


#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------

