
/***************************************************************************\
*																			*
*								Historie editace							*
*																			*
\***************************************************************************/

namespace HistEdit
{

// vynulování bufferu historie
	void DelAll();

// zrušení jedné položky z bufferu
	void Del(int index);

// zrušení položek v bufferu (včetně editované) podle dat (pro sprajt pic=-1 všechny obrázky)
// při chybě paměti vrací FALSE
	bool Del(int bufID, int index, int sprite, int pic);

// aktualizace voleb pro obsluhu historie
	void Update();

// uložení editovaného prvku do bufferu historie
	void Push();

// návrat editovaného prvku z bufferu historie (z aktivního ukazatele)
	void Pop();

// předešlá historie
	void OnPrev();

// následující historie
	void OnNext();
}
