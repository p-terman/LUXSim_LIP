////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorRa226.hh
*
* This is the header file for the Ra226 generator.
*
********************************************************************************
* Change log
*    2013-02-16 DCM - Original version (adapted from U238 generator)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorRa226_HH
#define LUXSimGeneratorRa226_HH 1

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
class LUXSimGeneratorRa226 : public LUXSimSource
{
  public:
    LUXSimGeneratorRa226();
    ~LUXSimGeneratorRa226();

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
