.\" $Id$
.TI "CGC Stars"
.pp
.EQ
delim off
.EN
The source code for all standard CGC stars is
in $PTOLEMY/src/domains/cgc/stars.
By "standard" we mean the stars that are owned by \*(PT.
Because of their large number, their icons are divided into
several palettes accessible inside the "main" palette,
following the SDF star organization.
The "main" palette is identified
as $PTOLEMY/src/domains/cgc/icons/main.pal, and accessible through
the pigi main menu. 
.pp
All CGC stars are designed to be compatible to the SDF stars in the
sense that a CGC stars has the same name and contain all the parameters
of the corresponding SDF star if exists. It allows the
programmer just to select the right domain for the intended purpose
(simulation or code generation), given a program graph (or ptcl file). 
Therefore, it is strongly recommended that this principle 
be followed when designing a new star.
.pp
Note that any star library in \*(PT should be viewed as a set of
examples, not as an all inclusive set.
Adding new stars is easy, so the library tends to contain only
stars for which we have encountered a need.
Most users will want to add their own.
.EQ
delim $$
.EN
