////////////////////////////////////////////////////////////////////////////////
/*	LUXSimPhysicsMuonNuclear.hh
*
*
********************************************************************************
* Change log
*	20-Oct-14 - Initial submission (David)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimPhysicsMuonNuclear_h
#define LUXSimPhysicsMuonNuclear_h 1

//
//  GEANT4 includes
//
#include "globals.hh"
#include "G4VPhysicsConstructor.hh"
#include "G4S1Light.hh"
#include "G4Cerenkov.hh"

//
//  LUXSim includes
//
#include "LUXSimManager.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimPhysicsMuonNuclear : public G4VPhysicsConstructor
{
	private:
		LUXSimManager *luxManager;

	public:

		LUXSimPhysicsMuonNuclear(const G4String& name = "muNucl");
		virtual ~LUXSimPhysicsMuonNuclear();

		virtual void ConstructParticle();
		virtual void ConstructProcess();
		
};
#endif
