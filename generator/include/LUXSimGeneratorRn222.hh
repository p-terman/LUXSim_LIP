////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorRn222.hh
*
* This is the header file for the Ra222 generator.
*
********************************************************************************
* Change log
*    26-Jun-2009 - file creation (Nick)
*    14-Jul-2012 - GenerateEvent changed to use binary search tree (Nick)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorRn222_HH
#define LUXSimGeneratorRn222_HH 1

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
class LUXSimGeneratorRn222 : public LUXSimSource
{
    public:
        LUXSimGeneratorRn222();
        ~LUXSimGeneratorRn222();

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
