/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include "video.h"
#include "cursor_self.h"
#ifdef UGS_LINUX_X11
#include "cursor_x11.h"
#endif
#include "cursor.h"

Cursor* cursor = NULL;

Cursor* Cursor::New(Sprite* s) {
#ifdef UGS_LINUX_X11
	if(video->xwindow)
		return new Cursor_X11();
#endif
	return new Cursor_Self(s);
}
