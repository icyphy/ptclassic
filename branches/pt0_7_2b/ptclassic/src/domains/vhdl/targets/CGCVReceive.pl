defstar {
    name { VReceive }
    domain { CGC }
    desc { Receive data synchronously from VHDL to CGC }
//    derivedFrom { VSynchComm }
    version { $Id$ }
    author { Michael C. Williamson, Jose Luis Pino }
    copyright { 
Copyright (c) 1994,1993 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }

    location { VHDL Target Directory }
    explanation { }
  public {
    int numXfer;
  }
  protected {
  }
    output {
	name {output}
	type {ANYTYPE}
      }
  defstate {
    name {pairNumber}
    type {int}
    default {21}
  }
setup {
  numXfer = output.numXfer();
//  CGCVSynchComm::setup();
}

initCode {
        CodeStream *compileOptions, *linkOptions;
        if ((compileOptions = getStream("compileOptions")) == FALSE)
            return;
        if ((linkOptions = getStream("linkOptions")) == FALSE)
            return;
//        compileOptions->put("-I$S56DSP/include ","S56X Include");
        linkOptions->put("-lsocket -lnsl","socket Link");
  addInclude("<stdio.h>");
  addInclude("<string.h>");
  addInclude("<unistd.h>");
  addInclude("<sys/types.h>");
  addInclude("<sys/socket.h>");
  addInclude("<sys/un.h>");
  addGlobal("#define BUFFSIZE 32", "buffer");
  addDeclaration("
  /* Decls */
  int $starSymbol(count) = 0;
  int $starSymbol(intptr) = 0;
  int $starSymbol(nbytes) = 12;
  int $starSymbol(recvsock), $starSymbol(xmitsock), $starSymbol(readstat);
  char $starSymbol(buffer)[BUFFSIZE];
  char *$starSymbol(dummy) = \"  csink Error\";
  char *$starSymbol(recvstring) = \"/tmp/rcv$val(pairNumber)\\0\";
  struct sockaddr $starSymbol(recvaddr), $starSymbol(xmitaddr);
  int $starSymbol(recvnamelen) = strlen($starSymbol(recvstring));
  int $starSymbol(xmitaddrlen) = sizeof($starSymbol(xmitaddr));
  int $starSymbol(recvaddrlen) = sizeof($starSymbol(recvaddr));
  int $starSymbol(i);
");
  addCode("
  /* Init */
  for ($starSymbol(i)=0 ; $starSymbol(i) < BUFFSIZE ; $starSymbol(i)++) {
    $starSymbol(buffer)[$starSymbol(i)] = (char) 0;
  }
  $starSymbol(recvsock) = socket(AF_UNIX, SOCK_STREAM, 0);
  if ($starSymbol(recvsock) == -1) {
    perror($starSymbol(dummy));
  }
  $starSymbol(recvaddr).sa_family = AF_UNIX;
  (void) strncpy($starSymbol(recvaddr).sa_data, $starSymbol(recvstring), $starSymbol(recvnamelen)+1);
  (void) unlink($starSymbol(recvaddr).sa_data);
  if(bind($starSymbol(recvsock), &$starSymbol(recvaddr), $starSymbol(recvaddrlen)) == -1) {
    perror($starSymbol(dummy));
  }
  if(listen($starSymbol(recvsock), 5) == -1) {
    perror($starSymbol(dummy));
  }
  $starSymbol(xmitsock) = accept($starSymbol(recvsock), &$starSymbol(xmitaddr), &$starSymbol(xmitaddrlen));
  if($starSymbol(xmitsock) < 0) {
    perror($starSymbol(dummy));
  }
");
}

go {
  addCode("
  /* Go */
  $starSymbol(intptr) = 0;
  $starSymbol(readstat) = 0;
  if($starSymbol(readstat) >= 0) {
    $starSymbol(readstat) = read($starSymbol(xmitsock), $starSymbol(buffer), $starSymbol(nbytes));
    while($starSymbol(readstat) == 0) {
      $starSymbol(readstat) = read($starSymbol(xmitsock), $starSymbol(buffer), $starSymbol(nbytes));
    }
    if($starSymbol(readstat) < 0) {
      perror($starSymbol(dummy));
    }
    else {
      (void) sscanf($starSymbol(buffer), \"%d\", &$ref(output));
    }
  }
");
}

wrapup {
  addCode("
  /* Wrapup */
  (void) shutdown($starSymbol(recvsock),2);
  (void) close($starSymbol(recvsock));
  (void) unlink($starSymbol(recvaddr).sa_data);
");
}

}
