/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include "zlib.h"
#undef FAR
#include "res_compress.h"

Res_compress::Res_compress(const char *fil, Res_mode pmode, bool res_doze) {
	mode = pmode;
	ressize = 0;
	write_pos = 0;
	if(res_doze)
		res = new Res_doze(fil);
	else {
		res_dos = new Res_dos(fil, mode);
		res = res_dos;
		exist = res_dos->exist;
	}
	if((mode == RES_TRY && exist) || mode == RES_READ) {
		read_uncompress();
	}
}

Res_compress::~Res_compress() {
	if(mode == RES_WRITE || mode == RES_CREATE) {
		if(res)
			write_compress(NULL);
	}
	if(res)
		delete res;
	if(_buf)
		free(_buf);
}

void Res_compress::read_uncompress() {
	exist = false;
	Byte *temp = (Byte *) res->buf(); // lit le fichier en entier dans '_buf'
	ressize = *(Dword *) temp;
	Byte *source = temp + 4;
	int src_size = res->size() - 4;
	skelton_msgbox("Res_compress::Res_compress: Reading compressed file original size = %i, compressed = %i\n", ressize, src_size);
	if(ressize < 0 || src_size < 0) {
		if(mode == RES_TRY)
			return;
		else
			(void) new Error("Unable to uncompress file.\n");
	}
	_buf = (Byte *) malloc(ressize);
	if(_buf == NULL) {
		if(mode == RES_TRY)
			return;
		else
			(void) new Error("Out of memory allocating buffer (%i bytes)\n",ressize);
	}

	unsigned long dest_len = ressize;
	int error = uncompress(_buf, &dest_len, source, src_size);
	if(error != Z_OK) {
		if(mode == RES_TRY)
			return;
		else
			(void) new Error("Unable to uncompress file, error #%i", error);
	}
	if(dest_len != ressize) {
		if(mode == RES_TRY)
			return;
		else
			(void) new Error("Error uncompressing, destination len should be:%i but returned:%i",ressize,dest_len);
	}
	exist = true;
}

void Res_compress::write(const void *b, int nb) {
	if(write_pos + nb > ressize) { 
		ressize = ressize + max(nb, 16384);
		_buf = (Byte *) realloc(_buf, ressize);
		if(_buf == NULL)
			(void) new Error("Unable to reallocate buffer (need %i bytes)\n", ressize);
	}
	memcpy(_buf + write_pos, b, nb);
	write_pos += nb;
}

Dword Res_compress::size() {
	return ressize;
}

Byte *Res_compress::write_compress(Dword *size) {
	if(!res)
		(void)new Error("Trying to write_compress a second time!");
	if(!res_dos)
		(void)new Error("Trying to write_compress a Res_doze!");
	if(!_buf)
		return NULL;
	unsigned long dest_len = write_pos + 65540;
	Byte *temp = (Byte *) malloc(dest_len);
	*((Dword *)temp)=write_pos;
	int error = compress(temp+4, &dest_len, _buf, write_pos);
	if(error != Z_OK) {
		(void) new Error("Unable to compress file, error #%i", error);
	}
	skelton_msgbox("Res_compress::write_compress: writing file, original was %i, packed = %i\n", write_pos, dest_len);
	res_dos->write(temp, dest_len+4);
	delete res_dos;
	res_dos=NULL;
	res=NULL;
	if(size) {
		*size=dest_len+4;
	}
	else {
		free(temp);
		temp=NULL;
	}
	return temp;
}
