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

struct BufPtr {
  BufPtr(const void* buf, uint32_t size)
    : buf_(buf),
      size_(size),
      pos_(0) {
  }

  const void* const buf_;
  const uint32_t size_;
  uint32_t pos_;
};

static void res_read_func(png_structp png,
                          png_bytep data,
                          png_size_t len) {
  BufPtr* bufptr(static_cast<BufPtr*>(png_get_io_ptr(png)));

  if (len > bufptr->size_ - bufptr->pos_) {
    memset(data, 0, len);
    len = bufptr->size_ - bufptr->pos_;
  }

  memcpy(data, static_cast<const char*>(bufptr->buf_) + bufptr->pos_, len);
  bufptr->pos_ += len;
}

Png::Png(const Res& res) : w(0), h(0), palsize(0), pal_(NULL), pic_(NULL) {
  png_structp png;
  png_infop info;
  png_infop end_info;
  png_colorp pngpal;
  unsigned int i;

  png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png)
    fatal_msgbox("Unable to initialize libpng");

  info = png_create_info_struct(png);
  if(!info)
    fatal_msgbox("Unable to create info of libpng");

  end_info = png_create_info_struct(png);
  if(!end_info)
    fatal_msgbox("Unable to create end_info of libpng");

  BufPtr bufptr(res.buf(), res.size());
  png_set_read_fn(png, &bufptr, res_read_func);

  png_read_info(png, info);

  w = png_get_image_width(png, info);
  h = png_get_image_height(png, info);

  if(png_get_bit_depth(png, info) != 8)
    fatal_msgbox("png is not 8 bit depth");

  if(png_get_color_type(png, info) != PNG_COLOR_TYPE_PALETTE)
    fatal_msgbox("png is not paletted");

  if(!png_get_valid(png, info, PNG_INFO_PLTE))
    fatal_msgbox("png palette info chunk missing");

  png_get_PLTE(png, info, &pngpal, reinterpret_cast<int*>(&palsize));

  pal_ = new uint8_t[palsize * 3];
  if(!pal_)
    fatal_msgbox("out of memory");

  for(i = 0; i < palsize; i++)
    reinterpret_cast<png_color*>(pal_)[i] = pngpal[i];

  pic_ = new uint8_t[w * h];

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

