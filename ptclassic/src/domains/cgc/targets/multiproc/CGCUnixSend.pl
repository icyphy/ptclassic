defstar {
	name { UnixSend }
	domain { CGC }
	desc {
Send star between UNIX processors.
	}
	version { $Id$ }
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
	location { CGC target library }
	explanation {
Produce code for inter-process communication (send-side)
	}
	private {
		friend class CGCMultiTarget;
	}
	input {
		name {input}
		type {FLOAT}
	}
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
	ccinclude { "StringList.h" }

	setup {
		numData = input.numXfer();
	}

	codeblock (ipcHandler) {

    void $val(IPCHandlerName)() {
	int timeout, localLoop;
	struct sockaddr_in addr;

	/* connect to the server */
	for (timeout = 0; ; timeout++) {
		/* Open a TCP socket (an Internet stream socket */
		if (($starSymbol(sId) = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			printf("cannot open stream socket in $val(IPCHandlerName).\n");
			exit(1);
		}

		/* Fill in the structure addr with the address of the server
	   	   that we want to connect with */
		bzero((char*) &addr, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr("$val(hostAddr)");
		addr.sin_port = htons($val(hostPort));

		if (connect($starSymbol(sId), (struct sockaddr*) &addr, sizeof(addr) ) == 0) 
			break;
		sleep(1);
		if (timeout > 10000) {
			printf("cannot connect to server on port $val(hostPort).\n");
			exit(1);
		}
		close($starSymbol(sId));
	}
    }	
	}
	initCode {
		StringList IPCproc = processCode(ipcHandler);	
		addProcedure(IPCproc);
		addGlobal(processCode("int $starSymbol(sId);\n"));
		addInclude("<stdio.h>");
		addInclude("<sys/types.h>");
		addInclude("<sys/socket.h>");
		addInclude("<netinet/in.h>");
		addInclude("<arpa/inet.h>");
	}
		
	codeblock (block) {
	int i, pos;
	double fData;
	for (i = 0; i < $val(numData); i++) {
		pos = $val(numData) - 1 + i;
		fData = $ref(input,pos);
		if (write($starSymbol(sId), &fData, sizeof(fData)) != sizeof(fData)) {
			printf("write fails.\n");
			exit(1);
		}
	}
	}
	go {
		gencode(block);
	}
	wrapup {
		gencode("\tclose($starSymbol(sId));\n");
	}
}

