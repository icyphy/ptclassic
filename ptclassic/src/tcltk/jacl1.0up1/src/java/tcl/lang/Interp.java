/*
 * Interp.java --
 *
 *	Implements the core Tcl interpreter.
 *
 * Copyright (c) 1997 Cornell University.
 * Copyright (c) 1997-1998 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 * 
 * SCCS: @(#) Interp.java 1.101 98/02/11 18:08:03
 *
 */

package tcl.lang;

import java.util.*;
import java.io.*;
import java.net.*;

/**
 * The Tcl interpreter class.
 */

public class Interp {

/*
 * The following three variables are used to maintain a translation
 * table between ReflectObject's and their string names. These two
 * variables are accessed by the ReflectObject class (the variables
 * are here because we want one translation table per Interp).
 */

/*
 * Translates integer ID to ReflectObject.
 */

Hashtable reflectIDTable;

/*
 * Translates Object to ReflectObject. This makes sure we have only
 * one ReflectObject internalRep for the same Object -- this
 * way Object identity can be done by string comparison.
 */

Hashtable reflectObjTable;

/*
 * Number of reflect objects created so far inside this Interp
 * (including those that have be freed)
 */

long reflectObjCount;

/*
 * The interpreter will execute scripts only inside this thread. All
 * calls to Interp.eval() by any other thread will be routed to this
 * thread.
 */

private Thread primaryThread;

/*
 * The number of chars to copy from an offending command into error
 * message.
 */

private static final int MAX_ERR_LENGTH = 200;

/*
 * We pretend this is Tcl 8.0, patch level 0.
 */

static final String TCL_VERSION     = "8.0";
static final String TCL_PATCH_LEVEL = "8.0";

/*
 * Total number of times a command procedure
 * has been called for this interpreter.
 */

protected int cmdCount = 0;

/*
 * Table of commands for this interpreter.
 */

Hashtable cmdTable;

/*
 * Table of channels currently registered in this interp.
 */

Hashtable interpChanTable;

/*
 * The Notifier associated with this Interp.
 */

private Notifier notifier;

/*
 * Hash table for associating data with this interpreter. Cleaned up
 * when this interpreter is deleted.
 */

Hashtable assocDataTab;

/*
 * Current working directory.
 */

private File workingDir;

/*
 * Points to top-most in stack of all nested procedure
 * invocations.  NULL means there are no active procedures.
 */

CallFrame frame;

/*
 * Points to the call frame whose variables are currently in use
 * (same as framePtr unless an "uplevel" command is being
 * executed).  NULL means no procedure is active or "uplevel 0" is
 * being exec'ed.
 */

CallFrame varFrame;

/*
 * Points to the global variable frame.
 */

CallFrame globalFrame;

/*
 * The script file currently under execution. Can be null if the
 * interpreter is not evaluating any script file.
 */

String scriptFile;

/*
 * Number of times the interp.eval() routine has been recursively
 * invoked.
 */

int nestLevel;

/*
 * Used to catch infinite loops in Parser.eval2.
 */

int maxNestingDepth;

/*
 * Flags used when evaluating a command.
 */

int evalFlags;

/*
 * Flags used when evaluating a command.
 */

int flags;

/*
 * Offset of character just after last one compiled or executed
 * by Parser.eval2().
 */

int termOffset;

/*
 * The expression parser for this interp.
 */

Expression expr;

/*
 * Used by the Expression class.  If it is equal to zero, then the 
 * parser will evaluate commands and retrieve variable values from 
 * the interp.
 */

int noEval;

/*
 * Used in the Expression.java file for the 
 * SrandFunction.class and RandFunction.class.
 * Set to true if a seed has been set.
 */

boolean randSeedInit;

/*
 * Used in the Expression.java file for the SrandFunction.class and
 * RandFunction.class.  Stores the value of the seed.
 */

long randSeed;

/*
 * If returnCode is TCL.ERROR, stores the errorInfo.
 */

String errorInfo = null;

/*
 * If returnCode is TCL.ERROR, stores the errorCode.
 */

String errorCode = null;

/*
 * Completion code to return if current procedure exits with a
 * TCL_RETURN code.
 */

protected int returnCode;

/* 
 * True means the interpreter has been deleted: don't process any
 * more commands for it, and destroy the structure as soon as all
 * nested invocations of eval() are done.
 */

protected boolean deleted;

/* 
 * True means an error unwind is already in progress. False
 * means a command proc has been invoked since last error occured.
 */

protected boolean errInProgress;

/*
 * True means information has already been logged in $errorInfo
 * for the current eval() instance, so eval() needn't log it
 * (used to implement the "error" command).
 */

protected boolean errAlreadyLogged;

/*
 * True means that addErrorInfo has been called to record
 * information for the current error. False means Interp.eval
 * must clear the errorCode variable if an error is returned.
 */

protected boolean errCodeSet;

/*
 * When TCL_ERROR is returned, this gives the line number within
 * the command where the error occurred (1 means first line).
 */

int errorLine;

/*
 * Stores the current result in the interpreter.
 */

private TclObject m_result;

/*
 * Value m_result is set to when resetResult() is called.
 */

private TclObject m_nullResult;

/*
 * Used by the PackageCmd.
 */

Hashtable packageTable;
String packageUnknown;

/*
 *----------------------------------------------------------------------
 *
 * Interp --
 *	Initializes an interpreter object.
 *
 * Side effects:
 *	Various parts of the interpreter are initialized; built-in
 *	commands are created; global variables are initialized, etc.
 *
 *----------------------------------------------------------------------
 */

public 
Interp() 
{
    cmdTable         = new Hashtable();
    frame            = newCallFrame();
    varFrame         = frame;
    globalFrame      = frame;
    expr             = new Expression();
    nestLevel        = 0;
    maxNestingDepth  = 1000;
    workingDir       = new File(Util.tryGetSystemProperty("user.dir", "."));
    noEval           = 0;
    scriptFile       = "";

    primaryThread    = Thread.currentThread();
    notifier	     = Notifier.getNotifierForThread(primaryThread);
    notifier.preserve();

    assocDataTab     = null;
    randSeedInit     = false;

    ReflectObject.init(this);

    errorLine        = 0;
    deleted          = false;
    errInProgress    = false;
    errAlreadyLogged = false;
    errCodeSet       = false;
    returnCode       = TCL.OK;

    dbg              = initDebugInfo();

    packageTable = new Hashtable();
    packageUnknown = null;
    
    /*
     * An empty result is used pretty often. We will use a shared
     * TclObject instance to represent the empty result so that we
     * don't need to create a new TclObject instance every time the
     * interpreter result is set to empty.
     */

    m_nullResult = TclString.newInstance("");
    m_nullResult.preserve();
    m_result = m_nullResult;

    /*
     * Initialize the Global (static) channel table and the local
     * interp channel table.
     */

    interpChanTable = TclIO.getInterpChanTable(this);

    /*
     * Sets up the variable trace for tcl_precision.
     */

    Util.setupPrecisionTrace(this);

    /*
     * Create the built-in commands.
     */

    createCommands();

    try {
	/*
	 * Set up tcl_platform, tcl_version, tcl_library and other
	 * global variables.
	 */

	setVar("tcl_platform", "platform", "java", TCL.GLOBAL_ONLY);
	setVar("tcl_platform", "javaVersion", 
		Util.tryGetSystemProperty("java.version", "?"),
		TCL.GLOBAL_ONLY);
	setVar("tcl_platform", "byteOrder", "bigEndian", TCL.GLOBAL_ONLY);
	
	setVar("tcl_platform", "os", 
		Util.tryGetSystemProperty("os.name", "?"), TCL.GLOBAL_ONLY);
	setVar("tcl_platform", "osVersion", 
		Util.tryGetSystemProperty("os.version", "?"), TCL.GLOBAL_ONLY);
	setVar("tcl_platform", "machine", 
		Util.tryGetSystemProperty("os.arch", "?"), TCL.GLOBAL_ONLY);
	
	setVar("tcl_version", TCL_VERSION, TCL.GLOBAL_ONLY);
	setVar("tcl_patchLevel", TCL_PATCH_LEVEL, TCL.GLOBAL_ONLY);
	setVar("tcl_library", "resource:/tcl/lang/library",
		TCL.GLOBAL_ONLY);
	if (Util.isWindows()) {
	    setVar("tcl_platform", "host_platform", "windows",
		    TCL.GLOBAL_ONLY);
	} else if (Util.isMac()) {
	    setVar("tcl_platform", "host_platform", "macintosh",
		    TCL.GLOBAL_ONLY);
	} else {
	    setVar("tcl_platform", "host_platform", "unix",
		    TCL.GLOBAL_ONLY);
	}

	/*
	 * Create the env array an populated it with proper
	 * values.
	 */

	Env.initialize(this);

	/*
	 * Register Tcl's version number.  Note: This MUST be 
	 * done before the call to evalResource, otherwise
	 * calls to "package require tcl" will fail.
	 */
	
	pkgProvide("Tcl", TCL_VERSION);
	
	/*
	 * Source the init.tcl script to initialize auto-loading.
	 */
	
	evalResource("/tcl/lang/library/init.tcl");

    } catch (TclException e) {
	System.out.println(getResult());
	e.printStackTrace();
	throw new TclRuntimeError("unexpected TclException: " + e);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * dispose --
 *
 *	This method cleans up the state of the interpreter so that
 *	it can be garbage collected safely. This routine needs to
 *	break any circular references that might keep the interpreter
 *	alive indefinitely.
 *
 *	[ToDo]
 *	The code in this method should run in an EventuallyFree proc,
 *	so that the interpreter won't be deleted until no active code
 *	depends on the interp.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Cleans up the interpreter.
 *
 *----------------------------------------------------------------------
 */

public void
dispose()
{
    if (deleted) {
	return;
    }

    deleted = true;

    if (nestLevel > 0) {
        throw new TclRuntimeError("dispose() called with active evals");
    }

    /*
     * Remove our association with the notifer (if we had one).
     */

    if (notifier != null) {
	notifier.release();
	notifier = null;
    }

    /*
     * Dismantle everything in the global namespace except for the
     * "errorInfo" and "errorCode" variables. These might be needed
     * later on if errors occur while deleting commands. We are careful
     * to destroy and recreate the "errorInfo" and "errorCode"
     * variables, in case they had any traces on them.
     *
     * Dismantle the namespace here, before we clear the assocData. If any
     * background errors occur here, they will be deleted below.
     */
    
    // [ToDo] TclTeardownNamespace(iPtr->globalNsPtr);

    /*
     * Delete all variables.
     */

    TclObject errorInfoObj = null, errorCodeObj = null;

    try {
	errorInfoObj = getVar("errorInfo", null, TCL.GLOBAL_ONLY|
		TCL.DONT_THROW_EXCEPTION);
	if (errorInfoObj != null) {
	    errorInfoObj.preserve();
	}

	errorCodeObj = getVar("errorCode", null, TCL.GLOBAL_ONLY|
		TCL.DONT_THROW_EXCEPTION);
	if (errorCodeObj != null) {
	    errorCodeObj.preserve();
	}
    } catch (TclException e) {
	throw new TclRuntimeError("unexpected TclException: " + e);
    }

    globalFrame.dispose();
    frame = newCallFrame();
    varFrame = frame;
    globalFrame = frame;

    try {
	if (errorInfoObj != null) {
	    setVar("errorInfo", null, errorInfoObj, TCL.GLOBAL_ONLY);
	    errorInfoObj.release();
	}
	if (errorCodeObj != null) {
	    setVar("errorCode", null, errorCodeObj, TCL.GLOBAL_ONLY);
	    errorCodeObj.release();
	}
    } catch (TclException e) {
	/*
	 * Ignore it -- same behavior as Tcl 8.0.
	 */
    }

    /*
     * Delete all commands.
     */

    if (cmdTable != null) {
	for (Enumeration e = cmdTable.keys(); e.hasMoreElements();) {
	    String cmdName = (String)e.nextElement();
	    deleteCommand(cmdName);
	}
	cmdTable = null;
    }

    /*
     * Tear down the math function table.
     */

    expr = null;

    /*
     * Remove all the assoc data tied to this interp and invoke
     * deletion callbacks; note that a callback can create new
     * callbacks, so we iterate.
     */

    while (assocDataTab != null) {
	Hashtable table = assocDataTab;
	assocDataTab = null;

	for (Enumeration e = table.keys(); e.hasMoreElements();) {
	    Object key = e.nextElement();
	    AssocData data = (AssocData) table.get(key);
	    data.disposeAssocData(this);
	    table.remove(key);
	}
    }

    /*
     * Finish deleting the global namespace.
     */
    
    // [ToDo] Tcl_DeleteNamespace((Tcl_Namespace *) iPtr->globalNsPtr);

    /*
     * Free up the result *after* deleting variables, since variable
     * deletion could have transferred ownership of the result string
     * to Tcl.
     */

    globalFrame.dispose();
    frame = null;
    varFrame = null;
    globalFrame = null;

    resetResult();
}

/*
 *----------------------------------------------------------------------
 *
 * finalize --
 *
 *	Interpreter finalization method.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Calls dispose() to ensure everything has been cleaned up.
 *
 *----------------------------------------------------------------------
 */

protected void
finalize()
{
    dispose();
}

/*
 *----------------------------------------------------------------------
 *
 * createCommands --
 *
 *	Create the build-in commands. These commands are loaded on
 *	demand -- the .class file of a Command class are loaded into
 *	the JVM the first time when the given command is executed.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Commands are registered.
 *
 *----------------------------------------------------------------------
 */

protected void 
createCommands() 
{
    Extension.loadOnDemand(this, "after",	  "tcl.lang.AfterCmd");
    Extension.loadOnDemand(this, "append",	  "tcl.lang.AppendCmd");
    Extension.loadOnDemand(this, "array",	  "tcl.lang.ArrayCmd");
    Extension.loadOnDemand(this, "break",	  "tcl.lang.BreakCmd");
    Extension.loadOnDemand(this, "case",	  "tcl.lang.CaseCmd");
    Extension.loadOnDemand(this, "catch",	  "tcl.lang.CatchCmd");
    Extension.loadOnDemand(this, "cd",	  	  "tcl.lang.CdCmd");
    Extension.loadOnDemand(this, "close",	  "tcl.lang.CloseCmd");
    Extension.loadOnDemand(this, "continue",  	  "tcl.lang.ContinueCmd");
    Extension.loadOnDemand(this, "concat",	  "tcl.lang.ConcatCmd");
    Extension.loadOnDemand(this, "eof",	 	  "tcl.lang.EofCmd");
    Extension.loadOnDemand(this, "eval",	  "tcl.lang.EvalCmd");
    Extension.loadOnDemand(this, "error",	  "tcl.lang.ErrorCmd");
    Extension.loadOnDemand(this, "exec",	  "tcl.lang.ExecCmd");
    Extension.loadOnDemand(this, "exit",	  "tcl.lang.ExitCmd");
    Extension.loadOnDemand(this, "expr",	  "tcl.lang.ExprCmd");
    Extension.loadOnDemand(this, "fblocked", 	  "tcl.lang.FblockedCmd");
    Extension.loadOnDemand(this, "fconfigure",	  "tcl.lang.FconfigureCmd");
    Extension.loadOnDemand(this, "file",      	  "tcl.lang.FileCmd");
    Extension.loadOnDemand(this, "flush",    	  "tcl.lang.FlushCmd");
    Extension.loadOnDemand(this, "for",      	  "tcl.lang.ForCmd");
    Extension.loadOnDemand(this, "foreach",  	  "tcl.lang.ForeachCmd");
    Extension.loadOnDemand(this, "format",   	  "tcl.lang.FormatCmd");
    Extension.loadOnDemand(this, "gets",   	  "tcl.lang.GetsCmd");
    Extension.loadOnDemand(this, "global",   	  "tcl.lang.GlobalCmd");
    Extension.loadOnDemand(this, "glob",     	  "tcl.lang.GlobCmd");
    Extension.loadOnDemand(this, "if",	  	  "tcl.lang.IfCmd");
    Extension.loadOnDemand(this, "incr",	  "tcl.lang.IncrCmd");
    Extension.loadOnDemand(this, "info",	  "tcl.lang.InfoCmd");
    Extension.loadOnDemand(this, "list",     	  "tcl.lang.ListCmd");
    Extension.loadOnDemand(this, "join",     	  "tcl.lang.JoinCmd");
    Extension.loadOnDemand(this, "lappend",	  "tcl.lang.LappendCmd");
    Extension.loadOnDemand(this, "lindex",	  "tcl.lang.LindexCmd");
    Extension.loadOnDemand(this, "linsert", 	  "tcl.lang.LinsertCmd");
    Extension.loadOnDemand(this, "llength", 	  "tcl.lang.LlengthCmd");
    Extension.loadOnDemand(this, "lrange",  	  "tcl.lang.LrangeCmd");
    Extension.loadOnDemand(this, "lreplace",	  "tcl.lang.LreplaceCmd");
    Extension.loadOnDemand(this, "lsearch",  	  "tcl.lang.LsearchCmd");
    Extension.loadOnDemand(this, "lsort",   	  "tcl.lang.LsortCmd");
    Extension.loadOnDemand(this, "namespace",	  "tcl.lang.NamespaceCmd");
    Extension.loadOnDemand(this, "open",    	  "tcl.lang.OpenCmd");
    Extension.loadOnDemand(this, "package",	  "tcl.lang.PackageCmd");
    Extension.loadOnDemand(this, "proc",	  "tcl.lang.ProcCmd");
    Extension.loadOnDemand(this, "puts",	  "tcl.lang.PutsCmd");
    Extension.loadOnDemand(this, "pwd",	  	  "tcl.lang.PwdCmd");
    Extension.loadOnDemand(this, "read",	  "tcl.lang.ReadCmd");
    Extension.loadOnDemand(this, "regsub",	  "tcl.lang.RegsubCmd");
    Extension.loadOnDemand(this, "rename",	  "tcl.lang.RenameCmd");
    Extension.loadOnDemand(this, "return",	  "tcl.lang.ReturnCmd");
    Extension.loadOnDemand(this, "scan",	  "tcl.lang.ScanCmd");
    Extension.loadOnDemand(this, "seek",	  "tcl.lang.SeekCmd");
    Extension.loadOnDemand(this, "set",	  	  "tcl.lang.SetCmd");
    Extension.loadOnDemand(this, "socket",	  "tcl.lang.SocketCmd");
    Extension.loadOnDemand(this, "source",	  "tcl.lang.SourceCmd");
    Extension.loadOnDemand(this, "split",	  "tcl.lang.SplitCmd");
    Extension.loadOnDemand(this, "string",	  "tcl.lang.StringCmd");
    Extension.loadOnDemand(this, "subst",	  "tcl.lang.SubstCmd");
    Extension.loadOnDemand(this, "switch",	  "tcl.lang.SwitchCmd");
    Extension.loadOnDemand(this, "tell",  	  "tcl.lang.TellCmd");
    Extension.loadOnDemand(this, "testcmdinfo", 
	    "tcl.lang.TestcmdinfoCmd");
    Extension.loadOnDemand(this, "testdcall", 
	    "tcl.lang.TestdcallCmd");
    Extension.loadOnDemand(this, "testfhandle", 
	    "tcl.lang.TestfhandleCmd");
    Extension.loadOnDemand(this, "testgetplatform", 
	    "tcl.lang.TestgetplatformCmd");
    Extension.loadOnDemand(this, "testsetplatform", 
	    "tcl.lang.TestsetplatformCmd");
    Extension.loadOnDemand(this, "testtranslatefilename", 
	    "tcl.lang.TesttranslatefilenameCmd");
    Extension.loadOnDemand(this, "time",	  "tcl.lang.TimeCmd");
    Extension.loadOnDemand(this, "trace",	  "tcl.lang.TraceCmd");
    Extension.loadOnDemand(this, "unset",	  "tcl.lang.UnsetCmd");
    Extension.loadOnDemand(this, "update",	  "tcl.lang.UpdateCmd");
    Extension.loadOnDemand(this, "uplevel",	  "tcl.lang.UplevelCmd");
    Extension.loadOnDemand(this, "upvar",	  "tcl.lang.UpvarCmd");
    Extension.loadOnDemand(this, "vwait",	  "tcl.lang.VwaitCmd");
    Extension.loadOnDemand(this, "while",	  "tcl.lang.WhileCmd");

    /*
     * Commands in the Java package.
     */

    try {
	(new BlendExtension()).init(this);
	RegexpCmd.init(this);
    } catch (TclException e) {
	System.out.println(getResult());
	e.printStackTrace();
	throw new TclRuntimeError("unexpected TclException: " + e);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * setAssocData --
 *
 *	Creates a named association between user-specified data and
 *	this interpreter. If the association already exists the data
 *	is overwritten with the new data. The data.deleteAssocData()
 *	method will be invoked when the interpreter is deleted.
 *
 *	NOTE: deleteAssocData() is not called when an old data is
 *	replaced by a new data. Caller of setAssocData() is
 *	responsible with deleting the old data.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Sets the associated data, creates the association if needed.
 *
 *----------------------------------------------------------------------
 */

public void
setAssocData(
    String name,		// Name for association.
    AssocData data)		// Object associated with the name.
{
    if (assocDataTab == null) {
	assocDataTab = new Hashtable();
    }
    assocDataTab.put(name, data);
}

/*
 *----------------------------------------------------------------------
 *
 * deleteAssocData --
 *
 *	Deletes a named association of user-specified data with
 *	the specified interpreter.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Deletes the association.
 *
 *----------------------------------------------------------------------
 */

public void
deleteAssocData(
    String name)		// Name of association.
{
    if (assocDataTab == null) {
	return;
    }

    assocDataTab.remove(name);
}

/*
 *----------------------------------------------------------------------
 *
 * getAssocData --
 *
 *	Returns the AssocData instance associated with this name in
 *	the specified interpreter.
 *
 * Results:
 *	The AssocData instance in the AssocData record denoted by the
 *	named association, or null.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

public AssocData
getAssocData(
    String name)			// Name of association.
{
    if (assocDataTab == null) {
	return null;
    } else {
	return (AssocData)assocDataTab.get(name);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * backgroundError --
 *
 *	This procedure is invoked to handle errors that occur in Tcl
 *	commands that are invoked in "background" (e.g. from event or
 *	timer bindings).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The command "bgerror" is invoked later as an idle handler to
 *	process the error, passing it the error message.  If that fails,
 *	then an error message is output on stderr.
 *
 *----------------------------------------------------------------------
 */

public void
backgroundError()
{
    BgErrorMgr mgr = (BgErrorMgr)getAssocData("tclBgError");
    if (mgr == null) {
	mgr = new BgErrorMgr(this);
	setAssocData("tclBgError", mgr);
    }
    mgr.addBgError();
}

/*-----------------------------------------------------------------
 *
 *	                     VARIABLES
 *
 *-----------------------------------------------------------------
 */

/*
 *----------------------------------------------------------------------
 *
 * setVar --
 *
 *	Sets a variable whose name and value are stored in TclObject.
 *
 * Results:
 *	The TclObject, as it was set is returned.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

final TclObject 
setVar(
    TclObject nameObj,		// Name of variable, array, or array element
				// to set.
    TclObject value,		// New value for variable.
    int flags)			// Various flags that tell how to set value:
				// any of GLOBAL_ONLY, NAMESPACE_ONLY,
				// APPEND_VALUE, or LIST_ELEMENT. 
throws 
    TclException 
{
    return varFrame.setVar(nameObj, value, flags);
}

/*
 *----------------------------------------------------------------------
 *
 * setVar --
 *
 *	Set the value of a variable.
 *
 * Results:
 *	Returns the new value of the variable.
 *
 * Side effects:
 *	May trigger traces.
 *
 *----------------------------------------------------------------------
 */

public final 
TclObject 
setVar(
    String name,		// Name of variable, array, or array element
				// to set.
    TclObject value,		// New value for variable.
    int flags)			// Various flags that tell how to set value:
				// any of GLOBAL_ONLY, NAMESPACE_ONLY,
				// APPEND_VALUE, or LIST_ELEMENT. 
throws
    TclException 
{
    return varFrame.setVar(name, value, flags);
}

/*
 *----------------------------------------------------------------------
 *
 * setVar --
 *
 *	Set the value of a variable.
 *
 * Results:
 *	Returns the new value of the variable.
 *
 * Side effects:
 *	May trigger traces.
 *
 *----------------------------------------------------------------------
 */

public final TclObject 
setVar(
    String name1,		// If name2 is null, this is name of a scalar
				// variable. Otherwise it is the name of an
				// array. 
    String name2,		// Name of an element within an array, or
				// null.
    TclObject value,		// New value for variable.
    int flags)			// Various flags that tell how to set value:
				// any of GLOBAL_ONLY, NAMESPACE_ONLY,
				// APPEND_VALUE or LIST_ELEMENT.
throws
    TclException
{
    return varFrame.setVar(name1, name2, value, flags);
}

/*
 *----------------------------------------------------------------------
 *
 * setVar --
 *
 *	Set the value of a variable.
 *
 * Results:
 *	Returns the new value of the variable.
 *
 * Side effects:
 *	May trigger traces.
 *
 *----------------------------------------------------------------------
 */

final void
setVar(
    String name,		// Name of variable, array, or array element
				// to set.
    String strValue,		// New value for variable.
    int flags)			// Various flags that tell how to set value:
				// any of GLOBAL_ONLY, NAMESPACE_ONLY,
				// APPEND_VALUE, orLIST_ELEMENT. 
throws 
    TclException 
{
    varFrame.setVar(name, TclString.newInstance(strValue), flags);
}

/*
 *----------------------------------------------------------------------
 *
 * setVar --
 *
 *	Set the value of a variable.
 *
 * Results:
 *	Returns the new value of the variable.
 *
 * Side effects:
 *	May trigger traces.
 *
 *----------------------------------------------------------------------
 */

final void
setVar(
    String name1,		// If name2 is null, this is name of a scalar
				// variable. Otherwise it is the name of an
				// array.  
    String name2,		// Name of an element within an array, or
				// null. 
    String strValue,		// New value for variable. 
    int flags)			// Various flags that tell how to set value:
				// any of GLOBAL_ONLY, NAMESPACE_ONLY,
				// APPEND_VALUE, or LIST_ELEMENT.
throws 
    TclException
{
    varFrame.setVar(name1, name2, TclString.newInstance(strValue),
	    flags);
}

/*
 *----------------------------------------------------------------------
 *
 * getVar --
 *
 *	Get the value of a variable.
 *
 * Results:
 *	Returns the value of the variable.
 *
 * Side effects:
 *	May trigger traces.
 *
 *----------------------------------------------------------------------
 */

final TclObject 
getVar(
    TclObject nameObj,		// The name of a variable, array, or array
				// element.
    int flags)			// Various flags that tell how to get value:
				// any of GLOBAL_ONLY or NAMESPACE_ONLY.
throws
    TclException 
{
    return varFrame.getVar(nameObj, flags);
}

/*
 *----------------------------------------------------------------------
 *
 * getVar --
 *
 *	Get the value of a variable.
 *
 * Results:
 *	Returns the value of the variable.
 *
 * Side effects:
 *	May trigger traces.
 *
 *----------------------------------------------------------------------
 */

public final TclObject
getVar(
    String name,		// The name of a variable, array, or array
				// element.
    int flags)			// Various flags that tell how to get value:
				// any of GLOBAL_ONLY or NAMESPACE_ONLY.
throws
    TclException
{
    return varFrame.getVar(name, flags);
}

/*
 *----------------------------------------------------------------------
 *
 * getVar --
 *
 *	Get the value of a variable.
 *
 * Results:
 *	Returns the value of the variable.
 *
 * Side effects:
 *	May trigger traces.
 *
 *----------------------------------------------------------------------
 */

public final TclObject
getVar(
    String name1,		// If name2 is null, this is name of a scalar
				// variable. Otherwise it is the name of an
				// array. 
    String name2,		// Name of an element within an array, or
				// null.
    int flags)			// Flags that tell how to get value:
				// GLOBAL_ONLY or NAMESPACE_ONLY. 
throws
    TclException
{
    return varFrame.getVar(name1, name2, flags);
}

/*
 *----------------------------------------------------------------------
 *
 * unsetVar --
 *
 *	Unset a variable.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	May trigger traces.
 *
 *----------------------------------------------------------------------
 */

final void
unsetVar(
    TclObject nameObj,		// The name of a variable, array, or array
				// element.
    int flags)			// Various flags that tell how to get value:
				// any of GLOBAL_ONLY or NAMESPACE_ONLY.
throws 
    TclException
{
    varFrame.unsetVar(nameObj, flags);
}

/*
 *----------------------------------------------------------------------
 *
 * unsetVar --
 *
 *	Unset a variable.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	May trigger traces.
 *
 *----------------------------------------------------------------------
 */

public final void
unsetVar(
    String name,		// The name of a variable, array, or array
				// element.
    int flags)			// Various flags that tell how to get value:
				// any of GLOBAL_ONLY or NAMESPACE_ONLY.
throws 
    TclException 
{
    varFrame.unsetVar(name, flags);
}

/*
 *----------------------------------------------------------------------
 *
 * unsetVar --
 *
 *	Unset a variable.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	May trigger traces.
 *
 *----------------------------------------------------------------------
 */

public final void
unsetVar(
    String name1,		// If name2 is null, this is name of a scalar
				// variable. Otherwise it is the name of an
				// array. 
    String name2,		// Name of an element within an array, or
				// null.
    int flags)			// Flags that tell how to get value:
				// GLOBAL_ONLY or NAMESPACE_ONLY. 
throws
    TclException
{
    varFrame.unsetVar(name1, name2, flags);
}

/*
 *----------------------------------------------------------------------
 *
 * traceVar --
 *
 *	Add a trace to a variable.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

void
traceVar(
    TclObject nameObj,		// Name of variable;  may end with "(index)"
				// to signify an array reference.
    VarTrace trace,		// Object to notify when specified ops are
				// invoked upon varName.
    int flags)			// OR-ed collection of bits, including any
				// of TCL.TRACE_READS, TCL.TRACE_WRITES,
				// TCL.TRACE_UNSETS, TCL.GLOBAL_ONLY,
				// TCL.NAMESPACE_ONLY.
throws
    TclException
{
    varFrame.traceVar(nameObj, trace, flags);
}

/*
 *----------------------------------------------------------------------
 *
 * traceVar --
 *
 *	Add a trace to a variable.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

public void
traceVar(
    String name,		// Name of variable;  may end with "(index)"
				// to signify an array reference.
    VarTrace trace,		// Object to notify when specified ops are
				// invoked upon varName.
    int flags)			// OR-ed collection of bits, including any
				// of TCL.TRACE_READS, TCL.TRACE_WRITES,
				// TCL.TRACE_UNSETS, TCL.GLOBAL_ONLY,
				// TCL.NAMESPACE_ONLY.
throws
    TclException
{
    varFrame.traceVar(name, trace, flags);
}

/*
 *----------------------------------------------------------------------
 *
 * traceVar --
 *
 *	Add a trace to a variable.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

public void
traceVar(
    String part1,		// Name of scalar variable or array.
    String part2,		// Name of element within array;  null means
				// trace applies to scalar variable or array
				// as-a-whole.  
    VarTrace trace,		// Object to notify when specified ops are
				// invoked upon varName.
    int flags)			// OR-ed collection of bits, including any
				// of TCL.TRACE_READS, TCL.TRACE_WRITES,
				// TCL.TRACE_UNSETS, TCL.GLOBAL_ONLY, and
				// TCL.NAMESPACE_ONLY.
throws
    TclException
{
    varFrame.traceVar(part1, part2, trace, flags);
}

/*
 *----------------------------------------------------------------------
 *
 * untraceVar --
 *
 *	Remove a trace from a variable.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

void
untraceVar(
    TclObject nameObj,		// Name of variable;  may end with "(index)"
				// to signify an array reference.
    VarTrace trace,		// Object associated with trace.
    int flags)			// OR-ed collection of bits describing current
				// trace, including any of TCL.TRACE_READS,
				// TCL.TRACE_WRITES, TCL.TRACE_UNSETS,
				// TCL.GLOBAL_ONLY and TCL.NAMESPACE_ONLY.
{
    varFrame.untraceVar(nameObj, trace, flags);
}

/*
 *----------------------------------------------------------------------
 *
 * untraceVar --
 *
 *	Remove a trace from a variable.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

public void
untraceVar(
    String name,		// Name of variable;  may end with "(index)"
				// to signify an array reference.
    VarTrace trace,		// Object associated with trace.
    int flags)			// OR-ed collection of bits describing current
				// trace, including any of TCL.TRACE_READS,
				// TCL.TRACE_WRITES, TCL.TRACE_UNSETS,
				// TCL.GLOBAL_ONLY and TCL.NAMESPACE_ONLY.
{
    varFrame.untraceVar(name, trace, flags);
}

/*
 *----------------------------------------------------------------------
 *
 * untraceVar --
 *
 *	Remove a trace from a variable.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

public void 
untraceVar(
    String part1,		// Name of scalar variable or array.
    String part2,		// Name of element within array;  null means
				// trace applies to scalar variable or array
				// as-a-whole.  
    VarTrace trace,		// Object associated with trace.
    int flags)			// OR-ed collection of bits describing current
				// trace, including any of TCL.TRACE_READS,
				// TCL.TRACE_WRITES, TCL.TRACE_UNSETS,
				// TCL.GLOBAL_ONLY and TCL.NAMESPACE_ONLY.
{
    varFrame.untraceVar(part1, part2, trace, flags);
}

/*
 *----------------------------------------------------------------------
 *
 * createCommand --
 *
 *	Define a new command in the interpreter.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If a command named cmdName already exists for interp, it is
 *	deleted. In the future, when cmdName is seen as the name of a
 *	command by eval(), cmd will be called. When the command is
 *	deleted from the table, cmd.disposeCmd() be called if cmd
 *	implements the CommandWithDispose interface.
 *
 *----------------------------------------------------------------------
 */

public void
createCommand(
    String cmdName,		// Name of command.
    Command cmd)		// Command object to associate with
				// cmdName.
{
    Command oldCmd = (Command)cmdTable.get(cmdName);

    if (oldCmd != null && oldCmd instanceof CommandWithDispose) {
	((CommandWithDispose)oldCmd).disposeCmd();
    }
    cmdTable.put(cmdName, cmd);
}

/*
 *----------------------------------------------------------------------
 *
 * deleteCommand --
 *
 *	Remove the given command from the given interpreter.
 *
 * Results:
 *	0 is returned if the command was deleted successfully.
 *	-1 is returned if there didn't exist a command by that
 *	name.
 *
 * Side effects:
 *	CmdName will no longer be recognized as a valid command for
 *	the interpreter.
 *
 *----------------------------------------------------------------------
 */

public int
deleteCommand(
    String cmdName)		// Name of command to remove.
{
    Command cmd = (Command)cmdTable.get(cmdName);
    if (cmd == null) {
	return -1;
    } else {
	cmdTable.remove(cmdName);
	if (cmd instanceof CommandWithDispose) {
	    ((CommandWithDispose)cmd).disposeCmd();
	}
	return 0;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * getCommand --
 *
 *	Returns the command procedure of the given command.
 *
 * Results:
 *	The command procedure of the given command, or null if
 *      the command doesn't exist.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

public Command
getCommand(
    String name) 		// String name of the command.
{
    return (Command)cmdTable.get(name);
}

/*
 *----------------------------------------------------------------------
 *
 * commandComplete --
 *
 *	Check if the string is a complete Tcl command string.
 *
 * Result:
 *	A boolean value indicating whether the string is a complete Tcl
 *	command string.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

public static boolean
commandComplete(
    String string)		// The string to check.
{
    return Parser.commandComplete(string, string.length());
}


/*-----------------------------------------------------------------
 *
 *	                     EVAL
 *
 *-----------------------------------------------------------------
 */


/*
 *----------------------------------------------------------------------
 *
 * getResult --
 *
 *	Queries the value of the result.
 *
 * Results:
 *	The current result in the interpreter.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

public  final TclObject
getResult() 
{
    return m_result;
}

/*
 *----------------------------------------------------------------------
 *
 * setResult --
 *
 *	Arrange for the given Tcl Object to be placed as the result 
 *	object for the interpreter.  Convenience functions are also
 *	available to create a Tcl Object out of the most common Java
 *	types.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The object result for the interpreter is updated.
 *
 *----------------------------------------------------------------------
 */

public final void 
setResult(
    TclObject r)	// A Tcl Object to be set as the result.
{
    if (r == null) {
	throw new NullPointerException(
		"Interp.setResult() called with null TclObject argument.");
    }
    if (m_result != m_nullResult) {
	m_result.release();
    }

    m_result = r;
    m_result.preserve();
}

/*
 *----------------------------------------------------------------------
 *
 * setResult --
 *
 *	Arrange for the given Tcl Object to be placed as the result 
 *	object for the interpreter.  Convenience functions are also
 *	available to create a Tcl Object out of the most common Java
 *	types.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The object result for the interpreter is updated.
 *
 *----------------------------------------------------------------------
 */

public final void 
setResult(
    String r)		// A string result.
{
    if (r == null) {
	resetResult();
    } else {
	setResult(TclString.newInstance(r));
    }
}

/*
 *----------------------------------------------------------------------
 *
 * setResult --
 *
 *	Arrange for the given Tcl Object to be placed as the result 
 *	object for the interpreter.  Convenience functions are also
 *	available to create a Tcl Object out of the most common Java
 *	types.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The object result for the interpreter is updated.
 *
 *----------------------------------------------------------------------
 */

public final void 
setResult(
    int r)		// An int result.
{
    setResult(TclInteger.newInstance(r));
}

/*
 *----------------------------------------------------------------------
 *
 * setResult --
 *
 *	Arrange for the given Tcl Object to be placed as the result 
 *	object for the interpreter.  Convenience functions are also
 *	available to create a Tcl Object out of the most common Java
 *	types.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The object result for the interpreter is updated.
 *
 *----------------------------------------------------------------------
 */

public final void 
setResult(
    double r)		// A double result.
{
    setResult(TclDouble.newInstance(r));
}

/*
 *----------------------------------------------------------------------
 *
 * setResult --
 *
 *	Arrange for the given Tcl Object to be placed as the result 
 *	object for the interpreter.  Convenience functions are also
 *	available to create a Tcl Object out of the most common Java
 *	types.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The object result for the interpreter is updated.
 *
 *----------------------------------------------------------------------
 */

public final void 
setResult(
    boolean r)		// A boolean result.
{
    setResult(TclBoolean.newInstance(r));
}

/*
 *----------------------------------------------------------------------
 *
 * resetResult --
 *
 *	This procedure resets the interpreter's result object.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	It resets the result object to an unshared empty object. It 
 *	also clears any error information for the interpreter.
 *
 *----------------------------------------------------------------------
 */

public final void 
resetResult() 
{
    if (m_result != m_nullResult) {
	m_result.release();
	m_result = m_nullResult;
    }
    errAlreadyLogged = false;
    errInProgress = false;
    errCodeSet = false;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AppendElement --
 *
 *	Convert a string to a valid Tcl list element and append it to the
 *	result (which is ostensibly a list).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The result in the interpreter given by the first argument is
 *	extended with a list element converted from string. A separator
 *	space is added before the converted list element unless the current
 *	result is empty, contains the single character "{", or ends in " {".
 *
 *	If the string result is empty, the object result is moved to the
 *	string result, then the object result is reset.
 *
 *----------------------------------------------------------------------
 */

void
appendElement(
    String string)		/* String to convert to list element and
				 * add to result. */
throws 
    TclException
{
    TclObject result;

    result = getResult();
    result.preserve();
    result = result.takeExclusive();
    TclList.append(this, result, TclString.newInstance(string));
    setResult(result.toString());
    result.release();
}

/*
 *----------------------------------------------------------------------
 *
 * eval --
 *
 *	Execute a Tcl command in a string or Tcl Object.
 *
 * Results:
 *	The return value is void.  However, a standard Tcl Exception
 *	may be generated.  The interpreter's result object will contain
 *	the value of the evaluation but will persist only until the next 
 *	call to one of the eval functions.
 *
 * Side effects:
 *	The side effects will be determined by the exact Tcl code to be 
 *	evaluated.
 *
 *----------------------------------------------------------------------
 */

public void 
eval(
    String string,	// A script to evaluate.
    int flags)		// Flags, either 0 or TCL_GLOBAL_ONLY.
throws 
    TclException 	// A standard Tcl exception.
{
    CharPointer script;
    
    script = new CharPointer(string);
    Parser.eval2(this, script, script.length(), flags);
}

/*
 *----------------------------------------------------------------------
 *
 * eval --
 *
 *	Execute a Tcl command in a string or Tcl Object.
 *
 * Results:
 *	The return value is void.  However, a standard Tcl Exception
 *	may be generated.  The interpreter's result object will contain
 *	the value of the evaluation but will persist only until the next 
 *	call to one of the eval functions.
 *
 * Side effects:
 *	The side effects will be determined by the exact Tcl code to be 
 *	evaluated.
 *
 *----------------------------------------------------------------------
 */

public void 
eval(
    String script)	// A script to evaluate.
throws 
    TclException 	// A standard Tcl exception.
{
    eval(script, 0);
}

/*
 *----------------------------------------------------------------------
 *
 * eval --
 *
 *	Execute a Tcl command in a string or Tcl Object.
 *
 * Results:
 *	The return value is void.  However, a standard Tcl Exception
 *	may be generated.  The interpreter's result object will contain
 *	the value of the evaluation but will persist only until the next 
 *	call to one of the eval functions.
 *
 * Side effects:
 *	The side effects will be determined by the exact Tcl code to be 
 *	evaluated.
 *
 *----------------------------------------------------------------------
 */

public void 
eval(
    TclObject tobj,	// A Tcl object holding a script to evaluate.
    int flags)		// Flags, either 0 or TCL_GLOBAL_ONLY.
throws 
    TclException 	// A standard Tcl exception.
{
    eval(tobj.toString(), flags);
}

/*
 *----------------------------------------------------------------------
 *
 * evalFile --
 *	Loads a Tcl script from a file and evaluates it in the
 * 	current interpreter.
 *
 * Results:
 * 	None.
 *
 * Side effects:
 *	The side effects will be determined by the exact Tcl code to be 
 *	evaluated.
 *
 *----------------------------------------------------------------------
 */

public void
evalFile(
    String s)			// The name of file to evaluate.
throws 
    TclException
{
    String fileContent;		// Contains the content of the file.

    fileContent = readScriptFromFile(s);

    if (fileContent == null) {
	throw new TclException(this, "couldn't read file \"" + s + "\"");
    }

    String oldScript = scriptFile;
    scriptFile = s;

    try {
	pushDebugStack(s, 1);
	eval(fileContent, 0);
    }
    finally {
	scriptFile = oldScript;
	popDebugStack();
    }
}

/*
 *----------------------------------------------------------------------
 *
 * evalURL --
 *
 *	Loads a Tcl script from a URL and evaluate it in the
 *	current interpreter.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The side effects will be determined by the exact Tcl code to be 
 *	evaluated.
 *
 *----------------------------------------------------------------------
 */

void
evalURL(
    URL context, 		// URL context under which s is interpreted.
    String s)  			// The name of URL.
throws 
    TclException
{
    String fileContent;		// Contains the content of the file.

    fileContent = readScriptFromURL(context, s);
    if (fileContent == null) {
	throw new TclException(this, "cannot read URL \"" + s + "\"");
    }

    String oldScript = scriptFile;
    scriptFile = s;

    try {
	eval(fileContent, 0);
    }
    finally {
	scriptFile = oldScript;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * readScriptFromFile --
 *
 *	Read the script file into a string.
 *
 * Results:
 *	Returns the content of the script file.
 *
 * Side effects:
 *	If a new File object cannot be created for s, the result is reset.
 *
 *----------------------------------------------------------------------
 */

private String
readScriptFromFile(
    String s)			// The name of the file.
{
    File sourceFile;
    FileInputStream fs;
    try {
	sourceFile = FileUtil.getNewFileObj(this, s);
	fs = new FileInputStream(sourceFile);
    } catch (TclException e) {
	resetResult();	
	return null;
    } catch (FileNotFoundException e) {
	return null;
    } catch (SecurityException sec_e) {
	return null;
    }

    try {
	byte charArray[] = new byte[fs.available()];
	fs.read(charArray);
	return new String(charArray);
    } catch (IOException e) {
	return null;
    } finally {
	closeInputStream(fs);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * readScriptFromURL --
 *
 *	Read the script file into a string, treating the file as 
 *	an URL.
 *
 * Results:
 *	The content of the script file.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

private String
readScriptFromURL(
    URL context, 	// Use as the URL context if s is a relative URL.
    String s)		// ???
{
    Object content = null;
    URL url;

    try {
	url = new URL(context, s);
    }
    catch (MalformedURLException e1) {
	return null;
    }

    try {
	content = url.getContent();
    }
    catch (IOException e2) {
	return null;
    }
    catch (SecurityException sec_e) {
	return null;
    }

    if (content instanceof String) {
	return (String)content;
    } else if (content instanceof InputStream) {
	InputStream fs = (InputStream)content;

	try {
	    byte charArray[] = new byte[fs.available()];
	    fs.read(charArray);
	    return new String(charArray);
	} catch (IOException e) {
	    return null;
	} finally {
	    closeInputStream(fs);
	}
    } else {
	return null;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * closeInputStream --
 *
 *	Close the InputStream; catch any IOExceptions and ignore them.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

private void
closeInputStream(
    InputStream fs)
{
    try {
	fs.close();
    }
    catch (IOException e) {;}
}

/*
 *----------------------------------------------------------------------
 *
 * evalResource --
 *
 *	Execute a Tcl script stored in the given Java resource location.
 *
 * Results:
 *	The return value is void.  However, a standard Tcl Exception
 *	may be generated. The interpreter's result object will contain
 *	the value of the evaluation but will persist only until the next 
 *	call to one of the eval functions.
 *
 * Side effects:
 *	The side effects will be determined by the exact Tcl code to be 
 *	evaluated.
 *
 *----------------------------------------------------------------------
 */

void 
evalResource(
    String resName) 	// The location of the Java resource. See
			// the Java documentation of
			// Class.getResourceAsStream()
			// for details on resources naming.
throws 
    TclException
{
    InputStream stream = this.getClass().getResourceAsStream(resName);
    if (stream == null) {
	throw new TclException(this, "cannot read resource \"" + resName
		+ "\"");
    }

    try {
	byte charArray[] = new byte[stream.available()];
	stream.read(charArray);
	eval(new String(charArray), 0);
    } catch (IOException e) {
	return;
    } finally {
	closeInputStream(stream);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * backslash --
 *
 *	Figure out how to handle a backslash sequence.  The index
 *	of the ChapPointer must be pointing to the first /.
 *
 * Results:
 *	The return value is an instance of BackSlashResult that 
 *	contains the character that should be substituted in place 
 *	of the backslash sequence that starts at src.index, and
 *	an index to the next character after the backslash sequence.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static BackSlashResult 
backslash(
    String s, 
    int i, 
    int len)
{
     CharPointer script;

     script = new CharPointer(s.substring(0, len));
     script.index = i;
     return(Parser.backslash(script));
}

/*
 *----------------------------------------------------------------------
 *
 * setErrorCode --
 *
 *	This procedure is called to record machine-readable information
 *	about an error that is about to be returned. The caller should
 *	build a list object up and pass it to this routine.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The errorCode global variable is modified to be the new value.
 *	A flag is set internally to remember that errorCode has been
 *	set, so the variable doesn't get set automatically when the
 *	error is returned.
 *
 *	If the errorCode variable have write traces, any arbitrary side
 *	effects may happen in those traces. TclException's caused by the
 *	traces, however, are ignored and not passed back to the caller
 *	of this function.
 *
 *----------------------------------------------------------------------
 */
public void
setErrorCode(
    TclObject code)		// The errorCode object.
{
    try {
	setVar("errorCode", null, code, TCL.GLOBAL_ONLY);
	errCodeSet = true;
    } catch (TclException excp) {
	/*
	 * Ignore any TclException's, possibly caused by variable traces on
	 * the errorCode variable. This is compatible with the behavior of
	 * the Tcl C API.
	 */
    }
}

/*
 *----------------------------------------------------------------------
 *
 * addErrorInfo --
 *
 *	Add information to the "errorInfo" variable that describes the
 *	current error.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The contents of message are added to the "errorInfo" variable.
 *	If eval() has been called since the current value of errorInfo
 *	was set, errorInfo is cleared before adding the new message.
 *	If we are just starting to log an error, errorInfo is initialized
 *	from the error message in the interpreter's result.
 *
 *	If the errorInfo variable have write traces, any arbitrary side
 *	effects may happen in those traces. TclException's caused by the
 *	traces, however, are ignored and not passed back to the caller
 *	of this function.
 *
 *----------------------------------------------------------------------
 */

public void
addErrorInfo(
    String message)		// The message to record.
{
    int flags = 0;

    if (!errInProgress) {
	errInProgress = true;

	try {
	    setVar("errorInfo", null, getResult().toString(),
		   TCL.GLOBAL_ONLY);
	} catch (TclException e1) {
	    /*
	     * Ignore (see try-block above).
	     */
	}

	/*
	 * If the errorCode variable wasn't set by the code
	 * that generated the error, set it to "NONE".
	 */

	if (!errCodeSet) {
	    try {
		setVar("errorCode", null, "NONE", TCL.GLOBAL_ONLY);
	    } catch (TclException e1) {
		/*
		 * Ignore (see try-block above).
		 */
	    }
	}
    }

    try {
	setVar("errorInfo", null, message,
		TCL.APPEND_VALUE|TCL.GLOBAL_ONLY);
    }  catch (TclException e1) {
	/*
	 * Ignore (see try-block above).
	 */
    }
}

/*
 *----------------------------------------------------------------------
 *
 * updateReturnInfo --
 *
 *	This procedure is called when procedures return, and at other
 *	points where the TCL.RETURN code is used. It examines fields
 *	such as interp.returnCode and interp.errorCode and modifies
 *	the real return status accordingly.
 *
 * Results:
 *	The return value is the true completion code to use for
 *	the procedure, instead of TCL.RETURN.
 *
 * Side effects:
 *	The errorInfo and errorCode variables may get modified.
 *
 *----------------------------------------------------------------------
 */

protected int
updateReturnInfo()
{
    int code;

    code = returnCode;
    returnCode = TCL.OK;

    if (code == TCL.ERROR) {
	try {
	    setVar("errorCode", null, (errorCode != null) ? errorCode : "NONE",
		    TCL.GLOBAL_ONLY);
	} catch (TclException e) {
	    /*
	     * An error may happen during a trace to errorCode. We ignore it.
	     * This may leave error messages inside Interp.result (which
	     * is compatible with Tcl 8.0 behavior.
	     */
	}
	errCodeSet = true;

	if (errorInfo != null) {
	    try {
		setVar("errorInfo", null, errorInfo, TCL.GLOBAL_ONLY);
	    } catch (TclException e) {
		/*
		 * An error may happen during a trace to errorInfo. We
		 * ignore it.  This may leave error messages inside
		 * Interp.result (which is compatible with Tcl 8.0
		 * behavior.
		 */
	    }
	    errInProgress = true;
	}
    }

    return code;
}

/*
 *----------------------------------------------------------------------
 *
 * newCallFrame --
 *
 *	Creates a new callframe. This method can be overrided to 
 *	provide debugging support.
 *
 * Results:
 *	A new CallFrame.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

protected CallFrame 
newCallFrame(
    Procedure proc, 		// The procedure which will later be 
				// execute inside the new callframe.
    TclObject argv[])  		// The arguments to pass to the procedure.
throws 
    TclException 		// Incorrect number of arguments passed.
{
    return new CallFrame(this, proc, argv);
}

/*
 *----------------------------------------------------------------------
 *
 * newCallFrame --
 *
 *	Creates a new callframe. This method can be overrided to 
 *	provide debugging support.
 *
 * Results:
 *	A new CallFrame.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

protected CallFrame 
newCallFrame() 
{
    return new CallFrame(this);
}

/*
 *----------------------------------------------------------------------
 *
 * getWorkingDir --
 *
 *	Retrieve the current working directory for this interpreter.
 *
 * Results:
 *	Returns the File for the directory.
 *
 * Side effects:
 *	If the working dir is null, set it.
 *
 *----------------------------------------------------------------------
 */

File
getWorkingDir()
{
    if (workingDir == null) {
	try {
	    String dirName = getVar("env", "HOME", 0).toString();
	    workingDir = FileUtil.getNewFileObj(this, dirName);
	} catch(TclException e) {
	    resetResult();
	}
	workingDir = new File(Util.tryGetSystemProperty("user.home", "."));
    }
    return workingDir;
}

/*
 *----------------------------------------------------------------------
 *
 * setWorkingDir --
 *
 *	Set the current working directory for this interpreter.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Set the working directory or throw a TclException.
 *
 *----------------------------------------------------------------------
 */

void
setWorkingDir(
    String dirName)
throws
    TclException
{
    File dirObj = FileUtil.getNewFileObj(this, dirName);

    /*
     *  Use the canonical name of the path, if possible.
     */

    try {
	dirObj = new File(dirObj.getCanonicalPath());
    } catch (IOException e) {
    }

    if (dirObj.isDirectory()) {
	workingDir = dirObj;
    } else {
	throw new TclException(this, 
		"couldn't change working directory to \""
                + dirObj.getName() + "\": no such file or directory");
    }    
}

/*
 *----------------------------------------------------------------------
 *
 * getNotifier --
 *
 *	Retrieve the Notifier associated with this Interp.
 *
 * Results:
 *	Returns the Notifier.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

public Notifier
getNotifier()
{
    return notifier;
}

/*
 *----------------------------------------------------------------------
 *
 * pkgProvide --
 *
 *	This procedure is invoked to declare that a particular version
 *	of a particular package is now present in an interpreter.  There
 *	must not be any other version of this package already
 *	provided in the interpreter.
 *
 * Results:
 *	Normally does nothing; if there is already another version
 *	of the package loaded then an error is raised.
 *
 * Side effects:
 *	The interpreter remembers that this package is available,
 *	so that no other version of the package may be provided for
 *	the interpreter.
 *
 *----------------------------------------------------------------------
 */
 
public final void
pkgProvide(
    String name, 
    String version)
throws 
    TclException
{
    PackageCmd.pkgProvide(this,name,version);
}

/*
 *----------------------------------------------------------------------
 *
 * pkgRequire --
 *
 *	This procedure is called by code that depends on a particular
 *	version of a particular package.  If the package is not already
 *	provided in the interpreter, this procedure invokes a Tcl script
 *	to provide it.  If the package is already provided, this
 *	procedure makes sure that the caller's needs don't conflict with
 *	the version that is present.
 *
 * Results:
 *	If successful, returns the version string for the currently
 *	provided version of the package, which may be different from
 *	the "version" argument.  If the caller's requirements
 *	cannot be met (e.g. the version requested conflicts with
 *	a currently provided version, or the required version cannot
 *	be found, or the script to provide the required version
 *	generates an error), a TclException is raised.
 *
 * Side effects:
 *	The script from some previous "package ifneeded" command may
 *	be invoked to provide the package.
 *
 *----------------------------------------------------------------------
 */

public final String
pkgRequire(
    String pkgname, 
    String version, 
    boolean exact)
throws
    TclException
{
    return PackageCmd.pkgRequire(this, pkgname, version, exact);
}

/*
 * Debugging API.
 *
 * The following section defines two debugging API functions for
 * logging information about the point of execution of Tcl scripts:
 *
 * - pushDebugStack() is called when a procedure body is
 *       executed, or when a file is source'd.
 *	   - popDebugStack() is called when the flow of control is about
 *       to return from a procedure body, or from a source'd file.
 *
 * Two other API functions are used to determine the current point of
 * execution:
 *
 *	   - getScriptFile() returns the script file current being executed.
 *	   - getArgLineNumber(i) returns the line number of the i-th argument
 *	     of the current command.
 *
 * Note: The point of execution is automatically maintained for
 *       control structures such as while, if, for and foreach,
 *	     as long as they use Interp.eval(argv[?]) to evaluate control
 *	     blocks.
 *	    
 *	     The case and switch commands need to set dbg.cmdLine explicitly
 *	     because they may evaluate control blocks that are not elements
 *	     inside the argv[] array. ** This feature not yet implemented. **
 *
 *	     The proc command needs to call getScriptFile() and
 *       getArgLineNumber(3) to find out the location of the proc
 *       body.
 *
 * The debugging API functions in the Interp class are just dummy stub
 * functions. These functions are usually implemented in a subclass of
 * Interp (e.g. DbgInterp) that has real debugging support.
 *
 */

protected DebugInfo dbg;

/*
 * Initialize the debugging information.
 * @return a DebugInfo object used by Interp in non-debugging mode.
 */
protected DebugInfo 
initDebugInfo() 
{
    return new DebugInfo(null, 1);
}

/*
 * Add more more level at the top of the debug stack.
 *
 * @param fileName the filename for the new stack level
 * @param lineNumber the line number at which the execution of the
 *	   new stack level begins.
 */
void pushDebugStack(
    String fileName, 
    int lineNumber)
{
    // do nothing.
}

/*
 * Remove the top-most level of the debug stack.
 */
void 
popDebugStack() 
throws
    TclRuntimeError {
    // do nothing
}
/*
 * Returns the name of the script file currently under execution.
 *
 * @return the name of the script file currently under execution.
 */
String getScriptFile() 
{
    return dbg.fileName;
}
/*
 * Returns the line number where the given command argument begins. E.g, if
 * the following command is at line 10:
 *
 *	foo {a
 *      b } c
 *
 * getArgLine(0) = 10
 * getArgLine(1) = 10
 * getArgLine(2) = 11
 * 
 * @param index specifies an argument.
 * @return the line number of the given argument.
 */   
int 
getArgLineNumber(
    int index)
{
    return 0;
}


} // end Interp
