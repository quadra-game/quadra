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

#include "image_png.h"

static void res_read_func(png_structp png,
                          png_bytep data,
                          png_size_t len) {
  Res* res = static_cast<Res*>(png_get_io_ptr(png));

  res->read(data, len);
}

Png::Png(Res& res) : w(0), h(0), palsize(0), pal_(NULL), pic_(NULL) {
  png_structp png;
  png_infop info;
  png_infop end_info;
  png_colorp pngpal;
  unsigned int i;

  png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png)
    (void)new Error("Unable to initialize libpng");

  info = png_create_info_struct(png);
  if(!info)
    (void)new Error("Unable to create info of libpng");

  end_info = png_create_info_struct(png);
  if(!end_info)
    (void)new Error("Unable to create end_info of libpng");

  png_set_read_fn(png, &res, res_read_func);

  png_read_info(png, info);

  w = png_get_image_width(png, info);
  h = png_get_image_height(png, info);

  if(png_get_bit_depth(png, info) != 8)
    (void)new Error("png is not 8 bit depth");

  if(png_get_color_type(png, info) != PNG_COLOR_TYPE_PALETTE)
    (void)new Error("png is not paletted");

  if(!png_get_valid(png, info, PNG_INFO_PLTE))
    (void)new Error("png palette info chunk missing");

  png_get_PLTE(png, info, &pngpal, reinterpret_cast<int*>(&palsize));

  pal_ = new Byte[palsize * 3];
  if(!pal_)
    (void)new Error("out of memory");

  for(i = 0; i < palsize; i++)
    reinterpret_cast<png_color*>(pal_)[i] = pngpal[i];

  pic_ = new Byte[w * h];

  png_bytepp row_pointers = new png_bytep[h];

  for(i = 0; i < h; i++)
    row_pointers[i] = pic_ + (i * w);

  png_read_image(png, row_pointers);

  delete[] row_pointers;

  png_destroy_read_struct(&png, &info, &end_info);
}

Png::~Png() {
  if(pal_)
    delete[] pal_;

  if(pic_)
    delete[] pic_;
}

