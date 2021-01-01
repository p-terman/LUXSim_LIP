////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorSingleParticle.hh
*
* This is the header file for the SingleParticle generator.
*
********************************************************************************
* Change log
*   04-Mar-12 - Initial submission (Nick)
*   14-Jul-12 - GenerateEvent method changed to use binary search tree (Nick)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorSingleParticle_HH
#define LUXSimGeneratorSingleParticle_HH 1

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
class LUXSimGeneratorSingleParticle : public LUXSimSource
{
  public:
    LUXSimGeneratorSingleParticle();
    ~LUXSimGeneratorSingleParticle();

  public:
    using LUXSimSource::GenerateEventList;
    void GenerateEventList( G4ThreeVector, G4int, G4int,G4String, 
                            G4double, G4double );
    using LUXSimSource::GenerateFromEventList;
    void GenerateFromEventList(G4GeneralParticleSource*,G4Event*,decayNode*);

    //using LUXSimSource::GenerateEvent;
    //void GenerateEvent( G4GeneralParticleSource*, G4Event*, G4String, G4double );

  protected:
    G4double GetEnergy() { return energy; };
    G4ParticleDefinition *GetParticleDefinition() 
        { return particlename ;};

  private:
    G4ParticleDefinition *particlename;
    G4double energy;
    std::stringstream uiStream;
    G4String uiString;

};

#endif
