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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "SDL.h"

#include "sprite.h"
#include "video_dumb.h"

Video* video = NULL;

static SDL_Renderer* CreateRenderer(SDL_Window* window) {
  SDL_Renderer* renderer(
    SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC));

  // If we couldn't get a renderer that supports vsync, get whatever we can.
  if (!renderer)
    renderer = SDL_CreateRenderer(window, -1, 0);

  return renderer;
}

class Video_SDL : public Video {
public:
  Video_SDL(int w, int h, const char* wname, bool fullscreen)
    : Video(new Video_bitmap_SDL(this, 0, 0, w, h), w, h, w),
      window_(SDL_CreateWindow(
        "Quadra", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, w, h, fullscreen ?
            SDL_WINDOW_FULLSCREEN|SDL_WINDOW_INPUT_GRABBED : 0)),
      renderer_(CreateRenderer(window_)),
      texture_(SDL_CreateTexture(
        renderer_, SDL_GetWindowPixelFormat(window_),
        SDL_TEXTUREACCESS_STREAMING, w, h)),
      surface_(SDL_CreateRGBSurface(0, w, h, 8, 0, 0, 0, 0)) {
    SDL_SetWindowSize (window_, 1280, 960);
    SDL_assert_release(window_);
    SDL_assert_release(renderer_);
    SDL_assert_release(texture_);
    SDL_assert_release(surface_);
  }

  ~Video_SDL() {
    if (surface_)
      SDL_FreeSurface(surface_);
    if (texture_)
      SDL_DestroyTexture(texture_);
    if (renderer_)
      SDL_DestroyRenderer(renderer_);
    if (window_)
      SDL_DestroyWindow(window_);
  }

  virtual void end_frame() {
    if (newpal) {
      pal.set();
      newpal = false;
    }

    Uint32 pixel_format;
    int w, h;
    SDL_QueryTexture(texture_, &pixel_format, NULL, &w, &h);

    int bpp;
    Uint32 rmask, gmask, bmask, amask;
    SDL_PixelFormatEnumToMasks(pixel_format, &bpp, &rmask, &gmask, &bmask,
                               &amask);

    void* pixels;
    int pitch;
    SDL_LockTexture(texture_, NULL, &pixels, &pitch);
    SDL_Surface* const tex_surf(SDL_CreateRGBSurfaceFrom(
      pixels, w, h, bpp, pitch, rmask, gmask, bmask, amask));

    SDL_BlitSurface(surface_, NULL, tex_surf, NULL);

    SDL_FreeSurface(tex_surf);
    SDL_UnlockTexture(texture_);

    SDL_RenderCopy(renderer_, texture_, NULL, NULL);
    SDL_RenderPresent(renderer_);
    ++framecount;
  }
  virtual void dosetpal(const SDL_Color pal[256], int size) {
    SDL_SetPaletteColors(surface_->format->palette, pal, 0, size);
  }
  virtual void snap_shot(int x, int y, int w, int h) {
    SDL_assert_release(false);
  }

  Video_bitmap* new_bitmap(int px, int py, int w, int h) {
    return new Video_bitmap_SDL(this, px, py, w, h);
  }
private:
  class Video_bitmap_SDL : public Video_bitmap {
  public:
    Video_bitmap_SDL(Video_SDL* video, int px, int py, int w, int h)
      : Video_bitmap(px, py, w, h),
        video_(video) {
    }

    virtual void rect(const int x, const int y, const int w, const int h, const int color) const {
      if (clip(x, y, w, h))
        return;

      clip_y1 += pos_y;
      clip_y2 += pos_y;
      clip_x1 += pos_x;

      for(uint8_t* bp = (uint8_t*)video_->surface_->pixels + (clip_y1 * video_->surface_->pitch);
          bp <= (uint8_t*)video_->surface_->pixels + ((clip_y2) * video_->surface_->pitch);
          bp += video_->surface_->pitch) {
        memset(&bp[clip_x1], color, clip_w);
      }
    }
    virtual void put_pel(const int x, const int y, const uint8_t c) const {
      Bitmap bitmap(get_pixels(), width, height, video_->surface_->pitch);
      bitmap.put_pel(x, y, c);
    }
    virtual void hline(const int y, const int x, const int w, const uint8_t c) const {
      Bitmap bitmap(get_pixels(), width, height, video_->surface_->pitch);
      bitmap.hline(y, x, w, c);
    }
    virtual void vline(const int x, const int y, const int h, const uint8_t c) const {
      Bitmap bitmap(get_pixels(), width, height, video_->surface_->pitch);
      bitmap.vline(x, y, h, c);
    }
    virtual void put_bitmap(const Bitmap& d, const int dx, const int dy) const {
      Bitmap bitmap(get_pixels(), width, height, video_->surface_->pitch);
      d.draw(bitmap, dx, dy);
    }
    virtual void put_sprite(const Sprite& d, const int dx, const int dy) const {
      Bitmap bitmap(get_pixels(), width, height, video_->surface_->pitch);
      d.draw(bitmap, dx, dy);
    }

  private:
    void* get_pixels() const {
      uint8_t* pixels(static_cast<uint8_t*>(video_->surface_->pixels));
      return pixels + (pos_y * video_->surface_->pitch) + pos_x;
    }

    Video_SDL* const video_;
  };

  SDL_Window* const window_;
  SDL_Renderer* const renderer_;
  SDL_Texture* const texture_;
  SDL_Surface* const surface_;
};

void Video_bitmap::box(const int x, const int y, const int w, const int h,
                       const int color) const {
  hline(y, x, w, color);
  hline(y + h - 1, x, w, color);
  vline(x, y, h, color);
  vline(x + w - 1, y, h, color);
}

Video* Video::New(int w, int h, const char *wname, bool dumb, bool fullscreen) {
  Video* obj;

  if (dumb)
    obj = new Video_Dumb(w, h, wname);
  else
    obj = new Video_SDL(w, h, wname, fullscreen);

  return obj;
}

