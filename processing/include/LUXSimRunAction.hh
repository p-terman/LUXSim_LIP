////////////////////////////////////////////////////////////////////////////////
/*	LUXSimRunAction.hh
*
* This is the header file to control the simulation run-by-run.
*
********************************************************************************
* Change log
*	13 March 2009 - Initial submission (Kareem)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimRunAction_HH
#define LUXSimRunAction_HH 1

//
//	GEANT4 includes
//
#include "G4UserRunAction.hh"
#include "globals.hh"

//
//	Class forwarding
//
class LUXSimManager;
class G4Run;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimRunAction : public G4UserRunAction
{
	public:
		LUXSimRunAction();
		~LUXSimRunAction();

	public:
		void BeginOfRunAction( const G4Run* );
		void EndOfRunAction( const G4Run* );
		
	private:
		LUXSimManager *luxManager;
		time_t startTime;
		
};

#endif
