.\" $Id$
.NA adaptFilter_multi
.SD
A demo of C-code generation for a multiprocessor target.
.DE
.SV $Revision$	$Date$
.AL "S. Ha"
.LO "~ptolemy/src/domains/cgc/demo"
.LD
.pp
This is a multiprocessor version of the "adaptFilter"
demo. We schedule the graph manually onto two machines whose names
are specified in
.c machineNames
target parameter. The
.c procId
parameter of each star determines the assignment.
Once codes are generated, they are shipped to the specified
machines, compiled, and run by remote shell command "xon".
.Ir "xon, shell command"
Communication between machines are performed by Unix socket mechanism.
In this demo, the user manually selects the port numbers by
.c portNumber
target parameter. 
.SA
adaptFilter.
.ES
