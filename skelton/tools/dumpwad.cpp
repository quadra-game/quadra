/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include <sys/mman.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "resfile.h"

char *usage = "incorrect number of parameters\n";

int main(int ARGC, char **ARGV, char **ENV) {
	Resfile *wad;

	if(ARGC != 2) {
		fprintf(stderr, "%s: %s", ARGV[0], usage);
		exit(1);
	}

	wad = new Resfile(ARGV[1]);

	delete wad;

	return 0;
}
