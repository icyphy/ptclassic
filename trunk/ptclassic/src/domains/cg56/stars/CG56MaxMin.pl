 defstar {
	name { MaxMin }
	domain { CG56 }
	desc { Finds Maximum or Minimum value. }
	version {$Id$}
	author { Chih-Tsung Huang }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 demo library }
	explanation {
If \fIcmpareMagnitude\fR is ``no'',
the star finds from among the \fIinput\fR inputs the one with
the maximum or minimum value; else it finds from among the \fIinput\fR inputs
the one with the maximum or minimum magnitude.
if \fIoutputMagnitude\fR is ``yes'',
the magnitude of the result is written to the output,
else the result itself is written to the output.
Returns maximum value among N (default 10) samples.
Also, the index of the output is provided (count starts at 0).
	}
	input {
		name {input}
		type {fix}
	}
	output {
		name {output}
		type {fix}
	}
	output {
		name {index}
		type {int}
	}
	state {
		name {N}
		type {int}
		default {10}
		desc {default samples}
	}
	state {
		name {MAX}
		type {int}
		default {"YES"}
		desc {output maximum value else minimum is the output}
	}    
	state {
		name {compareMagnitude}
		type {int}
		default {"NO"}
		desc {default is not to compare magnitude}
	}    	
	state {
		name {outputMagnitude}
		type {int}
		default {"NO"}
		desc {default is not to output magnitude}
	}    

	start {
		input.setSDFParams(int(N),int(N)-1);
	}		
	
        codeblock(main) {
	clr	b
	move	b0,r1
        move    #<$addr(input),r0
        nop
        move    x:(r0)+,a
        do      #$val(N)-1,$starSymbol(end)
        move    x:(r0)+,x1
        }
        codeblock(cmpMagYes) {
        cmpm   x1,a	(r1)+
        }
        codeblock(cmpMagNo) {
        cmp    x1,a	(r1)+
        }
        codeblock(maxcont) {
	tlt	x1,a	r1,r2
$starSymbol(end)
        }
        codeblock(mincont) {
	tgt	x1,a	r1,r2	; if x1 < a, move x1 into a and r1 into r2
$starSymbol(end)
        }
        codeblock(one) {
        move    $ref(input),a
        }
        codeblock(outMagYes) {  
        abs	a
        }
        codeblock(out) {
        move    a,$ref(output)
	move	r2,$ref(index)
       }
 	go {
		if(N>1) {
			addCode(main);
			if(int(compareMagnitude))
				addCode(cmpMagYes);
			else
				addCode(cmpMagNo);
			if(int(MAX))
				addCode(maxcont);
			else
				addCode(mincont);				
		}
	        else
			addCode(one);
	        if(int(outputMagnitude))
			addCode(outMagYes);
			
		addCode(out);
	}

	exectime {
                if(int(N)>10)
			return ((int(N)-1)*6)+7;
		else
			return 3;
 	}
}

