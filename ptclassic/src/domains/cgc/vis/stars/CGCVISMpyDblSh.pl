defstar {
	name { VISMpyDblSh }
	domain { CGC }
	version { $Id$ }
	author { William Chen }
	copyright {
Copyright (c) 1996-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC Visual Instruction Set library }
	desc { 
Multiply the corresponding 16-bit fixed point numbers of two
partitioned float particles.  Four signed 16-bit fixed point
numbers of a partitioned 64-bit float particle are multiplied to
those of another 64-bit float particle.  Each multiplication produces 
a 32-bit result.  Each 32-bit result is then left-shifted to fit
within a certain dynamic range and truncated to 16 bits.  The final 
result is four 16-bit fixed point numbers that are returned 
as a single float particle.
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
	defstate {
	  name {leftshift}
	  type {int}
	  default {"1"}
	  desc { Left shift each product so that it falls within the
		 dynamic of the output. }
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
	  vis_write_gsr($val(leftshift)<<3);
	  
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
