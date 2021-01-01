////////////////////////////////////////////////////////////////////////////////
/*	LUXSimPhysicsOpticalPhysics.hh
*
* This header file is part of the LUXSim physics list. It is based on the
* extended field example field04, with some additional improvements from 
* Peter Gumplinger.
*
********************************************************************************
* Change log
*	26-Aug-09 - Initial submission (Kareem)
*	28-Aug-09 - Added access and registration to the LUXSimManager, made
*				theScintProcess and theCerenovProcess private variables,
*				added Get methods for scintillation and Cerenkov (Kareem)
*       13-Sep-11 - Changed G4Scintillation calls to G4S1Light (Matthew)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimPhysicsOpticalPhysics_h
#define LUXSimPhysicsOpticalPhysics_h 1

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
class LUXSimPhysicsOpticalPhysics : public G4VPhysicsConstructor
{
	private:
		LUXSimManager *luxManager;

		G4S1Light *theScintProcess;
		G4Cerenkov *theCerenkovProcess;

	public:

		LUXSimPhysicsOpticalPhysics();
		virtual ~LUXSimPhysicsOpticalPhysics();

		virtual void ConstructParticle();
		virtual void ConstructProcess();
		
		G4S1Light *GetScintillation() { return theScintProcess; };
		G4Cerenkov *GetCerenkov() { return theCerenkovProcess; };
};
#endif
