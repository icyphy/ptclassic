/* $Id$ */

#include "sfg.h"
#include "dag.h"

pointer GetAttribute(), CheckFromHash();
ListPointer SetAttribute(), new_list(), ListAppend();
ListPointer ConstructExp(), SaveSilage();
NodePointer NodeListAppend();
EdgePointer EdgeListAppend();

extern int indexlevel;
extern int maxindexlevel;

static EntryPointer Edgetable[MAXINDEXLEVEL][TBLSIZE];
extern int indexlevel;
extern int maxindexlevel;

/**************************************************************************
  InitEdgetable() -- Initialize Edgetable[] at indexlevel
****************************************************************************/
InitEdgetable()
{
    InitializeHash(Edgetable[indexlevel]);
}

/**************************************************************************
  AddToEdgetable() -- Add edge "e" to Edgetable[] at indexlevel
****************************************************************************/
AddToEdgetable(e)
EdgePointer e;
{
    AddToHash(e->Name, (pointer)e, Edgetable[indexlevel]);
}

/**************************************************************************
  CheckEdgetable() -- Check if there is edge with "name" in Edgetable[] at 
  current indexlevel
****************************************************************************/
EdgePointer
CheckEdgetable(name)
char *name;
{
    return((EdgePointer) CheckFromHash(name, Edgetable[indexlevel]));
}

/**************************************************************************
  LabelDelayControlEdge(e) -- Assumes e is output of READ node.  Locate
  input control edge of READ node, and attach "delayedge" attributes.
****************************************************************************/
LabelDelayControlEdge(e)
EdgePointer e;
{
    NodePointer node;
    EdgePointer inedge;

    if (e->InNodes == NULL) {
        printmsg("LabelDelayControlEdge:","edge %s has no input nodes\n",
						e->Name);
	PrintSilage(e->Attributes);
    }
    node = NP(e->InNodes);
    if (strcmp(node->Master->Name, READ)) {
        printmsg("LabelDelayControlEdge:","node %s not a READ node\n",
						node->Name);
	PrintSilage(e->Attributes);
    }
    inedge = EP(node->InControl);
    inedge->Attributes = SetAttribute("delayedge", IntNode, (char *)1,
                                                        inedge->Attributes);
}

/**************************************************************************
  CheckExistingEdgeToEdge() -- used by AddIOEdges to build in/out control 
  edges of iteration nodes.  For input control edges, this function should
  return all edges in Edgetable whose name matches string s, and whose 
  indexvalue is inside the range of the subgraph edge (usually node:min and 
  node:max).  For output control edges, it does the same, except that there
  should not be any existing edge in this range.  Similar to CheckExisting-
  EdgeToNode except we compare to edge.
****************************************************************************/
ListPointer CheckExistingEdgeToEdge(SubGraphEdge, level)
    EdgePointer SubGraphEdge;
    int *level;
{
    char *s;
    EntryPointer ptr;
    ListPointer entry, list = NULL;
    int i, found = FALSE;

    s = SubGraphEdge->Name;
    for (i = indexlevel; i>= 0; i--) {
        for (ptr = Edgetable[i][hashfunc(s)]; ptr != NULL; ptr = ptr->Next) {
	    if ((strcmp(s, ptr->Name) == 0) && OverlapRangeToEdge(SubGraphEdge,
								EP(ptr))) {
	        entry = new_list(EdgeNode, ptr->Entry);
	        list = ListAppend(entry, list);
                found = TRUE;
	    }
	}
        if (found) break;
    }
    *level = i;
    return(list);
}

/**************************************************************************
  CheckExistingEdgeToNode() -- Checks for existing vectors falling in the 
  range of indices given by node.  If WRITE, the index edges or parameters 
  should be new and should not be already present.  If READ, they should be 
  present, return those that intersect the index range we want.  Similar
  to CheckExistingEdgeToEdge except we compare to node.
****************************************************************************/
ListPointer CheckExistingEdgeToNode(n, level)
    NodePointer n;
    int *level;
{
    EntryPointer ptr;
    ListPointer entry, list = NULL;
    char *name;
    int i, found = FALSE;

/* Get name of vector from node */
    name = (char *)GetAttribute(n->Arguments, "array_name");

/* Search for existing vectors with same name, and check for overlap range */

    for (i = indexlevel; i >= 0; i--) {
      for (ptr = Edgetable[i][hashfunc(name)]; ptr != NULL; ptr = ptr->Next) {
        if ((strcmp(name, ptr->Name) == 0) && OverlapRangeToNode(n,EP(ptr))) {
	    entry = new_list(EdgeNode, ptr->Entry);
	    list = ListAppend(entry, list);
	    found = TRUE;
	}
      }
      if (found) break;
    }
    *level = i;
    return(list);
}
	
/**************************************************************************
  CheckExactVectorEdge() -- Checks for existing vectors falling in the 
  range of indices given by E.
****************************************************************************/
ListPointer CheckExactVectorEdge(E, level)
Expression E;
int *level;
{
    EntryPointer ptr;
    ListPointer entry, list = NULL;
    char *name;
    int i, found = FALSE;

/* Get name of vector from node */
    name = E->ExpDef.SonsForIndexed.ThisArray->SymbolId.IdName;

/* Search for existing vectors with same name, and check for overlap range */

    for (i = indexlevel; i >= 0; i--) {
      for (ptr = Edgetable[i][hashfunc(name)]; ptr != NULL; ptr = ptr->Next) {
        if ((strcmp(name, ptr->Name) == 0) && ExactRangeToExp(E, EP(ptr))) {
	    entry = new_list(EdgeNode, ptr->Entry);
	    list = ListAppend(entry, list);
	    found = TRUE;
	}
      }
      if (found) break;
    }
    *level = i;
    return(list);
}

/**************************************************************************
  CheckOverlapVectorEdge() -- Checks for existing vectors falling in the 
  range of indices given by E.
****************************************************************************/
ListPointer CheckOverlapVectorEdge(E, level)
Expression E;
int *level;
{
    EntryPointer ptr;
    ListPointer entry, list = NULL;
    char *name;
    int i, found = FALSE;

/* Get name of vector from node */
    name = E->ExpDef.SonsForIndexed.ThisArray->SymbolId.IdName;

/* Search for existing vectors with same name, and check for overlap range */

    for (i = indexlevel; i >= 0; i--) {
      for (ptr = Edgetable[i][hashfunc(name)]; ptr != NULL; ptr = ptr->Next) {
        if ((strcmp(name, ptr->Name) == 0) && OverlapRangeToExp(E, EP(ptr))) {
	    entry = new_list(EdgeNode, ptr->Entry);
	    list = ListAppend(entry, list);
	    found = TRUE;
	}
      }
      if (found) break;
    }
    *level = i;
    return(list);
}

/**************************************************************************
  Indexconst() -- determines if E has constant indices or not.
                  Null indices is considered constant.
****************************************************************************/
Indexconst(E)
    Expression E;
{
    int val = TRUE;
    Expression E1, E2;
    ITEREXP Result;

    if (E->ExpDef.SonsForIndexed.ThisArray->SingleOrArray.AS.InPlace == FALSE){
	for (E1 = E->ExpDef.SonsForIndexed.IndexTupleExp;
             E1 != NULL;
             E1 = E1->ExpDef.SonsForTuple.RestOfTuple) {
	    E2 = E1->ExpDef.SonsForTuple.FirstExpression;
            if (E2 != NULL) {
		IterBounds(E2, &Result);
                if(Result.type != Inttype)
		     val = FALSE;
	    }
 	}
    }
    return(val);
}

/**************************************************************************
  OverlapRangeToEdge() -- check if indices in Subgraph edge parameters over- 
  lap to range of existing edge.  If existing edge is not "lhs", return
  FALSE because they were not written. If index_Eval of the edges are present,
  use it.  Otherwise, the index is constant, use it.
****************************************************************************/
OverlapRangeToEdge(sube, e)
    EdgePointer sube, e;
{
    char label[STRSIZE], labelEval[STRSIZE], *range;
    int lb1, ub1, lb2, ub2;
    int i = 0;
 
    sprintf(labelEval, "index%dEval", i);
    sprintf(label, "index%d", i);
    while (HasAttribute(sube->Arguments, label)) {

/* find range of subgraph edge index */
       if (HasAttribute(sube->Arguments, labelEval))
	   ExtractRange((char *) GetAttribute(sube->Arguments, labelEval), 
								&lb1,&ub1);
       else if (HasAttribute(sube->Arguments, label)) {
	   range = (char *)GetAttribute(sube->Arguments, label);  
           if (isnumber(range))   /* is a number */
	       lb1 = ub1 = atoi(range);
           else  	/* is of form lb..ub */
	       ExtractRange(range, &lb1,&ub1);
       } else {
	   printmsg("OverlapRangeToEdge:","Cannot find subgraph edge ");
	   printmsg(NULL, "%s index %s\n", sube->Name, label);
	   PrintSilage(sube->Attributes);
       }

/* find range of existing edge index */
       if (HasAttribute(e->Arguments, labelEval))
	   ExtractRange((char *) GetAttribute(e->Arguments, labelEval), 
								&lb2,&ub2);
       else if (HasAttribute(e->Arguments, label)) {
	   range = (char *)GetAttribute(e->Arguments, label);  
           if (isnumber(range))   /* is a number */
	       lb2 = ub2 = atoi(range);
           else  	/* is of form lb..ub */
	       ExtractRange(range, &lb2,&ub2);
       } else {
	   printmsg("OverlapRangeToEdge:","Cannot find subgraph edge ");
	   printmsg(NULL, "%s index %s\n", e->Name, label);
	   PrintSilage(e->Attributes);
       }

/* Check if subgraph edge overlaps existing edge.  If at any index, there
   is no overlap, then the entire array does not overlap.                 */
       if (ub1 < lb2 || ub2 < lb1 )
	   return(FALSE);

    sprintf(labelEval, "index%dEval", ++i);
    sprintf(label, "index%d", i);
    }
    return(TRUE);
}
       
/**************************************************************************
  OverlapRangeToNode() -- check if constant indices in node parameters falls 
  in range of edge, or if variable indices in node->InEdges overlap to range
  of edge.  If edge is not "lhs" return FALSE because they were not written.
  The first part check for constant indices and calls OverlapRange.  The second 
  does range overlapping.
****************************************************************************/
OverlapRangeToNode(n, e)
    NodePointer n;
    EdgePointer e;
{
    char label[STRSIZE], labelEval[STRSIZE], *range;
    ListPointer ptr;
    int lb1, ub1, lb2, ub2;
    int i;
 
/* For Read Node, index edge starts at n->InEdges.  For Write Node, index edge
   starts at n->InEdges->Next */
    if (strcmp(GetFunction(n), "read") == 0)
        ptr = n->InEdges;
    else
        ptr = n->InEdges->Next;

/* Check for constant indices , no index edges */
    if (ptr == NULL) return(OverlapRange(n, e));

/* Has index edges, must do overlap computation */
    for (i=0; ptr != NULL; ptr = ptr->Next, i++) {
/* Obtain range of index edge */
       ExtractRange((char *)GetAttribute(EP(ptr)->Arguments, "manifest"),
								&lb1, &ub1);
/* Obtain range of Edgetable edge.  Look for manifest range in index_Eval, 
   which must be of the form lb..ub.  If index_Eval does not exist, means 
   range in index is manifest.  This can be of the form lb or lb..ub    */
       sprintf(labelEval, "index%dEval", i);
       sprintf(label, "index%d", i);
       if (HasAttribute(e->Arguments, labelEval))
	   ExtractRange((char *) GetAttribute(e->Arguments, labelEval), 
								&lb2,&ub2);
       else if (HasAttribute(e->Arguments, label)) {
	   range = (char *)GetAttribute(e->Arguments, label);  
           if (isnumber(range))   /* is a number */
	       lb2 = ub2 = atoi(range);
           else  	/* is of form lb..ub */
	       ExtractRange(range, &lb2,&ub2);
       } else {
	   printmsg("OverlapRangeToNode:","Cannot find edge ");
	   printmsg(NULL, "%s index %s\n", e->Name, label);
	   PrintSilage(e->Attributes);
       }

/* Check if index edge overlaps Edgetable edge.  If at any index, there is
   no overlap, then the entire array does not overlap.                   */
       if (ub1 < lb2 || ub2 < lb1 )
	   return(FALSE);
    }
    return(TRUE);
}
       
/**************************************************************************
  OverlapRange() --  Assume n has only constant indices, check if index of n 
  overlaps the range of index arguments of edge e.
****************************************************************************/
OverlapRange(n, e)
    NodePointer n;
    EdgePointer e;
{
    char label[STRSIZE], labelEval[STRSIZE];
    char *range;
    int i=0, lb1, ub1, lb2, ub2;

    sprintf(labelEval, "index%dEval", i);
    sprintf(label, "index%d", i);
    while (HasAttribute(n->Arguments, label)) {
/* find range of node indices */
        range = (char *)GetAttribute(n->Arguments, label);
        if (isnumber(range))   /* is a number */
            lb1 = ub1 = atoi(range);
        else         /* is of form lb..ub */
            ExtractRange(range, &lb1,&ub1);

/* find range of existing edge */
        if (HasAttribute(e->Arguments, labelEval))
            ExtractRange((char *) GetAttribute(e->Arguments, labelEval),
                                                                &lb2,&ub2);
        else if (HasAttribute(e->Arguments, label)) {
            range = (char *)GetAttribute(e->Arguments, label);
            if (isnumber(range))   /* is a number */
                lb2 = ub2 = atoi(range);
            else         /* is of form lb..ub */
                ExtractRange(range, &lb2,&ub2);
        } else {
	   printmsg("OverlapRangeToNode:","Cannot find edge ");
	   printmsg(NULL, "%s index %s\n", e->Name, label);
	   PrintSilage(e->Attributes);
	}

/* If range of node is not disjoint, but enclose range of edge, issue warning
 * that not all range needed by node is found                            */
/*      if (lb1 < lb2 || ub1 > ub2)
	    printmsg("OverlapRange:","Range of index may not be satisfied\n"); */
/* If range of node is disjoint from range of edge, return FALSE */
        if (ub1 < lb2 || ub2 < lb1) return(FALSE);
        sprintf(labelEval, "index%dEval", ++i);
        sprintf(label, "index%d", i);
    }
    return(TRUE);
}
       
/**************************************************************************
  ExactRangeToExp() --  Assume E has only constant indices, check if E has the 
  exact range of index arguments of edge e.
****************************************************************************/
ExactRangeToExp(E, e)
    Expression E;
    EdgePointer e;
{
    Expression E1, E2;
    ITEREXP Result;
    int i;
    int lb1, lb2, ub1, ub2;
    char label[STRSIZE], labelEval[STRSIZE], *range;

    for (E1 = E->ExpDef.SonsForIndexed.IndexTupleExp, i=0;
         E1 != NULL;
         E1 = E1->ExpDef.SonsForTuple.RestOfTuple, i++) {
	E2 = E1->ExpDef.SonsForTuple.FirstExpression;
        if(E2 != NULL) {
            IterBounds(E2, &Result);
            if(Result.type == Inttype) {
		lb1 = ub1 = Result.value;
            } else {
		printmsg("ExactRangeToExp:","Non Constant index\n");
                continue;
            }
        }
        else {
	    lb1 = 0;
	    ub1 = E->ExpDef.SonsForIndexed.ThisArray-> SingleOrArray.AS.
							DimensionBounds[i] - 1;
        }

/* Obtain range of Edgetable edge.  Look for manifest range in index_Eval,
   which must be of the form lb..ub.  If index_Eval does not exist, means
   range in index is manifest.  This can be of the form lb or lb..ub    */
       sprintf(labelEval, "index%dEval", i);
       sprintf(label, "index%d", i);
       if (HasAttribute(e->Arguments, labelEval))
           ExtractRange((char *) GetAttribute(e->Arguments, labelEval),
                                                                &lb2,&ub2);
       else if (HasAttribute(e->Arguments, label)) {
           range = (char *)GetAttribute(e->Arguments, label);
       if (isnumber(range))   /* is a number */
               lb2 = ub2 = atoi(range);
           else         /* is of form lb..ub */
               ExtractRange(range, &lb2,&ub2);
       } else {
	   printmsg("ExactRangeToExp:","Cannot find edge ");
	   printmsg(NULL, "%s index %s\n", e->Name, label);
	   PrintSilage(e->Attributes);
       }

       if (lb1 != lb2 || ub1 != ub2)
	   return(FALSE);
    }
    return(TRUE);
}

/**************************************************************************
  OverlapRangeToExp() --  Assume E has only constant indices,check if E has the 
  overlap range of index arguments of edge e.
****************************************************************************/
OverlapRangeToExp(E, e)
    Expression E;
    EdgePointer e;
{
    Expression E1, E2;
    ITEREXP Result;
    int i;
    int lb1, lb2, ub1, ub2;
    char label[STRSIZE], labelEval[STRSIZE], *range;

    for (E1 = E->ExpDef.SonsForIndexed.IndexTupleExp, i=0;
         E1 != NULL;
         E1 = E1->ExpDef.SonsForTuple.RestOfTuple, i++) {
	E2 = E1->ExpDef.SonsForTuple.FirstExpression;
        if(E2 != NULL) {
            IterBounds(E2, &Result);
            if(Result.type == Inttype) {
		lb1 = ub1 = Result.value;
            } else {
		printmsg("ExactRangeToExp:","Non-constant index\n");
                continue;
            }
        }
        else {
	    lb1 = 0;
	    ub1 = E->ExpDef.SonsForIndexed.ThisArray-> SingleOrArray.AS.
							DimensionBounds[i] - 1;
        }

/* Obtain range of Edgetable edge.  Look for manifest range in index_Eval,
   which must be of the form lb..ub.  If index_Eval does not exist, means
   range in index is manifest.  This can be of the form lb or lb..ub    */
       sprintf(labelEval, "index%dEval", i);
       sprintf(label, "index%d", i);
       if (HasAttribute(e->Arguments, labelEval))
           ExtractRange((char *) GetAttribute(e->Arguments, labelEval),
                                                                &lb2,&ub2);
       else if (HasAttribute(e->Arguments, label)) {
           range = (char *)GetAttribute(e->Arguments, label);
       if (isnumber(range))   /* is a number */
               lb2 = ub2 = atoi(range);
           else         /* is of form lb..ub */
               ExtractRange(range, &lb2,&ub2);
       } else {
	   printmsg("OverlapRangeToExp:","Cannot find edge ");
	   printmsg(NULL, "%s index %s\n", e->Name, label);
	   PrintSilage(e->Attributes);
	}

       if (ub1 < lb2 || ub2 < lb1 )
	   return(FALSE);
    }
    return(TRUE);
}

/**************************************************************************
  IsVectorEdge() -- returns true if Edge has a range index.
****************************************************************************/
IsVectorEdge(edge)
    EdgePointer edge;
{
    char label[STRSIZE], labelEval[STRSIZE], *range;
    int lb1, ub1, lb2, ub2;
    int i = 0;

    if (edge == NULL || !HasAttribute(edge->Arguments, "index0"))
        return(FALSE);

    sprintf(labelEval, "index%dEval", i);
    sprintf(label, "index%d", i);
    while (HasAttribute(edge->Arguments, label)) {

/* find range of edge index */
       if (HasAttribute(edge->Arguments, labelEval))
           ExtractRange((char *) GetAttribute(edge->Arguments, labelEval),
                                                                &lb1,&ub1);
       else if (HasAttribute(edge->Arguments, label)) {
           range = (char *)GetAttribute(edge->Arguments, label);
           if (isnumber(range))   /* is a number */
               lb1 = ub1 = atoi(range);
           else         /* is of form lb..ub */
               ExtractRange(range, &lb1,&ub1);
       } else {
	   printmsg("IsVectorEdge:","Cannot find edge ");
	   printmsg(NULL, "%s index %s\n", edge->Name, label);
	   PrintSilage(edge->Attributes);
	}

       if (lb1 != ub1) return(TRUE);
    sprintf(labelEval, "index%dEval", ++i);
    sprintf(label, "index%d", i);
    }
    return(FALSE);
}

/**************************************************************************
  IsVector() -- returns true if E has a null index.
****************************************************************************/
IsVector(E)
    Expression E;
{
    Expression E1;

    if (E->ExpDef.SonsForIndexed.ThisArray->SingleOrArray.AS.InPlace
                                                                == FALSE) {
        for (E1 = E->ExpDef.SonsForIndexed.IndexTupleExp;
             E1 != NULL;
             E1 = E1->ExpDef.SonsForTuple.RestOfTuple) {
            if (E1->ExpDef.SonsForTuple.FirstExpression == NULL) {
		return(TRUE);
            }
	}
    }
    return(FALSE);
}

/**************************************************************************
  IsAllNullIndex() -- returns true if E has all null index.
****************************************************************************/
IsAllNullIndex(E)
    Expression E;
{
    Expression E1;

    if (E->ExpDef.SonsForIndexed.ThisArray->SingleOrArray.AS.InPlace
                                                                == FALSE) {
        for (E1 = E->ExpDef.SonsForIndexed.IndexTupleExp;
             E1 != NULL;
             E1 = E1->ExpDef.SonsForTuple.RestOfTuple) {
            if (E1->ExpDef.SonsForTuple.FirstExpression != NULL) {
		return(FALSE);
            }
	}
    }
    return(TRUE);
}

/****************************************************************************
   InstallConstIndex() -- Put const indices into node or edge as arguments.
****************************************************************************/
InstallConstIndex(E, entry, type)
    Expression E;
    pointer entry;
    int type;
{
    Expression E1, E2;
    char ptr1[STRSIZE], *ptr2, *SPrintExp();
    char str[STRSIZE];
    ITEREXP Result;
    int i;
    NodePointer n;
    EdgePointer e;
    
    n = (NodePointer)entry;
    e = (EdgePointer)entry;

    if (E->ExpDef.SonsForIndexed.ThisArray->SingleOrArray.AS.
							InPlace == FALSE) {
        for (E1 = E->ExpDef.SonsForIndexed.IndexTupleExp, i=0;
             E1 != NULL;
             E1 = E1->ExpDef.SonsForTuple.RestOfTuple, i++) {
	    E2 = E1->ExpDef.SonsForTuple.FirstExpression;
	    if (E2 != NULL) {
		IterBounds(E2, &Result);
		if (Result.type == Inttype) {
		    sprintf(ptr1, "%d", Result.value);
		} else {  /* IDtype or OPtype */
		    printmsg("InstallConstIndex:","non-constant type %d\n",
					Result.type);
		}
	    }
	    else {
		sprintf(ptr1,"0..%d",E->ExpDef.SonsForIndexed.ThisArray->
			      SingleOrArray.AS.DimensionBounds[i] - 1);
            }
	    sprintf(str,"index%d", i);
            if (type != NODETYPE) {
                e->Arguments = SetAttribute(str, CharNode,
                          Intern(ptr1), e->Arguments);
            }
            else {
                n->Arguments = SetAttribute(str, CharNode,
                          Intern(ptr1), n->Arguments);
            }
         }
    }
    else {
        printmsg("InstallConstIndex:","E->..InPlace = TRUE. Fatal!\n");
    }
}

/****************************************************************************
   InstallIndexToEdge() -- Put constant or variable indices to edge.
   For variable indices, put indexEval in them.
****************************************************************************/
InstallIndexToEdge(n, e)
    NodePointer n;
    EdgePointer e;
{
    ListPointer ptr;
    int i, lb1, ub1;
    char *range;
    char label[STRSIZE], labelEval[STRSIZE], str[STRSIZE];

/* For Read Node, index edge starts at n->InEdges.  For Write Node, index edge
   starts at n->InEdges->Next */
    if (strcmp(GetFunction(n), "read") == 0)
	ptr = n->InEdges;
    else
	ptr = n->InEdges->Next;

/* check for constant indices first and add as arguments  */
    if(ptr == NULL) {
         InstallConstIndexToEdge(n, e);
    } else {
/* Look at names or values of index edges of node to get indices */
        for (i=0; ptr != NULL; ptr = ptr->Next, i++) {
	    sprintf(label, "index%d", i);
	    sprintf(labelEval, "index%dEval", i);
	    range = (char *)GetAttribute(EP(ptr)->Arguments, "manifest");
	    if (strcmp(EP(ptr)->Class, "constant") == 0) {
                ExtractRange(range, &lb1, &ub1);
		if (lb1 == ub1)  /* constant index */
		    sprintf(str, "%d", lb1);
	 	else
		    sprintf(str, "%s", range);
            } else {
		sprintf(str, "%s", EP(ptr)->Name);
		e->Arguments = SetAttribute(labelEval, CharNode, range, 
								e->Arguments);
            }
  	    e->Arguments = SetAttribute(label, CharNode, Intern(str), 
								e->Arguments);
         }
    }
}

/****************************************************************************
   CopyIndexEval() -- Copy IndexEval of edge of lower indexlevel because this
   is the edge truly produced.  This will allow a[i]_c and a[i-1]_c not to
   overlap.
****************************************************************************/
CopyIndexEval(srce, deste)
EdgePointer srce, deste;
{
    char label[STRSIZE], labelEval[STRSIZE], *range;
    char *MakeRange();
    int i=0, lb1, ub1;

    sprintf(label,"index%d",i);
    while(HasAttribute(deste->Arguments,label)) {
	sprintf(labelEval, "index%dEval", i);
	if ((range =(char *)GetAttribute(srce->Arguments, labelEval)) != NULL)
            deste->Arguments = SetAttribute (labelEval, CharNode, range,
                                                    deste->Arguments);
        else {
	    range = (char *)GetAttribute(srce->Arguments, label);
            if (isnumber(range)) {  /* is a number */
               lb1 = ub1 = atoi(range);
	       range = MakeRange(lb1, ub1);
	    }
            deste->Arguments = SetAttribute (labelEval, CharNode, range,
                                                    deste->Arguments);
	}
        sprintf(label,"index%d",++i);
    }
}

/****************************************************************************
   InstallConstIndexToEdge() -- Put const indices into edge as arguments.
****************************************************************************/
InstallConstIndexToEdge(n, e)
    NodePointer n;
    EdgePointer e;
{
    char label[STRSIZE];
    char *range;
    int i=0;
    
    sprintf(label, "index%d", i);
    while (HasAttribute(n->Arguments, label)) {
        range = (char *)GetAttribute(n->Arguments, label);
	e->Arguments = SetAttribute(label, CharNode, range, e->Arguments);
        sprintf(label, "index%d", ++i);
    }
}

/****************************************************************************
   ConnectIndexToNode() -- Put indices into Read/Write node as arguments or 
   data edges.
****************************************************************************/
ConnectIndexToNode(E, n)
    Expression E;
    NodePointer n;
{
    extern NodePointer NodeList;
    extern EdgePointer EdgeList;
    Expression IndList;
    ListPointer l;
    char str[STRSIZE];
    EdgePointer e, NewEdge();
    NodePointer constnode, NewNode();
    int i;
    
/* check for constant indices first and add as arguments  */
    if(Indexconst(E)) {
         InstallConstIndex(E, n, NODETYPE);
    }
    else {
/* add indices as input edges */
         for (IndList = E->ExpDef.SonsForIndexed.IndexTupleExp, i=0;
              IndList != NULL;
              IndList = IndList->ExpDef.SonsForTuple.RestOfTuple, i++) {
	     if (IndList->ExpDef.SonsForTuple.FirstExpression != NULL) {
	          l = ConstructExp(IndList->ExpDef.SonsForTuple.
				FirstExpression);
		  ConnectInEdge(n, EP(l));
	 	  if (!HasAttribute(EP(l)->Arguments, "manifest")) {
	 	      printmsg("Warning",
			"Non-manifest index edge %s\n", EP(l)->Name);
		      PrintSilage(EP(l)->Attributes);
		      EP(l)->Arguments = SetAttribute("manifest", CharNode,
				Intern("unknown"), EP(l)->Arguments);
		  }
	     } 
	     else {
/* create new constant node, with temp output edge, whose range is 0 to
   DimensionBounds[i] - 1, put "value" on node, and "manifest" on edge.
   Connect to Read Node     */
		  constnode = NewNode(CONST, "data");
                  constnode->Attributes = SaveSilage(constnode->Attributes, 
						     E->ExpPos);
		  sprintf(str,"0..%d",E->ExpDef.SonsForIndexed.ThisArray->
			      SingleOrArray.AS.DimensionBounds[i] - 1);
                  constnode->Arguments = SetAttribute("value", CharNode,
			  Intern(str), constnode->Arguments);
		  NodeList = NodeListAppend(constnode, NodeList);
	   	  e = NewEdge(NULL, "constant", "int");
                  e->Attributes = SaveSilage(e->Attributes, E->ExpPos);
		  e->Arguments = SetAttribute("manifest", CharNode, Intern(str),
								e->Arguments);
		  ConnectOutEdge(constnode,e);
		  EdgeList = EdgeListAppend(e, EdgeList);
		  ConnectInEdge(n, e); 
	      }
   	  }
    }
}
