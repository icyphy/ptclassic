.\" $Id$
.EQ
delim off
.EN
.NA fsmDemo
.SD
The use of a finite state machine constructed from basic components.
.DE
.LO "$PTOLEMY/src/domains/vhdlb/demo"
.DM "VHDLB Universe"
.SV 1.1 1/15/94
.AL "Michael C. Williamson"
.EQ
delim $$
.EN
.LD
This demo generates code which specifies a finite state machine made
from simple components such as individual gates, registers, and an
internal clock.
The FSM has four inputs, four outputs, and four state bit variables.
The generated code specifies the FSM, instantiates it, and specifies
the source and sink star components around it.
The internal logic can be rearranged to implement any desired FSM of
similar size, or it can be used as a model for building FSMs of
different sizes.
In the future, to avoid the labor of drawing the circuit by hand, a
star could be created which would take an FSM tabular specification as
input and then implement that FSM.
This would effectively be a programmable module, specified at the
behavioral level rather than the structural level presented here.
.ES
