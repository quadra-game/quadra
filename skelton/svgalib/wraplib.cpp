/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * 
 * Quadra, an action puzzle game
 * Copyright (C) 1998-2000  Ludus Design
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "wraplib.h"
#include "error.h"

RCSID("$Id$")

static void* drv;
static Svgalib* svgalib;

Svgalib* getSvgalib() {
  Svgalib* (*func)();
  char* dir;

  if(svgalib)
    return svgalib;

  if(!drv) {
    char fn[1024];

    dir = getenv("QUADRADIR");
    if(!dir)
      dir = "/usr/lib/games";

    snprintf(fn, sizeof(fn) - 1, "%s/quadra-svga.so", dir);

    drv = dlopen(fn, RTLD_NOW);

    if(!drv) {
      skelton_msgbox(dlerror());
      skelton_msgbox("\n");
      return NULL;
    }

    skelton_msgbox("quadra-svga.so loaded successfully\n");
  }

  func = (Svgalib*(*)())dlsym(drv, "createSvgalib");

  if(!func) {
    skelton_msgbox("quadra-svga.so is not an Svgalib driver!\n");
    return NULL;
  }

  svgalib = func();

  return svgalib;
}

