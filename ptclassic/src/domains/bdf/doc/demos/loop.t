.\" $Id$
.NA loop
.SD
This system illustrates the classic dataflow mechanism
for implementing data-dependent iteration (a do-while).
A sequence of integers (a ramp) is the overall input.
Each input value gets multiplied by 0.5 inside the loop
until its magnitude is smaller than 0.5.
Then that smaller result is sent to the output.
.DE
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/bdf/demo"
.EQ
delim $$
.EN
.SV $Revision$ $Date$
.AL "J. T. Buck"
.ES
