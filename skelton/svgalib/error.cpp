/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "error.h"
#include "video.h"

#ifdef _DEBUG
	int copper=0;
#endif

#ifdef _DEBUG
	bool _debug = true;
#else
	bool _debug = false;
#endif

bool skelton_debug = true;

Error::Error(const char* m, ...) {
	char st[1024];
	va_list marker;
	va_start(marker, m);
	vsprintf(st, m, marker);
	va_end(marker);
	fprintf(stderr, "Error: %s\n", st);
	exit(1);
}

void msgbox(const char* m, ...) {
	if(_debug) {
		char st[1024];
		va_list marker;
		va_start(marker, m);
		vsprintf(st, m, marker);
		va_end(marker);
		fprintf(stderr, st);
	}
}

void skelton_msgbox(const char* m, ...) {
	if(_debug && skelton_debug) {
		char st[1024];
		va_list marker;
		va_start(marker, m);
		vsprintf(st, m, marker);
		va_end(marker);
		fprintf(stderr, st);
	}
}

void lock_msgbox(const char* m, ...) {
	if(_debug) {
		char st[1024];
		va_list marker;
		va_start(marker, m);
		vsprintf(st, m, marker);
		va_end(marker);
		fprintf(stderr, st);
	}
}

void user_output(const char* title, const char *msg) {
	printf("%s\n%s\n",title,msg);
}
