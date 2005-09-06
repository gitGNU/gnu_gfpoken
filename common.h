/* common.h - include file for GFingerPoken
 * Copyright 1999-2000  Martin Hock
 * Copyright 2005  Bas Wijnen <shevek@fmf.nl>
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
int gamestatus;
extern unsigned int dragbuf[DrNum]; /* Draggable buffer for list on right */
extern unsigned int bufsize, originalbuf; /* Number of items in buffer */
extern obj dragitem;
extern pos dragorigin;
extern pos dragx, dragy;

extern pos gridx, gridy; /* Size of world */
extern obj *grid;
extern obj *invisigrid;
extern obj *markgrid;


/* For animation */
extern pos animsrcx, animsrcy;
/*pos animdestx, animdesty; */
extern int animx, animy;
extern int oldanimx, oldanimy;
extern vect animsrcdir, animdir; /* Use D directions */
extern pos invisx, invisy;
extern vect invisdir;
extern int animmode; /* 0 = off, 1 = start, 2 = in progress */
extern int animframe; /* 0 through pixwidth/height - 1 */
extern int checked; /* 0 = unchecked, 1 = checked */
extern int netmode; /* 0 = no network, 1 = client, 2 = server */
extern int category; /* 0 = synchronous, 1 = assynchronous (for net mode) */
extern int paralyzed; /* 0 = local (my turn), 1 = remote (your turn) */
extern int completed; /* 0 = still in process of moving, 1 = have performed test */
extern int opponentdone; /* 0 = opponent still moving, 1 = opponent done */
extern int pendack; /* Number of pending acks */
extern int boardshow; /* 0: Show the hidden board */

/* extern int setmainpos; * Are mainxpos and mainypos valid? */
/*  extern gint mainxpos, mainypos; * X and Y position of main window */

extern pos outy, outx; /* Registers for grideval's output - DAMN I hate functions */
extern vect outdir;
extern int evalret;
extern pos updy, updx; /* For moving mirrors, the update places */


extern int deltabufwidth, deltabufheight;
extern int deltabufx, deltabufy;

extern pos arrhistptr; /* This is a queue */
extern pos arrinused[histsize];
extern pos arroutused[histsize];
extern pos arroutx[histsize], arrouty[histsize]; 
extern pos arroutdir[histsize]; 
extern pos arrinx[histsize], arriny[histsize];
extern vect arrindir[histsize];

extern unsigned int density; /* % of squares that should be filled */
extern unsigned int densityfuzz; /* +/- densityfuzz% */
extern unsigned int classweight[NumClasses];

extern histhash *histkeeper;




