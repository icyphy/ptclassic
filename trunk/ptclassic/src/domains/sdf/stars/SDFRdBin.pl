defstar
{
    name { RdBin }
    domain { SDF }
    version { $Id$ }
    author { Alan W. Peevers }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}



    location { SDF main library }

    descriptor { Read floating point data from a binary file. The data type \
read is 'float' }

    explanation{ The byte ordering of floating point data values may vary 
from machine to machine, so an option is provided to allow byte
swapping of the data. }

    ccinclude { "Scheduler.h" }

    output
    {
	name { output }
	type { float }
    }

    state
    {
	name { fileName }
	type { string }
	default { "/dev/null" }
	descriptor { Input file }
    }

    state
    {
	name { haltAtEnd }
	type { int }
	default { "NO" }
	descriptor { Halt the run at the end of the input file }
    }

    state
    {
	name { periodic }
	type { int }
	default { "NO" }
	descriptor { Make output periodic or zero-padded }
    }

    state
    {
	name { byteswap }
	type { string }
	default { "NO" }
	descriptor { If YES, do a byte swap before outputting }
    }

    protected
    {
	int fd;	 
    }

    code
    {
	union floatChar
	{
		float f;
		unsigned char c[sizeof(float)];
	};

	// return a byte-swapped version of arg.
	float swap(float arg)
	{
		floatChar a;
		a.f = arg;
		unsigned char t = a.c[0];
		a.c[0] = a.c[3];
		a.c[3] = t;
		t = a.c[1];
		a.c[1] = a.c[2];
		a.c[2] = t;
		return a.f;
	}
    }

   start
    {

	// open input file
	fd = open(expandPathName(fileName), 0);
	if (fd < 0) {
		Error::abortRun(*this, "can't open file ", fileName);
		return;
	}
    }

    go
    {
	float x = 0.0;
        int r;

        r = read(fd, &x, sizeof(x)); // inefficient, but don't ask!
        if (r != sizeof(x))
	{
	    if (haltAtEnd)		// halt the run
		Scheduler::requestHalt();
	    else if (periodic)		// close and re-open file
	    {
		fd = open(expandPathName(fileName), 0);
		if (fd < 0) 
                {
			Error::abortRun(*this, "can't re-open file ", fileName);
			return;
		}

	    }

	}

 	if (!strcmp(byteswap,"YES"))
		x=swap(x);
	
	output%0 << x;
    }

    wrapup
    {
        close(fd);
    }
}
