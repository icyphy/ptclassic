/* $Id$ */

#include "sfg.h"
#include "dag.h"
#include "stringswitch.h"

char *Intern();
ListPointer new_list(), ListAppend();
ListPointer SetAttribute();
pointer GetAttribute();

#define MIN(A,B) ((A) < (B) ? (A) : (B))
#define MAX(A,B) ((A) > (B) ? (A) : (B))

DoBinManifest(Op, l, r, out)
char *Op;
EdgePointer l, r, out;
{
    int lb1, ub1, lb2, ub2;
    int lbout, ubout;
    char *range, *MakeRange();

    if (!HasAttribute(l->Arguments, "manifest") ||
	!HasAttribute(r->Arguments, "manifest")) return;

/* Both input edges have manifest values, extract them */
    ExtractRange((char *)GetAttribute(l->Arguments, "manifest"),&lb1, &ub1);
    ExtractRange((char *)GetAttribute(r->Arguments, "manifest"),&lb2, &ub2);

/* perform operation */
    STRINGSWITCH(Op)
	FIRSTCASE(PLUS) { lbout = lb1+lb2; ubout = ub1+ub2;}   
	CASE(MINUS)     { lbout = lb1-lb2; ubout = ub1-ub2;}
	CASE(MULT)	{ lbout = lb1*lb2; ubout = ub1*ub2;}
	CASE(DIV)	{ lbout = lb1/lb2; ubout = ub1/ub2;}
	CASE(_MIN)	{ lbout = MIN(lb1,lb2); ubout = MIN(ub1,ub2);}
	CASE(_MAX)	{ lbout = MAX(lb1,lb2); ubout = MAX(ub1,ub2);}
    ENDSWITCH

/* propagate result to output edge */
    range = MakeRange(lbout, ubout);
    out->Arguments = SetAttribute("manifest",CharNode,range,out->Arguments);
    AddManifestEdge(out);

}

DoUniManifest(op, l, out)
char *op;
EdgePointer l, out;
{
    int lb1, ub1;
    char *range, *MakeRange();

    if (!HasAttribute(l->Arguments, "manifest")) return;

    ExtractRange((char *)GetAttribute(l->Arguments, "manifest"),&lb1, &ub1);
    if (strcmp(op, NEG) == 0)
        range = MakeRange(-ub1, -lb1);
    else
        range = MakeRange(lb1, ub1);
    out->Arguments = SetAttribute("manifest",CharNode,range,out->Arguments);
    AddManifestEdge(out);
}

static ListPointer ManifestEdge[MAXINDEXLEVEL];
extern int indexlevel;

/**************************************************************************
  InitManifestEdge() -- Initialize ManifestEdge[] at indexlevel 
****************************************************************************/
InitManifestEdge()
{
    ManifestEdge[indexlevel] = NULL;
}

/**************************************************************************
  ClearManifestEdge() -- Clear ManifestEdge[] at indexlevel 
****************************************************************************/
ClearManifestEdge()
{
    ClearList(ManifestEdge[indexlevel]);
}

/**************************************************************************
  AddManifestEdge() -- Add manifest edge "e" to ManifestEdge[] at indexlevel 
****************************************************************************/
AddManifestEdge(e)
EdgePointer e;
{
    ManifestEdge[indexlevel] = ListAppend(new_list(EdgeNode, e),
                                                ManifestEdge[indexlevel]);
}

/**************************************************************************
  DeriveManifestRange() -- Find manifest range of edge from ManifestEdge.
****************************************************************************/
DeriveManifestRange(e)
EdgePointer e;
{
    char *range, *GetType();
    int i;
    ListPointer ptr;

    if (strcmp(GetType(e), "int") != 0) return;
    for (i = indexlevel; i >= 0; i--) {
	for (ptr = ManifestEdge[i]; ptr != NULL; ptr = ptr->Next) {
	    if (strcmp(e->Name, EP(ptr)->Name) == 0) {
		range = (char *)GetAttribute(EP(ptr)->Arguments, "manifest");
		e->Arguments = SetAttribute("manifest", CharNode, range,
							e->Arguments);
		return;
	    }
	}
    }
}
