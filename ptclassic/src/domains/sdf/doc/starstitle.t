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
$PTOLEMY/src/domains/sdf/dsp/stars:
Stars implementing digital signal processing and communication
functions.
.bu
$PTOLEMY/src/domains/sdf/image/stars:
Stars that perform image processing functions.
.pp
By "standard" we mean the stars that are owned by \*(PT.
Because of their large number, their icons are divided into
sub-palettes.  When you open the main SDF palette in pigi,
you will see one icon representing each subpalette.
Look inside one of these icons, and you will see the icons
representing the stars in the subpalette.
The main palette is identified
as $PTOLEMY/src/domains/sdf/icons/main.pal.
The sub-palettes are:
.Id "SDF main palette"
.Id "palette, SDF main"
.Id "main palette, SDF"
.bu
.Id "SDF signal sources palette"
.Id "palette, SDF signal sources"
.Id "signal sources palette, SDF"
\fBsignal sources\fR: Stars with no inputs that generate signals.
.bu
.Id "SDF signal sinks palette"
.Id "palette, SDF signal sinks"
.Id "signal sinks palette, SDF"
\fBsignal sinks\fR: Stars with no outputs used for graphing
or printing signals or "playing" them to the workstation speaker.
.bu
.Id "SDF arithmetic palette"
.Id "palette, SDF arithmetic"
.Id "arithmetic palette, SDF"
\fBarithmetic\fR:
Basic arithmetic operations.
.bu
.Id "SDF nonlinear functions palette"
.Id "palette, SDF nonlinear functions"
.Id "nonlinear functions palette, SDF"
\fBnonlinear functions\fR:
Mathematical function operators, such as trigonometric functions.
.bu
.Id "SDF control palette"
.Id "palette, SDF control"
.Id "control palette, SDF"
\fBcontrol\fR:
Stars such as fork, upsample,
downsample, distributor, that manipulate the flow of particles.
All the stars in this palatte operate on
.c ANYTYPE
of particle.
.bu
.Id "SDF conversion palette"
.Id "palette, SDF conversion"
.Id "conversion palette, SDF"
\fBconversion\fR:
Type conversion functions, such as real
to complex and bits to integer.
.bu
\fBdsp\fR:
Signal processing functions.
.Id "SDF dsp palette"
.Id "palette, SDF dsp"
.Id "dsp palette, SDF"
.bu
\fBimage\fR:
Image display and processing.
.Id "SDF image palette"
.Id "palette, SDF image"
.Id "image palette, SDF"
.bu
.Id "SDF communications palette"
.Id "palette, SDF communications"
.Id "communications palette, SDF"
\fBcommuncations\fR:
Communication functions such as channel models, modulators,
and encoders.
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
