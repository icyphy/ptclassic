/**
    oct2ptcl.h :: oct2ptcl translator

    Author: Kennard White
    RCS: $Header$
**/


/**
    This is a character string that contains the name of this package.
    It is used for error reporting.
**/
extern char _OtpPackage[];

/**
    Some static buffers are used to store temporary strings.  This
    is the length of such buffers.
**/
#define OTP_NAMELEN	1024


/**
    This describes an entire design (e.g., a universe and all of
    its children).  It is the toplevel object in oct2ptcl.  The
    hash table {cellTbl} contains all facets (universes, galaxies & stars)
    used by the design.  The flag {recurB} indicates that a
    recursive translation is desired (instead of just the top-level
    universe).
**/
typedef struct OTPDesignInfo OTPDesignInfo;
struct OTPDesignInfo {
    int			flags;
    Tcl_HashTable	cellTbl;	/* all cells (facets) of the design */
};

/*
 * these flags are bit-ORd into OTPDesignInfo.flags
 */
#define OTP_DsgnF_Recur		TOP_BIT(0)	/* tranlate recursivly */
#define OTP_DsgnF_Go		TOP_BIT(1)	/* issue run params */
#define OTP_DsgnF_Verbose	TOP_BIT(2)	/* tell user whats going on */
#define OTP_DsgnF_NoTcltk	TOP_BIT(3)	/* skip tcltk universes */


/**
    A facet may be one of 3 mutually exclusive types, plus the psuedo-type
    target.
**/
enum _OTPFacetType {
    OTP_FtNull,
    OTP_FtStar,		/* star (leaf node) */
    OTP_FtGalaxy,	/* galaxy, has I/O ports and contains stars/gals */
    OTP_FtUniverse,	/* top-level, no I/O ports */
    OTP_FtPalette,	/* a palette of universes */
    OTP_FtTarget,	/* not really a facet type, but useful... */
    OTP_FtMarker,	/* special star (bus & delay) */
    OTP_FtContact,	/* net connector (contact) */
};
typedef enum _OTPFacetType OTPFacetType;


/**
    A given facet may be in one of several states as translation of the
    design proceeds.  Note that this usage of the "state" is very different
    than "galaxy state".
**/
enum _OTPFacetState {
    OTP_FcsNull,
    OTP_FcsInit,	/* facet is referenced but hasnt been xlated yet */
    OTP_FcsError,	/* facet couldnt be xlated */
    OTP_FcsBegin,	/* xlation of facet has begun */
    OTP_FcsDone,	/* facet has been sucessfully xlated */
};
typedef enum _OTPFacetState OTPFacetState;

/**
    All relevant information about the facet.  This structure is dynmically
    allocated when the facet is first referenced as an instance's master.
    Info is added as the facet is xlated.
**/
typedef struct OTPFacetInfo OTPFacetInfo;
struct OTPFacetInfo {
    OTPDesignInfo	*designInfo;
    OTPFacetType	type;
    TOPLogical		tcltkB;
    OTPFacetState	state;
    char		*facetName;
    char		*starName;
    char		*nameBindings;
    char		*outerDomain;
    char		*curDomain;
    int			sn;
    Tcl_HashTable	instTbl;
    octId		interfaceId;	/* contents facet */
    octId		contentsId;	/* contents facet */
};

extern octStatus otpXlateFacet( OTPFacetInfo *facetInfo, Tcl_DString *pStr);

extern char* otpCvtPropToStr( octObject *pProp);
extern octStatus otpXlateParams( octObject *pInst, char *instName,
			  OTPFacetType which, Tcl_DString *pStr);
extern OTPFacetInfo*otpGetMasterInfo( OTPFacetInfo *pFInfo, octObject *pInst);
extern OTPFacetInfo* otpGetFacetInfo( OTPDesignInfo *pDCxt, octObject *pFacet);
extern octStatus otpXlateInsts( OTPFacetInfo *pFInfo, octObject *pFacet,
  Tcl_DString *pStr);

#define OTP_PifIn	TOP_BIT(0)
#define OTP_PifOut	TOP_BIT(1)
#define OTP_PifFormal	TOP_BIT(2)

#define OTP_MAXPORTS	50

typedef struct _OTPPortInfo OTPPortInfo;
struct _OTPPortInfo {
    char*	instName;
    char*	portName;
    TOPMask	flags;
};

typedef struct _OTPNetInfo OTPNetInfo;
struct _OTPNetInfo {
    OTPFacetInfo	*facetInfo;	/* the facet we are in */
    char*		netName;
    char*		delayExpr;
    char*		widthExpr;
    int			numFormal;
    Tcl_HashTable	portTbl;	/* key'd by actual terminal Id */
    int			numIn;
    OTPPortInfo		inPorts[OTP_MAXPORTS];
    int			numOut;
    OTPPortInfo		outPorts[OTP_MAXPORTS];
};


extern octStatus otpXlateNets( OTPFacetInfo *pCxt, octObject *pFacet,
		Tcl_DString *pStr);

extern octStatus otpXlateFacet( OTPFacetInfo *facetInfo, Tcl_DString *pStr);
extern octStatus otpXlateDesign( octObject *pFacet, int flags, 
  Tcl_DString *pStr);
extern octStatus otpXlateDesignByName( char *name, int flags);

/*****************************************************************************
 *
 *			Misc. stuff (from misc.c)
 *
 * This stuff is not really part of this package: they are helper functions.
 *
 *****************************************************************************/

extern char* Tcl_DStringAppends( Tcl_DString *pStr, ...);
extern char* Tcl_DStringAppendEls( Tcl_DString *pStr, ...);
extern char* otpCvtPropToStr( octObject *pProp);

/**
    These are iteration macros built on top of octGenerate().
    They are used very much like while() loops.
**/
#define POH_LOOP_CONTENTS_BEGIN(pContainer,mask,contentsObj) {		\
    octObject		contentsObj;					\
    octGenerator	__poh_generator__;				\
    OH_FAIL(octInitGenContents( (pContainer), (mask), &__poh_generator__), \
      "Cannot generate contents (begin)", (pContainer));		\
    while ( octGenerate(&__poh_generator__,&contentsObj) == OCT_OK ) {	\
	/* APPLICATION CODE GOES HERE */

#define POH_LOOP_CONTENTS_END() }					\
    OH_ASSERT_DESCR(octFreeGenerator( &__poh_generator__),		\
      "Connot generate contents (end)");				\
}   /* END OF LOOP */

/* this is modeled after ohOpenInterface() */
#define ohOpenContents(t, f, m)						\
    ((t)->type = OCT_FACET,						\
    (t)->contents.facet.cell = (f)->contents.facet.cell,		\
    (t)->contents.facet.view = (f)->contents.facet.view,		\
    (t)->contents.facet.facet = "contents",				\
    (t)->contents.facet.version = OCT_CURRENT_VERSION,			\
    (t)->contents.facet.mode = m,					\
    octOpenFacet(t))
