defstar {
    name { VSend }
    domain { CGC }
    desc { Send data synchronously from CGC to VHDL }
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
explanation {}
  public {
    int numXfer;
//    StringList destDir;
//    StringList filePre;
  }
  protected {
  }
input {
  name {input}
  type {ANYTYPE}
}
  defstate {
    name {pairNumber}
    type {int}
    default {11}
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
setup {
//  destDir = "/users/cameron/PTOLEMY_SYSTEMS";
//  filePre = "CGCVHDLCG56Test2";
  numXfer = input.numXfer();
//  VHDLCSynchComm::setup();
}

initCode {
  CodeStream *compileOptions, *linkOptions;
  if ((compileOptions = getStream("compileOptions")) == FALSE)
    return;
  if ((linkOptions = getStream("linkOptions")) == FALSE)
    return;
//  compileOptions->put("-I$S56DSP/include ","S56X Include");
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
  int $starSymbol(sendsock), $starSymbol(xmitsock), $starSymbol(writestat);
  char $starSymbol(buffer)[BUFFSIZE];
  char *$starSymbol(dummy) = \"  csource Error\";
  char *$starSymbol(sendstring) = \"/tmp/snd$val(pairNumber)\\0\";
  struct sockaddr $starSymbol(sendaddr), $starSymbol(xmitaddr);
  int $starSymbol(sendnamelen) = strlen($starSymbol(sendstring));
  int $starSymbol(xmitaddrlen) = sizeof($starSymbol(xmitaddr));
  int $starSymbol(sendaddrlen) = sizeof($starSymbol(sendaddr));
  int $starSymbol(i);
");

// This must be the first call to add code to mainInit
  StringList command = "";
  command << "cd " << (const char*) destDir;
  command << " ; ";
  command << "vhdlsim -i " << filePre << ".com " << "parts";

//  system(command);
  StringList startvss = "";
  startvss << "
  /* Start VSS Simulator */
  system(\"";
  startvss << command << "&";
  startvss << "\");\n";
  addCode(startvss, "mainInit", "startVSS");
  addCode("
  /* Init */
  for ($starSymbol(i)=0 ; $starSymbol(i) < BUFFSIZE ; $starSymbol(i)++) {
    $starSymbol(buffer)[$starSymbol(i)] = (char) 0;
  }
  $starSymbol(sendsock) = socket(AF_UNIX, SOCK_STREAM, 0);
  if ($starSymbol(sendsock) == -1) {
    perror($starSymbol(dummy));
  }
  $starSymbol(sendaddr).sa_family = AF_UNIX;
  (void) strncpy($starSymbol(sendaddr).sa_data, $starSymbol(sendstring), $starSymbol(sendnamelen)+1);
  (void) unlink($starSymbol(sendaddr).sa_data);
  if(bind($starSymbol(sendsock), &$starSymbol(sendaddr), $starSymbol(sendaddrlen)) == -1) {
    perror($starSymbol(dummy));
  }
  if(listen($starSymbol(sendsock), 5) == -1) {
    perror($starSymbol(dummy));
  }
  $starSymbol(xmitsock) = accept($starSymbol(sendsock), &$starSymbol(xmitaddr), &$starSymbol(xmitaddrlen));
  if($starSymbol(xmitsock) < 0) {
    perror($starSymbol(dummy));
  }
");
}

go {
  addCode("
  /* Go */
  $starSymbol(intptr) = 0;
  $starSymbol(writestat) = 0;
  if($starSymbol(writestat) >= 0) {
    (void) sprintf($starSymbol(buffer), \"%d\", $ref(input));
    $starSymbol(writestat) = write($starSymbol(xmitsock), $starSymbol(buffer), $starSymbol(nbytes));
    if($starSymbol(writestat) < 0) {
      perror($starSymbol(dummy));
    }
  }
");
}

wrapup {
  addCode("
  /* Wrapup */
  (void) shutdown($starSymbol(sendsock),2);
  (void) close($starSymbol(sendsock));
  (void) unlink($starSymbol(sendaddr).sa_data);
  (void) shutdown($starSymbol(xmitsock),2);
  (void) close($starSymbol(xmitsock));
  (void) unlink($starSymbol(xmitaddr).sa_data);
");
}

}
