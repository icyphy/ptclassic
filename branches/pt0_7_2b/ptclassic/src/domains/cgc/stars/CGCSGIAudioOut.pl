defstar
{
    name { SGIAudioOut }
    derivedFrom { SGIAudio }
    domain { CGC }
    desc { Put samples into audio output port. }
    version { $Id$ }
    author { T. M. Parks }

    input
    {
	name { left }
	type { float }
	desc { Left channel. }
    }

    input
    {
	name { right }
	type { float }
	desc { Right channel. }
    }

    codeblock (open)
    {
	$starSymbol(port) = ALopenport("$starSymbol(port)", "w", $starSymbol(config));
    }

    codeblock (write)
    {
	{
	    short buffer[2];

	    buffer[0] = (short)($ref(left) * 32768.0);
	    buffer[1] = (short)($ref(right)* 32768.0);
	    ALwritesamps($starSymbol(port), buffer, 2);
	}
    }

    initCode
    {
	CGCSGIAudio::initCode();
	addCode(open);
    }

    go
    {
	addCode(write);
    }
}
