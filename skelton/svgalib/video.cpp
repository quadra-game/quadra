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
#include "sprite.h"
#include "command.h"
#include "version.h"
#include "image_png.h"
#include "cursor.h"
#include "input.h"

Video* video = NULL;

Video_bitmap::Video_bitmap(const int px, const int py, const int w,
                           const int h):
  Clipable(w, h),
  pos_x(px),
  pos_y(py) {
}

Video_bitmap::~Video_bitmap() {
}

void Video_bitmap::rect(int x, int y, int w, int h, int color) const {
  SDL_Rect rect;

  if(clip(x, y, w, h))
    return;

  rect.x = clip_x1 + pos_x;
  rect.y = clip_y1 + pos_y;
  rect.w = clip_x2 - clip_x1 + 1;
  rect.h = clip_y2 - clip_y1 + 1;

  SDL_FillRect(video->surface(), &rect, color);
  clip_dirty(x, y, w, h); 
}

void Video_bitmap::box(int x, int y, int w, int h, int c) const {
  hline(y, x, w, c);
  hline(y + h - 1, x, w, c);
  vline(x, y, h, c);
  vline(x + w - 1, y, h, c);
}

void Video_bitmap::put_pel(int x, int y, Byte c) const {
  rect(x, y, 1, 1, c);
}

void Video_bitmap::hline(int y, int x, int w, Byte c) const {
  rect(x, y, w, 1, c);
}

void Video_bitmap::vline(int x, int y, int h, Byte c) const {
  rect(x, y, 1, h, c);
}

void Video_bitmap::clip_dirty(int x, int y, int w, int h) const {
  if(clip(x, y, w, h))
    return;
  video->set_dirty(pos_x+clip_x1, pos_y+clip_y1, pos_x+clip_x2, pos_y+clip_y2);
}

void Video_bitmap::put_surface(SDL_Surface* surface, int dx, int dy) const {
  SDL_Rect rect;
  rect.x = rect.y = 0;
  rect.w = surface->w;
  rect.h = surface->h;

  put_surface(surface, rect, dx, dy);
}

void Video_bitmap::put_surface(SDL_Surface* surface, const SDL_Rect& _srcrect, int dx, int dy) const {
	assert(_srcrect.x >= 0 && _srcrect.y >= 0);
	assert(_srcrect.w > 0 && _srcrect.h > 0);
	assert(_srcrect.x < surface->w && _srcrect.y < surface->h);
	assert(_srcrect.x + _srcrect.w <= surface->w);
	assert(_srcrect.y + _srcrect.h <= surface->h);

  if (clip(dx, dy, _srcrect.w, _srcrect.h))
    return;

  SDL_Rect srcrect;
  srcrect.x = _srcrect.x + clip_x1 - dx;
  srcrect.y = _srcrect.y + clip_y1 - dy;
  srcrect.w = clip_x2 - clip_x1 + 1;
  srcrect.h = clip_y2 - clip_y1 + 1;

  SDL_Rect dstrect;
  dstrect.x = pos_x + clip_x1;
  dstrect.y = pos_y + clip_y1;

  clip_dirty(dx, dy, srcrect.w, srcrect.h);
  SDL_BlitSurface(surface, &srcrect, video->surface(), &dstrect);
}

void Video_bitmap::put_bitmap(const Bitmap &d, int dx, int dy) const {
  video->clone_palette(d.surface);
  put_surface(d.surface, dx, dy);
}

Video::Video():
  vb(0, 0, 640, 480),
  need_paint(),
  framecount(0),
  newpal(),
  pal(),
  needsleep(0),
  lastticks(SDL_GetTicks()),
  fullscreen(!command.token("nofullscreen")),
  mDirtyEmpty(true),
	mDirtyX1(),
	mDirtyY1(),
	mDirtyX2(),
	mDirtyY2(),
  display(NULL),
  offscreen(NULL),
  mHScaleDst2Src(NULL),
  mVScaleDst2Src(NULL)
{
  SetVideoMode();
  // Create offscreen surface which will be used for rendering by the game
  offscreen = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 480, 8, 0, 0, 0, 0);
}

Video::~Video() {
  delete(mHScaleDst2Src);
  delete(mVScaleDst2Src);
}

void Video::end_frame() {
  if (newpal) {
    SDL_SetColors(display, pal.pal, 0, pal.size);
    SDL_SetColors(offscreen, pal.pal, 0, pal.size);
    newpal = false;
    // Automatically set the whole screen 'dirty' when a palette change occur
    // to enforce SDL to redraw the screen
    set_dirty(0, 0, offscreen->w-1, offscreen->h-1);
  }

	// Draw and convert only the dirty region to screen
	if(!mDirtyEmpty)
	{
	  update_dirty_display();
	}

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

void Video::update_dirty_display()
{
  // Prepare source rect region (i.e.: in 640x480 offscreen surface) that needs to be shown
	SDL_Rect rect;
	rect.x = mDirtyX1;
	rect.y = mDirtyY1;
	rect.w = mDirtyX2 - mDirtyX1 + 1;
	rect.h = mDirtyY2 - mDirtyY1 + 1;

  // Prepare destination rect region
  SDL_Rect dstrect = rect;

  // Special optimized case: if display surface is exactly 640x480, no scaling needed: perform fast blit
  if(display->w == 640 && display->h == 480)
  {
    SDL_BlitSurface(offscreen, &rect, display, &dstrect);
  }
  else
  {
    // 2D arbitrary scaling
    // Evaluate destination rect coordinates:
    dstrect.x = mHScaleSrc2Dst[mDirtyX1];
    dstrect.y = mVScaleSrc2Dst[mDirtyY1];
    dstrect.w = mHScaleSrc2Dst[mDirtyX2+1]-1 - mHScaleSrc2Dst[mDirtyX1] + 1;
    dstrect.h = mVScaleSrc2Dst[mDirtyY2+1]-1 - mVScaleSrc2Dst[mDirtyY1] + 1;
    SDL_LockSurface(display);
    
    Uint8* dst = (Uint8*) display->pixels;
    dst += dstrect.x + (dstrect.y)*display->pitch;
    
    for(Uint32 j=0; j<dstrect.h; j++)
    {
      Uint8* src = (Uint8*) offscreen->pixels;
      src += mVScaleDst2Src[dstrect.y+j]*offscreen->pitch;
      
      for(Uint32 i=0; i<dstrect.w; i++)
      {
        *dst++ = *(src + mHScaleDst2Src[dstrect.x+i]);
      }
      dst += display->pitch - dstrect.w;
    }
    SDL_UnlockSurface(display);
  }
  
	SDL_UpdateRect(display, dstrect.x, dstrect.y, dstrect.w, dstrect.h);
	mDirtyEmpty = true;
}

void Video::setpal(const Palette &p) {
  pal = p;
  newpal = true;
}

void Video::snap_shot(int, int, int, int) {
  assert(false);
}

void Video::toggle_fullscreen() {
  fullscreen = !fullscreen;
  SetVideoMode();
}

void Video::clone_palette(SDL_Surface* surface)
{
	SDL_SetColors(surface, display->format->palette->colors, 0, display->format->palette->ncolors);
}

void Video::SetVideoMode()
{
	// Set window title and window icon using SDL
	{
		char st[256];
		sprintf(st, "Quadra %s", VERSION_STRING);
		SDL_WM_SetCaption(st, NULL);
		Res_doze res("window_newicon.png");
		Png img(res);
    SDL_Surface* surf = img.new_surface();

		// Fetch colorkey from top-left pixel value
		SDL_SetColorKey(surf, SDL_SRCCOLORKEY, img.pic()[0]);

		SDL_WM_SetIcon(surf, NULL);
		SDL_FreeSurface(surf);
	}

  create_display(640, 480);
}

void Video::create_display(int w, int h)
{
  int flags = SDL_SWSURFACE|SDL_HWPALETTE;
  if(fullscreen)
  {
    flags |= SDL_FULLSCREEN;
  }
  else
  {
    flags |= SDL_RESIZABLE; // In windowed mode, make the window resizable
  }
  display = SDL_SetVideoMode(w, h, 8, flags);
  assert(display);
  newpal = true; // Force palette to be applied to newly created display surface
  
  // build 2d scaling tables (horizontal and vertical)
  {
    // allocate memory according to window dimension
    delete(mHScaleDst2Src);
    mHScaleDst2Src = (Uint32*) malloc(display->w << 2); // 4 bytes per element
    delete(mVScaleDst2Src);
    mVScaleDst2Src = (Uint32*) malloc(display->h << 2); // 4 bytes per element
    memset(mHScaleSrc2Dst, 0xffffffff, sizeof(mHScaleSrc2Dst));
    memset(mVScaleSrc2Dst, 0xffffffff, sizeof(mVScaleSrc2Dst));
    
    Uint32 hscale = ((display->w << 16)) / 640;
    Uint32 src_x_scale = 32768;
    Uint32 src_x_scale_step = (4294967295 / hscale);   
    // Prepare scaling table
    for(int i=0; i<display->w; i++)
    {
      Uint32 t = (src_x_scale>>16);
      if(t > 639) t = 639; // Clamp to maximum pixel
      mHScaleDst2Src[i] = t;
      if(mHScaleSrc2Dst[t] == 0xffffffff) mHScaleSrc2Dst[t] = i;
      src_x_scale += src_x_scale_step;
    }
    // Check for missing source to dest conversion (in case window is smaller than 640x480)
    for(int i=1; i<640; i++)
    {
      if(mHScaleSrc2Dst[i] == 0xffffffff) mHScaleSrc2Dst[i] = mHScaleSrc2Dst[i-1];
    }
    mHScaleSrc2Dst[640] = w;
    mVScaleSrc2Dst[480] = h;

    Uint32 vscale = ((display->h << 16)) / 480;
    Uint32 src_y_scale = 32768;
    Uint32 src_y_scale_step = (4294967295 / vscale);
    for(int i=0; i<display->h; i++)
    {
      Uint32 t = (src_y_scale>>16);
      if(t > 479) t = 479; // Clamp to maximum pixel
      mVScaleDst2Src[i] = t;
      if(mVScaleSrc2Dst[t] == 0xffffffff) mVScaleSrc2Dst[t] = i;
      src_y_scale += src_y_scale_step;
    }
    // Check for missing source to dest conversion (in case window is smaller than 640x480)
    for(int i=1; i<480; i++)
    {
      if(mVScaleSrc2Dst[i] == 0xffffffff) mVScaleSrc2Dst[i] = mVScaleSrc2Dst[i-1];
    }

  }
}

void Video::transform_to_local(Uint16& x, Uint16& y)
{
  if(x >= 0 && x < display->w) x = mHScaleDst2Src[x];
  if(y >= 0 && y < display->h) y = mVScaleDst2Src[y];
}

void Video::set_dirty(int x1, int y1, int x2, int y2)
{
	if(mDirtyEmpty)
	{
		mDirtyX1 = x1;
		mDirtyY1 = y1;
		mDirtyX2 = x2;
		mDirtyY2 = y2;
		mDirtyEmpty = false;
	}
	else
	{
		if(x1 < mDirtyX1) mDirtyX1 = x1;
		if(y1 < mDirtyY1) mDirtyY1 = y1;
		if(x2 > mDirtyX2) mDirtyX2 = x2;
		if(y2 > mDirtyY2) mDirtyY2 = y2;
	}
}

// Handle SDL windowed-mode resize event
void Video::resize_event(int w, int h)
{
  bool fix_aspect = false;
  bool fix_step = false;
  // SDL BUG: after the window is resized, the keyboard gets 'reset' and all keys are released.
  // I don't know how to fix this.
  
  // When holding SHIFT while resizing window, enforce fixed aspect ratio
  if(SDL_GetModState() & KMOD_SHIFT)
  {
    fix_aspect = true;
  }
  // When holding CONTROL while resizing window, enforce integer-step scaling (2x, 3x, 4x, ...)
  if(SDL_GetModState() & KMOD_CTRL)
  {
    fix_step = true;
  }
  // Fix a minimum window size (to prevent scaling problem)
  if(w<16) w = 64;
  if(h<16) h = 48;
  
  float hscale = w / 640.f;
  float vscale = h / 480.f;
  if(fix_step)
  {
    hscale = float(int(hscale+0.5f));
    if(hscale < 1.f) hscale = 1.f;  
    w = int(640 * hscale);

    vscale = float(int(vscale+0.5f));
    if(vscale < 1.f) vscale = 1.f;  
    h = int(480 * vscale);
  }
  
  if(fix_aspect)
  {
    if(vscale < hscale)
    {
      w = int(640.f * vscale);
    }
    else
    {
      h = int(480.f * hscale);
    }
  }
  
  create_display(w, h); // Recreate display surface according to new window dimension
  msgbox("Video::resize_event: w=%i h=%i\n", w, h);
}
