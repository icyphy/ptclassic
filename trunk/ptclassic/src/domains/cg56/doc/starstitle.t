.\" $Id$
.EQ
delim off
.EN
.TI "CG56 Stars"
.pp
This section contains generic 56000 code generation stars.  They will
work for all available targets, unless otherwise stated in the
star documentation.
.pp
The source code for all supplied CG56 stars are found in the 
$PTOLEMY/src/domains/cg56/stars and $PTOLEMY/src/domains/cg56/dsp/stars 
directories. As 
in the SDF domain, the star icons are divided into many sub-palettes each 
accessible through the ``main'' palette accessible in the pigi palette 
menu. The ``main'' palette is identified as 
$PTOLEMY/src/domains/cg56/icons/main.pal, and contains the sub-palettes:
.Id "CG56 main palette"
.(c
Signal Sources
I/O
Arithmetic
Nonlinear Functions
Control
Signal Processing
.)c
In this section, all the stars found in the above palettes are documented.
.pp
Note that any star library in \*(PT should be viewed as a set of examples, 
not as an all inclusive set. Adding new stars is easy, so the library tends 
to contain only stars for which we have encountered a need. Most users will 
want to add their own. 
.EQ
delim $$
.EN
