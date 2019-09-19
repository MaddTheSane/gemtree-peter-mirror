
/***************************************************************************\
*																			*
*									Knihovny								*
*																			*
\***************************************************************************/

namespace ProgLib
{

// proměnné
extern	BOOL	m_Loading;			// probíhá obsluha načítání
extern	int		m_Parent;			// index položky k načtení obsahu
extern	int		m_Root;				// ROOT položka větve
extern	int		m_Func;				// identifikace funkce
extern	BOOL	m_AltExt;			// příznak alternativní přípony (JPG, MP3, RMI)
extern	CText	m_SubPath;			// podcesta (s "\" na konci nebo prázdný)
extern	CText	m_Path;				// cesta do adresáře (s "\" na konci)
extern	CText	m_Aliases;			// jméno INI souboru s ALIASES
extern	CText	m_Ext;				// přípona souborů 3 znaky
extern	CText	m_Ext2;				// alternativní přípona souborů 3 znaky (prázdné = není)
extern	CText	m_Name;				// plné jméno souboru
extern	CText	m_NameKey;			// jméno souboru použité jako klíč INI (jméno s příponou)
extern	CText	m_IconName;			// jméno ikony k souboru (prázdné = není)

// statická inicializace obsluhy
	void StartInit();

// příprava jména souboru (musí být platný potomek s platným rodičem!)
	void _fastcall InitName(int index);

// příprava cesty (index = poslední adresář)
	void _fastcall InitPath(int index);

// načtení obsahu položky v okně tříd
	void Load(int index);

// zrušení položky z bufferu tříd (se zrušením souboru)
	void Delete(int index);

// kopie položky z jiného okna (musí být platné!)
	int Copy(int parent, int pos, int bufID, int src);

// přejmenování položky v okně
	int Move(int parent, int pos, int src);
};
