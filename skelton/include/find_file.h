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

#ifndef _HEADER_FIND_FILE
#define _HEADER_FIND_FILE

#include <string.h>
#include "types.h"

class Find_file_entry {
public:
  char name[1024];
  bool is_folder;
  Dword size;
  char date[1024];
  Find_file_entry(const char *n, bool f);
};

class Find_file {
public:
  virtual ~Find_file() { }
  static Find_file* New(const char *n);
  static void get_current_directory(char *s);
  virtual bool eof() = 0;
  virtual Find_file_entry get_next_entry() = 0;
};

static const char *mybasename(const char* f);
#endif
