/* -*- Mode: C++; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * Copyright (c) 1998-2000 Ludus Design enr.
 * All Rights Reserved.
 * Tous droits réservés.
 */

#include <signal.h>
#include <unistd.h>
#include <sys/shm.h>
#define Font XFont
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>
#undef Font
#include "debug.h"
#ifdef _DEBUG
#include "command.h"
#ifdef FPSMETER
#include "overmind.h"
#endif
#endif
#include "main.h"
#include "bitmap.h"
#include "sprite.h"
#include "video_x11.h"

/*
 * Gack! This is HORRIBLE!
 */
#include "quadra.xpm"

Video_bitmap* Video_bitmap_X11::New(const int px, const int py,
				    const int w, const int h, const int rw) {
  if(getenv("DISPLAY"))
    return new Video_bitmap_X11(px, py, w, h, rw);
  else
    return NULL;
}

Video_bitmap* Video_bitmap_X11::New(const int px, const int py,
				    const int w, const int h) {
  if(getenv("DISPLAY"))
    return new Video_bitmap_X11(px, py, w, h);
  else
    return NULL;
}

Video_bitmap_X11::Video_bitmap_X11(const int px, const int py,
				   const int w, const int h, const int rw) {
  width = w;
  height = h;
  pos_x = px;
  pos_y = py;
  fb = new Bitmap(NULL, w, h, rw);
}

Video_bitmap_X11::Video_bitmap_X11(const int px, const int py,
				   const int w, const int h) {
  width = w;
  height = h;
  pos_x = px;
  pos_y = py;
  fb = new Bitmap(NULL, w, h, video->pitch);
}

Video_bitmap_X11::~Video_bitmap_X11() {
  if(VERIFY(fb))
    delete fb;
}

void Video_bitmap_X11::rect(const int x,const int y,const int w,const int h,
			    const int color) const {
  int i;
  char *bp;

  if(clip(x, y, w, h))
    return;

  clip_y1 += pos_y;
  clip_y2 += pos_y;
  clip_x1 += pos_x;

  i = 0;
  for(bp = (char*) ((Video_X11*)video)->vfb + ((clip_y1) * video->vb->width);
      bp <= (char*) ((Video_X11*)video)->vfb + ((clip_y2) * video->vb->width);
      bp += video->vb->width) {
    memset(&bp[clip_x1], color, clip_w);
  }

  ((Video_X11*)video)->dirty(clip_x1, clip_y1,
			     clip_x1+clip_w-1, clip_y2);
}

void Video_bitmap_X11::box(const int x,const int y,const int w,const int h,
			   const int color) const {
  hline(y, x, w, color);
  hline(y+h-1, x, w, color);
  vline(x, y, h, color);
  vline(x+w-1, y, h, color);
}

void Video_bitmap_X11::get_bitmap(const Bitmap* bit, const int x, const int y,
				  const int w, const int h) const {
  if(clip(x, y, w, h))
    return;

  Bitmap src((*fb)[clip_y1]+clip_x1,
	     clip_w,
	     clip_y2-clip_y1+1,
	     fb->realwidth);

  src.draw(*bit, clip_x1-x, clip_y1-y);

  ((Video_X11*)video)->dirty(clip_x1+pos_x, clip_y1+pos_y,
			     clip_x1+pos_x+clip_w, clip_y2+pos_y);
}

void Video_bitmap_X11::put_pel(const int x, const int y, const Byte c) const {
  fb->put_pel(x, y, c);

  ((Video_X11*)video)->dirty(x+pos_x, y+pos_y,
			     x+pos_x, y+pos_y);
}

void Video_bitmap_X11::hline(const int y, const int x,
			     const int w, const Byte c) const {
  fb->hline(y, x, w, c);

  ((Video_X11*)video)->dirty(x+pos_x, y+pos_y,
			     x+w+pos_x, y+pos_y);
}

void Video_bitmap_X11::vline(const int x, const int y,
			     const int h, const Byte c) const {
  fb->vline(x, y, h, c);

  ((Video_X11*)video)->dirty(x+pos_x, y+pos_y,
			     x+pos_x, y+h+pos_y);
}

void Video_bitmap_X11::line(const int x1, const int y1,
			    const int x2, const int y2,
			    const Byte c) const {
  fb->line(x1, y1, x2, y2, c);

  ((Video_X11*)video)->dirty(x1+pos_x, y1+pos_y,
			     x2+pos_x, y2+pos_y);
}

void Video_bitmap_X11::put_bitmap(const Bitmap& d,
				  const int dx, const int dy) const {
  d.draw(*fb, dx, dy);

  ((Video_X11*)video)->dirty(dx+pos_x, dy+pos_y,
			     dx+d.width+pos_x-1, dy+d.height+pos_y);
}

void Video_bitmap_X11::put_sprite(const Sprite& d,
				  const int dx, const int dy) const {
  d.draw(*fb, dx, dy);

  ((Video_X11*)video)->dirty(dx+pos_x, dy+pos_y,
			     dx+d.width+pos_x, dy+d.height+pos_y);
}

void Video_bitmap_X11::setmem() {
  if(fb)
    fb->setmem((((Video_X11*)video)->vfb) + (pos_y * video->pitch) + pos_x);
}

Video* Video_X11::New(int w, int h, int b, const char *wname) {
  Display* display;
  XVisualInfo visualinfo;

  if(!VERIFY(getenv("DISPLAY")))
    return NULL;

  display = XOpenDisplay(NULL);

  if(!VERIFY(display)) {
    msgbox("Could not open [DISPLAY=%s]", getenv("DISPLAY"));
    return NULL;
  }

#ifdef _DEBUG
  if(command.token("sync")) {
    skelton_msgbox("X11: synchronous communication with X server enabled.\n");
    XSynchronize(display, true);
  }
#endif

  if(XMatchVisualInfo(display,
		      DefaultScreen(display),
		      8,
		      PseudoColor,
		      &visualinfo)) {
    return NEW(Video_X11_8, (w, h, b, wname, display, visualinfo.visual));
  };

  if(XMatchVisualInfo(display,
		      DefaultScreen(display),
		      16,
		      TrueColor,
		      &visualinfo)) {
    return NEW(Video_X11_16, (w, h, b, wname, display, visualinfo.visual, 16));
  };

  if(XMatchVisualInfo(display,
		      DefaultScreen(display),
		      15,
		      TrueColor,
		      &visualinfo)) {
    return NEW(Video_X11_16, (w, h, b, wname, display, visualinfo.visual, 15));
  };

  if(XMatchVisualInfo(display,
		      DefaultScreen(display),
		      24,
		      TrueColor,
		      &visualinfo)) {
    return NEW(Video_X11_24, (w, h, b, wname, display, visualinfo.visual));
  };

  msgbox("X11: Could not find any compatible visual.\n");
 
  return NULL;
}

static volatile bool quit = false;

static void sigint_handler(int sig) {
  msgbox("SIGINT caught (probably ctrl-c), exiting.\n");
  quit = true;
}

static bool xerror = false;
static XErrorHandler xoldhandler = 0;

static int xhandler(Display* display, XErrorEvent* error) {
  xerror = true;
  return 0;
}

Video_X11::Video_X11(int w, int h, int b,
		     const char *wname,
		     Display* dpy,
		     Visual* vis,
		     int d):
  display(NULL),
  image(NULL),
  vfb(NULL),
  min_x2(w), max_x2(0), min_y2(h), max_y2(0),
  window(0),
  visual(NULL),
  delete_win(0),
  depth(d),
  do_shm(false) {
  /* NOTE: ont assume que "b" est toujours 8, même si ont essaye de
     faire le smart à des places (comme "bit = b"). */

  XSetWindowAttributes attribs;
  XGCValues gcvalues;
  XClassHint *classhint;
  XWMHints *wmhints;
  XSizeHints sizehints;
  Pixmap ico_pixmap;
  Pixmap ico_mask;
  char *argv[1];
  int tmp_y;

  setuid(getuid());  
  setgid(getgid());
  seteuid(getuid());
  setegid(getgid());

  xwindow = true;

  for(tmp_y = 0; tmp_y < 480; tmp_y++) {
    min_x[tmp_y] = w;
    max_x[tmp_y] = 0;
  };

  width = w;
  height = h;
  bit = b;
  pitch = w;
  framecount = 0;
  newpal = true;
  need_paint = 2;

  signal(SIGINT, sigint_handler);

  display = dpy;
  visual = vis;

  attribs.event_mask = ExposureMask
    | KeyPressMask
    | KeyReleaseMask
    | ButtonPressMask
    | ButtonReleaseMask
    | LeaveWindowMask
    | FocusChangeMask
    | PointerMotionMask;

  attribs.border_pixel = 0;

  attribs.colormap = DefaultColormap(display, DefaultScreen(display));

  window = XCreateWindow(display,
			 DefaultRootWindow(display),
			 0, 0, /* X, Y relative to parent window */
			 w, h, /* width, height */
			 0, /* border width */
			 depth, /* color depth */
			 InputOutput,
			 visual,
			 CWEventMask | CWBorderPixel | CWColormap,
			 &attribs);

  if(!window)
    (void)new Error("XCreateWindow failed");

  classhint = XAllocClassHint();
  classhint->res_name = "LudusSkelton";
  classhint->res_class = (char*)wname;
  XSetClassHint(display, window, classhint);
  XFree(classhint);

  argv[0] = "/usr/games/quadra";
  XSetCommand(display, window, argv, 1);

  XpmCreatePixmapFromData(display,
			  DefaultRootWindow(display),
			  quadra_xpm,
			  &ico_pixmap,
			  &ico_mask,
			  NULL);

  wmhints = XAllocWMHints();
  wmhints->flags = WindowGroupHint | IconPixmapHint | IconMaskHint;
  wmhints->icon_pixmap = ico_pixmap;
  wmhints->icon_mask = ico_mask;
  wmhints->window_group = window;
  XSetWMHints(display, window, wmhints);
  XFree(wmhints);

  sizehints.flags = PMinSize | PMaxSize | PBaseSize;
  sizehints.min_width = w;
  sizehints.min_height = h;
  sizehints.max_width = w;
  sizehints.max_height = h;
  sizehints.base_width = w;
  sizehints.base_height = h;
  XSetWMNormalHints(display, window, &sizehints);

  delete_win = XInternAtom(display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display, window, &delete_win, 1);

  XStoreName(display, window, wname);
  XSetIconName(display, window, wname);

  gcvalues.graphics_exposures = False;

  gc = XCreateGC(display,
		 window,
		 GCGraphicsExposures,
		 &gcvalues);

  if(!gc)
    (void)new Error("XCreateGC failed");

  XMapRaised(display, window);

  while(1) {
    XEvent event;

    XNextEvent(display, &event);

    if((event.type == Expose) && (event.xexpose.count == 0))
      break;
  }

  do {
    if(!XShmQueryExtension(display))
      break;

    msgbox("Using MIT-SHM extension\n");
    do_shm = true;
  } while(0);

  image = NULL;

  if(do_shm) {
    image = XShmCreateImage(display,
			    visual,
			    depth, /* color depth */
			    ZPixmap,
			    NULL,
			    &shminfo,
			    w, h);

    if(!image)
      (void)new Error("XCreateImage failed");

    shminfo.shmid = shmget(IPC_PRIVATE,
			   image->height * image->bytes_per_line,
			   IPC_CREAT|0777);

    if(shminfo.shmid == -1)
      (void)new Error("XShm: shmget failed");

    shminfo.shmaddr = image->data = (char*)shmat(shminfo.shmid, 0, 0);
    shminfo.readOnly = False;

    shmctl(shminfo.shmid, IPC_RMID, 0);

    /* voodoo warning: XShmAttach foire horriblement si le serveur X
       n'est pas local. Soyons futés. */
    XSync(display, False);
    xerror = false;
    if(xoldhandler != xhandler)
      xoldhandler = XSetErrorHandler(xhandler);

    if(!XShmAttach(display, &shminfo))
      (void)new Error("XShm: XShmAttach failed");

    XSync(display, False);
    if(xoldhandler)
      XSetErrorHandler(xoldhandler);
    xoldhandler = 0;

    if(xerror) {
      do_shm = false;
      XDestroyImage(image);
      XSync(display, False);
      shmdt(shminfo.shmaddr);
      image = 0;
    }
  }

  if(!image) {
    image = XCreateImage(display,
			 visual,
			 depth, /* color depth */
			 ZPixmap,
			 0,
			 NULL,
			 w, h,
			 8,
			 0);

    if(!image)
      (void)new Error("XCreateImage failed");

    image->data = (char*)malloc(image->height * image->bytes_per_line);

    if(!image->data)
      (void)new Error("Out of memory");
  }

  vb = Video_bitmap::New(0, 0, w, h, w);

  if(!vb)
    (void)new Error("Couldn't create a video bitmap.");
}

Video_X11::~Video_X11() {
  if(vb)
    delete vb;

  if(do_shm) {
    image->data = NULL;
  }

  if(image)
    XDestroyImage(image);

  if(gc)
    XFreeGC(display, gc);

  if(window) {
    XUnmapWindow(display, window);
    XDestroyWindow(display, window);
  }

  XCloseDisplay(display);
}

void Video_X11::dirty(int x1, int y1, int x2, int y2) {
  short y;

  dirty2(x1, y1, x2, y2);

  if(y1 < 0)
    y1 = 0;
  if(y2 > 479)
    y2 = 479;

  for(y = y1; y <= y2; y++) {
    if(x1 < min_x[y]) {
      min_x[y] = x1;
      if(min_x[y] < 0)
	min_x[y] = 0;
    }
    
    if(x2 > max_x[y]) {
      max_x[y] = x2;
      if(max_x[y] >= vb->width)
	max_x[y] = vb->width - 1;
    }
  }
};

void Video_X11::dirty2(int x1, int y1, int x2, int y2) {
  if(x1 < min_x2) {
    min_x2 = x1;
    if(min_x2 < 0)
      min_x2 = 0;
  }

  if(x2 > max_x2) {
    max_x2 = x2;
    if(max_x2 >= vb->width)
      max_x2 = vb->width - 1;
  }

  if(y1 < min_y2) {
    min_y2 = y1;
    if(min_y2 < 0)
      min_y2 = 0;
  }

  if(y2 > max_y2) {
    max_y2 = y2;
    if(max_y2 >= vb->height)
      max_y2 = vb->height - 1;
  }
};

void Video_X11::lock() {
  if(vb)
    vb->setmem();
}

void Video_X11::unlock() {
  skelton_msgbox("Unimplemented: Video_X11::unlock\n");
}

void Video_X11::start_frame() {
  if(quit)
    quit_game();

  lock();
}

void Video_X11::flip() {
  sleep(0);

  if(max_x2 > min_x2) {
    /* des fois que la frame précédente soit pas finie */
    XSync(display, False);

    if(do_shm)
      XShmPutImage(display,
		   window,
		   gc,
		   image,
		   min_x2, min_y2, /* src x, y */
		   min_x2, min_y2, /* dest x, y */
		   max_x2-min_x2, max_y2-min_y2,
		   false);
    else
      XPutImage(display,
		window,
		gc,
		image,
		min_x2, min_y2, /* src x, y */
		min_x2, min_y2, /* dest x, y */
		max_x2-min_x2, max_y2-min_y2);
  
    /* pour que tout passe */
    XFlush(display);

    /* reset le dirty rect */
    min_x2 = vb->width;
    min_y2 = vb->height;
    max_x2 = 0;
    max_y2 = 0;
  }
}

void Video_X11::end_frame() {
#ifdef FPSMETER
  static unsigned int lastupdate = 0;
  static unsigned int lastframe = 0;
  static char st[100];

  if(ecran && ecran->font) {
    lastframe = getmsec()-lastupdate;
    if(lastframe)
      sprintf(st, "%i", 1000 / (getmsec()-lastupdate));
    else
      sprintf(st, "high");
    video->vb->rect(0, 0, 50, 20, 0);
    ecran->font->draw(st, video->vb, 0, 0);
    lastupdate = getmsec();
  }
#endif

  flip();
}

void Video_X11::setpal(const Palette& p) {
  pal = p;
  newpal = true;
}

void Video_X11::restore() {
  newpal = true;
  need_paint = 2;
}

void Video_X11::clean_up() {
}

void Video_X11::snap_shot(int x, int y, int w, int h) {
  skelton_msgbox("Unimplemented: Video_X11::snap_shot\n");
}
