defstar {
        name { DummySink }
        domain { CG }
        derivedFrom { CGVarTime }
        desc {
Generic code generator source star.
        }
        version { $Id$ }
        author { Raza Ahmed }
        copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
        location { CG main library }
        explanation {
This star swallows an input sample of any type and does nothing. It serves 
the purpose to generate galaxies out of stars and broken galaxies.
        }
        input {
                name {input}
                type {ANYTYPE}
        }
        defstate {
                name {n}
                type {int}
                desc {number of samples to write}
                default {1}
        }
        setup {
                input.setSDFParams(int(n),int(n)-1);
        }
        codeblock (blockname) {
        }
        go {
                addCode(blockname);
        }
}
