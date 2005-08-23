/* myTclCmd.c: test file for evaluating C files inside Tycho by compiling
 * and then loading the shared object.  To see this work, open this file
 * within Tycho, then type C-x C-r, then type 'mytclcmd' in the Tcl console.
 */
#ifndef _TCL
#include <tcl.h>
#endif
static int
Tcl_MyTclCmd(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
    Tcl_AppendResult (interp, "mytclcmd: foo", (char *)NULL);
    return TCL_OK;
}

/* Tcl initialization procedure added for the Tcl load command
 */
int Mytclcmd_Init(Tcl_Interp *interp)
{
    Tcl_CreateCommand(interp, "myTclCmd", Tcl_MyTclCmd,
                         (ClientData) NULL, (Tcl_CmdDeleteProc*) NULL);
    return TCL_OK;
}
