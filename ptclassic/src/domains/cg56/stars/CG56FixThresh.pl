defstar {
	name { FixThresh }
	domain { CG56 }
	desc { Threshold Detector }
	version { $Id$ }
	author { Chih-Tsung Huang }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
This star zeroes out input data
that are smaller than the threshold in magnitude.
Input data larger than the threshold in magnitude
are passed through unchanged.
	}
        seealso { CG56Limit }
	execTime {
		return 7;
	}
	input {
		name {input}
		type {fix}
	}
	output {
		name {output}
		type {fix}
	}
        state {
                name {threshold}
	        type {fix}
	        default { 0.1 }
	        desc {threshold value}
        }
	codeblock (main) {
        move    #$val(threshold),b
        move    $ref(input),x0
        cmpm    x0,b
        jpl     $label(end)
        clr     x0
$label(end)
        move    x0,$ref(output)
	}

	go {
		gencode(main);
	}
}
