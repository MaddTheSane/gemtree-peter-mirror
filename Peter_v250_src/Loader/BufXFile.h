
/***************************************************************************\
*																			*
*								Buffery	objektů X souboru					*
*																			*
\***************************************************************************/

// --------------------- vypnuto pro MINI verzi --------------------
#ifndef _MINI

/////////////////////////////////////////////////////////////////////////////
// načtení objektu z bufferu

void ReadMesh(BYTE* data, int size, CString path);

/////////////////////////////////////////////////////////////////////////////
// duplikace datového bloku

void DuplikData(void** adr, int size);


#endif // _MINI
// --------------------- konec vypnutí pro MINI verzi -------------------
