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

#include "notify.h"

#include <algorithm>
#include "types.h"
#include "error.h"

using std::find;
using std::list; 

void Observable::add_watch(Notifyable* n) {
  // FIXME: I'm not 100% sure this is safe. Maybe it should be a set, after
  // all...
  //assert(find(notes.begin(), notes.end(), n) == notes.end());
  notes.push_back(n);
}

void Observable::remove_watch(Notifyable* n) {
  list<Notifyable*>::iterator it = find(notes.begin(), notes.end(), n);

  if (it != notes.end())
    notes.erase(it);
}

void Observable::notify_all() {
  list<Notifyable*>::const_iterator it;
  
  for (it = notes.begin(); it != notes.end(); ++it)
    (*it)->notify();
}

Observable::~Observable() {
	if(!notes.empty())
		skelton_msgbox("Observable %p was destroyed while still watched (%i watchers)\n", this, notes.size());
}
