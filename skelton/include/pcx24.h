/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_PCX24
#define _HEADER_PCX24

#include "types.h"
class Res;

#pragma pack(1)
class Pcx24 {
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
 public:
	Pcx24(Res& res);
	virtual ~Pcx24();
	int width() const { return width_; }
	int height() const { return height_; }
	Byte* pic() const { return pic_; }
};
#pragma pack()

#endif
