/* graphics.h - GFingerPoken main window's widget stuff
 * Copyright 1999-2000  Martin Hock
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

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


