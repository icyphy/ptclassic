defstar {
    name { AudioBase }
    domain { CGC }
    desc {
Base star for reading and writing audio data.
    }
    version { $Id$ }
    author { T. M. Parks and Brian L. Evans }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }

    defstate {
	name { fileName }
	type { string }
	default { "/dev/audio" }
	desc { File for PCM data.  If blank, use standard IO. }
    }

    defstate {
	name { blockSize }
	type { int }
	default { 128 }
	desc { Number of samples to read or write. }
    }

    protected {
	int standardIO:1;
    }

    setup {
	standardIO = fileName.null();
    }

    initCode {
	if (!standardIO) {
            addInclude("<fcntl.h>");		// Define open and O_RDONLY
	    addInclude("<unistd.h>");		// Define read and write
	}
    }

    codeblock(declarations, "const char* datatype, int size") {
	int $starSymbol(file);
	@datatype $starSymbol(buf)[@size];
    }

    codeblock(noOpen) {
	/* Use standard input for reading. */
	$starSymbol(file) = 0;
    }

    codeblock(openFileForReading) {
	/* Open file for reading */
	if (($starSymbol(file) = open("$val(fileName)",O_RDONLY,0666)) == -1)
	{
		perror("$val(fileName)");
		exit(1);
	}
    }

    codeblock(openFileForWriting) {
	/* Open file for writing */
	if (($starSymbol(file) = open("$val(fileName)",O_WRONLY|O_CREAT,0666)) == -1)
	{
		perror("$val(fileName)");
		exit(1);
	}
    }

    codeblock(read) {
	/* Read blockSize bytes of data from the file */
	if (read($starSymbol(file), $starSymbol(buf), $val(blockSize)) != $val(blockSize))
	{
	    perror("$val(fileName)");
	    exit(1);
	}
    }

    codeblock(write) {
	/* Write blockSize bytes to file */
	if (write($starSymbol(file), $starSymbol(buf), $val(blockSize)) != $val(
blockSize))
	{
	    perror("$val(fileName)");
	    exit(1);
	}
    }

    codeblock(closeFile) {
	/* Close file */
	if (close($starSymbol(file)) != 0) {
	    perror("$val(fileName)");
	    exit(1);
	}
    }

    wrapup {
	if (!standardIO) addCode(closeFile);
    }
}
