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
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <io.h>
#include "main.h"
#include "find_file.h"

Find_file_entry::Find_file_entry(const char *n, bool f) {
  strncpy(name, n, 1023);
	name[1023] = 0;
	is_folder = f;
	date[0] = 0;
	size = 0;
}

class Find_file_directx: public Find_file {
	struct _finddata_t c_file;
	long hFile;
	bool terminated;
	bool error_status;
public:
	Find_file_directx(const char *n);
	virtual ~Find_file_directx();
	virtual bool eof();
	virtual bool has_error();
	virtual Find_file_entry get_next_entry();
};

Find_file *Find_file::New(const char *n) {
	return new Find_file_directx(n);
}

void Find_file::get_current_directory(char *s) {
	char *w;
	if(GetCurrentDirectory(1024, s) == 0) {
		s[0] = 0;
	} else {
		do {
			w = strchr(s, '\\');
			if(w)
				*w = '/';
			else
				break;
		} while(1);
	}
	//Remove slashes from end
	w=s+strlen(s);
	while(w>=s) {
		if(*w=='/')
			*w=0;
		else
			break;
		w--;
	}
}

Find_file_directx::Find_file_directx(const char *n) {
	terminated = false;
	error_status = false;
	if((hFile = _findfirst(n, &c_file)) == -1L) {
		terminated = true;
		if(errno != ENOENT)
		{
			error_status = true;
		}
	}
}

Find_file_directx::~Find_file_directx() {
	_findclose(hFile);
}

bool Find_file_directx::has_error()
{
	return error_status;
}

bool Find_file_directx::eof() {
	return terminated;
}

Find_file_entry Find_file_directx::get_next_entry() {
	bool folder = (c_file.attrib & _A_SUBDIR) ? true:false;
	Find_file_entry f(c_file.name, folder);
	f.size = c_file.size;
	strcpy(f.date, ctime(&(c_file.time_write)));

	if(_findnext(hFile, &c_file) != 0)
		terminated = true;
	return f;
}
