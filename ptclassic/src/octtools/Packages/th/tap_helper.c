#ifndef lint
static char SccsId[]="$Id$";
#endif /*lint*/
/*
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
*/
#include "port.h"
#include "utility.h"
#include "oct.h"
#include "errtrap.h"
#include "tap.h"
#include "oh.h"
#include "th.h"

static char*	defTechName = "scmos";
static octObject *dummyFacet = 0;
static int	verbose = 0;

static void	thInit ARGS((octObject *obj));

void thVerbose( f )
    int f;
{
    verbose = f;
}

void thTechnology( obj )
    octObject *obj;
{
    thInit( 0 );
    if ( obj ) {
	octGetFacet( obj, dummyFacet );
    } else {
	octObject prop;
	OH_ASSERT(ohOpenFacet( dummyFacet, "th.dummy","symbolic","contents","w"));
	ohCreatePropStr( dummyFacet, &prop, "TECHNOLOGY", defTechName );
	ohCreatePropStr( dummyFacet, &prop, "VIEWTYPE", "SYMBOLIC" );
    }
}


static void thInit( obj )
    octObject *obj;
{
    if ( !dummyFacet ) {
	dummyFacet = ALLOC( octObject, 1 );
	dummyFacet->objectId = oct_null_id;
    }

    if ( obj ) thTechnology( obj );
}

    


static void thGetProp( prop , name, type )
    struct octProp* prop;
    char* name;
    octPropType type;
{
    thInit(0);
    
    prop->name = name;
    prop->type = type;

    if ( tapGetProp( dummyFacet, prop ) == 0 ) {
	errRaise(TH_PKG_NAME,1,"%s not found" , name );
    } 

    if ( prop->type != type ) {
	errRaise( TH_PKG_NAME, 1, "wrong type for prop %s", name );
    }
}

int thGetPropInteger( name )
    char* name;
{
    struct octProp prop;
    thGetProp( &prop, name, OCT_INTEGER );
    return prop.value.integer;
}

double thGetPropReal( name )
    char* name;
{
    struct octProp prop;
    thGetProp( &prop, name, OCT_REAL );
    return prop.value.real;
}


char *thGetPropString( name )
    char* name;
{
    struct octProp prop;
    thGetProp( &prop, name, OCT_STRING );
    return prop.value.string;
}



double thGetLayerLayerPropReal( layer1, layer2, name )
    octObject *layer1, *layer2;
    char* name;
{
    tapLayerListElement layerList[2];
    struct octProp prop;

    if ( layer1->type != OCT_LAYER ) {
	errRaise( "TH", 1, "first object not a layer" );
    }
    if ( layer2->type != OCT_LAYER ) {
	errRaise( "TH", 1, "second object not a layer" );
    }
    thInit( layer1 );
    layerList[0].layer = *layer1;
    layerList[1].layer = *layer2;

    prop.name = name;
    prop.type = OCT_REAL;

    if ( tapGetLyrProp( 2, layerList, &prop ) == 0 ) {
	errRaise(TH_PKG_NAME,1,"%s-%s %s not found",
		 ohFormatName( layer1 ), ohFormatName( layer2 ), name );
    } 
    if ( prop.type != OCT_REAL ) {
	errRaise( TH_PKG_NAME, 1, "wrong type for prop %s of layer %s",
		 name, layer1->contents.layer.name );
    }
    if ( verbose ) printf( "%s %s %g\n", ohFormatName(layer1), name, prop.value.real );
    return prop.value.real;
}

int  thGetLayerLayerPropInt( layer1, layer2, name )
    octObject *layer1, *layer2;
    char* name;
{
    tapLayerListElement layerList[2];
    struct octProp prop;

    if ( layer1->type != OCT_LAYER ) {
	errRaise( "TH", 1, "first object not a layer" );
    }
    if ( layer2->type != OCT_LAYER ) {
	errRaise( "TH", 1, "second object not a layer" );
    }
    thInit( layer1 );
    layerList[0].layer = *layer1;
    layerList[1].layer = *layer2;

    prop.name = name;
    prop.type = OCT_INTEGER;

    if ( tapGetLyrProp( 2, layerList, &prop ) == 0 ) {
	errRaise(TH_PKG_NAME,1,"%s-%s %s not found",
		 ohFormatName( layer1 ), ohFormatName( layer2 ), name );
    } 
    if ( prop.type != OCT_INTEGER ) {
	errRaise( TH_PKG_NAME, 1, "wrong type for prop %s of layer %s",
		 name, layer1->contents.layer.name );
    }
    if ( verbose )
      printf( "%s %s %g\n", ohFormatName(layer1), name,
	     (double)prop.value.integer );
    return prop.value.integer;
}

double thGetLayerPropReal( layer, name )
    octObject *layer;
    char* name;
{
    tapLayerListElement layerList[1];
    struct octProp prop;

    if ( layer->type != OCT_LAYER ) errRaise( "TH", 1, "not a layer" );
    thInit( layer );
    layerList[0].layer = *layer;

    prop.name = name;
    prop.type = OCT_REAL;

    if ( tapGetLyrProp( 1, layerList, &prop ) == 0 ) {
	errRaise(TH_PKG_NAME,1,"%s %s not found",
		 ohFormatName( layer ), name );
    } 
    if ( prop.type != OCT_REAL ) {
	errRaise( TH_PKG_NAME, 1, "wrong type for prop %s of layer %s",
		 name, layer->contents.layer.name );
    }
    if ( verbose ) printf( "%s %s %g\n", ohFormatName(layer), name, prop.value.real );
    return prop.value.real;
}


double thGetAreaCap( layer )
    octObject *layer;
{
    return thGetLayerPropReal( layer, "AREA-CAP-TO-GROUND" );
}



double thGetPerimCap( layer )
    octObject *layer;
{
    return thGetLayerPropReal( layer, "PERIM-CAP-TO-GROUND" );
}

double thGetSheetResistance( layer )
    octObject *layer;
{
    double r = -1.0;
    ERR_IGNORE(r = thGetLayerPropReal( layer, "SHEET-RESISTANCE" ));
    return r;
}

double thGetSheetInductance( layer )
    octObject *layer;
{
    double r = -1.0;
    ERR_IGNORE(r = thGetLayerPropReal( layer, "SHEET-INDUCTANCE" ));
    return r;
}

double thGetContactResistance( layer )
    octObject *layer;
{
    return thGetLayerPropReal( layer, "CONTACT-RESISTANCE" );
}

double thGetThickness( layer )
    octObject *layer;
{
    return thGetLayerPropReal( layer, "THICKNESS" );
}

char *thGetLayerClass( layer )
    octObject *layer;
{
    tapLayerListElement layerList[1];

    if ( layer->type != OCT_LAYER ) errRaise( "TH", 1, "not a layer" );
    thInit( layer );
    layerList[0].layer = *layer;
    if ( tapGetLyrProp( 1, layerList, TAP_LAYER_CLASS ) == 0 ) {
	printf( "%s layer class not found\n", ohFormatName( layer ) );
	return "OTHER";
    } 
    if ( verbose ) printf( "%s layer class %s\n", ohFormatName(layer ),
			  TAP_LAYER_CLASS->value.string );
    return TAP_LAYER_CLASS->value.string;
}


static char* nextWord( fp )
    FILE *fp;
{
    static char buffer[1024];
    
    while (1) {
	if ( fscanf( fp, "%s", buffer ) == EOF ) return 0;
	if ( buffer[0] == '#' ) { /* Found a comment */
	    while ( getc( fp ) != '\n' ); /* Go to the end of the line. */
	} else {
	    return buffer;
	}
    }
}

#define MAX_LAYERS 8


void thReadFile( filename )
    char* filename;
{
    FILE *fp;
    int  num_layers, i;
    char *word;
    char prop_name[128];
    char prop_type_code[64];
    char value[128];
    char layer_names[MAX_LAYERS][16];
    char last_line[1024];
    struct octProp prop;

    strcpy( last_line, "Beginning of file" );
    thInit(0);
    fp = fopen( filename, "r");
    if (fp == 0 ) errRaise( "TH", 1 , "file %s not found", filename );

    while  (1 ) {
	word = nextWord( fp );
	if ( word == 0 ) break;
	num_layers = atoi( word );

	if ( num_layers < 0 || num_layers > MAX_LAYERS ) {
	    errRaise("TH", 1, "illegal number of layers %d after %s", num_layers, last_line );
	}
	if ( num_layers ) {
	    for ( i = 0; i < num_layers; i++ ) {
		fscanf( fp, "%s", layer_names[i] );
	    }
	}
	if ( fscanf( fp, "%s %s %s", prop_name, prop_type_code, value ) != 3 ) {
	    errRaise ( "TH", 1, "Error parsing tech file after %s", last_line );
	}

	/* Save info to give meaningful error reporting. */
	sprintf( last_line, "%d ... %s %s %s", num_layers, prop_name, prop_type_code, value ); 
	
	prop.name = prop_name;
	switch(  prop_type_code[0] ) {
	case 'R':
	    prop.type = OCT_REAL;
	    prop.value.real = atof( value );
	    break;
	case 'I':
	    prop.type = OCT_INTEGER;
	    prop.value.integer = atoi( value );
	    break;
	case 'S':
	    prop.type = OCT_STRING;
	    prop.value.string = value; 
	    break;
	default:
	    errRaise( "TH", 1, "illegal prop type %s\nafter %s", prop_type_code, last_line );
	}

	if ( verbose ) printf( "TH: %d ", num_layers );
	if ( num_layers ) {
	    tapLayerListElement *layer_list = ALLOC( tapLayerListElement, num_layers);
	    for ( i = 0 ; i < num_layers; i++ ) {
		layer_list[i].layer.type = OCT_LAYER;
		layer_list[i].layer.contents.layer.name = layer_names[i];
		octGetOrCreate( dummyFacet, &layer_list[i].layer );
		if ( verbose ) printf( "%s ", layer_names[i] );
	    }
	    tapSetLyrProp( num_layers, layer_list, &prop );
	    FREE( layer_list );
	} else {
	    tapSetProp( dummyFacet, &prop );
	}
	if ( verbose ) {
	    octObject p;
	    p.type = OCT_PROP;
	    p.contents.prop = prop;
	    printf( "%s\n", ohFormatName( &p ) );
	}
    }

}

