//	LUXSimPyramid.hh
//
//	This is the header file to define the inverted pyramid.
//
// 08 Mar 2012 - Created (Dave)

#ifndef LUXSimPyramid_HH
#define LUXSimPyramid_HH 1

//
//	GEANT4 includes
//
#include "globals.hh"

//
//	LUXSim includes
//
#include "LUXSimDetector.hh"

//
//	Class forwarding
//
class G4LogicalVolume;
class LUXSimDetectorComponent;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimPyramid : public LUXSimDetector
{
		
	public:
		LUXSimPyramid();
		~LUXSimPyramid();
		
		inline G4double GetHeight() { return height; };
		
	private:
		G4double height;
};

#endif
