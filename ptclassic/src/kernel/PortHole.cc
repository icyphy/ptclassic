#include "Connect.h"
#include "Block.h"
#include "StringList.h"
#include "Output.h"
 
// SCCS version identification
// $Id$

extern Error errorHandler;
extern PlasmaList plasmaList;

PortHole& PortHole :: setPort(char* s,
                              Block* parent,
                              dataType t = FLOAT) {
        name = s;
        alias = NULL;
        myGeodesic = NULL;
	myPlasma = plasmaList.getPlasma(t);
        type = t;
        blockIamIn = parent;
        farSidePort = NULL;
	currentParticle = NULL;
        return *this;
}

PortHole& SDFPortHole :: setPort (char* s,
                             Block* parent,
                             dataType t = FLOAT,
                             unsigned numTokens = 1) {
        PortHole::setPort(s,parent,t);
        numberTokens = numTokens;
        return *this;
}

PortHole :: operator char* () {
        StringList out;
        char* parentBlock;

        // Get the name of the parent block, if there is one
        if( blockIamIn != NULL)
                parentBlock = blockIamIn->readFullName();
        else
                parentBlock = "null (no block)";
 
        if(isItInput())
           out = "      Input ";
        else if(isItOutput())
           out = "      Output ";
        
        out += "PortHole: ";
        out += parentBlock;
        out += ".";
        out += readName();
        out += "\n";
        
        if(alias != NULL) {
           PortHole& eventualAlias = realPort();
           out += "       Aliased to: ";
           out += eventualAlias.blockIamIn->readFullName();
           out += ".";
           out += eventualAlias.readName();
           out += "\n";
        } else {
        
           if (farSidePort != NULL) {
              out += "    Connected to port: ";
              out += farSidePort->blockIamIn->readFullName();
              out += ".";
              out += farSidePort->readName();
              out += "\n";
              }
           else
              out += "    Not connected.\n";
        }
        return out;
}

Particle& PortHole ::  operator % (int i)
{
        if(i != 0) errorHandler.error(
                "Attempt to access PortHole with delay"
		);
        else if(currentParticle == NULL) errorHandler.error(
                "Attempt to access nonexistent input Particle"
		);
        return *currentParticle;
}

void InSDFPort :: increment(int numberTimes)
{
        // CAUTION: currently we assume numberTime=1
        if(currentParticle != NULL)
                // Put current Particle back into Plasma 
                myPlasma->put(currentParticle);
 
        currentParticle = myGeodesic->get();
}

void OutSDFPort :: increment(int numberTimes)
{
	// CAUTION: currently we assume numberTime=1
        currentParticle = myPlasma->get();
        myGeodesic->put(currentParticle);
}

Particle& InOtherPort :: operator ++ ()
{
	if(currentParticle != NULL)
		// Put current Particle back into Plasma
		myPlasma->put(currentParticle);

	currentParticle = myGeodesic->get();
	return *currentParticle;
}
 
Particle& InOtherPort ::  operator -- ()
{
	if(currentParticle == NULL) errorHandler.error(
		"Attempt to push back nonexistent Particle"
		);
	else
		myGeodesic->pushBack(currentParticle);
}


Particle& OutOtherPort :: operator ++ ()
{
	currentParticle = myPlasma->get();
	myGeodesic->put(currentParticle);
	return *currentParticle;
}

MultiPortHole& MultiPortHole :: setPort(char* s,
                              Block* parent,
                              dataType t = FLOAT) {
        name = s;
        type = t;
        alias = NULL;
        blockIamIn = parent;
        return *this;
}

MultiPortHole :: operator char* () {
        StringList out;
        char* parentBlock;
        
        // Get the name of the parent block, if there is one
        if( blockIamIn != NULL)
                parentBlock = blockIamIn->readFullName();
        else
                parentBlock = "null (no block)";
        
        if(isItInput())
           out = "      Input ";
        else if(isItOutput())
           out = "      Output ";
        
        out += "MultiPortHole: ";
        out += parentBlock;
        out += ".";
        out += readName();
        out += "\n";
 
        // TO BE DONE:
        // Probably want to peruse ports in the MultiPort here,
        // using the char* cast of each to get its information.
        
        return out;
}
        
PortHole& MultiPortHole :: newPort() {
        PortHole* newport = new PortHole;
        ports.put(*newport);
        blockIamIn->addPort(newport->setPort(readName(), blockIamIn, type));
        return *newport;
}
 
 
MultiPortHole& MultiSDFPort :: setPort (char* s,
                             Block* parent,
                             dataType t = FLOAT,
                             unsigned numTokens = 1) {
        MultiPortHole::setPort(s,parent,t);
        numberTokens = numTokens;
        return *this;
}
 
PortHole& MultiSDFInPort :: newPort () {
        InSDFPort* newport = new InSDFPort;
        ports.put(*newport);
        blockIamIn->
            addPort(newport->
                        setPort(readName(), blockIamIn, type, numberTokens));
        return *newport;
}
 
 
PortHole& MultiSDFOutPort :: newPort () {
        OutSDFPort* newport = new OutSDFPort;
        ports.put(*newport);
        blockIamIn->
            addPort(newport->
                        setPort(readName(), blockIamIn, type, numberTokens));
        return *newport;
}

Particle* Geodesic::get()
{
	if(size() == 0) errorHandler.error(
		"Geodesic: Attempt to access empty Geodesic"
		);
	return (Particle*)popTop();
}
