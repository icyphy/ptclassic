defstar {
  name { VSynchComm }
  domain { CGC }
  desc { Base class for CGC-VHDL synchronous communication }
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
  defstate {
    name {pairNumber}
    type {int}
    default {0}
  }
  defstate {
    name {destDir}
    type {string}
    default {"~/PTOLEMY_SYSTEMS"}
  }
  defstate {
    name {filePre}
    type {string}
    default {"CGCVHDL"}
  }
  defstate {
    name {classname}
    type {string}
    default {"CGCVSynchComm"}
  }
  defstate {
    name {sndrcv}
    type {string}
    default {"sndrcv"}
  }
  defstate {
    name {format}
    type {string}
    default {"%"}
  }
  setup {
  }

  initCode {
    CodeStream *compileOptions, *linkOptions;
    if ((compileOptions = getStream("compileOptions")) == FALSE)
      return;
    if ((linkOptions = getStream("linkOptions")) == FALSE)
      return;
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
  int $starSymbol(count);
  int $starSymbol(intptr);
  int $starSymbol(nbytes);
  int $starSymbol(nearsock), $starSymbol(xmitsock), $starSymbol(status);
  char $starSymbol(buffer)[BUFFSIZE];
  char *$starSymbol(dummy);
  char *$starSymbol(nearstring);
");

/*
  StringList oneLine;

    oneLine = "  char *$starSymbol(dummy) = \"  ";
    oneLine << classname;
    oneLine << " socket error\";";
    addDeclaration(oneLine);

    oneLine = "  char *$starSymbol(nearstring) = \"/tmp/";
    oneLine << sndrcv;
    oneLine << "$val(pairNumber)\\0\";";
    addDeclaration(oneLine);
    */
    
    addDeclaration("  struct sockaddr $starSymbol(nearaddr), $starSymbol(xmitaddr);
  int $starSymbol(nearnamelen);
  int $starSymbol(xmitaddrlen);
  int $starSymbol(nearaddrlen);
  int $starSymbol(i);
");

// This must be the first call to add code to mainInit.
    StringList command = "";
    command << "cd " << (const char*) destDir;
    command << " ; ";
    command << "ptvhdlsim -nc -i " << filePre << ".com " << "parts";

    StringList startvss = "";
    startvss << "
  /* Start VSS Simulator */
  system(\"";
    startvss << command << "&";
    startvss << "\");\n";
    addCode(startvss, "mainInit", "startVSS");
    addCode("
  /* Init */
  $starSymbol(count) = 0;
  $starSymbol(intptr) = 0;
  $starSymbol(nbytes) = 12;
");
    
    StringList oneLine;

    oneLine = "  $starSymbol(dummy) = \"  ";
    oneLine << classname;
    oneLine << " socket error\";";
    addCode(oneLine);

    oneLine = "  $starSymbol(nearstring) = \"/tmp/";
    oneLine << sndrcv;
    oneLine << "$val(pairNumber)\\0\";";
    addCode(oneLine);

    addCode("
  $starSymbol(nearnamelen) = strlen($starSymbol(nearstring));
  $starSymbol(xmitaddrlen) = sizeof($starSymbol(xmitaddr));
  $starSymbol(nearaddrlen) = sizeof($starSymbol(nearaddr));

  for ($starSymbol(i)=0 ; $starSymbol(i) < BUFFSIZE ; $starSymbol(i)++) {
    $starSymbol(buffer)[$starSymbol(i)] = (char) 0;
  }
  $starSymbol(nearsock) = socket(AF_UNIX, SOCK_STREAM, 0);
  if ($starSymbol(nearsock) == -1) {
    perror($starSymbol(dummy));
  }
  $starSymbol(nearaddr).sa_family = AF_UNIX;
  (void) strncpy($starSymbol(nearaddr).sa_data, $starSymbol(nearstring), $starSymbol(nearnamelen)+1);
  (void) unlink($starSymbol(nearaddr).sa_data);
  if(bind($starSymbol(nearsock), &$starSymbol(nearaddr), $starSymbol(nearaddrlen)) == -1) {
    perror($starSymbol(dummy));
  }
  if(listen($starSymbol(nearsock), 5) == -1) {
    perror($starSymbol(dummy));
  }
  $starSymbol(xmitsock) = accept($starSymbol(nearsock), &$starSymbol(xmitaddr), &$starSymbol(xmitaddrlen));
  if($starSymbol(xmitsock) < 0) {
    perror($starSymbol(dummy));
  }
");
  }

  wrapup {
    addCode("
  /* Wrapup */
  (void) shutdown($starSymbol(xmitsock),2);
  (void) close($starSymbol(xmitsock));
  (void) unlink($starSymbol(xmitaddr).sa_data);
  (void) shutdown($starSymbol(nearsock),2);
  (void) close($starSymbol(nearsock));
  (void) unlink($starSymbol(nearaddr).sa_data);
");
  }

}
