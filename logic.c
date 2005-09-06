/* logic.c - GFingerPoken Game Logic
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

#include "gfp.h"
#include "common.h"

void grideval(pos x, pos y, vect dir, obj *gr) { /* Note: This routine is evil, in an efficient sort of way. */
  obj saver = gr[gridx*y+x];                    /* Not to claim it couldn't be made better. */
  pos xsave = 0, ysave = 0;

  if (histkeeper && saver != ObNone)
    if (histcheck(x, y, dir, gr, histkeeper)) { 
      evalret = RetEaten;
      return;
    }

  evalret = RetNormal;
  switch (saver) { /* Oh, man, a jump table would be SO perfect here. */
  case ObNone:
    break;
/* Simple S */
  case ObSULDRMirror:
    dir ^= 3;  /* XOR with 3: Flip first two bits. */
    break;
  case ObSURDLMirror:
    dir ^= 1;  /* XOR with 1: Flip first bit. */
    break;
/* Flipper F */
  case ObFULDR_URDLMirror:
    dir ^= 3;
    saver++;
    break;
  case ObFURDL_ULDRMirror:
    dir ^= 1;
    saver--;
    break;
  case ObFULDR_BlkMirror:
    dir ^= 3;
    saver++;
    break;
  case ObFBlk_URDLMirror:
    dir ^= 2; /* Flip both bits. */
    saver++;
    break;
  case ObFURDL_BlkMirror:
    dir ^= 1;
    saver++;
    break;
  case ObFBlk_ULDRMirror:
    dir ^= 2;
    saver -= 3;
    break;
/* Box and Sink B */
  case ObBBox:
    dir ^= 2;
    break;
  case ObBSink:
    dir = DDeath;
    break;
/* Axis A */
  case ObAVertMirror:
    if (dir & 1) /* if bit 1 set */
      dir ^= 2;
    break;
  case ObAHorizMirror:
    if (!(dir & 1)) /* if bit 1 not set*/
      dir ^= 2;
    break;
  case ObAFVertMirror:
    if (dir & 1) /* if bit 0 xor bit 1 */
      dir ^= 2;
    saver++;
    break;
  case ObAFHorizMirror:
    if (!(dir & 1)) /* if not bit 0 xor bit 1 */
      dir ^= 2;
    saver--;
    break;
/* Rotator R*/
  case ObRClock:
    dir++; dir &= 3; /* dir &= 3 same as dir %= 4 but cooler, faster? */
    break;
  case ObRCClock:
    dir += 3; dir &= 3; /* prevents underflow on unsigned */
    break;
  case ObRFClock:
    dir++; dir &= 3;
    saver++;
    break;
  case ObRFCClock:
    dir += 3; dir &= 3;
    saver--;
    break;
/* One-way O*/
  case ObOTULDRMirror:
    if (!(dir & 2)) /* i.e. if not coming FROM top (down) or FROM right (left) */
      dir ^= 3;
    break;
  case ObOBULDRMirror:
    if (dir & 2)
      dir ^= 3;
    break;
  case ObOTURDLMirror:
    if (!((dir & 1) ^ (dir >> 1)))
      dir ^= 1;
    break;
  case ObOBURDLMirror:
    if ((dir & 1) ^ (dir >> 1))
      dir ^= 1;
    break;
/* Evil E */
  case ObEOFCTULDRMirror: /* Since the increments are all relative, the SAME CODE works for each!  JOY! */
  case ObEOFCCTULDRMirror:
    if (!(dir & 2))
      dir ^= 3;
    saver++;
    break;
  case ObEOFCBURDLMirror:
  case ObEOFCCBURDLMirror:
    if ((dir & 1) ^ (dir >> 1))
      dir ^= 1;
    saver++;
    break;
  case ObEOFCBULDRMirror:
  case ObEOFCCBULDRMirror:
    if (dir & 2)
      dir ^= 3;
    saver++;
    break;
  case ObEOFCTURDLMirror:  /* Heh heh, hmm.  He said TURD. */
  case ObEOFCCTURDLMirror: /* Shut up, assfucker.  This code rules. */
    if (!((dir & 1) ^ (dir >> 1)))
      dir ^= 1;
    saver -= 3;
    break;
  case ObEMULDRMirror: /* Too much identical code.  Copying would be wasteful. */
  case ObEMURDLMirror:
    switch (dir) {
    case DUp:
      ysave = y-1;
      xsave = x;
      break;
    case DDown:
      ysave = y+1;
      xsave = x;
      break;
    case DLeft:
      xsave = x-1;
      ysave = y;
      break;
    case DRight:
      xsave = x+1;
      ysave = y;
      break;
    }
    if (saver == ObEMULDRMirror) dir ^= 2;
    dir ^= 1;
    if ((gr[ysave*gridx+xsave] == ObNone) && (xsave >= 0) && (ysave >= 0) && (xsave < gridx) && (ysave < gridy)) { 
      evalret = RetMovement;
      gr[ysave*gridx+xsave] = saver;
      updx = xsave; updy = ysave;
      saver = ObNone;
    }
    break;
  }
  gr[x+y*gridx] = saver;
  switch (dir) {
  case DUp:
    y--;
    break;
  case DDown:
    y++;
    break;
  case DLeft:
    x--;
    break;
  case DRight:
    x++;
    break;
  case DDeath:
    evalret = RetEaten;
    return;
  }
  outy = y; outx = x; outdir = dir;
  return;
}





