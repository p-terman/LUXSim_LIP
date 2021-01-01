////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorRn220.hh
*
* This is the header file for the Rn220 generator.
*
********************************************************************************
* Change log
*    16-February-2015 - file creation (Simon), copying from Rn222 generator
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorRn220_HH
#define LUXSimGeneratorRn220_HH 1

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
class LUXSimGeneratorRn220 : public LUXSimSource
{
    public:
        LUXSimGeneratorRn220();
        ~LUXSimGeneratorRn220();

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
