////////////////////////////////////////////////////////////////////////////////
/*	LUXSimExampleSimpleDetector.hh
*
* This is the header file to define an "simple" example detector.
*
********************************************************************************
* Change log
*	16 March 2009 - Initial submission (Kareem)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimExampleSimpleDetector_HH
#define LUXSimExampleSimpleDetector_HH 1

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
class LUXSimExamplePMTBank;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimExampleSimpleDetector : public LUXSimDetector
{
	public:
		LUXSimExampleSimpleDetector();
		~LUXSimExampleSimpleDetector();
		
		inline LUXSimDetectorComponent *GetLXeTarget() {return liquidXeTarget;};
		inline LUXSimDetectorComponent *GetTopBank() { return topBank; };
		inline LUXSimDetectorComponent *GetBottomBank() { return bottomBank; };
		
	private:
		
		LUXSimDetectorComponent *liquidXeTarget;
		LUXSimExamplePMTBank *banks[2];
		LUXSimDetectorComponent *topBank;
		LUXSimDetectorComponent *bottomBank;
};

#endif
