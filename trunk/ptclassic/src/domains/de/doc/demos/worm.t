.\" $Id$
.NA worm
.SD
This demo shows how easy it is to use the SDF stars to perform
computation on DE particles.
A Poisson process where particles have value 0.0 is sent into an
SDF wormhole, where Gaussian noise is added to the samples.
.DE
.LO "$PTOLEMY/src/domains/de/demo"
.SV $Revision$ $Date$
.AL "E. A. Lee"
.LD
The overall delay of the SDF wormhole is zero, so the result
is simply Poisson arrivals of Gaussian noise samples.
.SA
Poisson,
IIDGaussian.
.ES
