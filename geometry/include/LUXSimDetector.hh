////////////////////////////////////////////////////////////////////////////////
/*	LUXSimDetector.hh
*
* This is the header file to define the LUXSim detector. This "detector" is just
* the detector part, it does not include the water shield or the rock.
*
********************************************************************************
* Change log
*	16 March 2009 - Initial submission (Kareem)
*	13 Sep 2010 - Added a hook to the LUXSimManager singleton (Kareem)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimDetector_HH
#define LUXSimDetector_HH 1

//
//	LUXSim includes
//
#include "LUXSimManager.hh"

//
//	Class forwarding
//
class G4LogicalVolume;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimDetector
{
	public:
		LUXSimDetector();
		~LUXSimDetector();
		
		inline G4LogicalVolume *GetLogicalVolume() { return logicalVolume; };
		
	protected:
		G4LogicalVolume *logicalVolume;
		
		LUXSimManager *luxManager;
};

#endif
