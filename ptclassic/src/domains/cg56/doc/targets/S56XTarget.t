.\" $Id$ 
.NA "S56XTarget"
.SD
S56XTarget is a architecture description for the Ariel Sbus S-56X card.

.SE
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/cg56/targets"
.EQ
delim $$
.EN
.SV $Revision$ $Date$ 
.AL "J. Pino and K. White"
.SH
.NE
\fIRun S-56X?\fR : If yes, compile and download the application into the 
S-56X card. 
.DF "YES"
.NE
\fIHost for S-56X?\fR : Machine on which the S-56X is installed. 
.DF "localhost"
.NE
\fImonitor\fR : Loader/Monitor/Debugger program to use with S-56X target
.DF ""
.ET:
.LD
This target is used for generating assembly code for the Ariel S-56X Sbus
card.  To use it, select \fBS-56X\fR in the pigi \fBedit-target\fR menu.
It is derived from \fBCG56Target\fR class, thus inheriting all of its
states.  However, both CG56Target states \fIxMemMap\fR and \fIyMemMap\fR
have been hidden.
.SA
CG56Target
.ES
