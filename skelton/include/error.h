/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef _HEADER_ERROR
#define _HEADER_ERROR

extern bool _debug;
extern bool skelton_debug;

#ifdef UGS_DIRECTX
	#ifdef _DEBUG
		extern int copper;
		void COPPER(int a, int b, int c);
		void debug_point();
	#else
		#define COPPER(a,b,c) ;
	#endif
	void calldx(long hr);
#endif

extern void delete_obj();
void msgbox(const char* m, ...);
void skelton_msgbox(const char* m, ...);
void lock_msgbox(const char* m, ...);
void user_output(const char* title, const char *msg);

class Error {
public:
	Error(const char* m, ...);
};

#endif
