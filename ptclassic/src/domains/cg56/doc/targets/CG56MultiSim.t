.\" $Id$ 
.NA "CG56MultiSimTarget"
.SD
This is a target architecture for generating code for a multiprocessor
DSP system, where the processors communicate via a shared memory. 
.SE
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/cg56/targets"
.EQ
delim $$
.EN
.SV $Revision$ $Date$ 
.AL "Soonhoi Ha, Asawaree Kalavade and Jose Pino"
.SH
.NE
\fInprocs\fR : number of processors 
.DF "2"
.NE
\fIinheritProcessors\fR : If yes, inherit child targets 
.DF "NO"
.NE
\fIsendTime\fR : time to send one datum 
.DF "1"
.NE
\fIoneStarOneProc\fR : If yes, place all invocations of a star on same processor 
.DF "NO"
.NE
\fImanualAssignment\fR : If yes, specify processor assignment of stars manually 
.DF "NO"
.NE
\fIadjustSchedule\fR : If yes, overide the previously obtained schedule by manual assignment 
.DF "NO"
.NE
\fIchildType\fR : child proc type
.DF "default-CG"
.NE
\fIchildType\fR : child proc type
.DF "default-CG"
.NE
\fIfilePrefix\fR : prefix for output code files
.DF "code_proc"
.NE
\fIganttChart\fR : if true, display Gantt chart
.DF "YES"
.NE
\fIlogFile\fR : log file to write to (none if empty)
.DF ""
.NE
\fIignoreIPC\fR : ignore communication cost?
.DF "NO"
.NE
\fIoverlapComm\fR : processor can overlap communication?
.DF "NO"
.NE
\fIuseCluster\fR : Use Gil's declustering algorithm?
.DF "YES"
.NE
\fIdoCompile\fR : disallow compiling during development stage 
.DF "NO"
.NE
\fIsMemMap\fR : shared memory map 
.DF "4096-4195"
.ET:
.LD
This target is used for generating assembly code - corresponding to a 
CG56 algorithm description - for multiple processors connected in a 
shared memory configuration.
Given a CG56 domain description of the algorithm,  the user
can set the target to MultiSim-56000. A number of parameters
are available for this target. Many of these are similar to those of
.c CGMultiTarget 
(from which this is derived).
Those of particular interest here include the destination 
directory to write the code to, the number of processors, the prefix
for the generated code, and the shared memory map. The shared memory 
map is the region of memory used for inter-processor communication.
.pp
On running a CG56 system with this target, the CG56 algorithm 
is partitioned onto the specified number of processors, and 
assembly code for the various processors is generated and assembled. 
This can be followed by running the Thor demo
.c CG56MultiSim_2
which is a Thor domain simulation model for this target architecture.
It consists of two ThorDSP56000s 
connected via a dual-ported shared memory. The destination directory
parameter of the ThorDSP56000 stars can be specified to correspond to 
the destination directory specified for the MultiSim-56000 target.
In the simulation, the DSPs then run the code generated by this target.
This permits close coupling between assembly code generation and simulation of
a hardware system consisting of multiple programmable processors, where 
the processors execute this generated code. 
.SA
CG56MultiSim_2 (Thor demo)
.ES
