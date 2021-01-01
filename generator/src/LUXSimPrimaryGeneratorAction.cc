////////////////////////////////////////////////////////////////////////////////
/*	LUXSimPrimaryGeneratorAction.cc
*
* This is the code file to control the GEANT4 side of event generation.
*
********************************************************************************
* Change log
*	13 March 2009 - Initial submission (Kareem)
*	22 April 2009 - Moved the actual primary vertex generation to LUXSimSource
*					(Kareem)
*	28-Apr-09 - Added check to see if any sources have been explicitly set, and
*				if not, just generate the primary vertex (Kareem)
*	18-May-13 - Added emission time for primaries (Chao)
*/
////////////////////////////////////////////////////////////////////////////////

//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"

//
//	LUXSim includes
//
#include "LUXSimPrimaryGeneratorAction.hh"
#include "LUXSimManager.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					LUXSimPrimaryGeneratorAction()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimPrimaryGeneratorAction::LUXSimPrimaryGeneratorAction()
{
	luxManager = LUXSimManager::GetManager();
	luxManager->Register( this );
	particleGun = new G4GeneralParticleSource();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					~LUXSimPrimaryGeneratorAction()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimPrimaryGeneratorAction::~LUXSimPrimaryGeneratorAction()
{
	delete particleGun;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GeneratePrimaries()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimPrimaryGeneratorAction::GeneratePrimaries( G4Event *event )
{
	//	Have the management class determine which event is next and generate
	//	that event
	if( luxManager->GetTotalSimulationActivity() )
		luxManager->GenerateEvent( particleGun, event );
	else {
	    //LUXSimManager::primaryParticleInfo particle = GetParticleInfo(particleGun);
        particleGun->GeneratePrimaryVertex( event );
        luxManager->AddPrimaryParticle( GetParticleInfo(particleGun) );
    }
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//                  GetParticleInfo()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimManager::primaryParticleInfo LUXSimPrimaryGeneratorAction::GetParticleInfo(
        G4GeneralParticleSource *particleGun )
{
    LUXSimManager::primaryParticleInfo particle;
    particle.id = particleGun->GetParticleDefinition()->GetParticleName();
    particle.energy = particleGun->GetCurrentSource()->GetParticleEnergy();
    particle.time = particleGun->GetCurrentSource()->GetParticleTime();
    particle.position = particleGun->GetCurrentSource()->GetParticlePosition();
    particle.direction = particleGun->GetCurrentSource()->
                            GetParticleMomentumDirection();
    return particle;
}
