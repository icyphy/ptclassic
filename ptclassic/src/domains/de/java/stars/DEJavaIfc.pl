defstar {
	name { JavaIfc }
	domain { DE }
        derivedfrom { TclScript }
	version { 0.1   6/25/97 }
	author { J. Voigt }
	copyright {
            Copyright (c) 1997 Dresden University of Technology,
            Mobile Communications Systems
        }
	desc { }
        private {
            int dummy;
        }
        begin {
            // load the library only once
            tcl.callTclProc("loadlib"); 
            // now create us a JVM
            dummy = tcl.callTclProc("createjava");
        }
        wrapup {
            // and clean up
            tcl.wrapup();
        }
    }

   
