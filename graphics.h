/* Main window's widget stuff */

extern GtkWidget *mainwin;
extern GtkStyle  *mainstyle;
extern GtkWidget *layoutwin, *layoutpixmap;
extern GtkWidget *winpack;
extern GtkWidget *controlpack;
extern GtkWidget *testbutton;
extern GtkWidget *leveldraw, *tooldraw;
extern GtkWidget *levelscroll, *toolscroll;
extern GtkWidget *mainbar;
extern GtkWidget *mainbarbut[NumMainIcons];
extern GtkWidget *barpixwid[NumAllIcons];
extern GtkWidget *mainstatbar;

extern GdkPixmap *tilepic[NumObjects]; /* All them object tiles.  Square. */
extern GdkPixmap *borderpix; /* The border pictures.  Square. */
extern GdkPixmap *arrowpix[4]; /* Shows where it's supposed to come out */
extern GdkPixmap *warrowpix[4]; /* White arrows for recent move */
extern GdkPixmap *markpix[NumMarks]; /* Markers for board notes */
extern GdkPixmap *barpix[NumAllIcons]; /* Icons for toolbar */
extern GdkPixmap *marblepix;

extern GdkPixmap *iconpix;
extern GdkPixmap *vdeltabuf, *hdeltabuf;

extern GdkBitmap *arrowmask[4]; /* 4 different arrows, so 4 masks. (Strip method doesn't work) */
extern GdkBitmap *markmask[NumMarks];
extern GdkBitmap *barmask[NumAllIcons];
extern GdkBitmap *trashmask;
extern GdkBitmap *marblemask;
extern GdkBitmap *iconmask;
extern GdkGC *transgc;

extern GdkPixmap *levelpixmap, *toolpixmap;


