dnl Process this file with autoconf to produce a configure script.

AC_INIT
AC_CONFIG_SRCDIR([source/quadra.cpp])

AC_ARG_WITH(libpng,
	AS_HELP_STRING([--with-libpng], [Specify location for libpng]))
AC_ARG_ENABLE(debug,
	AS_HELP_STRING([--enable-debug], [debugging options]))
AC_ARG_ENABLE(framerate,
	AS_HELP_STRING([--enable-framerate], [framerate indicator]))
AC_ARG_ENABLE(version-check,
	AS_HELP_STRING([--disable-version-check], [New version notification]))

if test ! -z "$with_libpng"; then
   CPPFLAGS="$CPPFLAGS -I$with_libpng"
   LDFLAGS="$LDFLAGS -L$with_libpng"
fi

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL

dnl Checks for libraries.
AC_CHECK_LIB(z, compress,, nozlib=1)
AC_CHECK_LIB(png, png_create_read_struct,, nolibpng=1)
AC_CHECK_LIB(SDL, SDL_Init, SDL_LIBS="-lSDL", nosdl=1)

dnl Checks for header files.
AC_CHECK_HEADER(zlib.h,, nozlib=1)
AC_CHECK_HEADER(png.h,, nolibpng=1)
AC_CHECK_HEADER(SDL.h)
AC_CHECK_HEADERS(sys/utsname.h)

save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -x objective-c"
AC_CHECK_HEADERS(Cocoa/Cocoa.h)
CFLAGS="$save_CFLAGS"

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.
AC_CHECK_FUNCS(uname)

if test "$enable_version_check" != "no"; then
   	AC_DEFINE(ENABLE_VERSION_CHECK, 1, [Check for new versions.])
fi

if test -n "$nozlib"; then
	AC_MSG_WARN([zlib is required and could not be found!])
	missing_required=yes
fi

if test -n "$nolibpng"; then
	AC_MSG_WARN([libpng is required and could not be found!])
	missing_required=yes
fi

if test -n "$missing_required"; then
	AC_MSG_ERROR([Required dependencies missing.])
fi

if test "$enable_framerate" = "yes"; then
   	AC_DEFINE(FRAMECOUNTER,, [Framerate indicator])
fi

if test "$enable_debug" != "yes"; then
   	CPPFLAGS="$CPPFLAGS -DNDEBUG"
fi

if test "$ac_cv_header_Cocoa_Cocoa_h" = "yes"; then
   	SDL_MAIN_OBJ="source/macosx/SDLMain.o"
	SDL_LIBS="-framework SDL -framework Cocoa"
fi

AC_SUBST(SDL_MAIN_OBJ, [$SDL_MAIN_OBJ])
AC_SUBST(SDL_LIBS, [$SDL_LIBS])
AC_DEFINE(UGS_LINUX, 1, [FIXME: temporary hack, until we properly detect stuff.])

AC_CONFIG_HEADERS([include/autoconf.h])
AC_CONFIG_FILES([config/config.mk])
AC_OUTPUT
