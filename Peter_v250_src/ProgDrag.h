
/***************************************************************************\
*																			*
*							Obsluha tažení ikon								*
*																			*
\***************************************************************************/

// parametry tažení
extern	BOOL		Draging;			// příznak probíhajícího tažení
extern	int			DragIconWidth;		// šířka tažené ikony
extern	int			DragIconHeight;		// výška tažené ikony
extern	int			DragHotSpotX;		// bod uchycení prvku X
extern	int			DragHotSpotY;		// bod uchycení prvku Y

/////////////////////////////////////////////////////////////////////////////
// zahájení taženi

void ProgOnBeginDrag(HWND hWnd, TV_ITEM* tvi, int x, int y, BOOL right);


/////////////////////////////////////////////////////////////////////////////
// posun myši při tažení

void ProgOnMoveDrag(UINT flags, int x, int y);


/////////////////////////////////////////////////////////////////////////////
// ukončení tažení

void ProgOnEndDrag();


/////////////////////////////////////////////////////////////////////////////
// přerušení tažení

void BreakDrag();
