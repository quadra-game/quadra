/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_PCX
#define _HEADER_PCX

#include "res.h"
#include "image.h"

#pragma pack(1)
class Pcx: public Image {
	struct Head {
		Byte manufacturer;
		Byte version;
		Byte encoding;
		Byte bpp;
		Word x1,y1,x2,y2;
		Word hdpi, vdpi;
		Byte colormap[48];
		Byte reserved;
		Byte nplane;
		Word byteperline;
		Word paletteinfo;
		Word screensizeh, screensizev;
		Byte filler[54];
	} h;
	int width_, height_;
	Byte* pic_;
	Byte* pal_;
 public:
	Pcx(Res& res);
	virtual ~Pcx();
	int width() const { return width_; }
	int height() const { return height_; }
	Byte* pic() const { return pic_; }
	Byte* pal() const { return pal_; }
	int palettesize() const { return 256; }
};
#pragma pack()

#endif
