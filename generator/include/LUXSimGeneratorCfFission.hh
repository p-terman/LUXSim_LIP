////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorFissionNeutrons.hh
*
* This is the header file for the Cf252 fission neutron generator.
*
********************************************************************************
* Change log
*    13 Sep 2010 - Initial submission (Kareem)
*    03 Mar 2011 - Added support for fission gammas (Kareem)
*    14-Jul-2012 - Methods changed so as to use the binary search tree (Nick)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorCfFission_HH
#define LUXSimGeneratorCfFission_HH 1

//
//    GEANT4 includes
//
#include "globals.hh"

//
//    LUXSim includes
//
#include "LUXSimSource.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimGeneratorCfFission : public LUXSimSource
{
    public:
        LUXSimGeneratorCfFission();
        ~LUXSimGeneratorCfFission();

    public:
      using LUXSimSource::GenerateEventList;
      void GenerateEventList( G4ThreeVector, G4int, G4int, G4double );
      using LUXSimSource::GenerateFromEventList;
      void GenerateFromEventList(G4GeneralParticleSource*,G4Event*,decayNode*);
      //using LUXSimSource::GenerateEvent;
      //void GenerateEvent( G4GeneralParticleSource*, G4Event* );

    private:
        G4double GetNeutronEnergy();
        G4double GetGammaEnergy( G4double );

    private:
        G4ParticleDefinition *neutronDef;
        G4ParticleDefinition *gammaDef;
        
        G4double gammaCDF[86];
        G4double gammaEnergy[86];
        
        G4double Z;
        G4double A;
        G4double averageGammaEnergy;
};

#endif
