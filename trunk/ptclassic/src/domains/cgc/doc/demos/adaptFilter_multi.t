.\" $Id$
.NA adaptFilter_multi
.SD
A demo of C-code generation for a multiprocessor target.
.SV $Revision$	$Date$
.AL "S. Ha"
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/cgc/demo"
.EQ
delim $$
.EN
.LD
.pp
This is a multiprocessor version of the
.c adaptFilter
demo.
We schedule the graph manually onto two machines whose names are specified in
\fImachineNames\fR target parameter.
The \fIprocId\fR parameter of each star determines the assignment.
Once codes are generated, they are shipped to the specified
machines, compiled, and run by remote shell command "xon".
.Ir "xon, shell command"
Communication between machines are performed by Unix socket mechanism.
In this demo, the user manually selects the port numbers by
\fIportNumber\fR target parameter. 
.SA
adaptFilter.
.ES
