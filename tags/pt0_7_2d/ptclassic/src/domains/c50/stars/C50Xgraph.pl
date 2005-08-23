defstar {
	name { Xgraph }
	domain { C50 }
	desc {
Generate a plot of a single signal with the xgraph program.
	}
	version { @(#)C50Xgraph.pl	1.5	06 Oct 1996 }
	author { A. Baensch }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
The input signal is plotted using the <i>pxgraph</i> program.
The <i>title</i> parameter specifies a title for the plot.
The <i>saveFile</i> parameter optionally specifies a file for
storing the data in a syntax acceptable to xgraph.
A null string prevents any such storage.
The <i>options</i> string is passed directly to the xgraph program
as command-line options.  See the manual section describing xgraph
for a complete explanation of the options.
<a name="xgraph program, C50"></a>
<a name="graph, X window, C50"></a>
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

(pxgraph -t "$val(title)" $val(options) @outfile ; @(saveFile.null()?"":"/bin/rm -f $starSymbol(/tmp/cgxgraph)")) &
	}

	constructor {
		fileName.setAttributes(A_NONSETTABLE);
	}

	initCode {
		StringList outfile;
		if (saveFile.null())
			outfile << "$starSymbol(/tmp/cgxgraph)";
		else {
			char *expandedPath = expandPathName(saveFile);
			outfile << expandedPath;
			delete [] expandedPath;
		}
		addCode(xgraph(outfile), "shellCmds");
		C50WrtFile::initCode();
	}
}
