defstar
{
    name { SGIAudioIn }
    derivedFrom { SGIAudio }
    domain { CGC }
    desc { Get samples from audio input port. }
    version { $Id$ }
    author { T. M. Parks }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CGC main library }

    output
    {
	name { left }
	type { float }
	desc { Left channel. }
    }

    output
    {
	name { right }
	type { float }
	desc { Right channel. }
    }

    codeblock (open)
    {
	$starSymbol(port) = ALopenport("$starSymbol(port)", "r", $starSymbol(config));
    }

    codeblock (read)
    {
	{
	    short buffer[2];

	    ALreadsamps($starSymbol(port), buffer, 2);
	    $ref(left) = buffer[0] / 32768.0;
	    $ref(right) = buffer[1] / 32768.0;
	}
    }

    initCode
    {
	CGCSGIAudio::initCode();
	addCode(open);
    }

    go
    {
	addCode(read);
    }
}
