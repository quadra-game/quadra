/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include "video.h"
#include "input_dumb.h"
#ifdef UGS_LINUX_X11
#include "input_x11.h"
#endif
#ifdef UGS_LINUX_SVGA
#include "input_svga.h"
#endif

Input *input = NULL;

Input* Input::New(bool dumb) {
  if(dumb)
    return new Input_Dumb();
#ifdef UGS_LINUX_X11
  if(video->xwindow)
    return new Input_X11;
  else
#endif
#ifdef UGS_LINUX_SVGA
    return new Input_Svgalib;
#endif
  return NULL;
}
