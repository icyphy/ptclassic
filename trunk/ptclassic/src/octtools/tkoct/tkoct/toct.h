/**
    toct.h
**/

#define TOCT_DisjointB(pBoxA,pBoxB)			\
  (  (pBoxA)->lowerLeft.x > (pBoxB)->upperRight.x	\
  || (pBoxA)->upperRight.x < (pBoxB)->lowerLeft.x	\
  || (pBoxA)->lowerLeft.y > (pBoxB)->upperRight.y	\
  || (pBoxA)->upperRight.y < (pBoxB)->lowerLeft.y )

#define TOCT_SetMin(src,dst) { if ( (src) < (dst) )	(dst) = (src); }
#define TOCT_SetMax(src,dst) { if ( (src) > (dst) )	(dst) = (src); }

#define TOCT_SwapLoFirst(type,a,b) {		\
    if ( (a) > (b) )	TOP_SWAPTYPE(type,a,b);	\
}

#define TOCT_OctIdResult(ip,pObj) {				\
    char *__toct_strptr__;					\
    Tcl_ResetResult((ip));					\
    __toct_strptr__ = (ip)->result;				\
    *__toct_strptr__++ = '%';					\
    _toctCvtOctId( &(pObj)->objectId, &__toct_strptr__);	\
}

/**
    These are iteration macros built on top of octGenerate().
    They are used very much like while() loops.
**/
#define TOCT_LOOP_CONTENTS_BEGIN(pContainer,mask,contentsObj) {		\
    octObject		contentsObj;					\
    octGenerator	__toct_generator__;				\
    OH_FAIL(octInitGenContents( (pContainer), (mask), &__toct_generator__), \
      "Cannot generate contents (begin)", (pContainer));		\
    while ( octGenerate(&__toct_generator__,&contentsObj) == OCT_OK ) {	\
	/* APPLICATION CODE GOES HERE */

#define TOCT_LOOP_CONTENTS_END() }					\
    OH_ASSERT_DESCR(octFreeGenerator( &__toct_generator__),		\
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


/*
 * toctFacet.c
 */
extern int	toctGetFacetByName( char *name, octObject *pObj);
extern int	toctGetMasterByInst( octObject *pInst, char *facet,
		  octObject *pMaster);

/*
 * toct.c
 */
extern void toctBoxUnion2( octBox *pSrc, octBox *pDst);

extern int	_toctCvtStrId( char **idstr, octId *pId);
extern int	_toctCvtOctId( octId *pId, char **idstr);

extern int	toctGetByTag( Tcl_Interp *ip, char *tag, octObject *pObj);
extern int	toctGetByTagM( Tcl_Interp *ip, char *tag, octObject *pObj,
		  TOPMask typeMask);

extern int	toctInit( Tcl_Interp *ip);
extern int	toctGetBbByTag( Tcl_Interp *ip, char *tag, octBox *pBox);
