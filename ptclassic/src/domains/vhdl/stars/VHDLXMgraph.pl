defstar {
	name { XMgraph }
	domain { VHDL }
	version { $Id$ }
	desc {
Graph the input data.
	}
	author { Michael C. Williamson, J. T. Buck and E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	htmldoc {
	}
	inmulti {
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
	codeblock (std) {
$starSymbol(icount) := $starSymbol(icount) + 1;
	}
	codeblock (graph) {
! (pxgraph -t "$val(title)" $val(options) $starSymbol(xgraph).dat ; rm -f $starSymbol(xgraph).dat) &
	}
	initCode {
	  addCode(uses, "useLibs", "textio");
	  StringList filedecl = "";
	  for (int i = 1 ; i <= input.numberPorts() ; i++) {
	    filedecl << "file $starSymbol(fileout";
	    filedecl << i;
	    filedecl << "): text is out \"$starSymbol(xgraph";
	    filedecl << i;
	    filedecl << ").dat\";";
	    filedecl << "\n";
	  }
	  addCode(filedecl, "mainDecls");
	  addCode(init, "mainDecls");
	  StringList cmd = "";
	  cmd << "! /bin/rm -f ";
	  cmd << "$starSymbol(xgraph)";
	  cmd << ".dat";
	  cmd << "; ";
	  for (int j = 1 ; j <= input.numberPorts() ; j++) {
	    cmd << "/bin/cat ";
	    cmd << "$starSymbol(xgraph";
	    cmd << j << ").dat";
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
	    cmd << j << ").dat";
	    cmd << "; ";
	  }
	  addCode(cmd, "simWrapup");
	  addCode(graph, "simWrapup");
	}
	go {
	  StringList dataout = "";
	  for (int k = 1 ; k <= input.numberPorts() ; k++) {
	    dataout << "write($starSymbol(aline), $starSymbol(icount));\n";
	    dataout << "write($starSymbol(aline), $starSymbol(space));\n";
	    dataout << "write($starSymbol(aline), $ref(input#";
	    dataout << k;
	    dataout << "));\n";
	    dataout << "writeline($starSymbol(fileout";
	    dataout << k;
	    dataout << "), $starSymbol(aline));\n";
	  }
	  addCode(dataout);
	  addCode(std);
	}
	exectime {
		return 0;
	}
}
