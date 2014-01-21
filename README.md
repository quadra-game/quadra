Quadra Source Release
=====================

This is the Quadra source release.

Requirements
------------

 - a standard-compliant C++ compiler (gcc 4.0 was tested on Linux and
   Visual C++ 2008 on Windows)
 - zlib 1.2 or greater (http://www.info-zip.org/pub/infozip/zlib/)
 - libpng 1.2.0 or greater (http://www.libpng.org/pub/png/libpng.html)
 - SDL 1.2.12 or greater (http://www.libsdl.org/)

On Ubuntu, the following packages are needed to build:

 - g++
 - libpng12-dev
 - libsdl1.2-dev
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

To compile Quadra on a Windows machine, look for the folder 'VisualC++'
and open up 'quadra_sdl.sln' (tested using Visual C++ 2005 Express
Edition). Go in menu Tools / Options, and select Projects and Solutions,
VC++ Directories. Make sure you have zlib, libpng and SDL paths in both
'include files' and 'library files'.
I have tested with the following version:
  zlib-1.2.3
  libpng-1.2.12 (see below for build instruction)
  SDL-1.2.13
(note: on my system, I have added these paths to include files:
	C:\work\zlib-1.2.3
	C:\work\libpng-1.2.12
	C:\work\SDL-1.2.13\include

	and these paths to library files:
	C:\work\libpng-1.2.12\projects\visualc71\Win32_LIB_Release
	C:\work\zlib-1.2.3\projects\visualc6\Win32_LIB_Release
	C:\work\SDL-1.2.13\lib
Your setup should be relatively similar.)

By default, your workspace should be set to 'Debug'. Build the solution.
Fix any missing lib or includes if necessary. If all builds, don't 
launch the app yet. You now need to build the main ressource file using
'wadder', an utility that was built in the Quadra solution. You can
launch this utility through a batch file called 'res.bat' found in the
same folder as the VC++ solution. If the tool works properly, it should
output a 'quadra.res' in the parent folder, along with 'quadra.exe' which
*should* already be there (simply by building the solution).

Important notice: For a "more" enjoyable debugging/developping experience,
I recommend you pass a command argument '-nofullscreen' in VC properties
under Configuration Properties / Debugging. Else you might end up getting
stuck on a breakpoint in a 640x480x8 bit display mode with a locked 
keyboard.

Run "res.bat" after building to create the resource file (quadra.res).

How to build LIBPNG on Windows
------------------------------
I am using libpng-1.2.12 but newer version should work mostly the same way.
Open up libpng-1.2.12\projects\visualc71\libpng.sln. Select your current
configuration as 'LIB Release' in order to compile a .lib that Quadra will
link to. Edit the project setting (configuration LIB Release). In 
'Configuration Properties' / 'C/C++' / 'Code Generation', make sure that
'Runtime Library' is set to 'Multi-Thread (/MT)'. Build.

How to build ZLIB on Windows
----------------------------
I am using zlib-1.2.3 and like libpng, newer version should work too. Open
up zlib-1.2.3\projects\visualc6\zlib.sln (or zlib.dsw if the solution does
not exist, and Visual Studio will ask to create a solution). Select the 
configuration as 'LIB Release', Edit the project setting. In 
'Configuration Properties' / 'C/C++' / 'Code Generation', make sure that
'Runtime Library' is set to 'Multi-Thread (/MT)'. Build.

Any questions?
--------------

If you have any problem, patches, suggestions, bug reports or are
simply looking for help, please take advantage of the resources
available on our web site (http://quadra.googlecode.com/)! Among
others, we have a support request system, mailing lists, a bug
tracking system and plenty of other goodies, check it out!

Good luck, and have fun!

Building on Mac OS X
--------------------

A common problem on Mac OS X is that the libpng library is not
available. This is relatively easy to work around, by downloading the
libpng sources (available at http://libpng.sourceforge.net/, look for
a libpng-1.2.X.tar.gz file), compiling them (no need to install it on
your system!) and then adding a "--with-libpng=/path/to/libpng" to the
"configure" command line.

FIXME...

Mac OS X Leopard:

./configure --enable-debug CPPFLAGS=-I/usr/X11/include LDFLAGS=-L/usr/X11/lib SDL_CFLAGS=-I$HOME/Library/Frameworks/SDL.framework/Headers SDL_LIBS="-F$HOME/Library/Frameworks -framework SDL -framework Cocoa"

Mac OS X pre-Leopard:

./configure --enable-debug CPPFLAGS=-I$HOME/src/libpng-1.2.10 LDFLAGS=-L$HOME/src/libpng-1.2.10 SDL_CFLAGS=-I$HOME/Library/Frameworks/SDL.framework/Headers SDL_LIBS="-F$HOME/Library/Frameworks -framework SDL -framework Cocoa"

