defstar {
	name { SayDigit }
	domain { SDF }
	version { $Id$ }
	desc {
	"Speak the input integer values on the SparcStation speaker.\n"
		"Only works for 0 <= n <= 99."
	}
	explanation {
This atrocity sends files to a program that plays files over the
SparcStation speaker.  Needless to say, you have to be on the
SparcStation and you need to have the right program and files.
The files are from a "speaking clock" program, which is why
it only really works for 0-59 (though I fake it up to 100).
	}
	input {
		name { input }
		type { int }
	}
	ccinclude { "stream.h",  "std.h" }
	code {
// Start of code block to put in the SayDigit.cc file
#ifndef PLAYDIR
#define PLAYDIR "/usr/local/sound/lib/saytime"
#endif

// Function to say a file.
static void say (const char* file) {
	char buf[256];
	sprintf (buf, "play %s/%s", PLAYDIR, file);
	system (buf);
}

// Function to say two files (both numeric)
static void say2 (int n1, int n2) {
	char buf[256];
	if (n2 == 0)
		sprintf ("cat %s/%d.au %s/oh.au | play",
			 PLAYDIR, n1, PLAYDIR);
	else
		sprintf ("cat %s/%d.au %s/%d.au | play",
			 PLAYDIR, n1, PLAYDIR, n2);
	system (buf);
}
// end of code block
	}

// go function: say the input
	go {
		int numToSay = int(input%0);
		if (numToSay == 0) say ("oh.au"); // "oh" -- not very good
		else if (numToSay < 0 || numToSay >= 100) {
			cerr << "Out of range: can't say " << numToSay << "\n";
		}
		else if (numToSay >= 60) // "six three"
			say2 (numToSay / 10, numToSay % 10);
		else if (numToSay < 21 || numToSay % 10 == 0)
			say (form ("%d.au", numToSay));
		else			// "fifty four"
			say2 ((numToSay / 10) * 10, numToSay % 10);
	}
}
