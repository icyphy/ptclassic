ident {
// $Id$
// Joseph T. Buck

// This atrocity sends files to a program that plays files over the
// SparcStation speaker.  Needless to say, you have to be on the
// SparcStation and you need to have the right program and files.
// The files are from a "speaking clock" program, which is why
// it only really works for 0-59 (though I fake it up to 100).
}

defstar {
	name { SayDigit }
	domain { SDF }
	desc {
	"Speak the input integer values on the SparcStation speaker.\n"
		"Only works for 0 <= n <= 99."
	}
	input {
		name { input }
		type { int }
	}
	ccinclude { "stream.h",  "std.h" }
	code {
// Start of code block to put in the SayDigit.cc file
#ifndef PLAYCMD
#define PLAYCMD "/usr/local/sound/bin/play"
#endif

#ifndef PLAYDIR
#define PLAYDIR "/usr/local/sound/lib/saytime"
#endif
static const char* pcmd = PLAYCMD;
static const char* pdir = PLAYDIR;

// Function to say a file.
static say (char* file) {
	system (form ("%s %s/%s", pcmd, pdir, file));
}

// Function to say two files (both numeric)
static say2 (int n1, int n2) {
	if (n2 == 0) {
		system (form ("cat %s/%d.au %s/oh.au | %s",
			      pdir, n1, pdir,pcmd));
		return;
	}
	system (form ("cat %s/%d.au %s/%d.au | %s",
		      pdir, n1, pdir, n2, pcmd));
}
// end of code block
	}

// startup function: make sure the program exists
	start {
		if (access (pcmd, 1) == 0) return;
		cerr << "Can't find " << pcmd << "!\n";
		cerr << "The SayDigit star requires this program.\n";
		exit (1);
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
