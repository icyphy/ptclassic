#include <std.h>
#include "DataStruct.h"

/****************************************************************
StringList stores a list of char* strings, and can then read
them back one-by-one, or can concatinate them into a single
char* string. A StringList can only grow; there is no way to
take something away that has been added to the list.

Typical use: For a class that wants to cast into a char*,
the strings can be accumulated into a StringList, and then
let StringList do the cast into char*.

StringList implements a small subset of the function of the
String class

Programmer: D.G. Messerschmitt, U.C. Berkeley
Date: Jan 11, 1990
*******************************************************************/

class StringList : SequentialList
{
public:
	// Put first string on list; operators = and += act identically
	operator = (char*);
	operator = (int);
	operator = (float);

        // Add string to list
        operator += (char*);
	operator += (int);
	operator += (float);

        // Return size of list
        int size() {return SequentialList::size();}

        // Return next string on list
        char* next () {return (char*) SequentialList::next();}
 
        // Reset to start of the list
        void reset() {SequentialList::reset();}

	// Cast list of character strings into a single character string
	// This operator copies string into static memory (to save memory
	//   and avoid having to delete dynamically allocated memory)
	// Caution: if two instances of StringList are both cast into
	//   char*, the second will overwrite the first! 
	operator char* ();

	StringList() {totalSize=0;}

private:
	int totalSize;
};
