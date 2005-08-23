defstar {
	name { PlayAIFF2 }
	domain { SDF }
	version { @(#)SDFPlayAIFF2.pl	1.6	04/28/98 }
	desc {
Play a stereo AIFF input stream on the workstation speaker.
}
	htmldoc {
The "gain" state (default 10000.0) multiplies the input streams
before they are written.  The inputs should be in the range 
of -32000.0 to 32000.0.  The file is played at a fixed sampling 
rate entered in the dialog box.  Sampling rate is in samples/second 
and the default is 44100.  When the wrapup method is called, a file of 
samples is handed to a program named "ptaiffplay" which plays the file.
The "ptaiffplay" program must be in your path.
ptaiffplay is a simple
shell script:
<pre>
#!/bin/csh
/usr/sbin/sfplay $1
</pre>
/usr/sbin/sfplay is present under SGI Irix.  Under other operating
systems try the AudioFile program.
	}
	author { Charles B. Owen }
	copyright {
Copyright (c) 1997-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	hinclude {"pt_fstream.h"}
	ccinclude {"SimControl.h", <string.h>, <math.h>, "miscFuncs.h", "paths.h", <std.h>}

	header {

#define AIFF_NUM_CHANNELS 2
#define AIFF_BITS_PER_SAMPLE 16
#define AIFF_ID_LENGTH 4
#define AIFF_ID_EMPTY "   "

// AIFF Type Definitions
typedef char  AIFF_ID[AIFF_ID_LENGTH];

struct  AIFFChunk {
    AIFF_ID ckID;
    long    ckSize;
    AIFF_ID formType;
};

#define AIFF_CHUNK_INITIAL { AIFF_ID_EMPTY, 0L, AIFF_ID_EMPTY }

struct  AIFFChunkHeader {
    AIFF_ID ckID;
    long    ckSize;
};

#define AIFF_CHUNK_HEADER_INITIAL { AIFF_ID_EMPTY, 0L }

}

	input {
		name { left }
		type { float }
	}
	input {
		name { right }
		type { float }
	}
	defstate {
		name { gain }
		type { float }
		default { "10000.0" }
		desc { Output gain. }
	}
	defstate {
		name {fileName}
		type {string}
		default {""}
		desc {File name for the saved the AIFF file.}
	}
        defstate {
                name {playFile}
                type {int}
                default{"YES"}
                desc{Play file using ptaiffplay program?}
        }

	defstate {
		name {sampleRate}
		type {int}
		default {"44100"}
		desc {Sample rate to save file as.}
	}

	private {
	    int WriteChunk(const AIFFChunk &chunk);
	    int WriteChunkHeader(const AIFFChunkHeader &chunk);
	    int WriteID(const AIFF_ID id);
	    int WriteLONG(long item);
	    int WriteULONG(unsigned long item);
	    int WriteSHORT(int item);
	    void double_to_extended(unsigned char *ps, double pd);
	    
	    pt_ofstream output;
	    char *useName;
	    int numChannels;
	    unsigned long numSampleFrames;
            int delFile;
	}

        constructor { useName = 0; }
	destructor { delete [] useName; output.close(); }

	setup {
   // Check for required program
   if(playFile && progNotFound("ptaiffplay", 
		   "Sound files cannot be played without it."))
      return;

   // If name is empty, use a temp file
   delete [] useName;
   const char *sf = fileName;
   if(sf == NULL || *sf == 0)
   {
      useName = tempFileName();
      delFile = 1;
   }
   else
   {
      useName = expandPathName(fileName);
      delFile = 0;
   }

   output.open(useName);
   if(!output)
   {
      Error::abortRun(*this, "Can't open file ", useName);
      return;
   }

   // Write the file header
   AIFFChunk form = AIFF_CHUNK_INITIAL;
   strncpy(form.ckID, "FORM", AIFF_ID_LENGTH);
   strncpy(form.formType, "AIFF", AIFF_ID_LENGTH);
   form.ckSize = 0;      // Have to rewrite later
   WriteChunk(form);

   // Start the SSND chunk
   AIFFChunkHeader ssnd = AIFF_CHUNK_HEADER_INITIAL;
   strncpy(ssnd.ckID, "SSND", AIFF_ID_LENGTH);
   ssnd.ckSize = 0;           // Have to rewrite later
   WriteChunkHeader(ssnd);

   WriteULONG(0L);
   WriteULONG(0L);

   // We are ready to write audio at this point (we write the other stuff last)
   numSampleFrames = 0;
	}

	go {
	    // Ensure clipping of the signal.
	    float fsample1 = double(left%0) * double(gain);
	    if(fsample1 < -32768.)
		fsample1 = -32768.;
	    else if(fsample1 > 32767.)
		fsample1 = 32767.;
	      
	    float fsample2 = double(right%0) * double(gain);
	    if(fsample2 < -32768.)
		fsample2 = -32768.;
	    else if(fsample2 > 32767.)
		fsample2 = 32767.;
	      
	    int sample1 = int(fsample1);
	    int sample2 = int(fsample2);
	    WriteSHORT(sample1);
	    WriteSHORT(sample2);
	    numSampleFrames++;
	}

	wrapup {
   // Write the COMM header
   AIFFChunkHeader comm = AIFF_CHUNK_HEADER_INITIAL;
   strncpy(comm.ckID, "COMM", AIFF_ID_LENGTH);
   comm.ckSize = 18;
   WriteChunkHeader(comm);

   WriteSHORT(AIFF_NUM_CHANNELS);        // two channels
   WriteULONG(numSampleFrames);
   WriteSHORT(AIFF_BITS_PER_SAMPLE);     // sample size in bits

   unsigned char xsampleRate[10];
   double_to_extended(xsampleRate, sampleRate);
   output.write((const char *)xsampleRate, 10);

   // Now we have to go back and fill in the rest.
   unsigned long filelen = output.tellp();
   output.seekp(4);
   WriteULONG(filelen - 8);
   output.seekp(16);
   WriteULONG(numSampleFrames * sizeof(long) + 8);

   if(!output)
   {
      Error::abortRun(*this, "Failure writing AIFF file");
      return;
   }
   output.close();

   if(playFile)
   {
      StringList cmd;
      if(delFile)
	 cmd += "( ";
      cmd += "ptaiffplay ";
      cmd += useName;
      if(delFile)
      {
	 cmd += "; /bin/rm -f ";
	 cmd += useName;
	 cmd += ")";
      }

      cmd += "&";
      system(cmd);
      delFile = 0;
   }
	}

	code {
/*
 *  Name :         SDFPlayAIFF2::WriteChunk()
 *  Description :  This writes an object of type AIFFChunk to disk,
 *                 for any type of machine.
 */

int
SDFPlayAIFF2::WriteChunk(const AIFFChunk &chunk)
{
   WriteID(chunk.ckID);
   WriteLONG(chunk.ckSize);
   WriteID(chunk.formType);

   return !output.fail();
}


/*
 *  Name :         SDFPlayAIFF2::WriteChunkHeader()
 *  Description :  This writes an object of type AIFFChunkHeader to disk,
 *                 for any type of machine.
 */

int
SDFPlayAIFF2::WriteChunkHeader(const AIFFChunkHeader &chunk)
{
   WriteID(chunk.ckID);
   WriteLONG(chunk.ckSize);

   return !output.fail();
}

/*
 *  Name :         SDFPlayAIFF2::WriteID()
 *  Description :  Writes an AIFF/AIFC file object of type AIFF_ID.
 */

int
SDFPlayAIFF2::WriteID(const AIFF_ID id)
{
   output.write(id, AIFF_ID_LENGTH);
   return 1;
}


/*
 *  Name :         SDFPlayAIFF2::WriteLONG()
 *  Description :  Writes an AIFF/AIFC file object of type LONG.
 */

int
SDFPlayAIFF2::WriteLONG(long item)
{
   int i;
   unsigned char b[sizeof(long)];

   for(i = 0; i < int(sizeof(long));  i++)
   {
      b[3-i] = (unsigned char) (item & 0xff);
      item >>= 8;
   }

   output.write(b, sizeof(long));

   return 1;
}


/*
 *  Name :         SDFPlayAIFF2::WriteULONG()
 *  Description :  Writes an AIFF/AIFC file object of type LONG.
 */

int
SDFPlayAIFF2::WriteULONG(unsigned long item)
{
   int i;
   unsigned char b[sizeof(long)];

   for(i = 0; i < int(sizeof(long)); i++)
   {
      b[3-i] = (unsigned char)(item & 0xff);
      item >>= 8;
   }

   output.write(b, sizeof(long));

   return 1;
}


/*
 *  Name :         SDFPlayAIFF2::WriteSHORT()
 *  Description :  Writes an AIFF/AIFC file object of type SHORT
 */

int
SDFPlayAIFF2::WriteSHORT(int item)
{
   unsigned char b[2];

   b[0] = item >> 8;
   b[1] = item & 0xff;
   output.write(b, 2);
   return 1;
}


/*
 *  Name :         SDFPlayAIFF2::double_to_extended()
 *  Description :  Convert double to stupid APPLE SANE format.
 */

void
SDFPlayAIFF2::double_to_extended(unsigned char *ps, double pd)
{
   int neg = pd < 0;
   int i, e;
   double m;

   // Initialize locations to zero
   for(i=0;  i<10;  i++)
      ps[i] = 0;

   if(neg)
      pd = -pd;

   if(pd == 0)
   {
      /* Zero treated as special case */
   }
   else
   {
      int orval;
      int bval;

      /* Determine the exponent */
      e = int(log(pd) / log(2.));
      m = pd / pow(2., (double)e);

      /* Code the exponent */
      e += 16383;  /* offset the exponent */
      ps[0] = ((e >> 8) & 0x7f) | (neg ? 0x80 : 0);
      ps[1] = e & 0xff;

      /* Code the mantissa */

      orval = 0x80;
      bval = 2;

      for(i=0;  i<64;  i++, m *= 2.0)
      {
	 if(m >= 1)
	 {
	    ps[bval] |= orval;
	    m -= 1.0;
	 }

	 orval >>= 1;
	 if(orval == 0)
	 {
	    bval++;
	    orval = 0x80;
	 }
      }

   }
}

	}
}
