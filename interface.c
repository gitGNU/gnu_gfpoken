/********************************************
 GFingerPoken Graphics Routines (some)
********************************************/

#include "gfp.h"
#include "common.h"
#include "graphics.h"


/* Drawing routines - GTK specific */
void borderdraw() {
  int xcounter, pmloc, ycounter=0;
  arrinused[arrhistptr]=0; arroutused[arrhistptr]=0;
  for (; ycounter < gridy+2; ycounter++) {
    for (xcounter = 0; xcounter < gridx+2; xcounter++) {
      pmloc = 0;
      if (ycounter == 0) pmloc = 1;
      if (ycounter == gridy+1) pmloc = 2;
      if (xcounter == 0) pmloc += 3;
      if (xcounter == gridx+1) pmloc += 6;
      gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
		      borderpix, (pmloc-1)*pixwidth, 0, xcounter*pixwidth, ycounter*pixheight,
		      pixwidth, pixheight);
      if (xcounter == 0 && ycounter != 0 && ycounter != gridy+1) xcounter += gridx; /* Kludge City */
    }
  }
}

void borderrefresh() {
  gdk_draw_pixmap(leveldraw->window, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
		  levelpixmap, pixwidth, 0, pixwidth, 0, pixwidth*gridx, pixheight); /* Top */
  gdk_draw_pixmap(leveldraw->window, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
		  levelpixmap, 0, pixheight, 0, pixheight, pixwidth, pixheight*gridy); /* Left */
  gdk_draw_pixmap(leveldraw->window, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
		  levelpixmap, pixwidth, (gridy+1)*pixheight, pixwidth, (gridy+1)*pixheight,
		  pixwidth*gridx, pixheight); /* Bottom */
  gdk_draw_pixmap(leveldraw->window, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
		  levelpixmap, pixwidth*(gridx+1), pixheight, pixwidth*(gridx+1), pixheight,
		  pixwidth, pixheight*gridy); /* Right */
}

void histdraw(unsigned int item, unsigned char drawmarble, GdkPixmap *ptarrowpix[], GdkBitmap *ptarrowmask[]) { /* Draw a history item */
  if (checked) return;
  if (arrinused[item]) {
    gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], marblemask);
    switch (arrindir[item]) { /* First, the in arrows/ballhalves */
    case DUp: /* As long as I have to do special cases, may as well do the arrows too */
      if (drawmarble) {
	gdk_gc_set_clip_origin(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			       arrinx[item]*pixwidth, pixheight*(gridy+1)-pixheight/2);
	gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			marblepix, item*2*skipsize*pixwidth, pixheight/2, 
			arrinx[item]*pixwidth, pixheight*(gridy+1), pixwidth, pixheight/2);
      }
      gdk_gc_set_clip_origin(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			     arrinx[item]*pixwidth, pixheight*(gridy+1));
      gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], ptarrowmask[DUp]);   
      gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
		      ptarrowpix[DUp], 0, 0, arrinx[item]*pixwidth,
		      pixheight*(gridy+1), pixwidth, pixheight/2);
      break;
    case DDown:
      gdk_gc_set_clip_origin(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			     arrinx[item]*pixwidth, pixheight/2);
      if (drawmarble)
	gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			marblepix, item*2*skipsize*pixwidth, 0, arrinx[item]*pixwidth, 
			pixheight/2, pixwidth, pixheight/2);
      gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], ptarrowmask[DDown]);   
      gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
		      ptarrowpix[DDown], 0, 0, arrinx[item]*pixwidth,
		      pixheight/2, pixwidth, pixheight/2);
      break;
    case DLeft:
      if (drawmarble) {
	gdk_gc_set_clip_origin(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			       pixwidth*(gridx+1)-pixwidth/2, arriny[item]*pixheight);
	gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			marblepix, item*2*skipsize*pixwidth+pixwidth/2, 0, 
			pixwidth*(gridx+1), arriny[item]*pixheight, pixwidth/2, pixheight);
      }
      gdk_gc_set_clip_origin(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			     pixwidth*(gridx+1), arriny[item]*pixheight);
      gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], ptarrowmask[DLeft]);   
      gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
		      ptarrowpix[DLeft], 0, 0, pixwidth*(gridx+1),
		      arriny[item]*pixheight, pixwidth/2, pixheight);
      break;
    case DRight:
      gdk_gc_set_clip_origin(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			     pixwidth/2, arriny[item]*pixheight);
      if (drawmarble)
	gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			marblepix, item*2*skipsize*pixwidth, 0, 
			pixwidth/2, arriny[item]*pixheight, pixwidth/2, pixheight);
      gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], ptarrowmask[DRight]);   
      gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
		      ptarrowpix[DRight], 0, 0, pixwidth/2,
		      arriny[item]*pixheight, pixwidth/2, pixheight);
      break;
    }
  }
  if (arroutused[item]) {
    gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], marblemask);  
    switch (arroutdir[item]) { /* Slightly modified in arrow routines */
    case DUp:
      gdk_gc_set_clip_origin(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			     arroutx[item]*pixwidth, 0);
      if (drawmarble)
	gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			marblepix, item*2*skipsize*pixwidth, 0, arroutx[item]*pixwidth, 
			0, pixwidth, pixheight/2);
      gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], ptarrowmask[DUp]);   
      gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
		      ptarrowpix[DUp], 0, 0, arroutx[item]*pixwidth,
		      0, pixwidth, pixheight/2);
      break;
    case DDown:
      if (drawmarble) {
	gdk_gc_set_clip_origin(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			       arroutx[item]*pixwidth, pixheight*(gridy+1));
	gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			marblepix, item*2*skipsize*pixwidth, pixheight/2, 
			arroutx[item]*pixwidth, pixheight*(gridy+1)+pixheight/2, pixwidth, pixheight/2);
      }
      gdk_gc_set_clip_origin(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			     arroutx[item]*pixwidth, pixheight*(gridy+1)+pixheight/2);
      gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], ptarrowmask[DDown]);   
      gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
		      ptarrowpix[DDown], 0, 0, arroutx[item]*pixwidth,
		      pixheight*(gridy+1)+pixheight/2, pixwidth, pixheight/2);
      break;
    case DLeft:
      gdk_gc_set_clip_origin(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			     0, arrouty[item]*pixheight);
      if (drawmarble)
	gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			marblepix, item*2*skipsize*pixwidth, 0, 
			0, arrouty[item]*pixheight, pixwidth/2, pixheight);
      gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], ptarrowmask[DLeft]);   
      gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
		      ptarrowpix[DLeft], 0, 0, 0,
		      arrouty[item]*pixheight, pixwidth/2, pixheight);
      break;
    case DRight:
      if (drawmarble) {
	gdk_gc_set_clip_origin(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			       pixwidth*(gridx+1), arrouty[item]*pixheight);
	gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			marblepix, item*2*skipsize*pixwidth+pixwidth/2, 0, 
			pixwidth*(gridx+1)+pixwidth/2, arrouty[item]*pixheight, pixwidth/2, pixheight);
      }
      gdk_gc_set_clip_origin(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			     pixwidth*(gridx+1)+pixwidth/2, arrouty[item]*pixheight);
      gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], ptarrowmask[DRight]);   
      gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
		      ptarrowpix[DRight], 0, 0, pixwidth*(gridx+1)+pixwidth/2,
		      arrouty[item]*pixheight, pixwidth/2, pixheight);
      break;
    }
  }
  gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], NULL);
}


void bardraw() { /* This is ONLY used to create a new toolbar, which is frequent */
  int interiorcounter, vref = 0, objcounter = 0;
  gint colordepth;

  if (toolpixmap) {
    gdk_pixmap_unref(toolpixmap);
    toolpixmap = NULL;
  }
  if (!bufsize) {
    gdk_window_clear(tooldraw->window);
    gtk_widget_show(mainbarbut[ICheck]);
    return;
  }
  gtk_widget_hide(mainbarbut[ICheck]);

  colordepth = gdk_window_get_visual(tooldraw->window)->depth;
  toolpixmap = gdk_pixmap_new(tooldraw->window, pixwidth, pixheight*bufsize, colordepth);
  gtk_drawing_area_size(GTK_DRAWING_AREA(tooldraw), pixwidth, pixheight*bufsize);
  for (; objcounter < DrNum; objcounter++)
    for (interiorcounter = dragbuf[objcounter]; interiorcounter > 0; interiorcounter--)
      gdk_draw_pixmap(toolpixmap, tooldraw->style->fg_gc[GTK_WIDGET_STATE(tooldraw)],
		      tilepic[drtoobj(objcounter)], 0, 0, 0, vref++*pixheight,
		      pixwidth, pixheight);
  gdk_window_clear(tooldraw->window);
  gdk_draw_pixmap(tooldraw->window, tooldraw->style->fg_gc[GTK_WIDGET_STATE(tooldraw)],
		  toolpixmap, 0, 0, 0, 0, pixwidth, pixheight*bufsize); 

  /* And now for a horrible refresh hack that works OK */
  GTK_WIDGET_UNSET_FLAGS(GTK_SCROLLED_WINDOW(toolscroll)->vscrollbar, GTK_VISIBLE);
  gtk_widget_show(GTK_SCROLLED_WINDOW(toolscroll)->vscrollbar); 
}

void fullgriddraw() { /* This is ONLY used to create a new grid */ 
  int ycounter=0, xcounter, pmloc;
  gint colordepth;
  if (levelpixmap) gdk_pixmap_unref(levelpixmap);
  colordepth = gdk_window_get_visual(tooldraw->window)->depth;
  levelpixmap = gdk_pixmap_new(mainwin->window, pixwidth*(gridx+2), pixheight*(gridy+2), colordepth);
  gtk_drawing_area_size(GTK_DRAWING_AREA(leveldraw), pixwidth*(gridx+2), pixheight*(gridy+2));
  for (; ycounter < gridy+2; ycounter++) {
    for (xcounter = 0; xcounter < gridx+2; xcounter++) {
      pmloc = 0;
      if (ycounter == 0) pmloc = 1;
      if (ycounter == gridy+1) pmloc = 2;
      if (xcounter == 0) pmloc += 3;
      if (xcounter == gridx+1) pmloc += 6;
      if (pmloc)       
	gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			borderpix, (pmloc-1)*pixwidth, 0, xcounter*pixwidth, ycounter*pixheight,
			pixwidth, pixheight);		     	
      else 
	gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], 
			tilepic[grid[(xcounter-1)+(ycounter-1)*gridx]], 0, 0, xcounter*pixwidth, 
			ycounter*pixheight, pixwidth, pixheight);
    }
  }
  gdk_draw_pixmap(leveldraw->window, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
		  levelpixmap, 0, 0, 0, 0, pixwidth*(gridx+2), pixheight*(gridy+2));


  vdeltabuf = gdk_pixmap_new(leveldraw->window, pixwidth, pixheight+frameskip, colordepth);
  hdeltabuf = gdk_pixmap_new(leveldraw->window, pixwidth+frameskip, pixheight, colordepth);
}

void drawtile(int x, int y) {
      gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], 
		      tilepic[grid[x+y*gridx]], 0, 0, 
		      (x+1)*pixwidth, (y+1)*pixheight, pixwidth, pixheight);
      if (markgrid[x+y*gridx]) {
	gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], markmask[markgrid[x+y*gridx]-1]);  
	gdk_gc_set_clip_origin(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			       (x+1)*pixwidth, (y+1)*pixheight);
	gdk_draw_pixmap(levelpixmap, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
			markpix[markgrid[x+y*gridx]-1], 0, 0,
			(x+1)*pixwidth, (y+1)*pixheight, pixwidth, pixheight);
	gdk_gc_set_clip_mask(leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)], NULL);  
       
      }
      gdk_draw_pixmap(leveldraw->window, leveldraw->style->fg_gc[GTK_WIDGET_STATE(leveldraw)],
		      levelpixmap, (x+1)*pixwidth, (y+1)*pixheight, (x+1)*pixwidth, 
		      (y+1)*pixheight, pixwidth, pixheight);
}

void resizelevel() {
  toolscroll->allocation.width = 0;
  toolscroll->allocation.height = 0;
  levelscroll->allocation.width = 0;
  levelscroll->allocation.height = 0;
  
  
  gtk_widget_set_usize(toolscroll, pixwidth +
		       GTK_SCROLLED_WINDOW(toolscroll)->vscrollbar->allocation.width+7,
		       levelscroll->allocation.height);
  gtk_widget_set_usize(levelscroll, pixwidth*(gridx+2) +
		       GTK_SCROLLED_WINDOW(levelscroll)->vscrollbar->allocation.width+7,
		       pixheight*(gridy+2) +
		       GTK_SCROLLED_WINDOW(levelscroll)->hscrollbar->allocation.height+7);
}



