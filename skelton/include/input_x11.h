/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_INPUT_X11
#define _HEADER_INPUT_X11

/* forward declaration un peu laide */
union _XEvent;
typedef union _XEvent XEvent;
typedef struct _XIM *XIM;
typedef struct _XIC *XIC;

#include "input.h"

class Input_X11: public Input {
private:
  bool israw;
  XIM im;
  XIC ic;
  void process_key(XEvent event);
  void process_mouse(XEvent event);
public:
  Input_X11();
  virtual ~Input_X11();
  virtual void clear_key();
  virtual void check();
  virtual void deraw();
  virtual void reraw();
};

#endif /* _HEADER_INPUT_X11 */
