static const char file_id[] = "DEPolis.pl";

#ifdef __GNUG__
#pragma implementation
#endif

#include "DEPolis.h"

const char *star_nm_DEPolis = "DEPolis";
const char* DEPolis :: className() const {return star_nm_DEPolis;}
ISA_FUNC(DEPolis,DERepeatStar);
Block* DEPolis :: makeNew() const { LOG_NEW; return new DEPolis;}

FILE * DEPolis::fpoverflow;
FILE * DEPolis::fpfiring;

DEPolis::DEPolis ()
{
	setDescriptor("Common Polis Software Scheduler");
        isRCStar = 1;  // override to true (false in DEStar)
}


double DEPolis::getDelay ()
{
	return 0;
}


SequentialList* DEPolis::getEvents ()
{
	return emittedEvents;
}


FILE* DEPolis::Openoverflow ( char *name )
{
	if ( DEPolis::fpoverflow == NULL ) {
        	DEPolis::fpoverflow = fopen( name, "w" );
        	if ( DEPolis::fpoverflow ) {
          		fprintf( DEPolis::fpoverflow, "Overflow report\n\n" );
          	fflush( DEPolis::fpoverflow );
        	}
	}
	return( fpoverflow );
}


void DEPolis::Printoverflow ( char *st )
{
	if ( DEPolis::fpoverflow ) {
        	fprintf( DEPolis::fpoverflow, st );
        	fflush( DEPolis::fpoverflow );
	}
}


FILE * DEPolis::Openfiring ( char *name )
{
	if ( DEPolis::fpfiring == NULL ) {
        	DEPolis::fpfiring = fopen( name, "w" );
        	if ( DEPolis::fpfiring ) {
          		fprintf( DEPolis::fpfiring, "Firing report\n\n" );
          		fflush( DEPolis::fpfiring );
        	}
	}
	return( fpfiring );
}


void DEPolis::Printfiring ( char *st )
{
	if ( DEPolis::fpfiring ) {
        	fprintf( DEPolis::fpfiring, st );
        	fflush( DEPolis::fpfiring );
	}
}


void DEPolis::Closeflow ()
{
	if ( DEPolis::fpoverflow ) {
        	fclose( DEPolis::fpoverflow );
        	DEPolis::fpoverflow = NULL;
	}
	if ( DEPolis::fpfiring ) {
        	fclose( DEPolis::fpfiring );
        	DEPolis::fpfiring = NULL;
	}
}


static DEPolis proto;
static RegisterBlock registerBlock(proto,"Polis");
