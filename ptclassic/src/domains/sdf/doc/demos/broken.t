.\" $Id$
.NA broken
.SD
This is an example of an inconsistent SDF system.
It fails to run, generating an error message instead.
.DE
.LO "~ptolemy/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ $Date$
.AL "J. Buck"
.LD
This is an example of an SDF universe that cannot be run because
of sample rate inconsistencies.
.Ie "inconsistent SDF system"
.Ie "sample rate inconsistencies"
The FloatSum star requires a single
input on both input arcs, but because of the UpSample star, its
input arcs have different sample rates.  Because of this, generating
an SDF schedule is impossible.
.ES
