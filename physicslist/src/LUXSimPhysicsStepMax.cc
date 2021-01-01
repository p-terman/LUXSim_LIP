////////////////////////////////////////////////////////////////////////////////
/*	LUXSimPhysicsStepMax.cc
*
* This code file comes from Peter Gumplinger, and is intended to be a part of
* the extended example field04 (which should be part of GEANT4.9.3). It turns
* on physics for optical models.
*
********************************************************************************
* Change log
*	26-Aug-09 - Initial submission (Kareem)
*
*/
////////////////////////////////////////////////////////////////////////////////

//
//	GEANT4 includes
//
#include "G4Track.hh"
#include "G4VParticleChange.hh"

//
//	LUXSim includes
//
#include "LUXSimPhysicsStepMax.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					LUXSimPhysicsStepMax()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimPhysicsStepMax::LUXSimPhysicsStepMax( const G4String& aName )
  : G4VDiscreteProcess(aName), MaxChargedStep(DBL_MAX)
{
	if( verboseLevel>0 )
		G4cout << GetProcessName() << " is created "<< G4endl;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					LUXSimPhysicsStepMax()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimPhysicsStepMax::LUXSimPhysicsStepMax( LUXSimPhysicsStepMax& right ) : G4VDiscreteProcess(right) { }

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					~LUXSimPhysicsStepMax()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimPhysicsStepMax::~LUXSimPhysicsStepMax() { }

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					PostStepGetPhysicalInteractionLength()
//------++++++------++++++------++++++------++++++------++++++------++++++------
G4double LUXSimPhysicsStepMax::PostStepGetPhysicalInteractionLength(
                                              const G4Track&,
                                              G4double,
                                              G4ForceCondition* condition)
{
	// condition is set to "Not Forced"
	*condition = NotForced;

	G4double ProposedStep = DBL_MAX;
	if ( MaxChargedStep > 0.) ProposedStep = MaxChargedStep ;
	return ProposedStep;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					PostStepDoIt()
//------++++++------++++++------++++++------++++++------++++++------++++++------
G4VParticleChange* LUXSimPhysicsStepMax::PostStepDoIt(const G4Track& aTrack,
                                         const G4Step&         )
{
	// do nothing
	aParticleChange.Initialize(aTrack);
	return &aParticleChange;
}
