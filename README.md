Quadra Source Release
=====================

This is the Quadra source release.

Requirements
------------

 - a C++ compiler (gcc was tested on Linux and Visual C++ on Windows)
 - zlib (http://www.info-zip.org/pub/infozip/zlib/)
 - libpng (http://www.libpng.org/pub/png/libpng.html)
 - DirectX 5 (Windows only)
 - X Window System libraries with XPM support (Linux only)

On Ubuntu, the following packages are needed to build:

 - g++
 - libpng12-dev
 - libx11-dev
 - libxpm-dev
 - libxxf86vm-dev
 - make

Auto-update feature
-------------------

Some users have expressed privacy and performance concerns about the
auto-update feature. This auto-update is implemented by fetching a small file
using HTTP, no more than once per day. The information sent to the server is
very limited: your IP address (from the connection itself) and the Quadra
version you are currently using (from the HTTP User-Agent header). At the time
of this writing, the Quadra team does not even have access to the logs where
this information is stored (this file is hosted by Google Code). In the
interest of full disclosure, similar information is sent to Qserv when using
the Internet multi-player mode, with a few more technical informations (such
as the platform and the display driver in use), and this information is
accessible by the Quadra team (not used for anything at the moment, but could
be used to gauge the interest for support of specific platforms). We will not
release the specifics of any information collected, only aggregate (for
example, the percentages of users using Linux or Mac OS X), if ever.

This auto-update feature serves two purposes. One is to inform the user of new
versions of Quadra, which might have important security or bug fixes. The
other is to get the URL to use as the default Qserv address. The latter is so
that we have some flexibility in moving Qserv (which has been a problem in the
past, with downtime for Internet players). Fetching this file is done in the
background, and does not delay starting up the game.

Packagers which provide automatic updating (such as Ubuntu and openSUSE) can
disable the new version notification (with the --disable-version-check
configure option), as it would be redundant and unhelpful to the user. Note
that this does NOT disable fetching the file, as it is still necessary for
obtaining the Qserv address.

Building on Unix-style systems
------------------------------

To compile and install this software on a Unix-like system, do this:

$ make (if you checked out the source code from Subversion)
$ ./configure
$ make
$ make install (optional)

There you go! There is a 'quadra' binary and a 'quadra.res' resource
file in the top-level directory. If not, then something went
wrong. :-)

You can run this with the following command (in bash):

QUADRADIR=. ./quadra

If you ran the "make install" command, then you have 'quadra'
installed on your system and you can just run it (no need to set
QUADRADIR).

Building on Windows
-------------------

To compile Quadra on a Windows machine, use the Visual C++ workspace
and project file in the VisualC++ subdirectory. Make sure you have
zlib, libpng and DirectX (August 2007 SDK or earlier) properly installed
for development on your machine. Run "res.bat" after building to create
the resource file (quadra.res).

Building on Mac OS X
--------------------

A common problem on Mac OS X is that the libpng library is not
available. This is relatively easy to work around, by downloading the
libpng sources (available at http://libpng.sourceforge.net/, look for
a libpng-1.2.X.tar.gz file), compiling them (no need to install it on
your system!) and then adding a "--with-libpng=/path/to/libpng" to the
"configure" command line.

This platform is not supported in this release.

Any questions?
--------------

If you have any problem, patches, suggestions, bug reports or are
simply looking for help, please take advantage of the resources
available on our web site (http://quadra.googlecode.com/)! Among
others, we have a support request system, mailing lists, a bug
tracking system and plenty of other goodies, check it out!

Good luck, and have fun!

