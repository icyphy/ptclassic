defstar {
	name { VISMpyDblSh }
	domain { SDF }
	version { $Id$ }
	author { William Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF vis library }
	desc { 
Multiplies the shorts in a 16bit partitioned float to the
corresponding shorts in a 16bit partitioned float.
The result is four signed shorts that is returned as
a single floating point number.  Each multiplication
results in a 32 bit result, which is then rounded to 16bits.
	}
	input {
	  name { inA }
	  type { float }
	  desc { Input float type }
	}
	input {
	  name { inB }
	  type { float }
	  desc { Input float type }
	}
	output {
	  name { out }
	  type { float }
	  desc { Output float type }
	}
        ccinclude { <vis_proto.h> }
	go {
	  vis_write_gsr(8);

	  // setup the data
	  float dataAhi = vis_read_hi(double(inA%0));
	  float dataAlo = vis_read_lo(double(inA%0));
	  float dataBhi = vis_read_hi(double(inB%0));
	  float dataBlo = vis_read_lo(double(inB%0));
	  
	  // calculate the partial products
	  double resulthihi = vis_fmuld8sux16(dataAhi,dataBhi);
	  double resulthilo = vis_fmuld8ulx16(dataAhi,dataBhi);
	  double resulthi   = vis_fpadd32(resulthihi,resulthilo);
	  
	  double resultlohi = vis_fmuld8sux16(dataAlo,dataBlo);
	  double resultlolo = vis_fmuld8ulx16(dataAlo,dataBlo);
	  double resultlo   = vis_fpadd32(resultlohi,resultlolo);

	  // pack and concat the final product*/
	  float resultu = vis_fpackfix(resulthi);
	  float resultl = vis_fpackfix(resultlo);
	  double result = vis_freg_pair(resultu,resultl);
	  
          out%0 << result;
      	}
}
