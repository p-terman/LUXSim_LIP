////////////////////////////////////////////////////////////////////////////////
//	
//	LUXSimLZDetector.hh
//
//	This is the header file to define the LZ detector as defined in the DOE and
//	NSF proposals circa May 2012.
//
////////////////////////////////////////////////////////////////////////////////
//
//	Change log
//
//	26 May 2012 - Initial submission (Kareem)
//   3 Mar 2014 - Fixed the constructor parameters to work with the grid on/off
//                specification (Kareem)
//
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimLZDetector_HH
#define LUXSimLZDetector_HH 1

//
//      GEANT4 includes
//
#include "globals.hh"

//
//      LUXSim includes
//
#include "LUXSimDetector.hh"

//
//      Class forwarding
//
class G4LogicalVolume;
class LUXSimDetectorComponent;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimLZDetector : public LUXSimDetector
{
	public:
	
		LUXSimLZDetector( G4double, G4String );
		~LUXSimLZDetector();
		
  //inline G4double GetHeightOffset() { return detectorHeightOffset; };
		
	private:

		LUXSimMaterials *luxMaterials;
		
		G4double detectorHeightOffset;

};

#endif
