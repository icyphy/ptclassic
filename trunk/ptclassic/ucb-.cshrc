# Berkeley-local stuff
# $Id$

if (-x /usr/bin/X11) set path = ($path /usr/bin/X11)

# Get software warehouse in the path
set path = ( $path /usr/sww/bin )

# Modify the printer variable
setenv PRINTER sp524

# For FrameMaker
setenv FRAMEUSERSD_HOST mho.eecs.berkeley.edu
setenv FMHOME /usr/cluster/frame
setenv FM_FLS_HOST mho.eecs.berkeley.edu
set path = ($path $FMHOME/bin)

# HPPA needs these
if ( "$PTARCH" == hppa || "$PTARCH" == "hppa.cfront" ) then
	setenv PTX11DIR /usr/sww/X11
	setenv PT_DISPLAY "xterm -e vi %s"
endif


# Needed for SUN CC, may interfere with Synopsys
setenv LM_LICENSE_FILE /opt/lm/lmgrd.key
