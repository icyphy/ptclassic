.\" $Id$
.EQ
delim off
.EN
.NA serialAdd
.SD
The use of full adders, serial-to-parallel, and parallel-to-serial  blocks.
.LO "$PTOLEMY/src/domains/vhdlb/demo"
.DM "VHDLB Universe"
.SV 1.1 1/15/94
.AL "Michael C. Williamson"
.EQ
delim $$
.EN
.LD
In this demonstration universe, the addition of two 4-bit serial
values is accomplished by using a
.c Ser2Par
star, four
.c fullAdder
galaxies, and a
.c Par2Ser
star.
The two values (trivially generated here by a clock star) arrive
consecutively and are parallelized for the addition operation.
The result is then serialized.
The clock periods are set to allow enough time for the result to stabilize
at the inputs to the
.c Par2Ser
star.
The principle here is the mixed use of both behaviorally-specified stars
and structurally-specified galaxies to accomplish the overall function.
.ES
