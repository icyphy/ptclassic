defstar {
	name { Xgraph }
	domain { CG56 }
	desc {
Generate a plot of a single signal with the xgraph program.
	}
	version {$Id$}
	author { Jose Luis Pino }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 Motorola Sim I/O library }
	htmldoc {
The input signal is plotted using the <i>pxgraph</i></b> program.
The <i>title</i></b> parameter specifies a title for the plot.
The <i>saveFile</i></b> parameter optionally specifies a file for
storing the data in a syntax acceptable to xgraph.
A null string prevents any such storage.
The <i>options</i></b> string is passed directly to the xgraph program
as command-line options.  See the manual section describing xgraph
for a complete explanation of the options.
<a name="xgraph program, CG56"></a>
<a name="graph, X window, CG56"></a>
	}
	derivedFrom { WrtFile }
	defstate {
		name {title}
		type {string}
		default {"X graph"}
		desc { Title for the plot. }
	}
	defstate {
		name {saveFile}
		type {string}
		default {""}
		desc {File to save the input to the xgraph program.}
	}
	defstate {
		name {options}
		type {string}
		default {""}
		desc { Command line options for xgraph.}
	}
	defstate {
		name {ignore}
		type {int}
		default { 0 }
		desc { Number of initial values to ignore.}
	}
	defstate {
		name {xUnits}
		type {float}
		default { 1.0 }
		desc { For labeling, horizontal increment between samples. }
	}
	defstate {
		name {xInit}
		type {float}
		default { 0.0 }
		desc { For labeling, horizontal value of the first sample. }
	}

	codeblock(xgraph,"const char* outfile") {
/bin/rm -f @outfile

tail +$val(ignore) $starSymbol(/tmp/cgwritefile).io | awk '{n+=$val(xUnits);print n+$val(xInit)-$val(xUnits), $$1}' > @outfile

/bin/rm -f $starSymbol(/tmp/cgwritefile).io

(pxgraph -t "$val(title)" $val(options) @outfile ; @(saveFile.null()?"":"/bin/rm -f  $starSymbol(/tmp/cgxgraph)")) &
	}

constructor {
	fileName.setAttributes(A_NONSETTABLE);
}

initCode {
	StringList outfile;
	if ( saveFile.null() ) {
		outfile << "$starSymbol(/tmp/cgxgraph)";
	}
	else {
		char *expandedName = expandPathName(saveFile);
		outfile << expandedName;
		delete [] expandedName;
	}
	addCode(xgraph(outfile), "shellCmds");
	CG56WrtFile::initCode();
}


}
