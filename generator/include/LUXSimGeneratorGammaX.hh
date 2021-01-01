////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorGammaX.hh
*
* This is the header file for the GammaX generator.
*
********************************************************************************
* Change log
*   22-July-13 - Initial submission (Matthew)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorGammaX_HH
#define LUXSimGeneratorGammaX_HH 1

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
class LUXSimGeneratorGammaX : public LUXSimSource
{
    public:
        LUXSimGeneratorGammaX();
        ~LUXSimGeneratorGammaX();

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
