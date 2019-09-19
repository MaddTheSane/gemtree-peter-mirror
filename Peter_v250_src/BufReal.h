
/***************************************************************************\
*																			*
*							Buffer reálných čísel							*
*																			*
\***************************************************************************/

class CReal
{
	double	m_Data;
public:
	inline	CReal() {}
	inline	CReal(double data) { m_Data = data; }
	inline	const CReal operator= (const double src) { m_Data = src; return *this; }
	inline	operator double() const { return m_Data; }
	inline	void Init() { m_Data = 0; }
	inline	void Init(double data) { m_Data = data; }
	inline	void Term() {}
	inline	void Empty() { m_Data = 0; }
};

class CBufReal : public CBuffer<CReal>
{

// ---------------------------- veřejné funkce ------------------------------

public:

// načtení čísla ze souboru (načítá do proměnné, TRUE=operace OK)
	bool LoadFile(double& data, CText jmeno);

// uložení čísla do souboru formátu NUM (FALSE=chyba)
	bool SaveFile(double data, CText jmeno) const;
};
