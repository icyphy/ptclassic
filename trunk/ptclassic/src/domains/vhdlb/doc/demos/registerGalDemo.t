.\" $Id$
.EQ
delim off
.EN
.NA registerGal
.SD
An example of the use of hierarchy in VHDL code generation.
.DE
.LO "$PTOLEMY/src/domains/vhdlb/demo"
.DM "VHDLB Universe"
.SV 1.1 1/15/94
.AL "Michael C. Williamson"
.EQ
delim $$
.EN
.LD
This demo generates code which specifies two levels of hierarchy.  At
the top visible level, the universe contains two
.c Clock stars and a
.c BlackHole
star connected to a galaxy.
Within the galaxy is a single
.c Register
star and input and output arrows.
The VHDL code generation proceeds by first specifying the REGISTERGAL
design entity and then instantiating it into the top-level universe design.
For hierarchical universes, the generated VHDL code reflects the same
hierarchy, rather than first flattening the design and then specifying a large
connectivity list.
This is intended to make the generated code more readable and more manageable.
.ES
