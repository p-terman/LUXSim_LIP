//	LUXSimLZ20Detector.hh

//	This is the header file to define the LZ20 Detector.

//	01.10.10 - Initial submission from 1_0Detector(Melinda)
//	03.05.10 - got rid of WIRES (Melinda)

#ifndef LUXSimLZ20Detector_HH
#define LUXSimLZ20Detector_HH 1

//
//	GEANT4 includes
//
#include "globals.hh"

//
//	LUXSim includes
//
#include "LUXSimDetector.hh"
#include "LUXSimExample8778PMT.hh"
//
//	Class forwarding
//
class G4LogicalVolume;
class LUXSimDetectorComponent;
class LUXSimLZ20PMTBank;
class LUXSimGrid;
//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimLZ20Detector : public LUXSimDetector
{
		
	public:
		LUXSimLZ20Detector( G4bool GRIDS );
		~LUXSimLZ20Detector();
		
		inline LUXSimDetectorComponent *GetThermalShield() {return thermalShield;};
		inline LUXSimDetectorComponent *GetVacuum() {return vacuum;};
		inline LUXSimDetectorComponent *GetXenonVessel() {return xenonVessel;};
		inline LUXSimDetectorComponent *GetRadiationShield() {return radShield;};
		inline LUXSimDetectorComponent *GetRadiationDome() {return radDome;};
		inline LUXSimDetectorComponent *GetCopperRing() {return copperRing;};
		inline LUXSimDetectorComponent *GetLiquidXenon() {return liquidXenon;};
		inline LUXSimDetectorComponent *GetGasXenon() {return gasXenon;};
		inline LUXSimDetectorComponent *GetTopBank() {return topBank;};
		inline LUXSimDetectorComponent *GetBottomBank() {return bottomBank;};

	private:
		LUXSimDetectorComponent *thermalShield;
		LUXSimDetectorComponent *vacuum;
		LUXSimDetectorComponent *xenonVessel;
		LUXSimDetectorComponent *radShield;
		LUXSimDetectorComponent *radDome;
		LUXSimDetectorComponent *hdpeHolder;
		LUXSimDetectorComponent *ptfeSheets;
		LUXSimDetectorComponent *copperRing;
		LUXSimDetectorComponent *liquidXenon;
		LUXSimDetectorComponent *gasXenon;
		LUXSimLZ20PMTBank *banks[2];
		LUXSimDetectorComponent *topBank;
		LUXSimDetectorComponent *bottomBank;
		LUXSimExample8778PMT *PMT8778[122];
		LUXSimDetectorComponent *PMTs[122];
};

#endif
