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

#ifndef _HEADER_UTILS
#define _HEADER_UTILS
#include "types.h"
#include "error.h"

inline void mset(void* p, Byte c, Dword l) {
	for(Dword i(0); i<l; i++)
		((Byte*)p)[i]=c;
}

inline void cpy(void* d, void* s, Dword l) {
	for(Dword i(0); i<l; i++)
		((Byte*)d)[i]=((Byte*)s)[i];
}

/*template <class T>
inline T abs(const T n) {
	if(n<T(0))
		return -n;
	else
		return n;
}*/

template <class T>
inline void xchg(T& a, T& b) {
	T tmp=a;
	a=b;
	b=tmp;
}

class Vect {
 public:
	float x, y;
	Vect(float x_, float y_): x(x_), y(y_) {}
	virtual ~Vect() {}
	float length2() const {
		return x*x+y*y;
	}
	Vect operator+=(const Vect v) {
		x+=v.x;
		y+=v.y;
		return *this;
	}
	Vect operator-=(const Vect v) {
		x-=v.x;
		y-=v.y;
		return *this;
	}
	Vect operator+(const Vect v) const {
		return Vect(x+v.x, y+v.y);
	}
	Vect operator-(const Vect v) const {
		return Vect(x-v.x, y-v.y);
	}
	Vect operator-() const {
		return Vect(-x, -y);
	}
	Vect operator*(const float f) const {
		return Vect(x*f, y*f);
	}
	Vect operator/(const float f) const {
		return Vect(x/f, y/f);
	}
	// dot-product
	float operator*(const Vect v) const {
		return x*v.x+y*v.y;
	}
	// cross-product
	float crossprod(const Vect v) const {
		return x*v.y-y*v.x;
	}
	Vect project(const Vect v) const {
		float tmp=v*v;
		if(tmp)
			return v*((*this)*v/tmp);
		else
			return Vect(0.0f, 0.0f);
	}
	Vect transpose() const {
		return Vect(y, x);
	}
};

#endif
