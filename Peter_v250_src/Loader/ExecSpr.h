
/***************************************************************************\
*																			*
*							Provádění programu - sprajt						*
*																			*
\***************************************************************************/


/////////////////////////////////////////////////////////////////////////////
// inline volání příkazu - sprajt

inline void FSprite(CSprite& sprite)
{
	ExecItem++;
	ExecItem[-1].ExecSpr(sprite);
}


// funkce
void _fastcall FSpriteFunc(CSprite& sprite);		// funkce s návratem sprajtu

// proměnné
void _fastcall FSpriteEmpty(CSprite& sprite);		// prázdný sprajt
void _fastcall FSpriteObj(CSprite& sprite);			// globální sprajt (Data = index)
void _fastcall FSpriteObjList(CSprite& sprite);		// globalní sprajt v seznamu (Data = index, List = seznam)
void _fastcall FSpriteLoc(CSprite& sprite);			// lokální sprajt (Data = index)
void _fastcall FSpriteLocList(CSprite& sprite);		// lokální sprajt v seznamu (Data = index, List = seznam)

void _fastcall FGetFileSprite(CSprite& sprite);		// načtení sprajtu ze souboru
