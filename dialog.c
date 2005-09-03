/********************************************
 GFingerPoken Configuration Dialog
********************************************/

#include <stdlib.h> /* For initlevel */
#include <stdio.h> /* For error messages */
#include <string.h> /* For config file string manipulation */
#include "gfp.h"
#include "common.h"
#include "graphics.h"



GtkWidget *dialogwin, *dialogpack, *dialoggrid;

GtkWidget *dialogbar;
GtkWidget *dialogbarbut[NumDialogIcons];

GtkWidget *proppack, *propscale[4], *proplabel[4]; /* X, Y, density, fuzz */
GtkObject *propadjustment[4];

GtkWidget *classpack[NumClasses], *classlabel[NumClasses], *classpicpack[NumClasses], 
  *classpicleft[NumClasses], *classpicright[NumClasses], *classcale[NumClasses]; /* Simple mirrors, rotators, etc. */
GtkObject *classadjustment[NumClasses];

GtkWidget *buttonpack, *okbutton, *cancelbutton;


/*
obj testgrid[] = {1, 0, 2, 0, 0, 0,
		  0, 0, 2, 1, 2, 1,
		  0, 1, 0, 1, 1, 0,
		  0, 2, 0, 0, 0, 1,
		  1, 0, 2, 0, 0, 0,
		  0, 1, 0, 0, 0, 0};

obj testinvisigrid[] = {0, 0, 0, 1, 2, 0,
			0, 1, 0, 1, 1, 1,
			0, 1, 0, 0, 0, 2,
			0, 2, 0, 0, 0, 1,
			1, 0, 2, 0, 0, 0,
			0, 1, 0, 0, 0, 2};
*/
void initlevel() { /* Create new level, put it in grid, etc. */
  int counter=0, upperbound=0, sum=0, randinc=gridx*gridy+1, randland=0, randpop=0, randchoice=0;
  obj tempobj=ObNone;
  pos x=0, y=0;
  for (; counter < DrNum; counter++) dragbuf[counter] = 0;
  arrhistptr = 0;
  for (counter = 0; counter < histsize; counter++) {
    arrinused[counter] = 0;
    arroutused[counter] = 0;
  }
  bufsize = 0; 
  checked = 0;
  for (counter=0; counter < NumClasses; counter++) sum += classweight[counter];
  /* empty = (int)((double)sum/(double)(fill/100)) + sum; */


  if (grid) free(grid); grid = (obj *)calloc(gridx*gridy, sizeof(obj));
  if (invisigrid) free(invisigrid); invisigrid = (obj *)calloc(gridx*gridy, sizeof(obj));
  if (markgrid) free(markgrid); markgrid = (obj *)calloc(gridx*gridy, sizeof(obj));
  if (!(grid && invisigrid && markgrid)) {
    fprintf(stderr, "Insufficient memory for allocating level; needed %d bytes\n",
	    3*gridx*gridy*sizeof(obj));
    exit(1);
  }  

  randpop = (int)(density*gridx*gridy/100 + 1);
  randpop += (random() % (int)(randpop*densityfuzz*2/100 + 1)) - (int)(randpop*densityfuzz/100);
  if (randpop > gridx*gridy) randpop = gridx*gridy;
  if (randpop < 0) randpop = 0;
  for (; randpop > 0; randpop--) {
    randland = random() % randinc;
    randinc--;
    x=0; y=0;
    while (y < gridy) {
      if (!(invisigrid[x+gridx*y] || --randland)) {
	randchoice = random() % sum;
	upperbound = 0;
	for (counter=0; counter < NumClasses; counter++) {
	  upperbound += classweight[counter];
	  if (upperbound == 0) continue; /* Base case */
	  if (randchoice < upperbound) {
	    switch (counter) {
	    case ClSimple:
	      tempobj = (random() % 2)+ObSULDRMirror;
	      break;
	    case ClFlipper:
	      tempobj = (random() % 6)+ObFULDR_URDLMirror;
	      break;
	    case ClBoxSink:
	      tempobj = (random() % 2)+ObBBox;
	      break;
	    case ClAxis:
	      tempobj = (random() % 4)+ObAVertMirror;
	      break;
	    case ClRotator:
	      tempobj = (random() % 4)+ObRClock;
	      break;
	    case ClOneWay:
	      tempobj = (random() % 4)+ObOTULDRMirror;
	      break;
	    case ClEvil:
	      tempobj = (random() % 10)+ObEOFCTULDRMirror;
	      break;
	    default:
	      exit(1);
	    }
	    invisigrid[x+gridx*y] = tempobj;
	    dragbuf[objtodr(tempobj)]++;
	    bufsize++;
	    break;
	  }
	  
	}
      }
      x++; 
      if (x == gridx) {
	x = 0;
	y++;
      }
    }
  }
  if (dragbuf[DrOTULDRMirror] || dragbuf[DrOBULDRMirror] || dragbuf[DrOTURDLMirror] || dragbuf[DrOBURDLMirror] || dragbuf[DrEOFC4CycleMirror] || dragbuf[DrEOFCC4CycleMirror]) /* All the oneways */
    histkeeper = (histhash *)(!NULL); /* Any non-zero value OK - This is not a sentinel! */
  else histkeeper = NULL;
  gtk_widget_hide(mainbarbut[IFlipBoards]);
  gtk_widget_hide(mainbarbut[IFlipBack]);
}


  /* Old code for testing game with level mix similar to default.
  int x, y = 0;
  obj tempobj;
  if (grid) free(grid); grid = (obj *)calloc(gridx*gridy, sizeof(obj));
  if (invisigrid) free(invisigrid); invisigrid = (obj *)calloc(gridx*gridy, sizeof(obj));
  if (!(grid && invisigrid)) exit(0);
  for (; y < DrNum; y++) dragbuf[y] = 0;
  for (y = 0; y < gridy; y++) {
    for (x = 0; x < gridx; x++) { 
      if ((tempobj = (random() % 48)) < 12) {
        if (tempobj < 4) tempobj = tempobj/2 + 1;
        else if (tempobj < 8) tempobj = tempobj/2 + 1;
	else tempobj = tempobj - 4 + 1;
	invisigrid[x+gridx*y] = tempobj;
	dragbuf[objtodr(tempobj)]++;
	bufsize++;
      }
    }
  }
  originalbuf = bufsize;
}
  */

int savelevel(char *filename) {
  FILE *savefile = NULL;
  int x, y=0;
  if ((savefile = fopen(filename,"w"))) { 
    fprintf(savefile, "GFPSave %d\n", GFPSaveVer); /* Assumes that everything is properly laid out */
    fprintf(savefile, "%d %d\n", gridx, gridy); /* Don't forget the sizes */
    for (; y < gridy; y++) { /* First your grid */
      for (x = 0; x < gridx; x++)
	fprintf(savefile, "%d ", grid[x+y*gridx]);
      fprintf(savefile, "\n");
    }
    for (y=0; y < gridy; y++) { /* Next the mark grid */
      for (x = 0; x < gridx; x++)
	fprintf(savefile, "%d ", markgrid[x+y*gridx]);
      fprintf(savefile, "\n");
    }
    for (y=0; y < gridy; y++) { /* Then the invisible grid */
      for (x = 0; x < gridx; x++)
	fprintf(savefile, "%d ", invisigrid[x+y*gridx]);
      fprintf(savefile, "\n");
    }
    for (y=0; y < DrNum; y++) /* Finally the drag buffer's contents */
      fprintf(savefile, "%d ", dragbuf[y]); /* Provably under 80 characters */
    fprintf(savefile, "\n");
    fclose(savefile);
    return 0;
  }
  return 1; /* Error */
}

int loadlevel(char *filename) { /* Max 80 char width: Max 20 width levels, so A-OK */
  FILE *savefile = NULL;
  char readbuf[80];
  char header[80];
  char *p;
  int x, y=0;
  sprintf(header, "GFPSave %d\n", GFPSaveVer);
  if ((savefile = fopen(filename,"r"))) {
    if (fgets(readbuf, 79, savefile)) {
      if (!strcmp(readbuf, header)) {
	fgets(readbuf, 79, savefile);
	p = readbuf;
	gridx = strtol(p, &p, 10);
	gridy = strtol(p, &p, 10);
	if (grid) free(grid); grid = (obj *)malloc(gridx*gridy*sizeof(obj));
	if (invisigrid) free(invisigrid); invisigrid = (obj *)malloc(gridx*gridy*sizeof(obj));
	if (markgrid) free(markgrid); markgrid = (obj *)malloc(gridx*gridy*sizeof(obj));
	if (!(grid && invisigrid && markgrid)) {
	  fprintf(stderr, "Insufficient memory for allocating level; needed %d bytes\n",
		  3*gridx*gridy*sizeof(obj));
	  exit(1);
	}  

	bufsize = checked = netmode = 0;	
	
	for (; y < gridy; y++) { /* First your grid */
	  fgets(readbuf, 79, savefile);
	  p = readbuf;
	  for (x = 0; x < gridx; x++)
	    grid[x+y*gridx] = strtol(p, &p, 10);
	}

	for (y = 0; y < gridy; y++) {  /* Next the mark grid */
	  fgets(readbuf, 79, savefile);
	  p = readbuf;
	  for (x = 0; x < gridx; x++)
	    markgrid[x+y*gridx] = strtol(p, &p, 10);
	}

	for (y = 0; y < gridy; y++) {  /* Then the invisible grid */
	  fgets(readbuf, 79, savefile);
	  p = readbuf;
	  for (x = 0; x < gridx; x++)
	    invisigrid[x+y*gridx] = strtol(p, &p, 10);
	}

	fgets(readbuf, 79, savefile);
	p = readbuf;
	for (y = 0; y < DrNum; y++) { /* Finally the drag buffer's contents */
	  dragbuf[y] = strtol(p, &p, 10);
	  bufsize += dragbuf[y];
	}

	fclose(savefile);
	return 0;
      }
    }
    fclose(savefile);
  }
  return 1; /* Error */
}

void filesel_destroy_event(GtkWidget *widget, G_GNUC_UNUSED gpointer data) {
  gtk_grab_remove(widget);
}


/*
static gint filecancelbutton_clicked(GtkWidget *widget, gpointer data) {
  gtk_widget_destroy(data);
}
*/

static gint openfileok_button_clicked(G_GNUC_UNUSED GtkWidget *widget, gpointer data) {
  if (loadlevel(gtk_file_selection_get_filename(GTK_FILE_SELECTION(data)))) {
    gamebox("Error", "Could not load file");
    gtk_widget_destroy(data);
  }
  resizelevel();
  gtk_widget_show(mainwin);
  fullgriddraw();
  bardraw();
  gtk_widget_hide(dialogwin);
  gtk_widget_show(dialogbarbut[ISave-NumMainIcons]);
  gamestatus = 1;
  checked = 0;
  netmode = 0;
  gtk_widget_destroy(data);
  return FALSE;
}

static gint savefileok_button_clicked(G_GNUC_UNUSED GtkWidget *widget, gpointer data) {
  if (savelevel(gtk_file_selection_get_filename(GTK_FILE_SELECTION(data)))) 
    gamebox("Error", "Could not save level");
  gtk_widget_destroy(data);
  return FALSE;
}


static gint openbarbut_event(G_GNUC_UNUSED GtkWidget *widget, G_GNUC_UNUSED gpointer data) {
  GtkWidget *filesel = gtk_file_selection_new("Open GFP Game");
  gtk_signal_connect(GTK_OBJECT(filesel), "destroy", (GtkSignalFunc)filesel_destroy_event, filesel);
  gtk_window_set_modal(GTK_WINDOW(filesel), TRUE);
  gtk_file_selection_complete(GTK_FILE_SELECTION(filesel), "*.gfp");
  gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION(filesel)->cancel_button), "clicked", 
			    (GtkSignalFunc)gtk_widget_destroy, GTK_OBJECT(filesel));
  gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(filesel)->ok_button), "clicked", 
		     (GtkSignalFunc)openfileok_button_clicked, filesel);
  gtk_widget_show(filesel);
  return FALSE;
}

static gint savebarbut_event(G_GNUC_UNUSED GtkWidget *widget, G_GNUC_UNUSED gpointer data) {
  GtkWidget *filesel = gtk_file_selection_new("Save GFP Game");
  gtk_signal_connect(GTK_OBJECT(filesel), "destroy", (GtkSignalFunc)filesel_destroy_event, filesel);
  gtk_window_set_modal(GTK_WINDOW(filesel), TRUE);
  gtk_file_selection_complete(GTK_FILE_SELECTION(filesel), "*.gfp");
  gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION(filesel)->cancel_button), "clicked", 
			    (GtkSignalFunc)gtk_widget_destroy, GTK_OBJECT(filesel));
  gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(filesel)->ok_button), "clicked", 
		     (GtkSignalFunc)savefileok_button_clicked, filesel);
  gtk_widget_show(filesel);
  return FALSE;
}

static gint dialogwin_delete_event(G_GNUC_UNUSED GtkWidget *widget, G_GNUC_UNUSED GdkEvent *event, G_GNUC_UNUSED gpointer data) {
  if (netmode) return FALSE;
  if (gamestatus == 0) {
    gtk_main_quit();
    return FALSE;
  }
  gtk_widget_hide(dialogwin);
  gtk_widget_show(mainwin);
  return TRUE;
}

static gint cancelbutton_clicked(G_GNUC_UNUSED GtkWidget *widget, G_GNUC_UNUSED gpointer data) {
  if (netmode) return FALSE;
    if (gamestatus == 0) {
    gtk_main_quit();
    return FALSE;
  }
  gtk_widget_hide(dialogwin);
  /* if (setmainpos) gtk_widget_set_uposition(mainwin, mainxpos, mainypos); */
  gtk_widget_show(mainwin);
  gdk_window_show(mainwin->window);
  return TRUE;
}

static gint okbutton_clicked(G_GNUC_UNUSED GtkWidget *widget, G_GNUC_UNUSED gpointer data) {
  int i=0, tempsum=0;
  FILE *configfile;
  char configloc[80]; /* Good Enough */
  for (; i < NumClasses; i++) {
    classweight[i] = GTK_ADJUSTMENT(classadjustment[i])->value;
    tempsum += classweight[i];
  }
  if (tempsum == 0 ) return FALSE; /* Nothing selected */
  gridx = GTK_ADJUSTMENT(propadjustment[0])->value; 
  gridy = GTK_ADJUSTMENT(propadjustment[1])->value;
  density = GTK_ADJUSTMENT(propadjustment[2])->value; 
  densityfuzz = GTK_ADJUSTMENT(propadjustment[3])->value;
  strncpy(configloc,getenv("HOME"),65);
  strcat(configloc,"/.gfpoken");
  if ((configfile = fopen(configloc,"w"))) {
    fprintf(configfile, "%d\n",GFPVersion);
    fprintf(configfile, "%d %d %d %d %d %d %d %d %d %d %d\n", gridx, gridy, density, densityfuzz,
	    classweight[0], classweight[1], classweight[2], classweight[3], classweight[4],
	    classweight[5], classweight[6]);
    fclose(configfile);
  }
  /*
    toolscroll->requisition.width = 0;
    toolscroll->requisition.height = 0;
    levelscroll->requisition.width = 0;
    levelscroll->requisition.height = 0;
  */

  resizelevel();

  /* if (setmainpos) gtk_widget_set_uposition(mainwin, mainxpos, mainypos);  */
  
  if (netmode) { 
    initlevel();
    gtk_widget_hide(dialogwin);
    netsend('S', gridx, gridy);
    return FALSE;
  } else {
    gtk_widget_show(mainwin);
    initlevel();
  }
  fullgriddraw();
  bardraw();
  gtk_widget_hide(dialogwin);
  gtk_widget_show(dialogbarbut[ISave-NumMainIcons]);
  gamestatus = 1;
  return FALSE;
}



void initdialog() {
  int i=0, x=1, y=0;
  FILE *configfile=NULL;
  char readbuf[80];
  int version;
  strncpy(readbuf,getenv("HOME"),65);
  strcat(readbuf,"/.gfpoken");
  if ((configfile = fopen(readbuf,"r"))) {
    if (fgets(readbuf, 80, configfile)) {
      if (sscanf(readbuf, "%d", &version)) {
	if (version == GFPVersion) {
	  if (fgets(readbuf, 80, configfile)) {
	    char *p = readbuf;

	    gridx          = strtol(p, &p, 10);
	    gridy          = strtol(p, &p, 10);
	    density        = strtol(p, &p, 10);
	    densityfuzz    = strtol(p, &p, 10);
	    classweight[0] = strtol(p, &p, 10);
	    classweight[1] = strtol(p, &p, 10);
	    classweight[2] = strtol(p, &p, 10);
	    classweight[3] = strtol(p, &p, 10);
	    classweight[4] = strtol(p, &p, 10);
	    classweight[5] = strtol(p, &p, 10);
	    classweight[6] = strtol(p, &p, 10);
	  }
	}
      }
    }
  }
  if (configfile) fclose(configfile);
  dialogwin = gtk_window_new(GTK_WINDOW_DIALOG);
  gtk_window_set_title(GTK_WINDOW(dialogwin),"GFP Configuration");
  gtk_signal_connect(GTK_OBJECT(dialogwin), "delete_event", (GtkSignalFunc)dialogwin_delete_event, NULL);

  dialogpack = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(dialogwin), dialogpack);
  gtk_widget_show(dialogpack);
  dialogbar = gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
  gtk_box_pack_start(GTK_BOX(dialogpack), dialogbar, FALSE, FALSE, 0);
  gtk_widget_show(dialogbar);

  dialoggrid = gtk_table_new(2, 5, TRUE);
  gtk_box_pack_start(GTK_BOX(dialogpack), dialoggrid, FALSE, FALSE, 0);
  gtk_widget_show(dialoggrid);
  while (i < NumClasses) {
    classpack[i] = gtk_vbox_new(TRUE, 0);
    gtk_table_attach_defaults(GTK_TABLE(dialoggrid), classpack[i], x, x+1, y, y+1);
    classlabel[i] = gtk_label_new(classname(i));
    gtk_box_pack_start(GTK_BOX(classpack[i]), classlabel[i], TRUE, FALSE, 0);
    classpicpack[i] = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(classpack[i]), classpicpack[i], TRUE, TRUE, 0);
    classpicleft[i] = gtk_pixmap_new(tilepic[classleftpic(i)], NULL);
    gtk_box_pack_start(GTK_BOX(classpicpack[i]), classpicleft[i], TRUE, FALSE, 0);
    classpicright[i] = gtk_pixmap_new(tilepic[classrightpic(i)], NULL);
    gtk_box_pack_start(GTK_BOX(classpicpack[i]), classpicright[i], TRUE, FALSE, 0);    
    classadjustment[i]=gtk_adjustment_new(classweight[i], 0, 100, 1, 10, 0);
    classcale[i] = gtk_hscale_new(GTK_ADJUSTMENT(classadjustment[i]));
    gtk_scale_set_digits(GTK_SCALE(classcale[i]), 0);
    gtk_box_pack_start(GTK_BOX(classpack[i]), classcale[i], TRUE, FALSE, 0);
    gtk_widget_show(classcale[i]);
    gtk_widget_show(classlabel[i]);
    gtk_widget_show(classpack[i]);
    gtk_widget_show(classpicpack[i]);
    gtk_widget_show(classpicleft[i]);
    gtk_widget_show(classpicright[i]);
    x++; 
    if (i == 3) {
      x=1; y++;
    }
    i++;
  }

  buttonpack = gtk_vbox_new(FALSE, 0);
  gtk_table_attach_defaults(GTK_TABLE(dialoggrid), buttonpack, 4, 5, 1, 2);
  okbutton = gtk_button_new_with_label("OK");
  /* Problematic - We'll just have it look normal.
  GTK_WIDGET_SET_FLAGS(okbutton, GTK_CAN_DEFAULT | GTK_HAS_DEFAULT | GTK_HAS_GRAB | GTK_RECEIVES_DEFAULT);
  gtk_widget_grab_default(okbutton);
  */
  cancelbutton = gtk_button_new_with_label("Cancel");
  gtk_box_pack_start(GTK_BOX(buttonpack), okbutton, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(buttonpack), cancelbutton, TRUE, FALSE, 0);
  gtk_widget_show(buttonpack);
  gtk_widget_show(okbutton);
  gtk_widget_show(cancelbutton);

  i=0;
  proppack = gtk_vbox_new(TRUE, 0);
  gtk_table_attach_defaults(GTK_TABLE(dialoggrid), proppack, 0, 1, 0, 2);
  gtk_widget_show(proppack);

  propadjustment[0] = gtk_adjustment_new(gridx, 1, 20, 1, 10, 0);
  propadjustment[1] = gtk_adjustment_new(gridy, 1, 20, 1, 10, 0);
  propadjustment[2] = gtk_adjustment_new(density, 0, 100, 1, 10, 0);
  propadjustment[3] = gtk_adjustment_new(densityfuzz, 0, 100, 1, 10, 0);

  while (i < 4) {
    proplabel[i]=gtk_label_new(propname(i));
    gtk_box_pack_start(GTK_BOX(proppack), proplabel[i], FALSE, FALSE, 0);
    propscale[i] = gtk_hscale_new(GTK_ADJUSTMENT(propadjustment[i]));
    gtk_scale_set_digits(GTK_SCALE(propscale[i]),0);
    gtk_box_pack_start(GTK_BOX(proppack), propscale[i], FALSE, FALSE, 0);
    gtk_widget_show(proplabel[i]);
    gtk_widget_show(propscale[i]);
    i++;
  }

  barpixwid[IOpen] = gtk_pixmap_new(barpix[IOpen], barmask[IOpen]);
  gtk_widget_show(barpixwid[IOpen]);
  dialogbarbut[IOpen-NumMainIcons] = gtk_toolbar_append_item(GTK_TOOLBAR(dialogbar), NULL, "Open level file", NULL, barpixwid[IOpen], (GtkSignalFunc)openbarbut_event, NULL);
  gtk_widget_show(dialogbarbut[IOpen-NumMainIcons]);

  barpixwid[ISave] = gtk_pixmap_new(barpix[ISave], barmask[ISave]);
  gtk_widget_show(barpixwid[ISave]);
  dialogbarbut[ISave-NumMainIcons] = gtk_toolbar_append_item(GTK_TOOLBAR(dialogbar), NULL, "Save level", NULL, barpixwid[ISave], (GtkSignalFunc)savebarbut_event, NULL);
  gtk_widget_hide(dialogbarbut[ISave-NumMainIcons]);


  gtk_signal_connect(GTK_OBJECT(okbutton), "clicked", (GtkSignalFunc)okbutton_clicked, NULL);
  gtk_signal_connect(GTK_OBJECT(cancelbutton), "clicked", (GtkSignalFunc)cancelbutton_clicked, NULL);
  gtk_widget_show(dialogwin);
}


