/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#ifdef _DEBUG
#include <stdio.h>
#endif
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/cdrom.h>
#include "error.h"
#include "music.h"

#define CDROM_DEVICE "/dev/cdrom"

class MusicLinux: public Music {
private:
  int fd;
  int playing;
  bool loop_all;
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
	return new MusicLinux;
}

MusicLinux::MusicLinux() {
  active = false;
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

#ifdef _DEBUG
  if(status < 0)
    perror("CDROMPLAYTRKIND");
#endif
}

void MusicLinux::replay() {
  play(playing, loop_all);
}

void MusicLinux::stop() {
	int status;

  if(!active)
    return;

	status = ioctl(fd, CDROMSTOP);

#ifdef _DEBUG
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
#ifdef _DEBUG
		perror("open");
#endif
    return;
	}

	status = ioctl(fd, CDROMREADTOCHDR, &tochdr);
	if(status != 0) {
#ifdef _DEBUG
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

