static const char file_id[] = "distributions.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "distributions.h"
#include "dist_template.h"
#include "MultiTarget.h"
#include "IntState.h"
#include "FloatState.h"
#include "StringState.h"
#include "StringList.h"

/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.
							COPYRIGHTENDKEY

 Programmer:  Soonhoi Ha

 Methods for distributions.  If a user wants to add a new type, he/she
 must adjust "NUM" and "defType".

**************************************************************************/

	///////////////////////////
	// Methods for geometric //
	///////////////////////////

DistBase* DistGeometric :: copy() { LOG_NEW; return new DistGeometric; }
const char* DistGeometric :: myType() const { return "DistGeometric"; }

// parameter setting for geometric distribution.
int DistGeometric :: setParams(Galaxy* g, MultiTarget* tg) {

	// fetch the parameters from the target state.
	IntState* ist = (IntState*) tg->galParam(g,"paramMin");
	if (!ist) { 
		// default value is 0.
		min = 0;
	} else {   min = int(*ist);
		if (min < 0 ) return FALSE;
	}
	
	FloatState* fst = (FloatState*) tg->galParam(g,"paramGeo");
	if (!fst) {
		paramInfo();
		// default value is 0.5
		p = 0.5;
	} else  { 
		p = double(*fst);
		if (p > 1.0 || p < 0) return FALSE;
	}
	return TRUE;
}

double DistGeometric :: assumedValue() {

	// calculate the average
	double avg = p / (1 - p);
	
	// We multiply this average by 1.5 (magic number).
	return (double(min) + avg * 1.5);
}

void DistGeometric :: paramInfo() {
	StringList out;
	out += "parameters are (1) paramMin (default 0) - min value\n";
	out += "and (2) paramGeo (default 0.5) - prob(continue)\n";
	out += "paramGeo can be used to specify the TRUE probability in ";
	out += "If-then-else construct.\n";
	Error::message(out);
}

StringList DistGeometric :: printParams() {
	StringList out;
	out += "<GEOMETRIC distribution> \nparamMin = ";
	out += min;
	out += ",  paramGeo = ";
	out += p;
	out += "\n";
	return out;
} 

	/////////////////////////
	// Methods for uniform //
	/////////////////////////

DistBase* DistUniform :: copy() { LOG_NEW; return new DistUniform; }
const char* DistUniform :: myType() const { return "DistUniform"; }

// parameter setting for uniform distribution.
int DistUniform :: setParams(Galaxy* g, MultiTarget* tg) {

	// fetch the parameters from the target state.
	IntState* ist = (IntState*) tg->galParam(g,"paramMin");
	if (!ist) {
		// default value is 1
		min = 1;
	} else {
		min = int(*ist);
		if (min < 0 ) return FALSE;
	}
	
	IntState* jst = (IntState*) tg->galParam(g,"paramMax");
	if (!jst) {
		paramInfo();
		// default value is 10
		max = 10;
	} else {
	 	max = int(*jst);
		if (max < 0) return FALSE;
	}
	return TRUE;
}

double DistUniform :: assumedValue() {
	return (double(min + max) / 2.0);
}

void DistUniform :: paramInfo() {
	StringList out;
	out += "parameters are (1) paramMin (default 1) - min value\n";
	out += "and (2) paramMax (default 10) - max value.\n";
	Error::message(out);
}

StringList DistUniform :: printParams() {
	StringList out;
	out += "<UNIFORM distribution> \nparamMin = ";
	out += min;
	out += ",  paramMax = ";
	out += max;
        out += "\n";
	return out;
}

	/////////////////////////
	// Methods for general //
	/////////////////////////

DistBase* DistGeneral :: copy() { LOG_NEW; return new DistGeneral; }
const char* DistGeneral :: myType() const { return "DistGeneral"; }

char* DEFAULT_FILE = "_defParams";

// parameter setting for general distribution.
int DistGeneral :: setParams(Galaxy* g, MultiTarget* tg) {

	// fetch the parameters from the target state.
	StringState* st = (StringState*) tg->galParam(g,"paramFile");
	if (!st) {
		paramInfo();
		// default file is "defParams".
		name = DEFAULT_FILE;
	} else    name = st->initValue();
	
	return makeTable();
}

void DistGeneral :: paramInfo() {
	StringList out;
	out += "parameter is (1) paramFile(default \"defParams\") - file name\n";
	Error::message(out);
}

StringList DistGeneral :: printParams() {
	StringList out;
	out += "<GENERAL distribution> \nparamFile = ";
	out += name;
        out += "\n";
	return out;
}

double DistGeneral :: assumedValue() {

	double sum = 0;
	for (int i = 0; i < size; i++) {
		sum += double(table[i].index) * table[i].value;
	}
	return sum;
}

DistGeneral :: ~DistGeneral() {
	LOG_DEL; delete [] table;
	table = 0;
}
