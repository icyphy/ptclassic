/* $Id$ */

/* 
 * Definitions of useful macros for FlowGraph
 */
#define IsModule(graph)    ((graph)? (!strcmp((graph)->Class,"MODULE")):(FALSE))
#define list_entry(list, type) ((list)? ((type)((list)->Entry)) : (NULL))
#define for_each_list(list, initial)   for (list=(ListPointer)(initial); \
					    list!=NULL; list=(list)->Next)
#define for_each_node(node, initial)   for (node=(NodePointer)(initial); \
					    node!=NULL; node=(node)->Next)
#define for_each_edge(edge, initial)   for (edge=(EdgePointer)(initial); \
                                            edge!=NULL; edge=(edge)->Next)
#define for_each_graph(graph, initial) for (graph=(GraphPointer)(initial);\
	             (graph!=NULL)&&(IsModule(graph)); graph=graph->Next)

#define _parent_(graph)    ((graph)? ((LIST *)((graph)->Parents))  : (NULL))
#define ParentNode(graph)  (_parent_(graph) ? \
                           (list_entry(_parent_(graph)->Next, NodePointer)) :\
			   (NULL))
#define ParentGraph(graph) (_parent_(graph) ? \
			   (list_entry(_parent_(graph), GraphPointer)) : (NULL))

