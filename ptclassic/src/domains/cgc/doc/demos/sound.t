.\" $Id$
.NA sound
.SD
Generate a sound to play over the workstation speaker.
.SV $Revision$ $Date$
.AL "E. A. Lee"
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/cgc/demo"
.EQ
delim $$
.EN
.LD
.pp
.Id "workstation speaker"
.Ir "sound"
.Ir "audio, Sparcstation"
.Ir "Sparcstation audio"
This demo writes to the device /dev/audio a PCM encoded audio signal
and plays it in real time.
The audio signal generated is a 500 Hz sinusoid modulated by a 10 Hz sinusoid,
for a vibrato-like effect.
While in principle this should work on any workstation with a device
/dev/audio that accepts PCM encoded data, it has only been tested on
Sun Sparcstations.
.ES
