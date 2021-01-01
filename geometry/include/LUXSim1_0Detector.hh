//	LUXSim1_0Detector.hh

//	This is the header file to define the Lux1.0 Detector.

//	04.07.09 - Initial submission (Melinda)

//	09.29.09 - changed to accept G4bool GRIDS/WIRES parameters (Melinda)

//	03.05.10 - Got rid of WIRES (Melinda)

//	03.29.10 - Added field shaping rings (Melinda)

//	03.14.12 - Added the liquidXenonSkin volume (Kareem)

//	08.16.12 - Updated constructor to take optional second argument, to
//			   change PMT numbering to match real numbering (Rich)

//	10.08.12 - Added the ptfeSourcePanels volume to allow for surface emission
//			   from the inner PTFE walls. (Kareem)

//  2013-02-14 - Added the gaseousXenonSkin volume (Vic)

//  2013-04-26 - Added bottomXenonSkin and topXenonSkin (Vic)

#ifndef LUXSim1_0Detector_HH
#define LUXSim1_0Detector_HH 1

//
//	GEANT4 includes
//
#include "globals.hh"

//	LUXSim includes
#include "LUXSimDetector.hh"
#include "LUXSimExample8778PMT.hh"
//
//	Class forwarding
//
class G4LogicalVolume;
class LUXSimDetectorComponent;
class LUXSim1_0PMTBank;
class LUXSimGrid;
//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSim1_0Detector : public LUXSimDetector
{
		
 public:
		LUXSim1_0Detector( G4bool GRIDS, G4bool useRealNumbering = false,
				G4bool gasRun = false );
		~LUXSim1_0Detector();
		
		inline LUXSimDetectorComponent *GetThermalShield(){ return thermalShield; };
		inline LUXSimDetectorComponent *GetHDPEInsulation(){ return hdpeInsulation; };
		inline LUXSimDetectorComponent *GetMylarInsulation(){ return mylarInsulation; };
		inline LUXSimDetectorComponent *GetVacuum() { return vacuum; };
		inline LUXSimDetectorComponent *GetXenonVessel() { return xenonVessel;};
		inline LUXSimDetectorComponent *GetRadiationShield()
				{return radShield; };
		inline LUXSimDetectorComponent *GetRadiationDome() { return radDome; };
		inline LUXSimDetectorComponent *GetHDPEHolder() { return hdpeHolder; };
		inline LUXSimDetectorComponent *GetPTFESheets() { return ptfeSheets; };
		inline LUXSimDetectorComponent *GetCopperRing() { return copperRing; };
		inline LUXSimDetectorComponent *GetLiquidXenon() { return liquidXenon;};
		inline LUXSimDetectorComponent *GetLiquidXenonSkin() { return liquidXenonSkin; };
                inline LUXSimDetectorComponent *GetGaseousXenonSkin() {return gaseousXenonSkin; };
                inline LUXSimDetectorComponent *GetBottomXenonSkin() {return bottomXenonSkin; };
                inline LUXSimDetectorComponent *GetTopXenonSkin() {return topXenonSkin; }
		inline LUXSimDetectorComponent *GetGasXenon() { return gasXenon; };
		inline LUXSimDetectorComponent *GetTopBank() { return topBank; };
		inline LUXSimDetectorComponent *GetBottomBank() { return bottomBank; };
		inline LUXSimDetectorComponent *GetTopReflector() { return topReflector; };
		inline LUXSimDetectorComponent *GetBottomReflector() { return bottomReflector; };
		inline G4double GetS2VolumeHeight(){ return S2VolumeHeight; };
		inline G4double GetLiqGasBoundaryZ(){ return LiqGasBoundaryZ; };
		inline G4double GetTopPMTZ(){ return TopPMTZ; };
		inline G4double GetTopGridZ(){ return TopGridZ; };
		inline G4double GetAnodeGridZ(){ return AnodeGridZ; };
		inline G4double GetGateGridZ(){ return GateGridZ; };
		inline G4double GetCathodeGridZ(){ return CathodeGridZ; };
		inline G4double GetBottomGridZ(){ return BottomGridZ; };
		inline G4double GetBottomPMTZ(){ return BottomPMTZ; };

	private:
		LUXSimDetectorComponent *thermalShield;
		LUXSimDetectorComponent *hdpeInsulation;
		LUXSimDetectorComponent *mylarInsulation;
		LUXSimDetectorComponent *vacuum;
		LUXSimDetectorComponent *xenonVessel;
		LUXSimDetectorComponent *radShield;
		LUXSimDetectorComponent *radDome;
		LUXSimDetectorComponent *hdpeHolder;
		LUXSimDetectorComponent *ptfeSheets;
		LUXSimDetectorComponent *ptfeSourcePanels[12];
		LUXSimDetectorComponent *copperRing;
		LUXSimDetectorComponent *liquidXenon;
		LUXSimDetectorComponent *liquidXenonSkin;
		LUXSimDetectorComponent *gaseousXenonSkin;
        LUXSimDetectorComponent *bottomXenonSkin;
        LUXSimDetectorComponent *topXenonSkin;
		LUXSimDetectorComponent *gasXenon;
		LUXSim1_0PMTBank *banks[2];
		LUXSimDetectorComponent *topBank;
		LUXSimDetectorComponent *bottomBank;
		LUXSimDetectorComponent *topReflector;
		LUXSimDetectorComponent *bottomReflector;
		LUXSimDetectorComponent *fieldRings[47];
		LUXSimGrid *grids[5];
		//LUXSimDetectorComponent *bottomReflectorSpace[61]; //Commented out for now since this is unused.  --vmg
		//LUXSimDetectorComponent *topReflectorSpace[61]; //Commented out for now since this is unused.  --vmg
		//LUXSimExample8778PMT *PMT8778[122]; //Commented out for now since this is unused.  --vmg
		//LUXSimDetectorComponent *PMTs[122]; //Commented out for now since this is unused.  --vmg
		G4double S2VolumeHeight;
		G4double LiqGasBoundaryZ;
		G4double LiqLevelAboveGate;
		G4double TopPMTZ;
		G4double TopGridZ;
		G4double AnodeGridZ;
		G4double GateGridZ;
		G4double CathodeGridZ;
		G4double BottomGridZ;
		G4double BottomPMTZ;
};

#endif
