////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorYBe.hh
*
* This is the header file for the YBe generator.
*
********************************************************************************
* Change log
*    2 May 2014 - Initial submission (Kevin)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorYBe_HH
#define LUXSimGeneratorYBe_HH 1

//
//    GEANT4 includes
//
#include "globals.hh"

//
//    LUXSim includes
//
#include "LUXSimSource.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimGeneratorYBe : public LUXSimSource
{
    public:
        LUXSimGeneratorYBe();
        ~LUXSimGeneratorYBe();

    public:
        using LUXSimSource::GenerateEventList;
        void GenerateEventList( G4ThreeVector, G4int, G4int, G4double );
        using LUXSimSource::GenerateFromEventList;
        void GenerateFromEventList(G4GeneralParticleSource*,G4Event*,decayNode*);
        //using LUXSimSource::GenerateEvent;
        //void GenerateEvent( G4GeneralParticleSource*, G4Event* );

    private:
        G4ParticleDefinition *ionDef;
        G4ParticleDefinition *neutronDef;
        G4ParticleDefinition *gammaDef;

        G4double neutronEnergy;
        G4double gammaEnergy1;
        G4double gammaEnergy2;
        G4double neutronProb;
        
};

#endif
