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
#include "byteorder.h"

using std::max;

Res_compress::Res_compress(const char *fil, Res_mode pmode, bool res_doze) {
	mode = pmode;
	write_pos = 0;
	res_dos = new Res_dos(fil, mode);
	res = res_dos;
	exist = res_dos->exist;
  if((mode == RES_TRY && exist) || mode == RES_READ) {
		read_uncompress();
	}
}

Res_compress::Res_compress(const ResName& fil, Res_mode pmode) {
	mode = pmode;
	write_pos = 0;
	res = new Res_doze(fil);
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
	uint8_t *temp = (uint8_t *) res->buf(); // reads the entire file in '_buf'
	ressize = INTELDWORD(*(uint32_t *) temp);
	uint8_t *source = temp + 4;
	int src_size = res->size() - 4;
	skelton_msgbox("Res_compress::Res_compress: Reading compressed file original size = %i, compressed = %i\n", ressize, src_size);
	if(src_size < 0) {
		if(mode == RES_TRY)
			return;
		else
			fatal_msgbox("Unable to uncompress file.\n");
	}
	_buf = (uint8_t *) malloc(ressize);
	if(_buf == NULL) {
		if(mode == RES_TRY)
			return;
		else
			fatal_msgbox("Out of memory allocating buffer (%i bytes)\n",ressize);
	}

	unsigned long dest_len = ressize;
	int error = uncompress(_buf, &dest_len, source, src_size);
	if(error != Z_OK) {
		if(mode == RES_TRY)
			return;
		else
			fatal_msgbox("Unable to uncompress file, error #%i", error);
	}
	if(dest_len != ressize) {
		if(mode == RES_TRY)
			return;
		else
			fatal_msgbox("Error uncompressing, destination len should be:%i but returned:%i",ressize,dest_len);
	}
	exist = true;
}

void Res_compress::write(const void *b, int nb) {
	if(write_pos + nb > ressize) { 
		ressize = ressize + max(nb, 16384);
		_buf = (uint8_t *) realloc(_buf, ressize);
		if(_buf == NULL)
			fatal_msgbox("Unable to reallocate buffer (need %i bytes)\n", ressize);
	}
	memcpy(_buf + write_pos, b, nb);
	write_pos += nb;
}

void Res_compress::write_compress() {
	if(!res)
		fatal_msgbox("Trying to write_compress a second time!");
	if(!res_dos)
		fatal_msgbox("Trying to write_compress a Res_doze!");
	if(!_buf)
		return;
	unsigned long dest_len = write_pos + 65540;
	uint8_t *temp = (uint8_t *) malloc(dest_len);
	*((uint32_t *)temp)=INTELDWORD(write_pos);
	int error = compress(temp+4, &dest_len, _buf, write_pos);
	if(error != Z_OK) {
		fatal_msgbox("Unable to compress file, error #%i", error);
	}
	skelton_msgbox("Res_compress::write_compress: writing file, original was %i, packed = %i\n", write_pos, dest_len);
	res_dos->write(temp, dest_len+4);
	delete res_dos;
	res_dos=NULL;
	res=NULL;
	free(temp);
}
