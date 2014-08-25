
ACLOCAL_AMFLAGS = -Im4

datagamesdir = $(datadir)/games
pixmapsdir = $(datadir)/pixmaps

noinst_PROGRAMS = wadder dumpwad test_version

if APP_BUNDLE
noinst_PROGRAMS += Quadra.app quadra
Quadra_app_SOURCES =
else
bin_PROGRAMS = quadra
datagames_DATA = quadra.res
pixmaps_DATA = images/quadra.xpm
endif

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

if APP_BUNDLE
quadra_LDFLAGS = $(AM_LDFLAGS) -rpath @executable_path/../Frameworks
endif
quadra_LDADD = $(SDL2_LIBS) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)
quadra_SOURCES = \
	source/attack.h \
	source/bitmap.cc \
	source/bitmap.h \
	source/bloc.cc \
	source/bloc.h \
	source/buf.cc \
	source/buf.h \
	source/canvas.cc \
	source/canvas.h \
	source/cfgfile.cc \
	source/cfgfile.h \
	source/chat_text.cc \
	source/chat_text.h \
	source/clipable.cc \
	source/clipable.h \
	source/clock.cc \
	source/clock.h \
	source/color.cc \
	source/color.h \
	source/command.cc \
	source/command.h \
	source/crypt.cc \
	source/crypt.h \
	source/cursor.cc \
	source/cursor.h \
	source/dict.cc \
	source/dict.h \
	source/error.cc \
	source/error.h \
	source/fonts.cc \
	source/fonts.h \
	source/game.cc \
	source/game.h \
	source/game_menu.cc \
	source/game_menu.h \
	source/global.cc \
	source/global.h \
	source/highscores.cc \
	source/highscores.h \
	source/http_post.cc \
	source/http_post.h \
	source/http_request.cc \
	source/http_request.h \
	source/id.cc \
	source/id.h \
	source/image.h \
	source/image_png.cc \
	source/image_png.h \
	source/input.cc \
	source/input.h \
	source/inter.cc \
	source/inter.h \
	source/listbox.cc \
	source/listbox.h \
	source/main.cc \
	source/main.h \
	source/menu.cc \
	source/menu.h \
	source/menu_base.cc \
	source/menu_base.h \
	source/menu_demo_central.cc \
	source/menu_demo_central.h \
	source/misc.cc \
	source/misc.h \
	source/multi_player.cc \
	source/multi_player.h \
	source/net.cc \
	source/net.h \
	source/net_buf.h \
	source/net_call.h \
	source/net_list.cc \
	source/net_list.h \
	source/net_server.cc \
	source/net_server.h \
	source/net_stuff.cc \
	source/net_stuff.h \
	source/nglog.cc \
	source/nglog.h \
	source/notify.cc \
	source/notify.h \
	source/overmind.cc \
	source/overmind.h \
	source/packet.cc \
	source/packet.h \
	source/packets.cc \
	source/packets.h \
	source/palette.cc \
	source/palette.h \
	source/pane.cc \
	source/pane.h \
	source/player.cc \
	source/player.h \
	source/qserv.cc \
	source/qserv.h \
	source/quadra.cc \
	source/quadra.h \
	source/random.cc \
	source/random.h \
	source/recording.cc \
	source/recording.h \
	source/res.cc \
	source/res.h \
	source/res_compress.cc \
	source/res_compress.h \
	source/res_name.cc \
	source/res_name.h \
	source/resfile.cc \
	source/resfile.h \
	source/resmanager.cc \
	source/resmanager.h \
	source/score.cc \
	source/score.h \
	source/sons.cc \
	source/sons.h \
	source/sound.cc \
	source/sound.h \
	source/sprite.cc \
	source/sprite.h \
	source/stats.cc \
	source/stats.h \
	source/stringtable.cc \
	source/stringtable.h \
	source/texte.h \
	source/types.h \
	source/unicode.cc \
	source/unicode.h \
	source/update.cc \
	source/update.h \
	source/url.cc \
	source/url.h \
	source/version.h \
	source/video.cc \
	source/video.h \
	source/video_dumb.cc \
	source/video_dumb.h \
	source/zone.cc \
	source/zone.h \
	source/zone_list.cc \
	source/zone_list.h \
	source/zone_text_clock.cc \
	source/zone_text_clock.h

# FIXME: we use SDL_Log in wadder, should avoid.
wadder_LDADD = $(SDL2_LIBS) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)
wadder_SOURCES = \
	source/error.cc \
	source/error.h \
	source/res.cc \
	source/res.h \
	source/resfile.cc \
	source/resfile.h \
	source/resmanager.cc \
	source/resmanager.h \
	source/stringtable.cc \
	source/stringtable.h \
	source/wadder.cc

# FIXME: we use SDL_Log in dumpwad, should avoid.
dumpwad_LDADD = $(SDL2_LIBS)
dumpwad_SOURCES = \
	source/dumpwad.cc \
	source/error.cc \
	source/error.h \
	source/res.cc \
	source/res.h \
	source/resfile.cc \
	source/resfile.h \
	source/resmanager.cc

test_version_SOURCES = \
	source/test_version.cc

# Validate that the version strings are consistent when we make a distribution.
dist-hook: test_version
	$(AM_V_at)./test_version

# FIXME: Ideally, we would have a variable here listing the resource, instead
# of having them in a separate file, so we could use them for EXTRA_DIST well
# as dependencies (the shell call-out is a GNU make extension). But this would
# make it more difficult to build from Windows without extra tools.
quadra.res: resources.txt wadder$(EXEEXT) $(shell cat $(srcdir)/resources.txt)
	$(AM_V_GEN)./wadder$(EXEEXT) $(srcdir)/ $@ $^ || rm -f $@

if APP_BUNDLE
Quadra.app: quadra$(EXEEXT) quadra.res
	rm -rf $@
	$(MKDIR_P) $@/Contents/MacOS
	$(MKDIR_P) $@/Contents/Resources
	echo "APPL????" > $@/Contents/PkgInfo
	$(INSTALL_PROGRAM) quadra$(EXEEXT) $@/Contents/MacOS
	$(INSTALL_PROGRAM) quadra.res $@/Contents/Resources

clean-local:
	-rm -rf Quadra.app

install-exec-hook: Quadra.app
	$(MKDIR_P) $(DESTDIR)/Applications
	cp -R $^ $(DESTDIR)/Applications

uninstall-hook:
	rm -rf $(DESTDIR)/Applications/Quadra.app
endif
