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

#include <ctype.h>
#include <string.h>
#include "error.h"
#include "command.h"
#include "types.h"

RCSID("$Id$")

Command::Command() {
	st[0] = 0;
}

Command::~Command() {
}

void Command::add(const char *s) {
	char* i;
	i = st + strlen(st);
	while(*s && i<st+sizeof(st)-2) {
		*i++ = *s++;
	}
	*i++ = ' ';
	*i = 0;
}

bool Command::token(const char *s) {
	char tokens[1024];
	strcpy(tokens, s);
	char *token;
	char tok[64];

	token = strtok(tokens, " ");
	while(token != NULL) {
		tok[0] = '-';
		strcpy(&tok[1], token);
		strcat(tok, " "); // force a space at the end of the token

		char *temp;
		temp = strstr(st, tok);
		if(temp) {
			skelton_msgbox("Command::token: Found token [%s]\n",tok);
			last_param = strchr(temp, ' '); // look for the next space following this token
			if(last_param)
				last_param++;
			return true;
		}

		token = strtok(NULL, " ");
	}
	return false;
}

char *Command::get_param() {
	if(!last_param)
		return NULL;
	strncpy(param, last_param, sizeof(param));
	param[sizeof(param)-1]=0;
	char *temp = strstr(param, " -");
	if(!temp)
		temp = strstr(param, "\t-");
	if(!temp)
		temp = strstr(param, " /");
	if(!temp)
		temp = strstr(param, "\t/");
	if(!temp)
		temp = param+strlen(param);
	*temp-- = 0;
	while(temp && temp>=param && (*temp==' ' || *temp=='\t')) //Remove trailing spaces/tabs
		*temp-- = 0;
	if(strlen(param))
		return param;
	else
		return NULL; // if no parameter
}

Command command;
