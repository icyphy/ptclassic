defstar {
        name { DummySourceFix }
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
        output {
                name {output}
                type {FIX}
        }
        defstate {
                name {n}
                type {int}
                desc {number of samples to write}
                default {1}
        }
        setup {
                output.setSDFParams(int(n),int(n)-1);
        }
        codeblock (blockname) {
        }
        go {
                addCode(blockname);
        }
}
