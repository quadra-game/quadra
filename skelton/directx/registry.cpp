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

#include <stdio.h>
#include "error.h"
#include "main.h"
#include <winreg.h>
#include "registry.h"

class RegistryWin32: public Registry {
	char compagny[1024];
public:
	virtual void open(const char *n, const char *dir);
	virtual void write(const char *k, char *v);
	virtual void read(const char *k, char *buffer, unsigned long size);
	virtual void close();
};


Registry *Registry::alloc() {
	return new RegistryWin32();
}

void RegistryWin32::open(const char *n, const char *dir) {
	sprintf(compagny, "Software\\Ludus Design\\%s", n);
}

void RegistryWin32::write(const char *k, char *v) {
	HKEY hkey;
	DWORD disposition;
	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, compagny, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkey, &disposition) == ERROR_SUCCESS) {
		RegSetValueEx(hkey, k, 0, REG_SZ, (Byte *) v, strlen(v));
		RegCloseKey(hkey);
	} else {
		skelton_msgbox("RegistryWin32::write: failed to create key in [%s]/[%s] with value [%s].\n", compagny, k, v);
	}
}

void RegistryWin32::read(const char *k, char *buffer, unsigned long size) {
	HKEY hkey;
	skelton_msgbox("RegistryWin32::read: key [%s]/[%s] ", compagny, k);
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, compagny, 0, KEY_ALL_ACCESS, &hkey) == ERROR_SUCCESS) {
		skelton_msgbox("opened ");
		unsigned long type;
		if(RegQueryValueEx(hkey, k, 0, &type, (Byte *) buffer, &size) == ERROR_SUCCESS) {
			skelton_msgbox("Query: value is [%s]\n",buffer);
		} else {
			skelton_msgbox("failed to query value\n");
		}
		RegCloseKey(hkey);
	} else {
		skelton_msgbox("failed to open\n");
	}
}

void RegistryWin32::close() {
}
