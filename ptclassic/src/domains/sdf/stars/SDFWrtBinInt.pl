defstar
{
    name { WrtBinInt }
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
    descriptor { Write integers to a 16-bit binary file. The data type written \
is 'short'. 
               }


        ccinclude { "Scheduler.h" }

    input
    {
	name { input }
	type { int }
    }

    state
    {
	name { fileName }
	type { string }
	default { "/dev/null" }
	descriptor { Output file }
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

    start
    {

	// open input file
	fd = open(expandPathName(fileName), 0x201, 0x1A4); 
	if (fd < 0) {
		Error::abortRun(*this, "can't open file ", fileName);
		return;
	}
 	bigEndian = isBigEndian();
   }

    go
    {
	float x = 0;
	short y = 0;
	int r;
	
	x = (input%0);
        y = (short) x;
	
	if (((!bigEndian&&strcmp(endian,"b")) ||
	      ( bigEndian&&!strcmp(endian,"l")) &&
		!(strcmp(endian,"n")==0))) y=swap(y);
	
   	r = write(fd, &y, sizeof(y));
	if (r != sizeof(y))
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
