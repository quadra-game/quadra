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

#include "debug.h"

#ifndef NDEBUG

AllocNode* Debug::allocs = NULL;

bool Debug::initialized = false;

void Debug::dumpAllocs() {
  AllocNode* ptr;
  AllocNode* next;

  if(allocs) {
    skelton_msgbox("*** MEMORY LEAKS ***\n");

    ptr = allocs;

    while(ptr) {
      next = ptr->next;

      if(ptr->funcName)
	skelton_msgbox("%s:%i: %s (%i bytes) in \"%s\"\n",
		       ptr->fileName, ptr->lineNo, ptr->className, ptr->size, ptr->funcName);
      else
	skelton_msgbox("%s:%i: %s (%i bytes)\n",
		       ptr->fileName, ptr->lineNo, ptr->className, ptr->size);

      free(ptr->fileName);
      free(ptr->funcName);
      free(ptr->className);

      delete ptr;
      ptr = next;
    }
  }
}

#endif

