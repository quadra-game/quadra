/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include <string.h>
#include "unicode.h"

Unicode::Unicode(char *s): the_string(strlen(s)*2) {
	Dword i;
	for(i=0; i<strlen(s); i++) {
		the_string[i*2] = s[i];
		the_string[i*2+1] = 0;
	}
}

Unicode &Unicode::cat(char *s) {
	Byte a_zero=0;
	Dword i;
	for(i=0; i<strlen(s); i++) {
		the_string.append((Byte *)&s[i], 1);
		the_string.append(&a_zero, 1);
	}
	return *this;
}

Unicode::operator Byte *() {
	return the_string.get();
}

Dword Unicode::size() {
	return the_string.size();
}
