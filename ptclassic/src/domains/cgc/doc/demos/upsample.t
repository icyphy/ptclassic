.\" $Id$
.NA upsample
.SD
Up-sample demo
.DE
.SV $Revision$ $Date$
.AL "S. Ha"
.LO "~ptolemy/src/domains/cgc/demo"
.LD
.Ie "up-sample"
This simple up-sample demo tests the static buffering.
Each invocation of the
.c XMgraph
star reads the input from the fixed buffer location in the generated code
since the buffer between the
.c UpSample
star and the
.c XMgraph
star is static.
.Ie "static buffering"
.Ie "buffer, static"
.ES
