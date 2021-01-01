////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorKr83m.hh
*
* This is the header file for the Kr83m generator.
*
********************************************************************************
* Change log
*   13-Jan-12 - Initial submission (Nick)
*   14-Jul-12 - Methods changed so as to use the binary search tree (Nick)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorKr83m_HH
#define LUXSimGeneratorKr83m_HH 1

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
class LUXSimGeneratorKr83m : public LUXSimSource
{
    public:
        LUXSimGeneratorKr83m();
        ~LUXSimGeneratorKr83m();

    public:
        using LUXSimSource::GenerateEventList;
        void GenerateEventList( G4ThreeVector, G4int, G4int, G4double );
        using LUXSimSource::GenerateFromEventList;
        void GenerateFromEventList(G4GeneralParticleSource*,G4Event*,decayNode*);

    private:        
        G4ParticleDefinition *ionDef;
        G4ParticleDefinition *electronDef;
        G4ParticleDefinition *gammaDef;
        
};

#endif
