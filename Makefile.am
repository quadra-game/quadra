
ACLOCAL_AMFLAGS = -Im4

datagamesdir = $(datadir)/games
pixmapsdir = $(datadir)/pixmaps

bin_PROGRAMS = quadra
datagames_DATA = quadra.res
pixmaps_DATA = images/quadra.xpm
EXTRA_PROGRAMS = wadder dumpwad test_version

CLEANFILES = quadra.res $(EXTRA_PROGRAMS)

AM_CPPFLAGS = \
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
	crypt.pl \
	sons \
	stats \
	textes

quadra_LDADD = $(SDL2_LIBS) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)
quadra_SOURCES = \
	source/bitmap.cpp \
	source/bitmap.h \
	source/bloc.cpp \
	source/bloc.h \
	source/buf.cpp \
	source/buf.h \
	source/byteorder.h \
	source/canvas.cpp \
	source/canvas.h \
	source/cfgfile.cpp \
	source/cfgfile.h \
	source/chat_text.cpp \
	source/chat_text.h \
	source/clipable.cpp \
	source/clipable.h \
	source/clock.cpp \
	source/clock.h \
	source/color.cpp \
	source/color.h \
	source/command.cpp \
	source/command.h \
	source/config.h \
	source/crypt.cpp \
	source/crypt.h \
	source/cursor.cpp \
	source/cursor.h \
	source/dict.cpp \
	source/dict.h \
	source/error.cpp \
	source/error.h \
	source/fonts.cpp \
	source/fonts.h \
	source/game.cpp \
	source/game.h \
	source/game_menu.cpp \
	source/game_menu.h \
	source/global.cpp \
	source/global.h \
	source/highscores.cpp \
	source/highscores.h \
	source/http_post.cpp \
	source/http_post.h \
	source/http_request.cpp \
	source/http_request.h \
	source/id.cpp \
	source/id.h \
	source/image.h \
	source/image_png.cpp \
	source/image_png.h \
	source/input.cpp \
	source/input.h \
	source/inter.cpp \
	source/inter.h \
	source/listbox.cpp \
	source/listbox.h \
	source/main.cpp \
	source/main.h \
	source/menu.cpp \
	source/menu.h \
	source/menu_base.cpp \
	source/menu_base.h \
	source/menu_demo_central.cpp \
	source/menu_demo_central.h \
	source/misc.cpp \
	source/misc.h \
	source/multi_player.cpp \
	source/multi_player.h \
	source/net.cpp \
	source/net.h \
	source/net_buf.h \
	source/net_call.h \
	source/net_list.cpp \
	source/net_list.h \
	source/net_server.cpp \
	source/net_server.h \
	source/net_stuff.cpp \
	source/net_stuff.h \
	source/nglog.cpp \
	source/nglog.h \
	source/notify.cpp \
	source/notify.h \
	source/overmind.cpp \
	source/overmind.h \
	source/packet.cpp \
	source/packet.h \
	source/packets.cpp \
	source/packets.h \
	source/palette.cpp \
	source/palette.h \
	source/pane.cpp \
	source/pane.h \
	source/player.cpp \
	source/player.h \
	source/qserv.cpp \
	source/qserv.h \
	source/quadra.cpp \
	source/quadra.h \
	source/random.cpp \
	source/random.h \
	source/recording.cpp \
	source/recording.h \
	source/res.cpp \
	source/res.h \
	source/res_compress.cpp \
	source/res_compress.h \
	source/res_name.cpp \
	source/res_name.h \
	source/resfile.cpp \
	source/resfile.h \
	source/resmanager.cpp \
	source/resmanager.h \
	source/score.cpp \
	source/score.h \
	source/sons.cpp \
	source/sons.h \
	source/sound.cpp \
	source/sound.h \
	source/sprite.cpp \
	source/sprite.h \
	source/stats.cpp \
	source/stats.h \
	source/stringtable.cpp \
	source/stringtable.h \
	source/texte.h \
	source/types.h \
	source/unicode.cpp \
	source/unicode.h \
	source/update.cpp \
	source/update.h \
	source/url.cpp \
	source/url.h \
	source/version.h \
	source/video.cpp \
	source/video.h \
	source/video_dumb.cpp \
	source/video_dumb.h \
	source/zone.cpp \
	source/zone.h \
	source/zone_list.cpp \
	source/zone_list.h \
	source/zone_text_clock.cpp \
	source/zone_text_clock.h

# FIXME: we use SDL_Log in wadder, should avoid.
wadder_LDADD = $(SDL2_LIBS) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)
wadder_SOURCES = \
	source/error.cpp \
	source/error.h \
	source/res.cpp \
	source/res.h \
	source/resfile.cpp \
	source/resfile.h \
	source/resmanager.cpp \
	source/resmanager.h \
	source/stringtable.cpp \
	source/stringtable.h \
	source/wadder.cpp

# FIXME: we use SDL_Log in dumpwad, should avoid.
dumpwad_LDADD = $(SDL2_LIBS)
dumpwad_SOURCES = \
	source/dumpwad.cpp \
	source/error.cpp \
	source/error.h \
	source/res.cpp \
	source/res.h \
	source/resfile.cpp \
	source/resfile.h \
	source/resmanager.cpp

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
