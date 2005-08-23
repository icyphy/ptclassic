defstar {
	name { PlayAIFF }
	domain { SDF }
	version { @(#)SDFPlayAIFF.pl	1.7	04/28/98 }
	desc {
Play an AIFF input stream on the workstation speaker.
	}
	htmldoc {
The "gain" state (default 10000.0) multiplies the input stream
before it is written.  The inputs should be in the range of 
-32000.0 to 32000.0.  The file is played at a fixed sampling 
rate entered in the dialog box.  Sampling rate is in 
samples/second and the default is 44100.
When the wrapup method is called, a file of 
samples is handed to a program named "ptaiffplay" which plays the file.
The ptaiffplay program must be in your path.
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
	    // AIFF Type Definitions
	    typedef char  ID[4];

	    struct  Chunk {
		ID      ckID;
		long    ckSize;
		ID      formType;
	    };

	    struct  ChunkHeader {
		ID      ckID;
		long    ckSize;
	    };
	}

	input {
		name { input }
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
	    // Methods
	    int WriteChunk(const Chunk &chunk);
	    int WriteChunkHeader(const ChunkHeader &chunk);
	    void WriteFileHeader();
	    int WriteID(const ID id);
	    int WriteLONG(long item);
	    int WriteULONG(unsigned long item);
	    int WriteSHORT(int item);
	    void WriteCommHeader();
	    void double_to_extended(unsigned char *ps, double pd);

	    // Data members
	    pt_ofstream output;
	    char* useName;
	    int numChannels;
	    unsigned long numSampleFrames;
            int delFile;
	}

        constructor {
	    useName = 0;
	}
	destructor {
	    delete [] useName;
	    output.close();
	}

	setup {
	    // Check for required program
	    if (playFile && progNotFound("ptaiffplay", 
			   "Sound files cannot be played without it."))
	        return;

	    // If name is empty, use a temp file
	    delete [] useName;
	    const char *sf = fileName;
	    if (sf == NULL || *sf == 0)
	    {
		useName = tempFileName();
		delFile = TRUE;
	    }
	    else
	    {
		useName = expandPathName(fileName);
		delFile = 0;
	    }

	    output.open(useName);
	    if (!output)
	    {
		Error::abortRun(*this, "Can't open file ", useName);
		return;
	    }

	    WriteFileHeader();

	    // We're ready to write audio now (we write the other stuff last)
	    numSampleFrames = 0;
	}

	go {
	    // Ensure clipping of the signal.
	    float fsample = double(input%0) * double(gain);
	    if (fsample < -32768.)
		fsample = -32768.;
	    else if (fsample > 32767.)
		fsample = 32767.;

	    int sample = int(fsample);
	    WriteSHORT(sample);
	    numSampleFrames++;
	}

	wrapup {
	    WriteCommHeader();

	    if (!output)
	    {
		Error::abortRun(*this, "Failure writing AIFF file");
		return;
	    }
	    output.close();

	    if (playFile)
	    {
	   	StringList cmd;
		if (delFile) cmd += "( ";
		cmd += "ptaiffplay ";
		cmd += useName;
		if( delFile)
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
 *  Name :         SDFPlayAIFF::WriteFileHeader()
 *  Description :  This writes the header for an AIFF file
 */
void
SDFPlayAIFF::WriteFileHeader()
{
   // Write the file header
   Chunk form = {"",0,""};
   strncpy(form.ckID, "FORM", 4);
   strncpy(form.formType, "AIFF", 4);
   form.ckSize = 0;      // Have to rewrite later
   WriteChunk(form);

   // Start the SSND chunk
   ChunkHeader ssnd = {"",0};
   strncpy(ssnd.ckID, "SSND", 4);
   ssnd.ckSize = 0;           // Have to rewrite later
   WriteChunkHeader(ssnd);

   WriteULONG(0l);
   WriteULONG(0l);
}

/*
 *  Name :         SDFPlayAIFF::WriteChunk()
 *  Description :  This writes an object of type Chunk to disk,
 *                 for any type of machine.
 */
int
SDFPlayAIFF::WriteChunk(const Chunk &chunk)
{
   WriteID(chunk.ckID);
   WriteLONG(chunk.ckSize);
   WriteID(chunk.formType);

   return !output.fail();
}

/*
 *  Name :         SDFPlayAIFF::WriteChunkHeader()
 *  Description :  This writes an object of type ChunkHeader to disk,
 *                 for any type of machine.
 */
int
SDFPlayAIFF::WriteChunkHeader(const ChunkHeader &chunk)
{
   WriteID(chunk.ckID);
   WriteLONG(chunk.ckSize);

   return !output.fail();
}

/*
 *  Name :         SDFPlayAIFF::WriteID()
 *  Description :  Writes an AIFF/AIFC file object of type ID.
 */
int
SDFPlayAIFF::WriteID(const ID id)
{
   output.write(id, 4);
   return 1;
}

/*
 *  Name :         SDFPlayAIFF::WriteLONG()
 *  Description :  Writes an AIFF/AIFC file object of type LONG.
 */
int
SDFPlayAIFF::WriteLONG(long item)
{
   int i;
   unsigned char b[4];

   for(i = 0; i < 4; i++)
   {
      b[3-i] = (unsigned char) (item & 0xff);
      item >>= 8;
   }

   output.write(b, 4);

   return 1;
}

/*
 *  Name :         SDFPlayAIFF::WriteULONG()
 *  Description :  Writes an AIFF/AIFC file object of type LONG.
 */
int
SDFPlayAIFF::WriteULONG(unsigned long item)
{
   int i;
   unsigned char b[4];

   for(i = 0; i < 4; i++)
   {
      b[3-i] = (unsigned char) (item & 0xff);
      item >>= 8;
   }

   output.write(b, 4);

   return 1;
}

/*
 *  Name :         SDFPlayAIFF::WriteSHORT()
 *  Description :  Writes an AIFF/AIFC file object of type SHORT
 */
int
SDFPlayAIFF::WriteSHORT(int item)
{
   unsigned char b[2];

   b[0] = item >> 8;
   b[1] = item & 0xff;
   output.write(b, 2);
   return 1;
}

/*
 *  Name :         SDFPlayAIFF::double_to_extended()
 *  Description :  Convert double to stupid APPLE SANE format.
 */
void
SDFPlayAIFF::double_to_extended(unsigned char *ps, double pd)
{
   int neg = ( pd < 0 );
   int i, e;
   double m;

   // Initialize locations to zero
   for (i = 0; i < 10; i++)
      ps[i] = 0;

   if (neg) pd = -pd;

   // Zero is treated as special case
   if (pd != 0)
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

      for(i = 0; i < 64; i++, m *= 2.0)
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

/*
 *  Name :         SDFPlayAIFF::WriteCommHeader()
 *  Description :  Fill in the rest of the AIFF file.
 */
void
SDFPlayAIFF::WriteCommHeader()
{
   ChunkHeader comm = {"",0};
   strncpy(comm.ckID, "COMM", 4);
   comm.ckSize = 18;
   WriteChunkHeader(comm);
   WriteSHORT(1);			// 1 channel
   WriteULONG(numSampleFrames);
   WriteSHORT(16);			// sample size in bits
   unsigned char xsampleRate[10];
   double_to_extended(xsampleRate, sampleRate);
   output.write((const char *)xsampleRate, 10);

   // Now we have to go back and fill in the rest.
   unsigned long filelen = output.tellp();
   output.seekp(4);
   WriteULONG(filelen - 8);
   output.seekp(16);
   WriteULONG(numSampleFrames * 2 + 8);
}

	}
}
