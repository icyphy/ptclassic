#ifndef _DEDynMapBase_h
#define _DEDynMapBase_h 1
// header file generated from DEDynMapBase.pl by ptlang

#ifdef __GNUG__
#pragma interface
#endif

/*
copyright (c) 1997 Dresden University of Technology, WiNeS-Project              All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */
#include "checkConnect.h"
#include "MutableCQScheduler.h"
#include "CQScheduler.h"
#include "DEScheduler.h"
#include "DERepeatStar.h"
#include "DEDynMerge.h"
#include "DETarget.h"
#include "KnownBlock.h"
#include "InfString.h"
#include "DEStar.h"
#include "StringState.h"
#include "StringArrayState.h"
# line 168 "DEDynMapBase.pl"
struct blockCounter {
            int blockName;
            Block *myBlock_p;
            blockCounter *next;
        };

class DEDynMapBase : public DEStar
{
public:
	DEDynMapBase();
	/* virtual*/ const char* className() const;
	/* virtual*/ int isA(const char*) const;
protected:
	/* virtual */ void setup();
	StringState blockname;
	StringState where_defined;
	StringArrayState output_map;
	StringArrayState input_map;
	StringArrayState parameter_map;
# line 153 "DEDynMapBase.pl"
Galaxy* mom;
        DEBaseSched *myScheduler_p;
        blockCounter *list_h;
        static int nameCounter;
        StringList name;
	GenericPort* findTopGenericPort (PortHole *ph);
	void connectError ();
	virtual void connectInputs (Block*, int) = 0;
	virtual void connectOutputs (Block*) = 0;
	virtual void disconnectInputs (Block*) = 0;
	virtual void disconnectOutputs (Block*) = 0;
	virtual void callStartMethods (Block*);
	virtual void callStopMethods (Block*, double);
	Galaxy* myParent ();
	void connectPorts (PortHole* dest, PortHole* source);
	void deleteDynBlock (int delName);
	virtual Block * createBlock ();
	void createDynBlock (int number);
	void scanGalaxyForSources (Galaxy& myGalaxy);
	Block * scanGalaxyForBlockWithName (Galaxy& myGalaxy, const char *itsName);
	void deletePendingEventsOfPortHole (DEPortHole *myPortHole_p);
	void clearList ();
	void deleteList ();

private:
# line 160 "DEDynMapBase.pl"
PortHole *po;
        blockCounter *blockCounter_p, *list_t;
        Block *delBlock_p;
        DEScheduler *myDEScheduler_p;
        CQScheduler *myCQScheduler_p;
        MutableCQScheduler *myMutCQScheduler_p;
	void setParameter (Block *block_p);
	void makeStarsMutable (Block *block_p);
	void deletePendingEventsOfStar (Star *myStar_p);
	void scanGalaxyForContents (Galaxy& myGalaxy);
	void scanEventList (Block *myBlock_p);

};
#endif
