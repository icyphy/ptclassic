.\" $Id$
.NA distortionQ
.SD
Simulates the effect on a sinusoid of random delay and reordering of samples.
.DE
.LO "~ptolemy/src/domains/de/demo"
.SV $Revision$ "October 15, 1990"
.AL "Edward A. Lee"
.LD
This demo is very similar to the
.c distortion
demo.
The only difference is in the
.c reorderQ
wormhole, which simulates a simple packet switched communication network.
Unlike the
.c distortion
demo, this one has a
.c Queue
before the
.c Sampler.
After transients have died out,
the queue greatly reduces the
likelihood of distortion due to ``reuse'',
described in the explanation of the distortion demo.
The cost is a small increased delay in the output.
You can experiment with limitations on the size of the queue,
but notice that even a queue with capacity 2 produces
a far better signal than that in the distortion demo.
The ``reorder'' distortion is still present.
.SA
distortion,
Router,
ExpDelay,
Sampler.
.ES
