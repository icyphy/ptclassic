#
# calculate standard SCO UNIX modifications,
# based on information from Bela Lubkin
#

# New users (those without ~/.tkman startup files) pick this up
# automatically.  The code can also be interpolated into an existing
# startup file manually.


### your additions go below

set manx(dirnameindex) 4; # should be set already, but just in case

# Compute sections based on system settings.
set man(manList) ""
set man(manTitleList) ""

set manx(sco-master) {
	{1 "User Commands"} {3 "Functions"} {4 "File Formats"} {5 Support Files"} {7 "Packages"}
	{A "Administration"}
	{ADM "Operating system administration"}
	{ADMN "TCP/IP administration commands"}
	{ADMP "TCP/IP network protocols"}
	{C "Operating system commands, utilities"}
	{C++ "C++ library"}
	{CDMT "Custom Distribution Mastering Toolkit"}
	{CP "Programming commands"}
	{F "File formats"}
	{FP "Programming file formats"}
	{G Games}
	{H "Hardware and Drivers"}
	{HW "Device drivers, hardware"}
	{L "Local Commands"}
	{LMC "LAN Manager Client commands"}
	{M "Miscellaneous"}
	{MERGE "SCO Merge commands"}
	{NADM "NFS network administration commands"}
	{NC "NFS network user commands"}
	{NET "TLI/XTI transport stack library"}
	{NF "NFS file formats"}
	{NS "NFS, NIS, RPC, XDR network library"}
	{NWG "SCO Gateway for NetWare commands"}
	{PADM "IPX/SPX commands"}
	{PCI "PC-Interface commands"}
	{S "Operating system calls, library"}
	{SAPD "DCE SAP library"}
	{SFF "TCP/IP file formats"}
	{SLIB "Socket library"}
	{SSC "Socket system calls"}
	{TC "TCP/IP network user commands"}
	{TCL "SCO Visual Tcl commands"}
	{WABI "SCO Wabi commands"}
	{X "X server commands"}
	{XC "X client commands"}
	{Xext "X Extensions library"}
	{Xm "OSF/Motif commands, library"}
	{Xmu "X library (miscellaneous utilities)"}
	{XS "X library"}
	{Xt "X Toolkit Intrinsics library"}
}

# build default list based on ORDER= in /usr/default/man
set scolist ""
set manx(sco-config) "/etc/default/man"
#set manx(sco-config) "tmp"
if [file readable $manx(sco-config)] {
	set fid [open $manx(sco-config)]
	while {![eof $fid]} {
		set line [gets $fid]
		if [regexp  "^\[ \t]*ORDER\[ \t]*=\[ \t]*(.*)" $line all tmp] {
			set scolist $tmp
			# want last ORDER= line so don't break
		}
	}
	close $fid

	if {$scolist!=""} {
		# build list in given order
		foreach i [split [string trim $scolist] ":"] {
			if {[lsearch $man(manList) $i]==-1} {	# no duplicates
				lappend man(manList) $i
				if {[set title [lassoc $manx(sco-master) $i]]==""} { set title $i }
				lappend man(manTitleList) $title
			}
		}
	}
}

# if can't find ORDER, build up default list from master list
if {$scolist==""} {
	foreach i $manx(sco-master) {
		lappend man(manList) [lfirst $i]
		lappend man(manTitleList) [lsecond $i]
	}
}
