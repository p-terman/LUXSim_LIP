////////////////////////////////////////////////////////////////////////////////
/*	LUXSimPhysicsStepMax.hh
*
* This header file is part of the LUXSim physics list. It is based on the
* extended field example field04, with some additional improvements from 
* Peter Gumplinger.
*
********************************************************************************
* Change log
*	26-Aug-07 - Initial submission (Kareem)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimPhysicsStepMax_h
#define LUXSimPhysicsStepMax_h 1

//
//  GEANT4 includes
//
#include "globals.hh"
#include "G4Step.hh"
#include "G4VDiscreteProcess.hh"
#include "G4ParticleDefinition.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimPhysicsStepMax : public G4VDiscreteProcess
{
	public:
		LUXSimPhysicsStepMax( const G4String& processName = "UserStepMax" );
		LUXSimPhysicsStepMax( LUXSimPhysicsStepMax & );

		~LUXSimPhysicsStepMax();

		G4bool IsApplicable( const G4ParticleDefinition& particle ) {
			return( particle.GetPDGCharge() != 0. ); };

		void SetStepMax( G4double step ) { MaxChargedStep = step; };
		G4double GetStepMax() { return MaxChargedStep; };

		G4double PostStepGetPhysicalInteractionLength(const G4Track& track,
                                                  G4double previousStepSize,
                                                  G4ForceCondition* condition);

		G4VParticleChange* PostStepDoIt(const G4Track&, const G4Step&);

	protected:
		G4double GetMeanFreePath(const G4Track&, G4double, G4ForceCondition*)
			{ return 0.; };

	private:

		// hide assignment operator as private
		LUXSimPhysicsStepMax & operator=(const LUXSimPhysicsStepMax &right);
		LUXSimPhysicsStepMax( const LUXSimPhysicsStepMax& );

	private: 
		G4double MaxChargedStep;
};

#endif
