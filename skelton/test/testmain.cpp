/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include <stdlib.h>
#include "input.h"
#include "video.h"
#include "cursor.h"
#include "res.h"
#include "pcx.h"

/* Ceci est un test */


void start_game() {
	Res_dos foo("testpic.pcx", RES_READ);
	Pcx foopic(foo);
	Bitmap bit(foopic);
	int i = 0;
	Sprite *cur;
	{
		Res_dos res("cursor.raw");
		Raw raw(res);
		Bitmap bit(raw);
		cur = new Sprite(bit);
	}

	video=new Video(640, 480, 8, "Test pour Linux");
	input=new Input();
	cursor=new Cursor(cur);
	// cursor->restore_back(false);
	Palette pal(foopic);
	video->setpal(pal);
	Video_bitmap v(40, 40, 100, 100);

	video->start_frame();
	video->vb->put_bitmap(bit, 0, 0);
	video->end_frame();

	video->start_frame();
	video->vb->put_bitmap(bit, 0, 0);
	video->end_frame();

	while(input->quel_key == -1) {
		input->check();
		video->start_frame();
		v.setmem();
		// video->vb->rect(0,0,640,480,0);
		video->vb->hline(38, 38, 104, 100);
		video->vb->hline(141, 38, 104, 100);
		video->vb->vline(38, 38, 104, 100);
		video->vb->vline(141, 38, 104, 100);
		v.hline(0, 0, 100, 100);
		v.hline(99, 0, 100, 100);
		v.vline(0, 0, 100, 100);
		v.vline(99, 0, 100, 100);
		// v.rect(0,0,40,40,1);
		i++;
		if(i == 600)
			i = 0;
		video->end_frame();
	}

	delete cur;
}

