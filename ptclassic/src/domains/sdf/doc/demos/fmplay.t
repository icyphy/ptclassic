.\" $Id$
.NA fmplay
.SD
Sound generator using FM modulation that plays on the workstation speaker.
.DE
.SV 1.2 "February 8, 1991"
.AL "E. A. Lee and Tom Parks"
.LO "~ptolemy/src/domains/sdf/demo"
.LD
.pp
.Ir "Chowning, H. M."
.Ie "sound synthesis"
.Ie "audio, Sparcstation"
.Ie "Sparcstation audio"
.Ie "modulation, frequency"
.Ie "frequency modulation"
.Ie "FM"
This universe implements the FM-based sound synthesis system
invented by H. M. Chowning and described in
"The Synthesis of Complex Audio Spectra by Means of Frequency Modulation",
\fIJournal of the Audio Engineering Society\fR, \fB21(7)\fR, September, 1973.
The structure is identical to the \fIfm\fR demo system, which
plots its output on the screen, but instead of plotting the output
on the screen, it writes it to a file and invokes the "play" program
on that file.  On a sparcstation equipped with a speaker, this will
produce sound.  This will also work on any other workstation if
a compatible "play" program is in the users path.
.ES
