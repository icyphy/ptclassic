defstar
{
    name { PCM }
    domain { CGC }
    desc { Base class for reading and writing PCM data. }
    version { $Id$ }
    author { T. M. Parks }

    state
    {
	name { fileName }
	type { string }
	default { "/dev/audio" }
    }

    protected
    {
	int standardIO:1;
    }

    codeblock (globalDeclarations)
    {
	int pcmOffset[8];
    }

    codeblock (globalInit)
    {

	/* Initialize PCM offset table. */
	{
	    int i;
	    double x = 0.0;
	    double dx = 0.125;

	    for(i = 0; i < 8; i++, x += dx)
	    {
		pcmOffset[i] = 4080.0 * (pow(256.0,fabs(x)) - 1.0) / 255.0;
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
	addInclude("<mp.h>");
	addInclude("<stdio.h>");
	addGlobal(globalDeclarations);
	addGlobal(declarations);
	addCode(globalInit);
    }
}
