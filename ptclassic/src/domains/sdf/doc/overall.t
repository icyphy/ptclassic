.\" $Id$
.VR 0.$Revision$ "October 15, 1990"
.TI "SDF Domain"
.AU
Edward A. Lee
.AE
.H1 "Introduction"
.pp
SDF is a data-driven, statically scheduled domain in \*(PT.
Stars must all follow the basic SDF principle,
that the number of tokens consumed or produced on any
.c PortHole
does not change while the simulation runs.
These numbers are given for each porthole
as part of the star definition.
.H1 "The Scheduler"
.pp
The SDF scheduler determines the order of execution of stars
in a system at start time.
.H2 "Number of iterations"
.H2 "Inconsistencies"
.H1 "Writing SDF stars"
.H1 Particles
.H1 "EventHorizon"
