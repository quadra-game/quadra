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

#ifndef NDEBUG
#include <stdio.h>
#endif
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/cdrom.h>
#include "error.h"
#include "types.h"
#include "command.h"
#include "music.h"

RCSID("$Id$")

#define CDROM_DEVICE "/dev/cdrom"

class MusicLinux: public Music {
private:
  int fd;
  int playing;
  bool loop_all;
  bool is_playing;
	unsigned char starttrack;
	unsigned char endtrack;
public:
  MusicLinux();
  virtual ~MusicLinux();
  virtual void open();
  virtual void close();
  virtual void play(int quel, bool loop=false);
  virtual void replay();
  virtual void stop();
};

Music *music=NULL;

Music* Music::alloc() {
  if(!command.token("nocd"))
    return new MusicLinux;
  else
    return new MusicNull;
}

MusicLinux::MusicLinux() {
  active = false;
  is_playing = false;
  open();
}

MusicLinux::~MusicLinux() {
  stop();
}

void MusicLinux::play(int quel, bool loop) {
  struct cdrom_ti ti;
  int status;

  if(!active)
    return;

	if(quel < starttrack)
		quel = starttrack;

  if(loop_all = loop)
    playing = starttrack;
  else
    playing = quel;

  ti.cdti_trk0 = quel;
  ti.cdti_ind0 = 0;
  ti.cdti_trk1 = endtrack;
  ti.cdti_ind1 = 0;

  status = ioctl(fd, CDROMPLAYTRKIND, &ti);

#ifndef NDEBUG
  if(status < 0)
    perror("CDROMPLAYTRKIND");
#endif

  is_playing = true;
}

void MusicLinux::replay() {
  play(playing, loop_all);
}

void MusicLinux::stop() {
	int status;

  if(!active || !is_playing)
    return;

	status = ioctl(fd, CDROMSTOP);

#ifndef NDEBUG
  if(status != 0)
    perror("CDROMPLAYTRKIND");
#endif
}

void MusicLinux::open() {
	int status;
	struct cdrom_tochdr tochdr;

  if(active)
    return;

  if((fd = ::open(CDROM_DEVICE, O_RDONLY)) < 0) {
#ifndef NDEBUG
		perror("open");
#endif
    return;
	}

	status = ioctl(fd, CDROMREADTOCHDR, &tochdr);
	if(status != 0) {
#ifndef NDEBUG
		perror("CDROMREADTOCHDR");
#endif
		::close(fd);
		active = false;
		return;
	}

	starttrack = tochdr.cdth_trk0;
	endtrack = tochdr.cdth_trk1;

	skelton_msgbox("start track = %i\nend track = %i\n", starttrack, endtrack);

  active = true;
}

void MusicLinux::close() {
  if(!active)
    return;

  ::close(fd);

  active = false;
}

