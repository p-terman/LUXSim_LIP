////////////////////////////////////////////////////////////////////////////////
/*	LUXSimPrimaryGeneratorAction.hh
*
* This is the header file to control the GEANT4 side of event generation.
*
********************************************************************************
* Change log
*	13 March 2009 - Initial submission (Kareem)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimPrimaryGeneratorAction_HH
#define LUXSimPrimaryGeneratorAction_HH 1

//
//	GEANT4 includes
//
#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

//
//  LUXSim includes
//
#include "LUXSimManager.hh"

//
//	Class forwarding
//
class G4GeneralParticleSource;
class G4Event;
class LUXSimManager;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
	public:
		LUXSimPrimaryGeneratorAction();
		~LUXSimPrimaryGeneratorAction();

	public:
		void GeneratePrimaries( G4Event* );

    protected:
        LUXSimManager::primaryParticleInfo GetParticleInfo( G4GeneralParticleSource* );

	private:
		LUXSimManager *luxManager;
		G4GeneralParticleSource *particleGun;
};

#endif
