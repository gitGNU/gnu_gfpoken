/* main.c - GFingerPoken Main Window, main()
 * Copyright 1999-2000  Martin Hock
 * Copyright 2005-2009  Bas Wijnen <wijnen@debian.org>
 *
 * This file is part of Gfingerpoken.
 *
 * Gfingerpoken is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Gfingerpoken is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#define PNGDIR DATADIR "/png/"

#include <stdlib.h>  /* For rand */
#include <string.h>  /* For memcpy */
#include <stdio.h>   /* Only for printf */
#include <time.h>    /* For time (seed rand) */
#include <errno.h>   /* For errno */

#include "gfp.h"     /* Constants and "global" function declarations */

#include "initial.h" /* Initialize global vars */


extern GtkWidget *dialogwin, *netwin;

int leavecount = 0;


guint mainstatmsg;

static gchar target_name[] = "mirror";
static GtkTargetEntry targets[1] = { target_name, GTK_TARGET_SAME_APP, 0};

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
    animx = buttonx*pixwidth; animy = buttony*pixheight;
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
    if ((outx < 0) || (outy < 0) || (outx + 1 > gridx) || (outy + 1 > gridy) || evalret < 0) { /* All balls must roll to an end. */
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
	} while ((outx >= 0) && (outy >= 0) && (outx < gridx) && (outy < gridy) && (evalret >= 0));
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
    fprintf(stderr, "Insufficient memory for allocating temporary data; needed %zd bytes\n",
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
      } while ((outx >= 0) && (outy >= 0) && (outx < gridx) && (outy < gridy) && evalret >= 0);
      /* printf("Invis out: %d %d %d\n",outx,outy,outdir); */
      if (histkeeper) freehist(histkeeper);
      saveoutx = outx; saveouty = outy; saveret = evalret;
      outx = xcounter; outy = ycounter; outdir = savedir;
      if (histkeeper) histkeeper = inithist(tmpgrid);  
      do {
	grideval(outx, outy, outdir, tmpgrid);
      } while ((outx >= 0) && (outy >= 0) && (outx < gridx) && (outy < gridy) && evalret >= 0);
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
  gamebox(
    "About GFingerPoken","GFingerPoken " VERSION "\n"
    "Code by Martin Hock and Bas Wijnen <wijnen@debian.org>\n"
    "Maintained by Bas Wijnen <wijnen@debian.org>\n"
    "Toolbar graphics by Tuomas Kuosmanen (tigert@gimp.org) from gnome-stock\n"
    "\n"
    "Copyright Martin Hock, Bas Wijnen, Tuomas Kuosmanen\n"
    "\n"
    "Gfingerpoken is free software; you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
    "the Free Software Foundation; either version 3 of the License, or\n"
    "(at your option) any later version.\n"
    "\n"
    "Gfingerpoken is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    "GNU General Public License for more details.\n"
    "\n"
    "You should have received a copy of the GNU General Public License\n"
    "along with this program.  If not, see <http://www.gnu.org/licenses/>.\n");
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

static gint leveldraw_button_press_event(G_GNUC_UNUSED GtkWidget *widget, GdkEventButton *event) {
  unsigned int counter;
  static guint32 time=0;
  if (animmode) return FALSE;
  buttonx = event->x/pixwidth; buttony = event->y/pixheight; 
  if (buttonx < 0 || buttonx > gridx + 1 || buttony < 0 || buttony > gridy + 1)
	  return FALSE;

  if (((buttonx == 0) + (buttony == 0) + (buttonx == gridx + 1)
			  + (buttony == gridy + 1)) == 1) // i.e. one is true
  {
    if (netmode) {
      if (paralyzed || completed || pendack) return FALSE;
      else {
	if (category == CatShared) netsend('T', buttonx, buttony);
	else {
	  if (opponentdone) mainsetstat("Finish up - Opponent is done");
	  else mainsetstat("Finish up - Opponent is not done");
	}
	gtk_widget_show(mainbarbut[INetTurn]);
	completed = 1;
      }
    }
    for (counter = 0; counter < histsize; counter++)
      if (arrinx[counter] == buttonx && arriny[counter] == buttony)
	      arrinused[counter]=0;
    arrinx[arrhistptr] = buttonx;
    arriny[arrhistptr] = buttony;
    if (buttonx == 0) animdir = DRight;
    if (buttony == 0) animdir = DDown;
    if (buttonx == gridx + 1) animdir = DLeft;
    if (buttony == gridy + 1) animdir = DUp;
    arrindir[arrhistptr]=animdir;
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
  if ((((buttonx <= 0) || (buttonx >= gridx+1)) && ((buttony <= 0) || (buttony >= gridy+1))) || 
      ((grid[(--buttonx) + (--buttony)*gridx] == ObNone) && event->button != 3)) { /* if invalid */
    gdk_window_set_cursor(mainwin->window, makecursor(GDK_LEFT_PTR));
    return FALSE;
  }

  if (event->button == 3 && time != event->time) {
    if (markgrid[buttonx+buttony*gridx] == NumMarks) markgrid[buttonx+buttony*gridx] = MkNone;
    else markgrid[buttonx+buttony*gridx]++;
    drawtile(buttonx, buttony);
    time = event->time;
    return FALSE;
  }

  /*  gdk_window_set_cursor(mainwin->window, makecursor(GDK_ICON)); */
  return FALSE;
}

void leveldraw_button_released (GtkWidget *w, GdkEventButton *event)
{
	if (event->button == 1 && buttonx == (int)(event->x / pixwidth) - 1
			&& buttony == (int)(event->y / pixheight) - 1)
	{
		grid[buttonx + buttony * gridx]
			= nextobj (grid[buttonx + buttony * gridx]);
		drawtile (buttonx, buttony);
	}
}

int drag_x, drag_y; /* drag_x == -1 if source is tooldraw.  */
int drag_source; /* 0 == none, 1 == leveldraw, 2 == tooldraw.  */
gboolean dragging; /* flag to show that a drag is taking place.  */

void undrag (void)
{
	if (drag_source == 1)
		gtk_drag_source_unset (leveldraw);
	else if (drag_source == 2)
		gtk_drag_source_unset (tooldraw);
	drag_source = 0;
}

void leveldraw_pixel_to_tile (gint x, gint y, int *rx, int *ry)
{
	*rx = x / pixwidth - 1;
	*ry = y / pixheight - 1;
	if (*rx < 0 || *ry < 0 || *rx >= gridx || *ry >= gridy)
		*rx = -1;
}

void tooldraw_pixel_to_tile (gint y, int *ry)
{
	*ry = y / pixheight;
	if (*ry < 0 || *ry >= bufsize)
		*ry = -1;
}

/* These motion-notify callbacks must always return FALSE, otherwise the
 * drag-and-drop functions aren't called.  */
gboolean leveldraw_move (GtkWidget *w, GdkEventMotion *e)
{
	if (dragging)
		return FALSE;
	if (drag_source != 1)
		undrag ();
	leveldraw_pixel_to_tile ((gint)e->x, (gint)e->y, &drag_x, &drag_y);
	if (drag_x < 0 || grid[drag_x + drag_y * gridx] == ObNone)
	{
		undrag ();
		return FALSE;
	}

	if (drag_source == 1)
		return FALSE;

	gtk_drag_source_set (leveldraw, GDK_BUTTON1_MASK, targets, 1,
			GDK_ACTION_MOVE);
	drag_source = 1;
	return FALSE;
}

gboolean tooldraw_move (GtkWidget *w, GdkEventMotion *e)
{
	if (dragging)
		return FALSE;
	if (drag_source != 2)
		undrag ();
	tooldraw_pixel_to_tile ((gint)e->y, &drag_y);
	if (drag_y < 0)
	{
		undrag ();
		return FALSE;
	}

	if (drag_source == 2)
		return FALSE;

	gtk_drag_source_set (tooldraw, GDK_BUTTON1_MASK, targets, 1,
			GDK_ACTION_MOVE);
	drag_source = 2;
	return FALSE;
}

int drag_x_orig, drag_y_orig, drag_object;

void leveldraw_drag_begin ()
{
	dragging = TRUE;
	unsigned p = grid[drag_x + drag_y * gridx];
	GdkPixbuf *buf = gdk_pixbuf_get_from_drawable (NULL, tilepic[p],
			gtk_widget_get_colormap (leveldraw),
			0, 0, 0, 0, -1, -1);
	drag_x_orig = drag_x;
	drag_y_orig = drag_y;
	drag_object = p;
	gtk_drag_source_set_icon_pixbuf (leveldraw, buf);
	grid[drag_x + drag_y * gridx] = ObNone;
	drawtile (drag_x, drag_y);
}

void tooldraw_drag_begin ()
{
	dragging = TRUE;
	unsigned p = bufselect (drag_y);
	drag_x_orig = -1;
	drag_y_orig = drag_y;
	drag_object = drtoobj (p);
	GdkPixbuf *buf = gdk_pixbuf_get_from_drawable
		(NULL, tilepic[drag_object],
		 gtk_widget_get_colormap (tooldraw), 0, 0, 0, 0, -1, -1);
	gtk_drag_source_set_icon_pixbuf (tooldraw, buf);
	--dragbuf[p];
	--bufsize;
	bardraw ();
}

gboolean leveldraw_drag_end ()
{
	if (!dragging)
		return;
	dragging = FALSE;
	if (drag_object == ObNone)
		return;
	printf ("object = %d\n", drag_object);
	grid[drag_x_orig + gridx * drag_y_orig] = drag_object;
	drawtile (drag_x_orig, drag_y_orig);
}

void tooldraw_drag_end ()
{
	if (!dragging)
		return;
	dragging = FALSE;
	if (drag_object == ObNone)
		return;
	obj p = objtodr (drag_object);
	++dragbuf[p];
	++bufsize;
	bardraw ();
}

gboolean leveldraw_drag_drop (GtkWidget *widget, GdkDragContext *context,
		gint x, gint y, guint t)
{
	int dx, dy;
	leveldraw_pixel_to_tile (x, y, &dx, &dy);
	if (dx < 0 || grid[dx + dy * gridx] != ObNone)
		return FALSE;
	grid[dx + dy * gridx] = drag_object;
	drag_object = ObNone;
	drawtile (dx, dy);
	gtk_drag_finish (context, TRUE, TRUE, t);
	return TRUE;
}

gboolean tooldraw_drag_drop (GtkWidget *widget, GdkDragContext *context,
		gint x, gint y, guint t)
{
	if (drag_x_orig < 0)
		return FALSE;
	++dragbuf[objtodr (drag_object)];
	++bufsize;
	bardraw ();
	drag_object = ObNone;
	gtk_drag_finish (context, TRUE, TRUE, t);
	return TRUE;
}

gboolean leveldraw_drag_move (GtkWidget *widget, GdkDragContext *context,
		gint x, gint y, guint t)
{
	int dx, dy;
	leveldraw_pixel_to_tile (x, y, &dx, &dy);
	if (dx < 0 || grid[dx + dy * gridx] != ObNone)
		gdk_drag_status (context, 0, t);
	else
		gdk_drag_status (context, GDK_ACTION_MOVE, t);
}

gboolean tooldraw_drag_move (GtkWidget *widget, GdkDragContext *context,
		gint x, gint y, guint t)
{
	if (drag_x_orig < 0)
		gdk_drag_status (context, 0, t);
	else
		gdk_drag_status (context, GDK_ACTION_MOVE, t);
}


/* Initializers */
GdkPixmap *makemaskpm (const char *file, GdkBitmap **mask)
{
  GError *err = NULL;
  GdkPixbuf *im = gdk_pixbuf_new_from_file (file, &err);
  GdkPixmap *pm;
  gint w, h;
  if (!im)
    {
      fprintf (stderr, "Unable to load image %s: %s\n", file, err->message);
      exit (1);
    }
  gdk_pixbuf_render_pixmap_and_mask (im, &pm, mask, 128);
  return pm;
}

#define makepm(file) makemaskpm (file, 0)

void initpixmaps() {
  /*gdk_rgb_init();*/
  /* Non-masked (solid tile) */
  tilepic[0] = makepm(PNGDIR "00_bg.png");
  tilepic[1] = makepm(PNGDIR "01_normal.png");
  tilepic[2] = makepm(PNGDIR "02_normal.png");
  tilepic[3] = makepm(PNGDIR "03_flip2.png");
  tilepic[4] = makepm(PNGDIR "04_flip2.png");
  tilepic[5] = makepm(PNGDIR "05_flip4.png");
  tilepic[6] = makepm(PNGDIR "06_flip4.png");
  tilepic[7] = makepm(PNGDIR "07_flip4.png");
  tilepic[8] = makepm(PNGDIR "08_flip4.png");
  tilepic[9] = makepm(PNGDIR "09_block.png");
  tilepic[10] = makepm(PNGDIR "10_sink.png");
  tilepic[11] = makepm(PNGDIR "11_axial.png");
  tilepic[12] = makepm(PNGDIR "12_axial.png");
  tilepic[13] = makepm(PNGDIR "13_axial2.png");
  tilepic[14] = makepm(PNGDIR "14_axial2.png");
  tilepic[15] = makepm(PNGDIR "15_rotator.png");
  tilepic[16] = makepm(PNGDIR "16_rotator.png");
  tilepic[17] = makepm(PNGDIR "17_rotator2.png");
  tilepic[18] = makepm(PNGDIR "18_rotator2.png");
  tilepic[19] = makepm(PNGDIR "19_half.png");
  tilepic[20] = makepm(PNGDIR "20_half.png");
  tilepic[21] = makepm(PNGDIR "21_half.png");
  tilepic[22] = makepm(PNGDIR "22_half.png");
  tilepic[23] = makepm(PNGDIR "23_half4.png");
  tilepic[24] = makepm(PNGDIR "24_half4.png");
  tilepic[25] = makepm(PNGDIR "25_half4.png");
  tilepic[26] = makepm(PNGDIR "26_half4.png");
  tilepic[27] = makepm(PNGDIR "27_half4.png");
  tilepic[28] = makepm(PNGDIR "28_half4.png");
  tilepic[29] = makepm(PNGDIR "29_half4.png");
  tilepic[30] = makepm(PNGDIR "30_half4.png");
  tilepic[31] = makepm(PNGDIR "31_move.png");
  tilepic[32] = makepm(PNGDIR "32_move.png");
  borderpix = makepm(PNGDIR "picBorder.png");
  /* Masked (sprite) */
  arrowpix[DUp] = makemaskpm(PNGDIR "picDUp.png", &arrowmask[DUp]);
  arrowpix[DRight] = makemaskpm(PNGDIR "picDRight.png", &arrowmask[DRight]);
  arrowpix[DDown] = makemaskpm(PNGDIR "picDDown.png", &arrowmask[DDown]);
  arrowpix[DLeft] = makemaskpm(PNGDIR "picDLeft.png", &arrowmask[DLeft]);
  warrowpix[DUp] = makepm(PNGDIR "picDUpW.png");
  warrowpix[DRight] = makepm(PNGDIR "picDRightW.png");
  warrowpix[DDown] = makepm(PNGDIR "picDDownW.png");
  warrowpix[DLeft] = makepm(PNGDIR "picDLeftW.png");
  markpix[0] = makemaskpm(PNGDIR "picMkCircle.png", &markmask[0]);
  markpix[1] = makemaskpm(PNGDIR "picMkTriangle.png", &markmask[1]);
  markpix[2] = makemaskpm(PNGDIR "picMkSquare.png", &markmask[2]);
  marblepix = makemaskpm(PNGDIR "marbles.png", &marblemask);
  iconpix = makemaskpm(PNGDIR "gfpoken.png", &iconmask);
  barpix[INew] = makemaskpm(PNGDIR "picINew.png", &barmask[INew]);
  barpix[IStop] = makemaskpm(PNGDIR "picIStop.png", &barmask[IStop]);
  barpix[ICheck] = makemaskpm(PNGDIR "picICheck.png", &barmask[ICheck]);
  barpix[INet] = makemaskpm(PNGDIR "picINet.png", &barmask[INet]);
  barpix[INetTurn] = makemaskpm(PNGDIR "picINetTurn.png", &barmask[INetTurn]);
  barpix[IFlipBoards] = makemaskpm(PNGDIR "picIFlipBoards.png", &barmask[IFlipBoards]);
  barpix[IFlipBack] = makemaskpm(PNGDIR "picIFlipBack.png", &barmask[IFlipBack]);
  barpix[IAbout] = makemaskpm(PNGDIR "picIAbout.png", &barmask[IAbout]);
  barpix[IOpen] = makemaskpm(PNGDIR "picIOpen.png", &barmask[IOpen]);
  barpix[ISave] = makemaskpm(PNGDIR "picISave.png", &barmask[ISave]);
}


static void add_button (int code, const gchar *hint, GtkSignalFunc cb, int show)
{
  barpixwid[code] = gtk_pixmap_new(barpix[code], barmask[code]);
  gtk_widget_show(barpixwid[code]);
  mainbarbut[code] = gtk_toolbar_append_item(GTK_TOOLBAR(mainbar), NULL, hint,
		  NULL, barpixwid[code], cb, NULL);
  if (show)
  	gtk_widget_show(mainbarbut[code]);
  else
  	gtk_widget_hide(mainbarbut[code]);
}

void initmainwin() {
  mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_window_set_policy(GTK_WINDOW(mainwin), TRUE, FALSE, TRUE);
  mainstyle = gtk_widget_get_style(mainwin);
  gtk_window_set_title(GTK_WINDOW(mainwin),"GFingerPoken");
  gtk_signal_connect(GTK_OBJECT(mainwin), "delete_event", (GtkSignalFunc)mainwin_delete_event, NULL);
  winpack = gtk_hbox_new(FALSE, 0);
  controlpack = gtk_vbox_new(FALSE, 0);
  mainbar = gtk_toolbar_new();
  gtk_toolbar_set_style (GTK_TOOLBAR(mainbar), GTK_TOOLBAR_ICONS);
  gtk_toolbar_set_orientation (GTK_TOOLBAR(mainbar), GTK_ORIENTATION_HORIZONTAL);
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
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(levelscroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
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
  gtk_signal_connect(GTK_OBJECT(leveldraw), "button_release_event", (GtkSignalFunc)leveldraw_button_released, NULL);
  gtk_signal_connect(GTK_OBJECT(leveldraw), "motion_notify_event", (GtkSignalFunc)leveldraw_move, NULL);
  gtk_drag_dest_set(leveldraw, GTK_DEST_DEFAULT_MOTION | GTK_DEST_DEFAULT_DROP,
		  targets, 1, GDK_ACTION_MOVE);
  gtk_widget_set_events(leveldraw, GDK_BUTTON_PRESS_MASK
		  | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);
  gtk_signal_connect(GTK_OBJECT(leveldraw), "drag-begin", (GtkSignalFunc)leveldraw_drag_begin, NULL);
  gtk_signal_connect(GTK_OBJECT(leveldraw), "drag-drop", (GtkSignalFunc)leveldraw_drag_drop, NULL);
  gtk_signal_connect(GTK_OBJECT(leveldraw), "drag-motion", (GtkSignalFunc)leveldraw_drag_move, NULL);
  gtk_signal_connect(GTK_OBJECT(leveldraw), "drag-end", (GtkSignalFunc)leveldraw_drag_end, NULL);
  gtk_signal_connect(GTK_OBJECT(tooldraw), "motion_notify_event", (GtkSignalFunc)tooldraw_move, NULL);
  gtk_drag_dest_set(tooldraw, GTK_DEST_DEFAULT_MOTION | GTK_DEST_DEFAULT_DROP,
		  targets, 1, GDK_ACTION_MOVE);
  gtk_signal_connect(GTK_OBJECT(tooldraw), "drag-begin", (GtkSignalFunc)tooldraw_drag_begin, NULL);
  gtk_signal_connect(GTK_OBJECT(tooldraw), "drag-drop", (GtkSignalFunc)tooldraw_drag_drop, NULL);
  gtk_signal_connect(GTK_OBJECT(tooldraw), "drag-motion", (GtkSignalFunc)tooldraw_drag_move, NULL);
  gtk_signal_connect(GTK_OBJECT(tooldraw), "drag-end", (GtkSignalFunc)tooldraw_drag_end, NULL);
  
  gtk_widget_set_events(tooldraw, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);

  gtk_widget_realize(mainwin);
  initpixmaps();

  add_button (INew, "Generate new level", (GtkSignalFunc)newbarbut_event, 1);
  add_button (ICheck, "Check for correctness", (GtkSignalFunc)checkbarbut_event, 0);
  add_button (INet, "Initiate network game", (GtkSignalFunc)netbarbut_event, 1);
  add_button (INetTurn, "Complete turn", (GtkSignalFunc)netturnbarbut_event, 0);
  add_button (IFlipBoards, "Show your guess", (GtkSignalFunc)flipboardsbarbut_event, 0);
  add_button (IFlipBack, "Show the solution", (GtkSignalFunc)flipboardsbarbut_event, 0);
  add_button (IAbout, "About GFingerPoken", (GtkSignalFunc)aboutbarbut_event, 1);
  add_button (IStop, "Terminate network game", (GtkSignalFunc)stopbarbut_event, 0);

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
  gtk_widget_push_visual(gdk_get_visual());
  gtk_widget_push_colormap(gdk_screen_get_default_colormap(gdk_screen_get_default()));
  initmainwin();
  initnetwin();
  initdialog();
  gtk_main();
  return 0;
}
