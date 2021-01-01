//	LUXSimEmptyCryostat.hh

//	This is the header file to define the empty cryostat.

//	04.28.09 - Initial submission (Melinda)

#ifndef LUXSimEmptyCryostat_HH
#define LUXSimEmptyCryostat_HH 1

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
class LUXSimEmptyCryostat : public LUXSimDetector
{
		
	public:
		LUXSimEmptyCryostat();
		~LUXSimEmptyCryostat();
		
		inline LUXSimDetectorComponent *GetThermalShield() {return thermalShield;};
		inline LUXSimDetectorComponent *GetXenonVessel() {return xenonVessel;};
		inline LUXSimDetectorComponent *GetVacuum() {return vacuum;};


		
	private:
		LUXSimDetectorComponent *thermalShield;
		LUXSimDetectorComponent *xenonVessel;
		LUXSimDetectorComponent *vacuum;
};

#endif
