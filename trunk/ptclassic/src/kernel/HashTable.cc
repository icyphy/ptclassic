static const char file_id[] = "HashTable.cc";

/**************************************************************************
Version identification:
$Id$

Ported from John Ousterhout's Tcl implementation by Joe Buck

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


This code is derived from the hash table implementation in Tcl version 7.0.

It differs in that the key is always a character string, and is on the
heap; we don't use the trick of using a variable-sized array at the end
of a struct since it isn't legal C++ and would lead to problems for
derived classes.

Further, a mechanism is added to automatically clean up entries on
deletion.  Finally, "const" is used where appropriate.

This code is essentially just a C++ification of the tclHash code by
John Ousterhout from Tcl version 7.0, except that we only implement
string-valued hash keys.

**************************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "Error.h"
#include <stdio.h>
#include "miscFuncs.h"
#include "HashTable.h"

static unsigned int	HashString (const char *string);

// we wish to have less than REBUILD_MULTIPLIER entries per bin, on average.
// We rebuild when we exceed that.
const int REBUILD_MULTIPLIER = 3;

// This is the default value by which we increase the number of buckets on
// a rebuild.  It must be a power of two.
const int REBUILD_INCREASE_FACTOR = 4;

// Body of constructor for HashTable: prepare for use, initially has statically
// allocated buckets.

void HashTable::init()
{
	buckets = staticBuckets;
	for (int i = 0; i < SMALL_HASH_TABLE; i++)
		staticBuckets[i] = 0;
	numBuckets = SMALL_HASH_TABLE;
	numEntries = 0;
	rebuildSize = SMALL_HASH_TABLE*REBUILD_MULTIPLIER;
}

// Public insertion function: insert data into table.
// Clean up previous value if any.

void HashTable::insert(const char* key, Pointer data)
{
	HashEntry* h = createEntry(key);
	Pointer oldData = h->clientData;
	h->clientData = data;
	if (oldData != data)
		cleanup(oldData);
}

// remove entry from hash table.  The entry and its clientData is deleted
// and should not be referred to again.

int HashTable::removeEntry(HashEntry *entryPtr)
{
	if (entryPtr->tablePtr != this)
		return FALSE;
	register HashEntry *prev;

	if (*entryPtr->bucketPtr == entryPtr) {
		*entryPtr->bucketPtr = entryPtr->nextPtr;
	} else {
		for (prev = *entryPtr->bucketPtr; ; prev = prev->nextPtr) {
			if (prev == NULL) {
				// Impossible!
				Error::abortRun("internal error in HashTable");
				return FALSE;
			}
			if (prev->nextPtr == entryPtr) {
				prev->nextPtr = entryPtr->nextPtr;
				break;
			}
		}
	}
	numEntries--;
	LOG_DEL; delete entryPtr;
	return TRUE;
}


// Body of destructor: deletes entries and dynamically allocated buckets.
// Hashtable is unusable after calling this function!  But
// destroy followed by init is used to implement clear.

void HashTable::destroy() 
{
	if (buckets == 0) return; // permit repeated calls.

	register HashEntry *hPtr, *nextPtr;
	int i;

	// Free up all the entries in the table.

	for (i = 0; i < numBuckets; i++) {
		hPtr = buckets[i];
		while (hPtr != NULL) {
			nextPtr = hPtr->nextPtr;
			LOG_DEL; delete hPtr;
			hPtr = nextPtr;
		}
	}

	// Free up the bucket array, if it was dynamically allocated.

	if (buckets != staticBuckets) {
		LOG_DEL; delete [] buckets;
	}
	buckets = 0;		// for subsequent destroy calls.
}

// function to add all elements of the argument hashtable to this one.
// copyData does the appropriate manipulation of the clientData field
// (it may need to be duplicated).

// Since this function may be called from a copy constructor, call
// virtual functions (like copyData) on arg rather than on *this.

void HashTable::addAllElements(const HashTable& arg) {

	// it is more efficient to do any rebuilding before
	// rather than after adding the new elements: fewer elements
	// to re-hash.  We determine how many buckets we want first.

	int finalSize = size() + arg.size();
	if (finalSize >= rebuildSize) {
		int factor = REBUILD_INCREASE_FACTOR;
		while (rebuildSize*factor <= finalSize)
			factor <<= 1;
		rebuild(factor);
	}

	// insert all entries from arg into this table.

	HashEntry * hPtr;
	for (int i = 0; i < arg.numBuckets; i++) {
		hPtr = arg.buckets[i];
		while (hPtr) {
			insert(hPtr->stringkey,
			       arg.copyData(hPtr->clientData));
			hPtr = hPtr->nextPtr;
		}
	}
}

// assignment operator.  This is effectively a combination of
// the destructor and the copy constructor.
HashTable& HashTable::operator=(const HashTable& arg) {
	destroy();
	init();
	addAllElements(arg);
	return *this;
}

// default cleanup function: a no-op
void HashTable::cleanup(Pointer) {}

Pointer HashTable::copyData(Pointer arg) const { return arg;}

// * HashTable::stats --
// *
// *	Return statistics describing the layout of the hash table
// *	in its hash buckets.
// *
// * Results:
// *	The return value is a string containing information
// *	about tablePtr.  It is the caller's responsibility to free
// *	this string with delete [].
// *
// * Side effects:	None.

char *
HashTable::stats() const
{
#define NUM_COUNTERS 10
	int count[NUM_COUNTERS], overflow, i, j;
	double average, tmp;
	register HashEntry *hPtr;
	char *result, *p;

	// Compute a histogram of bucket usage.

	for (i = 0; i < NUM_COUNTERS; i++) {
		count[i] = 0;
	}
	overflow = 0;
	average = 0.0;
	for (i = 0; i < numBuckets; i++) {
		j = 0;
		for (hPtr = buckets[i]; hPtr != NULL; hPtr = hPtr->nextPtr) {
			j++;
		}
		if (j < NUM_COUNTERS) {
			count[j]++;
		} else {
			overflow++;
		}
		tmp = j;
		average += (tmp+1.0)*(tmp/numEntries)/2.0;
	}

	// Print out the histogram and a few other pieces of information.

	LOG_NEW; result = new char[(NUM_COUNTERS*60) + 300];
	sprintf(result, "%d entries in table, %d buckets\n",
		numEntries, numBuckets);
	p = result + strlen(result);
	for (i = 0; i < NUM_COUNTERS; i++) {
		sprintf(p, "number of buckets with %d entries: %d\n",
			i, count[i]);
		p += strlen(p);
	}
	sprintf(p, "number of buckets with %d or more entries: %d\n",
		NUM_COUNTERS, overflow);
	p += strlen(p);
	sprintf(p, "average search distance for entry: %.1f", average);
	return result;
}

// * HashString --
// *
// *	Compute a one-word summary of a text string, which can be
// *	used to generate a hash index.
// *
// * Results:
// *	The return value is a one-word summary of the information in
// *	string.
// *
// * Side effects:	None.

static unsigned int
HashString(const char* string)
{
	register unsigned int result;
	register int c;

	/*
	 * I tried a zillion different hash functions and asked many other
	 * people for advice.  Many people had their own favorite functions,
	 * all different, but no-one had much idea why they were good ones.
	 * I chose the one below (multiply by 9 and add new character)
	 * because of the following reasons:
	 *
	 * 1. Multiplying by 10 is perfect for keys that are decimal strings,
	 *    and multiplying by 9 is just about as good.
	 * 2. Times-9 is (shift-left-3) plus (old).  This means that each
	 *    character's bits hang around in the low-order bits of the
	 *    hash value for ever, plus they spread fairly rapidly up to
	 *    the high-order bits to fill out the hash value.  This seems
	 *    works well both for decimal and non-decimal strings.
	 */

	result = 0;
	while (1) {
		c = *string;
		string++;
		if (c == 0) {
			break;
		}
		result += (result<<3) + c;
	}
	return result;
}

// * HashTable::findEntry
// *
// *	Given a hash table with string keys, and a string key, find
// *	the entry with a matching key.
// *
// * Results:
// *	The return value is a token for the matching entry in the
// *	hash table, or NULL if there was no matching entry.
// *
// * Side effects:
// *	None.
// *
// * The main body of this function is now in HashTable::searchBucket,
// * as the code is common between findEntry and createEntry.

HashEntry *
HashTable::findEntry(const char* key) const
{
	int index = HashString(key) & (numBuckets-1);
	return searchBucket(key, index);
}

// * HashTable::searchBucket
// * This function searches a given hash bucket (the one numbered by
// * index in hashtable "this") for the given key.
// * Results:
// *	The return value is a token for the matching entry in the
// *	hash table, or NULL if there was no matching entry.
// *
// * Side effects:
// *	None.

HashEntry *
HashTable::searchBucket(const char* key, int index) const
{
	register HashEntry *hPtr;
	register const char *p1, *p2;
	/*
	 * Search all of the entries in the appropriate bucket.
	 */

	for (hPtr = buckets[index]; hPtr != NULL;
	     hPtr = hPtr->nextPtr) {
		for (p1 = key, p2 = hPtr->stringkey; ; p1++, p2++) {
			if (*p1 != *p2) {
				break;
			}
			if (*p1 == '\0') {
				return hPtr;
			}
		}
	}
	return NULL;
}

// * HashTable::createEntry
// *
// *	Given a hash table with string keys, and a string key, find
// *	the entry with a matching key.  If there is no matching entry,
// *	then create a new entry that does match.
// *
// * Results:
// *	The return value is a pointer to the matching entry.  If this
// *	is a new entry the value stored in the entry will initially be 0.
// *
// * Side effects:
// *	A new entry may be added to the hash table.

HashEntry *
HashTable::createEntry(const char* key)
{
	register HashEntry *hPtr;
	int index;

	index = HashString(key) & (numBuckets-1);

	// look for existing entry
	hPtr = searchBucket(key, index);
	if (hPtr) {
		return hPtr;
	}

	// Entry not found.  Add a new one to the bucket.

	LOG_NEW; hPtr = new HashEntry(this,&(buckets[index]),key);
	numEntries++;

	// If the table has exceeded a decent size, rebuild it with many
	// more buckets.

	if (numEntries >= rebuildSize) {
		rebuild(REBUILD_INCREASE_FACTOR);
	}
	return hPtr;
}


// * HashTable::rebuild
// *
// *	This procedure is invoked when the ratio of entries to hash
// *	buckets becomes too large.  It creates a new table with a
// *	larger bucket array and moves all of the entries into the
// *	new table.
// *
// * Results:
// *	None.
// *
// * Side effects:
// *	Memory gets reallocated and entries get re-hashed to new
// *	buckets.

void HashTable::rebuild(int factor)
{
	int oldSize, count, index;
	HashEntry **oldBuckets;
	register HashEntry **oldChainPtr, **newChainPtr;
	register HashEntry *hPtr;

	oldSize = numBuckets;
	oldBuckets = buckets;

	// Allocate and initialize the new bucket array, and set up
	// hashing constants for new array size.

	numBuckets *= factor;
	LOG_NEW; buckets = new HashEntry* [numBuckets];
	for (count = numBuckets, newChainPtr = buckets;
	     count > 0; count--, newChainPtr++) {
		*newChainPtr = NULL;
	}
	rebuildSize *= factor;

	// This case happens when using a copy constructor or
	// assignment operator to copy a big table.  If there
	// are 0 entries, oldBuckets == staticBuckets and
	// there is nothing to rehash.

	if (numEntries == 0) return;

	int mask = numBuckets - 1;
	// Rehash all of the existing entries into the new bucket array.

	for (oldChainPtr = oldBuckets; oldSize > 0; oldSize--, oldChainPtr++) {
		for (hPtr = *oldChainPtr; hPtr != NULL; hPtr = *oldChainPtr) {
			*oldChainPtr = hPtr->nextPtr;
			index = HashString(hPtr->stringkey) & mask;
			hPtr->bucketPtr = &(buckets[index]);
			hPtr->nextPtr = *hPtr->bucketPtr;
			*hPtr->bucketPtr = hPtr;
		}
	}

	// Free up the old bucket array, if it was dynamically allocated.

	if (oldBuckets != staticBuckets) {
		LOG_DEL; delete [] oldBuckets;
	}
}

// constructor for HashEntry: allocate memory for the key
HashEntry::HashEntry(HashTable* t,HashEntry** b,const char* k) {
	tablePtr = t;
	bucketPtr = b;
	nextPtr = *b;
	clientData = 0;
	stringkey = savestring(k);
	*bucketPtr = this;
}

// destructor for HashEntry: deallocates the key.  Important:
// nothing is done about any dangling pointers, we assume these
// are handled correctly by the HashTable class.
HashEntry::~HashEntry() {
	if (tablePtr) tablePtr->cleanup(clientData);
	LOG_DEL; delete [] stringkey;
}

// replace value of a HashEntry.  Uses table cleanup fn.
// side effects: old clientData will be processed by the cleanup function
// (possibly deleted).
HashEntry& HashEntry::setData(Pointer newData) {
	tablePtr->cleanup(clientData);
	clientData = newData;
	return *this;
}

// constructor for hash table iterator.

HashTableIter::HashTableIter(HashTable& h) {
	tablePtr = &h;
	nextIndex = 0;
	nextEntryPtr = 0;
}

// return the next hash entry in the hash table that is attached to
// the iterator.

HashEntry* HashTableIter::next() {
	HashEntry *hPtr;

	while (nextEntryPtr == NULL) {
		if (nextIndex >= tablePtr->numBuckets) {
			return NULL;
		}
		nextEntryPtr = tablePtr->buckets[nextIndex];
		nextIndex++;
	}
	hPtr = nextEntryPtr;
	nextEntryPtr = hPtr->nextPtr;
	return hPtr;
}

// TextTable functions (also TextEntry)

// setData: copy new value to heap before storing.
TextEntry& TextEntry::setData(const char* newVal) {
	HashEntry::setData(savestring(newVal));
	return *this;
}

// insert into TextTable: save data on heap, then insert
void TextTable::insert(const char* key, const char* data)
{
	char* s = data ? savestring(data) : 0;
	HashTable::insert(key,s);
}

// TextTable cleanup function.  Entry is really an array of characters.
void TextTable::cleanup(Pointer data) {
	LOG_DEL; delete [] (char*)data;
}

// TextTable copyData function.  Make a new copy of the clientData.
Pointer TextTable::copyData(Pointer arg) const {
	return savestring((char*)arg);
}
