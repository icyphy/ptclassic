defstar {
	name { QuadMult16x16 }
	domain { SDF }
	version { @(#)SDFQuadMult16x16.pl	1.2 3/14/96 }
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
        ccinclude {<vis_proto.h>}
	go {
	  
	  double resulthihi, resulthilo, resulthi;
	  double resultlohi, resultlolo, resultlo;
	  double result;
	  float  dataAlo, dataAhi, dataBlo, dataBhi;
	  float  resultu, resultl;
	  
	  vis_write_gsr(8);
	  
	  // setup the data
	       dataAhi=vis_read_hi(double(inA%0));
	  dataAlo=vis_read_lo(double(inA%0));
	  dataBhi=vis_read_hi(double(inB%0));
	  dataBlo=vis_read_lo(double(inB%0));
	  
	  //calculate the partial products
	      resulthihi = vis_fmuld8sux16(dataAhi,dataBhi);
	  resulthilo = vis_fmuld8ulx16(dataAhi,dataBhi);
	  resulthi   = vis_fpadd32(resulthihi,resulthilo);
	  
	  resultlohi = vis_fmuld8sux16(dataAlo,dataBlo);
	  resultlolo = vis_fmuld8ulx16(dataAlo,dataBlo);
	  resultlo   = vis_fpadd32(resultlohi,resultlolo);
	  
	  //pack and concat the final product*/
	      resultu = vis_fpackfix(resulthi);
	  resultl = vis_fpackfix(resultlo);
	  result = vis_freg_pair(resultu,resultl);
	  
          out%0 << result;
      	}
}
