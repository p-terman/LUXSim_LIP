////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorAmBe.hh
*
* This is the header file for the AmBe generator.
*
********************************************************************************
* Change log
*    20 April 2009 - Initial submission (Kareem)
*    27-May-2009 - This generator now works (Kareem)
*    17-Nov-2011 - Fixed the low-energy end of the neutron energy CDF (Kareem)
*    14-Jul-2012 - Methods changed so as to use the binary search tree (Nick)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorAmBe_HH
#define LUXSimGeneratorAmBe_HH 1

//
//    GEANT4 includes
//
#include "globals.hh"

//
//    LUXSim includes
//
#include "LUXSimSource.hh"

//------++++++------++++++------++++++------++++++------++++++------++++++------
class LUXSimGeneratorAmBe : public LUXSimSource
{
    public:
        LUXSimGeneratorAmBe();
        ~LUXSimGeneratorAmBe();

    public:
        using LUXSimSource::GenerateEventList;
        void GenerateEventList( G4ThreeVector, G4int, G4int, G4double );
        using LUXSimSource::GenerateFromEventList;
        void GenerateFromEventList(G4GeneralParticleSource*,G4Event*,decayNode*);
        //using LUXSimSource::GenerateEvent;
        //void GenerateEvent( G4GeneralParticleSource*, G4Event* );

    private:
        G4double GetNeutronEnergy();
        G4double GetGammaAngle();

    private:
        G4ParticleDefinition *ionDef;
        G4ParticleDefinition *neutronDef;
        G4ParticleDefinition *gammaDef;
        
        G4double neutronCDF[758];
        G4double neutronEnergy[758];
        
        G4double gammaCDF[5001];
        G4double gammaAngle[5001];
};

#endif
