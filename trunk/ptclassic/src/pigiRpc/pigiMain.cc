/*******************************************************************
SCCS version identification
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. T. Buck

 Main program for pigiRpc.

********************************************************************/

extern "C" {
	int KcInitLog(const char*);
	void KcCatchSignals();
	void CompileInit();
	void KcLoadInit(const char*);
	void RPCMain (int argc, char** argv);
};

main (int argc, char ** argv)
{
	KcLoadInit(argv[0]);
	KcInitLog("pigiLog.pt");
	CompileInit();
	KcCatchSignals();
	RPCMain (argc, argv);
	return 0;
}

extern const char *pigiVersion = "Pigi Version: 0.2.1";
