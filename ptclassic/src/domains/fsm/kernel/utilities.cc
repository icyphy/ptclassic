static const char file_id[] = "utilities.cc";

/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer:  Bilung Lee
 Date of creation: 9/26/96

 strParser parses a string using either double-quotes or curly-braces as
 the delimitter, and returns an array of parsed strings. 
 NOTE: User is responsible for deleting the memory allocated to those
 parsed strings by using "delete [] arrayName".

*************************************************************************/

#include <string.h>
#include "Error.h"
#include "type.h"

// Parse a string using either double-quotes or curly-braces as the delimitter.
char** strParser(const char* string,int& numStr,const char* type) {
    char** parsedStr = 0;
    int start = 0;
    int end = 0;

    if (strcmp(type, "double-quote") == 0) {
      int numQuote = 0;
      while (string[start] != '\0') {
	if (string[start] == '\"') numQuote++;
	start++;
      }
      if (numQuote%2 != 0) {
	Error::abortRun("Cannot parse the string: Unmatched double-quote. ",
			"Original string: ", string);
	return 0;	      
      }
      numStr = numQuote/2;

    }
    else if (strcmp(type, "curly-brace") == 0) {
      numStr = 0;
      while (string[start] != '\0') {
	if (string[start] == '{') numStr++;
	start++;
      }
    }
    else {
	Error::abortRun("Cannot parse the string: Unrecognized delimitter ",
			"type for strParser. Original string: ", string);
	return 0;
    }
    parsedStr = new char *[numStr];

    start = 0;
    end = 0;
    numStr = 0;
    while (1) {
      // Skip all the prefix spaces.
      while (string[start] == ' ') start++;
      
      if (string[start] == '\0') break ; // Hit end of string.
      
      if (strcmp(type, "double-quote") == 0) {
	// Find the left-side quote.
	if (string[start] != '\"') {
	  Error::abortRun("Cannot parse the string: Each string must be ",
	   "surrounded by a pair of double-quotes. Original string: ", string);
	  return 0;
	}
	start++;
	end = start;

 	// Find the right-side quote.
	while (string[end] != '\"') end++;
      }
      else if (strcmp(type, "curly-brace") == 0) {
	// Find the left-side curly-brace.
	if (string[start] != '{') {
	  Error::abortRun("Cannot parse the string: Each string must be ",
	    "surrounded by a pair of curly-braces. Original string: ", string);
	  return 0;
	}
	start++;
	end = start;

       	// Find the right-side curly-brace.
	while (string[end] != '}' && string[end] != '\0' ) end++;

	if (string[end] == '\0') {
	  Error::abortRun("Cannot parse the string: Missing right-side ",
			  "curly-brace. Original string: ", string);
	  return 0;
	}
      }
	
      parsedStr[numStr] = (char *) new char[end-start+1];
      for (int i = 0; i < end-start; i++) {
	parsedStr[numStr][i] = string[start+i];
      }
      parsedStr[numStr][i] = '\0'; 

      numStr++;

      start = end+1;
    }
    return parsedStr;
}
