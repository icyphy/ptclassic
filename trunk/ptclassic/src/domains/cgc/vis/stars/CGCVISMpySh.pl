defstar {
	name { VISMpyDblSh }
	domain { CGC }
	version { $Id$ }
	author { William Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC vis library }
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
	constructor {
	  noInternalState();
	}
	initCode{
	  addInclude("<vis_proto.h>");
	  addInclude("<vis_types.h>");
	}
	codeblock(localDecl){
	  vis_d64  resulthihi,resulthilo,resulthi;
	  vis_d64  resultlohi,resultlolo,resultlo,result;
	  vis_f32  dataAlo,dataAhi,dataBlo,dataBhi,resultu,resultl;
	}
	codeblock(multfour){
	  vis_write_gsr(8);
	  
	  /* setup the data */
	  dataAhi=vis_read_hi((double) $ref(inA));
	  dataAlo=vis_read_lo((double) $ref(inA));
	  dataBhi=vis_read_hi((double) $ref(inB));
	  dataBlo=vis_read_lo((double) $ref(inB));
	  
	  /* calculate the partial products */
	  resulthihi = vis_fmuld8sux16(dataAhi,dataBhi);
	  resulthilo = vis_fmuld8ulx16(dataAhi,dataBhi);
	  resulthi   = vis_fpadd32(resulthihi,resulthilo);
	  
	  resultlohi = vis_fmuld8sux16(dataAlo,dataBlo);
	  resultlolo = vis_fmuld8ulx16(dataAlo,dataBlo);
	  resultlo   = vis_fpadd32(resultlohi,resultlolo);
	  
	  /*pack and concat the final product*/
	  resultu = vis_fpackfix(resulthi);
	  resultl = vis_fpackfix(resultlo);
	  result = vis_freg_pair(resultu,resultl);
	  
          $ref(out) = result;
	}
	go {	  
	  addCode(localDecl);
	  addCode(multfour);
      	}
}
