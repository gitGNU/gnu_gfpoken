/* initial.h - GFingerPoken global variables definitions
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

/* Variable Declarations */
int gamestatus=0; /* 0 = starting, 1 = playing */
unsigned int dragbuf[DrNum]; /* Draggable buffer for list on right */
unsigned int bufsize, originalbuf; /* Number of items in buffer */
obj dragitem = DrNone;
pos dragorigin = PlNowhere;
pos dragx, dragy;

pos gridx = 5, gridy = 4; /* Size of world */
obj *grid = NULL;
obj *invisigrid = NULL;
obj *markgrid = NULL;

/* For animation */
pos animsrcx, animsrcy;
/*pos animdestx, animdesty; */
int animx, animy;
int oldanimx, oldanimy;
vect animsrcdir, animdir; /* Use D directions */
pos invisx, invisy;
vect invisdir;
int animmode = 0; /* 0 = off, 1 = start, 2 = in progress */
int animframe; /* 0 through pixwidth/height - 1 */
int checked = 0; /* 0 = unchecked, 1 = checked */
int netmode = 0; /* 0 = no network, 1 = client, 2 = server */
int category = 0; /* 0 = not playing, 1 = shared board, 2 = separate boards */
int paralyzed = 0; /* 0 = local (my turn), 1 = remote (your turn) */
int completed = 0; /* 0 = still in process of moving, 1 = have performed test */
int opponentdone = 0; /* 0 = opponent still moving, 1 = opponent done */
int pendack = 0; /* Pending acks */
int boardshow = 0; /* 0: Show the hidden board */

/* The position-saving code doesn't work because gdk_window_get_position() returns erroneous information */
/* int setmainpos = 0; * Are mainxpos and mainypos valid? */
/* gint mainxpos, mainypos; * X and Y position of main window */

pos outy, outx; /* Registers for grideval's output - DAMN I hate functions */
vect outdir;
int evalret = RetNormal;
pos updy, updx; /* For moving mirrors, the update places */

int deltabufwidth, deltabufheight;
int deltabufx, deltabufy;

pos arrhistptr = 0; /* This is a queue */
pos arrinused[histsize]={0,0,0,0};
pos arroutused[histsize]={0,0,0,0};
pos arroutx[histsize], arrouty[histsize]; 
pos arroutdir[histsize]; 
pos arrinx[histsize], arriny[histsize];
vect arrindir[histsize];

unsigned int density=30; /* % of squares that should be filled */
unsigned int densityfuzz=10; /* +/- densityfuzz% */
unsigned int classweight[NumClasses]={100,75,0,0,0,0,0};

/* GTK stuff */

GtkWidget *mainwin;
GtkStyle  *mainstyle;
GtkWidget *layoutwin, *layoutpixmap;
GtkWidget *winpack;
GtkWidget *controlpack;
GtkWidget *leveldraw, *tooldraw;
GtkWidget *levelscroll, *toolscroll;
GtkWidget *mainbar;
GtkWidget *mainbarbut[NumMainIcons];
GtkWidget *barpixwid[NumAllIcons];
GtkWidget *mainstatbar;

GdkPixmap *tilepic[33]; /* All them object tiles.  Square. */
GdkPixmap *borderpix; /* The border pictures.  Square. */
GdkPixmap *arrowpix[4]; /* Shows where it's supposed to come out */
GdkPixmap *warrowpix[4]; /* White arrows for recent move */
GdkPixmap *markpix[NumMarks]; /* Markers for board notes */
GdkPixmap *barpix[NumAllIcons]; /* Icons for toolbars */
GdkPixmap *marblepix;

GdkPixmap *iconpix;
GdkPixmap *vdeltabuf, *hdeltabuf;

GdkBitmap *arrowmask[4]; /* 4 different arrows, so 4 masks. (Strip method doesn't work) */
GdkBitmap *markmask[NumMarks];
GdkBitmap *barmask[NumAllIcons]; 
GdkBitmap *trashmask;
GdkBitmap *marblemask;
GdkBitmap *iconmask;

GdkGC *transgc;



GdkPixmap *levelpixmap = NULL, *toolpixmap = NULL;



histhash *histkeeper = NULL;
