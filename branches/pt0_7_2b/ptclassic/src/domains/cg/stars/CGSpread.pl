defstar {
	name { Spread }
	domain { CG }
	derivedFrom { CGVarTime }
	desc {
Takes one input and produces multiple outputs.
	}
	version {$Id$}
	author { S. Ha }
	copyright { 1991 The Regents of the University of California }
	location { CG demo library }
	explanation {
This star does not generate any code. In multiprocessor code generation
domain, this star will be automatically attached to a porthole whose
outputs are passed to more than one destination (one ordinary block and
one Send star, more than one Send stars, and so on.)
	}
	input {
		name {input}
		type {ANYTYPE}
	}
	outmulti {
		name {output}
		type {=input}
	}
	go {
	}
}