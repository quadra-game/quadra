/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include <sys/stat.h>
#include "res.h"

Res_mem::Res_mem() {
	pos = 0;
	_buf = NULL;
}

Res_dos::Res_dos(const char *fil, Res_mode mode) {
	int flag;
	Error* error;
	_buf = NULL;
	exist = 1;
	flag = 0;
	switch(mode) {
		case RES_READ:
		case RES_TRY:
		  flag = O_RDONLY;
		  break;
		case RES_WRITE:
		  flag = O_RDWR;
		  break;
		case RES_CREATE:
		  flag = O_CREAT|O_TRUNC|O_RDWR;
		  break;
	}
	handle = open(fil, flag, 0666);
	if(handle == -1)
		if(mode == RES_TRY || mode == RES_CREATE)
			exist = 0;
		else
			error = new Error("Unable to open file '%s'", fil);
}

Dword Res_dos::size() {
  struct stat buf;
  fstat(handle, &buf);
  return buf.st_size;
}

Res_dos::~Res_dos() {
	if(handle != -1)
		close(handle);
	if(_buf)
		delete _buf;
}

void Res_dos::position(Dword po) {
	lseek(handle, po, SEEK_SET);
}

int Res_dos::read(void *b, int nb) {
	Error* error;
	int n = ::read(handle, b, nb);
	if(n < 0)
		error = new Error("Error reading file");
	return n;
}

void Res_dos::write(const void *b, int nb) {
	Error* error;
	if(::write(handle, b, nb) != nb)
		error = new Error("Error writing file");
}

const void* Res_dos::buf() {
	if(_buf)
		return _buf;
	_buf = new Byte[size()];
	if(_buf == NULL)
		(void)new Error("Not enough memory");
	read(_buf, size());
	return _buf;
}

bool Res_dos::eof() {
	return (get_position() >= size()) ? true:false;
}

Dword Res_dos::get_position() {
	return lseek(handle, 0, SEEK_CUR);
}
