#ifndef _TclObj_h
#define _TclObj_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "tcl.h"

class StringList;

class TclObj {

protected:
        // the Tcl interpreter
        Tcl_Interp* interp;

        // flag to indicate that interp is owned by me
        short myInterp;

        // return a usage error
        int usage(const char*);

        // return the passed value (with proper cleanup) as the result
        // of a Tcl Command
        int result(StringList&);
        int result(char*);
        int result(int);

        // return a "static result".  Don't give this one a stringlist!
        int staticResult(const char*);

        // append a value to the result, using Tcl_AppendElement.
        void addResult(const char*);

};

#endif          // _TclObj_h


