/* $Id$ */

/* 
 * Definitions of data structures used to represent flow graphs
 */


/* Generic pointer structure */
typedef char *pointer;

/* Pointer definitions */
typedef struct LIST *ListPointer;
typedef struct NODE *NodePointer;
typedef struct EDGE *EdgePointer;
typedef struct GRAPH *GraphPointer;

/* Generic LIST structure */

typedef enum { IntNode, RealNode, CharNode, ListNode, GraphNode, EdgeNode,
	       NodeNode, ArrayNode, NullNode }
	EntryType;

typedef struct LIST {
	char *Label;
	EntryType Type;
	pointer Entry;
	ListPointer Next;
} LIST;

/* Graph describing data structures */

typedef struct GRAPH {
	char *Name;
	char *Class;
	EdgePointer EdgeList;
	NodePointer NodeList;
	EdgePointer ControlList;
	ListPointer Parents;
	ListPointer InEdges;
	ListPointer OutEdges;
	ListPointer InControl;
	ListPointer OutControl;
	ListPointer Arguments;
	ListPointer Attributes;
	ListPointer Model;
	GraphPointer Next;
	pointer Extension;      /* Attach your program structures here */
	int Token;		/* Bogus field available to the service
				   Programs (for ordering e.g.) */
} GRAPH;
	

typedef struct NODE {
	char *Name;
	char *Class;
	GraphPointer Master;
	ListPointer InEdges;
	ListPointer OutEdges;
	ListPointer InControl;
	ListPointer OutControl;
	ListPointer Arguments;
	ListPointer Attributes;
	NodePointer Next;
	pointer Extension;      /* Attach your program structures here */
	int Token;		/* Bogus field available to the service
				   Programs (for ordering e.g.) */
} NODE;

typedef struct EDGE {
	char *Name;
	char *Class;
	ListPointer InNodes;
	ListPointer OutNodes;
	ListPointer Arguments;
	ListPointer Attributes;
	EdgePointer Next;
	pointer Extension;      /* Attach your program structures here */
	int Token;		/* Bogus field available to the service
				   Programs (for ordering e.g. */
} EDGE;
