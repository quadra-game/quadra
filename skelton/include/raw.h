/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_RAW
#define _HEADER_RAW

#include "types.h"
#include "res.h"
#include "image.h"

#pragma pack(1)
class Raw: public Image {
	struct Head {
		static char signature[6];
		char sig[6];
		Word version;
		Word width;
		Word height;
		Word palettesize;
		Word HDPI;
		Word VDPI;
		Word gamma;
		char reserved[12];
		void xlat();
	} h;
	Byte* pic_;
	Byte* pal_;
 public:
	Raw(Res& res);
	Raw(int w, int h, int ps);
	virtual ~Raw();
	int width() const { return h.width; }
	int height() const { return h.height; }
	Byte* pic() const { return pic_; }
	Byte* pal() const { return pal_; }
	int palettesize() const { return h.palettesize; }
	void write(Res_dos& r);
};
#pragma pack()

#endif
