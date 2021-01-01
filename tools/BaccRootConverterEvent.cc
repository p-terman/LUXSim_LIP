////////////////////////////////////////////////////////////////////////////////
/*	BaccRootConverterEvent.cc
*
* This file defines the BaccRootConverterEvent class used by the
* BaccRootConverter code.
*
********************************************************************************
* Change log
*	23 Feb 2016 - Initial submission (Kareem Kazkaz)
*/
////////////////////////////////////////////////////////////////////////////////

#include "BaccRootConverterEvent.hh"
#include "BaccRootConverterEvent_dict.h"

using namespace std;

BaccRootConverterEvent::BaccRootConverterEvent() {
	ClearEverything();
}


void BaccRootConverterEvent::ClearEverything() {
    iEventNumber = -1;
    primaryParticles.clear();
    tracks.clear();
    volumes.clear();
};
