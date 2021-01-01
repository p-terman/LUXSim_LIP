////////////////////////////////////////////////////////////////////////////////
/*	LUXSimEventAction.cc
*
* This is the code file to control the simulation event-by-event.
*
********************************************************************************
* Change log
*	13 March 2009 - Initial submission (Kareem)
*	19-Aug-10 - Added checks to determine if the run ended cleanly, and set a
*				flag in the manager if it did (Kareem)
*   24-Mar-12 - Added support for the event progress report UI hooks (Mike)
*	23-Oct-12 - Added initialization for the global time of the primary particle
*				if it's a radioactive nucleus (Kareem)
*
*/
/////////////////////////////////////////////////////////////////////////////////

//
//	C/C++ includes
//
#include <ctime>

//
//	GEANT4 includes
//
#include "G4Event.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"

//
//	LUXSim includes
//
#include "LUXSimEventAction.hh"
#include "LUXSimManager.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					LUXSimEventAction()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimEventAction::LUXSimEventAction()
: simStartTime(0), simEndTime(0), eventNum(-1)
{
	luxManager = LUXSimManager::GetManager();
	luxManager->Register( this );
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					~LUXSimEventAction()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimEventAction::~LUXSimEventAction()
{}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					BeginOfEventAction()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimEventAction::BeginOfEventAction(const G4Event* evt )
{
	eventNum = evt->GetEventID();
	if( !eventNum ) {
		luxManager->SetRunEndedCleanly( false );
		time( &simStartTime );
	}
	
	radioactivePrimaryTime = 0;
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					EndOfEventAction()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimEventAction::EndOfEventAction( const G4Event* evt )
{
	//	Check to see if all the events have been processed, and if so, set the
	//	"run ended cleanly" flag
	if( eventNum == (luxManager->GetNumEvents() - 1) )
		luxManager->SetRunEndedCleanly( true );

	// Print out periodic progress reports
	if( eventNum% (luxManager->GetEventProgressFreqnecy()) == 0 ) {
		time( &simEndTime );
		G4cout << "\n\tProcessing event " << eventNum << " at "
			   << difftime( simEndTime, simStartTime ) << " seconds.";
		G4cout.flush();
	} else if( luxManager->GetEventProgressFreqnecy() >= 10 &&
                eventNum % (luxManager->GetEventProgressFreqnecy()/10) == 0 ) {
		G4cout << ".";
        G4cout.flush();
	}
	
	if( !luxManager->GetG4DecayBool() ){
	  luxManager->RecordValues( eventNum );
	  luxManager->ClearRecords();
	}

	//	Visualization support. The follow chunk of code draws the trajectories
	//	if the macro opens a visualization.
	if( eventNum < 100 && G4VVisManager::GetConcreteInstance() ) {
		G4TrajectoryContainer *trajectoryContainer =
				evt->GetTrajectoryContainer();
		G4int n_trajectories = 0;
		if( trajectoryContainer )
			n_trajectories = trajectoryContainer->entries();
		for( G4int i=0; i<n_trajectories; i++ ) {
			G4Trajectory* trj =
					(G4Trajectory *)( (*(evt->GetTrajectoryContainer()))[i] );
			trj->DrawTrajectory( 0 );
		}
	}
}
