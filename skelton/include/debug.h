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

#ifndef __HEADER_DEBUG_H
#define __HEADER_DEBUG_H

#ifndef NDEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

struct AllocNode {
  void* allocation;
  char* fileName;
  unsigned int lineNo;
  char* funcName;
  char* className;
  unsigned int size;
  AllocNode* next;
};

class Debug {
private:
  static AllocNode* allocs;
  static bool initialized;
public:
  static void init();
  static bool verify(bool cond,
                     const char* fileName,
                     unsigned int lineNo,
                     const char* funcName,
                     const char* condString);
  static void assert(bool cond,
                     const char* fileName,
                     unsigned int lineNo,
                     const char* funcName,
                     const char* condString);
  static void logMessage(const char* fileName,
                         unsigned int lineNo,
                         const char* funcName,
                         const char* message);
  static void* logAlloc(void* aObject,
                        const char* aClass,
                        unsigned int aSize,
                        const char* fileName,
                        unsigned int lineNo,
                        const char* funcName);
  static void logFree(void* aObject,
                      const char* objectName,
                      const char* fileName,
                      unsigned int lineNo,
                      const char* funcName);
  static void dumpAllocs();
};

inline void Debug::init() {
  if(!initialized) {
    atexit(Debug::dumpAllocs);
    initialized = true;
  }
}

inline bool Debug::verify(bool cond,
                          const char* fileName,
                          unsigned int lineNo,
                          const char* funcName,
                          const char* condString) {
  if(!cond)
    if(funcName)
      skelton_msgbox("%s:%i: failed verify \"%s\" in \"%s\"\n",
                     fileName, lineNo, condString, funcName);
    else
      skelton_msgbox("%s:%i: failed verify: %s\n",
                     fileName, lineNo, condString);
  
  return cond;
}

inline void Debug::assert(bool cond,
                          const char* fileName,
                          unsigned int lineNo,
                          const char* funcName,
                          const char* condString) {
  if(!cond) {
    if(funcName)
      skelton_msgbox("%s:%i: failed assert \"%s\" in \"%s\"\n",
                     fileName, lineNo, condString, funcName);
    else
      skelton_msgbox("%s:%i: failed assert: %s\n",
                     fileName, lineNo, condString);

    abort();
  }
}

inline void Debug::logMessage(const char* fileName,
                              unsigned int lineNo,
                              const char* funcName,
                              const char* message) {
  if(funcName)
    skelton_msgbox("%s:%i: %s in \"%s\"\n",
                   fileName, lineNo, message, funcName);
  else
    skelton_msgbox("%s:%i: %s\n",
                   fileName, lineNo, message);
}

inline void* Debug::logAlloc(void* aObject,
                             const char* aClass,
                             unsigned int aSize,
                             const char* fileName,
                             unsigned int lineNo,
                             const char* funcName) {
  AllocNode* tmp;

  tmp = new AllocNode;
  tmp->allocation = aObject;
  tmp->fileName = strdup(fileName);
  tmp->lineNo = lineNo;
  if(funcName)
    tmp->funcName = strdup(funcName);
  else
    tmp->funcName = NULL;
  tmp->className = strdup(aClass);
  tmp->size = aSize;
  tmp->next = allocs;
  allocs = tmp;

  init();

  return aObject;
}

inline void Debug::logFree(void* aObject,
                           const char* objectName,
                           const char* fileName,
                           unsigned int lineNo,
                           const char* funcName) {
  bool found;
  AllocNode* ptr;
  AllocNode** prev;

  found = false;

  ptr = allocs;
  prev = &allocs;

  while(ptr) {
    if(ptr->allocation == aObject) {
      found = true;
      *prev = ptr->next;
      delete ptr;
      break;
    }
    prev = &ptr->next;
    ptr = ptr->next;
  }

  if(!found)
    if(funcName)
      skelton_msgbox("%s:%i: DELETE() of unlogged object \"%s\" (%p) in \"%s\"\n",
                     fileName, lineNo, objectName, aObject, funcName);
    else
      skelton_msgbox("%s:%i: DELETE() of unlogged object \"%s\" (%p)\n",
                     fileName, lineNo, objectName, aObject);
}

#if (!defined __GNUC__ || __GNUC__ < 2 || __GNUC_MINOR__ < (defined __cplusplus ? 6 : 4))
#define __FUNCNAME__ ((const char*)0)
#else
#define __FUNCNAME__ __PRETTY_FUNCTION__
#endif

#define ASSERT(cond) \
Debug::assert(cond, __FILE__, __LINE__, __FUNCNAME__, #cond)

#define VERIFY(cond) \
Debug::verify((cond), __FILE__, __LINE__, __FUNCNAME__, #cond)

#define LOGMESSAGE(msg) \
Debug::logMessage(__FILE__, __LINE__, __FUNCNAME__, msg)

#define NEW(tclass, arg) \
(tclass*)Debug::logAlloc(new tclass arg, #tclass, sizeof(tclass), __FILE__, __LINE__, __FUNCNAME__)

#define DELETE(object) \
(Debug::logFree(object, #object, __FILE__, __LINE__, __FUNCNAME__), delete object)

#define INIT_DEBUG \
Debug::init()

#else /* NDEBUG */

#define ASSERT(cond) ((void)0)
#define VERIFY(cond) (cond)
#define LOGMESSAGE(msg) ((void)0)
#define NEW(tclass, arg) new tclass arg
#define DELETE(object) delete object
#define INIT_DEBUG ((void)0)

#endif /* !NDEBUG */

#endif /* !__HEADER_DEBUG_H */
