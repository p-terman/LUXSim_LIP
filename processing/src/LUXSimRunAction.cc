////////////////////////////////////////////////////////////////////////////////
/*	LUXSimRunAction.cc
*
* This is the code file to control the simulation run-by-run.
*
********************************************************************************
* Change log
*	13 March 2009 - Initial submission (Kareem)
*
*/
////////////////////////////////////////////////////////////////////////////////

//
//	Geant4 includes
//
#include "G4Run.hh"

//
//	LUXSim includes
//
#include "LUXSimRunAction.hh"
#include "LUXSimManager.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					LUXSimRunAction()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimRunAction::LUXSimRunAction()
{
	luxManager = LUXSimManager::GetManager();
	luxManager->Register( this );
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					~LUXSimRunAction()
//------++++++------++++++------++++++------++++++------++++++------++++++------
LUXSimRunAction::~LUXSimRunAction() {}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					BeginOfRunAction()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimRunAction::BeginOfRunAction( const G4Run* aRun )
{
	//	Print out the run number, and start the timer
	G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
	time( &startTime );
	luxManager->InitialiseEventCount();
	
}

//------++++++------++++++------++++++------++++++------++++++------++++++------
//					EndOfRunAction()
//------++++++------++++++------++++++------++++++------++++++------++++++------
void LUXSimRunAction::EndOfRunAction( const G4Run* aRun )
{
	//	Print out the last progress report
	time_t endTime;
	time( &endTime );
	G4double timeDifference = difftime( endTime, startTime );
	G4cout << G4endl << "Run " << aRun->GetRunID() << " time to completion: "
		   << timeDifference << " seconds" << G4endl;

}
