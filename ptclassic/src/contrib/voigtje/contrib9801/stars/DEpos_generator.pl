defstar {
    name { pos_generator }
    domain { DE }
    author { A. Wernicke }
    version { @(#)DEpos_generator.pl	1.1 06/24/97 }	
    copyright { copyright (c) 1997 Dresden University of Technology,
                Mobile Communications Systems 
    }
    location { The WiNeS-Library }
    desc { This star generates a uniformly distributed position in the area 
           (x_min, y_min) (x_max, y_max). The position is returned as a complex 
           value, where x is the real part and y is the imag. part of the 
           complex particle.
    }
    hinclude { <Uniform.h>, <complex.h> }
    ccinclude  { <ACG.h> }
    input {
        name { restart }
        type { int }
    }
    output {
        name { R_Pos }
        type { Complex }
    }
    defstate {
        name { x_min }
        type { int }
        default { "300" }
    }
    defstate {
        name { x_max }
        type { int }
        default { "5000" }
    }
    defstate {
        name { y_min }
        type { int }
        default { "50" }
    }
    defstate {
        name { y_max }
        type { int }
        default { "2600" }
    }
    protected  { 
        Uniform *random_x;
        Uniform *random_y; 
    }
    code { extern ACG* gen; }
    constructor { random_x = NULL;
                  random_y = NULL;	 
    }
    destructor  { if (random_x) delete random_x;
                  if (random_y) delete random_y; 
    }
    setup { 
        if (random_x) delete random_x;
        if (random_y) delete random_y;
	random_x = new Uniform(x_min, x_max, gen);
	random_y = new Uniform(y_min, y_max, gen);
    }
    go {
        if (restart.dataNew) {
            double re = (*random_x) ();
            double im = (*random_y) ();
            R_Pos.put(arrivalTime) << Complex(re, im);
            restart.dataNew= FALSE;
        }
    }
}
