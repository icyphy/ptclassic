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
.IE "spread spectrum phase shit keying"
This demo comprised of a
.c xmit2spread
and 
.c rec2spread 
galaxies illustrates a spread spectrum application using a binary
phase shift keying technique. At the top level two parameters are 
edited SamplingRate and PulseDurationInsample. Default SamplingRate
is 8Khz. The carrier frequency is parameterized using the SamplingRate.
.EQ
frequency = 2/(31/SamplingRate)
.EN 
where 31 is the PulseDurationInSample.
.pp
In the transmitter, the PulseDurationInSample is used to stretch the 
bit duration as to make the modulation possible. In the receiver, the 
PulseDurationInSample is used to down sample to the appropriate levels.
.pp
The
.c Spread
and
.c DeSpreader
galaxies are frame synchronized direct-sequence. That means that the pseudo
noise generated in the transmitter and the receiver must be synchronized.
The
.c Spreader
and 
.c DeSpreader
modulates the input binary bits with the pseudo noise generator. The maximum
length of the
.c PseudoNoise 
galaxy is set to 31, which matches the PulseDurationInSample.
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
xmit2spread,
rec2spread,
scrambler,
Spread,
DeSpread.
.ES
