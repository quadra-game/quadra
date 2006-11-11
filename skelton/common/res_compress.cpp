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

#include "res_compress.h"

#include "zlib.h"
#undef FAR
#include "SDL_endian.h"

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
			write_compress();
	}
	if(res)
		delete res;
	if(_buf)
		free(_buf);
}

void Res_compress::read_uncompress() {
	exist = false;
	Byte *temp = (Byte *) res->buf(); // reads the entire file in '_buf'
	ressize = SDL_SwapLE32(*(Dword *) temp);
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

void Res_compress::write_compress() {
	if(!res)
		(void)new Error("Trying to write_compress a second time!");
	if(!res_dos)
		(void)new Error("Trying to write_compress a Res_doze!");
	if(!_buf)
		return;
	unsigned long dest_len = write_pos + 65540;
	Byte *temp = (Byte *) malloc(dest_len);
	*((Dword *)temp)=SDL_SwapLE32(write_pos);
	int error = compress(temp+4, &dest_len, _buf, write_pos);
	if(error != Z_OK) {
		(void) new Error("Unable to compress file, error #%i", error);
	}
	skelton_msgbox("Res_compress::write_compress: writing file, original was %i, packed = %i\n", write_pos, dest_len);
	res_dos->write(temp, dest_len+4);
	delete res_dos;
	res_dos=NULL;
	res=NULL;
	free(temp);
}
