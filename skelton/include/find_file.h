/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_FIND_FILE
#define _HEADER_FIND_FILE

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

#endif
