/**
    tkh.h :: Tk Helper

**/

typedef struct _TKHFontList TKHFontList;
struct _TKHFontList {
    char		*specs;
    int			refcnt;
    TOPPtrVector	infos;		/* of XFontStruct* */
};

extern TKHFontList* tkhGetFontList(Tcl_Interp *ip, char *specs);
extern void	 tkhFreeFontList( TKHFontList *pFL);
extern XFontStruct* tkhGetFontByHeight( TKHFontList *pFL, int height);

