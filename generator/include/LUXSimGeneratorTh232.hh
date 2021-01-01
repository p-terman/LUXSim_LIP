////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorTh232.hh
*
* This is the header file for the Th232 generator.
*
********************************************************************************
* Change log
*    20 Apr 2009 - Initial submission (Kareem)
*    27-May-2009 - This generator now works (Kareem)
*    14-Jul-2012 - GenerateEvent method changed to use binary search tree (Nick)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorTh232_HH
#define LUXSimGeneratorTh232_HH 1

//
//    GEANT4 includes
//
#include "G4ParticleDefinition.hh"
#include "globals.hh"

//
//    LUXSim includes
//
#include "LUXSimSource.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimGeneratorTh232 : public LUXSimSource
{
    public:
        LUXSimGeneratorTh232();
        ~LUXSimGeneratorTh232();

    public:
        using LUXSimSource::GenerateEventList;
        void GenerateEventList( G4ThreeVector, G4int, G4int, G4double );
        using LUXSimSource::GenerateFromEventList;
        void GenerateFromEventList(G4GeneralParticleSource*,G4Event*,decayNode*);
        //using LUXSimSource::GenerateEvent;
        //void GenerateEvent( G4GeneralParticleSource*, G4Event* );

    private:
        G4ParticleDefinition *ion;

};

#endif
