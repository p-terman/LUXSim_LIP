////////////////////////////////////////////////////////////////////////////////
/*	LUXSimGeneratorRn222.cc
*
* This is the code file for the Rn222 generator.
*
********************************************************************************
* Change log
*	26-June-2009 - file creation (Nick)
*   30-Apr-2010 - Added primaryParticle recording (Nick)
*   14-Jul-2012 - GenerateEventList methods of binary search tree (Nick)
*
*/
////////////////////////////////////////////////////////////////////////////////

//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4GenericIon.hh"

//
//	LUXSim includes
//
#include "LUXSimGeneratorRn222.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					LUXSimGeneratorRn222()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimGeneratorRn222::LUXSimGeneratorRn222()
{
	name = "Rn222";
	activityMultiplier = 5;
	ion = G4GenericIon::Definition();
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					~LUXSimGeneratorRn222()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimGeneratorRn222::~LUXSimGeneratorRn222() {}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					GenerateEvent()
//------++++++------++++++------++++++------++++++------++++++------++++++------
//void LUXSimGeneratorRn222::GenerateEvent( G4GeneralParticleSource *particleGun,
//		G4Event *event )
//------++++++------++++++------++++++------++++++------++++++------++++++------
//                    GenerateEventList()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimGeneratorRn222::GenerateEventList( G4ThreeVector position,
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
void LUXSimGeneratorRn222::GenerateFromEventList( G4GeneralParticleSource
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
  	//radon222
  	if( probability < 1./5. ) {
  		UI->ApplyCommand( "/gps/ion 86 222 0 0" );
  		UI->ApplyCommand( "/grdm/nucleusLimits 222 222 86 86" );
  	}
  	//polonium21
  	else if( probability < 2./5. ) {
  		UI->ApplyCommand( "/gps/ion 84 218 0 0" );
  		UI->ApplyCommand( "/grdm/nucleusLimits 218 218 84 84" );
  	}
  	//lead214
  	else if( probability < 3./5. ) {
  		UI->ApplyCommand( "/gps/ion 82 214 0 0" );
  		UI->ApplyCommand( "/grdm/nucleusLimits 214 214 82 82" );
  	} 
  	//bimuth214
  	else if( probability < 4./5. ) {
  		UI->ApplyCommand( "/gps/ion 83 214 0 0" );
  		UI->ApplyCommand( "/grdm/nucleusLimits 214 214 83 83" );
  	} 		
  	//polonium214
  	else{
  			UI->ApplyCommand( "/grdm/nucleusLimits 214 214 84 84" );
  			UI->ApplyCommand( "/gps/ion 84 214 0 0" );
  	}
  	//ends at decay Po decay to Lead-210

  	particleGun->GeneratePrimaryVertex( event );
    luxManager->AddPrimaryParticle( GetParticleInfo(particleGun) );
}


