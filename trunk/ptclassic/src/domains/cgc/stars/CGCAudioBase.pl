defstar
{
    name { PCM }
    domain { CGC }
    desc { Base class for reading and writing mu-law encoded PCM data. }
    version { $Id$ }
    author { T. M. Parks }
    copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }

    state
    {
	name { fileName }
	type { string }
	default { "/dev/audio" }
	desc { File for PCM data.  If blank, use standard IO. }
    }

    state
    {
	name { blockSize }
	type { int }
	default { 128 }
	desc { Number of samples to read or write. }
    }

    protected
    {
	int standardIO:1;
    }

    setup
    {
	standardIO = strcmp(fileName,"") == 0;
    }

    codeblock (sharedDeclarations)
    {
	int $sharedSymbol(PCM,offset)[8];

	/* Convert from linear to mu-law */
	int $sharedSymbol(PCM,mulaw)(x)
	double x;
	{
	    double m;
	    m = (pow(256.0,fabs(x)) - 1.0) / 255.0;
	    return 4080.0 * m;
	}
    }

    codeblock (sharedInit)
    {
	/* Initialize PCM offset table. */
	{
	    int i;
	    double x = 0.0;
	    double dx = 0.125;

	    for(i = 0; i < 8; i++, x += dx)
	    {
		$sharedSymbol(PCM,offset)[i] = $sharedSymbol(PCM,mulaw)(x);
	    }
	}
    }

    codeblock (declarations)
    {
	int $starSymbol(file);
	unsigned char $starSymbol(buf)[$val(blockSize)];
    }

    codeblock (closeFile)
    {
	/* Close file. */
	if (close($starSymbol(file)) != 0)
	{
	    perror("$val(fileName)");
	    exit(1);
	}
    }

    initCode
    {
	addInclude("<math.h>");
	addDeclaration(declarations);
	if (addGlobal(sharedDeclarations, "$sharedSymbol(PCM,PCM)"))
	    addCode(sharedInit);
    }
}
