defstar {
	name { ReadAIFF }
	domain { SDF }
	version { $Id$ }
	desc {
Read a binary AIFF file.  Return one sample on each firing.  The file 
format that is read is the same as the one written by the PlayAIFF star.
The simulation can be halted on end-of-file, or the file contents can be
periodically repeated, or the file contents can be padded with zeros.
	}
	author { Charles B. Owen }
	copyright {
Copyright (c) 1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	hinclude { "pt_fstream.h" }
        
	ccinclude { "SimControl.h", <string.h>, <math.h> }

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

#define SDFREADAIFF_SUCCESS       0
#define SDFREADAIFF_NOFILE	 -1
#define SDFREADAIFF_CANT_READ	 -2
#define SDFREADAIFF_COMPRESSED	 -3
#define SDFREADAIFF_UNRECOGNIZED -4
	}

	output {
		name { output }
		type { int }
	}

	defstate {
		name { fileName }
		type { string }
		default { "sound.aiff" }
		descriptor { Input file. }
	}
	defstate {
		name { haltAtEnd }
		type { int }
		default { "YES" }
		descriptor { Halt the run at the end of the input file. }
	}

	defstate {
		name { periodic }
		type { int }
		default { "NO" }
		descriptor { Make output periodic or zero-padded. }
	}

	private {
	    // Methods
	    const char* ReadErrorString(int ret);
	    int ReadFileHeader();
	    int ReadChunk(Chunk &chunk);
	    int ReadID(ID id);
	    int ReadLONG(long &item);
	    int ReadULONG(unsigned long &item);
	    int ReadSHORT(short &item);
	    int ReadSHORT(int &item);
	    int ReadChunkHeader(ChunkHeader &chunk);
	    void extended_to_double(const unsigned char *ps, double &pd);

	    // Data members
	    pt_ifstream input;
	    int numChannels;
	    unsigned long numSampleFrames;
	    int sampleSize;
	    double sampleRate;
	    int sound_position;
	    int sound_read;
	}

	destructor {
	    input.close();
	}

	setup {
	    input.close();
	    input.open(fileName);
	    if (!input)
	    {
		Error::abortRun(*this, "Unable to open file ", fileName);
		return;
	    }

	    sound_position = -1;

	    int ret;
	    if ( (ret = ReadFileHeader()) < 0)
	    {
		Error::abortRun(*this, ReadErrorString(ret));
		return;
	    }

	    if (sound_position < 0)
	    {
		Error::abortRun(*this,
				"Unable to find sound data in AIFF file.");
		return;
	    }

	    input.clear();
	    input.seekg(sound_position);
	    sound_read = 0;
	}

	go {
	    if ((unsigned long)sound_read >= numSampleFrames)
	    {
		// End of file.  Check what we are to do.
		if(haltAtEnd)
		{
		    SimControl::requestHalt();
		    return;
		}
	        else if (periodic)
		{
		    input.seekg(sound_position);
		    sound_read = 0;
		}
	    }

	    // Read the sample
	    short sample;
	    ReadSHORT(sample);
	    output%0 << sample;

	    // Handle possible multiple channels
	    if (numChannels > 1) ReadSHORT(sample);

	    sound_read++;
	}

	wrapup {
	    input.close();
	}

        code {
/*
 *  Name :         SDFReadAIFF::ReadChunk()
 *  Description :  This reads an object of type Chunk from disk,
 *                 for any type of machine.
 */
int
SDFReadAIFF::ReadChunk(Chunk &chunk)
{
   ReadID(chunk.ckID);
   ReadLONG(chunk.ckSize);
   ReadID(chunk.formType);

   return !input.fail();
}


/*
 *  Name :         SDFReadAIFF::ReadChunkHeader()
 *  Description :  This reads an object of type ChunkHeader from disk,
 *                 for any type of machine.
 */
int
SDFReadAIFF::ReadChunkHeader(ChunkHeader &chunk)
{
   ReadID(chunk.ckID);
   ReadLONG(chunk.ckSize);

   return !input.fail();
}

/*
 *  Name :         SDFReadAIFF::ReadID()
 *  Description :  Reads an AIFF/AIFC file object of type ID.
 */
int
SDFReadAIFF::ReadID(ID id)
{
   input.read(id, 4);
   return 1;
}


/*
 *  Name :         SDFReadAIFF::ReadLONG()
 *  Description :  Reads an AIFF/AIFC file object of type LONG.
 */
int
SDFReadAIFF::ReadLONG(long &item)
{
   int i;
   unsigned char b[4];

   input.read(b, 4);

   item = 0;

   for(i=0;  i<4;  i++)
   {
      item <<= 8;
      item |= b[i];
   }

   return 1;
}


/*
 *  Name :         SDFReadAIFF::ReadULONG()
 *  Description :  Reads an AIFF/AIFC file object of type LONG.
 */
int
SDFReadAIFF::ReadULONG(unsigned long &item)
{
   int i;
   unsigned char b[4];

   input.read(b, 4);
   item = 0;

   for(i=0;  i<4;  i++)
   {
      item <<= 8;
      item |= b[i];
   }

   return 1;
}


/*
 *  Name :         SDFReadAIFF::ReadSHORT()
 *  Description :  Reads an AIFF/AIFC file object of type SHORT
 */
int
SDFReadAIFF::ReadSHORT(short &item)
{
   unsigned char b[2];

   input.read(b, 2);
   item = (b[0] << 8) | b[1];
   return 1;
}

/*
 *  Name :         SDFReadAIFF::ReadSHORT()
 *  Description :  Reads an AIFF/AIFC file object of type SHORT
 *                 into an integer.  Note:  Unsigned.
 */
int
SDFReadAIFF::ReadSHORT(int &item)
{
   unsigned char b[2];

   input.read(b, 2);
   item = (b[0] << 8) | b[1];
   return 1;
}

/*
 *  Name :         SDFReadAIFF::extended_to_double()
 *  Description :  Convert stupid Apple extended floating point format
 *                 to a double.
 */
void 
SDFReadAIFF::extended_to_double(const unsigned char *ps, double &pd)
{
   int i, neg, e, b;
   float m;
   unsigned char andval;
   
   neg = (ps[0] & 0x80) != 0;

   /* Determine the exponent */
   e = (((int)ps[0] & 0x7f) << 8) + ps[1];

   /* Compute the mantissa */
   m = 0;
   andval = 0x01;
   b = 9;

   for(i=0;  i<64;  i++)
   {
      m /= 2.;
      if(ps[b] & andval)
	 m += 1.;

      andval <<= 1;
      if(andval == 0)
      {
	 andval = 0x01;
	 b--;
      }

   }

   if(m == 0)
   {
      pd = 0;
   }
   else
   {
      pd = m * pow(2.0, (float)(e - 16383));
      if(neg)
	 pd = -pd;
   }

}

/*
 *  Name :         SDFReadAIFF::ReadErrorString()
 *  Description :  Convert error code return by ReadFileHeader into a string
 */
const char*
SDFReadAIFF::ReadErrorString(int ret)
{
    char* str = "";

    switch (ret) {
      case SDFREADAIFF_NOFILE:
	str = "File does not exist";
	break;

      case SDFREADAIFF_CANT_READ:
	str = "Error while reading the AIFF file";
	break;

      case SDFREADAIFF_COMPRESSED:
	str = "Compressed AIFF file not supported.";
	break;

      case SDFREADAIFF_UNRECOGNIZED:
	str = "AIFF file format not recognized";
	break;
    }

    return str;
}

/*
 *  Name :         ReadFileHeader::ReadFileHeader()
 *  Description :  Read the AIFF file
 */
int
SDFReadAIFF::ReadFileHeader()
{
   // Read the form chunk.
   Chunk FormChunk;
   if(!ReadChunk(FormChunk)) return SDFREADAIFF_NOFILE;

   // Ensure this is actually a FORM chunk.
   if (strncmp(FormChunk.ckID, "FORM", 4) != 0) return SDFREADAIFF_NOFILE;

   int isAIFC;
   if (strncmp(FormChunk.formType, "AIFF", 4) == 0)
      isAIFC = 0;
   else if (strncmp(FormChunk.formType, "AIFC", 4) == 0)
      isAIFC = 1;
   else
      return SDFREADAIFF_NOFILE;

   // Read the chunks.  Chunks need not be in any particular order.

   ChunkHeader Header;
   while (ReadChunkHeader(Header))
   {
      // Offset to point after the current chunk
      int seek_offset = input.tellg() + Header.ckSize;

      if (strncmp(Header.ckID, "COMM", 4) == 0) 
      {
	 // Read comm chunk
	 unsigned char xsampleRate[10];

	 ReadSHORT(numChannels);
	 ReadULONG(numSampleFrames);
	 ReadSHORT(sampleSize);
	 input.read(xsampleRate, sizeof(char[10]));

	 // Check for error while reading
	 if (!input) return SDFREADAIFF_CANT_READ;

	 extended_to_double(xsampleRate, sampleRate);
	    
	 if(isAIFC) 
	 {
	    ID compType;

	    /* 
	     * This is an AIFC audio file.  The format of the common
	     * data chunk is different from that of the AIFF file.  All
	     * is the same at the beginning, but there is an additional
	     * compression type at the end.
	     */

	    ReadID(compType);
	    if(strncmp(compType, "NONE", 4)) return SDFREADAIFF_COMPRESSED;
	 }
      } 
      else if (strncmp(Header.ckID, "SSND", 4) == 0) 
      {
	 /*
	  * read ssnd chunk
	  */
	 unsigned long offset;
	 unsigned long blockSize;

	 ReadULONG(offset);
	 ReadULONG(blockSize);

	 /*
	  * record position of sound data
	  */

	 sound_position = input.tellg();
      } 
      else if (strncmp(Header.ckID, "FVER", 4) == 0)
      {
	 /*
	  * This indicates an AIFF-C file version.  Ensure it
	  * matches the version for which is program was updated.
	  */

	 unsigned long timestamp;
	 ReadULONG(timestamp);

	 /* Ensure this is the correct version of AIFF-C */

         const unsigned long AIFCVersion1 = 0xA2805140l; // Version 1 of AIFF-C

	 if(timestamp != AIFCVersion1) return SDFREADAIFF_UNRECOGNIZED;
      }

      // Seek to after the current chunk
      input.seekg(seek_offset);
      if (!input) return SDFREADAIFF_CANT_READ;
   }

   return SDFREADAIFF_SUCCESS;
}

	}

}

