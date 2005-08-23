static const char file_id[] = "SDFWF4.pl";
// .cc file generated from SDFWF4.pl by ptlang
/*
Copyright (c) 1998 Sanders, a Lockheed Martian Company
	All rights reserved. 
        See the file $PTOLEMY/copyright for copyright notice,
        limitation of liability, and disclaimer of warranty provisions.
 */

#ifdef __GNUG__
#pragma implementation
#endif


# line 1 "SDFWF4.pl"
#include "SDFWF4.h"

const char *star_nm_SDFWF4 = "SDFWF4";

const char* SDFWF4 :: className() const {return star_nm_SDFWF4;}

ISA_FUNC(SDFWF4,SDFStar);

Block* SDFWF4 :: makeNew() const { LOG_NEW; return new SDFWF4;}

SDFWF4::SDFWF4 ()
{
	setDescriptor("Ptolemy Driver for Annapolis Microsystems WILDFORCE(TM) FPGA Board");
	addPort(input.setPort("input",this,FLOAT));
	addPort(output.setPort("output",this,FLOAT));
	addState(Design_Dir.setState("Design_Dir",this,"./","\"Name of the directory (after the Ptolemy path) where the design is stored\""));
	addState(WF_BOARD.setState("WF_BOARD",this,"0","\"Identity number for the Wildforce board to use\""));
	addState(WF_CLOCK.setState("WF_CLOCK",this,"2.5","\"Speed at which to run the algorithm on the Wildforce board\""));


}

void SDFWF4::setup() {
# line 272 "SDFWF4.pl"
MPHIter nextp(output), nexti(input);
	PortHole* p, temp;
	WF4_RetCode rc;
	char* filename=new char[120];

	// Obtain kickoff value for the fifo
	PE_Start_Word=ACSRetrieve_FifoVal(FIFO_FILENAME);


	// Obtain word counts and assign to port holes
	input_wordcounts=new WFIntArray;
	output_wordcounts=new WFIntArray;

	ACSRetrieve_WordCounts(INPUT_WC_FILENAME,input_wordcounts);
	ACSRetrieve_WordCounts(OUTPUT_WC_FILENAME,output_wordcounts);

	int icount=0;
	while ((p=nexti++) != NULL)  // increment through all inputs and assign word lengths
	{
	    int word_count=input_wordcounts->query(icount++);
	    ((DFPortHole*)p)->setSDFParams (word_count, word_count-1);
	}

	int ocount=0;
	while ((p=nextp++) != NULL)  // increment through all output and assign lengths 
	{
	    int word_count=output_wordcounts->query(ocount++);
	    ((DFPortHole*)p)->setSDFParams (word_count, word_count-1);
	}

	rc= WF4_SUCCESS;


	/* ******************************          */

	cerr << "\n\n" << STARNAME << "/Setup: Attempting to open Wildforce board...";
	// open the board
	if ((rc = WF4_Open(WF_BOARD, 0)) != WF4_SUCCESS)                           
	    ACS_FAILURE("Setup",rc);
	else
	{
	    cerr << " Board found.\n";
    
	    cerr << STARNAME << "/Setup: Setting clock...";
	    float clock_req=WF_CLOCK;
	    if (clock_req < WF_CLOCK_MIN)
	    {
		cerr << "\nRequested clock speed " << WF_CLOCK 
		     << " does not meet minimum requirements of " << WF_CLOCK_MIN
		     << " adjusting...\n";
		clock_req=WF_CLOCK_MIN;
	    }
	    if (clock_req > WF_CLOCK_MAX)
	    {
		cerr << "\nRequested clock speed " << WF_CLOCK 
		     << " exceeds maximum limit of " << WF_CLOCK_MAX
		     << " adjusting...\n";
		clock_req=WF_CLOCK_MAX;
	    }
	    if ( (rc = WF4_ClkInitLocal(WF_BOARD, TRUE, clock_req )) != WF4_SUCCESS )
		ACS_FAILURE("Setup",rc);
	    else
		cerr << " PCLK set to free run and running at " << clock_req << "MHz.\n";
    
	    cerr << STARNAME << "/Setup: Suspending clock...";
	    // suspend the clock
	    if ((rc= WF4_ClkSuspend( WF_BOARD, TRUE )) != WF4_SUCCESS )
		ACS_FAILURE("Setup",rc);          
	    else
		cerr << " PCLK suspended.\n";
    
    
	    if ( (rc= ACS_LoadPes(TRUE)) != WF4_SUCCESS )  // load the pes
		ACS_FAILURE("Setup",rc);
    
    
	    cerr << STARNAME << "/Setup: Block acccess to memory...";
	    // block memory
	    if ((rc = WF4_MemBlockPe(WF_BOARD, WF4_PE1, TRUE)) != WF4_SUCCESS)
		ACS_FAILURE("Setup",rc);
	    else
		cerr << " Success\n";
    
    
	    cerr << STARNAME << "/Setup: Clear memory on all PEs...";
	    // **** clear memory   (board,pemask(WF4_PEn),pattern)
	    if ((rc=WF4_MemFill(WF_BOARD, WF4_ALL_PES, 0)) != WF4_SUCCESS)
		ACS_FAILURE("Setup",rc);
	    else
		cerr << " Success\n";
	    
    
	    cerr << STARNAME << "/Setup: Unblocking PE access to memory...";
	    if ((rc = WF4_MemBlockPe(WF_BOARD, WF4_PE1, FALSE)) != WF4_SUCCESS)
		ACS_FAILURE("Setup",rc);
	    else
		cerr << " Success\n";
    
	    /////////////////////////
	    // Configure the crossbar
	    /////////////////////////
	    ACSRetrieve_XbarFilename(XBAR_FILENAME,filename);
	    cerr << STARNAME << "/Setup: Config'ing xbar with \"" << filename << "\"...";
	    if ( (rc= WF4_XbarSetConfig(WF_BOARD, filename )) != WF4_SUCCESS)
		ACS_FAILURE("Setup",rc);
	    else
		cerr << " Success\n";
    
	    cerr << STARNAME << "/Setup: Reset fifo 0...";
	    if ((rc = WF4_FifoReset(WF_BOARD, WF4_Fifo_Pe0)) != WF4_SUCCESS)
		ACS_FAILURE("Setup",rc);
	    else
		cerr << " Success.\n";
    
	    cerr << STARNAME << "/Setup: Attempting to close Wildforce board...";
	    if ( (rc= WF4_Close(WF_BOARD )) != WF4_SUCCESS )
		ACS_FAILURE("Setup",rc);
	    else
		cerr << " Board closed.\n";
	}  // board found

    // Cleanup
    delete []filename;
}

void SDFWF4::go() {
# line 885 "SDFWF4.pl"
WF4_RetCode rc;
    rc = WF4_SUCCESS;
    
    cerr << "\n\n" << STARNAME << "/Go: Attempting to open Wildforce board...";
    if ((rc = WF4_Open(WF_BOARD, 0)) == WF4_SUCCESS)                            
    {
        cerr << " Board found.\n";    
        
        cerr << STARNAME << "/Go: Suspending clock...";
	// suspend clock
	if ((rc= WF4_ClkSuspend(WF_BOARD, TRUE )) != WF4_SUCCESS )             
            ACS_FAILURE("Go",rc);          
	else
	    cerr << " PCLK suspended.\n";
        
        cerr << STARNAME << "/Go: Assert reset lines and disable interupts.\n";
        if ((rc= WF4_PeReset(WF_BOARD, WF4_ALL_PES ,TRUE )) != WF4_SUCCESS )   
        {
            cerr << "ERROR: Failed to assert PE reset(s).\n" ;
            ACS_FAILURE("Go",rc);
        }

        cerr << STARNAME << "/Go: Disabling PE interrupts.\n";
	if ((rc= WF4_PeDisableInterrupts(WF_BOARD, WF4_ALL_PES )) != WF4_SUCCESS )
        {           
            cerr << "ERROR: Failed to disable PE interrupt(s).\n";
            ACS_FAILURE("Go",rc);
        }
        
        cerr << STARNAME << "/Go: Deassert the reset line(s)...\n";
        if ((rc= WF4_PeReset(WF_BOARD, WF4_ALL_PES,FALSE )) != WF4_SUCCESS )
        {
            cerr << "failure";
            ACS_FAILURE("Go",rc);
        }  

        cerr << STARNAME 
	     << "/Go: Enable the interrupts on the selected PE(s) if specified...\n";
        if ((rc=WF4_PeEnableInterrupts(WF_BOARD, WF4_ALL_PES)) != WF4_SUCCESS )
        {
            cerr << "failure";
            ACS_FAILURE("Go",rc);
        }

	if ((rc=ACS_MainFunction()) != WF4_SUCCESS )                            
            ACS_FAILURE("Go",rc);
        
        
        cerr << STARNAME << "/Go: Attempting to close Wildforce board...";
        if ( (rc= WF4_Close( WF_BOARD )) != WF4_SUCCESS )                      
            ACS_FAILURE("Go",rc);
	else
	    cerr << " Board closed.\n";
    }
    else
        ACS_FAILURE("Go",rc);
}


int SDFWF4::BitTst (int o, int b)
{
# line 98 "SDFWF4.pl"
return ((o & (1 << b)) != 0);
}


int SDFWF4::ACSRetrieve_XbarFilename (const char* handler_file, char* xbar_file)
{
# line 108 "SDFWF4.pl"
int success=ACSRetrieve_Filename(handler_file,xbar_file);
	    if (!success)
	    {
		success=0;
		sprintf(xbar_file,"./%s",DEFAULT_XBAR);
	    }

	    // Return happy condition
	    return(success);
}


int SDFWF4::ACSRetrieve_BitstreamFilename (const char* handler_file, char* bitstream_file)
{
# line 126 "SDFWF4.pl"
int success=ACSRetrieve_Filename(handler_file,bitstream_file);
	    if (!success)
	    {
		success=0;
		sprintf(bitstream_file,"./%s",DEFAULT_PE);
	    }

	    // Return happy condition
	    return(success);
}


int SDFWF4::ACSRetrieve_Filename (const char* handler_file, char* needed_file)
{
# line 146 "SDFWF4.pl"
int success=1;
	    char* design_dir=new char[80];
	    strcpy(design_dir,Design_Dir.currentValue());

	    char* upload_file=new char[80];
	    sprintf(upload_file,"%s/%s",design_dir,handler_file);
 
 	    FILE* fp=fopen(upload_file,"r");
	    if (fp != NULL)
	    {
		char* loaded_name=new char[80];
		fscanf(fp,"%s",loaded_name);
		sprintf(needed_file,"%s/%s",design_dir,loaded_name);
		
		// Cleanup
		delete []loaded_name;
		fclose(fp);
	    }
	    else
	    {
		printf("Unable to load from file %s, will use default\n",upload_file);
		success=0;
	    }
	    
	    // Cleanup
	    delete []upload_file;
	    delete []design_dir;
	    
	    // Return w/ status
	    return(success);
}


int SDFWF4::ACSSet_Filename (const char* handler_file, char* needed_file)
{
# line 186 "SDFWF4.pl"
int success=1;
	    char* design_dir=new char[80];
	    strcpy(design_dir,Design_Dir.currentValue());

	    sprintf(needed_file,"%s/%s",design_dir,handler_file);

	    // Cleanup
	    delete []design_dir;
		
	    // Return w/ status
	    return(success);
}


int SDFWF4::ACSRetrieve_WordCounts (const char* handler_file, WFIntArray* word_counts)
{
# line 207 "SDFWF4.pl"
int success=1;
	    char* design_dir=new char[80];
	    char* needed_file=new char[80];
	    strcpy(design_dir,Design_Dir.currentValue());

	    sprintf(needed_file,"%s/%s",design_dir,handler_file);
	    FILE* fp=fopen(needed_file,"r");
	    if (fp != NULL)
	    {
		int word_count=-1;
		while (fscanf(fp,"%d",&word_count)!=-1)
		    word_counts->add(word_count);

		// Cleanup
		fclose(fp);
	    }
	    else
		success=0;
	    
	    // Cleanup
	    delete []design_dir;
	    delete []needed_file;
		
	    // Return w/ status
	    return(success);
}


int SDFWF4::ACSRetrieve_FifoVal (const char* handler_file)
{
# line 242 "SDFWF4.pl"
int fifo_val=0;
	    char* design_dir=new char[80];
	    char* needed_file=new char[80];
	    strcpy(design_dir,Design_Dir.currentValue());

	    sprintf(needed_file,"%s/%s",design_dir,handler_file);
	    FILE* fp=fopen(needed_file,"r");
	    if (fp != NULL)
	    {
		fscanf(fp,"%d",&fifo_val);

		// Cleanup
		fclose(fp);
	    }
	    else
		fifo_val=0;
		
	    // Cleanup
	    delete []design_dir;
	    delete []needed_file;
		
	    // Return w/ status
	    return(fifo_val);
}


void SDFWF4::ACS_FAILURE (const char* function, WF4_RetCode rc)
{
# line 404 "SDFWF4.pl"
cerr << "\n***Wildforce.pl/FAILURE: Board API failure in function "
		 << function
		 << " (code = " << rc << ") " << WF4_ErrorString(rc) << "\n\n";
}


WF4_RetCode SDFWF4::Ptolemy_to_WF ()
{
# line 417 "SDFWF4.pl"
WF4_RetCode rc=WF4_SUCCESS;
        MPHIter nextii(input);
        PortHole *p;
        double* incoming;
        DWORD *data, pe_num;
        ULONG address;
        int z,power=0, bitlength=0, positive;
        double temp;
        FILE *in;
        char* filename=new char[120];

	ACSSet_Filename(SOURCES_FILENAME,filename);

	// open this file to find the sources info
        if ((in = fopen (filename, "r")) == (FILE *) NULL)
        {
            cerr << "\n***Unable to open " << filename << "\n";
            ACS_FAILURE("Ptolemy_to_WF",23);
	    return(23);
        }
        else
        {
            DWORD x=0;
	    // interate thru all ptolemy input ports
	    while ((p=nextii++)!=NULL)
            {
		cerr << "Loading config...\n";
		// read the source addr and fixed-bit info
		fscanf(in, "%d %d %d %d", &pe_num, &address, &power, &bitlength);
		DWORD length=input_wordcounts->query(x++);

		cerr << "Creating data arrays...\n";
                incoming = new double[length];
                data = new DWORD[length];
                
		cerr << "Downloading data from Ptolemy...\n";
                for (int y = length-1; y >= 0; y--)
                {
		    // read input from ptolemy into an incoming array
		    incoming[length-y-1] = (*p%y);
                }            
                
		cerr << "Converting to fixed-point...\n";

                for (int y = 0; y < (int) length; y++)
                {
                    data[y] = 0;
                    
		    // check if integer or fixed-point
                    if (power+1 == bitlength)                   
                        data[y] = int(incoming[y]);
                    else  // fixed point
                    {
                        
			// check if negative
                        if (incoming[y] < 0)                    
                        {  
                            incoming[y] *= -1;
                            positive = -1;
                        }
                        else // positive
                            positive = 1;
                        
			// assign bits in fixed-point
                        for(z = bitlength-1; z >= 0; z--)
                        {
                            temp = incoming[y] / (pow(2,(power-bitlength+1+z)));
                            if (temp >= 1)  
                            {
				// move data in fixed-bit format to data array
				data[y] += (1 << z);
                                incoming[y] = incoming[y] - 
				    int(temp) * pow(2,power-bitlength+1+z);
                            }
                        }
			// assign sign
			data[y] *= positive;
		    } 
                }
                
		cerr << "Writing to hardware... (length = " << length 
		     << " addr = " << address 
		     << " pe = " << pe_num 
		     << ")\n";
                
		// transfer the data to wf4
                if ((rc = WF4_MemWrite(WF_BOARD,pe_num,address,length,data)) 
		    != WF4_SUCCESS)
		{
		    ACS_FAILURE("Ptolemy_to_WF",rc);
                    return(rc);         
		}
		else
		    cerr << "Destroying data arrays...\n"; 

                delete []incoming;
                delete []data;
            }           
        }  

	// Cleanup
	delete []filename;
        fclose(in);

        return(rc);
}


WF4_RetCode SDFWF4::WF_to_Ptolemy ()
{
# line 535 "SDFWF4.pl"
MPHIter nextpp(output);   
        PortHole* p;          
        WF4_RetCode rc=WF4_SUCCESS;
        DWORD pe_num, x, y, *data;
        ULONG address;
        int power, bitlength;
        FILE *in;
        char* filename=new char[120];
        
	ACSSet_Filename(SINKS_FILENAME,filename);
	
	// Open this file for output info
	if ((in = fopen (filename, "r")) == (FILE *) NULL)
        {
            cerr << "\n***Unable to open " << filename << "\n";
	    ACS_FAILURE("WF_to_Ptolemy",23);
            return(23);
        }
        else
        { 
            x=0;
            while ((p=nextpp++) != NULL)
            {
		// read addr of sinks and fixed-bit info
		fscanf(in, "%d %d %d %d", &pe_num, &address, &power, &bitlength);
		DWORD length=output_wordcounts->query(x++);
                data = new DWORD[length];
		printf("attempting to read %d words from pe %d, address %d\n",
		       length,
		       pe_num,
		       address);

		BOOLEAN blocked=TRUE;
		while (blocked)
		    WF4_MemGetBlockedStatus(WF_BOARD,pe_num,&blocked);
		if ((rc = WF4_MemRead(WF_BOARD, pe_num, address, length, data)) 
		    != WF4_SUCCESS)
		{
		    ACS_FAILURE("WF_to_Ptolemy",rc);
		    return(rc);         
		}

		// send data to output port
		for (y=0; y<length; y++) 
		    (*p)%y << double(int(data[length-y-1])*pow(2,0-bitlength+power+1));
                
                delete []data;
            }

        }

	// Cleanup
	delete []filename;
        fclose(in);

        return(rc);
}


WF4_RetCode SDFWF4::ACS_Peek (DWORD addr, DWORD length, int pe)
{
# line 607 "SDFWF4.pl"
DWORD data[length];
        WF4_RetCode rc;
        rc = WF4_SUCCESS;
        
	BOOLEAN blocked=FALSE;
	WF4_MemGetBlockedStatus(WF_BOARD,pe,&blocked);
	if (blocked)
	    cerr << "Memory to PE " << pe << " is currently blocked\n";
	else
	{
	    if ((rc = WF4_MemRead(WF_BOARD, pe, addr, length, data)) != WF4_SUCCESS)
		ACS_FAILURE("ACS_Peek",rc);         
	    
	    cerr << "\nDumping Memory from PE" << pe << " addr " << addr << " to " 
		<< addr+length << "\n";                   
	    
	    int max_slice=(int) ceil (length/6);
	    for (int slice = 0;slice <= max_slice ;slice++)
	    {
		int limit=6;
		if (slice==max_slice)
		    limit=(max_slice*6)-length;
		for (int y=0; y < limit; y++)
		{
		    int address=slice*6+y;
		    cerr << address << ": " << int(data[address]) << "  ";  
		}
		cerr << "\n";
	    }
	    cerr << "\n\n";                           
	}
	    
        return(rc);
}


WF4_RetCode SDFWF4::ACS_MainFunction ()
{
# line 654 "SDFWF4.pl"
DWORD PEZero, StatusWord;
        long timeOut;
        WF4_RetCode rc;
        BOOLEAN Done;
        
        rc= WF4_SUCCESS;
        PEZero= 0;
        timeOut= 0;
        Done= FALSE;
              
        cerr << STARNAME << "/Main: Loading Memory from input...\n";
        if ((rc = Ptolemy_to_WF()) == WF4_SUCCESS)     
            cerr << STARNAME << "/Main: Memory loaded\n";
        else
	{
	    ACS_FAILURE("ACS_MainFunction",rc);
            return(rc);
	}

//        ACS_Peek(0,30,1);
//        ACS_Peek(0,30,2);
//        ACS_Peek(0,30,3);
//        ACS_Peek(0,30,4);

	cerr << STARNAME << "/Main: Releasing clock...";               
        if ((rc= WF4_ClkSuspend(WF_BOARD, FALSE )) == WF4_SUCCESS )
            cerr << " PCLK released from suspension.\n";
        
	cerr << STARNAME << "/Main: Engaging algorithm...\n";
        if ((rc = ACS_Send_Tag()) == WF4_SUCCESS)              
            cerr << STARNAME << "/Main: FIFO loaded with Start Tag\n";
        else
            return(rc);

        cerr << STARNAME << "/Main: Waiting for CPE0 to interrupt.\n";
        do
        {                       
            if ((rc= WF4_PeQueryInterruptStatus(WF_BOARD, WF4_PE0, &StatusWord ))
		== WF4_SUCCESS )
            {
		// wait fot interupt
                if (BitTst( StatusWord,PEZero ))                               
                {
                    cerr << STARNAME << "/Main: Received interrupt from CPE(0).\n";
                    Done= TRUE;
                    rc= WF4_PeResetInterrupts(WF_BOARD, WF4_PE0 );  
                }
                else if ( timeOut++ == WF_TIMEOUT )
                {
                    cerr << STARNAME << "/Main: Interrupt ***not*** received, Timed Out.\n";
                    Done= TRUE;
                    rc= WF4_ERR_EVENT_TIMEOUT;
		    ACS_FAILURE("ACS_MainFunction",rc);
                    return(rc);
                }
            }
	    else
		ACS_FAILURE("ACS_MainFunction",rc);
        }
        while( rc==WF4_SUCCESS && !Done );
        
//        ACS_Peek(0,30,1);
//        ACS_Peek(0,30,2);
//        ACS_Peek(0,30,3);
//        ACS_Peek(0,30,4);

        cerr << STARNAME << "/Main: Outputting from memory...\n";    
        if ((rc = WF_to_Ptolemy()) != WF4_SUCCESS)                                  
            return(rc);

        return( rc );
}


WF4_RetCode SDFWF4::ACS_Send_Tag ()
{
# line 735 "SDFWF4.pl"
WF4_RetCode rc = WF4_SUCCESS;            
        DWORD Start_Tag = 0x01;
        DWORD buff, written;
        buff = 	PE_Start_Word;
        
	// send start tag
        cerr << STARNAME << "/Send_Data: Writing start tag to FIFO\n";
        if ((rc = WF4_FifoWriteTag(WF_BOARD, WF4_Fifo_Pe0, Start_Tag)) != WF4_SUCCESS)     
        {
	    ACS_FAILURE("ACS_Send_Tag",rc);
	    return(rc);
	}
        
        rc = WF4_FifoWrite(WF_BOARD, WF4_Fifo_Pe0, &buff, 1, &written, 5);
	if (rc != WF4_SUCCESS)
	    ACS_FAILURE("ACS_Send_Tag",rc);
	    
        return(rc);
}


WF4_RetCode SDFWF4::ACS_LoadPes (BOOLEAN enableInterrupts )
{
# line 764 "SDFWF4.pl"
WF4_RetCode rc=WF4_SUCCESS;
	char* bitstream_filename=new char[80];
        
        // Disable PE interrupts.
        if ((rc= WF4_PeDisableInterrupts(WF_BOARD, WF4_ALL_PES )) == WF4_SUCCESS )
        {                         
            // Assert reset lines.
            if ((rc= WF4_PeReset(WF_BOARD, WF4_ALL_PES ,TRUE )) != WF4_SUCCESS )
            {
                cerr << "ERROR: Failed to assert PE reset(s).\n" ;
		ACS_FAILURE("ACS_LoadPes",rc);
                return( rc );
            }
        }
        else
        {
            cerr << "ERROR: Failed to disable PE interrupt(s).\n";
	    ACS_FAILURE("ACS_LoadPes",rc);
            return( rc );
        }
        
        cerr << STARNAME << "/LoadPE: PE Reset asserted and Interrupts disabled.\n";
        
        
        // ***********************************LOAD PEs*****************************

	ACSRetrieve_BitstreamFilename(CPE0_FILENAME,bitstream_filename);
	cerr << STARNAME << "/LoadPE: Programming CPE0 with \"" << bitstream_filename 
	    << "\"...";
	if ((rc=WF4_PeProgram(WF_BOARD, WF4_PE(0), bitstream_filename)) == WF4_SUCCESS)
	    cerr << " Success\n";
	else
	{
	    cerr << " Failure\n";
	    ACS_FAILURE("ACS_LoadPes",rc);
	    return(rc);
	} 
        
	ACSRetrieve_BitstreamFilename(PE1_FILENAME,bitstream_filename);
        cerr << STARNAME << "/LoadPE: Programming PE1 with \"" 
	     << bitstream_filename << "\"...";
        if ((rc=WF4_PeProgram(WF_BOARD, WF4_PE(1), bitstream_filename)) == WF4_SUCCESS)
            cerr << " Success\n";
        else
        {
	    cerr << " Failure\n";
	    ACS_FAILURE("ACS_LoadPes",rc);
            return(rc);
        }
        
        
	ACSRetrieve_BitstreamFilename(PE2_FILENAME,bitstream_filename);
        cerr << STARNAME << "/LoadPE: Programming PE2 with \"" << bitstream_filename 
	     << "\"...";
        if ((rc=WF4_PeProgram(WF_BOARD, WF4_PE(2), bitstream_filename)) == WF4_SUCCESS)
            cerr << " Success\n";
        else
        {
	    cerr << " Failure\n";
	    ACS_FAILURE("ACS_LoadPes",rc);
            return(rc);
        }
        
	ACSRetrieve_BitstreamFilename(PE3_FILENAME,bitstream_filename);
        cerr << STARNAME << "/LoadPE: Programming PE3 with \"" << bitstream_filename 
	     << "\"...";
        if ((rc=WF4_PeProgram(WF_BOARD, WF4_PE(3), bitstream_filename)) == WF4_SUCCESS)
            cerr << " Success\n";
        else
        {
	    cerr << " Failure\n";
	    ACS_FAILURE("ACS_LoadPes",rc);
            return(rc);
        }
        
	ACSRetrieve_BitstreamFilename(PE4_FILENAME,bitstream_filename);
        cerr << STARNAME << "/LoadPE: Programming PE4 with \"" << bitstream_filename 
	     << "\"...";
        if ((rc=WF4_PeProgram(WF_BOARD, WF4_PE(4), bitstream_filename)) == WF4_SUCCESS)
            cerr << " Success\n";
        else
        {
	    cerr << " Failure\n";
	    ACS_FAILURE("ACS_LoadPes",rc);
            return(rc);
        }
        
        // Interrupts**************************************************
        
        cerr << STARNAME 
	    << "/LoadPE: Enable the interrupts on the selected PE(s) if specified...\n";
        if ((rc=WF4_PeEnableInterrupts(WF_BOARD, WF4_ALL_PES)) != WF4_SUCCESS )
        {
            cerr << "Failure";
	    ACS_FAILURE("ACS_LoadPes",rc);
            return( rc );
        }
        
        
        cerr << STARNAME << "/LoadPE: Deassert the reset line(s)...";
        if ((rc= WF4_PeReset(WF_BOARD, WF4_ALL_PES,FALSE )) != WF4_SUCCESS )
        {
            cerr << "Failure";
	    ACS_FAILURE("ACS_LoadPes",rc);
            return( rc );
        }  
        cerr << " PE Reset deasserted and Interrupts enabled.\n";

	// Cleanup
	delete []bitstream_filename;
        
        return( rc );
}


// prototype instance for known block list
static SDFWF4 proto;
static RegisterBlock registerBlock(proto,"WF4");
