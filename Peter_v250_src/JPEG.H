

/***************************************************************************\
*																			*
*							Obsluha JPEG souboru							*
*																			*
\***************************************************************************/

namespace JPEG
{

// dekomprese dat JPEG (vrací TRUE=operace OK)
	BOOL Decomp(CFileMap* inbuf, CBuf* outbuf);
}
