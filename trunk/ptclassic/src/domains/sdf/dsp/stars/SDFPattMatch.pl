defstar {
	name { PattMatch }
	domain { SDF }
	author { GSWalter, E. A. Lee }
	version { $Id$ }
        copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	location { Ptolemy SDF Signal Processing palette }
	desc {
This star accepts a template and a search window.
The template is slid over the window one sample
at a time and cross correlations are calculated
at each step.  The cross-correlations are output
on the 'values' output.  The 'index' output is the
value of the time-shift which gives the largest
cross correlation.  This index refers to a position
on the search window beginning with 0 corresponding
to the earliest arrived sample of the search window
which is part of the "best match" with the template.
for viewing the values of the cross correlations.
	}

	input { name { templ } type { float } }
	input { name { window } type { float } }
	output { name { index } type { int } }
	output { name { values } type { float } }

	defstate {
		name { tempSize }
		type { int }
		default { 32 }
		desc { number of samples in template }
	}
	defstate {
		name { winSize }
		type { int }
		default { 176 }
		desc { number of samples in search window }
	}

	protected { int N; }

	start {
		if ( winSize < tempSize )
			Error::abortRun( *this,
				"winSize must be >= tempSize" );
		N = winSize - tempSize + 1;
		templ.setSDFParams( tempSize, tempSize - 1 );
		window.setSDFParams( winSize, winSize - 1 );
		values.setSDFParams( N, N - 1 );
	}

	go {
		float Cmax = 0, Cofn, top = 0, bottom = 0;
		int n, m, nmax = 0;

		for ( n = 0; n < N; n++ ) {
			top = bottom = 0;
			for ( m = 0; m < tempSize; m++ ) {
				float wval = window%( winSize - 1 - ( n + m ));
				top += float(templ%(tempSize - 1 - m)) * wval;
				bottom += wval * wval;
			}
			Cofn = top / bottom;
			values%( N - 1 - n ) << Cofn;
			if ( Cofn > Cmax ) {
				Cmax = Cofn;
				nmax = n;
			}
		}
		index%0 << nmax;
	} // end go
} // end defstar
