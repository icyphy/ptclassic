static const char file_id[] = "DERCStar.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "DERCStar.h"

const char *star_nm_DERCStar = "DERCStar";
const char* DERCStar :: className() const {return star_nm_DERCStar;}
ISA_FUNC(DERCStar,DERepeatStar);
Block* DERCStar :: makeNew() const { LOG_NEW; return new DERCStar;}

FILE * DERCStar::fpoverflow;
FILE * DERCStar::fpfiring;

DERCStar::DERCStar ()
{
	setDescriptor("Common Polis(resource contention) Software Scheduler");
        isRCStar = 1;  // override to true (false in DEStar)
}


double DERCStar::getDelay ()
{
	return 0;
}


SequentialList* DERCStar::getEvents ()
{
	return emittedEvents;
}


FILE* DERCStar::Openoverflow ( char *name )
{
	if ( DERCStar::fpoverflow == NULL ) {
        	DERCStar::fpoverflow = fopen( name, "w" );
        	if ( DERCStar::fpoverflow ) {
          		fprintf( DERCStar::fpoverflow, "Overflow report\n\n" );
          	fflush( DERCStar::fpoverflow );
        	}
	}
	return( fpoverflow );
}


void DERCStar::Printoverflow ( char *st )
{
	if ( DERCStar::fpoverflow ) {
        	fprintf( DERCStar::fpoverflow, st );
        	fflush( DERCStar::fpoverflow );
	}
}


FILE * DERCStar::Openfiring ( char *name )
{
	if ( DERCStar::fpfiring == NULL ) {
        	DERCStar::fpfiring = fopen( name, "w" );
        	if ( DERCStar::fpfiring ) {
          		fprintf( DERCStar::fpfiring, "Firing report\n\n" );
          		fflush( DERCStar::fpfiring );
        	}
	}
	return( fpfiring );
}


void DERCStar::Printfiring ( char *st )
{
	if ( DERCStar::fpfiring ) {
        	fprintf( DERCStar::fpfiring, st );
        	fflush( DERCStar::fpfiring );
	}
}


void DERCStar::Closeflow ()
{
	if ( DERCStar::fpoverflow ) {
        	fclose( DERCStar::fpoverflow );
        	DERCStar::fpoverflow = NULL;
	}
	if ( DERCStar::fpfiring ) {
        	fclose( DERCStar::fpfiring );
        	DERCStar::fpfiring = NULL;
	}
}


static DERCStar proto;
static RegisterBlock registerBlock(proto,"RCStar");
