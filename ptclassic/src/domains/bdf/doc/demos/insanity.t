.\" $Id$
.NA insanity
.SD
This peculiar system applies two functions, log and cosine,
but the order of application is chosen at random.
The BDF clustering algorithm fails to complete on this graph.
If the "allowDynamic" parameter of the target is set to YES,
then the scheduler will construct four SDF subschedules, which
must then be invoked dynamically.
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
