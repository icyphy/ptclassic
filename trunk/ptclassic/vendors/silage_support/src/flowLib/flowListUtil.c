/* $Id$ */

/* Copyright (C) 1990 Regents of the University of California
 * All rights reserved.
 */

/* Support Routines for List structures */

#include "flowgraph.h"

/* global functions */
extern ListPointer new_list();
extern char *Intern();

/*
 * Generic Operators on Lists
 */

ListPointer
ListCreate(Label, Type, Entry)
char *Label;
EntryType Type;
pointer Entry;
{
    ListPointer Ptr;

    Ptr = new_list(Type, Entry);
    if (Label) Ptr->Label = Intern(Label);
    else       Ptr->Label = NULL;
    return (Ptr);
}

ListPointer
ListAdd(Entry, List)
ListPointer Entry, List;
{
    if (Entry == NULL)
	return (List);

    Entry->Next = List;
    return (Entry);
}

ListPointer
ListSelectiveAdd(Entry, List)
ListPointer Entry, List;
{
    register ListPointer Ptr;

    if (Entry == NULL)
	return (List);

    Entry->Next = NULL;    /* for safety */
    for (Ptr = List; Ptr != NULL; Ptr = Ptr->Next)
	if (Ptr->Type == Entry->Type && Ptr->Label == Entry->Label &&
	    Ptr->Entry == Entry->Entry) {
	    ClearList(Entry);
	    return (List);
    }

    Entry->Next = List;
    return (Entry);
}

ListPointer
ListAppend(Entry, List)
ListPointer Entry, List;
{
    register ListPointer p;

    if (List == NULL)
	return (Entry);
    for (p = List; p->Next != NULL; p = p->Next);
    p->Next = Entry;
    return (List);
}

ListPointer
ListSet(Entry, List, N)
ListPointer Entry, List;
int N;
{
    int Count;
    ListPointer Ptr;

    for (Count = 0, Ptr = List; Count < N && Ptr != NULL;
         Count++, Ptr = Ptr->Next);

    if (Count == N && Ptr != NULL) {
        Ptr->Entry = Entry->Entry;
        Ptr->Type = Entry->Type;
        ClearList(Entry);
    }
    else {
        for (; Count < N; Count++)
            List = ListAppend(new_list(NullNode, NULL), List);
        List = ListAppend(Entry, List);
    }
    return(List);
}

ListPointer
RemoveFromList(Entry, List)
ListPointer Entry, List;
{
    ListPointer Ptr, Last, val;

    for (Ptr = List, Last = NULL; Ptr != NULL; Last = Ptr, Ptr = Ptr->Next)
	if (Ptr == Entry)
	    break;
    if (Ptr == NULL)
	return (List);
    else if (Last == NULL)
	val = Ptr->Next;
    else {
        Last->Next = Entry->Next;
	val = List;
    }
    free_list(Entry);
    return (val);
}

ListPointer
RemoveEntryFromList(Type, Entry, List)
EntryType Type;
pointer Entry;
ListPointer List;
{
    ListPointer Ptr, Last, val;

    for (Ptr = List, Last = NULL; Ptr != NULL; Last = Ptr, Ptr = Ptr->Next)
	if (Ptr->Type == Type && Ptr->Entry == Entry)
	    break;
    if (Ptr == NULL)
	return (List);
    else if (Last == NULL)
	val = Ptr->Next;
    else {
        Last->Next = Ptr->Next;
	val = List;
    }
    free_list(Ptr);
    return (val);
}

ListPointer
OrderListInAscendingOrder(UnorderedList)
ListPointer UnorderedList;
{
    ListPointer OrderedList = NULL;
    register ListPointer Ptr2, Last2;
    ListPointer Ptr, Next;

    if (UnorderedList == NULL)
	return (NULL);

    for (Ptr = UnorderedList; Ptr != NULL; Ptr = Next) {
        Next = Ptr->Next;
	for (Ptr2 = OrderedList, Last2 = NULL; Ptr2 != NULL;
	     Last2 = Ptr2, Ptr2 = Ptr2->Next) 
	    if (Ptr2->Label >= Ptr->Label)
		break;

	Ptr->Next = Ptr2;
	if (Last2 == NULL)
	    OrderedList = Ptr;
	else
	    Last2->Next = Ptr;
    }
    return (OrderedList);
}

ListPointer
OrderListInDescendingOrder(UnorderedList)
ListPointer UnorderedList;
{
    ListPointer OrderedList = NULL;
    register ListPointer Ptr2, Last2;
    ListPointer Ptr, Next;

    if (UnorderedList == NULL)
	return (NULL);

    for (Ptr = UnorderedList; Ptr != NULL; Ptr = Next) {
        Next = Ptr->Next;
	for (Ptr2 = OrderedList, Last2 = NULL; Ptr2 != NULL;
	     Last2 = Ptr2, Ptr2 = Ptr2->Next) 
	    if (Ptr2->Label <= Ptr->Label)
		break;

	Ptr->Next = Ptr2;
	if (Last2 == NULL)
	    OrderedList = Ptr;
	else
	    Last2->Next = Ptr;
    }
    return (OrderedList);
}

ListPointer
CopyList(List)
ListPointer List;
{
    ListPointer NewList;
    ListPointer CopyListEntry();
    /* creates a complete copy of a list */

    NewList = NULL;
    for (; List; List = List->Next)
        NewList = ListAppend(CopyListEntry(List), NewList);
    return (NewList);
}

ListPointer
CopyListEntry(Entry)
ListPointer Entry;
{
    register ListPointer new; 

    new = new_list(Entry->Type, Entry->Entry);
    new->Label = Entry->Label;
    new->Next = NULL;
    return (new);
}

ListPointer
CopyListInDepth(List)
ListPointer List;
{
    ListPointer NewList;
    ListPointer CopyListEntryInDepth();
    /* creates a complete copy of a list */

    NewList = NULL;
    for (; List; List = List->Next)
        NewList = ListAppend(CopyListEntryInDepth(List), NewList);
    return (NewList);
}

ListPointer
CopyListEntryInDepth(Entry)
ListPointer Entry;
{
    register ListPointer new; 
    ListPointer CopyListInDepth();

    new = new_list(Entry->Type, (Entry->Type == ListNode) ?
	           (pointer) CopyListInDepth((ListPointer)Entry->Entry) :
		   Entry->Entry);
    new->Label = Entry->Label;
    new->Next = NULL;
    return (new);
}

ClearList(List)
ListPointer List;
{
    ListPointer Ptr, Next;

    for (Ptr = List; Ptr != NULL; Ptr = Next) {
	Next = Ptr->Next;
	if (Ptr->Type == ListNode)
	    ClearList( (ListPointer) Ptr->Entry);
	free_list(Ptr);
    }
}

int
LookupList(List, index, Entry, Type)
ListPointer List;
int index;
pointer *Entry;
EntryType *Type;
/* look up the index_th element in the List. return 0 if not found. */
{
    ListPointer Ptr;
    int count;
    int find = FALSE;

    for (Ptr = List, count = 0; Ptr != NULL; Ptr = Ptr->Next, count++) {
	if (count < index) continue;
	if (Entry) *Entry = Ptr->Entry;
	if (Type)  *Type = Ptr->Type;
	find = TRUE;
	break;
    }

    return (find);
}

int
GetIndexOfListEntry(List, Entry)
ListPointer List;
pointer Entry;
/* get the index of the Entry in the List (starting from 0.) 
 * return -1 if not found */
{
    ListPointer Ptr;
    int index = -1;
    int count;

    for (Ptr = List, count = 0; Ptr != NULL; Ptr = Ptr->Next, count++) {
	if (Ptr->Entry != Entry) continue;
        index = count;
	break;
    }

    return (index);
}

IsMemberOfList(Entry, Type, List)
pointer Entry;
EntryType Type;
ListPointer List;
{
    ListPointer Ptr;

    for (Ptr = List; Ptr != NULL; Ptr = Ptr->Next) {
	if (Ptr->Type == Type && Ptr->Entry == Entry)
	    return(TRUE);
    }
    return(FALSE);
}

ListCount(List)
ListPointer List;
{
    ListPointer Ptr;
    int i;

    for (Ptr = List, i=0; Ptr != NULL; Ptr = Ptr->Next, i++) ;
    return(i);
}

SelectiveListCount(List, Type)
ListPointer List;
EntryType Type;
{
    ListPointer Ptr;
    int i;

    i = 0;
    for (Ptr = List; Ptr != NULL; Ptr = Ptr->Next)
	if (Ptr->Type == Type)
	    i++;
    return(i);
}

PrintList(l)
ListPointer l;
{
    ListPointer ptr;

    for (ptr = l; ptr != NULL; ptr = ptr->Next) {
        switch(ptr->Type) {
            case IntNode:   { if (ptr->Label != NULL)
				printf("%s", ptr->Label);
			     else
				printf("Int");
			     printf(": %d", (int)ptr->Entry); 
			     break; }
	    case RealNode:  { if (ptr->Label != NULL)
                                 printf("%s", ptr->Label);
                              else
                                printf("Float");
			      printf(": %5.2f", *((float *)ptr->Entry));
                              break; }
            case CharNode:  { if (ptr->Label != NULL)
				printf("%s", ptr->Label);
			      else
			        printf("Char");
			      printf(": %s", (char *)ptr->Entry); 
			     break; }
            case EdgeNode:  if (ptr->Entry) 
				 printf("Edge: %s", 
				       ((EdgePointer) ptr->Entry)->Name);
                            else printf("Edge: nil");
			    break;
            case NodeNode:  if (ptr->Entry)
				 printf("Node: %s", 
				    ((NodePointer) ptr->Entry)->Name);
                            else printf("Node: nil");
			    break;
            case NullNode:  printf("Nill"); break;
            case GraphNode: printf("Parent"); break;
            case ListNode:  { printf("List: [\n"); 
			    PrintList((ListPointer)ptr->Entry); 
			    printf("]\n"); break; }
            default : printf("Unknown"); break;
        }
        printf(" -> ");
    }
    printf("\n");
}
