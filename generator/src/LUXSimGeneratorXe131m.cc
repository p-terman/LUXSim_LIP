////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorXe131m.cc
*
* This is the code file for the Xe131m generator.
*
********************************************************************************
* Change log
*	2013-03-12 DCM - Original version
*       2013-07-22 MMS - Changed output to mono-E gamma because G4 does not do
*                        the decay right and it screws up the yield in NEST
*/
////////////////////////////////////////////////////////////////////////////////

//
//    GEANT4 includes
//
#include "globals.hh"
#include "G4GenericIon.hh"
#include "G4Gamma.hh"

//
//     LUXSim includes
//
#include "LUXSimGeneratorXe131m.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
//                    LUXSimGeneratorXe131m()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimGeneratorXe131m::LUXSimGeneratorXe131m()
{
    name = "Xe131m";
    activityMultiplier = 1;
    ion = G4GenericIon::Definition();
    gammaDef = G4Gamma::Definition();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//                    ~LUXSimGeneratorXe131m()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimGeneratorXe131m::~LUXSimGeneratorXe131m() {}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//                    GenerateEventList()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimGeneratorXe131m::GenerateEventList( G4ThreeVector position,
                G4int sourceByVolumeID, G4int sourcesID, G4double time)
{
    G4int a=-1; G4int z=-1; G4double hl=-1;
    Isotope *currentIso = new Isotope(name, z, a, hl);
    luxManager->RecordTreeInsert( currentIso, time, position, 
                  sourceByVolumeID, sourcesID );    
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//                    GenerateFromEventList()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimGeneratorXe131m::GenerateFromEventList( G4GeneralParticleSource
       *particleGun, G4Event *event, decayNode *firstNode  )
{
    particleGun->GetCurrentSource()->SetParticleDefinition( gammaDef );
    particleGun->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(
            GetRandomDirection() );
    particleGun->GetCurrentSource()->GetEneDist()->SetMonoEnergy( 163.9308*keV );

    G4double time = (firstNode->timeOfEvent)/ns;
    particleGun->GetCurrentSource()->SetParticleTime( time*ns );

    G4ThreeVector pos = G4ThreeVector(firstNode->pos);
    particleGun->GetCurrentSource()->GetPosDist()->SetCentreCoords(pos);
    
    particleGun->GeneratePrimaryVertex( event );
    luxManager->AddPrimaryParticle( GetParticleInfo(particleGun) );
}
