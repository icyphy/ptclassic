/* $Id$ */
#include "codegen.h"  /* booltype,flowstruct.h,dag.h(node funcs),RootGraph?? */
#include "stringswitch.h"  /* do switch on node functions */
#include "st.h" 

pointer GetAttribute();
ListPointer SetAttribute(), ListAppend(), new_list();
bool IsArray(), IsFixedType(), IsIntType(), IsBoolType(), IsHierarchy();
bool IsDelayIO();
bool AllEdgesInt(), AllEdgesBool(), AllEdgesFixed();
bool AllInEdgesInt(), AllInEdgesFixed();
char *Intern(), *DeclName();

extern FILE *CFD, *LogFile;
extern GraphPointer Root;
extern int indexlevel;
extern st_table *Edgetable[];
extern ListPointer ListOfParams;
extern bool pl_flag;

GenStatements(Graph,pl_flag,bittrue)
GraphPointer Graph;
bool pl_flag;
bool bittrue;
{
    register NodePointer Node;

    for(Node = Graph->NodeList; Node != NULL; Node = Node->Next) {
        if (IsHierarchy(Node)) {
	    GenHierarchyNode(Node,pl_flag,bittrue);
	}
	else {
	    GenSingleNode(Node,pl_flag,bittrue);
	}
    }
}

GenHierarchyNode(Node,pl_flag,bittrue)
NodePointer Node;
bool pl_flag;
bool bittrue;
{
    char *model;
    
    model = (char *)GetAttribute(Node->Master->Model, "model_name");
    STRINGSWITCH(model)
	FIRSTCASE(FUNC) {
	    Indent();
	    GenFuncCall(Node);
	}
	CASE(ITER) {
	    GenIterCall(Node,pl_flag,bittrue);
	}
	CASE(DO) {
	    Indent();
	    GenDoLoopCall(Node,pl_flag,bittrue);
	}
	CASE(DOBODY) {
	    Indent();
	    GenDoLoopBodyCall(Node,pl_flag,bittrue);
	}
	else {
	    printmsg(NULL, "Node: %s, Not-implemented\n", Node->Name);
	}
    ENDSWITCH
}

GenFuncCall(node)
NodePointer node;
{
    bool GenAddressOfFuncParams(), comma = false;
    GraphPointer Graph;

    Graph = node->Master;
    fprintf(CFD, "Sim_%s (", Graph->Name);
    if (GE(Graph)->HasDelay) {
        fprintf(CFD, "&(pST->");
	GenDelayInstanceName(Graph, node);
	if (HasAttribute(node->Attributes, "NrOfApps")) {
	    fprintf(CFD, "[FI_");
            GenDelayInstanceName(Graph, node);
	    fprintf(CFD, "++]");
	}
	fprintf(CFD, ")");
        comma = true;
    }
    ListOfParams = (ListPointer) NULL;
    comma = GenAddressOfFuncParams(node->InEdges, comma);
    comma = GenAddressOfFuncParams(node->InControl, comma);
    comma = GenAddressOfFuncParams(node->OutEdges, comma);
    comma = GenAddressOfFuncParams(node->OutControl, comma);
    fprintf(CFD, ");\n");
    ClearList(ListOfParams);
}

bool GenAddressOfFuncParams(List, comma)
ListPointer List;
bool comma;
{
    register ListPointer ptr;
    register EdgePointer edge;
 
    if (List == NULL) return(comma);
    for (ptr = List; ptr != NULL; ptr = ptr->Next) {
        edge = EP(ptr);
        if (IsMemberOfList(DeclName(edge), CharNode, ListOfParams)) continue;
	if (comma == true)
	    fprintf(CFD, ", ");
	else
	    comma = true;
        if (!IsArray(edge) && (IsFixedType(edge) || 
				(!IsFixedType(edge) && IsOutput(edge))))
	    fprintf(CFD, "&(");
	GenSingleEdgeRef(edge);
	/* could be [edge_C] or [at (1, edge_C, 2)] */
	GenEdgeDelay(edge);
	GenDelayRef(edge);
        if (!IsArray(edge) && (IsFixedType(edge) || 
				(!IsFixedType(edge) && IsOutput(edge))))
	    fprintf(CFD, ")");
        ListOfParams = ListAppend(new_list(CharNode, DeclName(edge)),
					ListOfParams);
    }
    return(comma);
}

GenIterCall(node,pl_flag,bittrue)
NodePointer node;
bool pl_flag;
bool bittrue;
{
    GraphPointer Graph;
    char *index;
    int min, max;

    Graph = node->Master;
    index = (char *)GetAttribute(node->Arguments, "index");
    min = (int)GetAttribute(node->Arguments, "min");
    max = (int)GetAttribute(node->Arguments, "max");
/* Generate initializations for local delay variables */
    GenInitDelayVars(node->Master);
    Indent();
    fprintf(CFD, "for (%s = (%d); %s <= (%d); %s++) ",index, min, index,
	    max, index);
    indexlevel++;
    Edgetable[indexlevel] = st_init_table(strcmp, st_strhash);
    OpenBraces();
    GenDeclLocalVars(Graph);
    fprintf(CFD, "\n");
/*  Remove Delays, Topological-Order, and generate statements for loops */
    RemoveDelaysInGraph(Graph);
    RemoveLpDelaysInGraph(Graph);
    TopologicalOrder(Graph);
    GenStatements(Graph,pl_flag,bittrue);
/*  Generate statements to display requested signals */
    GenDisplays(Graph,Graph->EdgeList,pl_flag);
    GenDisplays(Graph,Graph->ControlList,pl_flag);
/* Update loop delay counter */
    GenIncrementLoopDelays(Graph);
    CloseBraces();
    st_free_table(Edgetable[indexlevel]);
    indexlevel--;
/* Decrement Loop counter by 1 */
    if (IsProcessNode(node))
        GenDecrementLoopDelays(Graph, TRUE);
    else
        GenDecrementLoopDelays(Graph, FALSE);
}

GenDoLoopCall (node,pl_flag,bittrue)
NodePointer node;
bool pl_flag;
bool bittrue;
{
    GraphPointer LoopGraph;
  
    LoopGraph = node->Master;

    indexlevel++;
    Edgetable[indexlevel] = st_init_table(strcmp, st_strhash);
    OpenBraces();
    GenDeclLocalVars(LoopGraph);
    fprintf(CFD, "\n");
/*  Remove Delays, Topological-Order, and generate statements for loops */
    RemoveDelaysInGraph(LoopGraph);
    RemoveLpDelaysInGraph(LoopGraph);
    TopologicalOrder(LoopGraph);
    GenStatements(LoopGraph,pl_flag,bittrue);
/*  Generate statements to display requested signals */
    GenDisplays(LoopGraph,LoopGraph->EdgeList,pl_flag);
    GenDisplays(LoopGraph,LoopGraph->ControlList,pl_flag);
/* Update loop delay counter */
    GenIncrementLoopDelays(LoopGraph);
    CloseBraces();
    st_free_table(Edgetable[indexlevel]);
    indexlevel--;
}

GenDoLoopBodyCall (node,pl_flag,bittrue)
NodePointer node;
bool pl_flag;
bool bittrue;
{
    GraphPointer LoopBodyGraph;

    LoopBodyGraph = node->Master;

    fprintf(CFD, "do ");
    indexlevel++;
    Edgetable[indexlevel] = st_init_table(strcmp, st_strhash);
    OpenBraces();
    GenDeclLocalVars(LoopBodyGraph);
    fprintf(CFD, "\n");
/*  Remove Delays, Topological-Order, and generate statements for loops */
    RemoveDelaysInGraph(LoopBodyGraph);
    RemoveLpDelaysInGraph(LoopBodyGraph);
    TopologicalOrder(LoopBodyGraph);
    GenStatements(LoopBodyGraph,pl_flag,bittrue);
/*  Generate statements to display requested signals */
    GenDisplays(LoopBodyGraph,LoopBodyGraph->EdgeList,pl_flag);
    GenDisplays(LoopBodyGraph,LoopBodyGraph->ControlList,pl_flag);
/* Update loop delay counter */
    GenIncrementLoopDelays(LoopBodyGraph);
    CloseBraces();
    st_free_table(Edgetable[indexlevel]);
    indexlevel--;
    Indent();
    fprintf(CFD, "while (1);\n");
}

GenSingleNode(Node,pl_flag,bittrue)
NodePointer Node;
bool pl_flag;
bool bittrue;
{
    STRINGSWITCH(Node->Master->Name)
        FIRSTCASE(CONST) { 
	    Error("CONSTANT node still exists.");
        }
        CASE(INPUT) {
            EdgePointer edge;
            edge = (Node->OutEdges == NULL) ? EP(Node->OutControl) :
                                                EP(Node->OutEdges);
            if (HasAttribute(edge->Attributes, "InFile") &&
              (!((int)GetAttribute(edge->Attributes, "IsConstant")))) {
	        Indent();
                GenNextInputsOfEdge(edge,pl_flag,bittrue);
            }
        }
        CASE(SIN) { 
	    Indent();
	    if (AllEdgesFixed(Node)) GenUnaryFixed(Node, "Sin");
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
        }
        CASE(COS) { 
	    Indent();
	    if (AllEdgesFixed(Node)) GenUnaryFixed(Node, "Cos");
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
        }
        CASE(SQRT) { 
	    Indent();
	    if (AllEdgesFixed(Node)) GenUnaryFixed(Node, "Sqrt");
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
        }
        CASE(NEG) { 
	    Indent();
   	    if (AllEdgesInt(Node)) GenUnaryInt(Node, "-");
	    else if (AllEdgesFixed(Node)) GenUnaryFixed(Node, "Negate");
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
        }
        CASE(PLUS) { 
	    Indent();
   	    if (AllEdgesInt(Node)) GenBinaryInt(Node, "+");
	    else if (AllEdgesFixed(Node))	
			if(pl_flag && bittrue)	CastAndGenFixed(Node, "Plus");
			else 		GenBinaryFixed(Node, "Plus");
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
        }
        CASE(MINUS) { 
	    Indent();
   	    if (AllEdgesInt(Node)) GenBinaryInt(Node, "-");
	    else if (AllEdgesFixed(Node))	
			if(pl_flag && bittrue)	CastAndGenFixed(Node, "Minus");
			else 		GenBinaryFixed(Node, "Minus");
		/* temporary hack for ptolemy  */
		/* when loop iterators are used in calculation, there
		   is no precision information, added a int_to_fix()
  		   to do this conversion for compilation */
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
        }
        CASE(_MIN) { 
	    Indent();
   	    if (AllEdgesInt(Node)) GenMinMaxInt(Node, "_Min");
	    else if (AllEdgesFixed(Node))	
			if(pl_flag && bittrue)	CastAndGenFixed(Node, "Min");
			else 		GenBinaryFixed(Node, "Min");
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
        }
        CASE(_MAX) { 
	    Indent();
   	    if (AllEdgesInt(Node)) GenMinMaxInt(Node, "_Max");
	    else if (AllEdgesFixed(Node))	
			if(pl_flag && bittrue)	CastAndGenFixed(Node, "Max");
			else 		GenBinaryFixed(Node, "Max");
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
        }
        CASE(MULT) { 
	    Indent();
   	    if (AllEdgesInt(Node)) GenBinaryInt(Node, "*");
	    else if (AllEdgesFixed(Node))	
			if(pl_flag && bittrue)	CastAndGenFixed(Node, "Mult");
			else 		GenBinaryFixed(Node, "Mult");
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
        }
        CASE(DIV) { 
	    Indent();
   	    if (AllEdgesInt(Node)) GenBinaryInt(Node, "/");
	    else if (AllEdgesFixed(Node))	
			if(pl_flag && bittrue)	CastAndGenFixed(Node, "Div");
			else 		GenBinaryFixed(Node, "Div");
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
        } 
	CASE(LSH) {
	    EdgePointer edge;
	    Indent();
            edge = EP(Node->InEdges);
	    if (IsIntType(edge)) GenShiftInt(Node, "<<");
	    else if (IsFixedType(edge)) GenShiftFixed(Node, "SL");
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
	}
 	CASE(RSH) {
	    EdgePointer edge;
	    Indent();
            edge = EP(Node->InEdges);
	    if (IsIntType(edge)) GenShiftInt(Node, ">>");
	    else if (IsFixedType(edge)) GenShiftFixed(Node, "SR");
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
	}
        CASE(AND) { 
	    Indent();
   	    if (AllEdgesInt(Node)) GenBinaryInt(Node, "&");
   	    else if (AllEdgesBool(Node)) GenBinaryInt(Node, "&&");
	    else if (AllEdgesFixed(Node))	
			if(pl_flag && bittrue)	CastAndGenFixed(Node, "And");
			else 		GenBinaryFixed(Node, "And");
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
        }
        CASE(OR) { 
	    Indent();
   	    if (AllEdgesInt(Node)) GenBinaryInt(Node, "|");
   	    else if (AllEdgesBool(Node)) GenBinaryInt(Node, "||");
	    else if (AllEdgesFixed(Node))	
			if(pl_flag && bittrue)	CastAndGenFixed(Node, "Or");
			else 		GenBinaryFixed(Node, "Or");
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
        }
        CASE(EXOR) { 
	    Indent();
   	    if (AllEdgesInt(Node)) GenBinaryInt(Node, "^");
	    else if (AllEdgesFixed(Node))	
			if(pl_flag && bittrue)	CastAndGenFixed(Node, "Xor");
			else 		GenBinaryFixed(Node, "Xor");
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
        }
        CASE(NOT) { 
	    Indent();
   	    if (AllEdgesInt(Node)) GenUnaryInt(Node, "~");
   	    else if (AllEdgesBool(Node)) GenUnaryInt(Node, "!");
	    else if (AllEdgesFixed(Node)) GenUnaryFixed(Node, "Not");
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
        }
 	CASE(EQ) {
	    Indent();
	    if (AllInEdgesInt(Node)) GenBinaryInt(Node, "==");
	    else if (AllInEdgesFixed(Node))	
			if(pl_flag && bittrue)	CastAndGenFixed(Node, "EQ");
			else 		GenBinaryFixed(Node, "EQ");
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
        }
 	CASE(NTE) {
	    Indent();
	    if (AllInEdgesInt(Node)) GenBinaryInt(Node, "!=");
	    else if (AllInEdgesFixed(Node))	
			if(pl_flag && bittrue)	CastAndGenFixed(Node, "NTE");
			else 		GenBinaryFixed(Node, "NTE");
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
        }
 	CASE(GT) {
	    Indent();
	    if (AllInEdgesInt(Node)) GenBinaryInt(Node, ">");
	    else if (AllInEdgesFixed(Node))	
			if(pl_flag && bittrue)	CastAndGenFixed(Node, "GT");
			else 		GenBinaryFixed(Node, "GT");
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
        }
 	CASE(GTE) {
	    Indent();
	    if (AllInEdgesInt(Node)) GenBinaryInt(Node, ">=");
	    else if (AllInEdgesFixed(Node))	
			if(pl_flag && bittrue)	CastAndGenFixed(Node, "GTE");
			else 		GenBinaryFixed(Node, "GTE");
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
        }
 	CASE(LT) {
	    Indent();
	    if (AllInEdgesInt(Node)) GenBinaryInt(Node, "<");
	    else if (AllInEdgesFixed(Node))	
			if(pl_flag && bittrue)	CastAndGenFixed(Node, "LT");
			else 		GenBinaryFixed(Node, "LT");
	    else if (AllInEdgesFixed(Node)) GenBinaryFixed(Node, "LT");
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
        }
 	CASE(LTE) {
	    Indent();
	    if (AllInEdgesInt(Node)) GenBinaryInt(Node, "<=");
	    else if (AllInEdgesFixed(Node))	
			if(pl_flag && bittrue)	CastAndGenFixed(Node, "LTE");
			else 		GenBinaryFixed(Node, "LTE");
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
        }
 	CASE(BIT) {
	    Indent();
	    if (AllInEdgesFixed(Node)) GenBitSelect(Node);
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
        }
 	CASE(BITSELECT) {
	    Indent();
	    if (AllInEdgesFixed(Node)) GenBitSelect(Node);
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
        }
 	CASE(BITMERGE) {
	    Indent();
	    if (AllInEdgesFixed(Node)) GenBitMerge(Node);
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
        }
  	CASE(MUX) {
	    Indent();
 	    GenMux(Node);
	}
	CASE(READ) {
	    /* read nodes requires adding "readout" attribute to output edge */
	    EdgePointer Inedge, Outedge;
	    int suf;

	    Inedge = EP(Node->InControl);
	    Outedge = EP(Node->OutEdges);
            /* This edge is already declared */
            st_insert(Edgetable[indexlevel], DeclName(Outedge), 0);
	}
	CASE(WRITE) {
	    EdgePointer edge;
	    Indent();
            edge = EP(Node->InEdges);
	    if (IsIntType(edge)) GenWriteInt(Node);
	    else if (IsFixedType(edge)) GenWriteFixed(Node);
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
	}
	CASE(ASSIGN) {
	    EdgePointer edge;
	    Indent();
            edge = EP(Node->InEdges);
	    if (IsIntType(edge)) GenAssignInt(Node);
	    else if (IsFixedType(edge)) GenAssignFixed(Node);
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
 	}
	CASE(CAST) {
	    EdgePointer edge;
	    Indent();
            edge = EP(Node->InEdges);
	    if (IsIntType(edge)) GenAssignInt(Node);
	    else if (IsFixedType(edge)) GenCastFixed(Node);
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
 	}
	CASE(EXIT) {
	    Indent();
   	    if (AllEdgesBool(Node)) GenDoLoopExit(Node);
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
	}
	CASE(UPSAMPLE) {
	    Indent();
	    GenUpSample(Node);
        }
	CASE(DOWNSAMPLE) {
	    Indent();
	    GenDownSample(Node);
        }
	CASE(INTERPOLATE) {
	    Indent();
	    GenInterpolate(Node);
        }
	CASE(DECIMATE) {
	    Indent();
	    GenDecimate(Node);
        }
	CASE(TIMEMUX) {
	    GenTimeMux(Node);
        }
	CASE(TIMEDEMUX) {
	    GenTimeDeMux(Node);
        }
	CASE(NOP) {
	    EdgePointer edge;
	    Indent();
            edge = EP(Node->InEdges);
	    if (IsIntType(edge)) GenAssignInt(Node);
	    else if (IsFixedType(edge)) GenAssignFixed(Node);
	    else printmsg(NULL, "Node: %s, type-mismatch\n", Node->Name);
 	}
 	CASE (OUTPUT) {
	}
	else {
	    printmsg(NULL, "Node: %s, Not-implemented\n", Node->Name);
	}
    ENDSWITCH
}

GenNextInputsOfEdge(edge,pl_flag,bittrue)
EdgePointer edge;
bool pl_flag;
bool bittrue;
{
/* For inputs which are NOT constants, we have to read once every sample */

    int i = 0;
    char label[STRSIZE], value[STRSIZE];
    char *lowstr, *highstr, *strtok();
    int low, high;

if(!bittrue)
{
    if(IsArray(edge))
        fprintf(CFD,"ReadArray_");
    else
        fprintf(CFD,"Read_");
    GenEdgeName(edge);
    if(IsArray(edge)) {
        fprintf(CFD,"(");
	GenIndexedEdgeRef(edge);
	GenEdgeDelay(edge);
    } else {
        fprintf(CFD,"(&");
        GenSingleEdgeDeref(edge);
    }
    if(pl_flag)
    {
	if(IsArray(edge))
	fprintf(CFD,",r_%s_",Root->Name);
	else
	fprintf(CFD,",*r_%s_",Root->Name);
/* NEED & HERE? */
    	GenEdgeName(edge);
    }
    fprintf(CFD, ");\n");
} /* not bittrue */
else if (bittrue)
{
    if(IsArray(edge))
    {
	/* to get the size of the array */
    	sprintf(label, "index%d", i);
    	strcpy(value, (char *)GetAttribute(edge->Arguments, label));
	if(isnumber(value))
		high = atoi(value) +1;
	else
	{	
		lowstr = strtok(value, "..");
		highstr = strtok(NULL, "..");
		if (isnumber(highstr))
		high = atoi(highstr);
		low = atoi(lowstr);
		if(high<low) high = low;
	}

	fprintf(CFD,"for(%s_cnt=0; %s_cnt <=  %d; %s_cnt++)\n    {\n    ",
			Root->Name,Root->Name,high,Root->Name);
    	GenEdgeName(edge);
	fprintf(CFD,"[%s_cnt] = r_%s_",Root->Name,Root->Name);
	/*fprintf(CFD,"[%d - %s_cnt] = r_%s_",high,Root->Name,Root->Name);*/
    	GenEdgeName(edge);
	fprintf(CFD,"[%s_cnt];\n    }\n",Root->Name);
    }
    else
    {
    	GenSingleEdgeDeref(edge);
    	/*GenEdgeName(edge);*/
	fprintf(CFD," = *r_%s_",Root->Name);
    	GenEdgeName(edge);
	fprintf(CFD,";\n");
    }
} /* bittrue */

}

GenMinMaxInt(node, op)
NodePointer node;
char *op;
{
    EdgePointer in1, in2, out;

    in1 = EP(node->InEdges);
    in2 = EP(node->InEdges->Next);
    out = EP(node->OutEdges);
    GenOperand(out);
    fprintf(CFD, " = ");
    fprintf(CFD, "%s(", op);
    GenOperand(in1);
    fprintf(CFD, ", ");
    GenOperand(in2);
    fprintf(CFD,");\n");
}
GenBinaryInt(node, op)
NodePointer node;
char *op;
{
    EdgePointer in1, in2, out;

    in1 = EP(node->InEdges);
    in2 = EP(node->InEdges->Next);
    out = EP(node->OutEdges);
    GenOperand(out);
    fprintf(CFD, " = ");
    GenOperand(in1);
    fprintf(CFD, " %s ", op);
    GenOperand(in2);
    fprintf(CFD,";\n");
}

GenBinaryFixed(node, op)
NodePointer node;
char *op;
{
    EdgePointer in1, in2, out;
    char str[STRSIZE];

    in1 = EP(node->InEdges);
    in2 = EP(node->InEdges->Next);
    out = EP(node->OutEdges);
    fprintf(CFD, "Fix%s (", op);
    GenOperand(in1);  GenFixedType(in1); fprintf(CFD, ", "); 
    GenOperand(in2);  GenFixedType(in2); fprintf(CFD, ", "); 
    GenOperand(out);
    if (IsFixedType(out))
        GenFixedType(out);
    fprintf(CFD, ");\n"); 
}


CastAndGenFixed(node, op)
NodePointer node;
char *op;
{
    EdgePointer in1, in2, out;
    char str[STRSIZE];
    char* value;
    bool in1_changed, in2_changed;

    in1_changed = 0; in2_changed = 0;

    in1 = EP(node->InEdges);
    in2 = EP(node->InEdges->Next);
    out = EP(node->OutEdges);
 
/* constant and attribute value is not a number */
/* for now make the simple(wrong?) assumption that if both are 
in this class, they are iterators, and then types are fix<5,0> */

   if( IsConstantEdge(in1) && HasAttribute(in1->Attributes,"value")) 
   {
	value = (char*)GetAttribute(in1->Attributes,"value");
	if(!isnumber(value)) in1_changed = 1;
   }
   if( IsConstantEdge(in2) && HasAttribute(in2->Attributes,"value")) 
   {
	value = (char*)GetAttribute(in2->Attributes,"value");
	if(!isnumber(value)) in2_changed = 1;
   }

	if(in1_changed)
	{
		fprintf(CFD,"int_to_fix(");
		GenOperand(in1);
		fprintf(CFD,",&");
		fprintf(CFD,"in1_conv");
		if(!in2_changed) GenFixedType(in2);	
		else fprintf(CFD,",5,0");
		fprintf(CFD,");\n");
	}
	if(in2_changed)
	{
		fprintf(CFD,"int_to_fix(");
		GenOperand(in2);
		fprintf(CFD,",&");
		fprintf(CFD,"in2_conv");
		if(!in1_changed) GenFixedType(in1);	
		else fprintf(CFD,",5,0");
		fprintf(CFD,");\n");
	}
	if(!in1_changed && !in2_changed) GenBinaryFixed(node, op);
	else
	{
		Indent();
    		fprintf(CFD, "Fix%s (", op);
		if(in1_changed) fprintf(CFD, "in1_conv"); 
		else GenOperand(in1);
		if(!in1_changed) GenFixedType(in1); 
		else if(!in2_changed) GenFixedType(in2);
		else fprintf(CFD,",5,0");
		fprintf(CFD, ", "); 
		if(in2_changed) fprintf(CFD, "in2_conv"); 
    		else GenOperand(in2);  
		if(!in2_changed) GenFixedType(in2); 
		else if(!in1_changed) GenFixedType(in1);
		else fprintf(CFD,",5,0");
		fprintf(CFD, ", "); 
    		GenOperand(out);
    		if (IsFixedType(out))
        	GenFixedType(out);
    		fprintf(CFD, ");\n"); 
	}
}

GenUnaryInt(node, op)
NodePointer node;
char *op;
{
    EdgePointer in1, out;

    in1 = EP(node->InEdges);
    out = EP(node->OutEdges);
    GenOperand(out);
    fprintf(CFD, " = ");
    fprintf(CFD, " %s ", op);
    GenOperand(in1);
    fprintf(CFD,";\n");
}

GenUnaryFixed(node, op)
NodePointer node;
char *op;
{
    EdgePointer in1, out;

    in1 = EP(node->InEdges);
    out = EP(node->OutEdges);
    fprintf(CFD, "Fix%s (", op);
    GenOperand(in1);
    if (strcmp(op, "Negate") == 0) {
	fprintf(CFD, ", "); 
        GenOperand(out);
        GenFixedType(in1);
    } else {
        GenFixedType(in1);
	fprintf(CFD, ", "); 
        GenOperand(out);
        GenFixedType(out);
    }	 
    fprintf(CFD, ");\n"); 
}

GenBitSelect(node)
NodePointer node;
{
    int bitselect = FALSE;
    int bit, width;
    EdgePointer in,out;

    if (strcmp(node->Master->Name, BITSELECT) == 0)
	bitselect = TRUE;
    in = EP(node->InEdges);
    fprintf(CFD, "FixBitSelect (");
    GenOperand(in); GenFixedType(in); fprintf(CFD, ", ");
    if (HasAttribute(node->Arguments, "bit")) {
	bit = (int)GetAttribute(node->Arguments, "bit");
	fprintf(CFD, "%d, ", bit);
    } else {
	in = EP(node->InEdges->Next);
	if (IsFixedType(in)) 
	    fprintf(CFD, "f2i(");
        GenOperand(in); 
	if (IsFixedType(in)) {
	    GenFixedType(in);
	    fprintf(CFD, ")");
	}
	fprintf(CFD, ", ");
    }
    if (HasAttribute(node->Arguments, "width")) {
	width = (int)GetAttribute(node->Arguments, "width");
	fprintf(CFD, "%d, ", width);
    } else if (bitselect == FALSE && node->InEdges->Next == NULL) {
	fprintf(CFD, "1, ", width);
    } else {
	in = EP(node->InEdges->Next->Next);
	if (IsFixedType(in)) 
	    fprintf(CFD, "f2i(");
        GenOperand(in); 
	if (IsFixedType(in)) {
	    GenFixedType(in);
	    fprintf(CFD, ")");
	}
        fprintf(CFD, ", ");
    }
    out = EP(node->OutEdges);
    GenOperand(out); GenFixedType(out); fprintf(CFD, ");\n");
}

GenBitMerge(node)
NodePointer node;
{
   EdgePointer in1, in2, out;

   in1 = EP(node->InEdges);
   in2 = EP(node->InEdges->Next);
   out = EP(node->OutEdges);
   fprintf(CFD, "FixBitMerge (");
   GenOperand(in1); GenFixedType(in1); fprintf(CFD, ", ");
   GenOperand(in2); GenFixedType(in2); fprintf(CFD, ", ");
   GenOperand(out); GenFixedType(out); fprintf(CFD, ");\n");
}

GenDoLoopExit(node)
NodePointer node;
{
    EdgePointer in;

    in = EP(node->InEdges);
    fprintf(CFD, "if (");
    GenOperand(in);
    fprintf(CFD, " != 0) break;\n");
}

GenConst(node, value)
NodePointer node;
char *value;
{
    EdgePointer out;

    out = EP(node->OutEdges);
    GenOperand(out);
    fprintf(CFD, " = ");
    fprintf(CFD, "%s;\n", value);
}
	
GenAssignInt(node)
NodePointer node;
{
    EdgePointer in1, out;

    in1 = EP(node->InEdges);
    if (node->OutEdges != NULL)
        out = EP(node->OutEdges);
    else if (node->OutControl != NULL)
        out = EP(node->OutControl);
    else {
	Error("GenAssign() -- No output edges\n");
    }
    if (node->InEdges->Next == NULL) {
	GenOperand(out);
    } else {  /* generate dimension expressions  */
	Error("GenAssignInt() -- Too many input edges\n");
    }
    fprintf(CFD, " = ");
    GenOperand(in1);
    fprintf(CFD,";\n");
}

GenAssignFixed(node)
NodePointer node;
{
    EdgePointer in1, out;

    in1 = EP(node->InEdges);
    if (node->OutEdges != NULL)
        out = EP(node->OutEdges);
    else if (node->OutControl != NULL)
        out = EP(node->OutControl);
    else {
	Error("GenAssign() -- No output edges\n");
    }
    fprintf(CFD, "FixAssign (");
    GenOperand(in1);
    fprintf(CFD, ", ");
    if (node->InEdges->Next == NULL)
        GenOperand(out);
    else 
	Error("GenAssignFix() -- Too many input edges\n");
    fprintf(CFD,");\n");
}

GenWriteInt(node)
NodePointer node;
{
    EdgePointer in1, out;

    in1 = EP(node->InEdges);
    if (node->OutEdges != NULL)
        out = EP(node->OutEdges);
    else if (node->OutControl != NULL)
        out = EP(node->OutControl);
    else {
	Error("GenAssign() -- No output edges\n");
    }
    GenIndexedEdgesDeref(out, node);
    fprintf(CFD, " = ");
    GenOperand(in1);
    fprintf(CFD,";\n");
}

GenWriteFixed(node)
NodePointer node;
{
    EdgePointer in1, out;

    in1 = EP(node->InEdges);
    if (node->OutEdges != NULL)
        out = EP(node->OutEdges);
    else if (node->OutControl != NULL)
        out = EP(node->OutControl);
    else {
	    Error("GenAssign() -- No output edges\n");
    }
    fprintf(CFD, "FixAssign (");
    GenOperand(in1);
    fprintf(CFD, ", ");
    GenIndexedEdgesDeref(out, node);
    fprintf(CFD,");\n");
}

GenCastFixed(node)
NodePointer node;
{
    EdgePointer in1, out;

    in1 = EP(node->InEdges);
    out = EP(node->OutEdges);
    fprintf(CFD, "FixCast (");
    GenOperand(in1);  GenFixedType(in1); fprintf(CFD, ", ");
    GenOperand(out);  GenFixedType(out); fprintf(CFD, ");\n");
}

GenUpSample(node)
NodePointer node;
{
    EdgePointer in, out;
    char *range;
    int scale, phase, lb, ub;

    scale = (int)GetAttribute(node->Arguments, "Scale");
    phase = (int)GetAttribute(node->Arguments, "Phase");
    out = EP(node->OutControl);
    range = (char *)GetAttribute(out->Arguments, "index0");
    ExtractRange(range, &lb, &ub);
    fprintf(CFD, "for (i = 0;i <= %d; i++)\n", ub);
    Indent(); Indent();
    GenEdgeName(out);
    fprintf(CFD, "[i] = (i%%%d == %d) ? ", scale, phase);
    if (node->InControl != NULL) {
        in = EP(node->InControl);
	GenEdgeName(in);
        fprintf(CFD, "[i/%d] : 0;\n", scale);
    } else {
        in = EP(node->InEdges);
	GenOperand(in); fprintf(CFD, " : 0;\n");
    }
}

GenDownSample(node)
NodePointer node;
{
    EdgePointer in, out;
    char *range;
    int scale, phase, lb, ub;

    scale = (int)GetAttribute(node->Arguments, "Scale");
    phase = (int)GetAttribute(node->Arguments, "Phase");
    in = EP(node->InControl);
    if (node->OutControl != NULL) {
        out = EP(node->OutControl);
        range = (char *)GetAttribute(out->Arguments, "index0");
        ExtractRange(range, &lb, &ub);
        fprintf(CFD, "for (i = 0; i<= %d; i++)\n", ub);
        Indent(); Indent();
	GenEdgeName(out);
        fprintf(CFD, "[i] = ");
	GenEdgeName(in);
        fprintf(CFD, "[%d*i+%d];\n", scale, phase);
    } else {
        out = EP(node->OutEdges);
        GenOperand(out);
        fprintf(CFD, " = ");
	GenEdgeName(in);
        fprintf(CFD, "[%d];\n", phase);
    }
}

GenInterpolate(node)
NodePointer node;
{
    EdgePointer in, out;
    char *range, *inname, *outname;
    int scale, phase, lb, ub;

    scale = (int)GetAttribute(node->Arguments, "Scale");
    phase = (int)GetAttribute(node->Arguments, "Phase");
    out = EP(node->OutControl);
    range = (char *)GetAttribute(out->Arguments, "index0");
    ExtractRange(range, &lb, &ub);
    fprintf(CFD, "for (i = 0;i <= %d; i++)\n", ub);
    Indent(); Indent();
    GenEdgeName(out);
    fprintf(CFD, "[i] = (i%%%d == %d) ? ", scale, phase);
    if (node->InControl != NULL) {
        in = EP(node->InControl);
	GenEdgeName(in);
        fprintf(CFD, "[i/%d] : 0;\n", scale);
    } else {
        in = EP(node->InEdges);
	GenOperand(in); fprintf(CFD, " : 0;\n");
    }
    /* now interpolate, doesn't work, don't have last point to interpolate. */
/*    Indent();
    fprintf(CFD, "for (i = 0;i <= %d; i++) {\n", ub);
    Indent();
    fprintf(CFD, "int j;\n");
    Indent(); Indent();
    fprintf(CFD, "j = i%%%d;\n", scale);
    Indent(); Indent();
    fprintf(CFD, "%s[i] = (j/%d)*%s[i+%d-j] + (1-j/%d)*%s[i-j];\n", outname,
		  scale, outname, scale, scale, outname);
    Indent(); fprintf(CFD, "}\n\n"); */
}

GenDecimate(node)
NodePointer node;
{
    EdgePointer in, out;
    char *range;
    int scale, phase, lb, ub;

    scale = (int)GetAttribute(node->Arguments, "Scale");
    phase = (int)GetAttribute(node->Arguments, "Phase");
    in = EP(node->InControl);
    if (node->OutControl != NULL) {
        out = EP(node->OutControl);
        range = (char *)GetAttribute(out->Arguments, "index0");
        ExtractRange(range, &lb, &ub);
        fprintf(CFD, "for (i = 0; i<= %d; i++)\n", ub);
        Indent(); Indent();
	GenEdgeName(out);
        fprintf(CFD, "[i] = ");
	GenEdgeName(in);
        fprintf(CFD, "[%d*i+%d];\n", scale, phase);
    } else {
        out = EP(node->OutEdges);
        GenOperand(out);
        fprintf(CFD, " = ");
	GenEdgeName(in);
        fprintf(CFD, "[%d];\n", phase);
    }
}

GenTimeMux(node)
NodePointer node;
{
    EdgePointer in, out;
    ListPointer Ptr;
    char *range;
    int lb, ub, cnt, offset;

    out = EP(node->OutControl);
    range = (char *)GetAttribute(out->Arguments, "index0");
    ExtractRange(range, &lb, &ub);
    for (Ptr = node->InControl, cnt = 0; Ptr != NULL; Ptr = Ptr->Next, cnt++);
    if (cnt == 0) {   /* multiple inedges, do each individually */ 
        for(Ptr = node->InEdges,offset = 0; Ptr != NULL; Ptr = Ptr->Next,offset++) {
	    Indent();
	    GenEdgeName(out);
	    fprintf(CFD, "[%d] = ", offset);
	    GenOperand(EP(Ptr));
	    fprintf(CFD, ";\n");
        }
    } else if (cnt == 1) {            /* control inedge, do loop */
        Indent();
        in = EP(node->InControl);
        fprintf(CFD, "for (i = 0; i<= %d; i++)\n", ub);
        Indent(); Indent();
	GenEdgeName(out);
        fprintf(CFD, "[i] = ");
	GenEdgeName(in);
        fprintf(CFD, "[i];\n");
    } else {     /* multiple array edges */
	for (Ptr=node->InControl,offset=0;Ptr != NULL;Ptr=Ptr->Next,offset++) {
	    in = EP(Ptr);
            range = (char *)GetAttribute(in->Arguments, "index0");
            ExtractRange(range, &lb, &ub);
	    Indent();
	    fprintf(CFD, "for (i = 0; i<= %d; i++)\n", ub);
            Indent(); Indent();
	    GenEdgeName(out);
            fprintf(CFD, "[%d*i+%d] = ", cnt, offset);
	    GenEdgeName(in);
            fprintf(CFD, "[i];\n");
	}
    }
}

GenTimeDeMux(node)
NodePointer node;
{
    EdgePointer in, out;
    ListPointer Ptr;
    char *range;
    int lb, ub, cnt, offset;

    in = EP(node->InControl);
    range = (char *)GetAttribute(in->Arguments, "index0");
    ExtractRange(range, &lb, &ub);
    for (Ptr = node->OutControl, cnt = 0; Ptr != NULL; Ptr = Ptr->Next, cnt++);
    if (cnt == 0) {  /* multiple outedges, do each individually */
        for(Ptr = node->OutEdges; Ptr != NULL; Ptr = Ptr->Next) {
            out = EP(Ptr);
	    offset = (int)GetAttribute(out->Attributes, "offset");
	    Indent();
	    GenOperand(out);
	    fprintf(CFD, "= ");
	    GenEdgeName(in);
	    fprintf(CFD, "[%d];\n", offset);
        }
    } else if (cnt == 1) {            /* control outedge, do loop */
	Indent();
        out = EP(node->OutControl);
        fprintf(CFD, "for (i = 0; i<= %d; i++)\n", ub);
        Indent(); Indent();
	GenEdgeName(out);
        fprintf(CFD, "[i] = ");
	GenEdgeName(in);
        fprintf(CFD, "[i];\n");
    } else {   /* multiple array out edges */
       for (Ptr=node->OutControl; Ptr != NULL; Ptr=Ptr->Next) {
            out = EP(Ptr);
	    offset = (int)GetAttribute(out->Attributes, "offset");
            range = (char *)GetAttribute(out->Arguments, "index0");
            ExtractRange(range, &lb, &ub);
            Indent();
            fprintf(CFD, "for (i = 0; i<= %d; i++)\n", ub);
            Indent(); Indent();
	    GenEdgeName(out);
            fprintf(CFD, "[i] = ");
	    GenEdgeName(in);
            fprintf(CFD, "[%d*i+%d];\n", cnt, offset);
        }
    }
}

GenOperand(edge)
EdgePointer edge;
{
    bool IsDelayOut();
    NodePointer Node;

    if (HasAttribute(edge->Attributes, "readout")) {
        Node = (NodePointer)GetAttribute(edge->Attributes, "readout");
	GenIndexedEdgesDeref(edge, Node);
    }
    else if (IsArray(edge))
	    GenIndexedEdgeDeref(edge);
    else if (IsDelayOut(edge))
	    GenDelayEdgeRef(edge);
    else
	    GenSingleEdgeDeref(edge);
}

GenIndexedEdgeDeref(edge)
EdgePointer edge;
{
    GenIndexedEdgeRef(edge);
    GenEdgeDelay(edge);
    GenEdgeDim(edge);
}

GenIndexedEdgesDeref(edge, node)
EdgePointer edge;
NodePointer node;
{
    GenIndexedEdgeRef(edge);
    GenEdgeDelay(edge);
    GenEdgeDimFromNode(node);
}

GenIndexedEdgeRef(edge)
EdgePointer edge;
{
    if (IsDelayIO(edge))
	    fprintf(CFD, "pST->");
    GenEdgeName(edge);
}

/* for generating constant indices extracted from arguments of output edge */
GenEdgeDim(edge)
EdgePointer edge;
{
    char label[STRSIZE], value[STRSIZE];
    int i = 0;

    sprintf(label, "index%d", i);
    while(HasAttribute(edge->Arguments, label)) {
        strcpy(value, (char *)GetAttribute(edge->Arguments, label));
        if (isnumber(value))
            fprintf(CFD, "[%d]", atoi(value));
        else 
            fprintf(CFD, "[]");
        i++;
        sprintf(label, "index%d", i);
    }
}

/* for generating indices extracted from names of index edges */
GenEdgeDimFromNode(node)
NodePointer node;
{
    register ListPointer ptr, IndexEdges;
    register EdgePointer edge;
    char label[STRSIZE], value[STRSIZE];
    int i = 0;

    IndexEdges = node->InEdges;
    if (strcmp(node->Master->Name, WRITE) == 0)
        IndexEdges = node->InEdges->Next;
    if (IndexEdges == NULL) { /* Constant Index */
        sprintf(label, "index%d", i);
        while(HasAttribute(node->Arguments, label)) {
            strcpy(value, (char *)GetAttribute(node->Arguments, label));
            if (isnumber(value))
                fprintf(CFD, "[%d]", atoi(value));
            else
                fprintf(CFD, "[]");
            i++;
            sprintf(label, "index%d", i);
        }
    } else {
        for (ptr = IndexEdges; ptr != NULL; ptr = ptr->Next) {
	    edge = EP(ptr);
	    fprintf(CFD, "[");
	    if (IsFixedType(edge))
	        fprintf(CFD, "f2i(");
	    GenOperand(edge);
	    if (IsFixedType(edge)) {
		GenFixedType(edge);
	        fprintf(CFD, ")");
            }
	    fprintf(CFD, "]");
        }
    }
}

GenEdgeDelay(edge)
EdgePointer edge;
{
    int delay;

    if (HasAttribute(edge->Attributes, "Maxdelay")) {
	fprintf(CFD, "[pST->");
	GenEdgeName(edge);
	fprintf(CFD, "_C]");
    } else if (HasAttribute(edge->Attributes, "Maxloopdelay")) {
	fprintf(CFD, "[");
	GenEdgeName(edge);
	fprintf(CFD, "_C]");
    }
}

GenShiftInt(node, op)
NodePointer node;
char *op;
{
    EdgePointer in1, shift, out;

    in1 = EP(node->InEdges);
    out = EP(node->OutEdges);
    GenOperand(out);
    fprintf(CFD, " = ");
    GenOperand(in1);
    fprintf(CFD, " %s ", op);
    if (node->InEdges->Next == NULL || node->InEdges->Next->Type == NullNode) {
	    fprintf(CFD, "%d", (int) GetAttribute(node->Arguments, "shift"));
    }
    else {  /* generate shift expressions  */
        shift = EP(node->InEdges->Next);
	GenSingleEdgeDeref(shift);
    }
    fprintf(CFD,";\n");
}

GenShiftFixed(node, op)
NodePointer node;
char *op;
{
    EdgePointer in1, shift, out;

    in1 = EP(node->InEdges);
    out = EP(node->OutEdges);
    fprintf(CFD, "Fix%s (", op);
    GenOperand(in1);
    fprintf(CFD, ", ");
    GenOperand(out);
    GenFixedType(in1);
    fprintf(CFD, ", ");
    if (node->InEdges->Next == NULL || node->InEdges->Next->Type == NullNode) {
	    fprintf(CFD, "%d", (int) GetAttribute(node->Arguments, "shift"));
    }
    else {  /* generate shift expressions  */
        shift = EP(node->InEdges->Next);
	if (IsFixedType(shift))
	    fprintf(CFD, "f2i(");
	GenSingleEdgeDeref(shift);
	if (IsFixedType(shift)) {
	    GenFixedType(shift);
	    fprintf(CFD, ")");
	}     
    }
    fprintf(CFD,");\n");
}

GenMux(node)
NodePointer node;
{
    EdgePointer cond, in1, in2 = NULL, out;

    cond = EP(node->InEdges);
    in1 = EP(node->InEdges->Next);
    if (node->InEdges->Next->Next != NULL && node->InEdges->Next->
						Next->Type == EdgeNode) {
	in2 = EP(node->InEdges->Next->Next);
    }
    out = EP(node->OutEdges);

    fprintf(CFD, "if (");
    GenSingleEdgeDeref(cond);
    fprintf(CFD, ") ");
    OpenBraces();

/* Do Integer code generation, if ELSE is NULL, don't print else field */
    if (IsIntType(in1) && IsIntType(in2)) {
	Indent();
        GenSingleEdgeDeref(out);
        fprintf(CFD, " = ");
        GenSingleEdgeDeref(in1);
	fprintf(CFD, ";\n");  CloseBraces();
	if (in2) {
	    Indent();  fprintf(CFD, "else ");  OpenBraces();
	    Indent();
            GenOperand(out);
            fprintf(CFD, " = ");
	    GenOperand(in2);
            fprintf(CFD, ";\n"); CloseBraces();
	}
    }
    else if (IsFixedType(in1) && IsFixedType(in2)) {
	Indent();
	fprintf(CFD, "FixAssign (");
        GenOperand(in1);
	fprintf(CFD, ", ");
        GenOperand(out);
	fprintf(CFD, ");\n");  CloseBraces();
	if (in2) {
	    Indent();  fprintf(CFD, "else ");  OpenBraces();
	    Indent();
	    fprintf(CFD, "FixAssign (");
            GenOperand(in2);
	    fprintf(CFD, ", ");
            GenOperand(out);
            fprintf(CFD, ");\n"); CloseBraces();
	}
    }
    else
	    printmsg(NULL, "GenMux -- Mismatch types\n");
}

/*
 * Routines for spacing statements.
 */

static int IndexCount = 1;
static char indent[] = "    ";

Indent()
{
    register int i;
 
    for (i=0; i < IndexCount; i++)
	    fprintf(CFD, "%s", indent);
}

OpenBraces()
{
    fprintf(CFD, "{\n");
    IndexCount++;
}

CloseBraces()
{
    IndexCount--;
    Indent();
    fprintf(CFD, "}\n");
}
