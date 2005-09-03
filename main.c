/********************************************
 GFingerPoken Main Window, main()
********************************************/

#include <stdlib.h>  /* For rand */
#include <string.h>  /* For memcpy */
#include <stdio.h>   /* Only for printf */
#include <time.h>    /* For time (seed rand) */

#include "gfp.h"     /* Constants and "global" function declarations */

#include "initial.h" /* Initialize global vars */

#include "tilepix.h" /* Lots and lots of graphics */
#include "borderpix.h"
#include "arrowpix.h"
#include "marblepix.h"
#include "markpix.h"
#include "barpix.h"
#include "iconpix.h"


extern GtkWidget *dialogwin, *netwin;

int dragmode = 0;
int leavecount = 0;


guint mainstatmsg;


/* Helper functions */
void mainsetstat(char *msg) {
  gtk_statusbar_pop(GTK_STATUSBAR(mainstatbar), mainstatmsg);
  gtk_statusbar_push(GTK_STATUSBAR(mainstatbar), mainstatmsg, msg);
}

GdkCursor *makecursor(GdkCursorType cursor_type) {
  static GdkCursor *mycursor = NULL;
  if (mycursor) gdk_cursor_destroy(mycursor);
  mycursor = gdk_cursor_new(cursor_type);
  return mycursor;
}

/* GTK event handlers */

gint anim_loop_timeout(G_GNUC_UNUSED gpointer data) { /* Writes directly to screen.  It's updated frequently anyway. */
  int counter=0;               /* Function used to be static */
  switch (animmode) {
  case 0:
    return TRUE;
  case 1: /* Starting animation */
    /* Initialization */
    evalret = RetNormal; /* To prevent false signal */
    animmode = 2;
    animframe = 0; /* Only the very first frame is 0. */
    animx = dragx*pixwidth; animy = dragy*pixheight;
    animsrcdir = animdir;
    oldanimx = 0; oldanimy = 0;
    outx = 0; outy = 0;
    if (histkeeper) histkeeper = inithist(grid);
    /* Draw that first frame */
    gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], marblemask);
    gdk_gc_set_clip_origin(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], animx, animy);
    gdk_draw_pixmap(leveldraw->window, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
		    marblepix, (arrhistptr*2*skipsize + (animframe/frameskip)%skipsize)*pixwidth, 
		    0, animx, animy, pixwidth, pixheight);
    gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], NULL);
	

  case 2:
    if (animframe && evalret >= 0) { /* Double-buffered refresh: Flicker-Free */
      switch (animdir) {
      case DUp:
	animy -= frameskip;
	gdk_draw_pixmap(vdeltabuf, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			levelpixmap, animx, animy, 0, 0, pixwidth, pixheight+frameskip);
	gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], marblemask);
	gdk_gc_set_clip_origin(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], 0, 0);
	gdk_draw_pixmap(vdeltabuf, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			marblepix, (arrhistptr*2*skipsize + (animframe/frameskip)%skipsize)*pixwidth,
			0, 0, 0, pixwidth, pixheight);
	gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], NULL);
	gdk_draw_pixmap(leveldraw->window, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			vdeltabuf, 0, 0, animx, animy, pixwidth, pixheight+frameskip);
	break;
      case DDown:
	animy += frameskip;
	gdk_draw_pixmap(vdeltabuf, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			levelpixmap, animx, animy-frameskip, 0, 0, pixwidth, pixheight+frameskip);
	gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], marblemask);
	gdk_gc_set_clip_origin(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], 0, frameskip);
	gdk_draw_pixmap(vdeltabuf, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			marblepix, (arrhistptr*2*skipsize + skipsize-animframe/frameskip)*pixwidth, 
			0, 0, frameskip, pixwidth, pixheight);
	gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], NULL);
	gdk_draw_pixmap(leveldraw->window, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			vdeltabuf, 0, 0, animx, animy-frameskip, pixwidth, pixheight+frameskip);
	break;
      case DLeft:
	animx -= frameskip;
	gdk_draw_pixmap(hdeltabuf, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			levelpixmap, animx, animy, 0, 0, pixwidth+frameskip, pixheight);
	gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], marblemask);
	gdk_gc_set_clip_origin(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], 0, 0);
	gdk_draw_pixmap(hdeltabuf, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			marblepix,((arrhistptr+1)*2*skipsize - 1 - (animframe/frameskip)%skipsize)*pixwidth, 
			0, 0, 0, pixwidth, pixheight);
	gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], NULL);
	gdk_draw_pixmap(leveldraw->window, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			hdeltabuf, 0, 0, animx, animy, pixwidth+frameskip, pixheight);
	break;
      case DRight:
	animx += frameskip;
	gdk_draw_pixmap(hdeltabuf, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			levelpixmap, animx-frameskip, animy, 0, 0, pixwidth+frameskip, pixheight);
	gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], marblemask);
	gdk_gc_set_clip_origin(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], frameskip, 0);
	gdk_draw_pixmap(hdeltabuf, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			marblepix,(arrhistptr*2*skipsize+skipsize+(animframe/frameskip)%skipsize)*pixwidth, 
			0, frameskip, 0, pixwidth, pixheight);
	gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], NULL);
	gdk_draw_pixmap(leveldraw->window, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			hdeltabuf, 0, 0, animx-frameskip, animy, pixwidth+frameskip, pixheight);
      }
    }
    animframe += frameskip;
  }
  if (animframe > pixsize || evalret < 0) {
    if (oldanimx) { /* Should be if evalret == RetChanged */

      drawtile(oldanimx/pixwidth-1,oldanimy/pixheight-1);
      /*
      gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], 
		      tilepic[grid[oldanimx/pixwidth-1+(oldanimy/pixheight-1)*gridx]], 0, 0, 
		      oldanimx, oldanimy, pixwidth, pixheight);
      gdk_draw_pixmap(leveldraw->window, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
		      levelpixmap, oldanimx, oldanimy, oldanimx, oldanimy, pixwidth, pixheight);
      */
    }
    else { animsrcx = animx/pixwidth-1; animsrcy = animy/pixheight-1;}
    if (evalret == RetMovement) {
      gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], 
		      tilepic[grid[updx+updy*gridx]], 0, 0, 
		      (updx+1)*pixwidth, (updy+1)*pixheight, pixwidth, pixheight);
      gdk_draw_pixmap(leveldraw->window, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
		      levelpixmap, (updx+1)*pixwidth, (updy+1)*pixheight, (updx+1)*pixwidth, 
		      (updy+1)*pixheight, pixwidth, pixheight);
    }
    if ((outx - 1) > (gridx-2) || (outy - 1) > (gridy-2) || evalret < 0) { /* All balls must roll to an end. */
      if (histkeeper) freehist(histkeeper);
      if (evalret >= 0) {
	borderdraw();
	gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], marblemask);
	gdk_gc_set_clip_origin(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], animx, animy);
	gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			marblepix, arrhistptr*2*skipsize*pixwidth, 
			0, animx, animy, pixwidth, pixheight);
	/*
	  gdk_draw_pixmap(leveldraw->window, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
	  marblepix, arrhistptr*2*skipsize*pixwidth, 
	  0, animx, animy, pixwidth, pixheight);
	*/
	gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], NULL);
      }
      gdk_window_set_cursor(mainwin->window, makecursor(GDK_LEFT_PTR));
      if (!checked) {
	outx = animsrcx; outy = animsrcy; outdir = animsrcdir;
	/*
	  printf("Original grid x: %d y: %d\n",outx,outy);
	  printf("Original screen x: %d y: %d\n",arrinx[arrhistptr], arriny[arrhistptr]);
	*/
	if (histkeeper) histkeeper = inithist(invisigrid);
	do {
	  /* printf("x: %d y: %d\n",outx, outy); */
	  grideval(outx, outy, outdir, invisigrid);
	} while (((outx + 1) < (gridx+1)) && ((outy + 1) < (gridy+1)) && (evalret >= 0));
	if (histkeeper) freehist(histkeeper);
	/* if (outx == 0 || outx == gridx+1) outy--; else outx--; */
	/* printf("Final x: %d y: %d\n",outx,outy); */
	outx++; outy++;
	arrinused[arrhistptr]=1; /* Arrin already set */
	for (; counter < histsize; counter++)
	  if (arroutx[counter] == outx && arrouty[counter] == outy) arroutused[counter]=0;
	if (evalret >= 0) arroutused[arrhistptr]=1;
	arroutx[arrhistptr]=outx; arrouty[arrhistptr]=outy; arroutdir[arrhistptr]=outdir;
	for (counter = 0; counter < histsize; counter++) {
	  if (counter == arrhistptr) histdraw(counter, 0, warrowpix, arrowmask);
	  else histdraw(counter, 1, arrowpix, arrowmask);
	}
      }
      borderrefresh();
      arrhistptr = (arrhistptr + 1) % histsize;
      animmode = 0;
      return FALSE;
    }
    oldanimx = animx; oldanimy = animy;
    grideval(animx/pixwidth-1, animy/pixheight-1, animdir, grid);
    animdir = outdir; 
    animframe = frameskip;
  }
  return TRUE;
}

void gamebox_destroy_event(GtkWidget *widget, G_GNUC_UNUSED gpointer data) {
  gtk_grab_remove(widget);
}

void gamebox_button_clicked(G_GNUC_UNUSED GtkWidget *widget, gpointer data) {
  gtk_widget_destroy(data);
}

void gamebox(char *title, char *msg) {
  GtkWidget *label, *button, *window;
  window = gtk_dialog_new();
  gtk_window_set_modal(GTK_WINDOW(window), TRUE);
  gtk_signal_connect(GTK_OBJECT(window), "destroy", (GtkSignalFunc)gamebox_destroy_event, window);
  gtk_window_set_title(GTK_WINDOW(window), title);
  gtk_container_set_border_width(GTK_CONTAINER(window), 5);
  label = gtk_label_new(msg);
  gtk_misc_set_padding(GTK_MISC(label), 10, 10);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(window)->vbox), label, TRUE, TRUE, 0);
  gtk_widget_show(label);
  button = gtk_button_new_with_label("OK");
  gtk_signal_connect(GTK_OBJECT(button), "clicked", (GtkSignalFunc)gamebox_button_clicked, window);
  GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(window)->action_area), button, TRUE, TRUE, 0);
  gtk_widget_grab_default(button);
  gtk_widget_show(button);
  gtk_widget_show(window);
  gtk_grab_add(window);
}

static gint newbarbut_event(G_GNUC_UNUSED GtkWidget *widget, G_GNUC_UNUSED gpointer data) {
  if (animmode) return FALSE;
  /*
    setmainpos = 1;
    gdk_window_get_position(mainwin->window, &mainxpos, &mainypos);
    printf("x: %d  y: %d\n",mainxpos, mainypos);
  */
  netmode = 0;
  gtk_widget_hide(mainwin);
  gtk_widget_show(dialogwin);
  return FALSE;
}

static gint stopbarbut_event(G_GNUC_UNUSED GtkWidget *widget, G_GNUC_UNUSED gpointer data) {
  if (animmode) return FALSE;
  netsend('N');
  netkill();
  return FALSE;
}

static gint checkbarbut_event(G_GNUC_UNUSED GtkWidget *widget, G_GNUC_UNUSED gpointer data) {
  int xcounter=0, ycounter=0;
  pos saveoutx, saveouty;
  vect savedir;
  int saveret;
  obj tmp;
  obj *tmpinvisigrid = NULL, *tmpgrid = NULL;
  if (animmode || (netmode && (pendack || paralyzed))) return FALSE;
  /* No longer necessary - Button only appears when valid
  if (checked) {
    gamebox("Oops", "You already guessed!");
    return FALSE;
  }
  if (bufsize) {
    gamebox("Oops", "You must use all of your widgets.");
    return FALSE;
  }
  */
  checked = 1; boardshow = 0;
  gtk_widget_hide(mainbarbut[ICheck]);
  while (invisigrid[ycounter*gridx+xcounter] == grid[ycounter*gridx+xcounter] && ycounter < gridy) {
    xcounter++;
    if (xcounter == gridx) {
      xcounter = 0;
      ycounter++;
    }
  }

  if (ycounter == gridy) { /* Made it through; must be identical */
    gamebox("Game Over", "That's right!  Yours is identical.");
    if (netmode) {
      netsend('W');
      netkill();
    }
    fullgriddraw();
    return FALSE;
  }
  gtk_widget_show(mainbarbut[IFlipBoards]);
  
  tmpgrid = (obj *)malloc(gridx*gridy*sizeof(obj));
  tmpinvisigrid = (obj *)malloc(gridx*gridy*sizeof(obj));
  if (!(tmpgrid && tmpinvisigrid)) {
    fprintf(stderr, "Insufficient memory for allocating temporary data; needed %d bytes\n",
	    2*gridx*gridy*sizeof(obj));
    if (netmode) {
      netsend('N');
      netkill();
    }
    exit(1);
  }    

  memcpy(tmpgrid, grid, gridx*gridy*sizeof(obj));
  memcpy(tmpinvisigrid, invisigrid, gridx*gridy*sizeof(obj));

  for (ycounter = 0; ycounter < gridy; ycounter++) 
    for (xcounter = 0; xcounter < gridx; xcounter++) {
      if (ycounter == 0) outdir = DDown;
      if (ycounter == gridy-1) outdir = DUp;
      if (xcounter == 0) outdir = DRight;
      if (xcounter == gridx-1) outdir = DLeft;
      savedir = outdir;
      outx = xcounter; outy = ycounter;
      /* printf("In: %d %d %d\n", outx, outy, outdir); */
      if (histkeeper) histkeeper = inithist(tmpinvisigrid);  
      do {
	grideval(outx, outy, outdir, tmpinvisigrid);
      } while ((outx + 1) < (gridx+1) && (outy + 1) < (gridy+1) && evalret >= 0);
      /* printf("Invis out: %d %d %d\n",outx,outy,outdir); */
      if (histkeeper) freehist(histkeeper);
      saveoutx = outx; saveouty = outy; saveret = evalret;
      outx = xcounter; outy = ycounter; outdir = savedir;
      if (histkeeper) histkeeper = inithist(tmpgrid);  
      do {
	grideval(outx, outy, outdir, tmpgrid);
      } while ((outx + 1) < (gridx+1) && (outy + 1) < (gridy+1) && evalret >= 0);
      /* printf("Your out: %d %d %d\n",outx,outy,outdir); */
      if (histkeeper) freehist(histkeeper);
      if ((saveoutx != outx || saveouty != outy) && (saveret != evalret || evalret >= 0)) {
	gamebox("Game Over", "Ours are functionally different.  Here is mine.");
	/* printf("Eval %d %d Outx %d %d Outy %d %d\n",saveret,evalret,saveoutx,outx,saveouty,outy); */
	if (netmode) {
	  netsend('L');
	  netkill();
	}
	for (xcounter = 0; xcounter < gridx*gridy; xcounter++) {
	  tmp = grid[xcounter];
	  grid[xcounter] = invisigrid[xcounter];
	  invisigrid[xcounter] = tmp;
	}
	free(tmpgrid);
	free(tmpinvisigrid);
	fullgriddraw();  
	return FALSE; 
      }
      if (xcounter == 0 && ycounter != 0 && ycounter != gridy-1) xcounter += gridx-2; /* Kludge City again */
    }
  gamebox("Game Over", "Our grids seem functionally identical.  Here is mine.");
  if (netmode) {
    netsend('W');
    netkill();
  }
  for (xcounter=0; xcounter < gridx*gridy; xcounter++) {
    tmp = grid[xcounter];
    grid[xcounter] = invisigrid[xcounter];
    invisigrid[xcounter] = tmp;
  }


  free(tmpgrid);
  free(tmpinvisigrid);
  
  fullgriddraw();
  return FALSE;
}
/*
static gint mainwin_configure_event(GtkWidget *widget, GdkEvent *event) {
  if (levelscroll->allocation.width >= pixwidth*(gridx+2)) {
    printf("Big-Ass\n");
    GTK_SCROLLED_WINDOW(levelscroll)->hscrollbar_visible = FALSE;
  }
  else GTK_SCROLLED_WINDOW(levelscroll)->hscrollbar_visible = TRUE;
  if (levelscroll->allocation.height >= pixheight*(gridy+2)) 
    GTK_SCROLLED_WINDOW(levelscroll)->vscrollbar_visible = FALSE;
  else GTK_SCROLLED_WINDOW(levelscroll)->hscrollbar_visible = TRUE;
  return FALSE;
}
*/

static gint netbarbut_event(G_GNUC_UNUSED GtkWidget *widget, G_GNUC_UNUSED gpointer data) {
  setstat("Ready");
  /*
    setmainpos = 1;
    gdk_window_get_position(mainwin->window, &mainxpos, &mainypos);
  */
  gtk_widget_hide(mainwin);
  gtk_widget_show(netwin);
  return FALSE;
}

static gint netturnbarbut_event(G_GNUC_UNUSED GtkWidget *widget, G_GNUC_UNUSED gpointer data) {
  if (paralyzed || pendack || !netmode) return FALSE;
  gtk_widget_hide(mainbarbut[INetTurn]);
  if (category == CatSeparate && opponentdone) {
    paralyzed = 0;
    completed = 0;
    opponentdone = 0;
    netsend('D');
    return FALSE;
  }
  netsend('D');
  paralyzed = 1;
  return FALSE;
}

static gint flipboardsbarbut_event(G_GNUC_UNUSED GtkWidget *widget, G_GNUC_UNUSED gpointer data) {
  int counter=0;
  obj tmp;
  boardshow = !boardshow;
  
  for (; counter < gridx*gridy; counter++) {
    tmp = grid[counter];
    grid[counter] = invisigrid[counter];
    invisigrid[counter] = tmp;
  }

  fullgriddraw();  

  if (boardshow) {
    gtk_widget_hide(mainbarbut[IFlipBoards]);
    gtk_widget_show(mainbarbut[IFlipBack]);
  }
  else {
    gtk_widget_hide(mainbarbut[IFlipBack]);
    gtk_widget_show(mainbarbut[IFlipBoards]);
  }
  return FALSE;
}  

static gint aboutbarbut_event(G_GNUC_UNUSED GtkWidget *widget, G_GNUC_UNUSED gpointer data) {
  gamebox("About GFingerPoken","GFingerPoken 0.25 January 14, 2000\nCode by Martin Hock (oxymoron@cmu.edu)\nGame-specific graphics by Jason Reed (godel@cmu.edu)\nToolbar graphics by Tuomas Kuosmanen (tigert@gimp.org) from gnome-stock\nLicensed under the GNU General Public License version 2 or later\nDistributed with NO WARRANTY, see COPYING for details");
  return FALSE;
}

static gint mainwin_delete_event(G_GNUC_UNUSED GtkWidget *widget, G_GNUC_UNUSED GdkEvent *event, G_GNUC_UNUSED gpointer data) {
  if (netmode) netkill();
  if (grid) free(grid);
  if (invisigrid) free(invisigrid);
  if (markgrid) free(markgrid);
  gtk_main_quit();
  return FALSE;
}

static gint leveldraw_expose_event(GtkWidget *widget, GdkEventExpose *event) {
  gdk_draw_pixmap(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
                  levelpixmap, event->area.x, event->area.y,
                  event->area.x, event->area.y,
                  event->area.width, event->area.height);

  return FALSE;
}

static gint tooldraw_expose_event(GtkWidget *widget, GdkEventExpose *event) {
  if (bufsize)
    gdk_draw_pixmap(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
		    toolpixmap, event->area.x, event->area.y,
		    event->area.x, event->area.y,
		    event->area.width, event->area.height);
  return FALSE;
}

static gint leveldraw_motion_notify_event(G_GNUC_UNUSED GtkWidget *widget, GdkEventButton *event) {
  int x, y;
  if (dragorigin != PlNowhere) {
    if (event->state & GDK_BUTTON1_MASK) {
      x = event->x/pixwidth;
      y = event->y/pixwidth;
      /* if ((event->x < 0) || (event->x >= widget->allocation.width) || 
	  (event->y < 0) || (event->y >= widget->allocation.height)) return FALSE; */
      if ((x - 1 == dragx) && (y - 1 == dragy) && dragorigin == PlLevel && dragmode == 0) 
	gdk_window_set_cursor(mainwin->window, makecursor(GDK_LEFT_PTR));
      else {
	dragmode = 1;
	if ((x > 0) && (x < gridx+1) && (y > 0) && (y < gridy+1))
	  if (grid[x-1+gridx*(y-1)] != ObNone) 
	    gdk_window_set_cursor(mainwin->window, makecursor(GDK_X_CURSOR));
	  else
	    gdk_window_set_cursor(mainwin->window, makecursor(GDK_ICON));
	else if ((((x == 0) || (x == gridx+1)) && (y >= 0) && (y <= gridy+1)) || 
		 (((y == 0) || (y == gridy+1)) && (x >= 0) && (x <= gridx+1))) 
	gdk_window_set_cursor(mainwin->window, makecursor(GDK_X_CURSOR)); 
      }
    }
    else {
      gdk_window_set_cursor(mainwin->window, makecursor(GDK_LEFT_PTR));
      dragorigin = PlNowhere;
    }
  }
  return FALSE;  
}

static gint draw_leave_notify_event(G_GNUC_UNUSED GtkWidget *widget, G_GNUC_UNUSED GdkEventCrossing *event) {
  /* Because GTK retardedly calls ANOTHER leave when the mouse button is released,
     I must count the number of leaves and entrances.
  */
  leavecount++;
  if (dragorigin != PlNowhere) {
    if (leavecount == 1)
      gdk_window_set_cursor(mainwin->window, makecursor(GDK_X_CURSOR));
    else {
      gdk_window_set_cursor(mainwin->window, makecursor(GDK_LEFT_PTR));
      dragorigin = PlNowhere;
    }
  }
  return FALSE;
}

static gint simple_enter_notify_event(G_GNUC_UNUSED GtkWidget *widget, G_GNUC_UNUSED GdkEventCrossing *event) {
  leavecount = 0;
  return FALSE;
}

static gint draw_enter_notify_event(G_GNUC_UNUSED GtkWidget *widget, G_GNUC_UNUSED GdkEventCrossing *event) {
  leavecount = 0;
  if (dragorigin != PlNowhere) 
    gdk_window_set_cursor(mainwin->window, makecursor(GDK_ICON));
  return FALSE;
}


/*
static gint tooldraw_motion_notify_event(GtkWidget *widget, GdkEventButton *event) {
  int x;
  if ((event->state & GDK_BUTTON1_MASK) && (dragorigin != PlNowhere)) {
    x = event->x/pixwidth;
    if ((x != 0) || (event->y < 0) || (event->y >= widget->allocation.height)) return FALSE;
    gdk_window_set_cursor(mainwin->window, makecursor(GDK_ICON));
  }
  return FALSE;
}
*/  
static gint mainwin_button_release_event(G_GNUC_UNUSED GtkWidget *widget, G_GNUC_UNUSED GdkEventButton *event) {
  if (animmode) return FALSE;
  gdk_window_set_cursor(mainwin->window, makecursor(GDK_LEFT_PTR));
  return FALSE;
}


static gint leveldraw_button_press_event(G_GNUC_UNUSED GtkWidget *widget, GdkEventButton *event) {
  unsigned int counter=0;
  static guint32 time=0;
  if (animmode) return FALSE;
  dragmode = 0;
  dragx = event->x/pixwidth; dragy = event->y/pixheight; 

  if (((dragx == 0) + (dragy == 0) + (dragx == gridx + 1) + (dragy == gridy + 1)) == 1) { /* i.e. one is true */
    if (netmode) {
      if (paralyzed || completed || pendack) return FALSE;
      else {
	if (category == CatShared) netsend('T', dragx, dragy);
	else {
	  if (opponentdone) mainsetstat("Finish up - Opponent is done");
	  else mainsetstat("Finish up - Opponent is not done");
	}
	gtk_widget_show(mainbarbut[INetTurn]);
	completed = 1;
      }
    }
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
    gdk_window_set_cursor(mainwin->window, makecursor(GDK_WATCH));
    animmode = 1;
    /* Clear perimeter */
    borderdraw();
    /* Draw ball halves and arrows */
    for (counter=0; counter < histsize; counter++) histdraw(counter, 1, arrowpix, arrowmask);
    /* Refresh perimeter */
    borderrefresh();
    /* And now for some action */
    gtk_timeout_add(5, (GtkFunction)anim_loop_timeout, NULL);
    return FALSE;
  }
  if (checked) return FALSE;
  if ((((dragx <= 0) || (dragx >= gridx+1)) && ((dragy <= 0) || (dragy >= gridy+1))) || 
      ((grid[(--dragx) + (--dragy)*gridx] == ObNone) && event->button != 3)) { /* if invalid */
    dragorigin = PlNowhere;
    gdk_window_set_cursor(mainwin->window, makecursor(GDK_LEFT_PTR));
    return FALSE;
  }

  if (event->button == 3 && time != event->time) {
    if (markgrid[dragx+dragy*gridx] == NumMarks) markgrid[dragx+dragy*gridx] = MkNone;
    else markgrid[dragx+dragy*gridx]++;
    drawtile(dragx, dragy);
    dragorigin = PlNowhere;
    time = event->time;
    return FALSE;
  }

  dragitem = grid[dragx+dragy*gridx];
  dragorigin = PlLevel;
  /*  gdk_window_set_cursor(mainwin->window, makecursor(GDK_ICON)); */
  return FALSE;
}

static gint tooldraw_button_press_event(G_GNUC_UNUSED GtkWidget *widget, GdkEventButton *event) {
  if (animmode || checked) return FALSE;
  dragmode = 1;
  dragx = 0; dragy = event->y/pixheight; 
  if (dragy >= bufsize) {
    dragorigin = PlNowhere;
    gdk_window_set_cursor(mainwin->window, makecursor(GDK_LEFT_PTR));
    return FALSE;
  }
  dragitem = bufselect(dragy);
  dragorigin = PlToolbar;
  gdk_window_set_cursor(mainwin->window, makecursor(GDK_ICON));
  return FALSE;
}


static gint leveldraw_button_release_event(G_GNUC_UNUSED GtkWidget *widget, GdkEventButton *event) {
  int x = event->x/pixwidth;
  int y = event->y/pixheight;
  if (animmode) return FALSE;
  gdk_window_set_cursor(mainwin->window, makecursor(GDK_LEFT_PTR));
  if (checked || (netmode && (pendack || paralyzed))) return FALSE;
  if (x <= 0 || y <= 0 || x >= gridx + 1 || y >= gridy + 1) {
    dragorigin = PlNowhere;
    return FALSE;
  }
  x--; y--;  
  switch (dragorigin) {
  case PlNowhere:
    return FALSE;
  case PlLevel:
    if (x == dragx && y == dragy && dragmode == 0) {
      if (netmode && (category == CatShared)) netsend('F', x, y);
      grid[x+y*gridx] = nextobj(grid[x+y*gridx]);
      drawtile(x, y);
      /*
      gdk_draw_pixmap(levelpixmap, widget->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], 
		      tilepic[grid[x+y*gridx]], 0, 0, (x+1)*pixwidth, (y+1)*pixheight,
		      pixwidth, pixheight);
      gdk_draw_pixmap(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], 
		      tilepic[grid[x+y*gridx]], 0, 0, (x+1)*pixwidth, (y+1)*pixheight,
		      pixwidth, pixheight);
      */
      dragorigin = PlNowhere;
      return FALSE;
    }
    else {
      if (grid[x+y*gridx] != ObNone) {dragorigin = PlNowhere; return FALSE;}
      if (netmode && (category == CatShared)) netsend('M', dragx, dragy, x, y);
      grid[x+y*gridx] = dragitem; grid[dragx+dragy*gridx] = ObNone;
      drawtile(x, y);
      drawtile(dragx, dragy);
      /*
      gdk_draw_pixmap(levelpixmap, widget->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], 
		      tilepic[dragitem], 0, 0, (x+1)*pixwidth, (y+1)*pixheight,
		      pixwidth, pixheight); 
      gdk_draw_pixmap(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], 
		      tilepic[dragitem], 0, 0, (x+1)*pixwidth, (y+1)*pixheight,
		      pixwidth, pixheight);
      gdk_draw_pixmap(levelpixmap, widget->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], 
		      tilepic[ObNone], 0, 0, (dragx+1)*pixwidth, (dragy+1)*pixheight, 
		      pixwidth, pixheight); 
      gdk_draw_pixmap(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], 
		      tilepic[ObNone], 0, 0, (dragx+1)*pixwidth, (dragy+1)*pixheight, 
		      pixwidth, pixheight);
      */
      dragorigin = PlNowhere;
      return FALSE;
    }
  case PlToolbar:
    if (grid[x+y*gridx] != ObNone) {dragorigin = PlNowhere; return FALSE;}
    if (netmode && (category == CatShared)) netsend('I', dragitem, x, y);
    dragbuf[dragitem]--;
    bufsize--;
    dragitem = drtoobj(dragitem);
    grid[x+y*gridx] = dragitem;
    drawtile(x, y);
    /*
    gdk_draw_pixmap(levelpixmap, widget->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], 
		    tilepic[dragitem], 0, 0, (x+1)*pixwidth, (y+1)*pixheight,
		    pixwidth, pixheight);
    gdk_draw_pixmap(widget->window, widget->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], 
		    tilepic[dragitem], 0, 0, (x+1)*pixwidth, (y+1)*pixheight,
		    pixwidth, pixheight);
    */
    bardraw(); /* Then refresh the source */
    dragorigin = PlNowhere;
    return FALSE;
  } 
  return FALSE;
}

static gint tooldraw_button_release_event(G_GNUC_UNUSED GtkWidget *widget, G_GNUC_UNUSED GdkEventButton *event) {
  if (animmode) return FALSE;  
  gdk_window_set_cursor(mainwin->window, makecursor(GDK_LEFT_PTR));
  if (checked || (netmode && (pendack || paralyzed))) {dragorigin = PlNowhere; return FALSE;}
  switch (dragorigin) {
  case PlNowhere:
  case PlToolbar:
    dragorigin = PlNowhere;
    return FALSE;
  case PlLevel:
    if (netmode && (category == CatShared)) netsend('R', dragx, dragy);
    dragbuf[objtodr(dragitem)]++;
    bufsize++;
    grid[dragx + dragy*gridx] = ObNone;
    drawtile(dragx, dragy);
    /*
    gdk_draw_pixmap(levelpixmap, widget->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], 
		    tilepic[ObNone], 0, 0, (dragx+1)*pixwidth, (dragy+1)*pixheight, 
		    pixwidth, pixheight); 
    gdk_draw_pixmap(leveldraw->window, widget->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], 
		    tilepic[ObNone], 0, 0, (dragx+1)*pixwidth, (dragy+1)*pixheight, 
		    pixwidth, pixheight);
    */
    bardraw(); 
    dragorigin = PlNowhere;
    return FALSE;
  }
  return FALSE;
}



/* Initializers */

#define makepm(a) gdk_pixmap_create_from_xpm_d(mainwin->window, &trashmask, NULL, a)
#define makemaskpm(a,b) gdk_pixmap_create_from_xpm_d(mainwin->window, &b, NULL, a)

void initpixmaps() { /* I really should be able to include raw GTK pixmaps, but I don't know of a way to. I have a feeling this is because GTK pixmaps are actually server-side X pixmaps. */
  /* Non-masked (solid tile) */
  tilepic[0] = makepm(tile00_xpm);
  tilepic[1] = makepm(tile01_xpm);
  tilepic[2] = makepm(tile02_xpm);
  tilepic[3] = makepm(tile03_xpm);
  tilepic[4] = makepm(tile04_xpm);
  tilepic[5] = makepm(tile05_xpm);
  tilepic[6] = makepm(tile06_xpm);
  tilepic[7] = makepm(tile07_xpm);
  tilepic[8] = makepm(tile08_xpm);
  tilepic[9] = makepm(tile09_xpm);
  tilepic[10] = makepm(tile10_xpm);
  tilepic[11] = makepm(tile11_xpm);
  tilepic[12] = makepm(tile12_xpm);
  tilepic[13] = makepm(tile13_xpm);
  tilepic[14] = makepm(tile14_xpm);
  tilepic[15] = makepm(tile15_xpm);
  tilepic[16] = makepm(tile16_xpm);
  tilepic[17] = makepm(tile17_xpm);
  tilepic[18] = makepm(tile18_xpm);
  tilepic[19] = makepm(tile19_xpm);
  tilepic[20] = makepm(tile20_xpm);
  tilepic[21] = makepm(tile21_xpm);
  tilepic[22] = makepm(tile22_xpm);
  tilepic[23] = makepm(tile23_xpm);
  tilepic[24] = makepm(tile24_xpm);
  tilepic[25] = makepm(tile25_xpm);
  tilepic[26] = makepm(tile26_xpm);
  tilepic[27] = makepm(tile27_xpm);
  tilepic[28] = makepm(tile28_xpm);
  tilepic[29] = makepm(tile29_xpm);
  tilepic[30] = makepm(tile30_xpm);
  tilepic[31] = makepm(tile31_xpm);
  tilepic[32] = makepm(tile32_xpm);
  borderpix = makepm(picBorder);
  /* Masked (sprite) */
  arrowpix[DUp] = makemaskpm(picDUp, arrowmask[DUp]);
  arrowpix[DRight] = makemaskpm(picDRight, arrowmask[DRight]);
  arrowpix[DDown] = makemaskpm(picDDown, arrowmask[DDown]);
  arrowpix[DLeft] = makemaskpm(picDLeft, arrowmask[DLeft]);
  warrowpix[DUp] = makepm(picDUpW);
  warrowpix[DRight] = makepm(picDRightW);
  warrowpix[DDown] = makepm(picDDownW);
  warrowpix[DLeft] = makepm(picDLeftW);
  markpix[0] = makemaskpm(picMkCircle, markmask[0]);
  markpix[1] = makemaskpm(picMkTriangle, markmask[1]);
  markpix[2] = makemaskpm(picMkSquare, markmask[2]);
  marblepix = makemaskpm(picMarble, marblemask);
  iconpix = makemaskpm(picIcon, iconmask);
  barpix[INew] = makemaskpm(picINew, barmask[INew]);
  barpix[IStop] = makemaskpm(picIStop, barmask[IStop]);
  barpix[ICheck] = makemaskpm(picICheck, barmask[ICheck]);
  barpix[INet] = makemaskpm(picINet, barmask[INet]);
  barpix[INetTurn] = makemaskpm(picINetTurn, barmask[INetTurn]);
  barpix[IFlipBoards] = makemaskpm(picIFlipBoards, barmask[IFlipBoards]);
  barpix[IFlipBack] = makemaskpm(picIFlipBack, barmask[IFlipBack]);
  barpix[IAbout] = makemaskpm(picIAbout, barmask[IAbout]);
  barpix[IOpen] = makemaskpm(picIOpen, barmask[IOpen]);
  barpix[ISave] = makemaskpm(picISave, barmask[ISave]);
}


void initmainwin() {
  mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_window_set_policy(GTK_WINDOW(mainwin), TRUE, FALSE, TRUE);
  mainstyle = gtk_widget_get_style(mainwin);
  gtk_window_set_title(GTK_WINDOW(mainwin),"GFingerPoken");
  gtk_signal_connect(GTK_OBJECT(mainwin), "delete_event", (GtkSignalFunc)mainwin_delete_event, NULL);
  winpack = gtk_hbox_new(FALSE, 0);
  controlpack = gtk_vbox_new(FALSE, 0);
  mainbar = gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
  gtk_widget_show(mainbar);


  gtk_container_add(GTK_CONTAINER(mainwin), controlpack);
  gtk_box_pack_start(GTK_BOX(controlpack), mainbar, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(controlpack), winpack, TRUE, TRUE, 0);
  mainstatbar = gtk_statusbar_new();
  mainstatmsg = gtk_statusbar_get_context_id(GTK_STATUSBAR(mainstatbar), "Main Network Status");
  gtk_box_pack_start(GTK_BOX(controlpack), mainstatbar, FALSE, FALSE, 0);
  gtk_widget_show(controlpack);
  leveldraw = gtk_drawing_area_new();
  levelscroll = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(levelscroll), leveldraw);
  gtk_widget_show(levelscroll); gtk_widget_show(leveldraw);
  tooldraw = gtk_drawing_area_new();
  toolscroll = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(toolscroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(toolscroll), tooldraw);
  gtk_widget_show(toolscroll); gtk_widget_show(tooldraw);
  gtk_box_pack_start(GTK_BOX(winpack), levelscroll, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(winpack), toolscroll, FALSE, FALSE, 0);
  gtk_widget_show(winpack);

  gtk_signal_connect(GTK_OBJECT(leveldraw), "expose_event", (GtkSignalFunc)leveldraw_expose_event, NULL);
  gtk_signal_connect(GTK_OBJECT(tooldraw), "expose_event",(GtkSignalFunc)tooldraw_expose_event, NULL);
  gtk_signal_connect(GTK_OBJECT(leveldraw), "button_press_event", (GtkSignalFunc)leveldraw_button_press_event, NULL);
  gtk_signal_connect(GTK_OBJECT(leveldraw), "button_release_event", (GtkSignalFunc)leveldraw_button_release_event, NULL);
  gtk_signal_connect(GTK_OBJECT(leveldraw), "motion_notify_event", (GtkSignalFunc)leveldraw_motion_notify_event, NULL);
  gtk_signal_connect(GTK_OBJECT(leveldraw), "leave_notify_event", (GtkSignalFunc)draw_leave_notify_event, NULL);
  gtk_signal_connect(GTK_OBJECT(leveldraw), "enter_notify_event", (GtkSignalFunc)simple_enter_notify_event, NULL);  
  gtk_widget_set_events(leveldraw, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK |
			GDK_LEAVE_NOTIFY_MASK | GDK_ENTER_NOTIFY_MASK);
  gtk_signal_connect(GTK_OBJECT(tooldraw), "button_press_event", (GtkSignalFunc)tooldraw_button_press_event, NULL);
  gtk_signal_connect(GTK_OBJECT(tooldraw), "button_release_event", (GtkSignalFunc)tooldraw_button_release_event, NULL);
  /*  gtk_signal_connect(GTK_OBJECT(tooldraw), "motion_notify_event", (GtkSignalFunc)tooldraw_motion_notify_event, NULL);
   */
  gtk_signal_connect(GTK_OBJECT(tooldraw), "leave_notify_event", (GtkSignalFunc)draw_leave_notify_event, NULL);
  gtk_signal_connect(GTK_OBJECT(tooldraw), "enter_notify_event", (GtkSignalFunc)draw_enter_notify_event, NULL);
  
  gtk_widget_set_events(tooldraw, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
			GDK_LEAVE_NOTIFY_MASK|  GDK_ENTER_NOTIFY_MASK);
  gtk_signal_connect(GTK_OBJECT(mainwin), "button_release_event", (GtkSignalFunc)mainwin_button_release_event, NULL);
  gtk_widget_set_events(mainwin, GDK_BUTTON_RELEASE_MASK);

  gtk_widget_realize(mainwin); /* For whatever reason, this line generates the error message:
Gtk-CRITICAL **: file gtkwindow.c: line 992 (gtk_window_realize): assertion `!GTK_WIDGET_REALIZED (widget)' failed.
			       */
  initpixmaps();  

  barpixwid[INew] = gtk_pixmap_new(barpix[INew], barmask[INew]);
  gtk_widget_show(barpixwid[INew]);
  mainbarbut[INew] = gtk_toolbar_append_item(GTK_TOOLBAR(mainbar), NULL, "Generate new level", NULL, barpixwid[INew], (GtkSignalFunc)newbarbut_event, NULL);
  gtk_widget_show(mainbarbut[INew]);

  barpixwid[IStop] = gtk_pixmap_new(barpix[IStop], barmask[IStop]);
  gtk_widget_show(barpixwid[IStop]);
  mainbarbut[IStop] = gtk_toolbar_append_item(GTK_TOOLBAR(mainbar), NULL, "Terminate network game", NULL, barpixwid[IStop], (GtkSignalFunc)stopbarbut_event, NULL);
  gtk_widget_hide(mainbarbut[IStop]);

  barpixwid[ICheck] = gtk_pixmap_new(barpix[ICheck], barmask[ICheck]);
  gtk_widget_show(barpixwid[ICheck]);
  mainbarbut[ICheck] = gtk_toolbar_append_item(GTK_TOOLBAR(mainbar), NULL, "Check for correctness", NULL, barpixwid[ICheck], (GtkSignalFunc)checkbarbut_event, NULL);
  gtk_widget_show(mainbarbut[ICheck]);

  barpixwid[INet] = gtk_pixmap_new(barpix[INet], barmask[INet]);
  gtk_widget_show(barpixwid[INet]);
  mainbarbut[INet] = gtk_toolbar_append_item(GTK_TOOLBAR(mainbar), NULL, "Initiate network game", NULL, barpixwid[INet], (GtkSignalFunc)netbarbut_event, NULL);
  gtk_widget_show(mainbarbut[INet]);

  barpixwid[INetTurn] = gtk_pixmap_new(barpix[INetTurn], barmask[INetTurn]);
  gtk_widget_show(barpixwid[INet]);
  mainbarbut[INetTurn] = gtk_toolbar_append_item(GTK_TOOLBAR(mainbar), NULL, "Complete turn", NULL, barpixwid[INetTurn], (GtkSignalFunc)netturnbarbut_event, NULL);
  gtk_widget_hide(mainbarbut[INetTurn]);

  barpixwid[IFlipBoards] = gtk_pixmap_new(barpix[IFlipBoards], barmask[IFlipBoards]);
  gtk_widget_show(barpixwid[IFlipBoards]);
  mainbarbut[IFlipBoards] = gtk_toolbar_append_item(GTK_TOOLBAR(mainbar), NULL, "Show your guess", NULL, barpixwid[IFlipBoards], (GtkSignalFunc)flipboardsbarbut_event, NULL);
  gtk_widget_hide(mainbarbut[IFlipBoards]);

  barpixwid[IFlipBack] = gtk_pixmap_new(barpix[IFlipBack], barmask[IFlipBack]);
  gtk_widget_show(barpixwid[IFlipBack]);
  mainbarbut[IFlipBack] = gtk_toolbar_append_item(GTK_TOOLBAR(mainbar), NULL, "Show the solution", NULL, barpixwid[IFlipBack], (GtkSignalFunc)flipboardsbarbut_event, NULL);
  gtk_widget_hide(mainbarbut[IFlipBack]);

  barpixwid[IAbout] = gtk_pixmap_new(barpix[IAbout], barmask[IAbout]);
  gtk_widget_show(barpixwid[IAbout]);
  mainbarbut[IAbout] = gtk_toolbar_append_item(GTK_TOOLBAR(mainbar), NULL, "About GFingerPoken", NULL, barpixwid[IAbout], (GtkSignalFunc)aboutbarbut_event, NULL);
  gtk_widget_show(mainbarbut[IAbout]);

  
  gdk_window_set_icon(mainwin->window, NULL, iconpix, iconmask);
}

void shownetturn() {
  gtk_widget_hide(mainbarbut[INet]);
  gtk_widget_hide(mainbarbut[INew]);
  gtk_widget_show(mainbarbut[IStop]);
  gtk_widget_show(mainstatbar);
}

			     
/* Main */
int main(int argc, char *argv[]) {
  srandom(time(NULL));
  gtk_init(&argc, &argv);
  initmainwin();
  initnetwin();
  initdialog();
  gtk_main();
  return 0;
}




