/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include "debug.h"

#ifdef _DEBUG

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

