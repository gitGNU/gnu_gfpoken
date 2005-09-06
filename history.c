/* history.c - GFingerPoken History Hash Table for Halting
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

#include <stdlib.h> /* For malloc */
#include <stdio.h> /* For error message */
#include "gfp.h"
#include "common.h"


histhash *inithist(obj *gr) {
  pos x, y=0;
  histhash *newhash;
  histnode **newnode;
  unsigned int itemcounter=0, changecounter=0, changesize;
  for (; y < gridy; y++)
    for (x=0; x < gridx; x++) {
      switch (gr[x+gridx*y]) {
      case ObNone:
      case ObEMULDRMirror: /* Could be dangerous */
      case ObEMURDLMirror:
	continue;
      case ObSULDRMirror:
      case ObSURDLMirror:
      case ObBBox:
      case ObBSink:
      case ObAVertMirror:
      case ObAHorizMirror:
      case ObRClock:
      case ObRCClock:
      case ObOTULDRMirror:
      case ObOBULDRMirror:
      case ObOTURDLMirror:
      case ObOBURDLMirror:
	itemcounter++;
	continue;
      default:
	itemcounter++;
	changecounter++;
      }
    }
  changesize = changecounter / PackBig;
  if (changecounter % PackBig) changesize++;
  newhash = (histhash*)malloc(sizeof(histhash));
  newnode = (histnode**)calloc(itemcounter, sizeof(histnode*)); /* Zeroed out */
  newhash->size = changesize;
  newhash->hashsize = itemcounter;
  newhash->history = newnode;
  return newhash;
}

int histcheck(pos xpos, pos ypos, vect dir, obj *gr, histhash *hist) { /*Bool*/
  /* First, build history */
  histbit *newhist;
  histnode *node, *newnode;
  pos x, y=0;
  int n, hashspot=-1;
  unsigned int itemcounter=0, changecounter=0;
  unsigned int histregister;
  unsigned int majoroffset, minoroffset;
  if ((gr[xpos+gridx*ypos] == ObEMULDRMirror) || (gr[xpos+gridx*ypos] == ObEMURDLMirror))
    return 0; /* No hash entry for moving mirrors */
  newhist = calloc(hist->size, sizeof(histbit));
  for (; y < gridy; y++)
    for (x=0; x < gridx; x++) {
      switch (gr[x+gridx*y]) {
      case ObNone:
      case ObEMULDRMirror:
      case ObEMURDLMirror:
	continue;
      case ObSULDRMirror:
      case ObSURDLMirror:
      case ObBBox:
      case ObBSink:
      case ObAVertMirror:
      case ObAHorizMirror:
      case ObRClock:
      case ObRCClock:
      case ObOTULDRMirror:
      case ObOBULDRMirror:
      case ObOTURDLMirror:
      case ObOBURDLMirror:
	if ((x == xpos) && (y == ypos)) hashspot = itemcounter;
	itemcounter++;
	continue;
      default:
	if ((x == xpos) && (y == ypos)) hashspot = itemcounter;
	itemcounter++;
	majoroffset = (changecounter*PackSize) / PackBig;
	minoroffset = (changecounter*PackSize) % PackBig;
	histregister = gr[x+gridx*y];

	switch (gr[x+gridx*y]) {
	case ObFULDR_URDLMirror:
	case ObFURDL_ULDRMirror:
	  histregister -= ObFULDR_URDLMirror;
	  break;
	case ObFULDR_BlkMirror:
	case ObFBlk_URDLMirror:
	case ObFURDL_BlkMirror:
	case ObFBlk_ULDRMirror:
	  histregister -= ObFULDR_BlkMirror;
	  break;
	case ObAFVertMirror:
	case ObAFHorizMirror:
	  histregister -= ObAFVertMirror;
	  break;
	case ObRFClock:
	case ObRFCClock:
	  histregister -= ObRFClock;
	  break;
	case ObEOFCTULDRMirror:
	case ObEOFCBURDLMirror:
	case ObEOFCBULDRMirror:
	case ObEOFCTURDLMirror:
	  histregister -= ObEOFCTULDRMirror;
	  break;
	case ObEOFCCBULDRMirror:
	case ObEOFCCBURDLMirror:
	case ObEOFCCTULDRMirror:
	case ObEOFCCTURDLMirror:
	  histregister -= ObEOFCCBULDRMirror;
	  break;
	}
	histregister <<= minoroffset;
	newhist[majoroffset] |= histregister;
	changecounter++;
      }
    }
  if (hashspot < 0 || hashspot >= hist->hashsize) {
    fprintf(stderr, "Hash overrun: Value %d, maximum %d\n",hashspot,hist->hashsize);
    exit(1);
  }
  if (!(hist->history[hashspot])) { /* i.e. Nothing there */
    newnode = (histnode*)malloc(sizeof(histnode));
    newnode->history = newhist;
    newnode->direction = dir;
    newnode->next = NULL;
    hist->history[hashspot] = newnode;
    return 0;
  }  
  node = hist->history[hashspot];
  while (1) { /* Would look nice if it was while (node->next) but that wouldn't work */
    if (node->direction == dir) { /* DEATH HERE */
      for (n=0; n < hist->size; n++)
	if (newhist[n] != node->history[n]) break;    
      if (n == hist->size) return 1; /* Found a match!  Looks like we're loopin' */
    }
    if (!node->next) break;
    node = node->next;
  } /* Got thru without match; gotta add it to the list. */
  newnode = (histnode*)malloc(sizeof(histnode));
  newnode->history = newhist;
  newnode->direction = dir;
  newnode->next = NULL;
  node->next = newnode;
  return 0;
}

void freehist(histhash *hash) {
  int i=0;
  histnode *destroyer, *destroyernext;
  for (; i < hash->hashsize; i++) {
    destroyer = hash->history[i];
    while (destroyer) {
      destroyernext = destroyer->next;
      free(destroyer);
      destroyer = destroyernext;
    }
  }
  free(hash->history);
  free(hash);
}

