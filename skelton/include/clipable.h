/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_CLIPABLE
#define _HEADER_CLIPABLE

class Bitmap;

class Clipable {
public:
  int width, height;
  /* return values variables */
  static int clip_x1, clip_x2, clip_y1, clip_y2, clip_w;
  Clipable(int w, int h) {
    width = w;
    height = h;
  }
  /* 'true' si out of range */
  bool clip(const int x, const int y, const int w, const int h) const;
  bool clip(const int x, const int y, const Bitmap &b) const;
  bool clip(const int x, const int y, const Bitmap *b) const;
};

#endif /* _HEADER_CLIPABLE */
