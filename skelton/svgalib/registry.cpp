/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
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
