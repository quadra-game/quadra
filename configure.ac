AC_PREREQ([2.68])

dnl The version number should be kept in sync with version.h.
AC_INIT([quadra], [1.2.0], [quadra-dev@googlegroups.com])
AC_CONFIG_SRCDIR([source/quadra.cpp])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign subdir-objects -Wall])
AM_SILENT_RULES([yes])

AC_ARG_ENABLE(version-check,
	AS_HELP_STRING([--disable-version-check], [new version notification]))

AC_LANG(C++)

# Checks for programs.
AC_PROG_CXX
AC_PROG_SED

# Checks for libraries.
AX_PATH_GENERIC([sdl2],,,, AC_MSG_ERROR([Could not find SDL2 (required).]))
AX_BOOST_BASE(,, AC_MSG_ERROR([Could not find Boost (required).]))
AX_BOOST_SYSTEM
AX_BOOST_FILESYSTEM

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h memory.h netdb.h netinet/in.h stdint.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h sys/timeb.h unistd.h machine/endian.h machine/byte_order.h])
AC_CHECK_HEADER([zlib.h],, AC_MSG_ERROR([Could not find header for zlib (required).]))
AC_CHECK_HEADER([png.h],, AC_MSG_ERROR([Could not find header for libpng (required).]))

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([atexit ftime getcwd gethostbyname gethostname gettimeofday inet_ntoa memmove memset mkdir select socket strcasecmp strchr strerror strncasecmp strrchr strstr])
AC_SEARCH_LIBS([compress], [z],, AC_MSG_ERROR([Could not find library for zlib (required).]))
AC_SEARCH_LIBS([png_create_read_struct], [png],, AC_MSG_ERROR([Could not find library for libpng (required).]))

if test "x$enable_version_check" != "xno"; then
	AC_DEFINE(ENABLE_VERSION_CHECK, 1, [Check for new versions.])
fi

AC_CONFIG_HEADERS([source/autoconf.h])
AC_CONFIG_FILES([Makefile quadra.spec:packages/quadra.spec.in packages/readme-win32.txt packages/quadra.nsi])
AC_OUTPUT
