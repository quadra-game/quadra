/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include "video.h"
#include "video_dumb.h"
#ifdef UGS_LINUX_X11
#include "video_x11.h"
#endif
#ifdef UGS_LINUX_SVGA
#include "video_svga.h"
#endif

Video* video = NULL;

Video_bitmap* Video_bitmap::New(const int px, const int py,
				const int w, const int h, const int rw) {
  Video_bitmap* obj;

#ifdef UGS_LINUX_X11
  if((obj = Video_bitmap_X11::New(px, py, w, h, rw)))
    return obj;
  else
#endif
#ifdef UGS_LINUX_SVGA
    return Svgalib_Video_bitmap::New(px, py, w, h, rw);
#endif
  return NULL;
}

Video_bitmap* Video_bitmap::New(const int px, const int py,
				const int w, const int h) {
  Video_bitmap* obj;

#ifdef UGS_LINUX_X11
  if((obj = Video_bitmap_X11::New(px, py, w, h)))
    return obj;
  else
#endif
#ifdef UGS_LINUX_SVGA
    return Svgalib_Video_bitmap::New(px, py, w, h);
#endif
  return NULL;
}

Video* Video::New(int w, int h, int b, const char *wname, bool dumb) {
	if(dumb)
		return Video_Dumb::New(w, h, b, wname);
  Video* obj;
#ifdef UGS_LINUX_X11
  if((obj = Video_X11::New(w, h, b, wname))) {
    return obj;
  } else
#endif
#ifdef UGS_LINUX_SVGA
      return Svgalib_Video::New(w, h, b, wname);
#endif
  return NULL;
}
