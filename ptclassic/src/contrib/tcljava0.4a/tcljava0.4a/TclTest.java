/* 
 * TclTest.java --
 *
 *	This file is used to test the tcljava package.
 *
 * Copyright (c) 1995 by Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

import java.util.Enumeration;
import java.util.Date;
import tcl.*;

/*
 * This first callback object is used to invoke the widget demo, running
 * under Java.
 */
class Callback1 implements Command {
    /*
     * All callback objects have an invoke method for when they're called,
     * a deleted method for when they are removed from the interpreter,
     * and a finalized destructor method.
     */
    public Value invoke(Interp interp, Value args[])
	    throws Tcl_ErrorException
    {
	
	/*
	 * Eval the command, and ignore the result.
	 */
	interp.eval("source $tk_library/demos/widget");
	
	/*
	 * Make sure the window can be seen, and return the result of this
	 * operation.
	 */
	return interp.eval("wm deiconify .");
    }
    
    /* 
     * This method is called when the command is deleted from the
     * interpreter.
     */
    public void deleted(Interp interp) {
	System.out.println("deleting callback1...");
    }

    protected void finalize() { System.out.println("finalized Callback1"); }
}

class Callback2 implements Command {
    /*
     * Note that for this callback, we don't return a result, and
     * therefore don't need a Result object.
     */
    public Value invoke(Interp interp, Value args[])
    {
	Runtime rt = Runtime.getRuntime();
	rt.gc();
	return Value.EMPTY;
    }
    public void deleted(Interp interp) {
	System.out.println("deleting callback2...");
    }
    protected void finalize() { System.out.println("finalized Callback2"); }
}

class facCalc implements Command {
    /*
     * A silly little command to test call times.
     */
    private int fac(int x) {
	if (x < 2)
	    return 1;
	else
	    return x*fac(x-1);
    }
    public Value invoke(Interp interp, Value args[]) {
	return new Value(Integer.toString(fac(Integer.parseInt(args[1].toString()))));
    }
    public void deleted(Interp interp) {
    }
}
class Thrower implements Command
{
    public Value
    invoke(Interp interp, Value args[]) throws Tcl_ResultCodeException
    {
	if ("Break".equals(args[1].toString())) {
	    throw new Tcl_BreakException();
	} else if ("Continue".equals(args[1].toString())) {
	    throw new Tcl_ContinueException();
	} else if ("Error".equals(args[1].toString())) {
	    throw new Tcl_ErrorException("Error signalled on demand by "
					 + "Thrower.");
	} else if ("Return".equals(args[1].toString())) {
	    throw new Tcl_ReturnException("Result string supplied by "
					  + "Thrower.");
	} else if ("User".equals(args[1].toString())) {
	    throw new Tcl_UserException("Description supplied by Thrower.",
					34);
	} else {
	    return new Value("Nothing for Thrower to throw!");
	}
    }

    public void
    deleted(Interp interp) 
    {
    }
}

class TclTest {

    /* jdk1.1 gets errors about 'create' being undefined unless
     * we load the library at the top of main().
     */	
    //static {
    //   System.loadLibrary("jtk");
    //}

    /*
     * Eval the given command, printing it out on stdout, and 
     * also showing whether it was successful, or if it failed.
     */
    public static void evalPrint(Interp interp, String s) {
	try {
	    System.out.println("eval(" + s + ") succeed: "
                               + interp.eval(s, true));
	}
	catch (Tcl_ErrorException e) {
	    System.out.println("eval(" + s + ") failed: " + e
			       + ": code " + e.getCode()
			       + ", description " + e.getDescrip());
	}
	catch (Tcl_ResultCodeException e) {
	    System.out.println("eval(" + s + ") failed: " + e
			       + ": code " + e.getCode());
	}
    }

    public static void main(String args[]) {
	System.loadLibrary("jtk");
	/* 
	 * We create an interpreter, and initialize it to handle
	 * tk.
	 */
	Interp interp = new Interp();
	TkApplication.tkInit(interp);

	/* 
	 * Create the 3 commands in TCL, and associate them with the
	 * callback objects.
	 */
	interp.createCommand("callback1", new Callback1());
	interp.createCommand("callback2", new Callback2());
	interp.createCommand("javafac", new facCalc());
	interp.createCommand("throwException", new Thrower());
	
	/*
	 * Set up our demo application.
	 */
	evalPrint(interp, "toplevel .java");
	evalPrint(interp, "wm withdraw .");
	evalPrint(interp, "wm title .java {Java Demonstration}");
	evalPrint(interp, "button .java.b -text demo -command callback1");
	evalPrint(interp, "button .java.b2 -text delete -command {rename callback1 {}}");
	evalPrint(interp, "button .java.b3 -text gc -command callback2");
	evalPrint(interp, "button .java.b5 -text {Tcl console} -command { set argv0 JTk; set tcl_interactive 1;source tkcon.tcl}");
	evalPrint(interp, "pack .java.b .java.b2 .java.b3 .java.b5");

	/*
	 * Handle all the events that happen.  We put the event loop in Java, 
	 * instead of calling Tk_MainLoop so that we can allow threads.
	 * 
	 * BEWARE! Threads are dangerous with TK.  You *must* ensure that
	 * only *one* thread is doing tk/tcl calls, and is being called
	 * from tk/tcl.  If you have more than one thread doing anything
	 * with tk/tcl, you'll probably notice with the heinous core dumps
	 * your brand new app starts dumping.
	 * 
	 * You've been warned.
	 */
	TkApplication.mainLoop();
	
	/*
	 * We *only* get to this point when all the main windows in the
	 * Tk Application goes away, as contrasted with AWT that exits
	 * the main thread as soon as things are initialized.
	 */
	System.out.println("exiting...");
    }
}
