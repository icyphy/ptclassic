defstar {
	name {ComplexReal}
	domain {SDF}
	desc { Convert complex data to real and imaginary parts. }
	version {$Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	input {
		name {input}
		type {complex}
	}
	output {
		name {real}
		type {float}
	}
	output {
		name {imag}
		type {float}
	}
	go {
		Complex &t = Complex(input%0);
		real%0 << t.real();
		imag%0 << t.imag();
	}
}
