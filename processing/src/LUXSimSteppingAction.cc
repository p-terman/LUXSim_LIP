/////////////////////////////////////////////////////////////////////////////////
/*	LUXSimSteppingAction.cc
*
* This is the code file to control the simulation step-by-step.
*
********************************************************************************
* Change log
*	13 March 2009 - Initial submission (Kareem)
*	27-May-2009 - Added the debugging output. Change the #define to invoke.
*				  (Kareem)
*	19 June 2010 - Added the check for particles transiting a volume made of
*				   blackium. If they are, the track is killed and no information
*				   is recorded. (Kareem)
*   24-Aug-2010 - Fixed position recording in UserSteppingAction so that post-
*                 step position is recorded. (Dave)
*	11-Nov-2010 - Changed the blackium behavior so that a particle's first step
*				  inside blackium is recorded before being killed (Kareem)
*	31-Jan-2011 - Added support for interpreting record level 4 (Kareem)
*	08-Sep-2011 - Force energy depositions from optical photons to be zero
*				  (Kareem)
*	02-Dec-2011 - We now record the creator process in the step record (Kareem)
*	13-Apr-2012 - Added the position to the debugging output (Kareem)
*	01-May-2012 - Added support for thermal electrons. (Chao)
*	23-Oct-2012 - The global time of daughter particles of a primary radioactive
*				  particle is defined as the creation time of the primary
*				  radioactive particle itself. This was we don't have to wait
*				  for the particle to decay before obtaining its daughters
*				  (Kareem)
*   07-Apr-2014 - Added support for the 100-keV hack (Kareem)
*   19-May-2014 - Fixed a bug in the timing of decay particles from
*                 radioisotopes (Kareem)
*   21-May-2014 - Changed the way the energy in the active liquid xenon is
*                 recorded to make it compatible with the arbitrary 100 keV
*                 hack (Kareem)
*   20-Aug-2015 - Added the poststep processes to the step record (Kareem)
*   27-Aug-2015 - Tracks are now killed if the total energy depostion in the
*                 liquid xenon is greater than the upper limit set in the
*                 /LUXSim/io/upperEnergyHack command (Kareem)
*   06-Oct-2015 - Changes to accommodate the G4Decay generator (David W).
*/
////////////////////////////////////////////////////////////////////////////////

//
//	GEANT4 includes
//
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4EventManager.hh"
#include "G4StackManager.hh"

//
//	LUXSim includes
//
#include "LUXSimSteppingAction.hh"
#include "LUXSimManager.hh"
#include "LUXSimDetectorComponent.hh"
#include "LUXSimMaterials.hh"
#include "LUXSimEventAction.hh"

//
//	Definitions
//
#define DEBUGGING 0

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				LUXSimSteppingAction()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimSteppingAction::LUXSimSteppingAction()
{
	luxManager = LUXSimManager::GetManager();
	luxManager->Register( this );
	
	LUXSimMaterials *luxMaterials = LUXSimMaterials::GetMaterials();
	blackiumMat = luxMaterials->Blackium();

	optPhotRecordLevel = 0;
	thermElecRecordLevel = 0;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				~LUXSimSteppingAction()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimSteppingAction::~LUXSimSteppingAction() {}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//				UserSteppingAction()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimSteppingAction::UserSteppingAction( const G4Step* theStep )
{
	//	Initialize step specifics
	theTrack = theStep->GetTrack();
    
    if( luxManager->Get100keVHack() &&
           luxManager->GetLiquidXenonEnergy() > luxManager->Get100keVHack() )
        theTrack->SetTrackStatus( fStopAndKill );
    else {
        trackPosition = theStep->GetPostStepPoint()->GetPosition();
        particleDirection = theStep->GetPreStepPoint()->GetMomentumDirection();
        recordLevel = luxManager->GetComponentRecordLevel(
                (LUXSimDetectorComponent*)theTrack->GetVolume() );
        optPhotRecordLevel = luxManager->GetComponentRecordLevelOptPhot(
                (LUXSimDetectorComponent*)theTrack->GetVolume() );
        thermElecRecordLevel = luxManager->GetComponentRecordLevelThermElec(
                            (LUXSimDetectorComponent*)theTrack->GetVolume() );
        
        //	Record relevant parameters in the step record
        aStepRecord.stepNumber = theTrack->GetCurrentStepNumber();
        aStepRecord.particleID = theTrack->GetDefinition()->GetPDGEncoding();
        aStepRecord.particleName = theTrack->GetDefinition()->GetParticleName();
        if( theTrack->GetCreatorProcess() )
            aStepRecord.creatorProcess =
                    theTrack->GetCreatorProcess()->GetProcessName();
        else 
            aStepRecord.creatorProcess = "primary";
        aStepRecord.stepProcess = theStep->GetPostStepPoint()->
                GetProcessDefinedStep()->GetProcessName();
        aStepRecord.trackID = theTrack->GetTrackID();
        aStepRecord.parentID = theTrack->GetParentID();
        aStepRecord.particleEnergy =
                theStep->GetPreStepPoint()->GetKineticEnergy()/keV;
        aStepRecord.particleDirection[0] = particleDirection.x();
        aStepRecord.particleDirection[1] = particleDirection.y();
        aStepRecord.particleDirection[2] = particleDirection.z();
        aStepRecord.energyDeposition = theStep->GetTotalEnergyDeposit()/keV;
        aStepRecord.position[0] = trackPosition.x()/cm;
        aStepRecord.position[1] = trackPosition.y()/cm;
        aStepRecord.position[2] = trackPosition.z()/cm;
        
        //	Record whether or not the primary particle is a radioactive ion
        if( (aStepRecord.parentID==0) && (aStepRecord.stepNumber==1) &&
                !theTrack->GetDefinition()->GetPDGStable() &&
                (aStepRecord.particleName.find("[") < G4String::npos) )
            luxManager->GetEvent()->SetRadioactivePrimaryTime(
                    luxManager->GetPrimaryParticles()[0].time );
        
        //	Check to see if the particle is an immediate daughter of a primary
        //	radioactive nucleus, and if so, wipe out the global time. This has
        //	the effect of having the primary particle decaying at global time
        //  = 0.
        if( (aStepRecord.parentID==1) && (aStepRecord.stepNumber==1) &&
                luxManager->GetEvent()->GetRadioactivePrimaryTime() )
            theTrack->SetGlobalTime(
                    luxManager->GetEvent()->GetRadioactivePrimaryTime() );
        
        aStepRecord.stepTime = theTrack->GetGlobalTime()/ns;


       	//      When using the G4Decay generator the global time needs to be reset 
	//      for all radioisotopes.  The reset is done in LUXSimStackingAction 
	//      which is invoked by the method 
	//      G4EventManager::GetEventManager()->GetStackManager()->ReClassify()
	if( luxManager->GetG4DecayBool() ){
	  
	  std::map<G4int,bool> radIsoMap = luxManager->GetRadioIsotopeMap();
	
	  // reset for first step in event
	  if(theTrack->GetTrackID() == 1){	
	    for(itMap=radIsoMap.begin(); itMap!=radIsoMap.end(); ++itMap)
	      itMap->second = false; 
	  }

	  if( (aStepRecord.particleName.find("[")) < G4String::npos ){	   
	    for(itMap=radIsoMap.begin(); itMap!=radIsoMap.end(); ++itMap){
	      if((itMap->first==aStepRecord.particleID) && (itMap->second == false)){
		
		//    For the first step of a radioisotope, write all the accumulated steps
		if(theTrack->GetCurrentStepNumber()==1 && theTrack->GetTrackID() != 1){		  
		  luxManager->IterateEventCount();		 
		  luxManager->RecordValues( luxManager->GetEventCount() );
		  luxManager->ClearRecords();
		  
		  //    Record this isotope as a "primary particle"
		  primaryParticles.id = theTrack->GetDefinition()->GetParticleName();
		  primaryParticles.energy = theStep->GetPreStepPoint()->GetKineticEnergy()/keV;
		  primaryParticles.time = theTrack->GetGlobalTime()/ns;
		  primaryParticles.position = G4ThreeVector(trackPosition.x()/mm,trackPosition.y()/mm,trackPosition.z()/mm);
		  primaryParticles.direction = G4ThreeVector(particleDirection.x(), particleDirection.y(), particleDirection.z());
		  luxManager->AddPrimaryParticle( primaryParticles );
		}
		
		//    This next line calls stacking manager
		G4EventManager::GetEventManager()->GetStackManager()->ReClassify();
	
		//    This isotope has been considered, set flag to true
		itMap->second = true; 
		
	      }
	    }	      
	  }

	  luxManager->UpdateRadioIsotopeMap(radIsoMap);
	}

        G4bool inTheLXenonTarget = false;
        if( luxManager->GetDetectorSelection() == "1_0Detector" &&
                theTrack->GetVolume()->GetName() == "LiquidXenon" )
            inTheLXenonTarget = true;
        else if( luxManager->GetDetectorSelection() == "LZDetector" &&
                ( theTrack->GetVolume()->GetName() == "LiquidXenonTarget" ||
                  theTrack->GetVolume()->GetName() == "InnerLiquidXenon") )
            inTheLXenonTarget = true;
        else if( luxManager->GetDetectorSelection() == "LZSimple" &&
                ( theTrack->GetVolume()->GetName() == "ActiveLiquidXenon" ||
                  theTrack->GetVolume()->GetName() == "LiquidGammaXXenon") )
            inTheLXenonTarget = true;

        if( inTheLXenonTarget )
            luxManager->AddLiquidXenonEnergy( theStep->GetTotalEnergyDeposit());
        
        //	Handle the case of optical photon record keeping
        if( aStepRecord.particleName == "opticalphoton" ) {
        
            aStepRecord.energyDeposition = 0;
        
            if( optPhotRecordLevel )
                luxManager->AddDeposition(
                        (LUXSimDetectorComponent*)theTrack->GetVolume(),
                        aStepRecord );
            
            if( optPhotRecordLevel == 1 || optPhotRecordLevel == 3 )
                theTrack->SetTrackStatus( fStopAndKill );

        } else if ( aStepRecord.particleName == "thermalelectron" ){

            aStepRecord.energyDeposition = 0;

            if( thermElecRecordLevel )
                luxManager->AddDeposition(
                        (LUXSimDetectorComponent*)theTrack->GetVolume(),
                        aStepRecord );

            if( thermElecRecordLevel == 1 || thermElecRecordLevel == 3 )
                theTrack->SetTrackStatus( fStopAndKill );

        } else
            luxManager->AddDeposition(
                    (LUXSimDetectorComponent*)theTrack->GetVolume(),
                    aStepRecord );
        
        //	Kill the particle if the current volume is made of blackium, or if
        //	the record level is set to 4. The blackium support is kept for
        //	historical reasons. Note that the particle is killed only after the
        //	first step is recorded, so that we at least know what went into the
        //  volume and where.
        if( (theTrack->GetMaterial() == blackiumMat) || (recordLevel == 4) )
            theTrack->SetTrackStatus( fStopAndKill );
        
        //	Put debugging code here
        if( DEBUGGING ) {
            G4cout << "Tracking a " << aStepRecord.particleEnergy << "-keV "
                   << aStepRecord.particleName << " in "
                   << theTrack->GetVolume()->GetName() << " at ( "
                   << aStepRecord.position[0] << ", " << aStepRecord.position[1]
                   << ", " << aStepRecord.position[2] << " )" << G4endl;
            G4cout << "\tTrack " << aStepRecord.trackID << ", Step "
                   << aStepRecord.stepNumber << ", process: "
                   << aStepRecord.stepProcess << ", created by "
                   << aStepRecord.creatorProcess << G4endl;
    //		if( aStepRecord.particleName == "gamma" ||
    //				aStepRecord.particleName == "alpha" ||
    //				aStepRecord.particleName == "e-" ||
    //				aStepRecord.particleName == "anti_nu_e" ||
    //				aStepRecord.particleName == "neutron"
    //				)
    //			theTrack->SetTrackStatus( fStopAndKill );
        }
    }
}
