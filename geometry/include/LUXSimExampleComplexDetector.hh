////////////////////////////////////////////////////////////////////////////////
/*	LUXSimExampleComplexDetector.hh
*
* This is the header file to define the example "complex" detector.
*
********************************************************************************
* Change log
*	16 March 2009 - Initial submission (Kareem)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimExampleComplexDetector_HH
#define LUXSimExampleComplexDetector_HH 1

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
class LUXSimExampleComplexDetector : public LUXSimDetector
{
	public:
		LUXSimExampleComplexDetector();
		~LUXSimExampleComplexDetector();
		
		inline LUXSimDetectorComponent *GetInnerCryostat()
				{ return innerCryostat; };
		inline LUXSimDetectorComponent *GetLXeTarget() {return liquidXeTarget;};
		inline LUXSimDetectorComponent *GetTopBank() { return topBank; };
		inline LUXSimDetectorComponent *GetBottomBank() { return bottomBank; };
		inline LUXSimDetectorComponent *GetVacuum() {return cryostatVacuum; };		

	private:
		LUXSimDetectorComponent *innerCryostat;
		LUXSimDetectorComponent *liquidXeTarget;
		LUXSimExamplePMTBank *banks[2];
		LUXSimDetectorComponent *topBank;
		LUXSimDetectorComponent *bottomBank;
		LUXSimDetectorComponent *cryostatVacuum;
};

#endif
