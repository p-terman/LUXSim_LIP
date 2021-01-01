////////////////////////////////////////////////////////////////////////////////
//	
//	LUXSimLZSimple.hh
//
//	This is the header file to define a simple LZ geometry. See the code file
//	for more details.
//
////////////////////////////////////////////////////////////////////////////////
//
//	Change log
//
//	08 Apr 13 - Initial submission (Kareem)
//	23 Apr 13 - Added hooks for picking up dimensions relevant for the neutron
//				calibration tube (Kareem)
//  23 Jun 13 - Added hooks for picking up the PTFE wall inner radius, i.e., the
//              active xenon radius (Kareem)
//  16 May 14 - Added functions to output neutron calibration tube info. (Kevin)
//					
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimLZSimple_HH
#define LUXSimLZSimple_HH 1

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
class LUXSimLZSimple : public LUXSimDetector
{
	public:
	
		LUXSimLZSimple();
		~LUXSimLZSimple();
		
		G4double GetOuterAcrylicRadius() { return outerAcrylicRadius; };
		G4double GetInnerAcrylicRadius() { return innerAcrylicRadius; };
		G4double GetNeutronCalibrationTubeOD()
				{ return neutronCalibrationTubeOD; };
		G4double GetNeutronCalibrationTubeThickness()
				{ return neutronCalibrationTubeThickness; };
		G4double GetNeutronCalibrationTubeZOffset()
				{ return neutronCalibrationTubeZOffset; };
        G4double GetPTFEWallInnerRadius()
                { return ptfeWallInnerRadius; };
		
	private:

		LUXSimMaterials *luxMaterials;
		
		G4double outerAcrylicRadius;
		G4double innerAcrylicRadius;
		G4double neutronCalibrationTubeOD;
		G4double neutronCalibrationTubeThickness;
                G4double neutronCalibrationTubeZOffset;
        G4double ptfeWallInnerRadius;

};

#endif
