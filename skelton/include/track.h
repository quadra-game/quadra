/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifndef __HEADER_TRACK_H
#define __HEADER_TRACK_H

#if defined(_DEBUG) && defined(UGS_DIRECTX) 

void * __cdecl operator new(unsigned int, int, const char *, int);

#define TRACKED public: \
                  void *operator new(size_t s) { \
										return ::operator new(s, 1, __FILE__, __LINE__); \
									} \
                private:

#else /* !_DEBUG || !UGS_DIRECTX*/

#define TRACKED

#endif

#endif
