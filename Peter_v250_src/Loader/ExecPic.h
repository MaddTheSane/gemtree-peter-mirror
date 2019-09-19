
/***************************************************************************\
*																			*
*							Provádění programu - obrázek					*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// inline volání příkazu - obrázek

inline void FPicture(CPicture& pic)
{
	ExecItem++;
	ExecItem[-1].ExecPic(pic);
}


void _fastcall FPicGet(CPicture& pic);			// načtení výřezu obrázku z plochy

// funkce
void _fastcall FPicFunc(CPicture& pic);			// funkce s návratem obrázku

// proměnné
void _fastcall FPicEmpty(CPicture& pic);		// prázdný obrázek
void _fastcall FPicObj(CPicture& pic);			// globální obrázek (Data = index)
void _fastcall FPicObjList(CPicture& pic);		// globalní obrázek v seznamu (Data = index, List = seznam)
void _fastcall FPicLoc(CPicture& pic);			// lokální obrázek (Data = index)
void _fastcall FPicLocList(CPicture& pic);		// lokální obrázek v seznamu (Data = index, List = seznam)

void _fastcall FGetFilePicture(CPicture& pic);	// načtení obrázku ze souboru

void _fastcall FPicXFlip(CPicture& pic);		// horizontální převrácení obrázku
void _fastcall FPicYFlip(CPicture& pic);		// vertikální převrácení obrázku
void _fastcall FPicZoom(CPicture& pic);			// změna velikosti obrázku
void _fastcall FPicLevel(CPicture& pic);		// změna jasu obrázku
void _fastcall FPicRotate(CPicture& pic);		// otočení obrázku
void _fastcall FGetWindowPic(CPicture& pic);	// načtení obrázku okna
void _fastcall FMapPicture(CPicture& pic);		// konverze plochy na obrázek
void _fastcall FIconPicture(CPicture& pic);		// konverze ikony na obrázek
void _fastcall FPicMask(CPicture& pic);			// maskování obrázku
void _fastcall FPicXCol(CPicture& pic);			// záměna barev obrázku
void _fastcall FPicSCol(CPicture& pic);			// náhrada barvy obrázku


void _fastcall FGetD3DTexture(CPicture& pic);	// zjištění obrázku textury Direct3D objektu
void _fastcall FGetD3DSceneTexture(CPicture& pic);	// zjištění obrázku textury Direct3D pozadí scény
