.\" $Id$
.NA SpreadBPSK
.SD
Demonstrate the Spread Spectrum Binary Phase Shift Keying technique.
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/demo" 
.DM SDF Universe 
.SV $Revision$ $Date$
.AL "Farhad Jalilvand"
.EQ
delim $$
.EN
.LD
.IE "spread spectrum phase shift keying"
This demo comprised of
.c xmit2spread
and 
.c rec2spread 
galaxies illustrates a spread spectrum application using a binary
phase shift keying technique.
The top-level parameters are \fISamplingRate\fR and
\fIPulseDurationInSample\fR.
The default \fISamplingRate\fR is 8 kHz.
The carrier frequency is parameterized using the SamplingRate.
.EQ
frequency = 2/(31/SamplingRate)
.EN 
where 31 is the pseudo-noise sequence length.
.pp
In the transmitter, the \fIPulseDurationInSample\fR is used to stretch the 
bit duration to make the modulation possible. In the receiver, the 
\fIPulseDurationInSample\fR is used to downsample to the original bit
stream sampling rate.
.pp
The
.c Spread
and
.c DeSpreader
galaxies are frame-synchronized direct-sequence systems.
That means that the pseudo-noise generated in the transmitter and
the receiver must be synchronized.
The
.c Spreader
and 
.c DeSpreader
modulates the input binary bits with the pseudo noise generator.
The maximum length of the
.c PseudoNoise 
galaxy is harcoded to 31.
.UH REFERENCES
.ip [1]
Edward A. Lee and David G. Messerschmitt, \fIDigital Communication\fR,
Kluwer Academic Publishers, 2nd ed., 1994, ISBN 0-7923-9391-0.
.ip [2]
Simon Haykin, \fICommunication Systems\fR,
Wiley & Sons, 3rd ed., 1994, ISBN 0-471-57176-8.
.ip [3]
Leon W. Couch II, \fIDigital and Analog Communication Systems\fR,
Macmillan, 4th ed., 1993, ISBN 0-02-325281-2.
.SA
rec2spread,
scrambler,
xmit2spread,
DeSpread,
Spread.
.ES
