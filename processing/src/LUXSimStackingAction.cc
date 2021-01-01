////////////////////////////////////////////////////////////////////////////////
/*   LUXSimStackingAction.cc
*
* This is an implementation of the user stacking action which deals with tracks
* as they come into existence. It is initially created to reset the global time
* for daughter tracks of radioisotopes when using the G4Decay generator.
*
********************************************************************************
* Change log
*   06-Oct-2015 - Initial submission (David W)
*/
////////////////////////////////////////////////////////////////////////////////

//
//GEANT4 includes
//
#include "G4ClassificationOfNewTrack.hh"
#include "G4Track.hh"

//
//LUXSim includes
//
#include "LUXSimStackingAction.hh"



//------++++++------++++++------++++++------++++++------++++++------++++++------
//                              LUXSimSteppingAction()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimStackingAction::LUXSimStackingAction(){

    luxManager = LUXSimManager::GetManager();
    double lastTime = 0;

}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//                              ~LUXSimSteppingAction()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimStackingAction::~LUXSimStackingAction() {;}


//------++++++------++++++------++++++------++++++------++++++------++++++------
//                              ClassifyNewTrack()
//------++++++------++++++------++++++------++++++------++++++------++++++------
G4ClassificationOfNewTrack LUXSimStackingAction::ClassifyNewTrack( const G4Track * aTrack ){
  
  G4ClassificationOfNewTrack result( fUrgent );  
  
  if(luxManager->GetG4DecayBool()){
    lastTime = aTrack->GetGlobalTime();
    std::map<G4int,bool> radIsoMap = luxManager->GetRadioIsotopeMap();
    std::map<G4int,bool>::iterator itMap;
    bool foundIso = false;
    
    for(itMap=radIsoMap.begin(); itMap!=radIsoMap.end(); ++itMap)
      if( itMap->first == aTrack->GetDefinition()->GetPDGEncoding() && !itMap->second) foundIso = true;
    
    if(!foundIso){ 
      if(lastTime > 1e17) lastTime = 1e10;
      (const_cast<G4Track *>(aTrack))->SetGlobalTime( lastTime );      
    }
    else
      (const_cast<G4Track *>(aTrack))->SetGlobalTime( 0 );
  }
  
  return result;
    
}
