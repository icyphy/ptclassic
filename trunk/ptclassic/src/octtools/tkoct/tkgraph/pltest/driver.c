/**
	tkpole.c
**/

#include "topFixup.h"
#include "tk.h"

#include "topStd.h"
#include "topMisc.h"
#include "topTcl.h"

int main( int argc, char **argv) {
    Tcl_Interp		*ip;
    Tk_Window		win;
    int			r;

    memInitialize();
    topSetAppName( topBasename(argv[0]));
    topMsgSetDbgMode( 1);

    ip = Tcl_CreateInterp();
    if ( (r=topTkInitialize1( ip, &argc, argv, &win)) != TCL_OK ) {
	fprintf( stderr, "Error: %s\n", topTclGetRetMsg(ip,r));
	exit(1);
    }

    memRegisterCmds( ip);
    topMsgRegisterCmds( ip);
    xpSeqWdgRegisterCmds( ip, win);
    xpPlaneWdgRegisterCmds( ip, win);
    xpPlaneLociDhRegisterCmds( ip);

    if ( (r=topTkInitialize2( ip, win, argc, argv)) != TCL_OK ) {
	fprintf( stderr, "Error: %s\n", topTclGetRetMsg(ip,r));
	exit(1);
    }

    Tk_MainLoop();
    exit(0);
}
