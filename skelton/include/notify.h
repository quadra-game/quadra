/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_NOTIFY
#define _HEADER_NOTIFY
#include "array.h"

class Notifyable {
public:
	virtual void notify()=0;
};

class Observable {
	Array<Notifyable *> notes;
public:
	virtual ~Observable();
	virtual void add_watch(Notifyable *n);
	virtual void remove_watch(Notifyable *n);
	virtual void notify_all();
};

#endif
