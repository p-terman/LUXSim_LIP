////////////////////////////////////////////////////////////////////////////////
/*	LUXSimEventAction.hh
*
* This is the header file to control the simulation event-by-event.
*
********************************************************************************
* Change log
*	13-Mar-2009 - Initial submission (Kareem)
*	23-Oct-2012 - Added a hook for recording the global time of the primary
*				  particle (Kareem)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimEventAction_HH
#define LUXSimEventAction_HH 1

//
//	GEANT4 includes
//
#include "globals.hh"
#include "G4UserEventAction.hh"

//
//	Class forwarding
//
class G4Event;
class LUXSimManager;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimEventAction : public G4UserEventAction
{
	public:
		LUXSimEventAction();
		~LUXSimEventAction();

		void BeginOfEventAction( const G4Event* );
		void EndOfEventAction( const G4Event* );
		
		void SetManager( LUXSimManager *man ) { luxManager = man; };
		
		inline G4double GetTimeSinceStart() { return simStartTime; };
                inline void SetTimeSinceStart( G4double t ) { simStartTime = (time_t)(t); };
		
		inline G4int GetEventNum() { return eventNum; };
		
		inline void SetRadioactivePrimaryTime( G4double val )
				{ radioactivePrimaryTime = val; };
		inline G4double GetRadioactivePrimaryTime()
				{ return radioactivePrimaryTime; };
			
	private:
		time_t simStartTime, simEndTime;
		G4int eventNum;
		G4double radioactivePrimaryTime;
	
		LUXSimManager *luxManager;
};

#endif
