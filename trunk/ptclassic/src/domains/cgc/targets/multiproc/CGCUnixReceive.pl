defstar {
	name { UnixReceive }
	domain { CGC }
	desc {
Receive star between Unix processors.
	}
	version { $Id$ }
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
	location { CGC local library }
	explanation {
Produce code for inter-process communication (receive-side)
	}
	private {
		friend class CGCMultiTarget;
	}
	output {
		name {output}
		type {FLOAT}
	}
	ccinclude { "StringList.h" }

	state {
		name { numData }
		type { int }
		default { 1 }
		desc { number of tokens to be transferred }
		attributes { A_NONSETTABLE }
	}
	state {
		name { IPCHandlerName }
		type { STRING }
		default { "IPCHandler" }
		desc { Name od sender's IPC handler function. }
		attributes { A_NONSETTABLE }
	}
	state {
		name { hostPort }
		type { int }
		default { 0 }
		desc { Host port number }
		attributes { A_NONSETTABLE }
	}
	state {
		name { hostAddr }
		type { STRING }
		default { " " }
		desc { Host address for server }
		attributes { A_NONSETTABLE }
	}

	setup {
		numData = output.numXfer();
	}

	codeblock (ipcHandler) {

    void $val(IPCHandlerName)() {
	int len;
	struct sockaddr_in addr, far_addr;

	/* Open a TCP socket (an Internet stream socket */
	if (($starSymbol(sId) = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("cannot open stream socket in $val(IPCHandlerName).\n");
		exit(1);
	}

	/* Bind local address */
	bzero((char*) &addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons($val(hostPort));

	if (bind($starSymbol(sId),(struct sockaddr*) &addr,sizeof(addr)) < 0) {
		printf("bind on port $val(hostPort) failed.\n");
		exit(1);
	}

	listen($starSymbol(sId), 5);

	/* wait for connection from a sender */
	len = sizeof(far_addr);
	$starSymbol(newSId) = accept($starSymbol(sId), 
		(struct sockaddr*) &far_addr, &len);
	if ($starSymbol(newSId) < 0) {
		printf("accept error on port $val(hostPort).\n");
		exit(1);
	}
	printf("accept connection on port $val(hostPort).\n");
    }	
	}
	initCode {
		StringList IPCproc = processCode(ipcHandler);	
		addProcedure(IPCproc);
		addGlobal(processCode("int $starSymbol(sId);\n"));
		addGlobal(processCode("int $starSymbol(newSId);\n"));
		addInclude("<stdio.h>");
		addInclude("<sys/types.h>");
		addInclude("<sys/socket.h>");
		addInclude("<netinet/in.h>");
		addInclude("<arpa/inet.h>");
	}
		
	codeblock (block) {
	int i, pos;
	union {
		unsigned long l;
		float f;
	} myData;

	for (i = 0; i < $val(numData); i++) {
		pos = $val(numData) - 1 + i;
		if (read($starSymbol(newSId), &myData.l, sizeof(myData)) != sizeof(myData)) {
			printf("write fails.\n");
			break;
		}
		myData.l = ntohl(myData.l);
		$ref(output,pos) = myData.f;
	}
	}
	go {
		addCode(block);
	}
	wrapup {
		addCode("\tclose($starSymbol(sId));\n");
		addCode("\tclose($starSymbol(newSId));\n");
	}
}

