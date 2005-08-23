#
# Add the SGI's magically-appearing catman volumes and directories,
# a multiyear project of Paul Raines (raines@bohr.physics.upenn.edu),
# Craig Ruff (cruff@ncar.ucar.edu), and Scott Nelson (nelson18@llnl.gov)
#

# New users (those without ~/.tkman startup files) pick up these
# settings automatically.  The settings can also be interpolated
# into an existing startup file manually.  Unless you have numerous
# highlights and shortcuts, it probably wouldn't hurt to replace
# the existing startup file if interpolation would be too laborious.


set man(buttbg) gray75
set man(guibg) gray90
set man(textbg) white
set man(manList) {1 2 3 4 5 6 7 8 l o n p D}
set man(manTitleList) {
	"User Commands"  "System Calls"  Subroutines  Devices  "File Formats" 
	Games  Miscellaneous  "System Administration"  Local  Old  New  Public
	"Device Drivers"
}
set man(print) {tbl | eqn | gtroff -Tpsc | psdit | lpr}

manDot


### your additions go below

# If you don't want a million volumes popping up in the list, simply delete
# the line that adds that volume.  The man pages in that volume will be
# grouped with the next most specific volume.  For instance, if 1M doesn't
# exist, those pages are placed in volume 1; likewise if a 3Xm doesn't exist
# but an Xm group does.

manDescAddSects {
	{1L  "Commands (Local)"}
	{1P  "Commands (Programmer)"}
	{1M  "Commands (Admin)"}
	{1X  "Commands (X11)"}
	{1Xm "Commands (Xm)"}
	{1Iv "Commands (Inventor)"}
	{1ex "Commands (explorer)"}
	{1dm "Commands (dmedia)"}
} after 1

manDescAddSects {{2f "System Calls (ftn)"}} after 2

manDescAddSects {
	{3dm  "Subroutines (Digital Media)"}
	{3L   "Subroutines (local)"}
	{3X   "Subroutines (X11)"}
	{3Xt  "Subroutines (Xt)"}
	{3Xm  "Subroutines (Motif)"}
	{3Xi  "Subroutines (Xirisw)"}
	{3Xo  "Subroutines (misc X)"}
	{3XS  "Subroutines (X SGI)"}
	{3Vk  "Subroutines (ViewKit)"}
	{3cc  "Subroutines (C++)"}
	{3Iv  "Subroutines (Inventor)"}
	{3ex  "Subroutines (explorer)"}
	{3ogl "Subroutines (OpenGL)"}
	{3gl  "Subroutines (GL)"}
	{3pex "Subroutines (pex)"}
	{3f   "Subroutines (ftn)"}
	{3l   "Subroutines (libblas)"}
	{3il  "Subroutines (il)"}
	{3comp "Subroutines (comp)"}
	{3n   "Subroutines (nonstandard)"}
	{3lib "Subroutines (lib)"}
	{3m   "Subroutines (misc)"}
} after 3

manDescAddSects {
	{4L   "Devices (Local)"}
	{4dm  "Devices (Digital Media)"}
	{4ex  "Devices (Explorer)"}
} after 4

manDescAddSects {
	{5L  "File Formats (Local)"}
	{5Xm "File Formats (Motif)"}
} after 5

manDescAddSects {{7L  "Miscellaneous (Local)"}} after 7

manDescAddSects {{8L  "System Administration (Local)"}} after 8

manDescAddSects {{gnu "GNU stuff"}}



# All directories in the catman hierarchy are matched against
# the volume-pattern pairs below.  If there is a match for the
# directory in the same numerical volume, then the directory is
# grouped with that volume.  Otherwise, a match in any volume
# is taken.  If the directory doesn't match any pattern, it
# is placed in next most specific grouping.  If you edit this list,
# place more general groupings after more specific ones.

manDescSGI {
	{ 1P /p_man/*1* }
	{ 1M /a_man/*1* }
	{ 2 2/standard }
	{ 3dm 3dm/* }
	{ 3Xo {Xau Xext Xi Xnmbx} }
	{ 3XS Sg* }
	{ 3Vk Vk }
	{ 3cc c++ }
	{ 3l libblas }
	{ 3ogl {OpenGL widget} }
	{ 3 p_man/*3*/standard }
	{ 3gl 3/standard }
	{ 3n 3/nonstandard }
	{ 3il il }
	{ 3Xi Xirisw }
	{ 3lib lib }
	{ 3m {3b 3c 3g 3n 3p 3s 3t 3w 3x}}
	{ 3pex pex }
	{ 3comp comp }

	{ f ftn }
	{ Iv Inventor }
	{ ex explorer }
	{ dm {dmedia audio video movie midi} }
	{ X {X X11} }
	{ L {/local* /alt*} }
	{ Xt Xt }
	{ Xm Xm }
	{ gnu gnu }
}
