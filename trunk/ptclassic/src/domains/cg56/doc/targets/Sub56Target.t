.\" $Id$ 
.NA "Sub56Target"
.SD
Sub56Target is a architecture description that is used to generate
subroutines that can be called from hand-written 56000 code.
.SE
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/cg56/targets"
.EQ
delim $$
.EN
.SV $Revision$ $Date$ 
.AL "J. Pino"
.LD
.pp
This is a simple target that is used to generate subroutines to be called 
from code written outside of \*(PT environment. To use it, select 
\fBSub-CG56\fR in the pigi \fBedit-target\fR.  This target defines no additional 
states of its own, and inherits all of \fBCG56Target\fR's states. This 
target is an example of where the \fImainLoopCode\fR CGTarget 
method is redefined. 
.Ie "mainLoopCode, redefinition"
.SA
CG56Target
.ES
