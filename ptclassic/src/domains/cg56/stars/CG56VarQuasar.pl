defstar {
	name { VarQuasar }
	domain { CG56 }
	desc {
A sequence of values is repeated at the ouput with period N, zero-padding
or tuncating to N if necessary.  A value of 0 for N outputs an aperiodic
sequence
        }
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 nonlinear functions library }
        explanation {
A sequence of values is repeated at the ouput with period N, zero-padding
or tuncating to N if necessary.  A value of 0 for N outputs an aperiodic
sequence
	}

        input  {
                name { N }
	        type { fix }
	}
        output {
		name { output }
		type { fix }
	}
        state  {
                name { data }
		type { fixarray }
		desc { list of values. }
		default { "0.1 0.2 0.3 0.4" }
                attributes { A_NONCONSTANT|A_YMEM }
        }
        state  {
                name { dataCount }
                type { int }
                default { 0 }
                desc { keep track number of data }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM }
        }
        state  {
                name { dataLen }
	        type { int }
	        default { 0 }
	        desc { length of data (internal) }
                attributes { A_NONCONSTANT|A_NONSETTABLE }
        }
      
        codeblock(std) {
        clr     a       $ref(dataCount),b
        move    #$val(dataLen),a1         ; len may be SHORT
        cmp     a,b     #<0.0,a ; output = zero
        jge     <$label(output)
        move    #$addr(data),r0        ; base may be SHORT
        move    b,n0
        nop                     ; wait for n0
        move    y:(r0+n0),a1    ; output = next sample
$label(output)
        move    a1,$ref(output)
        move    #1,a1
        add     a,b     $ref(N),a1  ; increment counter
        tst     a
        jeq     <$label(cont)
        cmp     a,b     #<0.0,a
        tge     a,b             ; reset counter
$label(cont)
        move    b,$ref(dataCount)
        }

        setup {
                dataLen=data.size();
        }		

        go {
                addCode(std);
        }		

	execTime { 
                 return 20;
	}
}


