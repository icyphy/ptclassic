.\" $Id$ 
.NA "CG56Target"
.SD
This is the base target from which all CG56 single processor
targets are defined.
.SE
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/cg56/kernel"
.EQ
delim $$
.EN
.SV $Revision$ $Date$ 
.AL "J. Buck and J. Pino"
.SH
.NE
\fIdestDirectory\fR : directory to write the generated code to 
.DF "~/DSPcode"
.NE
\fIloopingLevel\fR : Valid values are 0 - 2.  See the
\fISingle-Processor Schedulers\fR section in the \fBCG\fR chapter. 
.DF "0"
.NE
\fIDisplay Code\fR : If yes, display the generated code. 
.DF "YES"
.NE
\fIxMemMap\fR : X memory space, segmented memory can be specified by
separating the intervals with commas. 
.DF "0-4095"
.NE
\fIyMemMap\fR : Y memory space, segmented memory can be specified by
separating the intervals with commas. 
.DF "0-4095"
.ET:
.LD
.pp
This target the default target for generating 56000 assembly code  
The \fBCG56Target\fR is also the base target from which all single-processor 
\fBCG56\fR targets are derived.  
.pp
The \fBCG56Target\fR target is used by default when the \fBCG56\fR domain 
is selected via the pigi \fIedit-domains\fR command. The target can also be 
chosen by selecting the \fBdefault-CG56\fR from the pigi \fIedit-targets\fR 
menu. The main use of the \fBCG56Target\fR is as the base target from which 
all single-processor \fBCG56\fR targets are derived. This target will only 
generate the code. 
.SA
\fISingle-Processor Schedulers\fR section in the \fBCG\fR chapter
.ES
