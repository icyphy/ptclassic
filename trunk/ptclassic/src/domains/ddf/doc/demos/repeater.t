.\" $Id$
.NA repeater
.SD
Demonstrates the Repeater star.
.DE
.LO "~ptolemy/src/domains/ddf/demo"
.SV $Revision$ $Date$
.AL "E. A. Lee"
.LD
.Se Repeater
This demo shows what the
.c Repeater
star does.
The
.c Repeater
star consumes one data input and one control input.
The value of the control input determines how many copies
of the input data are to be produced.  The
.c Repeater
star may be used to represent a data-dependent iteration 
construct.   In this demo, since the
.c Repeater 
star is not a SDF star, the SDF domain has a wormhole which internally
uses the DDF domain.  The inputs to the
.c Repeater
star are given with two ramp generators. The display
shows a staircase with growing flat length.
.SA
Repeater
.ES

