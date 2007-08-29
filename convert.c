/* convert.c - GFingerPoken Conversion, 1:1 Style Functions
 * Copyright 1999-2000  Martin Hock
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

#include <stdlib.h>

#include "gfp.h"
#include "common.h"

obj bufselect(pos position) {
  int interiorcounter, objcounter = 0;
  for (; objcounter < DrNum; objcounter++)
    for (interiorcounter = dragbuf[objcounter]; interiorcounter > 0; interiorcounter--) {
      if (position == 0) return objcounter;
      position--;
    }
  return ObNone;
}
  

obj nextobj(obj object) { 
  switch (object) {
  case ObNone:
  case ObSULDRMirror:      /* Non-rotatables */
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
  case ObEMULDRMirror:
  case ObEMURDLMirror:
    return object;
  case ObFULDR_URDLMirror: /* Rotate once */
  case ObFULDR_BlkMirror:
  case ObFBlk_URDLMirror:
  case ObFURDL_BlkMirror:
  case ObRFClock:
  case ObAFVertMirror:
  case ObEOFCTULDRMirror:
  case ObEOFCBURDLMirror:
  case ObEOFCBULDRMirror:
  case ObEOFCCBULDRMirror:
  case ObEOFCCBURDLMirror:
  case ObEOFCCTULDRMirror:
    return ++object;
  case ObFURDL_ULDRMirror: /* Two-cycle wrap */
  case ObRFCClock:
  case ObAFHorizMirror:
    return --object; 
  case ObFBlk_ULDRMirror:  /* Four-cycle wrap */
  case ObEOFCTURDLMirror:
  case ObEOFCCTURDLMirror:
    return (object - 3);
  default:                 /* Unknown case */
    exit(1);
  }
}

obj objtodr(obj object) { 
  switch (object) {
  case ObSULDRMirror:
    return DrSULDRMirror;
  case ObSURDLMirror:
    return DrSURDLMirror;
  case ObFULDR_URDLMirror:
  case ObFURDL_ULDRMirror:
    return DrF2CycleMirror;
  case ObFULDR_BlkMirror:
  case ObFBlk_URDLMirror:
  case ObFURDL_BlkMirror:
  case ObFBlk_ULDRMirror:
    return DrF4CycleMirror;
  case ObBBox:
    return DrBBox;
  case ObBSink:
    return DrBSink;
  case ObAVertMirror:
    return DrAVertMirror;
  case ObAHorizMirror:
    return DrAHorizMirror;
  case ObAFVertMirror:
  case ObAFHorizMirror:
    return DrA2CycleMirror;
  case ObRClock:
    return DrRClock;
  case ObRCClock:
    return DrRCClock;
  case ObRFClock:
  case ObRFCClock:
    return DrR2Cycle;
  case ObOTULDRMirror:
    return DrOTULDRMirror;
  case ObOBULDRMirror:
    return DrOBULDRMirror;
  case ObOTURDLMirror:
    return DrOTURDLMirror;
  case ObOBURDLMirror:
    return DrOBURDLMirror;
  case ObEOFCTULDRMirror:
  case ObEOFCBURDLMirror:
  case ObEOFCBULDRMirror:
  case ObEOFCTURDLMirror:
    return DrEOFC4CycleMirror;
  case ObEOFCCBULDRMirror:
  case ObEOFCCBURDLMirror:
  case ObEOFCCTULDRMirror:
  case ObEOFCCTURDLMirror:
    return DrEOFCC4CycleMirror;
  case ObEMULDRMirror:
    return DrEMULDRMirror;
  case ObEMURDLMirror:
    return DrEMURDLMirror;
 default:
    exit(1);
  }
}

obj drtoobj(obj drag) { 
  switch (drag) {
  case DrSULDRMirror:
    return ObSULDRMirror;
  case DrSURDLMirror:
    return ObSURDLMirror;
  case DrF2CycleMirror:
    return ObFULDR_URDLMirror;
  case DrF4CycleMirror:
    return ObFULDR_BlkMirror;
  case DrBBox:
    return ObBBox;
  case DrBSink:
    return ObBSink;
  case DrAVertMirror:
    return ObAVertMirror;
  case DrAHorizMirror:
    return ObAHorizMirror;
  case DrA2CycleMirror:
    return ObAFVertMirror;
  case DrRClock:
    return ObRClock;
  case DrRCClock:
    return ObRCClock;
  case DrR2Cycle:
    return ObRFClock;
  case DrOTULDRMirror:
    return ObOTULDRMirror;
  case DrOBULDRMirror:
    return ObOBULDRMirror;
  case DrOTURDLMirror:
    return ObOTURDLMirror;
  case DrOBURDLMirror:
    return ObOBURDLMirror;
  case DrEOFC4CycleMirror:
    return ObEOFCTULDRMirror;
  case DrEOFCC4CycleMirror:
    return ObEOFCCBULDRMirror;
  case DrEMULDRMirror:
    return ObEMULDRMirror;
  case DrEMURDLMirror:
    return ObEMURDLMirror;
  default:
    exit(1);
  }
}

char *classname(obj class) {
  switch (class) {
  case ClSimple:
    return "Simple mirrors";
  case ClFlipper:
    return "Flipping mirrors";
  case ClBoxSink:
    return "Box and sink";
  case ClAxis:
    return "Axial mirrors";
  case ClRotator:
    return "Rotators";
  case ClOneWay:
    return "One-way mirrors";
  case ClEvil:
    return "PURE EVIL";
  default:
    exit(1);
  }
}

obj classleftpic(obj class) {
  switch (class) {
  case ClSimple:
    return ObSULDRMirror;
  case ClFlipper:
    return ObFULDR_URDLMirror;
  case ClBoxSink:
    return ObBBox;
  case ClAxis:
    return ObAVertMirror;
  case ClRotator:
    return ObRClock;
  case ClOneWay:
    return ObOTULDRMirror;
  case ClEvil:
    return ObEOFCTULDRMirror;
  default:
    exit(1);
  }
}

obj classrightpic(obj class) {
  switch (class) {
  case ClSimple:
    return ObSURDLMirror;
  case ClFlipper:
    return ObFBlk_URDLMirror;
  case ClBoxSink:
    return ObBSink;
  case ClAxis:
    return ObAFHorizMirror;
  case ClRotator:
    return ObRFCClock;
  case ClOneWay:
    return ObOBURDLMirror;
  case ClEvil:
    return ObEMULDRMirror;
  default:
    exit(1);
  }
}

char *propname(unsigned int property) {
  switch (property) {
  case 0:
    return "Width";
  case 1:
    return "Height";
  case 2:
    return "Density";
  case 3:
    return "Fuzz";
  default:
    exit(1);
  }
}





/*
obj randobj(obj class) {
  switch (class) {
  case ClSimple:
    return (random() % 2)+ObSULDRMirror;
  case ClFlipper:
    return (random() % 6)+ObFULDR_URDLMirror;
  case ClBoxSink:
    return (random() % 2)+ObBBox;
  case ClAxis:
    return (random() % 4)+ObAVertMirror;
  case ClRotator:
    return (random() % 4)+ObRClock;
  case ClOneWay:
    return (random() % 4)+ObOTULDRMirror;
  case ClEvil:
    return (random() % 8)+ObEOFCTULDRMirror;
  }
  return ObNone;
}
*/  
