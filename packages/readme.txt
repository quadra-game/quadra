
Quadra version @VERSION@
http://quadra.sourceforge.net/

Description
-----------

This is the official release @VERSION@ of Quadra, a full-featured
multiplayer action puzzle game for the X Window System and Svgalib.
Features include:

 - Recursive block chaining
 - Block shadow
 - Team play
 - TCP/IP networking (free Internet play! Supports SOCKS proxies)
 - Smooth block falling
 - Sound effects
 - Watches on other players
 - Chat window
 - CD-based music
 - And much more!

Installation
------------

Run the install.sh shell script as the root user, like this:

cd quadra-@VERSION@
su
sh install.sh

If you do not have root access on your machine, you can still install
Quadra in your home directory. In this case, you have to set the
QUADRADIR environment variable to the directory that contains the
'quadra.res' resource file. For example:

export QUADRADIR=$HOME/quadra-@VERSION@ (for a Bourne shell like bash)

or

setenv QUADRADIR $HOME/quadra-@VERSION@ (for a C shell like csh)

You can also transform the quadra-@VERSION@.tar.gz package into a RPM
package. You need a working RPM environment (the root user on a Red
Hat system already has such an environment), then it is simply a
matter of entering this command:

rpm -tb quadra-@VERSION@.tar.gz

For further support questions, refer to the support section of the
Ludus Design web site at http://ludusdesign.com/support.html or e-mail
us at support@ludusdesign.com, including all relevant information,
like error messages, computer model, video and sound card models and
operating system version.

Have fun playing Quadra!

