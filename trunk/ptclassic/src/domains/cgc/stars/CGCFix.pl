defstar {
	name { Fix }	
	domain { CGC }
	desc {
Based star for the fixed-point stars in the CGC domain.
	}
	version { $Id$ }
        author { J.Weiss }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
        location { CGC main library }
        defstate {
                name { ReportOverflow }
                type { int }
                default { "NO" }
                desc {
If non-zero, e.g. YES or TRUE, then after a simulation has finished,
the star will report the number of overflow errors if any occurred
during the simulation.
                }
        }
	state {
		name { ov_cnt }
		type { int }
		default { 0 }
		desc { counter for overflow errors }
		attributes { A_NONSETTABLE|A_NONCONSTANT }
	}
	state {
		name { ck_cnt }
		type { int }
		default { 0 }
		desc { counter for overflow check operations }
		attributes { A_NONSETTABLE|A_NONCONSTANT }
	}

    codeblock(report_overflow, "const char* starName") {
	if ($ref(ov_cnt)) {
		double percentage = (100.0*$ref(ov_cnt)) / ($ref(ck_cnt) ? $ref(ck_cnt):1.0);
		fprintf(stderr, "star @starName: \
experienced overflow in %d out of %d fixed-point calculations checked (%.1lf%%)\n",
			$ref(ov_cnt), $ref(ck_cnt), percentage);
	}
    }
	// Invoke these methods before and after generating a code region that
	// is to be checked for fixed-point overflow conditions.
	// The generated code depends on the internal fix_overflow variable
	// that is ored with the error status of the invoked fixed-point operations.
	method {
		name { clearOverflow }
		type { void }
		access { protected }
		code {
			if ((int)ReportOverflow) {
				addCode("\tfix_overflow = 0;\n");
			}
		}
	}
	method {
		name { checkOverflow }
		type { void }
		access { protected }
		code {
			if ((int)ReportOverflow) {
				addCode("\tif ($ref(ck_cnt)++, fix_overflow)\n");
				addCode("\t\t$ref(ov_cnt)++;\n");
			}
		}
	}

	// Currently, the setup routine does nothing, but this might change
	// in future.  Then stars whose setup routines forget to call
	// CGCFix::setup explicitly are likely to fail.

	setup {
		// do nothing
	}

	// derived stars should call this method if they defined their own
	// wrapup method
	wrapup {
		if (int(ReportOverflow)) {
			StringList& s = this->fullName();
			addCode(report_overflow((const char*)s));
		}
	}
}
