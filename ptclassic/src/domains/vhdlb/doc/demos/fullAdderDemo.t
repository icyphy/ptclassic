.\" $Id$
.EQ
delim off
.EN
.NA fullAdder
.SD
The use of a full adder block constructed from basic components.
.LO "$PTOLEMY/src/domains/vhdlb/demo"
.DM "VHDLB Universe"
.SV 1.1 1/15/94
.AL "Michael C. Williamson"
.EQ
delim $$
.EN
.LD
This demo generates code which specifies a full adder from individual gates.
Note the use of
.c Through
stars inside the FULLADDER galaxy to avoid fanning out signal lines
directly connected to galaxy input terminals.
The gate logic implements the sum result output and
carry-out output functions using 2-input and 3-input logic gates.
Similar methods can be used to build up multi-bit adders or
arithmetic-logic units as desired.
If these larger design units become too unwieldy to draft by hand,
then behaviorally-specified designs can be used where the functions
are accomplished in behavioral VHDL code.
.ES
