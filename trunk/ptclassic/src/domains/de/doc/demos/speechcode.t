.\" $Id$
.NA speechcode
.SD
This demo performs speech compression with a combination of
silence detection, adaptive quantization, and adaptive estimation.
After speech samples are read from a file, they are encoded, packetized,
depacketized, decoded, and played on the workstation speaker.
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/de/demo"
.EQ
delim $$
.EN
.SV $Revision$ $Date$
.AL "Rachel Bowers, Erick Hamilton."
.LD
.Ir "speech coding"
Packets containing only silence are discarded; the receiver
detects discarded packets and replaces them with zero samples.
.pp
The silence detection is performed by estimating the average
power over a window of speech samples and comparing this estimate
to a fixed threshold.
.Ir "silence detection"
A better strategy might be to compare the local power estimate to
an adaptive threshold that varies with the characteristics of
the input signal.
.pp
The adaptive quantizer consists of an adaptive gain stage and
a fixed quantization stage.
The adaptive gain tries to follow changes in the input signal power.
.Ir "adaptive quantization"
With adaptive quantization, the coder can quantize speech samples to
many fewer levels than could a fixed quantizer with the same distortion.
.pp
The adaptive estimation is performed by the
.c LMSLeak
star.
This star contains an adaptive filter that uses a modified version
\fIleast mean squares\fR algorithm.
This modification helps the adaptive estimators in the coder and decoder
stay synchronized in the event of transmission errors.
Users may wish to vary the parameters of the adaptive estimator
to see their effects on the output speech.
.pp
The coder and decoder contained in this demonstration could be used
as part of a larger simulation of transmission networks and other
types of sources.
However, the adaptive estimation algorithm in this coder is not very
robust to transmission delay variations or transmission losses.
.UH "Necessary Software"
The audio output is sent to a program called "play", which should
be able read mu-law encoded audio and play the audio over a speaker.
Sun SparcStations typically contain this program.
The "play" program must be in a user's search path for the audio
data to be output.
.SA
LMSLeak.
.ES
