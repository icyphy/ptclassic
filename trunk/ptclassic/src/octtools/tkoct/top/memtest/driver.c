/**
**/

#include "topFixup.h"
#include <stdio.h>

#include "topStd.h"
#include "topMem.h"
#include "memStats.h"
#include "topDlList.h"

typedef struct _Obj1 Obj1;
struct _Obj1 {
    int a;
    int b;
};

/**
    Use the approach if you have several heaps all containing the same
    size objects.  The memory is all allocated in the superheap,
    but each heap keeps seperate allocation stats.
**/
void test_heaps() {
    int			i, j;
    MEMSuperHeapHandle	obj1_sheap;
    MEMHeapHandle	obj1_heap;
    Obj1		*pObj1;

    obj1_sheap = memSuperHeapCreate("obj1", sizeof(Obj1));
    for (i=0; i < 10; i++) {
        obj1_heap = memHeapCreate(memStrSaveFmt("heap%d",i), obj1_sheap);
	for (j=0; j<100 + i*1000; j++) {
	    pObj1 = (Obj1*) memHeapObAlloc(obj1_heap);
	}
    }
}

/**
    There are a set of "public" super heaps that are builtin
    to the mem system.  Thus you can get your own heap attached
    to a public super heap: you have mem stats private to yourself,
    but you share memory with everyone else.
**/
void test_pubs() {
    int			i, j;
    MEMHeapHandle	obj1_heap;
    Obj1		*pObj1;

    for (i=0; i < 10; i++) {
        obj1_heap = memPublicHeapCreate(memStrSaveFmt("pheap%d",i), sizeof(Obj1));
	for (j=0; j<100 + i*1000; j++) {
	    pObj1 = (Obj1*) memHeapObAlloc(obj1_heap);
	}
    }
}

/**
    Create several double linked lists of objects.  The dllist package
    internally uses the mem package.
**/
void test_dllist() {
    int			i, j;
    DLListHandle	obj1_list;
    Obj1		*pObj1;

    for (i=0; i < 10; i++) {
        obj1_list = dlListCreate(memStrSaveFmt("dllist%d",i), sizeof(Obj1));
	for (j=0; j <10 + i*100; j++) {
            pObj1 = (Obj1*) dlListCreateAtHead(obj1_list);
	}
    }
}

int main( int argc, char **argv) {
    memInitialize();
    memCoreStatsShow();
    test_heaps();
    test_pubs();
    test_dllist();
    printf("\n\nAllocations done.\n\n");
    memCoreStatsShow();

    exit(0);
}
