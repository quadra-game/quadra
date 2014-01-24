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

#ifndef _HEADER_IMAGE_PNG
#define _HEADER_IMAGE_PNG

#include <stdint.h>
#include <png.h>

#include "res.h"
#include "image.h"

class Png: public Image {
private:
  unsigned long int w;
  unsigned long int h;
  unsigned int palsize;
  uint8_t* pal_;
  uint8_t* pic_;
public:
  Png(Res& res);
  virtual ~Png();
  virtual int width() const {
    return w;
  }
  virtual int height() const {
    return h;
  }
  virtual uint8_t* pic() const {
    return pic_;
  }
  virtual uint8_t* pal() const {
    return reinterpret_cast<uint8_t*>(pal_);
  }
  virtual int palettesize() const {
    return palsize;
  }
};

#endif /* _HEADER_IMAGE_PNG */
