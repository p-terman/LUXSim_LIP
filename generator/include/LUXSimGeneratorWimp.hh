////////////////////////////////////////////////////////////////////////////////
/*	LUXSimGeneratorWimp.hh
*
* This is the header file for the Wimp generator.
*
********************************************************************************
* Change log
*	18-May-2012 - file creation (Daniel)
*   14-Jul-2012 - modified to account for the earth and galactic
*                 escape velocities. (Daniel)
*   14-Jul-2012 - GenerateEvent replaced with GenerateEventList and 
*                 GenerateFromEventList (Nick)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorWimp_HH
#define LUXSimGeneratorWimp_HH 1

//
//	GEANT4 includes
//
#include "G4ParticleDefinition.hh"
#include "globals.hh"

//
//	LUXSim includes
//
#include "LUXSimSource.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimGeneratorWimp : public LUXSimSource
{
	public:
        LUXSimGeneratorWimp();
        ~LUXSimGeneratorWimp();

	public:
        using LUXSimSource::GenerateEventList;
        void GenerateEventList( G4ThreeVector, G4int, G4int, G4double, 
                                    G4double );
        using LUXSimSource::GenerateFromEventList;
        void GenerateFromEventList( G4GeneralParticleSource*, G4Event*,
                    decayNode*);

    private:
        G4double dR(G4double, G4double, G4double);
        G4ParticleDefinition *ion;

};

#endif
