/*** DECLARATIONS ***/

#include <gtk/gtk.h> /* The Gee! I wonder why they implemented it that way Toolkit */

/* Typedefs */
typedef unsigned int obj;
typedef unsigned int vect;
typedef int pos;

/* For history */
typedef unsigned int histbit;

struct histnode_t;
typedef struct histnode_t histnode;
struct histhash_t;
typedef struct histhash_t histhash;

struct histnode_t {
  histbit *history;
  vect direction;
  histnode *next;
};

struct histhash_t {
  int size; /* Not the size of the hash, but the size of the histories within */
  int hashsize; /* The size of the hash, for freehash */
  histnode **history;
};

/* Objects within Maze */
#define ObNone 0              /* Floor */
/* Simple S*/
#define ObSULDRMirror 1       /* Simple UpLeft/DownRight Mirror \ */ 
#define ObSURDLMirror 2       /* Simple UpRight/DownLeft Mirror / */
/* Flipper F*/
#define ObFULDR_URDLMirror 3  /* Flipping ULDR to URDL Mirror */
#define ObFURDL_ULDRMirror 4  /* Flipping URDL to ULDR Mirror */
#define ObFULDR_BlkMirror 5   /* Flipping ULDR to Block Mirror */
#define ObFBlk_URDLMirror 6   /* Flipping Block to URDL Mirror */
#define ObFURDL_BlkMirror 7   /* Flipping URDL to Block Mirror */
#define ObFBlk_ULDRMirror 8   /* Flipping Block to ULDR Mirror */
/* Box and Sink B*/
#define ObBBox 9              /* Box/Sink Block Mirror */
#define ObBSink 10            /* Box/Sink Ball Eater */
/* Axis A */
#define ObAVertMirror 11      /* Vertical Axis Mirror ||*/
#define ObAHorizMirror 12     /* Horizontal Axis Mirror = */
#define ObAFVertMirror 13     /* Vertical Flipping Axis Mirror */
#define ObAFHorizMirror 14    /* Horizontal Flipping Axis Mirror */
/* Rotator R*/
#define ObRClock 15           /* Clockwise Rotator */
#define ObRCClock 16          /* Counterclockwise Rotator */
#define ObRFClock 17          /* Clockwise Flipping Rotator */
#define ObRFCClock 18         /* Counterclockwise Flipping Rotator */
/* One-way O*/
#define ObOTULDRMirror 19     /* One-Way Top UpLeft/DownRight Mirror */ 
#define ObOBULDRMirror 20     /* One-Way Bottom UpLeft/DownRight Mirror */ 
#define ObOTURDLMirror 21     /* One-Way Top UpRight/DownLeft Mirror */
#define ObOBURDLMirror 22     /* One-Way Bottom UpRight/DownLeft Mirror */ 
/* Evil E */
#define ObEOFCTULDRMirror 23     /* Evil Clockwise Flipping One-Way Top UpLeft/DownRight Mirror */ 
#define ObEOFCBURDLMirror 24     /* Evil Clockwise Flipping One-Way Bottom UpRight/DownLeft Mirror */ 
#define ObEOFCBULDRMirror 25     /* Evil Clockwise Flipping One-Way Bottom UpLeft/DownRight Mirror */
#define ObEOFCTURDLMirror 26     /* Evil Clockwise Flipping One-Way Top UpRight/DownLeft Mirror */ 

#define ObEOFCCBULDRMirror 27    /* Evil Counterclockwise Flipping One-Way Bottom UpLeft/DownRight Mirror */
#define ObEOFCCBURDLMirror 28    /* Evil Counterclockwise Flipping One-Way Bottom UpRight/DownLeft Mirror */ 
#define ObEOFCCTULDRMirror 29    /* Evil Counterclockwise Flipping One-Way Top UpLeft/DownRight Mirror */ 
#define ObEOFCCTURDLMirror 30    /* Evil Counterclockwise Flipping One-Way Top UpRight/DownLeft Mirror */ 

#define ObEMULDRMirror 31      /* Evil Moving UpLeft/DownRight Mirror */   
#define ObEMURDLMirror 32      /* Evil Moving UpRight/DownLeft Mirror */
#define NumObjects 33

/* Classes */
#define ClSimple 0   /* Objects 1-2 */
#define ClFlipper 1  /* Objects 3-8 */
#define ClBoxSink 2  /* Objects 9-10 */
#define ClAxis 3     /* Objects 11-14 */
#define ClRotator 4  /* Objects 15-18 */
#define ClOneWay 5   /* Objects 19-22 */
#define ClEvil 6     /* Objects 23-32 */
#define NumClasses 7

/* Draggables: Like objects, but only one version of flipping types (indicated by "cycle"). */
#define DrNum 20 /* Total # of draggables */
#define DrNone 255
#define DrSULDRMirror 0
#define DrSURDLMirror 1
#define DrF2CycleMirror 2
#define DrF4CycleMirror 3
#define DrBBox 4
#define DrBSink 5
#define DrAVertMirror 6
#define DrAHorizMirror 7
#define DrA2CycleMirror 8
#define DrRClock 9
#define DrRCClock 10
#define DrR2Cycle 11
#define DrOTULDRMirror 12
#define DrOBULDRMirror 13
#define DrOTURDLMirror 14
#define DrOBURDLMirror 15
#define DrEOFC4CycleMirror 16
#define DrEOFCC4CycleMirror 17
#define DrEMULDRMirror 18
#define DrEMURDLMirror 19
/* Borders - The margins on the screen.  I *almost* don't need these.  But where to put arrows, etc? */
#define BoU  0 /* Calculated by adding their versions +1, then subtracting 1. */
#define BoD  1
#define BoL  2
#define BoUL 3 /* BoU+1 + BoL+1 - 1 */
#define BoDL 4
#define BoR  5
#define BoUR 6
#define BoDR 7
/* Places - Where a drag originates from */
#define PlNowhere 0
#define PlToolbar 1
#define PlLevel 2
/* Sprites */
#define SpMarble 0
#define SpRing 1
/* Directions */
#define DUp 0   /* This is set up in such a way as to allow clockwise rotates.  Mirrors still work. */
#define DRight 1
#define DDown 2
#define DLeft 3
#define DDeath 4
/* Markers */
#define MkNone 0
#define MkCircle 1
#define MkTriangle 2
#define MkSquare 3
#define NumMarks 3
/* Toolbar Icons */
#define INew 0 /* Then Open, then Save..? */
#define IStop 1
#define ICheck 2
#define INet 3
#define INetTurn 4
#define IFlipBoards 5
#define IFlipBack 6
#define IAbout 7
/* Dialog Icons */
#define IOpen 8
#define ISave 9
#define NumMainIcons 8
#define NumDialogIcons 2 
#define NumAllIcons (NumMainIcons + NumDialogIcons)
/* Categories */
#define CatNone 0
#define CatShared 1
#define CatSeparate 2

/* Returns */
#define RetNormal 0
#define RetChanged 1 /* i.e. something rotated */
#define RetMovement 2 /* i.e. moving mirror */
#define RetEaten -1 /* i.e. disappears due to inf loop or pit */
/* Universal Constants */
#define GFPVersion 1 /* Version stamp for config file */
#define GFPNetVer "0.25" /* Network version, same as actual version */
#define GFPSaveVer 1 /* Version stamp for saved games */
#define PackSize 2 /* Objects need 2 bits */
#define PackBits (8 / PackSize) /* Objects that fit in a byte */
#define PackBig (PackBits*sizeof(int)) /* Objects that fit in an int */
#define pixwidth 32
#define pixheight 32
#define pixsize 32 /* Bad - Assumes square.  Well, it should be anyway. */
#define frameskip 4
#define skipsize (pixsize / frameskip)
#define histsize 4 /* Rather fixed at 4 */



/* Function Declarations */
/* From main.c */
void initmainwin();
void shownetturn();
void mainsetstat(char *msg);
gint anim_loop_timeout(gpointer data);
void gamebox(char *title, char *msg);

/* From interface.c */
void borderdraw();
void histdraw(unsigned int item, unsigned char drawmarble, GdkPixmap *ptarrowpix[], GdkBitmap *ptarrowmask[]);
void bardraw();
void borderrefresh();
void fullgriddraw();
void drawtile(int x, int y);
void resizelevel();

/* From dialog.c */
void initdialog();

/* From convert.c */
obj bufselect(pos position);
obj nextobj(obj object);
obj objtodr(obj oject);
obj drtoobj(obj drag);
char *classname(obj class);
char *propname(unsigned int property);
obj classleftpic(obj class);
obj classrightpic(obj class);

/* From logic.c */
void grideval(pos x, pos y, vect dir, obj *gr);

/* From history.c */
histhash *inithist(obj *gr);
int histcheck(pos xpos, pos ypos, vect dir, obj *gr, histhash *hist);
void freehist(histhash *hash);

/* From multi.c */
void initnetwin();
void netsend(char type, ...);
void netkill();
void setstat(char *msg);



