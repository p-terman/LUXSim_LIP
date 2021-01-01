////////////////////////////////////////////////////////////////////////////////
/*	LUXSimSource.hh
*
* This is the header file for individual generators. Each generator will inherit
* from this generalized source class.
*
********************************************************************************
* Change log
*	20 April 2009 - Initial submission (Kareem)
*	27-May-2009 - Modified to work better with LUXSimDetectorComponent (Kareem)
*   4-Jan-2010 - Overloaded GenerateEvent() to include single nucleus decays
*                (Nick)
*   30-Apr-2010 - Added GetParticleInfo() (Nick)
*   21 Jul 2011 - Added DecayChain methods (Nick)
*   04-Mar-2012 - Overloaded GenerateEvent for SingleParticle sources (Nick)
*   18-Jun-2012 - Added a GenerateEvent for the Wimp generator (Daniel)
*   14-Jul-2012 - Replace GenerateEvent with GenereateEventList (Nick)
*   24-Aug-2012 - Add ParentDecayTime() method used only in DecayChain source
*                 so DetectorComponent stops asking for new decays after the
*                 recordTree timeWindow is reaches (Nick)
*   18-Dec-2015 - Added a GenerateEvent for the muon generator (David W) (merged into git by Doug T)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimSource_HH
#define LUXSimSource_HH 1

//
//	CLHEP includes
//
#include "Randomize.hh"

//
//	GEANT4 includes
//
#include "G4UImanager.hh"
#include "G4GeneralParticleSource.hh"
#include "G4Event.hh"
#include "globals.hh"
#include "G4Gamma.hh"

//
//	LUXSim includes
//
#include "LUXSimManager.hh"
#include "LUXSimBST.hh"
#include "LUXSimIsotope.hh"

//
//	Class forwarding
//
class LUXSimDetectorComponent;

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimSource
{
	public:
		LUXSimSource();
		~LUXSimSource();

	public:
		G4String GetName() { return name; };
		G4double GetActivityMultiplier() { return activityMultiplier; };

        // generate event list for different conditions
        //   default
		virtual void GenerateEventList( G4ThreeVector, G4int, G4int, G4double );
        //   single decay
		virtual void GenerateEventList( G4ThreeVector, G4int, G4int, 
                            G4int, G4int, G4double );
        //   single particle
		virtual void GenerateEventList( G4ThreeVector, G4int, G4int, 
                G4String, G4double, G4double );
        //   wimp
		virtual void GenerateEventList( G4ThreeVector, G4int, G4int, G4double,
                G4double );
        //   decay chain
        virtual void CalculatePopulationsInEventList( G4double, G4double, 
                            G4String);
        virtual void GenerateEventList( G4ThreeVector, G4int, G4int, G4String );
        virtual G4double GetParentDecayTime() {return 0.;};
		//   musun  
		virtual void GenerateEventList(G4double);

        // generate from event list
		virtual void GenerateFromEventList( G4GeneralParticleSource*, G4Event*,
                            decayNode* );

	protected:
		G4UImanager *UI;
		G4String name;
		G4double activityMultiplier;
		G4double probability;
		
	protected:
        LUXSimManager::primaryParticleInfo GetParticleInfo( 
                        G4GeneralParticleSource* );
		G4ThreeVector GetRandomDirection();
		virtual G4double GetEnergy() { return 1.*MeV; };
		virtual G4ParticleDefinition *GetParticleDefinition()
			{ return G4Gamma::Definition(); };

    protected:
		LUXSimManager *luxManager;
	
	private:
		G4double xPos, yPos, zPos;
		G4double xDir, yDir, zDir;
		G4double cosTheta, sinTheta, phi;
		G4double prob;
};

#endif
