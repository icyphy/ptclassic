static const char file_id[] = "DEWeighted_List.pl";
// .cc file generated from DEWeighted_List.pl by ptlang
/*
copyright (c) 1997 Dresden University of Technology,
                    Mobile Communications Systems
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "DEWeighted_List.h"

const char *star_nm_DEWeighted_List = "DEWeighted_List";

const char* DEWeighted_List :: className() const {return star_nm_DEWeighted_List;}

ISA_FUNC(DEWeighted_List,DEStar);

Block* DEWeighted_List :: makeNew() const { LOG_NEW; return new DEWeighted_List;}

DEWeighted_List::DEWeighted_List ()
{
	setDescriptor("An input value is read in. Depending on its value one or more \n        counters in a list are increased. The outputs are the contents of the \n        counters, weighted by the number of all incoming values. Thus, the\n        probability that an incoming value is smaller than a specific value is \n        given out for all values in the list. Note that all outputs have the \n        same timestamp.");
	addPort(CIR.setPort("CIR",this,FLOAT));
	addPort(testx.setPort("testx",this,FLOAT));
	addPort(testy.setPort("testy",this,FLOAT));


}

void DEWeighted_List::setup() {
# line 39 "DEWeighted_List.pl"
number = 0;
            how_many_outputs = 20;
            for (int i = 1; i <= how_many_outputs; i++) liste[i] = 0;
}

void DEWeighted_List::go() {
# line 44 "DEWeighted_List.pl"
completionTime = arrivalTime;
            value = CIR.get();
            if (value < 5.0) liste[1]++;
            if (value < 10.0) liste[2]++;
            if (value < 15.0) liste[3]++;
            if (value < 20.0) liste[4]++;
            if (value < 25.0) liste[5]++;
            if (value < 30.0) liste[6]++;
            if (value < 35.0) liste[7]++;
            if (value < 40.0) liste[8]++;
            if (value < 45.0) liste[9]++;
            if (value < 50.0) liste[10]++;
            if (value < 55.0) liste[11]++;
            if (value < 60.0) liste[12]++;
            if (value < 65.0) liste[13]++;
            if (value < 70.0) liste[14]++;
            if (value < 75.0) liste[15]++;
            if (value < 80.0) liste[16]++;
            if (value < 85.0) liste[17]++;
            if (value < 90.0) liste[18]++;
            if (value < 95.0) liste[19]++;
            if (value < 100.0) liste[20]++;
            number++;
            for (int i = 1; i <= 20; i++) {
                dummy = (float) ((float) liste[i] / (float) number);
                dummy *= 100.0;
                testx.put(completionTime) << (float)(5*i);
                testy.put(completionTime) << (float)(dummy);
            };
}

// prototype instance for known block list
static DEWeighted_List proto;
static RegisterBlock registerBlock(proto,"Weighted_List");
