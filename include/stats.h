/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_STATS
#define _HEADER_STATS

#include "types.h"

class Stat {
	int value;
public:
	Stat() {
		value=0;
	}
	void add(int n) {
		value+=n;
	}
	int get_value() const {
		return value;
	}
	int *get_address() {
		return &value;
	}
	void set_value(const int i) {
		value = i;
	}
	bool operator<(const Stat& o) const {
		return value<o.value;
	}
	bool operator>(const Stat& o) const {
		return value>o.value;
	}
	bool operator!=(const Stat& o) const {
		return value!=o.value;
	}
};

class CS {
public:
	enum Stat_type {
		//COMPTE0 must be first
		COMPTE0, COMPTE1, COMPTE2, COMPTE3, COMPTE4, COMPTE5,
		COMPTE6, COMPTETOT,
		CLEAR00, CLEAR01, CLEAR02, CLEAR03, CLEAR04, CLEAR05,
		CLEAR06, CLEAR07, CLEAR08, CLEAR09, CLEAR10, CLEAR11,
		CLEAR12, CLEAR13, CLEARMORE,
		DEATH, FRAG, LINESTOT, SCORE,
		LAST_BEFORE_120, //Actual value irrelevent
		//All new 1.2.0 stats must be after LAST_BEFORE_120
		CLEAR14, CLEAR15, CLEAR16, CLEAR17, CLEAR18, CLEAR19, CLEAR20,
		ROTATED0, ROTATED1, ROTATED2, ROTATED3, ROTATED4, ROTATED5,
		ROTATED6, ROTATEDTOT,
		PLAYING_TIME, PPM, BPM, ROUND_WINS,
		SUICIDES, OVERKILLEE, OVERKILLER, MAXOVERKILLEE, MAXOVERKILLER,
		COMBO00, COMBO01, COMBO02, COMBO03, COMBO04, COMBO05,
		COMBO06, COMBO07, COMBO08, COMBO09, COMBO10, COMBO11,
		COMBO12, COMBO13, COMBO14, COMBO15, COMBO16, COMBO17,
		COMBO18, COMBO19, COMBO20,
		//LAST must be last (duh)
		LAST
	};
	Stat stats[LAST];
	void add(const CS& other);
	void clear();
	bool better(const CS& o, Stat_type type);
	static Stat_type clear_trans(int i) {
		if(i<=13)
			return (Stat_type) ((int) CLEAR00 + i);
		else
			return (Stat_type) ((int) CLEAR14 + i - 14);
	}
};

class GS {
public:
	enum Stat_type {
		PLAYING_TIME, ROUND_NUMBER,
		LAST
	};
	Stat stats[LAST];
	void clear();
};

#endif
