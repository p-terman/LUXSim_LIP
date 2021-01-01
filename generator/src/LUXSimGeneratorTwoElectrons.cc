////////////////////////////////////////////////////////////////////////////////
/*	LUXSimGeneratorTwoElectrons.cc
*
* This is the code file for the Two Electron generator.
*
********************************************************************************
* Change log
*   22 Jul 13 - Initial submission, for Two Electron event generation. (Matthew)
*
*/
////////////////////////////////////////////////////////////////////////////////

//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4GenericIon.hh"
#include "G4Electron.hh"

//
//	LUXSim includes
//
#include "LUXSimGeneratorTwoElectrons.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					LUXSimGeneratorTwoElectrons()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimGeneratorTwoElectrons::LUXSimGeneratorTwoElectrons()
{
	name = "TwoElectrons";
	activityMultiplier = 1;
	ionDef = G4GenericIon::Definition();
        electronDef = G4Electron::Definition();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					~LUXSimGeneratorTwoElectrons()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimGeneratorTwoElectrons::~LUXSimGeneratorTwoElectrons() {}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//                    GenerateEventList()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimGeneratorTwoElectrons::GenerateEventList( G4ThreeVector position,
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
void LUXSimGeneratorTwoElectrons::GenerateFromEventList( G4GeneralParticleSource
       *particleGun, G4Event *event, decayNode *firstNode  )
{
  
    G4ThreeVector pos;
    G4double phi = twopi * G4UniformRand();
    G4double rad = 23. * cm * sqrt(G4UniformRand());
    G4double Z = 5.6 * cm + 48.32 * cm * G4UniformRand();
    pos[0] = rad*cos(phi); pos[1] = rad*sin(phi);
    pos[2] = Z;
    
    G4double timeDelay = (firstNode->timeOfEvent)/ns;//stored as seconds
    timeDelay *= ns;
    
    G4double scatterEnergy1 = luxManager->GetTwoElectronsEmin() + (luxManager->GetTwoElectronsEmax() - luxManager->GetTwoElectronsEmin()) * G4UniformRand();
    G4double scatterEnergy2 = luxManager->GetTwoElectronsEmin() + (luxManager->GetTwoElectronsEmax() - luxManager->GetTwoElectronsEmin()) * G4UniformRand();

    particleGun->GetCurrentSource()->SetParticleTime( timeDelay );
    
    particleGun->GetCurrentSource()->GetPosDist()->SetCentreCoords(pos);
    particleGun->GetCurrentSource()->SetParticleDefinition( electronDef );
    particleGun->GetCurrentSource()->GetAngDist()->
      SetParticleMomentumDirection( GetRandomDirection() );
    particleGun->GetCurrentSource()->GetEneDist()->
      SetMonoEnergy( scatterEnergy1  );
    particleGun->GeneratePrimaryVertex( event );
    luxManager->AddPrimaryParticle( GetParticleInfo(particleGun) ) ;

    phi = twopi * G4UniformRand();
    rad = 23. * cm * sqrt(G4UniformRand());
    pos[0] = rad*cos(phi); pos[1] = rad*sin(phi);
    pos[2] = Z + luxManager->GetTwoElectronsDeltaZMax() * (G4UniformRand() - .5);
    
    particleGun->GetCurrentSource()->GetPosDist()->SetCentreCoords(pos);
    particleGun->GetCurrentSource()->SetParticleDefinition( electronDef );
    particleGun->GetCurrentSource()->GetAngDist()->
      SetParticleMomentumDirection( GetRandomDirection() );
    particleGun->GetCurrentSource()->GetEneDist()->
      SetMonoEnergy( scatterEnergy2 );
    particleGun->GeneratePrimaryVertex( event );
    luxManager->AddPrimaryParticle( GetParticleInfo(particleGun) ) ;


}
