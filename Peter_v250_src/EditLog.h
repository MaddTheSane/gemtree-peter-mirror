
/***************************************************************************\
*																			*
*							Editor logických hodnot							*
*																			*
\***************************************************************************/

#define EDITLOGID 30231

namespace EditLog
{
// inicializace při startu
	void StartInit();

// zahájení editace
	void BegEdit(int index);

// přesun editačního pole
	HDWP MoveEdit(HDWP hdwp);

// zobrazení
	void Disp();

// přepnutí přepínače
	void Switch();
}
