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

#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#if __GLIBC_MINOR__ == 1
#include <db_185.h>
#else
#include <db.h>
#endif
#include "registry.h"

class RegistryLinux: public Registry {
private:
  DB* db;
public:
  static Registry *alloc();
  RegistryLinux();
  virtual ~RegistryLinux();
  virtual void open(const char *n, const char *dir);
  virtual void write(const char *k, char *v);
  virtual void read(const char *k, char *buffer, unsigned long size);
  virtual void close();
};

Registry *Registry::alloc() {
  return new RegistryLinux();
}

RegistryLinux::RegistryLinux() {
  db = NULL;
}

RegistryLinux::~RegistryLinux() {
  this->close();
}

void RegistryLinux::open(const char *n, const char *dir) {
	char fn[1024];
	sprintf(fn, "%s/registry", dir);
  db = dbopen(fn, O_CREAT|O_RDWR, 0666, DB_HASH, NULL);
}

void RegistryLinux::write(const char *k, char *v) {
  DBT key, data;

  key.data = (void*)k;
  key.size = strlen(k);
  data.data = v;
  data.size = strlen(v);

  if(db)
    db->put(db, &key, &data, 0);
}

void RegistryLinux::read(const char *k, char *buffer, unsigned long size) {
  DBT key, data;
  
  key.data = (void*)k;
  key.size = strlen(k);
  data.data = NULL;
  data.size = 0;
  
  memset(buffer, 0, size);

  if(db)
    db->get(db, &key, &data, 0);
  
  if(size > data.size)
    size = data.size;
  
  strncpy(buffer, (char*)data.data, size);
}

void RegistryLinux::close() {
  if(db)
    db->close(db);

  db = NULL;
}
