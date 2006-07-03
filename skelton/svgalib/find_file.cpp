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

#include <sys/stat.h>
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <glob.h>

#include "find_file.h"

RCSID("$Id$")

Find_file_entry::Find_file_entry(const char *n, bool f) {
  name[0] = 0;
  name[1023] = 0;
  date[0] = 0;
  date[1023] = 0;
  strncpy(name, n, 1023);
  size = 0;
  is_folder = f;
}

class Find_file_Unix: public Find_file {
private:
  glob_t globbuf;
  int count;
  struct stat sbuf;
public:
  Find_file_Unix(const char *n);
  virtual ~Find_file_Unix();
  virtual bool eof();
  virtual Find_file_entry get_next_entry();
};

Find_file *Find_file::New(const char *n) {
  return new Find_file_Unix(n);
}

void Find_file::get_current_directory(char *s) {
  if(getcwd(s, 1024) == NULL)
    s[0] = 0;
	//Remove slashes from end
	char *w=s+strlen(s);
	while(w>=s) {
		if(*w=='/')
			*w=0;
		else
			break;
		w--;
	}
}

Find_file_Unix::Find_file_Unix(const char *n) {
  count = -1;

  globbuf.gl_offs = 0;

  glob(n, 0, NULL, &globbuf);
}

Find_file_Unix::~Find_file_Unix() {
  globfree(&globbuf);
}

bool Find_file_Unix::eof() {
  return !(globbuf.gl_pathc-count);
}

Find_file_entry Find_file_Unix::get_next_entry() {
  struct stat sbuf;
  char* buf;
  bool isdir;

  if(count == -1)
    buf = "..";
  else
    buf = globbuf.gl_pathv[count];

  count++;

  if(stat(buf, &sbuf) == -1)
    isdir = false;
  else
    isdir = S_ISDIR(sbuf.st_mode);
  
  Find_file_entry f(mybasename(buf), isdir);
  
  f.size = sbuf.st_size;
  strncpy(f.date, ctime(&sbuf.st_mtime), 1023);
  
  return f;
}
