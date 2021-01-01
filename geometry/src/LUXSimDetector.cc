////////////////////////////////////////////////////////////////////////////////
/*	LUXSimDetector.cc
*
* This is the code file to define the LUXSim detector. This "detector" is just
* the detector part, it does not include the water shield or the rock.
*
********************************************************************************
* Change log
*	16 March 2009 - Initial submission (Kareem)
*	13 Sep 2010 - Added a hook to the LUXSimManager singleton (Kareem)
*
*/
////////////////////////////////////////////////////////////////////////////////

//
//	GEANT4 includes
//
#include "G4LogicalVolume.hh"

//
//	LUXSim includes
//
#include "LUXSimDetector.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					LUXSimDetector()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimDetector::LUXSimDetector()
{
	luxManager = LUXSimManager::GetManager();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					~LUXSimDetector()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimDetector::~LUXSimDetector() {}
