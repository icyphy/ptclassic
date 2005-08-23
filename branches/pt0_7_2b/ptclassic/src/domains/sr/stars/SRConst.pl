defstar {
    name { Const }
    domain { SR }
    desc {
In every "interval" instant, output a signal with value given by the "level"
parameter.
    }
    version { @(#)SRConst.pl	1.6 11/12/98 }
    author { S. A. Edwards, Bilung Lee }
    copyright {
Copyright (c) 1996-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
    output {
	name { output }
	type { int }
    }
    defstate {
	name { level }
	type { int }
	default { "0" }
	desc { The constant value }
    }
    defstate {
	name { interval }
	type { int }
	default { "1" }
	desc { The interval of instants for constant emission }
    }
    protected {
        int numInstants;
    }
    setup {
        numInstants = 0;
    }
    go {
        if ((numInstants % int(interval)) == 0) {
            output.emit() << int(level);
        } else {
            output.makeAbsent();
        }
    }
    tick {
        numInstants++;
    }
}
