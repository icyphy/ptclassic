.\" $Id$
.NA worm
.SD
Noisy Poisson process \(em Wormhole demo
.DE
.LO "~ptolemy/src/domains/de/demo"
.SV $Revision$ "October 22, 1990"
.AL "E. A. Lee"
.LD
A Poisson process where particles have value 0.0 is sent into an
SDF wormhole, where Gaussian noise is added to the samples.
This demo shows how easy it is to use the SDF stars to perform
computation on DE particles.
The overall delay of the SDF wormhole is zero, so the result
is simply Poisson arrivals of Gaussian noise samples.
.SA
Poisson,
IIDGaussian.
.ES
