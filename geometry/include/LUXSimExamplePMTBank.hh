////////////////////////////////////////////////////////////////////////////////
/*	LUXSimExamplePMTBank.hh
*
* This is the header file to define an example bank of 8778-model PMTs.
*
********************************************************************************
* Change log
*	16 March 2009 - Initial submission (Kareem)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimExamplePMTBank_HH
#define LUXSimExamplePMTBank_HH 1

//
//	GEANT4 includes
//
#include "globals.hh"

//
//	LUXSim includes
//
#include "LUXSimExample8778PMT.hh"

//
//	Class forwarding
//
class G4LogicalVolume;
class LUXSimDetectorComponent;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimExamplePMTBank
{
	public:
		LUXSimExamplePMTBank();
		~LUXSimExamplePMTBank();
		
		inline G4LogicalVolume *GetLogicalVolume() { return logicalVolume; };
		inline LUXSimDetectorComponent *GetPMT( G4int num )
				{ return PMTs[num]; };
		inline LUXSimExample8778PMT *GetPMTClass( G4int num )
				{ return PMT8778[num]; };
		
	private:
		
		G4LogicalVolume *logicalVolume;
		LUXSimExample8778PMT *PMT8778[3];
		LUXSimDetectorComponent *PMTs[3];
};

#endif
