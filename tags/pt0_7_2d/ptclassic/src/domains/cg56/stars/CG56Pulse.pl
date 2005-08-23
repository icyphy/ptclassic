defstar {
	name { Pulse }
	domain { CG56 }
	desc { Pulse generator }
	version { @(#)CG56Pulse.pl	1.13 03 Oct 1996 }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
<a name="pulse generator"></a>
A variable length pulse generator.
A pulse begins when a non-zero trigger is received.
The pulse duration varies between 1 and <i>maxDuration</i>
as the control varies between -1.0 and 1.0.
	}

        input  {
                name { trigger }
	        type { fix }
	}
	input  { 
		name { duration }
          	type { fix }
	}
        output {
		name { output }
		type { fix }
	}
        state  {
                name { counter }
		type { fix }
		desc { internal }
		default { 0.0 }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM }
        }
        state  {
                name { onVal }
		type { fix }
		desc { maximum val }
		default { ONE }
        }
        state  {
                name { offVal }
		type { fix }
		desc { minimum }
		default { 0.0 }
        }
        state  {
                name { maxDuration }
		type { int }
		desc { maximum Duration }
		default { 10 }
        }
        codeblock(std) {
        clr     a       $ref(counter),b
        move    $ref(trigger),a1       ; leave rest of A clear
        tst     a       #1,a1
        tne     a,b             ; start counter
        tst     b       #$val(offVal),x0 ; OFF may be short immediate
        jeq     <$label(cont)
        add     a,b     #$val(maxDuration),a1 ; MAX may be short
        move    $ref(duration),x1        a,y1
        mac     x1,y1,a         ; compute duration
        asr     a
        rnd     a       #$val(onVal),x0  ; ON may be short immediate
        cmp     a,b     #0,a1
        tgt     a,b             ; reset counter
        move    b,$ref(counter)
$label(cont)
        move    x0,$ref(output)
        }
        go {
                addCode(std);
        }		

	execTime { 
                 return 21;
	}
}
