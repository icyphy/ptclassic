defstar
{
    name { RdBinInt }
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
    descriptor { Read integers from a 16-bit binary file. The data type \
assumed is 'short'.}

    ccinclude { "Scheduler.h" }
    ccinclude { <iostream.h> }
    output
    {
	name { output }
	type { int }
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
	name { endian }
	type { string }
	default { "n" }
	descriptor { Endian-ness of data: b=big-endian, l=little-endian, n=native }
    }

    protected
    {
	int fd;	 
	int bigEndian;
    }

    code
    {
	union shortChar
	{
		short s;
		unsigned char c[sizeof(short)];
	};

	// This routine determines whether the machine is big-endian.
	// Returns TRUE (1) if it is.
	int isBigEndian()
	{
		shortChar a;
		a.s = 1;
		return a.c[1];
	}

	// return a byte-swapped version of arg.
	short swap(short arg)
	{
		shortChar a;
		a.s = arg;
		unsigned char t = a.c[0];
		a.c[0] = a.c[1];
		a.c[1] = t;
		return a.s;
	}
    }

    setup
    {
	// open input file
	fd = open(expandPathName(fileName), 0);
	if (fd < 0)
	 {
		Error::abortRun(*this, "can't open file ", fileName);
		return;
	}
	bigEndian = isBigEndian();
     }

    go
    {
	short x = 0;
	int r;

   	r = read(fd, &x, sizeof(x)); // inefficient, don't ask!
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

	if (((!bigEndian&&strcmp(endian,"b")) ||
	      ( bigEndian&&!strcmp(endian,"l")) &&
		!(strcmp(endian,"n")==0))) x=swap(x);
	
	output%0 <<  x;
    }

    wrapup
    {
        close(fd);
    }
}











