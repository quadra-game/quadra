/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_ARRAY
#define _HEADER_ARRAY
#include <vector>

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
		v.insert(&v[i], t);
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
		v.erase(&v[i]);
	}
	int size() const {
		return v.size();
	}
	const T& operator[](int i) const {
		return v[i];
	}
	void replace(const int i, const T& t) {
		v[i] = t;
	}
	void clear() {
		v.clear();
	}
	void sort() {
		bool done;
		do {
			done=true;
			int i;
			for(i=0; i<size()-1; i++) {
				if(*v[i]<*v[i+1]) {
					T tmp=v[i];
					v[i]=v[i+1];
					v[i+1]=tmp;
					done=false;
					break;
				}
			}
		} while(!done);
	}
	void deleteall() {
		while(size()) {

			delete last();
			removelast();
		}
	}
	const T& last() const {
		return v[size()-1];
	}
};

#endif
