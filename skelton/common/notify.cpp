/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include "error.h"
#include "notify.h"

void Observable::add_watch(Notifyable *n) {
	notes.add(n);
}

void Observable::remove_watch(Notifyable *n) {
	notes.remove_item(n);
}

void Observable::notify_all() {
	for(int i=0; i<notes.size(); i++) {
		notes[i]->notify();
	}
}

Observable::~Observable() {
	if(notes.size())
		skelton_msgbox("Observable %p was destroyed while still watched (%i watchers)\n", this, notes.size());
}
