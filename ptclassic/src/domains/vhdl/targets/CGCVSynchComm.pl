defstar {
  name { VSynchComm }
  domain { CGC }
  desc { Base class for CGC-VHDL synchronous communication }
  version { $Id$ }
  author { Michael C. Williamson and Jose Luis Pino }
  copyright { 
Copyright (c) 1993-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location { VHDL Target Directory }
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
    default {""}
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

  // Define prototype for destDirectoryName
  hinclude { "CGTarget.h" }

  // Define constant SOCK_BASE_NAME
  ccinclude { "CLIConstants.h" }

  constructor {
    StringList destDirName = destDirectoryName("MIXED");
    destDir.setInitValue(destDirName);
  }

  setup {
  }

  codeblock (firstDecl) {
  /* Decls */
  int $starSymbol(count);
  int $starSymbol(intptr);
  int $starSymbol(nbytes);
  int $starSymbol(nearsock), $starSymbol(xmitsock), $starSymbol(status);
  char $starSymbol(buffer)[BUFFSIZE];
  char *$starSymbol(dummy);
  char *$starSymbol(nearstring);
  }

  codeblock (secondDecl) {
  struct sockaddr_un $starSymbol(nearaddr), $starSymbol(xmitaddr);
  int $starSymbol(nearnamelen);
  int $starSymbol(xmitaddrlen);
  int $starSymbol(nearaddrlen);
  int $starSymbol(i);
  }

  codeblock (genInit) {
  /* Init */
  $starSymbol(count) = 0;
  $starSymbol(intptr) = 0;
  $starSymbol(nbytes) = 12;
  }

  codeblock (body) {
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
  $starSymbol(nearaddr).sun_family = AF_UNIX;
  (void) strcpy($starSymbol(nearaddr).sun_path,
		$starSymbol(nearstring));
  (void) unlink($starSymbol(nearaddr).sun_path);
  if(bind($starSymbol(nearsock),
	  (struct sockaddr *) &$starSymbol(nearaddr),
	  (strlen($starSymbol(nearaddr).sun_path) +
	   sizeof($starSymbol(nearaddr).sun_family))) == -1) {
    perror($starSymbol(dummy));
  }
  if(listen($starSymbol(nearsock), 5) == -1) {
    perror($starSymbol(dummy));
  }
  $starSymbol(xmitsock) = accept($starSymbol(nearsock),
				 (struct sockaddr *) &$starSymbol(xmitaddr),
				 &$starSymbol(xmitaddrlen));
  if($starSymbol(xmitsock) < 0) {
    perror($starSymbol(dummy));
  }
  }

  codeblock (wrap) {
  /* Wrapup */
  (void) shutdown($starSymbol(xmitsock),2);
  (void) close($starSymbol(xmitsock));
  (void) unlink($starSymbol(xmitaddr).sun_path);
  (void) shutdown($starSymbol(nearsock),2);
  (void) close($starSymbol(nearsock));
  (void) unlink($starSymbol(nearaddr).sun_path);
  }

  initCode {
    addLinkOption("-lsocket");
    addLinkOption("-lnsl");
    addInclude("<stdio.h>");
    addInclude("<string.h>");
    addInclude("<unistd.h>");
    addInclude("<sys/types.h>");
    addInclude("<sys/socket.h>");
    addInclude("<sys/un.h>");
    addGlobal("#define BUFFSIZE 32", "buffer");
    addDeclaration(firstDecl);
    addDeclaration(secondDecl);

    // This must be the first call to add code to mainInit.
    StringList command;
    command << "cd " << (const char*) destDir;
    command << " ; ";
    command << "ptvhdlsim -nc -i " << filePre << ".com " << filePre
	    << "_parts";

    StringList startvss;
    startvss << "\n";
    startvss << "  /* Start VSS Simulator */\n";
    startvss << "  system(\"";
    startvss << command << "&";
    startvss << "\");\n";
    addCode(startvss, "mainInit", filePre);
    addCode(genInit);
    
    StringList oneLine = "  $starSymbol(dummy) = \"  ";
    oneLine << classname;
    oneLine << " socket error\";";
    addCode(oneLine);

    oneLine = "  $starSymbol(nearstring) = \"";
    oneLine << SOCK_BASE_NAME;
    oneLine << sndrcv;
    oneLine << "$val(pairNumber)\";";
    addCode(oneLine);

    addCode(body);
  }

  wrapup {
    addCode(wrap);
  }
}
