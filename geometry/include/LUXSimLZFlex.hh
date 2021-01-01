////////////////////////////////////////////////////////////////////////////////
//	
//	LUXSimLZFlex.hh
//
//	This is the header file to define an LZ-like detector that sits inside the
//	current LUX water tank. It has a total liquid mass set by a messenger
//	command, but with a default mass of 10 tonnes. The liquid xenon volume is a
//	right cylinder with diameter = height. All encompassing volume IDs and ODs
//	are set according to the requested volume mass, although the wall
//	thicknesses are always the same.
//
////////////////////////////////////////////////////////////////////////////////
//
//	Change log
//
//	24 Feb 2012 - Initial submission (Kareem)
//					
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimLZFlex_HH
#define LUXSimLZFlex_HH 1

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
class LUXSimLZFlex : public LUXSimDetector
{
	public:
	
		LUXSimLZFlex( G4double, G4double, G4double, G4double );
		~LUXSimLZFlex();
		
	private:

		LUXSimMaterials *luxMaterials;

};

#endif
