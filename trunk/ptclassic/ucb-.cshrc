# Berkeley-local stuff
# $Id$

if (-x /usr/bin/X11) set path = ($path /usr/bin/X11)
# Make files group writable.  The .cshrc we distribute should not
# have umask set to 2, it should be set to 22.
umask 02

# Get software warehouse in the path
set path = ( $path /usr/sww/bin )

# Modify the printer variable
setenv PRINTER sp524

# For FrameMaker
setenv FRAMEUSERSD_HOST mho.eecs.berkeley.edu
setenv FMHOME /usr/cluster/frame
setenv FM_FLS_HOST mho.eecs.berkeley.edu
set path = ($path $FMHOME/bin)

# For Purify: allow 128 files to be open at the same time
limit descriptors 128

# HPPA needs these
if ( "$PTARCH" == hppa || "$PTARCH" == "hppa.cfront" ) then
	setenv PTX11DIR /usr/sww/X11
	setenv PT_DISPLAY "xterm -e vi %s"
endif

# Only include /usr/tools/bin in our path if we are running under Solaris
# otherwise the sun4 build will fail because /usr/tools/mathematica is Solaris
if ( $PTARCH =~ sol?* ) set path = ($path /usr/tools/bin)


# Needed for SUN CC, may interfere with Synopsys
setenv LM_LICENSE_FILE /opt/lm/lmgrd.key
