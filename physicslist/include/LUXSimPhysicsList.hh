////////////////////////////////////////////////////////////////////////////////
/*	LUXSimPhysicsList.hh
*
* This header file is part of the LUXSim physics list. See the code file for
* additional comments on the contents of the physics list.
*
********************************************************************************
* Change log
*	13 March 2009 - Initial submission (Kareem)
*	14-Aug-09 - Major rewrite of the physics list to use the modern physics
*				list builders. This breaks compatability with any version of
*				GEANT4 prior to 4.9.2. (Kareem)
*   18-Dec-2015 -Added flag to build shielding physics list from construction (David W) (merged into git by Doug T)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimPhysicsList_h
#define LUXSimPhysicsList_h 1

//
//  GEANT4 includes
//
#include "G4VModularPhysicsList.hh"
#include "globals.hh"

//
//  LUXSim includes
//
#include "LUXSimManager.hh"

//
//	Class forwarding
//
class G4VPhysicsConstructor;
class LUXSimPhysicsStepMax;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimPhysicsList: public G4VModularPhysicsList
{
	public:
		LUXSimPhysicsList(G4bool useShielding);
		virtual ~LUXSimPhysicsList();

	public:
		void SetCuts();
		void SetCutsLong();
		void SetCutsShort();
		
		void ClearPhysics();
		void RemoveFromPhysicsList( const G4String& );
		
		void ConstructParticle();
		void ConstructProcess();

		void SetStepMax( G4double );
		void AddStepMax();
		LUXSimPhysicsStepMax *GetStepMaxProcess() { return stepMaxProcess; };
		
	private:
		LUXSimManager *luxManager;
	
		G4int VerboseLevel;
		G4int OpVerbLevel;

		G4double defaultCutValue;
		G4double shortCutValue;
		G4double longCutValue;

		G4double cutForGamma;
		G4double cutForElectron;
		G4double cutForPositron;
		G4double cutForProton;
		G4double cutForAlpha;
		G4double cutForGenericIon;

		G4double cutForGammaShort;
		G4double cutForElectronShort;
		G4double cutForPositronShort;

		G4double cutForGammaLong;
		G4double cutForElectronLong;
		G4double cutForPositronLong;
		
		G4double MaxChargedStep;
		LUXSimPhysicsStepMax *stepMaxProcess;
		
};

#endif







