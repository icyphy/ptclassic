.\" $Id$
.NA upsample
.SD
Upsampling demonstration.
.DE
.SV $Revision$ $Date$
.AL "S. Ha"
.EQ
delim off
.EN
.LO "$PTOLEMY/src/domains/cgc/demo"
.EQ
delim $$
.EN
.LD
.Ie "upsample"
This simple up-sample demo tests static buffering.
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
