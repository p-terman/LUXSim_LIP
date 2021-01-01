////////////////////////////////////////////////////////////////////////////////
/*	LUXSimSteppingAction.hh
*
* This is the header file to control the simulation step-by-step.
*
********************************************************************************
* Change log
*	13 March 2009 - Initial submission (Kareem)
*	14-Sep-09 - Added support for recording optical photons (Kareem)
*	31-Jan-11 - Added support for obtaining the record level in a step (Kareem)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimSteppingAction_HH
#define LUXSimSteppingAction_HH 1

//
//	GEANT4 includes
//
#include "G4UserSteppingAction.hh"
#include "globals.hh"

//
//	LUXSim includes
//
#include "LUXSimManager.hh"

//
//	Class forward declarations
//
class G4UserEventAction;
class G4Track;
class G4Material;

class LUXSimEventAction;
class LUXSimDetectorComponent;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimSteppingAction : public G4UserSteppingAction
{
	public:
		LUXSimSteppingAction();
		~LUXSimSteppingAction();

		void UserSteppingAction( const G4Step *theStep );

                //      Primary particle information
                LUXSimManager::primaryParticleInfo primaryParticles;

	private:
		LUXSimEventAction* theEventAct;
		
	private:
		G4Track *theTrack;
		G4ThreeVector trackPosition;
		G4ThreeVector particleDirection;
		
		G4int recordLevel;
		G4int optPhotRecordLevel;
		G4int thermElecRecordLevel;	

		LUXSimManager *luxManager;
		LUXSimManager::stepRecord aStepRecord;
		
		G4Material *blackiumMat;
  
                std::map<G4int,bool> radIsoMap;
                std::map<G4int,bool>::iterator itMap;
  
                int currentZ;
                int currentA;

};

#endif
