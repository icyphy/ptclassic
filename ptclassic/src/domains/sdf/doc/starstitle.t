.\" $Id$
.TI "SDF Stars"
.pp
The source code for all standard SDF stars is in one of three
directories:
.bu
The general-purpose stars are in $PTOLEMY/src/domains/sdf/stars.
Because there are so many, stars with source code in this directory
may appear in many of the sub-palettes described below.
.bu
Stars implementing digital signal processing and communication
functions are in
$PTOLEMY/src/domains/sdf/dsp/stars.
.bu
Stars that perform image processing functions are in
$PTOLEMY/src/domains/sdf/image/stars.
.pp
By "standard" we mean the stars that are owned by \*(PT.
Because of their large number, their icons are divided into
sub-palettes.
The "main" palette is identified
as $PTOLEMY/src/domains/sdf/icons/main.pal, and contains
an icon for each of the sub-palettes.
.Id "SDF main palette"
.Id "palette, SDF main"
.Id "main palette, SDF"
.pp
.Id "SDF signal sources palette"
.Id "palette, SDF signal sources"
.Id "signal sources palette, SDF"
The "signal sources" sub-palette contains a variety of actors
with no inputs and a single output, that provide various types
of signals.
.pp
.Id "SDF signal sinks palette"
.Id "palette, SDF signal sinks"
.Id "signal sinks palette, SDF"
The "signal sinks" sub-palette contains stars for graphing
or printing signals or "playing" them to the workstation speaker.
.pp
.Id "SDF arithmetic palette"
.Id "palette, SDF arithmetic"
.Id "arithmetic palette, SDF"
The "arithmetic" palette contains basic arithmetic operations.
.pp
.Id "SDF nonlinear functions palette"
.Id "palette, SDF nonlinear functions"
.Id "nonlinear functions palette, SDF"
The "nonlinear functions" palette contains various mathematical
function operators, such as trigonometric functions.
.pp
.Id "SDF control palette"
.Id "palette, SDF control"
.Id "control palette, SDF"
The "control" palette contains actors such as fork, upsample,
downsample, distributor, that are defined independently of the
type of input data.
.pp
.Id "SDF conversion palette"
.Id "palette, SDF conversion"
.Id "conversion palette, SDF"
The "conversion" palette contains conversion functions: real
to complex, bits to integer, etc.
.pp
The "dsp" palette contains
more esoteric and specialized signal processing functions.
.Id "SDF dsp palette"
.Id "palette, SDF dsp"
.Id "dsp palette, SDF"
The "image" palette  contains
stars dedicated to image display and processing.
.Id "SDF image palette"
.Id "palette, SDF image"
.Id "image palette, SDF"
.pp
.Id "SDF communications palette"
.Id "palette, SDF communications"
.Id "communications palette, SDF"
The "communications" palette contains stars and galaxies with
communication applications.
.pp
In this section, all stars in any of these palettes are listed,
and the only distinction made between them is their "location".
.pp
Note that any star library in \*(PT should be viewed as a set of
examples, not as an all inclusive set.
Adding new stars is easy, so the library tends to contain only
stars for which we have encountered a need.
Many users will want to add their own.
