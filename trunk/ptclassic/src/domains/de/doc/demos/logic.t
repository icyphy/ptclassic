.\" $Id$
.NA logic
.SD
Demonstrate the use of the
.c Logic
star in its various instantiations as AND, NAND, OR, NOR, XOR, XNOR
and inverter gates.
The three test signals consist of square waves with periods 2, 4, and 6.
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/de/demo"
.EQ
delim $$
.EN
.SV $Revision$ $Date$
.AL "Edward A. Lee"
.LD
.Se Logic DE
The most recent arrival on each input is used.
This has the effect of a zero-order hold on each input:
a value persists until a new event sets a new value. 
.SA
Test.
.ES
