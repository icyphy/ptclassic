.\" $Id$
.NA broken
.SD
This is an example of an inconsistent SDF system.
It fails to run, generating an error message instead.
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ $Date$
.AL "J. T. Buck"
.EQ
delim $$
.EN
.LD
This is an example of an SDF universe that cannot be run because
of sample rate inconsistencies.
.Ie "inconsistent SDF system"
.Ie "sample rate inconsistencies"
The
.c Add
star requires a single input sample on both input arcs, but because of the
.c UpSample
star, its input arcs have different sample rates.
Because of this, generating an SDF schedule is impossible because
one arc will accumulate an infinite number of samples.
.ES
