////////////////////////////////////////////////////////////////////////////////
/*    LUXSimGeneratorPb210.hh
*
* This is the header file for the Pb210 generator.
*
********************************************************************************
* Change log
*    07-Nov-2012 - Adapted from LUXSimGeneratorU238.hh (Dave)
*
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef LUXSimGeneratorPb210_HH
#define LUXSimGeneratorPb210_HH 1

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
class LUXSimGeneratorPb210 : public LUXSimSource
{
  public:
    LUXSimGeneratorPb210();
    ~LUXSimGeneratorPb210();

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
