#include <std.h>
#include "StringList.h"

#define SMALL_STRING 10

StringList :: operator = (char* s)
	 {put(s);totalSize+=strlen(s);}
 
StringList :: operator += (char* s)
	 {put(s);totalSize+=strlen(s);}

StringList :: operator = (int i)
{
	char* s = new char[SMALL_STRING];
	sprintf(s,"%d",i);
	put(s);
	totalSize+=strlen(s);
}
  
StringList :: operator += (int i)
{
        char* s = new char[SMALL_STRING]; 
        sprintf(s,"%d",i); 
        put(s);
        totalSize+=strlen(s);
}

StringList :: operator = (float f)
{
        char* s = new char[SMALL_STRING]; 
        sprintf(s,"%f",f); 
        put(s);
        totalSize+=strlen(s);
}
  
StringList :: operator += (float f)
{
        char* s = new char[SMALL_STRING]; 
        sprintf(s,"%f",f); 
        put(s);
        totalSize+=strlen(s);
}

StringList :: operator char* ()
{
	char* s = new char[totalSize];
	int totalSoFar = 0;
	reset();
	for(int i = size(); i > 0; i--) {
		char* ss = next();
		strcpy(s+totalSoFar,ss);
		totalSoFar += strlen(ss);
		}
	return s;
}
