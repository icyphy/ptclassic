.\" $Id$
.NA qam
.SD
This universe produces a 16-point quadrature amplitude modulated (QAM) signal
and displays the eye diagram for the in-phase part, the constellation,
and the modulated transmit signal.
.DE
.LO "$PTOLEMY/src/domains/sdf/demo"
.DM SDF Universe
.SV $Revision$ $Date$
.AL "Joseph T. Buck"
.LD
.pp
.Ie "modulation, quadrature amplitude"
.Ie "quadrature amplitude modulation"
.Se RaisedCos
.Ie "raised cosine pulse"
A raised cosine pulse is used, where
the excess bandwidth is 100% and the carrier frequency is
twice the symbol rate.
Run it for about 100 iterations.
.pp
This is a port of the ``transmitter'' demo from Gabriel version 0.7.
.Ir "Gabriel"
.UH "References"
.ip [1]
E. A. Lee and D. G. Messerschmitt,
.i "Digital Communication" ,
Kluwer Academic Publishers, Boston, MA, 1988, pp. 154-172
.SA
RaisedCos, Xscope
.ES
