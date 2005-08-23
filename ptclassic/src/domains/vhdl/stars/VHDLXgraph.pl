defstar {
	name { Xgraph }
	domain { VHDL }
	version { @(#)VHDLXgraph.pl	1.7 10/01/96 }
	desc {
Graph the input data.
	}
	author { Michael C. Williamson, J. T. Buck and E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	htmldoc {
	}
	input {
	  name{ input }
	  type{ float }
	}
	defstate {
	  name { title }
	  type { string }
	  default { "Ptolemy Xgraph" }
	  desc { Title for the plot. }
	}
	defstate {
	  name { options }
	  type { string }
	  default { "-bb -tk =800x400" }
	  desc { Command line options for the xgraph program. }
	}
	constructor {
		noInternalState();
	}
	codeblock (uses) {
use std.Textio.all;
	}
	codeblock (init) {
variable $starSymbol(space): character := ' ';
variable $starSymbol(aline): line;
variable $starSymbol(icount): integer := 0;
	}
	codeblock (graph) {
! (pxgraph -t "$val(title)" $val(options) $starSymbol(xgraph).dat ; rm -f $starSymbol(xgraph).dat) &
	}
	initCode {
	  addCode(uses, "useLibs", "textio");
	  StringList filedecl = "";
	    filedecl << "file $starSymbol(fileout";
	    filedecl << "): text is out \"$starSymbol(xgraph";
	    filedecl << ").dat\";";
	    filedecl << "\n";
	  addCode(filedecl, "mainDecls");
	  addCode(init, "mainDecls");
/*
  StringList cmd = "";
	  cmd << "/bin/rm -f ";
	  cmd << "$starSymbol(xgraph)";
	  cmd << ".dat";
	  cmd << "; ";
	    cmd << "/bin/cat ";
	    cmd << "$starSymbol(xgraph";
	    cmd <<  ").dat";
	    cmd << " >> ";
	    cmd << "$starSymbol(xgraph)";
	    cmd << ".dat";
	    cmd << "; ";
	    cmd << "/bin/echo \"\" >> ";
	    cmd << "$starSymbol(xgraph)";
	    cmd << ".dat";
	    cmd << "; ";
	    cmd << "/bin/rm -f ";
	    cmd << "$starSymbol(xgraph";
	    cmd << ").dat";
	    cmd << "; ";
	  addCode(cmd, "sysWrapup");
	  */
	  addCode(graph, "simWrapup");
	}
}
