//	LUXSimLLNL_SinglePhase.hh
//
//	This is the header file for the LLNL single-phase detector geometry. It is
//	in the moveable-source configuration, with six rings defining the primary
//	volume, and a 4-cm-high secondary volume defined by wires. The collimator is
//	also fully simulated.

//	24 Nov 2010 - Initial submission (Kareem)
//	3 Dec 2010  - Removed the viewport window as a separate object. Now it's
//				  just the viewport. Also added the PMT window to get the
//				  correct reflections between the port and PMT window. (Kareem)

#ifndef LUXSimLLNL_SinglePhase_HH
#define LUXSimLLNL_SinglePhase_HH 1

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
class LUXSimGrid;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimLLNL_SinglePhase : public LUXSimDetector
{
	public:
		LUXSimLLNL_SinglePhase();
		~LUXSimLLNL_SinglePhase();
	
	private:
		LUXSimDetectorComponent *gasVolume;
		LUXSimDetectorComponent *viewport;
		LUXSimDetectorComponent *pmtWindow;
		LUXSimDetectorComponent *activeVolume;
		LUXSimDetectorComponent *fieldRings[8];
		LUXSimDetectorComponent *shapingRings[2];
		LUXSimDetectorComponent *hemisphere;
		LUXSimDetectorComponent *collimator;
		LUXSimDetectorComponent *source;
		LUXSimDetectorComponent *gridHolder[2];

};

#endif
