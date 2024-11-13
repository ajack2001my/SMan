#include <eikenv.h>
#include <e32def.h>
#include <eikimage.h>
#include <e32std.h>
#include <barsread.h>		// TResourceReader

#include "systemicon.h"
#include "config.h"

#if !defined(__VIEWBASE_H__)
#define __VIEWBASE_H__

class CViewBase : public CCoeControl
{
public:
	CViewBase(CConfig *cData);
	~CViewBase();
	void doViewActivated();
	void DrawSystemIcon(TInt aIconId, TBool iDoDraw);
	void InitBJackIcon();
	void InitCellAreaIcon();
	void generateZoomFont(TInt zoomLevel);
	void releaseFont();
	TUid CurrentViewUid();

	TPoint iBJackIconPosition;
	TPoint iCellAreaIconPosition;
	
	TUint activateCount;
	CConfig *configData;
	TInt viewId;
	CFont *font;
	TBool firstTime;
	
	TFileName appPathNoExt;
	CArrayPtrSeg<CCoeControl>* controlsArray;
	int CountComponentControls() const;
	CCoeControl* ComponentControl(int aIndex) const;
	void Draw(const TRect& aRect) const;
	
private:
	CEikImage *iBJackIconLabel[NUM_BJACK_ICONS];
	CEikImage *iCellAreaIconLabel[NUM_CELLAREA_ICONS];	
	TInt iBJackIconLabelCurrentImage;
	TInt iCellAreaIconLabelCurrentImage;
};

#endif