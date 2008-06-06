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

/*
 * Here's a post I made to the RZ forum (www.recognize.nu) about
 * making Quadra themes.  Please ignore the part with the
 * ranting. Better yet: I'll cut it out for you :).
 * 
 * This is Dada, one of the original Quadra programmers. I saw some
 * interrest in making Quadra themes so I'd like to clarify some stuff
 * and answer questions. First of all, I'd like to apologize for not
 * providing this information earlier. Somehow I thought people would
 * like to figure things out for themselves by digging thru 25000
 * lines of bad C++, but I can see how that's not for everybody (there
 * was even somebody who had bytes and bits laughing at them? Really
 * sorry about that :)). Se here's a little summary:
 *
 * First of all, most (if not all) of what AngelGurl and logix have
 * figured out is right: the best way to make res files is with the
 * wadder.exe tool that can be built from any distribution of the
 * Quadra source code (the file format has never changed). I know it
 * would have been better if it had been a simple zip file, but the
 * wadder thing is not too hard to use either.
 *
 * One important capability (as far as themes go) that hasn't been
 * discovered yet is the possibility to make a patching res file. When
 * Quadra starts up, it reads in everything it finds from quadra.res,
 * but then goes on to read another quadraxyz.res file (where 'xyz' is
 * the Quadra version number of quadra.exe; for example, Quadra
 * version 1.1.5 tries to open quadra115.res), replacing like-named
 * resources that were found in the big quadra.res file with the ones
 * from the patching res. This patching res is the exact same format
 * as quadra.res so it can be built using wadder.exe too, but of
 * course it can be much smaller because it has to contain only the
 * resources you want to change. Besides, the patching resources
 * themselves don't have to be the same file size as the original
 * resources in quadra.res, which was needed when trying to patch the
 * quadra.res file itself with a hex editor (needless to say, I don't
 * recommend that approach; a patching res file is the best way to
 * make a theme pack).
 * 
 * PS: The current version of Quadra only ever loads the main res file
 * (quadra.res) and a single patching res file (quadra115.res or
 * whatever the exe version number is). The reason is that the
 * patching feature was made to do just that: patches. We didn't think
 * about themes back then. However, I've just decided right this
 * moment that the next version will have a -patch command line option
 * allowing people to specify additional patching res file(s) to load
 * (while still loading the version-specific file if found).  This
 * should make it much easier for most people to use a theme pack, if
 * somebody actually manage to produce one :).
 * 
 * Ok, that was the good news. Now on to the bad news: the resources
 * themselves.
 * 
 * As some of you have found out, Quadra is *really* picky about the
 * palettes found in it's many image resources. In practice, you have
 * to use a paint program (or image converter) that gives you direct
 * control over the 256-colors palette found in the png files. You
 * can't just scan in a background in Photoshop and save it as a 8-bit
 * paletted png, it won't look right at all (24-bit images are also
 * out of the question). Most of the palette is available to use for
 * the background images, but a good portion of it is used to draw the
 * blocks and the text in the chat window. You can customize these
 * colors too (and use them in the background image), but they have to
 * "make sense" to Quadra for you to get a correct display. Given the
 * proper image-editing tools with good palette control, you should be
 * able to make new backgrounds with customized block colors by
 * imitating the organization of the palettes in the default pngs that
 * come with the source code package (the simple and irrefutable proof
 * of that is that we could do it ourselves in the first place :)).
 *
 * One other piece of bad news: a lot of sound effects are re-used in
 * more than one scheme in various ways, so it is not possible to
 * fully customize all the sound effects for level 1 (for example)
 * without affecting the sound effects that will play on other
 * levels. Since most people only play multiplayer (where only one
 * scheme is visible) this isn't too much of an issue but I thought
 * I'd mention it anyway.
 *
 * Last bit of bad news: if you haven't figured it out from the
 * palette discussion above, it is not possible to customize the
 * appearance of blocks beyond their colors (the inside "fill" color
 * and the little shading/beveling/sunking perimeter lines). Quadra
 * itself does nearly everything that is possible to do over its 10
 * levels as far as block customization is concerned. I know it is
 * limited, but that's how it is...
 *
 * Anyway, I hope that can help somebody. I'd like to see some Quadra
 * themes myself, if only for the sake of change :).
 */

#include <stdio.h>
#include "stringtable.h"
#include "res.h"
#include "find_file.h"

RCSID("$Id$")

const char usage[] = "usage: wadder <working directory> <output res> <input text>\n";
Resfile *wad;

void addfile(const char* fname) {
	Res_dos *res;
	char *data;

	printf("%s: ", fname);
	res = new Res_dos(fname, RES_TRY);
	data = new char[res->size()];
	res->read(data, res->size());

	wad->add(mybasename(fname), res->size(), data);

	delete res;
	delete data;

	printf("done\n");
}

int main(int ARGC, char **ARGV, char **ENV) {
	Res_dos *res;
	Byte* data;

	if(ARGC < 4) {
		fprintf(stderr, "%s: %s", ARGV[0], usage);
		exit(1);
	}

	char wad_file[256];
	sprintf(wad_file, "%s%s", ARGV[1], ARGV[2]);

	wad = new Resfile(wad_file, false);

	wad->clear();

	char res_file[256];
	sprintf(res_file, "%s%s", ARGV[1], ARGV[3]);

	res = new Res_dos(res_file, RES_READ);
	data = new Byte[res->size()+1];

	memcpy(data, res->buf(), res->size());

	Stringtable st(data, res->size());

	for(int i=0; i<st.size(); i++)
	{
		char temp[256];
		sprintf(temp, "%s%s", ARGV[1], st.get(i));
		addfile(temp);
	}

	delete data;
	delete res;

	wad->freeze();

	delete wad;

	return 0;
}

void start_game(void) {
}
