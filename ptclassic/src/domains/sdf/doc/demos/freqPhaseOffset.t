.\" $Id$
.NA freqPhaseOffset
.SD
Impose frequency jitter and phase offset on a sinusoid.
.DE
.LO "~ptolemy/src/domains/sdf/demo"
.DM SDF Universe
.Ir "frequency offset"
.Ir "phase jitter"
.SV $Revision$	$Date$
.AL "E. A. Lee"
.LD
A 400 Hz sinusoid, sampled at 8kHz, is generated.
On the upper path, 15 degrees (peak) phase jitter is
imposed on the sinusoid, and the result displayed using overlaid
traces, so that the jitter can be easily seen.
On the lower path, frequency offset of -200 Hz is imposed
on the signal, resulting in a 200 Hz sinusoid.
The "freqPhase" galaxy is used to accomplish this.
.Ie freqPhase
.SA
freqPhase
Xscope
.ES
