.\" $Id$
.NA fmplay
.SD
Sound generator using FM modulation that plays on the workstation speaker.
.DE
.LO "~ptolemy/src/domains/sdf/demo"
.DM SDF Universe
.SV 1.2 "February 8, 1991"
.AL "E. A. Lee and Tom Parks"
.LD
.pp
.Ir "Chowning, H. M."
.Ie "sound synthesis"
.Ie "audio, Sparcstation"
.Ie "Sparcstation audio"
.Ie "modulation, frequency"
.Ie "frequency modulation"
.Ie "FM"
.Ie "play program"
This universe implements the FM-based sound synthesis system
invented by H. M. Chowning and described in
"The Synthesis of Complex Audio Spectra by Means of Frequency Modulation",
\fIJournal of the Audio Engineering Society\fR, \fB21(7)\fR, September, 1973.
This technique produces a sound that is rich in harmonics
but relatively inexpensive to compute.
.pp
A
.c FloatDC
star controls the center frequency, relative to $2 pi$,
the sampling frequency.
The samples are written to a file; the star then invokes the "play" program
on that file.  On a sparcstation equipped with a speaker, this will
produce sound.  This will also work on any other workstation if
a compatible "play" program is in the user's path.
.pp
The galaxy fmGalaxy
actually computes the waveform.  It has three inputs,
\fIpitch\fR, which controls the perceived pitch,
\fIindex\fR, which controls the modulation index,
and \fIamp\fR, which controls the amplitude of the sound.
In addition, there are three parameters.
The \fINc\fR parameter multiplies the pitch input
to give the carrier frequency of the FM waveform.
The \fINm\fR parameter multiplies the pitch input
to give the frequency of the sinusoid that modulates the carrier.
The \fIImax\fR parameter multiplies the \fIindex\fR input.
Since this input must be smaller than one in magnitude,
the \fIImax\fR parameter controls the maximum modulation index.
The modulation index controls the relative strength of
the harmonics.  For instance, a modulation index of zero will yield
a pure tone.
For more information, see [1].
.UH REFERENCES
.ip [1]
C. Dodge and T. Jerse, \fIComputer Music:
Synthesis, Composition, and Performance\fR, Schirmer Books,
New York, 1985.
.ES
