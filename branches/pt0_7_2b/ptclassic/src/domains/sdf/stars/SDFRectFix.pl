defstar {
	name { RectFix }
	domain { SDF }
	desc {
Generates a fixed-point rectangular pulse of height "height" 
(default 1.0).  and width "width" (default 8).  If "period" is 
greater than zero, then the pulse is repeated with the given period. 
The precision of "height" can be specified in bits.
	}
        author { A. Khazeni }
	version { $Id$ }
	location { SDF main library }
        explanation {
The value of the "height" parameter and its precision in bits can 
currently be specified using two different notations.
Specifying only a value by itself in the dialog box would create a
fixed-point number with the default precision which has a total length
of 24 bits with the number of range bits as required by the value.
For example, the default value 1.0 creates a fixed-point object with
precision 2.22, and a value like 0.5 would create one with precision
1.23 and etc.  The alternative way of specifying the value and the
precision of this parameter is to use the paranthesis notation which will be
interpreted as (value, precision).  For example, filling the dialog
box of this parameter by let's say (2.546, 3.5) would create a fixed-point
object by casting the double 2.546 to the precision 3.5. }
	output {
		name { output }
		type { fix }
	}
	defstate {
		name { height }
		type { fix }
		default { "1.0" }
                desc { Height of the rectangular pulse. }
	}
	defstate {
		name { width }
		type { int }
		default { 8 }
		desc { Width of the rectangular pulse. }
	}
	defstate {
		name { period }
		type { int }
		default { 0 }
		desc { If greater than zero, the period of the pulse stream. }
	}
	defstate {
		name { count }
		type { int }
		default { 0 }
		desc { Internal counting state. }
		attributes { A_NONSETTABLE|A_NONCONSTANT }
	}
	go {
                Fix t;
		if (int(count) < int(width)) t = Fix(height);
		output%0 << t;
		count = int(count) + 1;
		if (int(period) > 0 && int(count) >= int(period)) count = 0;
	}
}
