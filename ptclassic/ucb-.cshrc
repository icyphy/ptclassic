# Berkeley-local stuff
# $Id$

if (-x /usr/bin/X11) set path = ($path /usr/bin/X11)
# Make files group writable.  The .cshrc we distribute should not
# have umask set to 2, it should be set to 22.
umask 02


# Get kerberos in path before sun rsh and kinit
# Get software warehouse in the path, however this will mean that
#  gcc uses gnm from sww/bin, so when building a ptolemy to distribute
#  don't include gcc in the path
set path = ( /usr/kerb.local/bin \
		$path \
		/usr/sww/bin )

if ( "$PTARCH" == sol2 || "$PTARCH" == "sol2.cfront" ) then
	# Needed for SUN CC, may interfere with Synopsys
	setenv LM_LICENSE_FILE /opt/lm/lmgrd.key

	# Synopsys needs these
	setenv SYNOPSYS /usr/tools/synopsys
	setenv SIM_ARCH sparcOS5
	# vhdlan which is run in utils/ptvhdlsim requires that $ARCH be set
	setenv ARCH $PTARCH

	# Get Synopsys in path
	set path = ( $path \
		$SYNOPSYS/$SIM_ARCH/syn/bin \
                $SYNOPSYS/$SIM_ARCH/sim/bin )
endif

# Modify the printer variable
setenv PRINTER sp524

# For FrameMaker
setenv FRAMEUSERSD_HOST mho.eecs.berkeley.edu
setenv FMHOME /usr/cluster/frame
setenv FM_FLS_HOST mho.eecs.berkeley.edu
set path = ($path $FMHOME/bin /usr/sww/urt/bin)


if ( "$PTARCH" == hppa || "$PTARCH" == "hppa.cfront" ) then
	# HPPA needs these
	setenv PTX11DIR /usr/sww/X11
	setenv PT_DISPLAY "xterm -e vi %s"
else
	# For Purify: allow 128 files to be open at the same time
	# HPPA does not have a limit command
	limit descriptors 128
endif

# Only include /usr/tools/bin in our path if we are running under Solaris
# otherwise the sun4 build will fail because /usr/tools/mathematica is Solaris
if ( $PTARCH =~ sol?* ) set path = ($path /usr/tools/bin)


# Needed for s56x demos
setenv S56DSP /users/ptdesign/vendors/s56dsp
setenv QCKMON qckMon5
setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${S56DSP}/lib
