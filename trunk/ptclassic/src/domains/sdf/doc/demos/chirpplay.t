.\" $Id$
.NA chirpplay
.SD
Chirp generator that plays on the workstation speaker.
.DE
.SV 1.2 "February 8, 1991"
.AL "E. A. Lee"
.LO "~ptolemy/src/domains/sdf/demo"
.LD
.pp
.Id "chrip generator"
.Ir "audio, Sparcstation"
.Ir "Sparcstation audio"
This universe generates a chirp (a sinusoid with slowly increasing
frequency) and plays it over the workstation speaker.
It does this by writing its output to a temporary file and invoking
the "play" program
on that file.  On a sparcstation equipped with a speaker, this will
produce sound.  This will also work on any other workstation if
a compatible "play" program is in the users path.
.ES
