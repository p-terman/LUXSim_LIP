////////////////////////////////////////////////////////////////////////////////
/*	LUXSimSource.cc
*
* This is the code file for individual generators. Each generator will inherit
* from this generalized source class.
*
********************************************************************************
* Change log
*	20 April 2009 - Initial submission (Kareem)
*	27-May-2009 - Modified to work better with LUXSimDetectorComponent (Kareem)
*   4-Jan-2010 - Overloaded GenerateEvent method to accept single nucleus 
*                decays (Nick)
*   30-Apr-2010 - Added GetParticleInfo() to interfaced with AddPrimaryParicle 
*                 of the manager (Nick)
*   21 Jul 2011 - Added DecayChain methods (Nick)
*   04-Mar-2012 - Overload GenerateEvent for SingleParticle sources (Nick)
*   18-Jun-2012 - Overloaded a GenerateEvent for Wimp sources (Daniel)
*   14-Jul-2012 - GenerateEventList and GenerateFromEventList for all sources.
*                 All sources are added to the binary search tree (Nick)
*   18-May-2013 - Added emission time for primaries (Chao)
*   18-Dec-2015 -Overloaded a GenerateEventList for muon generator (David W) (merged into git by Doug T)
*/
////////////////////////////////////////////////////////////////////////////////

//
//	GEANT4 includes
//
#include "globals.hh"

//
//	LUXSim includes
//
#include "LUXSimSource.hh"
#include "LUXSimDetectorComponent.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					LUXSimSource()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimSource::LUXSimSource()
{
	luxManager = LUXSimManager::GetManager();
	UI = G4UImanager::GetUIpointer();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					~LUXSimSourceCatalog()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimSource::~LUXSimSource() {}


//------++++++------++++++------++++++------++++++------++++++------++++++------
//					CalculatePopulationsInEventList()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimSource::CalculatePopulationsInEventList( G4double, G4double, 
                      G4String )
{}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GenerateEventList() // Default
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimSource::GenerateEventList( G4ThreeVector, G4int, G4int, G4double)
{}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GenerateEventList() // DecayChain
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimSource::GenerateEventList( G4ThreeVector, G4int, G4int, G4String )
{}


//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GenerateEventList() //SingleDecay
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimSource::GenerateEventList( G4ThreeVector, G4int, G4int, G4int, 
                      G4int, G4double )
{
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GenerateEventList() //SingleParticle
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimSource::GenerateEventList( G4ThreeVector, G4int, G4int, G4String, 
                      G4double, G4double )
{
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GenerateEventList() //Wimp
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimSource::GenerateEventList( G4ThreeVector, G4int, G4int, G4double, 
                      G4double)
{
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GenerateEventList() //MUSUN
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimSource::GenerateEventList(G4double)
{
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GenerateFromEventList()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimSource::GenerateFromEventList( G4GeneralParticleSource*, G4Event*, 
                      decayNode* )
{
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GetParticleInfo()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimManager::primaryParticleInfo LUXSimSource::GetParticleInfo(
                  G4GeneralParticleSource *particleGun )
{
    LUXSimManager::primaryParticleInfo particle;
    particle.id = particleGun->GetParticleDefinition()->GetParticleName();
    //particle.id = name ;
    particle.energy = particleGun->GetCurrentSource()->GetParticleEnergy(); 
    particle.time = particleGun->GetCurrentSource()->GetParticleTime();	
    particle.position = particleGun->GetCurrentSource()->GetParticlePosition();
    particle.direction = particleGun->GetCurrentSource()->
                                        GetParticleMomentumDirection();
    return particle; 
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GetRandomDirection()
//------++++++------++++++------++++++------++++++------++++++------++++++------
G4ThreeVector LUXSimSource::GetRandomDirection()
{
	cosTheta = -1. + 2.*G4UniformRand();
	sinTheta = sqrt( 1. - cosTheta*cosTheta );
	phi = 2.*3.141592653589793238*G4UniformRand();
	xDir = sinTheta * cos( phi );
	yDir = sinTheta * sin( phi );
	zDir = cosTheta;
	G4ThreeVector direction( xDir, yDir, zDir );

	return direction;
}
