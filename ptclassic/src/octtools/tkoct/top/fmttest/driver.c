/**
**/

#include "topFixup.h"
#include <stdio.h>
#include <math.h>
#include "tcl.h"

#include "topStd.h"
#include "topMisc.h"

main( int argc, char **argv) {
    Tcl_Interp		*ip;

    ip = Tcl_CreateInterp();
    topMsgRegisterCmds(ip);
    topFmtRegisterCmds(ip);

    topTclMainLoop( ip, "fmttest: ", argv[0]);
    exit(0);
}
