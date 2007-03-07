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

#include "video.h"

#include <assert.h>
#ifdef WIN32
#include <malloc.h>
#else
#include <alloca.h>
#endif
#include "SDL.h"
#include "video_dumb.h"
#include "bitmap.h"
#include "sprite.h"
#include "command.h"

Video* video = NULL;

class Video_SDL;

class Video_bitmap_SDL: public Video_bitmap {
public:
  Bitmap *fb;
  Video_bitmap_SDL(const int px, const int py,
                   const int w, const int h, const int rw);
  virtual ~Video_bitmap_SDL();
  virtual void rect(const int, const int, const int, const int, const int) const;
  virtual void box(const int, const int, const int, const int, const int) const;
  virtual void put_pel(const int, const int, const Byte) const;
  virtual void hline(const int, const int, const int, const Byte) const;
  virtual void vline(const int, const int, const int, const Byte) const;
  virtual void put_bitmap(const Bitmap&, const int, const int) const;
  virtual void put_sprite(const Sprite&, const int, const int) const;
  virtual void setmem();
};

Video_bitmap* Video_bitmap::New(const int px, const int py,
				const int w, const int h, const int rw) {
  return new Video_bitmap_SDL(px, py, w, h, rw);
}

Video_bitmap* Video_bitmap::New(const int px, const int py,
				const int w, const int h) {
  return new Video_bitmap_SDL(px, py, w, h, video->pitch);
}

class Video_SDL: public Video {
protected:
  int needsleep;
  int lastticks;
  bool fullscreen;
  void SetVideoMode();
public:
  SDL_Surface *screen;

  Video_SDL();
  virtual ~Video_SDL();
  virtual void lock();
  virtual void unlock();
  virtual void flip();
  virtual void start_frame();
  virtual void end_frame();
  virtual void dirty(int, int, int, int);
  virtual void setpal(const Palette&);
  virtual void dosetpal(SPalette*, int);
  virtual void restore();
  virtual void snap_shot(int, int, int, int);
  virtual void toggle_fullscreen();
};

Video* Video::New(int w, int h, int b, const char *wname, bool dumb) {
  assert(w == 640 && h == 480 && b == 8);

  if(dumb)
    return Video_Dumb::New(w, h, b, wname);
  else
    return new Video_SDL;
}

Video_bitmap_SDL::Video_bitmap_SDL(const int px, const int py,
                                   const int w, const int h, const int rw) {
  width = w;
  height = h;
  pos_x = px;
  pos_y = py;
  fb = new Bitmap(NULL, w, h, rw);
}

Video_bitmap_SDL::~Video_bitmap_SDL() {
  if(fb)
    delete fb;
}

void Video_bitmap_SDL::rect(int x, int y, int w, int h, int color) const {
  SDL_Rect rect;

  if(clip(x, y, w, h))
    return;

  rect.x = clip_x1 + pos_x;
  rect.y = clip_y1 + pos_y;
  rect.w = clip_x2 - clip_x1 + 1;
  rect.h = clip_y2 - clip_y1 + 1;

  SDL_FillRect(static_cast<Video_SDL*>(video)->screen, &rect, color);
}

void Video_bitmap_SDL::box(int x, int y, int w, int h, int c) const {
  hline(y, x, w, c);
  hline(y + h - 1, x, w, c);
  vline(x, y, h, c);
  vline(x + w - 1, y, h, c);
}

void Video_bitmap_SDL::put_pel(int x, int y, Byte c) const {
  fb->put_pel(x, y, c);
}

void Video_bitmap_SDL::hline(int y, int x, int w, Byte c) const {
  fb->hline(y, x, w, c);
}

void Video_bitmap_SDL::vline(int x, int y, int h, Byte c) const {
  fb->vline(x, y, h, c);
}

void Video_bitmap_SDL::put_bitmap(const Bitmap &d, int dx, int dy) const {
  d.draw(*fb, dx, dy);
}

void Video_bitmap_SDL::put_sprite(const Sprite &d, int dx, int dy) const {
  d.draw(*fb, dx, dy);
}

void Video_bitmap_SDL::setmem() {
  Video_SDL *vid = static_cast<Video_SDL*>(video);
  unsigned char *vfb = static_cast<unsigned char *>(vid->screen->pixels);
  if(fb)
    fb->setmem(vfb + (pos_y * vid->screen->pitch) + pos_x);
}

Video_SDL::Video_SDL() {
  xwindow = true;
  vb = Video_bitmap::New(0, 0, 640, 480, 640);
  width = 640;
  height = 480;
  bit = 8;
  framecount = 0;

  fullscreen = false;
  if(!command.token("nofullscreen"))
  {
	fullscreen = true;
  }
  SetVideoMode();

  lastticks = SDL_GetTicks();
  needsleep = 0;
}

Video_SDL::~Video_SDL() {
  delete vb;
}

void Video_SDL::lock() {
  assert(false);
}

void Video_SDL::unlock() {
  assert(false);
}

void Video_SDL::flip() {
  assert(false);
}

void Video_SDL::start_frame() {
  if(vb)
    vb->setmem();
}

void Video_SDL::end_frame() {
  if (newpal) {
    pal.set();
    newpal = false;
  }

  SDL_UpdateRect(screen, 0, 0, 0, 0);

	// RV: Make system sleep a little bit to keep framerate around 100 FPS
	{
		int CurrentTicks = SDL_GetTicks();
		int LastDeltaTicks = CurrentTicks - lastticks;
		// If a fairly large delay occured, ignore it and clamp it down to a 1/60.
		// This prevents application from trying to skip several seconds in case
		// system was halted or user dragged the window
		if(LastDeltaTicks > 100)
		{
			LastDeltaTicks = 10;
		}
		lastticks = CurrentTicks;

		// Try to get about 100 FPS (1000 msec / 100 = 10)
		needsleep += (1000/100)-LastDeltaTicks;
		// If needed, sleep a bit to slow down execution speed
		if(needsleep > 0)
		{
			SDL_Delay(needsleep);
		}
		else // Else, simply briefly yield to system
		{
			SDL_Delay(0);
		}
	}

  ++framecount;
}

void Video_SDL::dirty(int, int, int, int) {
  assert(false);
}

void Video_SDL::setpal(const Palette &p) {
  pal = p;
  newpal = true;
}

void Video_SDL::dosetpal(SPalette pal[256], int size) {
  SDL_Color *colors;
  int i;

  colors = static_cast<SDL_Color*>(alloca(sizeof(SDL_Color) * size));

  for(i = 0; i < size; i++) {
    colors[i].r = pal[i].peRed;
    colors[i].g = pal[i].peGreen;
    colors[i].b = pal[i].peBlue;
  }

  SDL_SetPalette(screen, SDL_LOGPAL|SDL_PHYSPAL, colors, 0, size);
}

void Video_SDL::restore() {
  assert(false);
}

void Video_SDL::snap_shot(int, int, int, int) {
  assert(false);
}

void Video_SDL::toggle_fullscreen() {
  fullscreen = !fullscreen;
  SetVideoMode();
}

void Video_SDL::SetVideoMode()
{
  int flags = SDL_SWSURFACE | SDL_HWPALETTE;
  if(fullscreen) flags |= SDL_FULLSCREEN;
  screen = SDL_SetVideoMode(640, 480, 8, flags);
  assert(screen);
  pitch = screen->pitch;
  need_paint = 2;
  newpal = true;
}
