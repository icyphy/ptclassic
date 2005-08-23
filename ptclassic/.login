#! /bin/csh
# sample ptolemy .login
# @(#).login	1.11	05/13/97

set noglob noclobber filec

if ($?PTARCH) then
   if ("$PTARCH" != "hppa") then
	   # Under Solaris, '/bin/stty dec' fails, '/usr/ucb/stty dec' works
	   # stty dec
	   set mail=/usr/spool/mail/$USER
   else
	   stty kill ^U
	   stty intr ^C
	   stty erase '^H'
   endif
endif

# Under solaris tset is not in the default path
set path = ($path /usr/ucb)

eval `tset -s -Q -m 'xterm:xterm' -m su:\?$TERM sun:sun \?$TERM`

if (! $?DISPLAY) then
	echo -n "DISPLAY? "
	setenv DISPLAY $<
endif

source .cshrc

if ($TERM == "sun") then
	# If we get here, we're on a bare Sun tube.  Fire up X.
	# To fire up OpenWindows instead one might do
	#
	# setenv OPENWINHOME /usr/openwin
	# exec $OPENWINHOME/bin/openwin
	exec Xrun
endif

date

