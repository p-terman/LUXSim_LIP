////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorRa226.cc
*
* This is the code file for the Ra226 generator.
*
********************************************************************************
* Change log
*   2013-02-16 DCM - Original version (adapted from U238 generator)
*
*/
////////////////////////////////////////////////////////////////////////////////

//
//    GEANT4 includes
//
#include "globals.hh"
#include "G4GenericIon.hh"

//
//     LUXSim includes
//
#include "LUXSimGeneratorRa226.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
//                    LUXSimGeneratorRa226()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimGeneratorRa226::LUXSimGeneratorRa226()
{
    name = "Ra226";
    activityMultiplier = 6;
    ion = G4GenericIon::Definition();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//                    ~LUXSimGeneratorRa226()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimGeneratorRa226::~LUXSimGeneratorRa226() {}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//                    GenerateEventList()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimGeneratorRa226::GenerateEventList( G4ThreeVector position,
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
void LUXSimGeneratorRa226::GenerateFromEventList( G4GeneralParticleSource
       *particleGun, G4Event *event, decayNode *firstNode  )
{
    particleGun->GetCurrentSource()->SetParticleDefinition( ion );
    particleGun->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(
            GetRandomDirection() );
    particleGun->GetCurrentSource()->GetEneDist()->SetMonoEnergy( 0.*keV );

    G4double time = (firstNode->timeOfEvent)/ns;
    particleGun->GetCurrentSource()->SetParticleTime( time*ns );

    G4ThreeVector pos = G4ThreeVector(firstNode->pos);
    particleGun->GetCurrentSource()->GetPosDist()->SetCentreCoords(pos);
    probability = G4UniformRand();
    
    if( probability < 1./activityMultiplier ) {
        UI->ApplyCommand( "/gps/ion 88 226 0 0" );
        UI->ApplyCommand( "/grdm/nucleusLimits 226 226 88 88" );
    } else if( probability < 2./activityMultiplier ) {
        UI->ApplyCommand( "/gps/ion 86 222 0 0" );
        UI->ApplyCommand( "/grdm/nucleusLimits 222 222 86 86" );
    } else if( probability < 3./activityMultiplier ) {
        UI->ApplyCommand( "/gps/ion 84 218 0 0" );
        UI->ApplyCommand( "/grdm/nucleusLimits 218 218 84 84" );
    } else if( probability < 4./activityMultiplier ) {
        UI->ApplyCommand( "/gps/ion 82 214 0 0" );
        UI->ApplyCommand( "/grdm/nucleusLimits 214 214 82 82" );
    } else if( probability < 5./activityMultiplier ) {
        UI->ApplyCommand( "/gps/ion 83 214 0 0" );
        UI->ApplyCommand( "/grdm/nucleusLimits 214 214 83 83" );
    } else {
        if( G4UniformRand() < .99979) {
            UI->ApplyCommand( "/grdm/nucleusLimits 214 214 84 84" );
            UI->ApplyCommand( "/gps/ion 84 214 0 0" );
        } else {
            UI->ApplyCommand( "/grdm/nucleusLimits 210 210 81 81" );
            UI->ApplyCommand( "/gps/ion 81 210 0 0" );
        }
    }

    particleGun->GeneratePrimaryVertex( event );
    luxManager->AddPrimaryParticle( GetParticleInfo(particleGun) );
}
