# Berkeley-local stuff
# $Id$

if (-x /usr/bin/X11) set path = ($path /usr/bin/X11)

# Get software warehouse in the path
set path = ( $path /usr/sww/bin )

# Modify the printer variable
setenv PRINTER sp524

# For FrameMaker
setenv FMHOME /usr/sww/frame-4.0X
setenv FM_FLS_HOST mho.eecs.berkeley.edu
setenv FM_FLS_AUTO /usr/cluster/adm/frame-4.0/fm_fls_auto
set path = ($path $FMHOME/bin)

# HPPA needs these
if ( "$ARCH" == hppa || "$ARCH" == "hppa.cfront" ) then
	setenv PTX11DIR /usr/sww/X11
	setenv PT_DISPLAY "xterm -e vi %s"
endif
