.\" $Id$
.NA bfsk
.SD
Demonstrate the Binary Frequency Shift Keying modulation technique on 
a noisy telephone channel.
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/sdf/demo" 
.DM SDF Universe 
.SV $Revision$ $Date$
.AL "Farhad Jalilvand and Richard Johannesson"
.EQ
delim $$
.EN
.LD
.IE "frequency shift keying"
This demo comprised of
.c xmit2fsk
,
.c telephoneChannel
and 
.c rec2fsk 
galaxies illustrates a Binary Frequency Shift Keying (BFSK) system.
This is a discontinuous-phase implementation of a BFSK system,
in which the synchronization of transmitter and receiver is required.
The power spectrum of the input and output of the 
.c telephoneChannel 
is plotted.
In the
.c telephoneChannel 
galaxy, various channel parameters can be varied.
FSK signaling consists of conveying information by shifting the
frequency of the carrier wave. In the discontinuous-phase FSK signaling,
the binary symbols 0 and 1 are represented by two separate frequencies, f1
and f2. By switching between these two frequencies, the FSK transmitter 
indicates which of the two symbols is being sent.
.UH REFERENCES
.ip [1]
Simon Haykin, \fICommunication Systems\fR,
Wiley & Sons, 3rd ed., 1994, ISBN 0-471-57176-8.
.ip [2]
Leon W. Couch II, \fIDigital and Analog Communication Systems\fR,
Macmillan, 4th ed., 1993, ISBN 0-02-325281-2.
.SA
xmit2fsk,
telephoneChannel,
rec2fsk.
.ES
