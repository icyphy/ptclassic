defstar {
	name { JNITest }
	domain { DE }
	version { $Id$ }
	author { J. Voigt }
	copyright {
            Copyright (c) 1997 Dresden University of Technology,
            Mobile Communications Systems
        }
	desc { This is a simple test of interfacing Ptolemy and Java using
the JDK1.1 JNI}
	explanation {
        }
	hinclude { "TclStarIfc.h", "ptkTclCommands.h", <iostream.h> }
	ccinclude { "ptk.h" }
	input {
            name { Number }
            type { int }
            desc {  }
	}
        output {
            name { Factorial }
            type { int }
            desc {  }
        }
	defstate {
            name { tcl_file }
            type { string }
            default { "$PTOLEMY/src/domains/de/java/stars/jnitest.tcl" }
            desc { The file from which to read the Tcl script }
	}
	header {
            // Overload TclStarIfc (we might need to distinguish the ports)
            class myTclStarIfc : public TclStarIfc {
                public:
                myTclStarIfc();
                ~myTclStarIfc();
                InfString getInputValue_Number ();
                void setOutputValue_Factorial(int);
                int mySetup ( Block*, const char*);
                //just to make callTclProc public
                inline int callTcl(const char *name) {
                    return (TclStarIfc::callTclProc(name));
                }
		int num;
	    };
        }
	code {  
	    static int grabNumber(ClientData tcl, Tcl_Interp*, int, char*[])
            {
                InfString inputs = ((myTclStarIfc*)tcl)->getInputValue_Number();
                Tcl_SetResult(ptkInterp, (char*) inputs, TCL_VOLATILE);
                return TCL_OK;
            }
            
            static int setFactorial( ClientData tcl, Tcl_Interp*, int /*argc*/, char *argv[]) {
                int dummy;
                //read the value
                sscanf(argv[1], "%i", &dummy);
                ((myTclStarIfc*)tcl)->setOutputValue_Factorial(dummy);
                return TCL_OK;
            }

            myTclStarIfc::myTclStarIfc() {
                starID = "tclStar";
                starID += unique++;
            }
            myTclStarIfc::~myTclStarIfc() {
               // exit immediately if there is no ptk interpreter.  This happens
               // if there are global TclStarIfc objects in the program but the
               // interpreter failed to set up.
                if (!ptkInterp) return;

                InfString buf;

               // First, invoke the Tcl desctructor procedure, if it was defined
                buf = "info procs destructorTcl_";
                buf += starID;
                if(Tcl_GlobalEval(ptkInterp, (char*)buf) == TCL_OK &&
                strlen(ptkInterp->result) != 0)
                callTclProc("destructorTcl");

                // Delete Tcl commands created for this star
                buf = "grabInputsNumber_";
                buf += starID;
                Tcl_DeleteCommand(ptkInterp, (char*)buf);

                buf = "setOutputFactorial_";
                buf += starID;
                Tcl_DeleteCommand(ptkInterp, (char*)buf);
            }

            InfString myTclStarIfc::getInputValue_Number() {
                InfString buffer;
                buffer += "{";
                buffer += StringList (num);
                buffer += "} "; 
		return buffer;
            }
            void myTclStarIfc::setOutputValue_Factorial(int number) {
                num = number;
            }
            int myTclStarIfc::mySetup(Block *star, const char *tcl_file) {
                myStar = star;
                InfString buffer;
                synchronous = 1; 

                buffer = "grabInputsNumber_";
                buffer += starID;
                Tcl_CreateCommand(ptkInterp, (char*) buffer, grabNumber, (ClientData) this , NULL);
              
                buffer = "setOutputFactorial_";
                buffer += starID;
                Tcl_CreateCommand(ptkInterp, (char*) buffer, setFactorial, (ClientData) this , NULL);

                buffer = star->fullName();
                Tcl_SetVar2(ptkInterp, (char*)starID, "fullName",
                (char*)buffer, TCL_GLOBAL_ONLY);

            
                // Step through all the states
                State* s;
                BlockStateIter next(*star);
                while ((s = next++) != 0) {
                    InfString val = s->currentValue();
                    InfString name = s->name();
                    // In case the currentValue is null, use an empty string.
                    char* v = (char*)val;
                    if (v == NULL) { v = ""; }
                    Tcl_SetVar2(ptkInterp, (char*)starID, (char*)name,
                    v, TCL_GLOBAL_ONLY);
                }
            
                // Add the full name of the star to the $starID array
                InfString fullname = star->fullName();
                Tcl_SetVar2(ptkInterp, (char*)starID, "fullName",
                (char*)fullname, TCL_GLOBAL_ONLY);
            
                // Before sourcing the Tcl script, we set the global variable
                // starID, which will be used by the Tcl script.
                if(Tcl_SetVar(ptkInterp, "starID", (char*)starID, TCL_GLOBAL_ONLY)
                == NULL) {
                    Error::abortRun(*star, "Failed to set starID");
                    return FALSE;
                }
            
                // Source the Tcl script.
                if(tcl_file[0] == '$') {
                    buffer = "source [ptkExpandEnvVar \\";
                    buffer += tcl_file;
                    buffer += "]";
                } else { 
                    buffer = "source ";
                    buffer += tcl_file;
                }
            
                if(Tcl_GlobalEval(ptkInterp, (char*)buffer) != TCL_OK) {
                    char tkErrorCmd[ sizeof(PTK_DISPLAY_ERROR_INFO) ];
                    strcpy(tkErrorCmd, PTK_DISPLAY_ERROR_INFO);
                    Tcl_GlobalEval(ptkInterp, tkErrorCmd);
                    Error::abortRun(*star, "Cannot source tcl script ", tcl_file);
                    return FALSE;
                }
                // Determine whether the star is synchronous by checking to
                // see whether the goTcl procedure has been defined by the
                // Tcl script.
                buffer = "info procs goTcl_";
                buffer += starID;
                if(Tcl_GlobalEval(ptkInterp, (char*)buffer) != TCL_OK ||
                strlen(ptkInterp->result) == 0)
                synchronous = 0;
                else 
                synchronous = 1;
           
                // Determine whether Tk has been loaded into the system
                // (if so, then its event loop will need to be updated).
                char tkExistsCmd[ sizeof(PTK_TK_EXISTS) ];
                strcpy(tkExistsCmd, PTK_TK_EXISTS);
                Tcl_GlobalEval(ptkInterp, tkExistsCmd);
                if (*(ptkInterp->result)=='1') tkExists = 1;
                else tkExists = 0;
                
                return TRUE;
            }
        } 
        protected {
	    myTclStarIfc tcl;
        }
	private {
	    int dummy;
	}
	begin {
            tcl.mySetup( this, (const char*) tcl_file );
            // load the library only once
            tcl.callTcl("loadlib"); 
            // now create us a JVM
            dummy = tcl.callTcl("createjava");
        }
	go {
            if ( Number.dataNew ) {
                completionTime = arrivalTime;
		dummy = Number%0;
                tcl.num = dummy;
                cout << "Java calculates the factorial of " << dummy << endl;
		tcl.callTcl("new");
                Factorial.put(completionTime) << tcl.num;
                Number.dataNew = FALSE;
            }
        }
	wrapup {
            // and clean up
            tcl.wrapup();
        }
}
