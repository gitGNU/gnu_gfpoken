/* multi.c - GFingerPoken Multiplayer Window, Networking
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdarg.h> /* For variable arguments in netsend */
#include <sys/types.h> /* For types needed in net stuff */
#include <sys/socket.h> /* For network communication */
#include <netinet/in.h> /* More net stuff */
#include <arpa/inet.h> /* Even more net stuff */
#include <netdb.h> /* You get the idea - For gethostbyname */
#include <unistd.h> /* For read and write */
#include <stdio.h> /* For the various printfs */
#include <string.h> /* For string manipulation */
#include <stdlib.h> /* For strtol */
#include <fcntl.h> /* For fcntl, to set nonblocking */
#include <errno.h> /* For errno */
extern int errno;

#include "gfp.h"
#include "common.h"

#define Linelen 80
/* NetT = Net token */
#define NetTAck 'A'
#define NetTNak 'N'
#define NetTDone 'D'
#define NetTNewTurn 'N'
#define NetTTest 'T'
#define NetTInitialMove 'I'
#define NetTMove 'M'
#define NetTFlip 'F'
#define NetTReturn 'R'
#define NetTWin 'W'
#define NetTLose 'L'
#define NetTGFP 'G'
#define NetTVersion 'V'
#define NetTCategory 'C'
#define NetTBoardData 'B'
#define NetTBoardSize 'S'

struct sockaddr_in remoteaddr, localaddr;
int remotesock = -1, localsock = -1;
char netin[Linelen] = "";
int blockspot = 0;
char *bigblock = NULL;
char servmode;
gint monitortag = -1;
gint accepttag = -1;

GtkWidget *netwin, *netpack, *netgrid, *netstatbar;
GtkWidget *netlframe, *netrframe;
GtkWidget *netlpack, *netrpack, *netstatpack, *netbutpack;
GtkWidget *netlrad[3], *netrrad[2];
GtkWidget *netlabel[2], *netentry[2];
GtkWidget *netok, *netcancel;
guint netstatmsg;

extern GtkWidget *mainwin, *levelscroll, *toolscroll, *dialogwin;
extern GdkPixmap *arrowpix[4];
extern GdkBitmap *arrowmask[4];
extern GtkWidget *mainbarbut[NumMainIcons];
extern GtkWidget *mainstatbar;

extern GtkWidget *dialogbarbut[NumDialogIcons];

int initnet(int type, char *hostname, char *port);

void setstat(char *msg) {
  gtk_statusbar_pop(GTK_STATUSBAR(netstatbar), netstatmsg);
  gtk_statusbar_push(GTK_STATUSBAR(netstatbar), netstatmsg, msg);
  /* printf("%s\n",msg); */
}

static gint netok_clicked(G_GNUC_UNUSED GtkWidget *widget, G_GNUC_UNUSED gpointer data) {
  int counter=0;
  int connection=0;
  if (netmode) return FALSE;
  gtk_widget_hide(dialogbarbut[ISave-NumMainIcons]);
  for (; counter < 3; counter++)
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(netlrad[counter]))) {
      connection = counter;
      break;
    }
  for (counter = 0; counter < 2; counter++)
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(netrrad[counter]))) {
      category = counter + 1;
      break;
    }
  switch (connection) {
  case 0: /* Auto */
    setstat("Trying client...");
    if (initnet(0, gtk_entry_get_text(GTK_ENTRY(netentry[0])), gtk_entry_get_text(GTK_ENTRY(netentry[1]))))
      netmode = 1;
    else { 
      setstat("Trying server...");
      if (initnet(1, gtk_entry_get_text(GTK_ENTRY(netentry[0])), gtk_entry_get_text(GTK_ENTRY(netentry[1]))))
	netmode = 2;
      else {
	netmode = 0;
	setstat("Could not set up networking!");
      }
    }
    break;
  case 1: /* Client */
    if (!initnet(0, gtk_entry_get_text(GTK_ENTRY(netentry[0])), gtk_entry_get_text(GTK_ENTRY(netentry[1])))) {
      netmode = 0;
    }
    else netmode = 1;
    break;
  case 2: /* Server */
    if (!initnet(1, gtk_entry_get_text(GTK_ENTRY(netentry[0])), gtk_entry_get_text(GTK_ENTRY(netentry[1])))) {
      netmode = 0;
    }
    else netmode = 2;
    break;
  }
  return FALSE;
}

static gint netcancel_clicked(G_GNUC_UNUSED GtkWidget *widget, G_GNUC_UNUSED gpointer data) {
  if (netmode) netkill();
  gtk_widget_hide(netwin);
  gtk_widget_show(mainwin);
  return FALSE;
}

static gint netwin_delete_event(G_GNUC_UNUSED GtkWidget *widget, G_GNUC_UNUSED GdkEvent *event, G_GNUC_UNUSED gpointer data) {
  if (netmode) netkill();
  gtk_widget_hide(netwin);
  gtk_widget_show(mainwin);
  return FALSE;
}

void initnetwin() {
  netwin = gtk_window_new(GTK_WINDOW_DIALOG);
  gtk_window_set_title(GTK_WINDOW(netwin), "GFP Network Game");
  netpack = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(netwin), netpack);
  gtk_widget_show(netpack);
  netgrid = gtk_table_new(6, 3, FALSE);
  gtk_widget_show(netgrid);
  gtk_box_pack_start(GTK_BOX(netpack), netgrid, TRUE, TRUE, 0);
  netstatbar = gtk_statusbar_new();
  gtk_box_pack_start(GTK_BOX(netpack), netstatbar, TRUE, TRUE, 0);
  gtk_widget_show(netstatbar);
  netstatmsg = gtk_statusbar_get_context_id(GTK_STATUSBAR(netstatbar), "Network Status");
  gtk_statusbar_push(GTK_STATUSBAR(netstatbar), netstatmsg, "Ready");

  netlframe = gtk_frame_new("Connection type");
  gtk_table_attach(GTK_TABLE(netgrid), netlframe, 0, 1, 0, 2, GTK_FILL, GTK_FILL|GTK_EXPAND, 5, 5);
  gtk_widget_show(netlframe);
  netlpack = gtk_vbox_new(TRUE, 0);
  gtk_container_add(GTK_CONTAINER(netlframe), netlpack);
  gtk_widget_show(netlpack);
  netlrad[0] = gtk_radio_button_new_with_label(NULL, "Auto");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(netlrad[0]), TRUE);
  gtk_box_pack_start(GTK_BOX(netlpack), netlrad[0], TRUE, TRUE, 0);
  gtk_widget_show(netlrad[0]);
  netlrad[1] = gtk_radio_button_new_with_label(gtk_radio_button_group(GTK_RADIO_BUTTON(netlrad[0])),
					       "Client");
  gtk_box_pack_start(GTK_BOX(netlpack), netlrad[1], TRUE, TRUE, 0);
  gtk_widget_show(netlrad[1]);
  netlrad[2] = gtk_radio_button_new_with_label(gtk_radio_button_group(GTK_RADIO_BUTTON(netlrad[1])),
					       "Server");
  gtk_box_pack_start(GTK_BOX(netlpack), netlrad[2], TRUE, TRUE, 0);
  gtk_widget_show(netlrad[2]);

  netrframe = gtk_frame_new("Game type");
  gtk_table_attach(GTK_TABLE(netgrid), netrframe, 1, 2, 0, 2, GTK_FILL, GTK_FILL|GTK_EXPAND, 5, 5);
  gtk_widget_show(netrframe);
  netrpack = gtk_vbox_new(TRUE, 0);
  gtk_container_add(GTK_CONTAINER(netrframe), netrpack);
  gtk_widget_show(netrpack);
  netrrad[0] = gtk_radio_button_new_with_label(NULL, "Shared board");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(netrrad[0]), TRUE);
  gtk_box_pack_start(GTK_BOX(netrpack), netrrad[0], TRUE, TRUE, 0);
  gtk_widget_show(netrrad[0]);
  netrrad[1] = gtk_radio_button_new_with_label(gtk_radio_button_group(GTK_RADIO_BUTTON(netrrad[0])),
					       "Individual boards");
  gtk_box_pack_start(GTK_BOX(netrpack), netrrad[1], TRUE, TRUE, 0);
  gtk_widget_show(netrrad[1]);

  netstatpack = gtk_vbox_new(FALSE, 0);
  gtk_table_attach_defaults(GTK_TABLE(netgrid), netstatpack, 2, 3, 0, 2);
  gtk_widget_show(netstatpack);
  netlabel[0] = gtk_label_new("Hostname");
  gtk_misc_set_alignment(GTK_MISC(netlabel[0]), 0, 0);
  gtk_box_pack_start(GTK_BOX(netstatpack), netlabel[0], TRUE, TRUE, 0);
  gtk_widget_show(netlabel[0]);
  netentry[0] = gtk_entry_new();
  gtk_box_pack_start(GTK_BOX(netstatpack), netentry[0], TRUE, TRUE, 0);
  gtk_widget_show(netentry[0]);
  netlabel[1] = gtk_label_new("Port");
  gtk_misc_set_alignment(GTK_MISC(netlabel[1]), 0, 0);
  gtk_box_pack_start(GTK_BOX(netstatpack), netlabel[1], TRUE, TRUE, 0);
  gtk_widget_show(netlabel[1]);
  netentry[1] = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(netentry[1]), "1279"); /* 'g'+'f'+'i'+'n'+'g'+'e'+'r'+'p'+'o'+'k'+'e'+'n' */
  gtk_box_pack_start(GTK_BOX(netstatpack), netentry[1], TRUE, TRUE, 0);
  gtk_widget_show(netentry[1]);

  netbutpack = gtk_hbox_new(TRUE, 0);
  gtk_table_attach(GTK_TABLE(netgrid), netbutpack, 0, 3, 2, 3, GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND, 5, 5);
  gtk_widget_show(netbutpack);
  netok = gtk_button_new_with_label("OK");
  gtk_box_pack_start(GTK_BOX(netbutpack), netok, TRUE, TRUE, 5);
  gtk_widget_show(netok);
  netcancel = gtk_button_new_with_label("Cancel");
  gtk_box_pack_start(GTK_BOX(netbutpack), netcancel, TRUE, TRUE, 5);
  gtk_widget_show(netcancel);

  gtk_signal_connect(GTK_OBJECT(netok), "clicked", (GtkSignalFunc)netok_clicked, NULL);
  gtk_signal_connect(GTK_OBJECT(netcancel), "clicked", (GtkSignalFunc)netcancel_clicked, NULL);
  gtk_signal_connect(GTK_OBJECT(netwin), "delete_event", (GtkSignalFunc)netwin_delete_event, NULL);

}

void netkill() {
  if (monitortag != -1) gdk_input_remove(monitortag);
  monitortag = -1;
  switch (netmode) {
  case 2: /* Server */
    if (accepttag != -1) gtk_timeout_remove(accepttag);
    accepttag = -1;
    if (localsock != -1) close(localsock);
    localsock = -1;
  case 1: /* Fall thru */
    if (remotesock != -1) close(remotesock);
    remotesock = -1;
  }
  netmode = 0; checked = 1;
  gtk_widget_hide(mainstatbar);
  gtk_widget_hide(mainbarbut[ICheck]);
  gtk_widget_hide(mainbarbut[INetTurn]);
  gtk_widget_hide(mainbarbut[IStop]);
  gtk_widget_show(mainbarbut[INew]);
  gtk_widget_show(mainbarbut[INet]);
  netin[0] = '\0';
}

void endgame() {
  int tmp=0, counter=0;
  obj tmpobj;
  netkill();
  for (; counter < gridx*gridy; counter++) {
    if (grid[counter] == invisigrid[counter]) continue;
    else tmp = 1;
    tmpobj = grid[counter];
    grid[counter] = invisigrid[counter];
    invisigrid[counter] = tmpobj;
  }
  if (tmp) gtk_widget_show(mainbarbut[IFlipBoards]);
  fullgriddraw();
  boardshow = 0;
}

void netsend(char type, ...) { /* type, then each arg in turn, i.e.: 'M', x1, y1, x2, y2 */
  va_list argptr;
  int argpiece;
  int numargs;
  char transdata[80] = " "; /* Plenty o' room */
  char tempstr[10];
  
  va_start(argptr, type);  
  sprintf(transdata,"%c", type);

  switch (type) {
  case 'A': /* Acknowledge: Universal*/
  case 'N': /* Nack - Terminate connection: Universal */
  case 'D': /* Done with turn: Synch, assynch game */
  case 'W': /* Win: Synch, assynch games */
  case 'L': /* Lose: Synch, assynch games */
    numargs = 0;
    break;
  case 'C': /* Category */
    numargs = 1;
    break;
  case 'F': /* Flip piece at x, y */
  case 'R': /* Revert piece at x, y back to toolbar */
  case 'S': /* Board size is x, y */
  case 'T': /* Test at location x, y */
    numargs = 2;
    break;
  case 'I': /* Initial move a piece of category n to x, y */
    numargs = 3;
    break;
  case 'M': /* Move piece at x1, y1 to x2, y2 */
    numargs = 4;
    break;
  default:
    fprintf(stderr, "Netsend: Received unknown op %c\n",netin[0]);
    exit(1);
  }    
  for(; numargs > 0; numargs--) {
    argpiece = va_arg(argptr, int);
    sprintf(tempstr, " %d",argpiece);
    strcat(transdata, tempstr);
  }
  strcat(transdata, "\n");
  write(remotesock, transdata, strlen(transdata));
  /* printf("Netsend: Sending string %s", transdata); */
  if (type != 'A') {
    pendack++;
    if (category) {
      mainsetstat("Transmitting data, please wait...");
    }
  }
  va_end(argptr);
  
}

gint netmonitor(G_GNUC_UNUSED gpointer data, G_GNUC_UNUSED gint source, G_GNUC_UNUSED GdkInputCondition condition) {
  char newin[Linelen] = "";
  char *inptr = netin+2;
  int tmp, x, y;
  int counter=0;
  switch (read(remotesock, newin, SSIZE_MAX > Linelen-1 ? Linelen-1 : SSIZE_MAX)) {
  case 0:
    return TRUE;
  case -1:
    netkill();
    gamebox("Network Error","Network problems.  Connection terminated.");
    return FALSE;
  }
  strcat(netin, newin);
  if (netin[strlen(netin)-1] != '\n') return TRUE;
  switch (netin[0]) {
  case 'A': /* Ack */
    if (pendack) pendack--;
    if (!pendack && category) {
      if (category == CatShared) {
	if (paralyzed) mainsetstat("Opponent's Turn: Please wait");
	else if (completed) mainsetstat("Your Turn: Finish up");
	else mainsetstat("Your Turn: Make a move");
      } else {
	if (paralyzed) mainsetstat("Waiting for opponent to finish");
	else {
	  if (completed) strcpy(newin, "Finish up - ");
	  else strcpy(newin, "Make a move - ");
	  if (opponentdone) strcat(newin, "Opponent is done");
	  else strcat(newin, "Opponent is not done");
	  mainsetstat(newin);
	}
      }
    }
    netin[0] = '\0';
    return TRUE;
  case 'I': /* Initial Move */
    tmp = strtol(inptr, &inptr, 10);
    x = strtol(inptr, &inptr, 10); 
    y = strtol(inptr, &inptr, 10);
    grid[x+gridx*y] = drtoobj(tmp);
    dragbuf[tmp]--;
    bufsize--;
    bardraw();
    drawtile(x, y);
    break;
  case 'M': /* Move */
    x = strtol(inptr, &inptr, 10);
    y = strtol(inptr, &inptr, 10);
    tmp = grid[x+gridx*y];
    grid[x+gridx*y] = ObNone;
    drawtile(x, y);
    x = strtol(inptr, &inptr, 10);
    y = strtol(inptr, &inptr, 10);
    grid[x+gridx*y] = tmp;
    drawtile(x, y);
    break;
  case 'F': /* Flip */
    x = strtol(inptr, &inptr, 10);
    y = strtol(inptr, &inptr, 10);
    grid[x+gridx*y] = nextobj(grid[x+gridx*y]);
    drawtile(x, y);
    break;
  case 'R': /* Return */
    x = strtol(inptr, &inptr, 10);
    y = strtol(inptr, &inptr, 10);
    dragbuf[objtodr(grid[x+gridx*y])]++;
    bufsize++;
    grid[x+gridx*y] = ObNone;
    drawtile(x, y);
    bardraw();
    break;
  case 'T': /* Test */
    dragx = strtol(inptr, &inptr, 10);
    dragy = strtol(inptr, &inptr, 10);
    /* Copied from leveldraw_button_press_event */
    for (; counter < histsize; counter++)
      if (arrinx[counter] == dragx && arriny[counter] == dragy) arrinused[counter]=0;
    
    arrinx[arrhistptr] = dragx;
    arriny[arrhistptr] = dragy;
    if (dragx == 0) animdir = DRight;
    if (dragy == 0) animdir = DDown;
    if (dragx == gridx + 1) animdir = DLeft;
    if (dragy == gridy + 1) animdir = DUp;
    arrindir[arrhistptr]=animdir;
    dragorigin = PlNowhere;
    gdk_window_set_cursor(mainwin->window, gdk_cursor_new(GDK_WATCH));
    animmode = 1;
    borderdraw();
    for (counter=0; counter < histsize; counter++) histdraw(counter, 1, arrowpix, arrowmask);
    borderrefresh();
    gtk_timeout_add(5, (GtkFunction)anim_loop_timeout, NULL);
    break;
  case 'D': /* New turn */
    switch (category) {
    case CatShared:
      mainsetstat("Your Turn: Make a move");
      break;
    case CatSeparate:
      if (!paralyzed) {
	mainsetstat("Make a move - Opponent is done");
	opponentdone = 1;
	netsend('A');
	netin[0] = '\0';
	return TRUE;
      }
      mainsetstat("Make a move - Opponent is not done");
    }
    paralyzed = 0;
    completed = 0;
    opponentdone = 0;
    break;
  case 'W': /* I lose */
    endgame();
    gamebox("Game Over", "You lose - your opponent guessed correctly.\nHere is the answer:");    
    return FALSE;
  case 'L': /* I win */
    endgame();
    gamebox("Game Over", "You win by default - your opponent messed up.\nHere is the answer:");
    return FALSE;
  case 'N':
    gamebox("Game Over", "Opponent terminated game.");
    netkill();
    return FALSE;
  default:
    fprintf(stderr,"Game: Received unknown op %c\n",netin[0]);
    exit(1);
  }
  netsend('A');
  netin[0] = '\0';
  return TRUE;
}


gint clientmonitor(G_GNUC_UNUSED gpointer data, G_GNUC_UNUSED gint source, G_GNUC_UNUSED GdkInputCondition condition) {
  char newin[Linelen] = "";
  char *st = newin+2;
  int counter=0;
  int tmp;
  switch ((tmp = read(remotesock, newin, SSIZE_MAX > Linelen-1 ? Linelen-1 : SSIZE_MAX))) {
  case 0:
    return TRUE;
  case -1:
    netkill();
    setstat("Network problems.  Connection terminated");
    return FALSE;
  }
  strcat(netin, newin);
  if (netin[strlen(netin)-1] != '\n') return TRUE;
  /* printf("Client: Received string %s", netin); */
  switch (netin[0]) {
  case 'G': /* Start of "GFingerPoken" */
    if (strcmp(netin, "GFingerPoken\n")) {
      setstat("Client: Not a GFingerPoken server.");
      /* How to clean up? */
      netkill();
      return FALSE;
    } else {
      setstat("Client: Found GFingerPoken server, waiting for version reply");
      sprintf(newin, "V %s\n", GFPNetVer);
      write(remotesock, newin, strlen(newin));
    }
    break;
  case 'N': /* Nak - Your version is bad */
    sprintf(newin, "Client: Server running an incompatible version, %s", netin+2);
    newin[strlen(newin)-2] = '\0'; /* Get rid of offending \n */
    setstat(newin);
    gdk_input_remove(monitortag);
    netmode = 0;
    netin[0] = '\0';
    return FALSE;
    break;
  case 'C': /* Category - If different, what to do? */
    category = strtol(st, &st, 10);
    switch (category) {
    case CatShared:
      setstat("Client: Category set to Shared, waiting for board generation...");
      break;
    case CatSeparate:
      setstat("Client: Category set to Separate, waiting for board generation...");
    }
    netsend('A');
    break;
  case 'S': /* Board size */
    setstat("Client: Creating board...");
    gridx = strtol(st, &st, 10);
    gridy = strtol(st, &st, 10);
    if (grid) free(grid); grid = (obj *)calloc(gridx*gridy, sizeof(obj));
    if (invisigrid) free(invisigrid); invisigrid = (obj *)malloc(gridx*gridy*sizeof(obj)); /* will be completely filled */
    if (markgrid) free(markgrid); markgrid = (obj *)calloc(gridx*gridy, sizeof(obj));
    if (!(grid && invisigrid && markgrid)) {
      fprintf(stderr, "Insufficient memory for allocating level; needed %zd bytes\n",
	      3*gridx*gridy*sizeof(obj));
      exit(1);
    }
    
    for (; counter < DrNum; counter++) dragbuf[counter] = 0;
    arrhistptr = 0;
    for (counter = 0; counter < histsize; counter++) {
      arrinused[counter] = 0;
      arroutused[counter] = 0;
    }
    bufsize = 0; 
    checked = 0;

    setstat("Client: Downloading board, 0% complete");
    blockspot = 0;
    netsend('A');
    break;
  case 'B': /* Board data */
    /* printf("Client: Decoding board data...\n"); */
    for(; *st && *st != '\n'; blockspot++) {
      if ((invisigrid[blockspot] = strtol(st, &st, 10))) {
	dragbuf[objtodr(invisigrid[blockspot])]++;
	bufsize++;
      }
    }
    netsend('A');
    if (blockspot == gridx*gridy) {
      setstat("Client: Completed download, starting game...");
      netsend('A');
      gdk_input_remove(monitortag);
      monitortag = gdk_input_add(remotesock, GDK_INPUT_READ, (GdkInputFunction)netmonitor, NULL);
      shownetturn();
      gtk_widget_show(mainwin);
      fullgriddraw();
      bardraw();

      toolscroll->allocation.width = 0;
      toolscroll->allocation.height = 0;
      levelscroll->allocation.width = 0;
      levelscroll->allocation.height = 0;

      gtk_widget_set_usize(levelscroll, pixwidth*(gridx+2) +
			   GTK_SCROLLED_WINDOW(levelscroll)->vscrollbar->allocation.width+7,
			   pixheight*(gridy+2) +
			   GTK_SCROLLED_WINDOW(levelscroll)->hscrollbar->allocation.height+7);
      gtk_widget_set_usize(toolscroll, pixwidth +
			   GTK_SCROLLED_WINDOW(toolscroll)->vscrollbar->allocation.width+7,
			   levelscroll->allocation.height);

      /* if (setmainpos) gtk_widget_set_uposition(mainwin, mainxpos, mainypos); */

      gtk_widget_hide(netwin);
      netin[0] = '\0';
      pendack = 0;
      switch (category) {
      case CatShared: /* Shared */
	paralyzed = 1;
	mainsetstat("Opponent's Turn: Please wait");
	break;
      case CatSeparate: /* Separate */
	paralyzed = 0;
	mainsetstat("Make a move - Opponent is not done");
      }
      completed = 0;
      return FALSE;
    } else {
      sprintf(newin, "Client: Downloading board, %d%% complete", (100*blockspot)/(gridx*gridy));
      setstat(newin);
      break;
    }
  default:
    fprintf(stderr,"Client: Received unknown op %c\n",netin[0]);
    exit(1);
  }
  netin[0] = '\0';
  return TRUE;
}

gint servermonitor(G_GNUC_UNUSED gpointer data, G_GNUC_UNUSED gint source, G_GNUC_UNUSED GdkInputCondition condition) {
  char newin[Linelen] = "";
  int counter = 0;
  switch (read(remotesock, newin, SSIZE_MAX > Linelen-1 ? Linelen-1 : SSIZE_MAX)) {
  case 0:
    return TRUE;
  case -1:
    netkill();
    setstat("Network problems.  Connection terminated.");
    return FALSE;
  }
  strcat(netin, newin);
  if (netin[strlen(netin)-1] != '\n') return TRUE;
  switch (netin[0]) {
  case 'V':
    netin[strlen(netin)-1] = '\0';
    if (strcmp(netin+2, GFPNetVer)) {
      sprintf(newin, "Server: Incompatible client version %s",netin+2);
      setstat(newin);
      sprintf(newin, "N %s\n", GFPNetVer);
      write(remotesock, newin, strlen(newin));
      gdk_input_remove(monitortag);
      netmode = 0;
      netin[0] = '\0';
      return FALSE;
    }
    servmode = 'S';
    netsend('C', category); /* They'll take it and they'll like it! */
    break;
  case 'A':
    switch(servmode) {
    case 'S': /* Sizing board up as we speak probably */
      setstat("Server: Please configure the level as you'd like it.");
      gtk_widget_show(dialogwin);
      servmode = 'B';
      blockspot = 0;
      break;
    case 'D': /* Metamode - Done */
      setstat("Server: Completed upload, starting game...");
      gdk_input_remove(monitortag);
      monitortag = gdk_input_add(remotesock, GDK_INPUT_READ, (GdkInputFunction)netmonitor, NULL);
      shownetturn();
      gtk_widget_show(mainwin);
      fullgriddraw();
      bardraw();

      toolscroll->allocation.width = 0;
      toolscroll->allocation.height = 0;
      levelscroll->allocation.width = 0;
      levelscroll->allocation.height = 0;      
      gtk_widget_set_usize(levelscroll, pixwidth*(gridx+2) +
			   GTK_SCROLLED_WINDOW(levelscroll)->vscrollbar->allocation.width+7,
			   pixheight*(gridy+2) +
			   GTK_SCROLLED_WINDOW(levelscroll)->hscrollbar->allocation.height+7);
      gtk_widget_set_usize(toolscroll, pixwidth +
			   GTK_SCROLLED_WINDOW(toolscroll)->vscrollbar->allocation.width+7,
			   levelscroll->allocation.height);

      /* if (setmainpos) gtk_widget_set_uposition(mainwin, mainxpos, mainypos); */

      gtk_widget_hide(netwin);
      netin[0] = '\0';
      pendack = 0;
      switch (category) {
      case CatShared: /* Shared */
	paralyzed = 0;
	mainsetstat("Your turn: Make a move");
	break;
      case CatSeparate: /* Separate */
	paralyzed = 0;
	mainsetstat("Make a move - Opponent is not done");
      }
      completed = 0;
      return FALSE;
    case 'B': /* Board send */
      sprintf(newin, "B");
      for(; counter < 25 && blockspot < gridx*gridy; counter++, blockspot++) {
	sprintf(netin, " %d", invisigrid[blockspot]);
	strcat(newin, netin);
      }
      if (blockspot == gridx*gridy) servmode = 'D';
      strcat(newin,"\n");
      /* printf("Server: Writing board data %s",newin); */
      write(remotesock, newin, strlen(newin));
      sprintf(newin, "Server: Uploading board, %d%% complete", (100*blockspot)/(gridx*gridy));
      setstat(newin);
      break;
    default:
      fprintf(stderr, "Server: Unknown mode %c\n", servmode);
      exit(1);
    }
    break;
  default:
    fprintf(stderr,"Server: Received unknown op %c\n", netin[0]);
    exit(1);
  }
  netin[0] = '\0';
  return TRUE;
}

gint accept_loop_timeout(G_GNUC_UNUSED gpointer data) {
  socklen_t clientlen = sizeof (struct sockaddr_in);
  if ((remotesock = accept(localsock, (struct sockaddr *)&remoteaddr, &clientlen)) == -1) {
    switch (errno) {
    case EWOULDBLOCK:
      return TRUE;
    default:
      netmode = 0;
      close(localsock);
      setstat("Server: Could not listen for connection");
      accepttag = -1;
      return FALSE;
    }
  }
  setstat("Server: Connection received...");
  write(remotesock, "GFingerPoken\n", 13);
  monitortag = gdk_input_add(remotesock, GDK_INPUT_READ, (GdkInputFunction)servermonitor, NULL);
  accepttag = -1;
  return FALSE;
}

int initnet(int type, char *hostname, char *port) { /* Return: 1 if successful, 0 if unsuccessful */
  struct hostent *hoststuff = NULL;
  int flags;
  switch (type) {
  case 0: /* Client */
    if (hostname[0] == '\0') {
      setstat("Client: No hostname entered!");
      return 0;
    }
    setstat("Client: Looking up hostname...");
    if (!(hoststuff = gethostbyname(hostname))) {
      setstat("Client: Hostname not found.");
      return 0;
    }
    setstat("Client: Hostname found, setting up connection...");
    if ((remotesock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      setstat("Client: Could not create socket.");
      return 0;
    }
    remoteaddr.sin_family = AF_INET;
    remoteaddr.sin_addr = *(struct in_addr *)*hoststuff->h_addr_list;
    remoteaddr.sin_port = htons(atoi(port));
    if (connect(remotesock, (struct sockaddr *)&remoteaddr, sizeof(remoteaddr)) == -1) { 
      setstat("Client: Could not connect to server."); 
      return 0;
    }
    setstat("Client: Connected to server, waiting for reply...");
    monitortag = gdk_input_add(remotesock, GDK_INPUT_READ, (GdkInputFunction)clientmonitor, NULL);
    return 1;
  case 1: /* Server */
    if (hostname[0] != '\0') {
      setstat("Server: Looking up hostname...");
      if (!(hoststuff = gethostbyname(hostname))) {
	setstat("Server: Hostname not found.");
	return 0;
      }
    }
    setstat("Server: Creating and binding socket...");
    localsock = socket(AF_INET, SOCK_STREAM, 0);
    flags = fcntl(localsock, F_GETFL, 0);
    fcntl(localsock, F_SETFL, O_NONBLOCK+flags);
    localaddr.sin_family = AF_INET;
    if (hoststuff) localaddr.sin_addr = *(struct in_addr *)*hoststuff->h_addr_list;
    else localaddr.sin_addr.s_addr = INADDR_ANY;
    localaddr.sin_port = htons(atoi(port));
    if (bind(localsock, (struct sockaddr *)&localaddr, sizeof(localaddr)) == -1) {
      setstat("Server: Could not bind socket.  Try a different port.");
      return 0;
    }
    setstat("Server: Waiting for connection...");
    if (listen(localsock, 5) == -1) {
      setstat("Server: Could not listen.");
      return 0;
    }    
    accepttag = gtk_timeout_add(1000, (GtkFunction)accept_loop_timeout, NULL);
    return 1;
  default:
    return 0;
  }
}


