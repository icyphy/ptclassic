defstar
{
    name { WrtBin }
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
    descriptor { Write floating point data to a binary file. The data type \
written is 'float'.
    		}

    explanation{ The byte ordering of floating point data values may vary 
from machine to machine, so an option is provided to allow byte
swapping of the data. }

    ccinclude { "Scheduler.h" }

    input
    {
	name { input }
	type { float }
    }

    state
    {
	name { fileName }
	type { string }
	default { "/dev/null" }
	descriptor { Output File }
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
	fd = open(expandPathName(fileName), 0x202, 0x1A4); // Yeah, I know...
	if (fd < 0) {
		Error::abortRun(*this, "can't open file ", fileName);
		return;
	}
    }

    go
    {
	float x = 0;
	int r;
	
	x = (input%0);

 	if (!strcmp(byteswap,"YES"))
		x=swap(x);
	
   	r = write(fd, &x, sizeof(x)); // Inefficient, but it works...
	if (r != sizeof(x))
	{
	    fprintf(stderr, "Error on write!!!\n");
            Scheduler::requestHalt();

	}        

      }

    wrapup
    {
        close(fd);
    }
}











