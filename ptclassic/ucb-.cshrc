# Berkeley-local stuff
# $Id$

if (-x /usr/bin/X11) set path = ($path /usr/bin/X11)

# Get software warehouse in the path
set path = ( $path /usr/sww/bin )

# Modify the printer variable
setenv PRINTER sp524
