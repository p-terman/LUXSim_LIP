////////////////////////////////////////////////////////////////////////////////
/*	LUXSimGeneratorGammaX.cc
*
* This is the code file for the Gamma-X generator.
*
********************************************************************************
* Change log
*   22 Jul 13 - Initial submission, for gamma-X event generation. (Matthew)
*
*/
////////////////////////////////////////////////////////////////////////////////

//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4GenericIon.hh"
#include "G4Electron.hh"
#include "G4Gamma.hh"

//
//	LUXSim includes
//
#include "LUXSimGeneratorGammaX.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					LUXSimGeneratorGammaX()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimGeneratorGammaX::LUXSimGeneratorGammaX()
{
	name = "gamma-X";
	activityMultiplier = 1;
	ionDef = G4GenericIon::Definition();
    electronDef = G4Electron::Definition();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					~LUXSimGeneratorGammaX()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimGeneratorGammaX::~LUXSimGeneratorGammaX() {}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//                    GenerateEventList()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimGeneratorGammaX::GenerateEventList( G4ThreeVector position,
                G4int sourceByVolumeID, G4int sourcesID, G4double time)
{
    G4int a=-1; G4int z=-1;
    G4double hl=-1;
    Isotope *currentIso = new Isotope(name, z, a, hl);
    luxManager->RecordTreeInsert( currentIso, time, position, 
                  sourceByVolumeID, sourcesID );
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//                    GenerateFromEventList()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimGeneratorGammaX::GenerateFromEventList( G4GeneralParticleSource
       *particleGun, G4Event *event, decayNode *firstNode  )
{
  
    G4ThreeVector pos;
    G4double phi = twopi * G4UniformRand();
    G4double rad = luxManager->GetGammaXFiducialR() * sqrt(G4UniformRand());
    G4double fiducialHeight = luxManager->GetGammaXFiducialHiZ() - luxManager->GetGammaXFiducialLoZ();
    pos[0] = rad*cos(phi); pos[1] = rad*sin(phi);
    pos[2] = luxManager->GetGammaXFiducialLoZ() + fiducialHeight * G4UniformRand();
    
    G4double timeDelay = (firstNode->timeOfEvent)/ns;//stored as seconds
    timeDelay *= ns;
    
    G4double upperScatter = G4UniformRand() * luxManager->GetGammaXMaxEnergy();
    G4double lowerScatter = G4UniformRand() * luxManager->GetGammaXMaxEnergy();

    particleGun->GetCurrentSource()->SetParticleTime( timeDelay );
    
    particleGun->GetCurrentSource()->GetPosDist()->SetCentreCoords(pos);
    particleGun->GetCurrentSource()->SetParticleDefinition( electronDef );
    particleGun->GetCurrentSource()->GetAngDist()->
      SetParticleMomentumDirection( GetRandomDirection() );
    particleGun->GetCurrentSource()->GetEneDist()->
      SetMonoEnergy( upperScatter );
    particleGun->GeneratePrimaryVertex( event );
    luxManager->AddPrimaryParticle( GetParticleInfo(particleGun) ) ;

    G4double reverseFieldHeight = luxManager->GetGammaXReverseFieldHiZ() - luxManager->GetGammaXReverseFieldLoZ();
    phi = twopi * G4UniformRand();
    rad = luxManager->GetGammaXReverseFieldR() * sqrt(G4UniformRand());
    pos[0] = rad*cos(phi); pos[1] = rad*sin(phi);
    pos[2] = luxManager->GetGammaXReverseFieldLoZ() + reverseFieldHeight * G4UniformRand();
    
    particleGun->GetCurrentSource()->GetPosDist()->SetCentreCoords(pos);
    particleGun->GetCurrentSource()->SetParticleDefinition( electronDef );
    particleGun->GetCurrentSource()->GetAngDist()->
      SetParticleMomentumDirection( GetRandomDirection() );
    particleGun->GetCurrentSource()->GetEneDist()->
      SetMonoEnergy( lowerScatter );
    particleGun->GeneratePrimaryVertex( event );
    luxManager->AddPrimaryParticle( GetParticleInfo(particleGun) ) ;


}
