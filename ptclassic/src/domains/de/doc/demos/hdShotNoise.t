.\" $Id$
.NA hdShotNoise
.SD
Generates a high density shot noise process.
.DE
.LO "~ptolemy/src/domains/de/demo"
.SV $Revision$ "October 22, 1990"
.AL "E. A. Lee"
.LD
.IE Filter
.IE Discard
.IE Xhistogram
This system generates shot noise the same way that the \fIshotNoise\fR
demo does, but the arrival rate of the Poisson process is much higher
compared to the rate at which the shot noise is sampled.
According to the theory, high density shot noise is approximately Gaussian
in distribution.
This demo verifies this by plotting a histogram of the samples of the process.
Initial transients are discarded using the
.c Discard
block.
.IE "shot noise"
.SA
Discard,
Filter,
ShotNoise,
Poisson,
Xhistogram.
.ES
