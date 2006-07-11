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

#ifndef _HEADER_ARRAY
#define _HEADER_ARRAY
#include <vector>

#ifdef UGS_DIRECTX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

template <class T>
class Array {
	std::vector<T> v;
public:
	void add(const T& t) {
		v.push_back(t);
	}
	void add_before(const T& t, int i) {
		v.insert(v.begin() + i, t);
	}
	bool remove_item(const T& t) {
		for(int i=0; i<size(); i++)
			if(v[i] == t) {
				remove(i);
				return true;
			}
		return false;
	}
	void removelast() {
		v.pop_back();
	}
	void remove(int i) {
		v.erase(v.begin() + i);
	}
	int size() const {
		return v.size();
	}
	const T& operator[](int i) const {
		return v[i];
	}
	T& operator[](int i) {
		return v[i];
	}
	void replace(const int i, const T& t) {
		v[i] = t;
	}
	void clear() {
		v.clear();
	}
	void deleteall() {
		while(size()) {
			delete last();
			removelast();
		}
	}
	T& last() {
		return v[size()-1];
	}
};

#endif
