defstar
{
    name { PCM }
    domain { CGC }
    desc { Base class for reading and writing mu-law encoded PCM data. }
    version { $Id$ }
    author { T. M. Parks }

    state
    {
	name { fileName }
	type { string }
	default { "/dev/audio" }
	desc { File for PCM data.  If blank, use standard IO. }
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
	FILE* $starSymbol(stream);
	unsigned char $starSymbol(byte);
    }

    codeblock (closeFile)
    {
	/* Close file. */
	if (fclose($starSymbol(stream)) != 0)
	{
	    perror("$val(fileName)");
	    exit(1);
	}
    }

    initCode
    {
	addInclude("<math.h>");
	addInclude("<stdio.h>");
	addDeclaration(declarations);
	if (addGlobal(sharedDeclarations, "$sharedSymbol(PCM,PCM)"))
	    addCode(sharedInit);
    }
}
