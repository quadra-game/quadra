/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_COMMAND
#define _HEADER_COMMAND

class Command {
	char st[1024];
	char *last_param;
	char param[1024];
public:
	Command();
	virtual ~Command();
	void add(const char *s);
	bool token(const char *s);
	char *get_param();
};

extern Command command;

#endif
