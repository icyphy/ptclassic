# Berkeley-local stuff
# $Id$

if (-x /usr/bin/X11) set path = ($path /usr/bin/X11)

# Get Gnu tools for mips from Moby.

if ($ARCH == mips) set path = ( $path /usr/sww/bin )
