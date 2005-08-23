defstar {
	name {DB}
	domain {MDSDF}
	desc {
Convert each entry of an input matrix to a decibels (dB) scale. 
Zero and negative values
are assigned the value "min" (default -100). The "inputIsPower"
parameter should be set to �YES� if the input signal is a power
measurement (vs. an amplitude measurement).
	}
	version { @(#)MDSDFDB.pl	1.7 10/07/96 }
	author { J. T. Buck, modified for MDSDF by Mike J. Chen }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { MDSDF library }
	htmldoc {
For inputs that are greater than zero, the output is either

<i>N</i>log<sub>10</sub> (<i>input</i>) or <i>min</i>, whichever is
larger, where

<i>N</i>=10 if <i>inputIsPower</i> is TRUE, and <i>N</i>=20 otherwise.
<a name="decibel"></a>
The default is <i>N</i>=20.
For inputs that are zero or negative, the output is <i>min</i>.
	}
	input {
		name{input}
		type{FLOAT_MATRIX}
	}
	output {
		name{output}
		type{FLOAT_MATRIX}
	}
        defstate {
                name { numRows }
	        type { int }
	        default { 2 }
	        desc { The number of rows in the matrix being converted. }
	}
	defstate {
	        name { numCols }
		type { int }
		default { 2 }
                desc { The number of columns in the matrix being converted. }
        }
	defstate {
		name{min}
		type{float}
		default{"-100"}
		desc{Minimum output value.}
	}
	defstate {
		name{inputIsPower}
		type{int}
		default{"FALSE"}
		desc{
TRUE if input is a power measurement, FALSE if it's an amplitude measurement.
		}
	}
	private {
		double gain;
	}
	setup {
                input.setMDSDFParams(int(numRows), int(numCols));
                output.setMDSDFParams(int(numRows), int(numCols));
		if (int(inputIsPower)) gain=10.0;
		else gain = 20.0;
	}
	go {
                FloatSubMatrix* data = (FloatSubMatrix*)(input.getInput());
                FloatSubMatrix* result = (FloatSubMatrix*)(output.getOutput());

                for(int i = 0; i < int(numRows)*int(numCols); i++) {
                  if(data->entry(i) <= 0.0) 
                    result->entry(i) = double(min);
                  else {
                    result->entry(i) = gain * log10(data->entry(i));
                    if(result->entry(i) < double(min))
                      result->entry(i) = double(min);
                  }
                }

                delete data;
                delete result;
        }
}
