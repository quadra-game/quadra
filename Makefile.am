
ACLOCAL_AMFLAGS = -Im4

datagamesdir = $(datadir)/games
pixmapsdir = $(datadir)/pixmaps

bin_PROGRAMS = quadra
datagames_DATA = quadra.res
pixmaps_DATA = images/quadra.xpm
EXTRA_PROGRAMS = wadder dumpwad test_version

CLEANFILES = quadra.res $(EXTRA_PROGRAMS)

AM_CPPFLAGS = \
	-I$(srcdir)/include -I$(srcdir)/skelton/include -I$(srcdir)/images \
	-DDATAGAMESDIR='"$(datagamesdir)"' $(SDL2_CFLAGS) $(BOOST_CPPFLAGS)

AM_LDFLAGS = $(BOOST_LDFLAGS)

EXTRA_DIST = \
	ChangeLog \
	LICENSE \
	NEWS.md \
	README.md \
	VisualC++ \
	demos \
	fonts \
	images \
	packages/quadra.nsi \
	packages/quadra.nsi.in \
	packages/quadra.spec.in \
	packages/readme-win32.txt \
	packages/readme-win32.txt.in \
	packages/winzip_icone.ico \
	quadra.spec \
	resources.txt \
	server \
	skelton/README \
	skelton/common/crypt.pl \
	skelton/tools/dumpdb \
	sons \
	stats \
	textes

quadra_LDADD = $(SDL2_LIBS) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)
quadra_SOURCES = \
	images/quadra.xpm \
	include/bloc.h \
	include/canvas.h \
	include/cfgfile.h \
	include/chat_text.h \
	include/color.h \
	include/config.h \
	include/fonts.h \
	include/game.h \
	include/game_menu.h \
	include/global.h \
	include/highscores.h \
	include/menu.h \
	include/menu_base.h \
	include/menu_demo_central.h \
	include/misc.h \
	include/multi_player.h \
	include/net_list.h \
	include/net_server.h \
	include/net_stuff.h \
	include/nglog.h \
	include/packets.h \
	include/pane.h \
	include/qserv.h \
	include/quadra.h \
	include/recording.h \
	include/res_name.h \
	include/score.h \
	include/sons.h \
	include/stats.h \
	include/texte.h \
	include/update.h \
	include/version.h \
	include/zone.h \
	include/zone_list.h \
	skelton/common/bitmap.cpp \
	skelton/common/buf.cpp \
	skelton/common/clipable.cpp \
	skelton/common/clock.cpp \
	skelton/common/command.cpp \
	skelton/common/crypt.cpp \
	skelton/common/cursor.cpp \
	skelton/common/dict.cpp \
	skelton/common/http_post.cpp \
	skelton/common/http_request.cpp \
	skelton/common/id.cpp \
	skelton/common/image_png.cpp \
	skelton/common/inter.cpp \
	skelton/common/listbox.cpp \
	skelton/common/net.cpp \
	skelton/common/notify.cpp \
	skelton/common/overmind.cpp \
	skelton/common/packet.cpp \
	skelton/common/random.cpp \
	skelton/common/res_compress.cpp \
	skelton/common/resfile.cpp \
	skelton/common/resmanager.cpp \
	skelton/common/sprite.cpp \
	skelton/common/stringtable.cpp \
	skelton/common/unicode.cpp \
	skelton/common/url.cpp \
	skelton/common/video_dumb.cpp \
	skelton/common/zone_text_clock.cpp \
	skelton/include/bitmap.h \
	skelton/include/buf.h \
	skelton/include/byteorder.h \
	skelton/include/clipable.h \
	skelton/include/clock.h \
	skelton/include/command.h \
	skelton/include/crypt.h \
	skelton/include/cursor.h \
	skelton/include/dict.h \
	skelton/include/error.h \
	skelton/include/http_post.h \
	skelton/include/http_request.h \
	skelton/include/id.h \
	skelton/include/image.h \
	skelton/include/image_png.h \
	skelton/include/input.h \
	skelton/include/inter.h \
	skelton/include/listbox.h \
	skelton/include/main.h \
	skelton/include/net.h \
	skelton/include/net_buf.h \
	skelton/include/net_call.h \
	skelton/include/notify.h \
	skelton/include/overmind.h \
	skelton/include/packet.h \
	skelton/include/palette.h \
	skelton/include/random.h \
	skelton/include/res.h \
	skelton/include/res_compress.h \
	skelton/include/resfile.h \
	skelton/include/resmanager.h \
	skelton/include/sound.h \
	skelton/include/sprite.h \
	skelton/include/stringtable.h \
	skelton/include/types.h \
	skelton/include/unicode.h \
	skelton/include/url.h \
	skelton/include/video.h \
	skelton/include/video_dumb.h \
	skelton/include/zone_text_clock.h \
	skelton/svgalib/error.cpp \
	skelton/svgalib/input.cpp \
	skelton/svgalib/main.cpp \
	skelton/svgalib/palette.cpp \
	skelton/svgalib/res.cpp \
	skelton/svgalib/sound.cpp \
	skelton/svgalib/video.cpp \
	source/bloc.cpp \
	source/canvas.cpp \
	source/cfgfile.cpp \
	source/chat_text.cpp \
	source/color.cpp \
	source/fonts.cpp \
	source/game.cpp \
	source/game_menu.cpp \
	source/global.cpp \
	source/highscores.cpp \
	source/menu.cpp \
	source/menu_base.cpp \
	source/menu_demo_central.cpp \
	source/misc.cpp \
	source/multi_player.cpp \
	source/net_list.cpp \
	source/net_server.cpp \
	source/net_stuff.cpp \
	source/nglog.cpp \
	source/packets.cpp \
	source/pane.cpp \
	source/qserv.cpp \
	source/quadra.cpp \
	source/recording.cpp \
	source/res_name.cpp \
	source/score.cpp \
	source/sons.cpp \
	source/stats.cpp \
	source/update.cpp \
	source/zone.cpp \
	source/zone_list.cpp

# FIXME: we use SDL_Log in wadder, should avoid.
wadder_LDADD = $(SDL2_LIBS) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)
wadder_SOURCES = \
	skelton/common/resfile.cpp \
	skelton/common/resmanager.cpp \
	skelton/common/stringtable.cpp \
	skelton/include/error.h \
	skelton/include/res.h \
	skelton/include/resfile.h \
	skelton/include/resmanager.h \
	skelton/include/stringtable.h \
	skelton/svgalib/error.cpp \
	skelton/svgalib/res.cpp \
	skelton/tools/wadder/wadder.cpp

# FIXME: we use SDL_Log in dumpwad, should avoid.
dumpwad_LDADD = $(SDL2_LIBS)
dumpwad_SOURCES = \
	skelton/common/resfile.cpp \
	skelton/common/resmanager.cpp \
	skelton/include/error.h \
	skelton/include/res.h \
	skelton/include/resfile.h \
	skelton/svgalib/error.cpp \
	skelton/svgalib/res.cpp \
	skelton/tools/dumpwad.cpp

test_version_SOURCES = \
	source/test_version.cpp

# Validate that the version strings are consistent when we make a distribution.
dist-hook: test_version
	$(AM_V_at)./test_version

# FIXME: Ideally, we would have a variable here listing the resource, instead
# of having them in a separate file, so we could use them for EXTRA_DIST well
# as dependencies (the shell call-out is a GNU make extension). But this would
# make it more difficult to build from Windows without extra tools.
quadra.res: resources.txt wadder$(EXEEXT) $(shell cat $(srcdir)/resources.txt)
	$(AM_V_GEN)./wadder$(EXEEXT) $(srcdir)/ $@ $^ || rm -f $@
