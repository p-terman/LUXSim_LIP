////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorDD.hh
*
* This is the header file for the GammaX generator.
*
********************************************************************************
* Change log
*   2-Feb-15 - Initial submission (Kevin)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorDD_HH
#define LUXSimGeneratorDD_HH 1

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
class LUXSimGeneratorDD : public LUXSimSource
{
    public:
        LUXSimGeneratorDD();
        ~LUXSimGeneratorDD();

    public:
        using LUXSimSource::GenerateEventList;
        void GenerateEventList( G4ThreeVector, G4int, G4int, G4double );
        using LUXSimSource::GenerateFromEventList;
        void GenerateFromEventList(G4GeneralParticleSource*,G4Event*,decayNode*);

    private:
        G4ParticleDefinition *ionDef;
        G4ParticleDefinition *neutronDef;
        G4double ddEnergy;
        
};

#endif
