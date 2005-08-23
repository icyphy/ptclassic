#ifndef _SDFWF4_h
#define _SDFWF4_h 1
// header file generated from SDFWF4.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
Copyright (c) 1998 Sanders, a Lockheed Martian Company
	All rights reserved. 
        See the file $PTOLEMY/copyright for copyright notice,
        limitation of liability, and disclaimer of warranty provisions.
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <iomanip.h>
#include <sys/types.h>
#include "wildforce.h"
#include "wf4errs.h"
#include "wf4api.h"
#include "WFIntArray.h"
#include <time.h>
#include "SDFStar.h"
#include "IntState.h"
#include "FloatState.h"
#include "StringState.h"
# line 41 "SDFWF4.pl"
static const char STARNAME[]="SDFWF4";

	// Board control/interface controller parameters
	static const int WF_TIMEOUT=3000;
	static const float WF_CLOCK_MAX=50.0;
	static const float WF_CLOCK_MIN=2.5;
	int PE_Start_Word;

	// ACS Target Standard Filenames
	static const char XBAR_FILENAME[]="XbarFileName";
	static const char CPE0_FILENAME[]="Cpe0FileName";
	static const char PE1_FILENAME[]="Pe1FileName";
	static const char PE2_FILENAME[]="Pe2FileName";
	static const char PE3_FILENAME[]="Pe3FileName";
	static const char PE4_FILENAME[]="Pe4FileName";
	static const char SOURCES_FILENAME[]="Sources";
	static const char SINKS_FILENAME[]="Sinks";
	static const char INPUT_WC_FILENAME[]="Inputs";
	static const char OUTPUT_WC_FILENAME[]="Outputs";
	static const char FIFO_FILENAME[]="FifoVal";

	// WordCount Structures
	WFIntArray* input_wordcounts;
	WFIntArray* output_wordcounts;

	// Defaults
	static const char DEFAULT_PE[]="defaults/blank.bin";
	static const char DEFAULT_XBAR[]="defaults/xbar_cfg.dat";

class SDFWF4 : public SDFStar
{
public:
	SDFWF4();
	/* virtual */ Block* makeNew() const;
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
	MultiInSDFPort input;
	MultiOutSDFPort output;
	int BitTst (int o, int b);
	int ACSRetrieve_XbarFilename (const char* handler_file, char* xbar_file);
	int ACSRetrieve_BitstreamFilename (const char* handler_file, char* bitstream_file);
	int ACSRetrieve_Filename (const char* handler_file, char* needed_file);
	int ACSSet_Filename (const char* handler_file, char* needed_file);
	int ACSRetrieve_WordCounts (const char* handler_file, WFIntArray* word_counts);
	int ACSRetrieve_FifoVal (const char* handler_file);
	void ACS_FAILURE (const char* function, WF4_RetCode rc);
	WF4_RetCode Ptolemy_to_WF ();
	WF4_RetCode WF_to_Ptolemy ();
	WF4_RetCode ACS_Peek (DWORD addr, DWORD length, int pe);
	WF4_RetCode ACS_MainFunction ();
	WF4_RetCode ACS_Send_Tag ();
	WF4_RetCode ACS_LoadPes (BOOLEAN enableInterrupts );

protected:
	/* virtual */ void setup();
	/* virtual */ void go();
	StringState Design_Dir;
	IntState WF_BOARD;
	FloatState WF_CLOCK;

};
#endif
