.\" $Id$
.TI "SDF Stars and Galaxies"
.pp
.EQ
delim off
.EN
The source code for all standard SDF stars is in one of three
directories:
.bu
$PTOLEMY/src/domains/sdf/stars:
The general-purpose stars.
Because there are so many, these stars are divided into
many sub-palettes, as described below.
.bu
$PTOLEMY/src/domains/sdf/atm/stars:
Stars implementing asychronous transfer mode 
functions.
.bu
$PTOLEMY/src/domains/sdf/dsp/stars:
Stars implementing digital signal processing and communication
functions.
.bu
$PTOLEMY/src/domains/sdf/image/stars:
Stars that perform image processing functions.
.bu
$PTOLEMY/src/domains/sdf/matrix/stars:
Stars that perform matrix processing functions.
.bu
$PTOLEMY/src/domains/sdf/tcltk/stars:
Stars that use the tcl interpreter and tk windows.
.pp
By "standard" we mean the stars that are owned by \*(PT.
Because of their large number, their icons are divided into
sub-palettes.  When you open the main SDF palette in pigi,
you will see one icon representing each subpalette.
Look inside one of these icons, and you will see the icons
representing the stars in the subpalette.
.pp
All galaxy definitions are stored in
$PTOLEMY/src/domains/sdf/demo, but since they will almost always
be accessed through a palette, this should not matter to a user.
Below all stars and galaxies (blocks)
in any of these palettes are listed.
The location field (usually) indicates the palette in which the
block can be found.
.pp
Note that any star library in \*(PT should be viewed as a set of
examples, not as an all inclusive set.
Adding new stars is easy, so the library tends to contain only
stars for which we have encountered a need.
Many users will want to add their own.
.EQ
delim $$
.EN
.bp
