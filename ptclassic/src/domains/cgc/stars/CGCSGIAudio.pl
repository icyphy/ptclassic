defstar
{
    name { SGIAudio }
    domain { CGC }
    desc { Base class for reading and writing SGI audio ports. }
    version { $Id$ }
    author { T. M. Parks }

    codeblock (declare)
    {
	ALport $starSymbol(port);
	ALconfig $starSymbol(config);
    }

    codeblock (setup)
    {
	$starSymbol(config) = ALnewconfig();
	ALsetwidth($starSymbol(config), AL_SAMPLE_16);
	ALsetchannels($starSymbol(config), AL_STEREO);
	ALsetqueuesize($starSymbol(config), 0x1000);
    }

    codeblock (close)
    {
	ALcloseport($starSymbol(port));
	ALfreeconfig($starSymbol(config));
    }

    initCode
    {
	addInclude("<audio.h>");
	addGlobal(declare);
	addCode(setup);
    }

    wrapup
    {
	addCode(close);
    }
}
