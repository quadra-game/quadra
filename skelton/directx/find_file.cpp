/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

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
public:
	Find_file_directx(const char *n);
	virtual ~Find_file_directx();
	virtual bool eof();
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
	if((hFile = _findfirst(n, &c_file)) == -1L) {
		terminated = true;
	}
}

Find_file_directx::~Find_file_directx() {
	_findclose(hFile);
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
