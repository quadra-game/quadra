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

#ifdef ONVEUTDESRESDOZEPOCHES
Res_doze::Res_doze(LPCTSTR lpName) {
	HGLOBAL hResData;
	if((hResInfo = FindResource(NULL, lpName, "CUSTOM")) == NULL)
		new Error("Can't find resource '%s'", lpName);
	if((hResData = LoadResource(NULL, hResInfo)) == NULL)
		new Error("Can't load resource '%s'", lpName);
	if((_buf = (Byte *) LockResource(hResData)) == NULL)
		new Error("Can't lock resource '%s'", lpName);
}
#endif

Res_dos::Res_dos(const char *fil, Res_mode mode) {
	int flag;
	_buf = NULL;
	exist = true;
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
	handle = open(fil, O_BINARY | flag, S_IREAD | S_IWRITE);
	if(handle == -1) {
		if(mode == RES_TRY || mode == RES_CREATE)
			exist = false;
		else
			new Error("Unable to open dos file '%s'", fil);
	}
}

Res_dos::~Res_dos() {
	if(handle != -1)
		close(handle);
	if(_buf)
		delete _buf;
}

Dword Res_dos::size() {
	return _filelength(handle);
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
	Error* error;
	_buf = new Byte[size()];
	if(_buf == NULL)
		error = new Error("Not enough memory to load file");
	read(_buf, size());
	return _buf;
}

bool Res_dos::eof() {
	return _eof(handle) ? true:false;
}

Dword Res_dos::get_position() {
	return _tell(handle);
}
